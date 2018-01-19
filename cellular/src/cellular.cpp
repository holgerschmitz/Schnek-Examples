/*
 * cellular.cpp
 *
 *  Created on: 13 Sep 2016
 *  Author: Holger Schmitz
 */

#include "cellular.hpp"
#include "neighbourhood.hpp"
#include "rules.hpp"

#include <schnek/parser.hpp>
#include <schnek/tools/fieldtools.hpp>
#include <schnek/util/logger.hpp>

//#include <schnek/variables/block.hpp>
//#include <schnek/grid/array.hpp>

#include <boost/ref.hpp>
#include <boost/random.hpp>
#include <boost/make_shared.hpp>

#include <mpi.h>

#include <cstdlib>

Index Cellular::globalMax;


boost::random::mt19937 rng;

int randomRange(int from, int to, double x, double y)
{
  boost::random::uniform_int_distribution<> dist(from, to);
  return dist(rng);
}

void Cellular::initParameters(schnek::BlockParameters &parameters)
{
  parameters.addArrayParameter("size", size);
  parameters.addParameter("tMax", &tMax);
  x_parameters = parameters.addArrayParameter("", x, schnek::BlockParameters::readonly);
  paramField = parameters.addParameter("D", &domainInit, 0);

  spaceVars = schnek::pParametersGroup(new schnek::ParametersGroup());
  spaceVars->addArray(x_parameters);
}

void Cellular::registerData()
{
  addData("domain", domain);
}

void Cellular::fillValues()
{
  Range range(Index(0,0), size);

  schnek::pBlockVariables blockVars = getVariables();
  schnek::pDependencyMap depMap(new schnek::DependencyMap(blockVars));

  schnek::DependencyUpdater updater(depMap);

  updater.addIndependentArray(x_parameters);
  schnek::fill_field(*domain, x, domainInit, updater, paramField);
}

void Cellular::init()
{
  globalMax = size;
  globalMax -= 1;

  subdivision.init(size, 2);

  Index low  = subdivision.getLo();
  Index high = subdivision.getHi();

  Index lowIn  = subdivision.getInnerLo();
  Index highIn = subdivision.getInnerHi();

  rng.seed(subdivision.getUniqueId());

  innerRange = Range(lowIn, highIn);
  schnek::Range<double, DIMENSION> domainSize(schnek::Array<double, DIMENSION>(lowIn[0],lowIn[1]), schnek::Array<double, DIMENSION>(highIn[0],highIn[1]));
  schnek::Array<bool, DIMENSION> stagger;

  stagger = false;

  domain = boost::make_shared<Domain>(lowIn, highIn, domainSize, stagger, 2);

  neighbourCells.clear();

  BlockContainer<Neighbourhood>::iterator_range childRange = BlockContainer<Neighbourhood>::childBlocks();

  for (BlockContainer<Neighbourhood>::iterator it = childRange.begin(); it!=childRange.end(); ++it)
    (*it)->getNeighbourCells(neighbourCells);

  if (BlockContainer<Rule>::numChildren()!=1)
    throw std::string("Must specify exactly one Rule");

  fillValues();
}


void Cellular::execute()
{

  boost::shared_ptr<Rule> rule = *(BlockContainer<Rule>::childBlocks().begin());

  int t=0;

  schnek::DiagnosticManager::instance().setTimeCounter(&t);
  schnek::DiagnosticManager::instance().setMaster(subdivision.master());
  schnek::DiagnosticManager::instance().setRank(subdivision.getUniqueId());

  subdivision.exchange(*domain);

  schnek::DiagnosticManager::instance().execute();


  for (t=1; t<tMax; ++t)
  {
    if (subdivision.master())
      schnek::Logger::instance().out() <<"Time "<< t << std::endl;

    rule->execute(neighbourCells);
    subdivision.exchange(*domain);

    schnek::DiagnosticManager::instance().execute();

    typedef schnek::BlockContainer<ColorPlot<Domain> > PlotType;

    PlotType::iterator end = PlotType::childBlocks().end();
    for (PlotType::iterator it = PlotType::childBlocks().begin();
         it != end;
         ++it)
    {
      (*it)->execute();
    }
  }
}


CellularDiagnostic::IndexType CellularDiagnostic::getGlobalMin()
{
  return IndexType(0);
}

CellularDiagnostic::IndexType CellularDiagnostic::getGlobalMax()
{
  return Cellular::getGlobalMax();
}


int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  try
  {
    schnek::BlockClasses blocks;

    blocks.registerBlock("cellular").setClass<Cellular>();
    blocks("Diagnostic").setClass<CellularDiagnostic>();

    blocks("Neumann").setClass<NeumannNeighbourhood>();
    blocks("Moore").setClass<MooreNeighbourhood>();
    blocks("NeumannNoCentre").setClass<NeumannNeighbourhoodNoCentre>();
    blocks("MooreNoCentre").setClass<MooreNeighbourhoodNoCentre>();

    blocks("BelousovZhabotinsky").setClass<BelousovZhabotinskyRule>();

    blocks("Plot").setClass< ColorPlot<Domain> >();

    blocks("cellular").addChildren("Diagnostic")
        ("Neumann")("Moore")("NeumannNoCentre")("MooreNoCentre")
        ("BelousovZhabotinsky")("Plot");

    std::ifstream in("cellular.setup");
    if (!in) throw std::string("Could not open file 'cellular.setup'");

    schnek::Parser P("my_simulation", "cellular", blocks);
    registerCMath(P.getFunctionRegistry());

    P.getFunctionRegistry().registerFunction("random",randomRange);

    schnek::pBlock application = P.parse(in);

    Cellular &mysim = dynamic_cast<Cellular&>(*application);
    mysim.initAll();
    mysim.execute();
  }
  catch (schnek::ParserError &e)
  {
    std::cerr << "Parse error in " << e.getFilename() << " at line "
        << e.getLine() << ": " << e.message << std::endl;
    exit(-1);
  }
  catch (schnek::VariableNotInitialisedException &e)
  {
    std::cerr << "Variable was not initialised: " << e.getVarName() << std::endl;
    return -1;
  }
  catch (schnek::EvaluationException &e)
  {
    std::cerr << "Error in evaluation: " << e.getMessage() << std::endl;
    return -1;
  }
  catch (schnek::VariableNotFoundException &e)
  {
    std::cerr << "Error: " << e.getMessage() << std::endl;
    return -1;
  }
  catch (SchnekException &e)
  {
    std::cerr << "An error occured" << std::endl;
    return -1;
  }
  catch (std::string &err)
  {
    std::cerr << "FATAL ERROR: " << err << std::endl;
    return -1;
  }

  MPI_Finalize();

  return 0;
}

