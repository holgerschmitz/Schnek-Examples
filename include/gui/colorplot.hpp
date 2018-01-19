/*
 * colorplot.hpp
 *
 *  Created on: 19 Jan 2018
 *      Author: holger
 */

#ifndef INCLUDE_GUI_COLORPLOT_HPP_
#define INCLUDE_GUI_COLORPLOT_HPP_


#include <schnek/grid/grid.hpp>
#include <schnek/grid/gridtransform.hpp>
#include <schnek/variables.hpp>
#include <schnek/variables/blockcontainer.hpp>

#include <boost/type_traits.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>

struct WindowRunner
{
    void operator()();
};

template<class GridType>
class ColorPlot : public schnek::ChildBlock<ColorPlot<GridType> >
{
  private:
    std::string fieldName;
    double min;
    double max;

    typedef boost::shared_ptr<GridType> pGridType;

    struct GridCaster {
      typedef schnek::GridTransform<
          GridType,
          schnek::TypeCastTransform<typename GridType::value_type, double>
      > DestType;
      typedef boost::shared_ptr<DestType> pDestType;
      static pDestType getDest(pGridType grid)
      {
        pDestType dest(new DestType(*grid));
        return dest;
      }
    };
    struct GridPasser {
      typedef GridType DestType;
      typedef boost::shared_ptr<DestType> pDestType;
      static pGridType getDest(pGridType grid)
      {
        return grid;
      }
    };

    typedef typename boost::conditional<
        boost::is_same<typename GridType::value_type, double>::value,
        GridPasser,
        GridCaster >::type ConverterType;


    typename ConverterType::pDestType grid;
    boost::shared_ptr<WindowRunner> runner;
    boost::thread runner_thread;
  protected:
    void initParameters(schnek::BlockParameters &blockPars);
  public:
    void execute();
    void init();

};

#include "colorplot.t"

#endif /* INCLUDE_GUI_COLORPLOT_HPP_ */
