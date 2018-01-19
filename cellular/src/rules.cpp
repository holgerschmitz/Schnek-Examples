/*
 * rules.cpp
 *
 *  Created on: 20 Sep 2016
 *  Author: Holger Schmitz
 */

#include "rules.hpp"

#include <boost/make_shared.hpp>

void Rule::init()
{
  retrieveData("domain", domain);

  domainBuffer = boost::make_shared<Domain>(*domain);
}

int Rule::sumNeighbours(Index pos, const std::list<Index>& neighbourCells)
{
  int sum = 0;
  for (std::list<Index>::const_iterator it=neighbourCells.begin(); it!=neighbourCells.end(); ++it)
    sum += (*domain)[Index(pos + (*it))];
  return sum;
}

void BelousovZhabotinskyRule::initParameters(schnek::BlockParameters &blockPars)
{
  blockPars.addParameter("states", &states);
  blockPars.addParameter("k1", &k1);
  blockPars.addParameter("k2", &k2);
  blockPars.addParameter("g", &g);
}

void BelousovZhabotinskyRule::postInit()
{
  Domain &d = *domain;
  Range range(domain->getInnerLo(), domain->getInnerHi());
  for (Range::iterator p = range.begin(); p!=range.end(); ++p) {
    Index pos = *p;
    d[pos] = std::max(0,std::min(states, d[pos]));
  }
}

void BelousovZhabotinskyRule::execute(const std::list<Index>& neighbourCells)
{
  Domain &d = *domain;
  Range range(domain->getInnerLo(), domain->getInnerHi());
  for (Range::iterator p = range.begin(); p!=range.end(); ++p)
  {
    Index pos = *p;

    int newValue = 0;
    if (d[pos]==states)
    {
      newValue = 0;
    }
    else if (d[pos]==0)
    {
      int infected = 0;
      int ill = 0;
      for (std::list<Index>::const_iterator it=neighbourCells.begin(); it!=neighbourCells.end(); ++it)
      {
        Index pr = pos + (*it);
        //std::cout << "   > " << pr[0] << ", " << pr[1] << std::endl;
        int n = d[pr];
        if (n==states) ++ill;
        else if (n>0) ++infected;
      }

      newValue = (int)floor(infected/k1 + ill/k2);
    }
    else
    {
      int sum = 0;
      int infected = 0;
      int ill = 0;
      for (std::list<Index>::const_iterator it=neighbourCells.begin(); it!=neighbourCells.end(); ++it)
      {
        Index pr = pos + (*it);
        //std::cout << "   > " << pr[0] << ", " << pr[1] << std::endl;
        int n = d[pr];
        sum += n;
        if (n==states) ++ill;
        else if (n>0) ++infected;
      }
      newValue = (int)floor(sum/(infected + ill + 1.0) + g);
    }
    (*domainBuffer)[pos] = std::max(0,std::min(newValue, states));

    //std::cout << "(" << pos[0] << ", "  << pos[1] << "): " <<  d[pos] << " -> " << (*domainBuffer)[pos] << std::endl;
  }

  *domain = *domainBuffer;
}
