/**
 @file KTAnalyticAssociator.hh
 @brief Contains KTAnalyticAssociator
 @details Creates an analytic associate of a time series
 @author: N. S. Oblath
 @date: Dec 17, 2012
 */

#ifndef KTANALYTICASSOCIATOR_HH_
#define KTANALYTICASSOCIATOR_HH_

#include "KTProcessor.hh"
#include "KTTimeSeriesData.hh"

#include "KTComplexFFTW.hh"
#include "KTSlot.hh"


namespace Katydid
{
    class KTEggHeader;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTNormalizedFSDataFFTW;
    class KTTimeSeriesFFTW;

    /*!
     @class KTAnalyticAssociator
     @author N. S. Oblath

     @brief Creates an analytic associate of a time series

     @details

     Configuration name: "analytic-associator"
 
     Available configuration values:
     - "save-frequency-spectrum": bool -- Option to save the intermediate frequency spectrum that is calculated while creating the analytic associate
     - "complex-fftw": nested config: -- See KTComplexFFTW

     Slots:
     - "header": void (KTDataPtr) -- Initializes the FFT; Requires KTEggHeader
     - "ts": void (KTDataPtr) -- Calculates an analytic associate of the time series; Requires KTTimeSeriesData; Adds KTAnalyticAssociateData; Optionally adds KTFrequencySpectrumDataFFTW
     - "fs-fftw": void (KTDataPtr) -- Calculates an analytic associate of the frequency spectrum; Requires KTFrequencySpectrumDataFFTW; Adds KTAnalyticAssociateData
     - "norm-fs-fftw": void (KTDataPtr) -- Calculates an analytic associate of the frequency spectrum; Requires KTNormalizedFSDataFFTW; Adds KTAnalyticAssociateData

     Signals:
     - "aa": void (KTDataPtr) -- Emitted upon creation of an analytic associate; Guarantees KTAnalyticAssociateData
    */
    class KTAnalyticAssociator : public KTProcessor
    {
        public:
            KTAnalyticAssociator(const std::string& name = "analytic-associator");
            virtual ~KTAnalyticAssociator();

            bool Configure(const KTParamNode* node);

            bool InitializeWithHeader(KTEggHeader& header);

            KTComplexFFTW* GetFullFFT();

            bool GetSaveFrequencySpectrum() const;
            void SetSaveFrequencySpectrum(bool flag);

        private:
            KTComplexFFTW fFullFFT;

            bool fSaveFrequencySpectrum;

        public:
            bool CreateAssociateData(KTTimeSeriesData& tsData);
            bool CreateAssociateData(KTFrequencySpectrumDataFFTW& fsData);
            bool CreateAssociateData(KTNormalizedFSDataFFTW& fsData);

           /// Calculates the AA and returns the new time series; the intermediate FS is assigned to the given output pointer.
            KTTimeSeriesFFTW* CalculateAnalyticAssociate(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS=NULL);
            KTTimeSeriesFFTW* CalculateAnalyticAssociate(const KTFrequencySpectrumFFTW* inputFS);

            //***************
            // Signals
            //***************

         private:
             KTSignalData fAASignal;

             //***************
             // Slots
             //***************

         private:
             KTSlotDataOneType< KTEggHeader > fHeaderSlot;
             KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;
             KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
             KTSlotDataOneType< KTNormalizedFSDataFFTW > fNormFSFFTWSlot;

    };

    inline KTComplexFFTW* KTAnalyticAssociator::GetFullFFT()
    {
        return &fFullFFT;
    }

    inline bool KTAnalyticAssociator::GetSaveFrequencySpectrum() const
    {
        return fSaveFrequencySpectrum;
    }

    inline void KTAnalyticAssociator::SetSaveFrequencySpectrum(bool flag)
    {
        fSaveFrequencySpectrum = flag;
        return;
    }

} /* namespace Katydid */
#endif /* KTANALYTICASSOCIATOR_HH_ */
