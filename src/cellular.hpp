/*
 * cellular.hpp
 *
 *  Created on: 13 Sep 2016
 *      Author: Holger Schmitz
 */

#ifndef SRC_CELLULAR_HPP_
#define SRC_CELLULAR_HPP_


#include <schnek/grid.hpp>
#include <schnek/variables.hpp>
#include <schnek/diagnostic/diagnostic.hpp>
#include <schnek/diagnostic/hdfdiagnostic.hpp>

#include <boost/ref.hpp>

#include <deque>

using namespace schnek;

static const int DIMENSION = 2;

typedef Array<int, DIMENSION> Index;
typedef Field<int, DIMENSION> Domain;
typedef Domain* pDomain;

class Cellular : public Block
{
  private:
    static Index globalMax;
    Index size;
    Range<int, DIMENSION> innerRange;
    int states;
    pDomain domain;
    pDomain domain_swap;

    MPICartSubdivision<Domain> subdivision;

  protected:
    void initParameters(BlockParameters &blockPars);
    void registerData();
  public:
    void execute();
    void init();
    static Index getGlobalMax() { return globalMax; }
};

class CellularDiagnostic : public schnek::HDFGridDiagnostic<Domain, Domain* >
{
  protected:
    typedef HDFGridDiagnostic<Domain, Domain* >::IndexType IndexType;
    IndexType getGlobalMin();
    IndexType getGlobalMax();
};

#endif /* SRC_CELLULAR_HPP_ */
