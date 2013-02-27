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

#include <boost/shared_ptr.hpp>


namespace Katydid
{
    class KTEggHeader;
    class KTData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTTimeSeriesFFTW;

    class KTAnalyticAssociateData : public KTTimeSeriesDataCore, public KTExtensibleData< KTAnalyticAssociateData >
    {
        public:
            KTAnalyticAssociateData() :
                KTTimeSeriesDataCore(),
                KTExtensibleData< KTAnalyticAssociateData >()
            {}
            virtual ~KTAnalyticAssociateData()
            {}

            virtual KTAnalyticAssociateData& SetNComponents(UInt_t num)
            {
                fTimeSeries.resize(num);
                return *this;
            }
    };


    /*!
     @class KTAnalyticAssociator
     @author N. S. Oblath

     @brief Creates an analytic associate of a time series

     @details
 
     Available configuration values:
     \li \c "save-frequency-spectrum": bool -- Option to save the intermediate frequency spectrum that is calculated while creating the analytic associate

     Slots:
     \li \c "header": void (const KTEggHeader*) -- Initializes the FFT
     \li \c "ts": void (shared_ptr< KTData >) -- Calculates an analytic associate of the time series; Requires KTTimeSeriesData; Adds KTAnalyticAssociateData; Optionally adds KTFrequencySpectrumDataFFTW
     \li \c "fs-fftw": void (shared_ptr< KTData >) -- Calculates an analytic associate of the frequency spectrum; Requires KTFrequencySpectrumDataFFTW; Adds KTAnalyticAssociateData

     Signals:
     \li \c "aa": void (shared_ptr< KTData >) -- Emitted upon creation of an analytic associate; Guarantees KTAnalyticAssociateData
    */
    class KTAnalyticAssociator : public KTProcessor
    {
        public:
            KTAnalyticAssociator(const std::string& name = "analytic-associator");
            virtual ~KTAnalyticAssociator();

            Bool_t Configure(const KTPStoreNode* node);

            void InitializeWithHeader(const KTEggHeader* header);

            KTComplexFFTW* GetFullFFT();

            Bool_t GetSaveFrequencySpectrum() const;
            void SetSaveFrequencySpectrum(Bool_t flag);

        private:
            KTComplexFFTW fFullFFT;

            Bool_t fSaveFrequencySpectrum;

        public:
            Bool_t CreateAssociateData(KTTimeSeriesData& tsData);
            Bool_t CreateAssociateData(KTFrequencySpectrumDataFFTW& fsData);

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
             KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
             KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;
             KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;

    };

    inline KTComplexFFTW* KTAnalyticAssociator::GetFullFFT()
    {
        return &fFullFFT;
    }

    inline Bool_t KTAnalyticAssociator::GetSaveFrequencySpectrum() const
    {
        return fSaveFrequencySpectrum;
    }

    inline void KTAnalyticAssociator::SetSaveFrequencySpectrum(Bool_t flag)
    {
        fSaveFrequencySpectrum = flag;
        return;
    }

} /* namespace Katydid */
#endif /* KTANALYTICASSOCIATOR_HH_ */
