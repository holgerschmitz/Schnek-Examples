/*
 * colorplot.t
 *
 *  Created on: 19 Jan 2018
 *      Author: Holger Schmitz
 */


template<class GridType>
void ColorPlot<GridType>::initParameters(schnek::BlockParameters &blockPars)
{
  blockPars.addParameter("field", &fieldName);
  blockPars.addParameter("min", &min);
  blockPars.addParameter("max", &max);
}


template<class GridType>
void ColorPlot<GridType>::execute()
{
  boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
  GtkWindowInterface::GridType &buffer = *GtkWindowInterface::instance().getBuffer();

  GtkWindowInterface::GridType::IndexType lo = buffer.getLo();
  GtkWindowInterface::GridType::IndexType hi = buffer.getHi();

  GridType &g = *grid;

  for (int i=lo[0]; i<=hi[0]; ++i)
    for (int j=lo[1]; j<=hi[1]; ++j)
    {
      buffer(i,j) = std::max(1.0, std::min(0.0, (g(i,j) - min)/(max - min)));
    }

  GtkWindowInterface::instance().updateBuffer();

}

template<class GridType>
void ColorPlot<GridType>::init()
{
  this->retrieveData(fieldName, grid);
  GtkWindowInterface::pGridType buffer = GtkWindowInterface::instance().getBuffer();
  buffer->resize(*grid);
  GtkWindowInterface::instance().updateBuffer();
  buffer = GtkWindowInterface::instance().getBuffer();
  buffer->resize(*grid);
  GtkWindowInterface::instance().updateBuffer();
}


