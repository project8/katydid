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
     \li \c "aa-fs-output-data-name": string -- If saving the intermediate frequency spectrum, the will be the name given to the FS data.
     \li \c "input-data-name": string -- name of the data to find when processing an event
     \li \c "output-data-name": string -- name given to the analytic associate data

     Slots:
     \li \c "header": void ProcessHeader(const KTEggHeader*)
     \li \c "event": void ProcessEvent(boost::shared_ptr<KTEvent>)
     \li \c "ts-data": void ProcessTimeSeriesData(const KTTimeSeriesData*)

     Signals:
     \li \c "analytic-associate": void (const KTFrequencySpectrumData*) emitted upon creation of an analytic associate
    */
    class KTAnalyticAssociator : public KTProcessor
    {
        protected:
            typedef KTSignalConcept< void (boost::shared_ptr< KTData >) >::signal AASignal;

        public:
            KTAnalyticAssociator();
            virtual ~KTAnalyticAssociator();

            Bool_t Configure(const KTPStoreNode* node);

            KTComplexFFTW* GetFullFFT();

            Bool_t GetSaveFrequencySpectrum() const;
            void SetSaveFrequencySpectrum(Bool_t flag);

        protected:
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
             AASignal fAASignal;

             //***************
             // Slots
             //***************

         public:
             void ProcessHeader(const KTEggHeader* header);
             void ProcessTimeSeriesData(boost::shared_ptr<KTData> data);
             void ProcessFrequencySpectrumDataFFTW(boost::shared_ptr<KTData> data);

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
