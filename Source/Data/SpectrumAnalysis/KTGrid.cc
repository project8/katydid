/*
 * KTGrid.hh
 *
 *  Created on: May 1, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTGrid.hh"

namespace Katydid
{
  KTGrid::KTGrid()
  {
  }
  
  KTGrid::~KTGrid()
  {
    fGridPoints.clear();
  }
  
  void KTGrid::SetNGridPoints(unsigned num)
  {
    unsigned oldSize = fGridPoints.size();
    if(oldSize>num)
    {
      fGridPoints.erase(fGridPoints.begin()+num,fGridPoints.begin()+oldSize);
      fSummedGridVoltages.erase(fSummedGridVoltages.begin()+num,fSummedGridVoltages.begin()+oldSize);
    }
    fGridPoints.resize(num);
    fSummedGridVoltages.resize(num);
  }
} /* namespace Katydid */
