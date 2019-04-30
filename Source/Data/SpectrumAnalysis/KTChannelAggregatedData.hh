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
  
  class KTAggregatedFrequencySpectrumDataFFTW :public KTFrequencySpectrumDataFFTWCore,public Nymph::KTExtensibleData< KTAggregatedFrequencySpectrumDataFFTW >
  {
  public:
    KTAggregatedFrequencySpectrumDataFFTW();
    virtual ~KTAggregatedFrequencySpectrumDataFFTW();
    
    virtual KTAggregatedFrequencySpectrumDataFFTW& SetNComponents(unsigned);
    
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
  public:
    static const std::string sName;
    
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
  
  inline void KTAggregatedFrequencySpectrumDataFFTW::SetGridPoint(int component,double gridValueX,double gridValueY)
  {
    fGridPoints[component]=std::make_pair(gridValueX,gridValueY);
    return;
  }
  
  inline void KTAggregatedFrequencySpectrumDataFFTW::GetGridPoint(int component, double &gridLocationX,double &gridLocationY) const
  {
    gridLocationX=fGridPoints[component].first;
    gridLocationY=fGridPoints[component].second;
    return;
  }
  
  inline void KTAggregatedFrequencySpectrumDataFFTW::SetSummedGridVoltage(int component,double voltage)
  {
    fSummedGridVoltages[component]=voltage;
    return;
  }
  
  inline double KTAggregatedFrequencySpectrumDataFFTW::GetSummedGridVoltage(int component) const
  {
    return fSummedGridVoltages[component];
  }
  
  inline void KTAggregatedFrequencySpectrumDataFFTW::SetOptimizedGridPoint(int nGridPoint)
  {
    fOptimizedGridPoint=nGridPoint;
    const KTFrequencySpectrumFFTW* freqSpectrum=GetSpectrumFFTW(nGridPoint);
    isGridOptimized=true;
    return;
  }
  
  inline void KTAggregatedFrequencySpectrumDataFFTW::SetOptimizedGridPointValue(int nGridPoint, double value)
  {
    fOptimizedGridPointValue=value;
    SetOptimizedGridPoint(nGridPoint);
    return;
  }
  
  inline int KTAggregatedFrequencySpectrumDataFFTW::GetOptimizedGridPoint() const
  {
    return fOptimizedGridPoint;
  }
  
  inline double KTAggregatedFrequencySpectrumDataFFTW::GetOptimizedGridValue() const
  {
    return fOptimizedGridPointValue;
  }
  
  inline void KTAggregatedFrequencySpectrumDataFFTW::SetActiveRadius(double activeRadius){
    fActiveRadius=activeRadius;
    return;
  }
  
  inline double KTAggregatedFrequencySpectrumDataFFTW::GetActiveRadius() const{
    return fActiveRadius;
  }
  
  inline bool KTAggregatedFrequencySpectrumDataFFTW::GetIsSquareGrid() const
  {
    return isSquareGrid;
  }
} /* namespace Katydid */

#endif /* KTCHANNELAGGREGATEDDATA_HH_ */
