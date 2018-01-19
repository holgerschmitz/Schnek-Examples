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
}

template<class GridType>
void ColorPlot<GridType>::init()
{
  pGridType tmp_field;
  this->retrieveData(fieldName, tmp_field);
  grid = ConverterType::getDest(tmp_field);

  runner = boost::make_shared<WindowRunner>();
  runner_thread = boost::thread(*runner);
}


