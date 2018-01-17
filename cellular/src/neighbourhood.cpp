/*
 * neighbourhood.cpp
 *
 *  Created on: 14 Sep 2016
 *  Author: Holger Schmitz
 */

#include "neighbourhood.hpp"

#include <schnek/grid/range.hpp>

void NeumannNeighbourhood::getNeighbourCells(std::list<Index>& neighbourCells) const
{
  neighbourCells.push_back(Index( 0, 0));

  neighbourCells.push_back(Index( 1, 0));
  neighbourCells.push_back(Index(-1, 0));
  neighbourCells.push_back(Index( 0, 1));
  neighbourCells.push_back(Index( 0,-1));
}

void MooreNeighbourhood::getNeighbourCells(std::list<Index>& neighbourCells) const
{
  neighbourCells.push_back(Index( 0, 0));

  neighbourCells.push_back(Index( 1, 0));
  neighbourCells.push_back(Index(-1, 0));
  neighbourCells.push_back(Index( 0, 1));
  neighbourCells.push_back(Index( 0,-1));

  neighbourCells.push_back(Index( 1, 1));
  neighbourCells.push_back(Index(-1, 1));
  neighbourCells.push_back(Index( 1,-1));
  neighbourCells.push_back(Index(-1,-1));
}

void NeumannNeighbourhoodNoCentre::getNeighbourCells(std::list<Index>& neighbourCells) const
{
  neighbourCells.push_back(Index( 1, 0));
  neighbourCells.push_back(Index(-1, 0));
  neighbourCells.push_back(Index( 0, 1));
  neighbourCells.push_back(Index( 0,-1));
}

void MooreNeighbourhoodNoCentre::getNeighbourCells(std::list<Index>& neighbourCells) const
{
  neighbourCells.push_back(Index( 1, 0));
  neighbourCells.push_back(Index(-1, 0));
  neighbourCells.push_back(Index( 0, 1));
  neighbourCells.push_back(Index( 0,-1));

  neighbourCells.push_back(Index( 1, 1));
  neighbourCells.push_back(Index(-1, 1));
  neighbourCells.push_back(Index( 1,-1));
  neighbourCells.push_back(Index(-1,-1));
}
