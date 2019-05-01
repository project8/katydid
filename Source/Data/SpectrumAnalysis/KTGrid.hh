
/**
 @file KTGrid.hh
 @brief Contains KTGrid
 @details This class defines a grid class with X,Y and summed voltages for the position. that can then be used for searching/optimizing the position of the electron. Currently only done in cartesian coordinate systems. This can later be used to extended to polar as well if needed. For later, this can actually be just a struct with each struct object having positions and voltages and the classes that uses objects of this class can just make an array of these objects.
 @author: P. T. Surukuchi
 @date: May 1, 2019
 */

#ifndef KTGRID_HH_
#define KTGRID_HH_

#include <vector>

namespace Katydid
{
  
  class KTGrid
  {
    public :
    KTGrid();
    
    virtual ~KTGrid();
    
    virtual void SetNGridPoints(unsigned);
    
    /*
     Set the X,Y pair corresponding to the grid point number.
     */
    virtual void SetGridPoint(int,double,double);
    
    /*
     Get the X,Y pair corresponding to the grid point number.
     */
    virtual void GetGridPoint(int,double &, double &) const;
    
    //Set the X,Y pair corresponding to the grid point number.
    virtual void SetSummedGridVoltage(int,double);
    
    //
    virtual double GetSummedGridVoltage(int) const;
    
    /// Set the grid number and the value of the optimal grid point
    virtual void SetOptimizedGridPointValue(int, double);
    
    /*
     Set the element number in the vector of the grid point that corresponds to the optimum summed value.
     */
    virtual void SetOptimizedGridPoint(int);
    
    /*
     Get the element number in the vector of the grid point that corresponds to the optimum summed value.
     */
    virtual int GetOptimizedGridPoint() const;
    
    /// Get the value of the optimal grid point
    virtual double GetOptimizedGridValue() const;
    
    /// Get whether the defined grid is a square grid. PTS: Currently just assumes a square grid. Will need updating
    virtual bool GetIsSquareGrid() const;
    
    /// Set the radius of the active volume
    virtual void SetActiveRadius(double);
    
    /// Set the radius of the active volume
    virtual double GetActiveRadius() const;
    
  protected:
    /// A vector of pairs containing the X and Y grid locations respectively
    std::vector<std::pair<double,double>> fGridPoints;
    
    /// A vector of summed values of the grid voltages
    std::vector<double> fSummedGridVoltages;
    
    /// The element in the vector corresponding to the optimized grid point
    int fOptimizedGridPoint=-1;
    
    /// The optimized value of summed value
    double fOptimizedGridPointValue;
    
    /// The radius of the active detector volume
    double fActiveRadius;
    
    bool isGridOptimized=false;
    
    bool isSquareGrid=true;
  };
  
  inline void KTGrid::SetGridPoint(int component,double gridValueX,double gridValueY)
  {
    fGridPoints[component]=std::make_pair(gridValueX,gridValueY);
    return;
  }
  
  inline void KTGrid::GetGridPoint(int component, double &gridLocationX,double &gridLocationY) const
  {
    gridLocationX=fGridPoints[component].first;
    gridLocationY=fGridPoints[component].second;
    return;
  }
  
  inline void KTGrid::SetSummedGridVoltage(int component,double voltage)
  {
    fSummedGridVoltages[component]=voltage;
    return;
  }
  
  inline double KTGrid::GetSummedGridVoltage(int component) const
  {
    return fSummedGridVoltages[component];
  }
  
  inline void KTGrid::SetOptimizedGridPoint(int nGridPoint)
  {
    fOptimizedGridPoint=nGridPoint;
    isGridOptimized=true;
    return;
  }
  
  inline void KTGrid::SetOptimizedGridPointValue(int nGridPoint, double value)
  {
    fOptimizedGridPointValue=value;
    SetOptimizedGridPoint(nGridPoint);
    return;
  }
  
  inline int KTGrid::GetOptimizedGridPoint() const
  {
    return fOptimizedGridPoint;
  }
  
  inline double KTGrid::GetOptimizedGridValue() const
  {
    return fOptimizedGridPointValue;
  }
  
  inline void KTGrid::SetActiveRadius(double activeRadius){
    fActiveRadius=activeRadius;
    return;
  }
  
  inline double KTGrid::GetActiveRadius() const{
    return fActiveRadius;
  }
  
  inline bool KTGrid::GetIsSquareGrid() const
  {
    return isSquareGrid;
    }
}/* namespace Katydid */

#endif /* KTGRID_HH_ */
