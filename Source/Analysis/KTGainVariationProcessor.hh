/*
 * KTGainVariationProcessor.hh
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#ifndef KTGAINVARIATIONPROCESSOR_HH_
#define KTGAINVARIATIONPROCESSOR_HH_

#include "KTProcessor.hh"

#include "KTPhysicalArray.hh"

#include <boost/shared_ptr.hpp>

class TSpline;

namespace Katydid
{
    class KTEvent;
    class KTFrequencySpectrumData;
    class KTFrequencySpectrumDataFFTW;
    class KTGainVariationData;
    class KTPStoreNode;

    /*!
     @class KTGainVariationProcessor
     @author N. S. Oblath

     @brief Fit the gain variation

     @details
     Fit the gain variation to a parabola using linear regression.
     The fit is performed between fMinBin and fMaxBin, inclusive.  If the [min,max] range has been set by frequency, those frequencies are turned into bins the first time they're used.
     The x-axis of the fit space is bin number, not frequency.

     Available configuration values:
     \li \c "min-frequency": double -- minimum frequency for the fit
     \li \c "max-frequency": double -- maximum frequency for the fit
     \li \c "min-bin": unsigned -- minimum bin for the fit
     \li \c "max-bin": unsigned -- maximum bin for the fit
     \li \c "input-data-name": string -- name of the data to find when processing an event
     \li \c "output-data-name": string -- name to give to the data produced

     Slots:
     \li \c void ProcessEvent(boost::shared_ptr<KTEvent>)
     \li \c void ProcessFrequencySpectrumData(const KTFrequencySpectrumData*)
     \li \c void ProcessFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW*)

     Signals:
     \li \c void (const KTGainVariationProcessorData*) emitted upon performance of a fit.
    */

    class KTGainVariationProcessor : public KTProcessor
    {
        public:
            typedef KTSignal< void (const KTGainVariationData*) >::signal GainVarSignal;

            typedef KTPhysicalArray< 1, Double_t > GainVariation;

        public:
            KTGainVariationProcessor();
            virtual ~KTGainVariationProcessor();

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetMinFrequency() const;
            void SetMinFrequency(Double_t freq);

            Double_t GetMaxFrequency() const;
            void SetMaxFrequency(Double_t freq);

            UInt_t GetMinBin() const;
            void SetMinBin(UInt_t bin);

            UInt_t GetMaxBin() const;
            void SetMaxBin(UInt_t bin);

            UInt_t GetNFitPoints() const;
            void SetNFitPoints(UInt_t nPoints);

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        private:
            Double_t fMinFrequency;
            Double_t fMaxFrequency;
            UInt_t fMinBin;
            UInt_t fMaxBin;
            UInt_t fNFitPoints;
            Bool_t fCalculateMinBin;
            Bool_t fCalculateMaxBin;

            std::string fInputDataName;
            std::string fOutputDataName;

        public:
            KTGainVariationData* CalculateGainVariation(const KTFrequencySpectrumData* data);
            KTGainVariationData* CalculateGainVariation(const KTFrequencySpectrumDataFFTW* data);

        private:
            GainVariation* CreateGainVariation(TSpline* spline, UInt_t nBins, Double_t rangeMin, Double_t rangeMax) const;

            //***************
            // Signals
            //***************

        private:
            GainVarSignal fGainVarSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessEvent(boost::shared_ptr<KTEvent> event);
            void ProcessFrequencySpectrumData(const KTFrequencySpectrumData* data);
            void ProcessFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* data);

    };

    inline Double_t KTGainVariationProcessor::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTGainVariationProcessor::SetMinFrequency(Double_t freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline Double_t KTGainVariationProcessor::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTGainVariationProcessor::SetMaxFrequency(Double_t freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline UInt_t KTGainVariationProcessor::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTGainVariationProcessor::SetMinBin(UInt_t bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline UInt_t KTGainVariationProcessor::KTGainVariationProcessor::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTGainVariationProcessor::SetMaxBin(UInt_t bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }

    inline UInt_t KTGainVariationProcessor::GetNFitPoints() const
    {
        return fNFitPoints;
    }

    inline void KTGainVariationProcessor::SetNFitPoints(UInt_t nPoints)
    {
        fNFitPoints = nPoints;
    }

    inline const std::string& KTGainVariationProcessor::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTGainVariationProcessor::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    inline const std::string& KTGainVariationProcessor::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTGainVariationProcessor::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }


} /* namespace Katydid */
#endif /* KTGAINVARIATIONPROCESSOR_HH_ */
