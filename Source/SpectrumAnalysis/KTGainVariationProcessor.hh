/**
 @file KTGainVariationProcessor.hh
 @brief Contains KTGainVariationProcessor
 @details Processes the gain variation.
 @author: N. S. Oblath
 @date: Dec 10, 2012
 */

#ifndef KTGAINVARIATIONPROCESSOR_HH_
#define KTGAINVARIATIONPROCESSOR_HH_

#include "KTProcessor.hh"

#include "KTPhysicalArray.hh"
#include "KTSlot.hh"

#include <fftw3.h>

#include <list>

namespace Katydid
{
    class KTConvolvedFrequencySpectrumDataPolar;
    class KTConvolvedFrequencySpectrumDataFFTW;
    class KTConvolvedFrequencySpectrumVarianceDataPolar;
    class KTConvolvedFrequencySpectrumVarianceDataFFTW;
    class KTConvolvedPowerSpectrumData;
    class KTConvolvedPowerSpectrumVarianceData;
    class KTCorrelationData;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumVarianceDataCore;
    class KTFrequencySpectrumVarianceDataPolar;
    class KTFrequencySpectrumVarianceDataFFTW;
    class KTGainVariationData;
    class KTPowerSpectrumData;
    class KTPowerSpectrumDataCore;
    class KTPowerSpectrumVarianceData;
    //class KTSpline;

    /*!
     @class KTGainVariationProcessor
     @author N. S. Oblath

     @brief Fit the background shape and variance using 3rd-order splines.

     @details
     The spline fit is performed between fMinBin and fMaxBin, inclusive.
     If the [min,max] range has been set by frequency, those frequencies are turned into bins the first time they're used.

     There is an option to normalize the fit to the minimum value.

     For complex data types (e.g. fs-polar, and fs-fftw), the gain variation curve is taken from the modulus of the data.

     Configuration name: "gain-variation"

     Available configuration values:
     - "normalize": bool -- whether or not to normalize the output to the minimum value (default: true)
     - "min-frequency": double -- minimum frequency for the fit
     - "max-frequency": double -- maximum frequency for the fit
     - "min-bin": unsigned -- minimum bin for the fit
     - "max-bin": unsigned -- maximum bin for the fit
     - "fit-points": unsigned -- number of spline points to use in the fit
     - "variance-n-bins": unsigned -- number of bins to use around each point to calculate the variance if the variance is not provided using one of the "-var" slots

     Slots:
     - "fs-polar-var": void (Nymph::KTDataPtr) -- Calculates gain variation on a frequency spectrum and variance (Polar); Requires KTFrequencySpectrumDataPolar and KTFrequencySpectrumVarianceDataPolar; Adds KTGainVariationData
     - "fs-fftw-var": void (Nymph::KTDataPtr) -- Calculates gain variation on a frequency spectrum and variance (FFTW); Requires KTFrequencySpectrumDataFFTW and KTFrequencySpectrumVarianceDataFFTW; Adds KTGainVariationData
     - "ps-var": void (Nymph::KTDataPtr) -- Calculates gain variation on a power spectrum and variance; Requires KTPowerSpectrumData and KTPowerSpectrumVarianceData; Adds KTGainVariationData
     - "conv-fs-polar-var": void (Nymph::KTDataPtr) -- Calculates gain variation on a convolved frequency spectrum and variance (Polar); Requires KTConvolvedFrequnecySpectrumDataPolar and KTConvolvedFrequencySpectrumVarianceDataPolar; Adds KTGainVariationData
     - "conv-fs-fftw-var": void (Nymph::KTDataPtr) -- Calculates gain variation on a convolved frequency spectrum and variance (FFTW); Requires KTConvolvedFrequencySpectrumDataFFTW and KTConvolvedFrequencySpectrumVarianceDataFFTW; Adds KTGainVariationData
     - "conv-ps-var": void (Nymph::KTDataPtr) -- Calculates gain variation on a convolved power spectrum and variance; Requires KTConvolvedPowerSpectrumData and KTConvolvedPowerSpectrumVarianceData; Adds KTGainVariationData

     These slots have been temporarily removed because the variance is not calculated correctly here (in CoreVarianceCalc)
     See GitHub issue #159
     - "fs-polar": void (Nymph::KTDataPtr) -- Calculates gain variation on a frequency spectrum (Polar); Requires KTFrequencySpectrumDataPolar; Adds KTGainVariationData
     - "fs-fftw": void (Nymph::KTDataPtr) -- Calculates gain variation on a frequency spectrum (FFTW); Requires KTFrequencySpectrumDataFFTW; Adds KTGainVariationData
     - "corr": void (Nymph::KTDataPtr) -- Calculates gain variation on a correlation spectrum; Requires KTCorrlationData; Adds KTGainVariationData
     - "ps": void (Nymph::KTDataPtr) -- Calculates gain variation on a power spectrum; Requires KTPowerSpectrumData; Adds KTGainVariationData
     - "conv-fs-polar": void (Nymph::KTDataPtr) -- Calculates gain variation on a convolved frequency spectrum (Polar); Requires KTConvolvedFrequnecySpectrumDataPolar; Adds KTGainVariationData
     - "conv-fs-fftw": void (Nymph::KTDataPtr) -- Calculates gain variation on a convolved frequency spectrum (FFTW); Requires KTConvolvedFrequencySpectrumDataFFTW; Adds KTGainVariationData
     - "conv-ps": void (Nymph::KTDataPtr) -- Calculates gain variation on a convolved power spectrum; Requires KTConvolvedPowerSpectrumData; Adds KTGainVariationData

     Signals:
     - "gain-var": void (Nymph::KTDataPtr) emitted upon performance of a fit; Guarantees KTGainVariationData
    */

