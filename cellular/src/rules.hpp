/*
 * rules.hpp
 *
 *  Created on: 14 Sep 2016
 *  Author: Holger Schmitz
 */

#ifndef SRC_RULES_HPP_
#define SRC_RULES_HPP_

#include "cellular.hpp"
#include <schnek/variables/blockcontainer.hpp>
#include <set>

class Rule : public schnek::ChildBlock<Rule>
{
  protected:
    pDomain domain;
    pDomain domainBuffer;

    int sumNeighbours(Index pos, const std::list<Index> &neighbourCells);
    void limitValues(int min, int max);
  public:
    void init();
    virtual void execute(const std::list<Index> &neighbourCells) = 0;
};

class ConwayLife : public Rule
{
  public:
    void postInit();
    void execute(const std::list<Index> &neighbourCells);
};

class BelousovZhabotinskyRule : public Rule
{
  private:
    int states;

    double k1;
    double k2;
    double g;
  public:
    void initParameters(schnek::BlockParameters &blockPars);
    void postInit();
    void execute(const std::list<Index> &neighbourCells);
};

#endif /* SRC_RULES_HPP_ */
