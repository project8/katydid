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

#include "KTForwardFFTW.hh"
#include "KTReverseFFTW.hh"
#include "KTSlot.hh"


namespace Katydid
{
    
    class KTEggHeader;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTNormalizedFSDataFFTW;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesReal;

    /*!
     @class KTAnalyticAssociator
     @author N. S. Oblath

     @brief Creates an analytic associate of a real time series

     @details
     Equivalent to application of a Hilbert Transform.

     Removes negative frequency components and results in a complex time series.

     Configuration name: "analytic-associator"
 
     Available configuration values:
     - "save-frequency-spectrum": bool -- Option to save the intermediate frequency spectrum that is calculated while creating the analytic associate
     - "forward-fftw": nested config: -- See KTForwardFFTW
     - "reverse-fftw": nested config: -- See KTReverseFFTW

     Slots:
     - "header": void (Nymph::KTDataPtr) -- Initializes the FFT; Requires KTEggHeader
     - "ts": void (Nymph::KTDataPtr) -- Calculates an analytic associate of the real time series; Requires KTTimeSeriesData; Adds KTAnalyticAssociateData; Optionally adds KTFrequencySpectrumDataFFTW
     - "fs-fftw": void (Nymph::KTDataPtr) -- Calculates an analytic associate of the frequency spectrum; Requires KTFrequencySpectrumDataFFTW; Adds KTAnalyticAssociateData
     - "norm-fs-fftw": void (Nymph::KTDataPtr) -- Calculates an analytic associate of the frequency spectrum; Requires KTNormalizedFSDataFFTW; Adds KTAnalyticAssociateData

     Signals:
     - "aa": void (Nymph::KTDataPtr) -- Emitted upon creation of an analytic associate; Guarantees KTAnalyticAssociateData
    */
    class KTAnalyticAssociator : public Nymph::KTProcessor
    {
        public:
            KTAnalyticAssociator(const std::string& name = "analytic-associator");
            virtual ~KTAnalyticAssociator();

            bool Configure(const scarab::param_node* node);

            bool InitializeWithHeader(KTEggHeader& header);

            KTForwardFFTW* GetForwardFFT();
            KTReverseFFTW* GetReverseFFT();

            bool GetSaveFrequencySpectrum() const;
            void SetSaveFrequencySpectrum(bool flag);

        private:
            KTForwardFFTW fForwardFFT;
            KTReverseFFTW fReverseFFT;

            bool fSaveFrequencySpectrum;

        public:
            bool CreateAssociateData(KTTimeSeriesData& tsData);
            bool CreateAssociateData(KTFrequencySpectrumDataFFTW& fsData);
            bool CreateAssociateData(KTNormalizedFSDataFFTW& fsData);

            /// Calculates the AA and returns the new time series; the intermediate FS is assigned to the given output pointer.
            KTTimeSeriesFFTW* CalculateAnalyticAssociate(const KTTimeSeriesReal* inputTS, KTFrequencySpectrumFFTW** outputFS=NULL);
            KTTimeSeriesFFTW* CalculateAnalyticAssociate(const KTFrequencySpectrumFFTW* inputFS);

        private:
            bool CheckAndDoFFTInit();

            //***************
            // Signals
            //***************

         private:
             Nymph::KTSignalData fAASignal;

             //***************
             // Slots
             //***************

         private:
             Nymph::KTSlotDataOneType< KTEggHeader > fHeaderSlot;
             Nymph::KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;
             Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
             Nymph::KTSlotDataOneType< KTNormalizedFSDataFFTW > fNormFSFFTWSlot;

    };

    inline KTForwardFFTW* KTAnalyticAssociator::GetForwardFFT()
    {
        return &fForwardFFT;
    }

    inline KTReverseFFTW* KTAnalyticAssociator::GetReverseFFT()
    {
        return &fReverseFFT;
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
