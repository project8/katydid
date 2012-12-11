/*
 * KTGainVariation.hh
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#ifndef KTGAINVARIATION_HH_
#define KTGAINVARIATION_HH_

#include "KTProcessor.hh"


#include <boost/shared_ptr.hpp>


namespace Katydid
{
    class KTEvent;
    class KTFrequencySpectrumData;
    class KTFrequencySpectrumDataFFTW;
    class KTGainVariationData;
    class KTPStoreNode;

    /*!
     @class KTGainVariation
     @author N. S. Oblath

     @brief Fit the gain variation

     @details
     Fit the gain variation to a parabola using linear regression.
     The fit is performed between fMinBin and fMaxBin, inclusive.  If the [min,max] range has been set by frequency, those frequencies are turned into bins the first time they're used.

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
     \li \c void (const KTGainVariationData*) emitted upon performance of a fit.
    */

    class KTGainVariation : public KTProcessor
    {
        public:
            typedef KTSignal< void (const KTGainVariationData*) >::signal GainVarSignal;

        private:
            struct FitPoint
            {
                Double_t fX;
                Double_t fY;
                Double_t fSigma;
            };

            struct FitResult
            {
                // y = fA * x^2 + fB * x + fC
                Double_t fA;
                Double_t fB;
                Double_t fC;
            };

        public:
            KTGainVariation();
            virtual ~KTGainVariation();

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
            KTGainVariationData* PerformFit(const KTFrequencySpectrumData* data);
            KTGainVariationData* PerformFit(const KTFrequencySpectrumDataFFTW* data);

        private:
            FitResult DoFit(const std::vector< FitPoint >& fitPoints);

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

    Double_t KTGainVariation::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    void KTGainVariation::SetMinFrequency(Double_t freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    Double_t KTGainVariation::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    void KTGainVariation::SetMaxFrequency(Double_t freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    UInt_t KTGainVariation::GetMinBin() const
    {
        return fMinBin;
    }

    void KTGainVariation::SetMinBin(UInt_t bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    UInt_t KTGainVariation::KTGainVariation::GetMaxBin() const
    {
        return fMaxBin;
    }

    void KTGainVariation::SetMaxBin(UInt_t bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }

    UInt_t KTGainVariation::GetNFitPoints() const
    {
        return fNFitPoints;
    }

    void KTGainVariation::SetNFitPoints(UInt_t nPoints)
    {
        fNFitPoints = nPoints;
    }

    const std::string& KTGainVariation::GetInputDataName() const
    {
        return fInputDataName;
    }

    void KTGainVariation::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    const std::string& KTGainVariation::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    void KTGainVariation::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }


} /* namespace Katydid */
#endif /* KTGAINVARIATION_HH_ */