    class KTGainVariationProcessor : public Nymph::KTProcessor
    {
        public:
            KTGainVariationProcessor(const std::string& name = "gain-variation");
            virtual ~KTGainVariationProcessor();

            bool Configure(const scarab::param_node* node);

            bool GetNormalize() const;
            void SetNormalize(bool flag);

            double GetMinFrequency() const;
            void SetMinFrequency(double freq);

            double GetMaxFrequency() const;
            void SetMaxFrequency(double freq);

            unsigned GetMinBin() const;
            void SetMinBin(unsigned bin);

            unsigned GetMaxBin() const;
            void SetMaxBin(unsigned bin);

            unsigned GetNFitPoints() const;
            void SetNFitPoints(unsigned nPoints);

            unsigned GetVarianceCalcNBins() const;
            void SetVarianceCalcNBins(unsigned nBins);

        private:
            bool fNormalize;
            double fMinFrequency;
            double fMaxFrequency;
            unsigned fMinBin;
            unsigned fMaxBin;
            unsigned fNFitPoints;
            bool fCalculateMinBin;
            bool fCalculateMaxBin;
            unsigned fVarianceCalcNBins;

        public:
            // These functions have been removed because the variance is not calculated correctly here (in CoreVarianceCalc)
            // See GitHub issue #159
            //bool CalculateGainVariation(KTFrequencySpectrumDataPolar& data);
            //bool CalculateGainVariation(KTFrequencySpectrumDataFFTW& data);
            //bool CalculateGainVariation(KTCorrelationData& data);
            //bool CalculateGainVariation(KTPowerSpectrumData& data);
            //bool CalculateGainVariation(KTConvolvedFrequencySpectrumDataPolar& data);
            //bool CalculateGainVariation(KTConvolvedFrequencySpectrumDataFFTW& data);
            //bool CalculateGainVariation(KTConvolvedPowerSpectrumData& data);

            bool CalculateGainVariation(KTFrequencySpectrumDataPolar& data, KTFrequencySpectrumVarianceDataPolar& varData);
            bool CalculateGainVariation(KTFrequencySpectrumDataFFTW& data, KTFrequencySpectrumVarianceDataFFTW& varData);
            bool CalculateGainVariation(KTCorrelationData& data, KTFrequencySpectrumVarianceDataPolar& varData); // there's currently no variance data type for KTCorrelationData
            bool CalculateGainVariation(KTPowerSpectrumData& data, KTPowerSpectrumVarianceData& varData);
            bool CalculateGainVariation(KTConvolvedFrequencySpectrumDataPolar& data, KTConvolvedFrequencySpectrumVarianceDataPolar& varData);
            bool CalculateGainVariation(KTConvolvedFrequencySpectrumDataFFTW& data, KTConvolvedFrequencySpectrumVarianceDataFFTW& varData);
            bool CalculateGainVariation(KTConvolvedPowerSpectrumData& data, KTConvolvedPowerSpectrumVarianceData& varData);

