/*
 * sandpile.cpp
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


#include "sandpile.hpp"

#include <schnek/parser.hpp>
//#include <schnek/variables/block.hpp>
//#include <schnek/grid/array.hpp>

#include <boost/ref.hpp>

#include <mpi.h>

Index globalMax;

void Sandpile::initParameters(BlockParameters &blockPars)
{
  blockPars.addArrayParameter("size", globalSize);
  blockPars.addParameter("grains", &grains);
  blockPars.addParameter("doublings", &doublings);
}

void Sandpile::registerData()
{
  addData("pile", pile_ptr);
}

void Sandpile::init()
{
  globalMax = globalSize;
  globalMax -= 1;
  subdivision.init(globalSize, 1);

  Index low  = subdivision.getLo();
  Index high = subdivision.getHi();

  Index lowIn  = subdivision.getInnerLo();
  Index highIn = subdivision.getInnerHi();

  innerRange = Range<int, DIMENSION>(lowIn, highIn);
  Range<double, DIMENSION> domain(Array<double, DIMENSION>(lowIn[0],lowIn[1]), Array<double, DIMENSION>(highIn[0],highIn[1]));
  Array<bool, DIMENSION> stagger(false, false);
  pile_ptr = new Pile(lowIn, highIn, domain, stagger, 1);
  temp_pile_ptr = new Pile(lowIn, highIn, domain, stagger, 1);
  queueFlags.resize(lowIn, highIn, domain, stagger, 1);

  Pile &pile = *pile_ptr;

  pile = 0;
  queueFlags = 0;

  Index mid = globalSize/2;

  if (innerRange.inside(mid))
  {
    pile[mid] = grains;
    queueFlags[mid] = 1;
    indexQueue.push_back(mid);
  }
}

inline void Sandpile::addGrains(int n, int px, int py)
{
  Pile &pile = *pile_ptr;
  int nb;
  nb = pile(px,py) + n;
  Index pos(px,py);
  if ((nb > GLIM) && innerRange.inside(pos) && (queueFlags(px,py)==0))
  {
    queueFlags(px,py)=1;
    indexQueue.push_back(pos);
  }
  pile(px,py) = nb;
}

void Sandpile::execute()
{
  Pile &pile = *pile_ptr;
  bool running = true;
  long stepCount = 0l;
  int diagCount = 0;
  bool master = subdivision.master();
  DiagnosticManager::instance().setTimeCounter(&diagCount);
  DiagnosticManager::instance().setMaster(master);
  DiagnosticManager::instance().setRank(subdivision.procnum());

  DiagnosticManager::instance().execute();
  ++diagCount;

  Index lo = pile.getLo();
  Index hi = pile.getHi();

  int xmin = innerRange.getLo()[0];
  int xmax = innerRange.getHi()[0];
  int ymin = innerRange.getLo()[1];
  int ymax = innerRange.getHi()[1];

  for (int d=0; d<=doublings; d++)
  {
    while (running)
    {
      while (!indexQueue.empty())
      {
        ++stepCount;
        Index pos = indexQueue.front();
        indexQueue.pop_front();
        int n = pile[pos] - 1;
        int d = n/GLIM; // integer division
        pile[pos] = n % GLIM + 1;
        queueFlags[pos] = 0;

        addGrains(d, pos[0]-1, pos[1]);
        addGrains(d, pos[0]+1, pos[1]);
        addGrains(d, pos[0], pos[1]-1);
        addGrains(d, pos[0], pos[1]+1);
        if (master && (stepCount%1000000l == 0l))
        {
          std::cout << "Step " << stepCount << ": q=" << indexQueue.size() << std::endl;
        }
      }

      subdivision.accumulate(pile,0);
      subdivision.accumulate(pile,1);

      int queueCount = 0;

      for (int x=xmin; x<=xmax; ++x)
      {
        if (pile(x,ymin)>GLIM)
        {
          ++queueCount;
          queueFlags(x,ymin)=1;
          indexQueue.push_back(Index(x,ymin));
        }
        if (pile(x,ymax)>GLIM)
        {
          ++queueCount;
          queueFlags(x,ymax)=1;
          indexQueue.push_back(Index(x,ymax));
        }
        pile(x,ymin-1) = 0;
        pile(x,ymax+1) = 0;
      }

      for (int y=ymin; y<=ymax; ++y)
      {
        if (pile(xmin,y)>GLIM)
        {
          ++queueCount;
          queueFlags(xmin,y)=1;
          indexQueue.push_back(Index(xmin,y));
        }
        if (pile(xmax,y)>GLIM)
        {
          ++queueCount;
          queueFlags(xmax,y)=1;
          indexQueue.push_back(Index(xmax,y));
        }
        pile(xmin-1,y) = 0;
        pile(xmax+1,y) = 0;
      }
      pile(xmin-1,ymin-1) = 0;
      pile(xmin-1,ymax+1) = 0;
      pile(xmax+1,ymin-1) = 0;
      pile(xmax+1,ymax+1) = 0;

      queueCount = subdivision.sumReduce(queueCount);
      running = (queueCount>0);
    }

    if (d<doublings) {
      std::cout << "Doubling!\n";
      Pile &temp = *temp_pile_ptr;
      //temp = pile;
      //pile = 0;

      running = true;
      int xlo=xmax;
      int xhi=xmin;
      int ylo=ymax;
      int yhi=ymin;

      for (int x=xmin; x<=xmax; ++x)
        for (int y=ymin; y<=ymax; ++y) {
          temp(x,y) = pile(x,y);
          pile(x,y) = 0;
          if (temp(x,y)>0) {
            xlo = std::min(xlo,x);
            xhi = std::max(xhi,x);
            ylo = std::min(ylo,y);
            yhi = std::max(yhi,y);
          }
        }

      int xh = (xmax+xmin)/2;
      int yh = (ymax+ymin)/2;

      std::cout << "Domain: " << xlo << " " << xhi << " " << ylo << " " << yhi <<  "\n";

      for (int x=xlo; x<=xhi; ++x)
        for (int y=ylo; y<=yhi; ++y) {
          int n = temp(x,y);
          if (n>0) {
            for (int xt=xlo; xt<=xhi; ++xt)
              for (int yt=ylo; yt<=yhi; ++yt) {
                int m = temp(xt,yt);
                if (m>0) {
                  addGrains(x+xt-xh, y+yt-yh, n*m);
                }
              }
          }
        }

      std::cout << "Done doubling!\n";
    }

  }
  diagCount = -1;
  DiagnosticManager::instance().execute();
  std::cout << "Run completed" << std::endl;
  std::cout << "at step " << stepCount << std::endl;
}


SandpileDiagnostic::IndexType SandpileDiagnostic::getGlobalMin()
{
  return IndexType(0);
}

SandpileDiagnostic::IndexType SandpileDiagnostic::getGlobalMax()
{
  return globalMax;
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  try
  {
    BlockClasses blocks;

    blocks.registerBlock("sandpile").setClass<Sandpile>();
    blocks("Diagnostic").setClass<SandpileDiagnostic>();
    blocks("sandpile").addChildren("Diagnostic");

    std::ifstream in("sandpile.setup");

    Parser P("my_simulation", "sandpile", blocks);
    registerCMath(P.getFunctionRegistry());
    pBlock application = P.parse(in);

    Sandpile &mysim = dynamic_cast<Sandpile&>(*application);
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
