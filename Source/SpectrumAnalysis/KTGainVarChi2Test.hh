/**
 @file KTGainVarChi2Test.hh
 @brief Contains KTVGainVarChi2Test
 @details Performs chi-squared analysis on a power spectrum and a corresponding spline implementation
 @author: E. Zayas
 @date: Oct 8, 2015
 */

#ifndef KTGainVarChi2Test_HH_
#define KTGainVarChi2Test_HH_

#include "KTGainVariationData.hh"
#include "KTProcessor.hh"

#include "KTSlot.hh"

#include <vector>


namespace Katydid
{
    using namespace Nymph;
    class KTPowerSpectrum;
    class KTPowerSpectrumData;
    class KTSpline;

    /*!
     @class KTGainVarChi2Test
     @author E. Zayas

     @brief Performs a chi-squared test on the spline fit to an accumulated power spectrum

     @details Performs a chi-squared test on the spline fit to an accumulated power spectrum. Returns chi-squared value and NDF

     Available configuration values:
     - MinFrequency: minimum frequency bound for chi-squared calculation
     - MaxFrequency: maximum frequency bound for chi-squared calculation
     - MinBin: bin associated with minimum frequency bound
     - MaxBin: bin associated with maximum frequency bound

     Slots:
     - "ps": void (KTDataPtr) -- Requires KTPowerSpectrumData, KTPowerSpectrumUncertaintyData and KTGainVariationData; Adds KTGainVarChi2Data
     
     Signals:
     - "chi2-test": void (KTDataPtr) Emitted upon chi-squared and NDF calculation; Guarantees KTGainVarChi2Data
    */
     
    class KTGainVarChi2Test : public KTProcessor
    {

        public:
            KTGainVarChi2Test(const std::string& name = "variable-spectrum-chi2test");
            virtual ~KTGainVarChi2Test();

            bool Configure(const KTParamNode* node);

            double GetMinFrequency() const;
            void SetMinFrequency(double freq);

            double GetMaxFrequency() const;
            void SetMaxFrequency(double freq);

            unsigned GetMinBin() const;
            void SetMinBin(unsigned bin);

            unsigned GetMaxBin() const;
            void SetMaxBin(unsigned bin);

        private:
            double fMinFrequency;
            double fMaxFrequency;
            unsigned fMinBin;
            unsigned fMaxBin;
            bool fCalculateMinBin;
            bool fCalculateMaxBin;

        public:
            
            bool Calculate(KTPowerSpectrumData& data, KTPowerSpectrumUncertaintyData& sigma, KTGainVariationData& gvData);

            bool CalculateSpectrum(const KTPowerSpectrum* spectrum, const KTPowerSpectrum* sigma, const KTSpline* spline, KTDiscriminatedPoints1DData& newData, unsigned component=0);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fChi2TestSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataThreeTypes< KTPowerSpectrumData, KTPowerSpectrumUncertaintyData, KTGainVariationData > fPSSlot;

    };

    inline double KTVariableSpectrumDiscriminator::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTVariableSpectrumDiscriminator::SetMinFrequency(double freq)
    {
        fMinFrequency = freq;
        return;
    }

    inline double KTVariableSpectrumDiscriminator::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTVariableSpectrumDiscriminator::SetMaxFrequency(double freq)
    {
        fMaxFrequency = freq;
        return;
    }

    inline unsigned KTVariableSpectrumDiscriminator::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTVariableSpectrumDiscriminator::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline unsigned KTVariableSpectrumDiscriminator::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTVariableSpectrumDiscriminator::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }


} /* namespace Katydid */

#endif /* KTVARIABLESPECTRUMDISCRIMINATOR_HH_ */