/*
 * sandpile.hpp
 *
 * Created on: 28 Sep 2015
 * Author: Holger Schmitz
 * Email: holger@notjustphysics.com
 *
 * Copyright 2012 Holger Schmitz
 *
 * This file is part of Schnek Examples.
 *
 * Schnek Examples is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Schnek Examples is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Schnek Examples.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SANDPILE_HPP_
#define SANDPILE_HPP_

#include <schnek/grid.hpp>
#include <schnek/variables.hpp>
#include <schnek/diagnostic/diagnostic.hpp>
#include <schnek/diagnostic/hdfdiagnostic.hpp>

#include <boost/ref.hpp>

#include <deque>

using namespace schnek;

static const int DIMENSION = 2;
static const int GLIM = 4;

typedef Array<int, DIMENSION> Index;
typedef Field<int, DIMENSION> Pile;
typedef Pile* pPile;

class Sandpile : public Block
{
  private:
    Index globalSize;
    pPile pile_ptr;
    pPile temp_pile_ptr;
    Field<char, DIMENSION> queueFlags;
    int grains;
    int doublings;

    MPICartSubdivision<Pile> subdivision;
    std::deque<Index> indexQueue;
    Range<int, DIMENSION> innerRange;

    void addGrains(int n, int px, int py);
  protected:
    void initParameters(BlockParameters &blockPars);
    void registerData();
  public:
    void execute();
    void init();
};

class SandpileDiagnostic : public schnek::HDFGridDiagnostic<Pile, Pile* >
{
  protected:
    typedef HDFGridDiagnostic<Pile, Pile* >::IndexType IndexType;
    IndexType getGlobalMin();
    IndexType getGlobalMax();
};


#endif /* SANDPILE_HPP_ */