        private:
            bool CoreGainVarCalc(KTFrequencySpectrumDataPolarCore& data, KTGainVariationData& newData);
            bool CoreGainVarCalc(KTFrequencySpectrumDataFFTWCore& data, KTGainVariationData& newData);
            bool CoreGainVarCalc(KTPowerSpectrumDataCore& data, KTGainVariationData& newData);
            bool CoreGainVarCalc(KTFrequencySpectrumVarianceDataCore& data, KTGainVariationData& newData);

            // These functions have been removed because the variance is not calculated correctly here
            // See GitHub issue #159
            //bool CoreVarianceCalc(KTFrequencySpectrumDataPolarCore& data, KTFrequencySpectrumVarianceDataCore& newVarData);
            //bool CoreVarianceCalc(KTFrequencySpectrumDataFFTWCore& data, KTFrequencySpectrumVarianceDataCore& newVarData);
            //bool CoreVarianceCalc(KTPowerSpectrumDataCore& data, KTFrequencySpectrumVarianceDataCore& newVarData);

            //double CalculateVariance(const std::list< double >& binValuesInUse, double runningSum, double invNBinsInUse);
            //double CalculateVariance(const std::list< std::complex<double> >& binValuesInUse, std::complex<double> runningSum, double invNBinsInUse);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fGainVarSignal;

            //***************
            // Slots
            //***************

        private:
            // These functions have been removed because the variance is not calculated correctly here (in CoreVarianceCalc)
            // See GitHub issue #159
            //Nymph::KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarSlot;
            //Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
            //Nymph::KTSlotDataOneType< KTCorrelationData > fCorrSlot;
            //Nymph::KTSlotDataOneType< KTPowerSpectrumData > fPSSlot;
            //Nymph::KTSlotDataOneType< KTConvolvedFrequencySpectrumDataPolar > fConvFSPolarSlot;
            //Nymph::KTSlotDataOneType< KTConvolvedFrequencySpectrumDataFFTW > fConvFSFFTWSlot;
            //Nymph::KTSlotDataOneType< KTConvolvedPowerSpectrumData > fConvPSSlot;
            
            Nymph::KTSlotDataTwoTypes< KTFrequencySpectrumDataPolar, KTFrequencySpectrumVarianceDataPolar > fFSPolarWithVarSlot;
            Nymph::KTSlotDataTwoTypes< KTFrequencySpectrumDataFFTW, KTFrequencySpectrumVarianceDataFFTW > fFSFFTWWithVarSlot;
            Nymph::KTSlotDataTwoTypes< KTPowerSpectrumData, KTPowerSpectrumVarianceData > fPSWithVarSlot;
            Nymph::KTSlotDataTwoTypes< KTConvolvedFrequencySpectrumDataPolar, KTConvolvedFrequencySpectrumVarianceDataPolar > fConvFSPolarWithVarSlot;
            Nymph::KTSlotDataTwoTypes< KTConvolvedFrequencySpectrumDataFFTW, KTConvolvedFrequencySpectrumVarianceDataFFTW > fConvFSFFTWWithVarSlot;
            Nymph::KTSlotDataTwoTypes< KTConvolvedPowerSpectrumData, KTConvolvedPowerSpectrumVarianceData > fConvPSWithVarSlot;

    };

    inline bool KTGainVariationProcessor::GetNormalize() const
    {
        return fNormalize;
    }

    inline void KTGainVariationProcessor::SetNormalize(bool flag)
    {
        fNormalize = flag;
        return;
    }

    inline double KTGainVariationProcessor::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTGainVariationProcessor::SetMinFrequency(double freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline double KTGainVariationProcessor::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTGainVariationProcessor::SetMaxFrequency(double freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline unsigned KTGainVariationProcessor::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTGainVariationProcessor::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline unsigned KTGainVariationProcessor::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTGainVariationProcessor::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }

    inline unsigned KTGainVariationProcessor::GetNFitPoints() const
    {
        return fNFitPoints;
    }

    inline void KTGainVariationProcessor::SetNFitPoints(unsigned nPoints)
    {
        fNFitPoints = nPoints;
    }

    inline unsigned KTGainVariationProcessor::GetVarianceCalcNBins() const
    {
        return fVarianceCalcNBins;
    }

    inline void KTGainVariationProcessor::SetVarianceCalcNBins(unsigned nBins)
    {
        fVarianceCalcNBins = nBins;
    }

} /* namespace Katydid */
#endif /* KTGAINVARIATIONPROCESSOR_HH_ */
