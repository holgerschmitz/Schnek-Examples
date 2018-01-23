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
#include <schnek/util/singleton.hpp>

#include <boost/type_traits.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>

#include <gtk/gtk.h>

class GtkWindowInterface : public schnek::Singleton<GtkWindowInterface> {
  public:
    typedef schnek::Grid<double, 2> GridType;
    typedef boost::shared_ptr<GridType> pGridType;
  private:
    struct WindowRunner
    {
      GtkWindowInterface &interface;
      WindowRunner(GtkWindowInterface &interface);
      void operator()();
    };

    struct pixel
    {
        guchar c[3];
    };

    typedef schnek::Grid<pixel, 2> Image;

    boost::mutex mtx;

    pGridType bufferA, bufferB;
    Image image;
    bool bufferUpdated;

    boost::shared_ptr<WindowRunner> runner;
    boost::thread runner_thread;

    GdkPixbuf *pixbuf;

    GtkWindowInterface();
    void startGtkApplication();

    friend class schnek::CreateUsingNew<GtkWindowInterface>;
    friend class schnek::Singleton<GtkWindowInterface>;
  public:
    pGridType getBuffer();
    void updateBuffer();
    void updateCheck();
    void activate(GtkApplication* app, gpointer user_data);
    gboolean render(); //GtkGLArea *area, GdkGLContext *context);
};

template<class GridType>
class ColorPlot : public schnek::ChildBlock<ColorPlot<GridType> >
{
  private:
    std::string fieldName;
    double min;
    double max;

    typedef boost::shared_ptr<GridType> pGridType;
    pGridType grid;
  protected:
    void initParameters(schnek::BlockParameters &blockPars);
  public:
    void execute();
    void init();

};

#include "colorplot.t"

#endif /* INCLUDE_GUI_COLORPLOT_HPP_ */
