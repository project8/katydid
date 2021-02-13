/**
 @file KTAggregatedFrequencyData
 @brief Contains KTAggregatedDataCore and KTAggregatedFrequencySpectrumDataFFTW
 @details The summed spectra from all the channels in the azimuthal direction.
 Also includes the reconstructed position of the electron in cartesian coordinate system.
 Currently voltage summation in the frequency domain is performed.
 Can be extended to the power summation as well as in the time domain.
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
#include "KTPowerSpectrumData.hh"

#include "KTMemberVariable.hh"

#include <vector>

namespace Katydid
{
    class KTAggregatedDataCore
    {
        public:
            KTAggregatedDataCore();

            virtual ~KTAggregatedDataCore();

            virtual void SetNGridPoints(unsigned num);

            unsigned GetNGridPoints();
            
            /*
             Set the X,Y pair corresponding to the grid point number.
             */
            virtual void SetGridPoint(unsigned component, double gridValueX, double gridValueY, double gridValueZ);

            /*
             Get the X,Y pair corresponding to the grid point number.
             */
            virtual void GetGridPoint(unsigned component, double &gridLocationX, double &gridLocationY, double &gridValueZ) const;

            ///Set the number of axial points
            /* 
             The points along the axis are discrete since it is defined by the number of rings/suarrays used
             */
            virtual void SetNAxialPositions(unsigned num);

            ///Get the number of axial points
            unsigned GetNAxialPositions() const;

            //Set the X,Y pair corresponding to the grid point number.
            virtual void SetSummedGridMagnitude(unsigned component, double magnitude);

            //Get the magnitude given the component
            virtual double GetSummedGridMagnitude(unsigned component) const;

            /// Set the grid number and the value of the optimal grid point
            virtual void SetOptimizedGridPointValue(unsigned nGridPoint, double value);

            /*
             Set the element number in the vector of the grid point that corresponds to the optimum summed value.
             */
            virtual void SetOptimizedGridPoint(unsigned nGridPoint);

            /*
             Get the element number in the vector of the grid point that corresponds to the optimum summed value.
             */
            virtual unsigned GetOptimizedGridPoint() const;

            /// Get the value of the optimal grid point
            virtual double GetOptimizedGridValue() const;

        protected:

            struct KTGrid
            {
                /// The X position of the grid point
                double fGridPointX;

                /// The Y position of the grid point
                double fGridPointY;

                /// The Z index of the grid point
                double fZIndex;

                /// The Z position of the grid point
                double fGridPointZ;

                /// Magnitude at the defined grid location
                double fMagnitude;
            };
            typedef std::vector< KTGrid > SetOfGridPoints;

            SetOfGridPoints fGridPoints;

            unsigned fNAxialPositions;

            /// The element in the vector corresponding to the optimized grid point
            unsigned fOptimizedGridPoint;

            /// The optimized value of summed value
            double fOptimizedGridPointValue;

        public:

            /// The radius of the active detector volume
            MEMBERVARIABLE(double, ActiveRadius);

            MEMBERVARIABLE(bool, IsGridOptimized);

            MEMBERVARIABLE(bool, IsSquareGrid);
    };

    class KTAggregatedFrequencySpectrumDataFFTW : public KTAggregatedDataCore, public KTFrequencySpectrumDataFFTWCore, public Nymph::KTExtensibleData< KTAggregatedFrequencySpectrumDataFFTW >
    {
        public:
            KTAggregatedFrequencySpectrumDataFFTW();

            virtual ~KTAggregatedFrequencySpectrumDataFFTW();

            virtual KTAggregatedFrequencySpectrumDataFFTW& SetNComponents(unsigned);

            virtual void SetSummedGridVoltage(unsigned, double);

            virtual double GetSummedGridVoltage(unsigned) const;

            static const std::string sName;
    };

    class KTAggregatedPowerSpectrumData : public KTAggregatedDataCore, public KTPowerSpectrumDataCore, public Nymph::KTExtensibleData< KTAggregatedPowerSpectrumData >
    {
        public:
            KTAggregatedPowerSpectrumData();

            virtual ~KTAggregatedPowerSpectrumData();

            virtual KTAggregatedPowerSpectrumData& SetNComponents(unsigned);

            virtual void SetSummedGridPower(unsigned, double);

            virtual double GetSummedGridPower(unsigned) const;

            static const std::string sName;
    };

    inline unsigned KTAggregatedDataCore::GetNGridPoints()
    {
        return fGridPoints.size();
    }

    inline void KTAggregatedDataCore::SetGridPoint(unsigned component, double gridValueX, double gridValueY, double gridValueZ)
    {
        fGridPoints[component].fGridPointX = gridValueX;
        fGridPoints[component].fGridPointY = gridValueY;
        fGridPoints[component].fGridPointZ = gridValueZ;
        return;
    }

    inline void KTAggregatedDataCore::GetGridPoint(unsigned component, double &gridLocationX, double &gridLocationY, double &gridLocationZ) const
    {
        gridLocationX = fGridPoints[component].fGridPointX;
        gridLocationY = fGridPoints[component].fGridPointY;
        gridLocationZ = fGridPoints[component].fGridPointZ;
        return;
    }

    inline void KTAggregatedDataCore::SetNAxialPositions(unsigned num)
    {
        fNAxialPositions=num;
    }

    inline unsigned KTAggregatedDataCore::GetNAxialPositions() const
    {
        return fNAxialPositions;
    }

    inline void KTAggregatedDataCore::SetSummedGridMagnitude(unsigned component, double magnitude)
    {
        fGridPoints[component].fMagnitude = magnitude;
        return;
    }

    inline double KTAggregatedDataCore::GetSummedGridMagnitude(unsigned component) const
    {
        return fGridPoints[component].fMagnitude;
    }

    inline void KTAggregatedDataCore::SetOptimizedGridPoint(unsigned nGridPoint)
    {
        fOptimizedGridPoint = nGridPoint;
        fIsGridOptimized = true;
        return;
    }

    inline void KTAggregatedDataCore::SetOptimizedGridPointValue(unsigned nGridPoint, double value)
    {
        fOptimizedGridPointValue = value;
        SetOptimizedGridPoint(nGridPoint);
        return;
    }

    inline unsigned KTAggregatedDataCore::GetOptimizedGridPoint() const
    {
        return fOptimizedGridPoint;
    }

    inline double KTAggregatedDataCore::GetOptimizedGridValue() const
    {
        return fOptimizedGridPointValue;
    }

    //  inline bool KTAggregatedDataCore::GetIsSquareGrid() const
    //  {
    //    return isSquareGrid;
    //  }

    inline void KTAggregatedFrequencySpectrumDataFFTW::SetSummedGridVoltage(unsigned component, double magnitude)
    {
        KTAggregatedDataCore::SetSummedGridMagnitude(component, magnitude);
    }

    inline double KTAggregatedFrequencySpectrumDataFFTW::GetSummedGridVoltage(unsigned component) const
    {
        return KTAggregatedDataCore::GetSummedGridMagnitude(component);
    }

    inline void KTAggregatedPowerSpectrumData::SetSummedGridPower(unsigned component, double magnitude)
    {
        KTAggregatedDataCore::SetSummedGridMagnitude(component, magnitude);
    }

    inline double KTAggregatedPowerSpectrumData::GetSummedGridPower(unsigned component) const
    {
        return KTAggregatedDataCore::GetSummedGridMagnitude(component);
    }

}/* namespace Katydid */
#endif /* KTCHANNELAGGREGATEDDATA_HH_ */
