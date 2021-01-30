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
#include "KTTimeSeriesFFTW.hh"

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

            int GetNGridPoints();

            /*
             Set the X,Y pair corresponding to the grid point number.
             */
            virtual void SetGridPoint(int component, double gridValueX, double gridValueY);

            /*
             Get the X,Y pair corresponding to the grid point number.
             */
            virtual void GetGridPoint(int component, double &gridLocationX, double &gridLocationY) const;

            //Set the X,Y pair corresponding to the grid point number.
            virtual void SetSummedGridMagnitude(int component, double magnitude);

            //
            virtual double GetSummedGridMagnitude(int component) const;

            /// Set the grid number and the value of the optimal grid point
            virtual void SetOptimizedGridPointValue(int nGridPoint, double value);

            /*
             Set the element number in the vector of the grid point that corresponds to the optimum summed value.
             */
            virtual void SetOptimizedGridPoint(int nGridPoint);

            /*
             Get the element number in the vector of the grid point that corresponds to the optimum summed value.
             */
            virtual int GetOptimizedGridPoint() const;

            /// Get the value of the optimal grid point
            virtual double GetOptimizedGridValue() const;

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
            typedef std::vector< KTGrid > SetOfGridPoints;

            SetOfGridPoints fGridPoints;

            /// The element in the vector corresponding to the optimized grid point
            int fOptimizedGridPoint;

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

            virtual void SetSummedGridVoltage(int, double);

            virtual double GetSummedGridVoltage(int) const;

            static const std::string sName;
    };

    class KTAggregatedTimeSeriesDataFFTW : public KTAggregatedDataCore, public KTTimeSeriesFFTW, public Nymph::KTExtensibleData< KTAggregatedTimeSeriesDataFFTW >
    {
        public:
            KTAggregatedTimeSeriesDataFFTW();

            virtual ~KTAggregatedTimeSeriesDataFFTW();

            virtual KTAggregatedTimeSeriesDataFFTW& SetNComponents(unsigned);

            virtual void SetSummedGridVoltage(int, double);

            virtual double GetSummedGridVoltage(int) const;

            static const std::string sName;
    };

    class KTAggregatedPowerSpectrumData : public KTAggregatedDataCore, public KTPowerSpectrumDataCore, public Nymph::KTExtensibleData< KTAggregatedPowerSpectrumData >
    {
        public:
            KTAggregatedPowerSpectrumData();

            virtual ~KTAggregatedPowerSpectrumData();

            virtual KTAggregatedPowerSpectrumData& SetNComponents(unsigned);

            virtual void SetSummedGridPower(int, double);

            virtual double GetSummedGridPower(int) const;

            static const std::string sName;
    };

    inline int KTAggregatedDataCore::GetNGridPoints()
    {
        return fGridPoints.size();
    }

    inline void KTAggregatedDataCore::SetGridPoint(int component, double gridValueX, double gridValueY)
    {
        fGridPoints[component].fGridPointX = gridValueX;
        fGridPoints[component].fGridPointY = gridValueY;
        return;
    }

    inline void KTAggregatedDataCore::GetGridPoint(int component, double &gridLocationX, double &gridLocationY) const
    {
        gridLocationX = fGridPoints[component].fGridPointX;
        gridLocationY = fGridPoints[component].fGridPointY;
        return;
    }

    inline void KTAggregatedDataCore::SetSummedGridMagnitude(int component, double magnitude)
    {
        fGridPoints[component].fMagnitude = magnitude;
        return;
    }

    inline double KTAggregatedDataCore::GetSummedGridMagnitude(int component) const
    {
        return fGridPoints[component].fMagnitude;
    }

    inline void KTAggregatedDataCore::SetOptimizedGridPoint(int nGridPoint)
    {
        fOptimizedGridPoint = nGridPoint;
        fIsGridOptimized = true;
        return;
    }

    inline void KTAggregatedDataCore::SetOptimizedGridPointValue(int nGridPoint, double value)
    {
        fOptimizedGridPointValue = value;
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

    //  inline bool KTAggregatedDataCore::GetIsSquareGrid() const
    //  {
    //    return isSquareGrid;
    //  }

    inline void KTAggregatedFrequencySpectrumDataFFTW::SetSummedGridVoltage(int component, double magnitude)
    {
        KTAggregatedDataCore::SetSummedGridMagnitude(component, magnitude);
    }

    inline double KTAggregatedFrequencySpectrumDataFFTW::GetSummedGridVoltage(int component) const
    {
        return KTAggregatedDataCore::GetSummedGridMagnitude(component);
    }

    inline void KTAggregatedTimeSeriesDataFFTW::SetSummedGridVoltage(int component, double magnitude)
    {
        KTAggregatedDataCore::SetSummedGridMagnitude(component, magnitude);
    }

    inline double KTAggregatedTimeSeriesDataFFTW::GetSummedGridVoltage(int component) const
    {
        return KTAggregatedDataCore::GetSummedGridMagnitude(component);
    }

    inline void KTAggregatedPowerSpectrumData::SetSummedGridPower(int component, double magnitude)
    {
        KTAggregatedDataCore::SetSummedGridMagnitude(component, magnitude);
    }

    inline double KTAggregatedPowerSpectrumData::GetSummedGridPower(int component) const
    {
        return KTAggregatedDataCore::GetSummedGridMagnitude(component);
    }

}/* namespace Katydid */
#endif /* KTCHANNELAGGREGATEDDATA_HH_ */
