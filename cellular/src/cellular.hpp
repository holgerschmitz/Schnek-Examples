/*
 * cellular.hpp
 *
 *  Created on: 13 Sep 2016
 *      Author: Holger Schmitz
 */

#ifndef SRC_CELLULAR_HPP_
#define SRC_CELLULAR_HPP_

#include <schnek/variables/blockcontainer.hpp>

#include <gui/colorplot.hpp>

#include <schnek/grid.hpp>
#include <schnek/variables.hpp>
#include <schnek/diagnostic/diagnostic.hpp>
#include <schnek/diagnostic/hdfdiagnostic.hpp>

#include <boost/ref.hpp>

#include <set>
#include <list>

static const int DIMENSION = 2;

typedef schnek::Array<int, DIMENSION> Index;
typedef schnek::Field<int, DIMENSION> Domain;
typedef boost::shared_ptr<Domain> pDomain;
typedef schnek::Range<int, DIMENSION> Range;

class Neighbourhood;
class Rule;

class Cellular : public schnek::Block,
                 public schnek::BlockContainer<Neighbourhood>,
                 public schnek::BlockContainer<Rule>,
                 public schnek::BlockContainer<ColorPlot<Domain> >
{
  private:
    static Index globalMax;
    Index size;
    int tMax;
    Range innerRange;
    pDomain domain;
    std::list<Index> neighbourCells;

    schnek::MPICartSubdivision<Domain> subdivision;

    schnek::Array<double, 2> x;
    schnek::Array<schnek::pParameter, 2> x_parameters;
    schnek::pParameter paramField;
    schnek::pParametersGroup spaceVars;
    int domainInit;

  protected:
    void initParameters(schnek::BlockParameters &blockPars);
    void registerData();
    void fillValues();
  public:
    void execute();
    void init();

    pDomain getDomain() { return domain; }

    static Index getGlobalMax() { return globalMax; }
};

class CellularDiagnostic : public schnek::HDFGridDiagnostic<Domain, pDomain >
{
  protected:
    typedef HDFGridDiagnostic<Domain, pDomain >::IndexType IndexType;
    IndexType getGlobalMin();
    IndexType getGlobalMax();
};

#endif /* SRC_CELLULAR_HPP_ */
