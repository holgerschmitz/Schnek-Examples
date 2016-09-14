/*
 * cellular.cpp
 *
 *  Created on: 13 Sep 2016
 *  Author: Holger Schmitz
 */

#include "cellular.hpp"

#include <schnek/parser.hpp>
//#include <schnek/variables/block.hpp>
//#include <schnek/grid/array.hpp>

#include <boost/ref.hpp>

#include <mpi.h>

Index Cellular::globalMax;

void Cellular::initParameters(BlockParameters &blockPars)
{
  blockPars.addArrayParameter("size", size);
  blockPars.addParameter("states", &states);
}

void Cellular::registerData()
{
  addData("domain", domain);
}

void Cellular::init()
{
  globalMax = size;
  globalMax -= 1;

  subdivision.init(size, 1);

  Index low  = subdivision.getLo();
  Index high = subdivision.getHi();

  Index lowIn  = subdivision.getInnerLo();
  Index highIn = subdivision.getInnerHi();

  innerRange = Range<int, DIMENSION>(lowIn, highIn);
  Range<double, DIMENSION> domain(Array<double, DIMENSION>(lowIn[0],lowIn[1]), Array<double, DIMENSION>(highIn[0],highIn[1]));
  Array<bool, DIMENSION> stagger(false, false);

  domain = new Domain(lowIn, highIn, domain, stagger, 1);
  domain_swap = new Domain(lowIn, highIn, domain, stagger, 1);
}


void Cellular::execute()
{
  // just a shortcut
  Domain &D = *domain;
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
    BlockClasses blocks;

    blocks.registerBlock("cellular").setClass<Cellular>();
    blocks("Diagnostic").setClass<CellularDiagnostic>();
    blocks("cellular").addChildren("Diagnostic");

    std::ifstream in("cellular.setup");

    Parser P("my_simulation", "cellular", blocks);
    registerCMath(P.getFunctionRegistry());
    pBlock application = P.parse(in);

    Cellular &mysim = dynamic_cast<Cellular&>(*application);
    mysim.initAll();
    mysim.execute();
  }
  catch (ParserError &e)
  {
    std::cerr << "Parse error in " << e.getFilename() << " at line "
        << e.getLine() << ": " << e.message << std::endl;
    exit(-1);
  }
  catch (VariableNotInitialisedException &e)
  {
    std::cerr << "Variable was not initialised: " << e.getVarName() << std::endl;
    return -1;
  }
  catch (EvaluationException &e)
  {
    std::cerr << "Error in evaluation: " << e.getMessage() << std::endl;
    return -1;
  }
  catch (VariableNotFoundException &e)
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
    std::cerr << "FATAL ERROR: >>" << err << "<<" << std::endl;
    return -1;
  }

  MPI_Finalize();

  return 0;
}

