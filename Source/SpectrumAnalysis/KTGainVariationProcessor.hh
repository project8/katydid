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

namespace Katydid
{
    using namespace Nymph;
    class KTCorrelationData;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTGainVariationData;
    class KTPowerSpectrumData;
    //class KTSpline;

    /*!
     @class KTGainVariationProcessor
     @author N. S. Oblath

     @brief Fit the gain variation

     @details
     Fit the gain variation to a parabola using linear regression.
     The fit is performed between fMinBin and fMaxBin, inclusive.  If the [min,max] range has been set by frequency, those frequencies are turned into bins the first time they're used.

     Configuration name: "gain-variation"

     Available configuration values:
     - "normalize": bool -- whether or not to normalize the output to the minimum value (default: true)
     - "min-frequency": double -- minimum frequency for the fit
     - "max-frequency": double -- maximum frequency for the fit
     - "min-bin": unsigned -- minimum bin for the fit
     - "max-bin": unsigned -- maximum bin for the fit

     Slots:
     - "fs-polar": void (KTDataPtr) -- Calculates gain variation on a polar fs data object; Requires KTFrequencySpectrumDataPolar; Adds KTGainVariationData
     - "fs-fftw": void (KTDataPtr) -- Calculates gain variation on a fftw fs data object; Requires KTFrequencySpectrumDataFFTW; Adds KTGainVariationData
     - "corr": void (KTDataPtr) -- Calculates gain variation on a corrlation data object; Requires KTCorrlationData; Adds KTGainVariationData
     - "ps": void (KTDataPtr) -- Calculates gain variation on a power power spectrum; Requires KTPowerSpectrumData; Adds KTGainVariationData

     Signals:
     - "gain-var": void (KTDataPtr) emitted upon performance of a fit; Guarantees KTGainVariationData
    */

    class KTGainVariationProcessor : public KTProcessor
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

        private:
            bool fNormalize;
            double fMinFrequency;
            double fMaxFrequency;
            unsigned fMinBin;
            unsigned fMaxBin;
            unsigned fNFitPoints;
            bool fCalculateMinBin;
            bool fCalculateMaxBin;

        public:
            bool CalculateGainVariation(KTFrequencySpectrumDataPolar& data);
            bool CalculateGainVariation(KTFrequencySpectrumDataFFTW& data);
            bool CalculateGainVariation(KTCorrelationData& data);
            bool CalculateGainVariation(KTPowerSpectrumData& data);

        private:
            bool CoreGainVarCalc(KTFrequencySpectrumDataPolarCore& data, KTGainVariationData& newData);
            bool CoreGainVarCalc(KTFrequencySpectrumDataFFTWCore& data, KTGainVariationData& newData);
            //GainVariation* CreateGainVariation(KTSpline* spline, unsigned nBins, double rangeMin, double rangeMax) const;
            bool CoreGainVarCalc(KTPowerSpectrumData& data, KTGainVariationData& newData);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fGainVarSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarSlot;
            KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
            KTSlotDataOneType< KTCorrelationData > fCorrSlot;
            KTSlotDataOneType< KTPowerSpectrumData > fPSSlot;

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

} /* namespace Katydid */
#endif /* KTGAINVARIATIONPROCESSOR_HH_ */
