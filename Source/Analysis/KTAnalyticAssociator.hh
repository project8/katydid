/*
 * KTAnalyticAssociator.hh
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */
/**
 @file KTAnalyticAssociator.hh
 @brief Contains KTAnalyticAssociator
 @details (?)
 @author: N. S. Oblath
 @date: Dec 17, 2012
 */

#ifndef KTANALYTICASSOCIATOR_HH_
#define KTANALYTICASSOCIATOR_HH_

#include "KTProcessor.hh"

#include "KTComplexFFTW.hh"

#include <boost/shared_ptr.hpp>


namespace Katydid
{
    class KTEggHeader;
    class KTEvent;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;

    /*!
     @class KTAnalyticAssociator
     @author N. S. Oblath

     @brief A one-dimensional real-to-complex FFT class.

     @details
 
     Available configuration values:
     \li \c "save-frequency-spectrum": bool -- 
     \li \c "aa-ts-output-data-name": string - -
     \li \c "input-data-name": string -- name of the data to find when processing an event
     \li \c "output-data-name": string -- name to give to the data produced by an analyticassociator

     Slots:
     \li \c "header": void ProcessHeader(const KTEggHeader*)
     \li \c "event": void ProcessEvent(boost::shared_ptr<KTEvent>)
     \li \c "ts-data": void ProcessTimeSeriesData(const KTTimeSeriesData*)

     Signals:
     \li \c void (const KTFrequencySpectrumData*) emitted upon performance of a transform.
    */

    class KTAnalyticAssociator : public KTProcessor
    {
        protected:
            typedef KTSignal< void (const KTTimeSeriesData*) >::signal AASignal;

        public:
            KTAnalyticAssociator();
            virtual ~KTAnalyticAssociator();

            Bool_t Configure(const KTPStoreNode* node);

            KTComplexFFTW* GetFullFFT();

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

            Bool_t GetSaveFrequencySpectrum() const;
            void SetSaveFrequencySpectrum(Bool_t flag);

            const std::string& GetFSOutputDataName() const;
            void SetFSOutputDataName(const std::string& name);

        protected:
            KTComplexFFTW fFullFFT;

            std::string fInputDataName;
            std::string fOutputDataName;

            Bool_t fSaveFrequencySpectrum;
            std::string fFSOutputDataName;


        public:
            /// Performs the W-V transform on the given time series data.
            /// In the process, the data is FFTed, and then reverse FFTed; if you want to keep the intermediate frequency spectrum, pass a KTFrequencySpectrumDataFFTW** as the second parameter..
            /// @note A frequency spectrum data object can still be returned even if the full W-V transform fails!
            KTTimeSeriesData* CreateAssociateData(const KTTimeSeriesData* data, KTFrequencySpectrumDataFFTW** outputFSData=NULL);

            /// Calculates the AA and returns the new time series; the intermediate FS is assigned to the given output pointer.
            KTTimeSeriesFFTW* CalculateAnalyticAssociate(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS=NULL);

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
             void ProcessEvent(boost::shared_ptr<KTEvent> event);
             void ProcessTimeSeriesData(const KTTimeSeriesData* tsData);

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

    inline const std::string& KTAnalyticAssociator::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTAnalyticAssociator::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    inline const std::string& KTAnalyticAssociator::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTAnalyticAssociator::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }

    inline const std::string& KTAnalyticAssociator::GetFSOutputDataName() const
    {
        return fFSOutputDataName;
    }

    inline void KTAnalyticAssociator::SetFSOutputDataName(const std::string& name)
    {
        fFSOutputDataName = name;
        return;
    }

} /* namespace Katydid */
#endif /* KTANALYTICASSOCIATOR_HH_ */
