/*
 * neighbourhood.hpp
 *
 *  Created on: 14 Sep 2016
 *      Author: holger
 */

#ifndef SRC_NEIGHBOURHOOD_HPP_
#define SRC_NEIGHBOURHOOD_HPP_

#include "blockcontainer.hpp"
#include "cellular.hpp"

#include <set>

class Neighbourhood : public ChildBlock<Neighbourhood>
{
  public:
    virtual void getNeighbourCells(std::list<Index> &neighbourCells) const = 0;
};

class NeumannNeighbourhood : public Neighbourhood
{
  public:
    void getNeighbourCells(std::list<Index> &neighbourCells) const;
};

class MooreNeighbourhood : public Neighbourhood
{
  public:
    void getNeighbourCells(std::list<Index> &neighbourCells) const;
};

class NeumannNeighbourhoodNoCentre : public Neighbourhood
{
  public:
    void getNeighbourCells(std::list<Index> &neighbourCells) const;
};

class MooreNeighbourhoodNoCentre : public Neighbourhood
{
  public:
    void getNeighbourCells(std::list<Index> &neighbourCells) const;
};

#endif /* SRC_NEIGHBOURHOOD_HPP_ */
