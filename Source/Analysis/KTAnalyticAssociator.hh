/*
 * KTAnalyticAssociator.hh
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#ifndef KTANALYTICASSOCIATOR_HH_
#define KTANALYTICASSOCIATOR_HH_

#include "KTProcessor.hh"

#include "KTComplexFFTW.hh"

#include <boost/shared_ptr.hpp>


namespace Katydid
{
    class KTEggHeader;
    class KTBundle;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;

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
            KTTimeSeriesData* CreateAssociateData(const KTTimeSeriesData* data, KTFrequencySpectrumDataFFTW** outputFSData=NULL);
            KTTimeSeriesData* CreateAssociateData(const KTFrequencySpectrumDataFFTW* inputFSData);

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
             void ProcessEvent(boost::shared_ptr<KTBundle> bundle);
             void ProcessTimeSeriesData(const KTTimeSeriesData* tsData);
             void ProcessFrequencySpectrumData(const KTFrequencySpectrumDataFFTW* fsData);

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
