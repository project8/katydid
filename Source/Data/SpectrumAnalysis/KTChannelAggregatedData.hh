/**
 @file KTAggregatedFrequencyData
 @brief Contains KTAggregatedFrequencyData
 @details The summed spectra from all the channels in the azimuthal direction.
 Also includes of the reconstructed radial position of the electron.
 Currently voltage summation in the frequency domain is performed.
 Can be extended to the power summation and in the time domain.
 Warning: It is the user's responsibility to make sure that the spectra
 corresponds to the right grid point.
 @author: P. T. Surukuchi
 @date: Apr 8, 2019
 */

#ifndef KTCHANNELAGGREGATEDDATA_HH_
#define KTCHANNELAGGREGATEDDATA_HH_

#include "KTData.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"

#include <vector>

namespace Katydid
{
  class KTAggregatedDataCore
  {
  public:
    KTAggregatedDataCore();
    
    virtual ~KTAggregatedDataCore();
    
    virtual void SetNGridPoints(unsigned);
    
    int GetNGridPoints();
    
    /*
     Set the X,Y pair corresponding to the grid point number.
     */
    virtual void SetGridPoint(int,double,double);
    
    /*
     Get the X,Y pair corresponding to the grid point number.
     */
    virtual void GetGridPoint(int,double &, double &) const;
    
    //Set the X,Y pair corresponding to the grid point number.
    virtual void SetSummedGridMagnitude(int,double);
    
    //
    virtual double GetSummedGridMagnitude(int) const;
    
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
    
    /// Get whether the defined grid is a square grid. PTS: Currently just assumes a square grid. Will need updating. Currently just dummy
//    virtual bool GetIsSquareGrid() const;
    
    /// Set the radius of the active volume
    virtual void SetActiveRadius(double);
    
    /// Set the radius of the active volume
    virtual double GetActiveRadius() const;
    
  protected:
    
    struct KTGrid
    {
      /// The X position of the grid point
      double fGridPointX;
      
      /// The Y position of the grid point
      double fGridPointY;
      
      /// Magnitude at the defined grid location
      double fMagnitude;
    };
    typedef std::vector<KTGrid> SetOfGridPoints;
    
    SetOfGridPoints fGridPoints;
    
    /// The element in the vector corresponding to the optimized grid point
    int fOptimizedGridPoint=-1;
    
    /// The optimized value of summed value
    double fOptimizedGridPointValue;
    
    /// The radius of the active detector volume
    double fActiveRadius;
    
    bool isGridOptimized=false;
    
    bool isSquareGrid=true;
  };
  
  class KTAggregatedFrequencySpectrumDataFFTW :public KTAggregatedDataCore,public KTFrequencySpectrumDataFFTWCore,public Nymph::KTExtensibleData< KTAggregatedFrequencySpectrumDataFFTW >
  {
  public:
    KTAggregatedFrequencySpectrumDataFFTW();
    
    virtual ~KTAggregatedFrequencySpectrumDataFFTW();
    
    static const std::string sName;
    
    virtual KTAggregatedFrequencySpectrumDataFFTW& SetNComponents(unsigned);
    
    virtual void SetSummedGridVoltage(int,double);
    
    virtual double GetSummedGridVoltage(int) const;
  };
  
  inline int KTAggregatedDataCore::GetNGridPoints()
  {
    return fGridPoints.size();
  }
  
  inline void KTAggregatedDataCore::SetGridPoint(int component,double gridValueX,double gridValueY)
  {
    fGridPoints[component].fGridPointX=gridValueX;
    fGridPoints[component].fGridPointY=gridValueY;
    return;
  }
  
  inline void KTAggregatedDataCore::GetGridPoint(int component, double &gridLocationX,double &gridLocationY) const
  {
    gridLocationX=fGridPoints[component].fGridPointX;
    gridLocationY=fGridPoints[component].fGridPointY;
    return;
  }
  
  inline void KTAggregatedDataCore::SetSummedGridMagnitude(int component,double magnitude)
  {
    fGridPoints[component].fMagnitude=magnitude;
    return;
  }
  
  inline double KTAggregatedDataCore::GetSummedGridMagnitude(int component) const
  {
    return fGridPoints[component].fMagnitude;
  }
  
  inline void KTAggregatedDataCore::SetOptimizedGridPoint(int nGridPoint)
  {
    fOptimizedGridPoint=nGridPoint;
    isGridOptimized=true;
    return;
  }
  
  inline void KTAggregatedDataCore::SetOptimizedGridPointValue(int nGridPoint, double value)
  {
    fOptimizedGridPointValue=value;
    SetOptimizedGridPoint(nGridPoint);
    return;
  }
  
  inline int KTAggregatedDataCore::GetOptimizedGridPoint() const
  {
    return fOptimizedGridPoint;
  }
  
  inline double KTAggregatedDataCore::GetOptimizedGridValue() const
  {
    return fOptimizedGridPointValue;
  }
  
  inline void KTAggregatedDataCore::SetActiveRadius(double activeRadius){
    fActiveRadius=activeRadius;
    return;
  }
  
  inline double KTAggregatedDataCore::GetActiveRadius() const{
    return fActiveRadius;
  }
  
//  inline bool KTAggregatedDataCore::GetIsSquareGrid() const
//  {
//    return isSquareGrid;
//  }
  
  inline void KTAggregatedFrequencySpectrumDataFFTW::SetSummedGridVoltage(int component,double magnitude)
  {
    KTAggregatedDataCore::SetSummedGridMagnitude(component,magnitude);
  }
  
  inline double KTAggregatedFrequencySpectrumDataFFTW::GetSummedGridVoltage(int component) const{
    return KTAggregatedDataCore::GetSummedGridMagnitude(component);
  }
  
}/* namespace Katydid */
#endif /* KTCHANNELAGGREGATEDDATA_HH_ */
