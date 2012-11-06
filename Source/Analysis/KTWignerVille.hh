/*
 * KTWignerVille.hh
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#ifndef KTWIGNERVILLE_HH_
#define KTWIGNERVILLE_HH_

#include "KTProcessor.hh"

namespace Katydid
{
    class KTComplexFFTW;
    class KTEvent;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTSlidingWindowFFTW;
    class KTSlidingWindowFSDataFFTW;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;
    class KTWriteableData;

    typedef std::pair< UInt_t, UInt_t > KTWVPair;

    class KTWignerVille : public KTProcessor
    {
        protected:
            typedef KTSignal< void (const KTWriteableData*) >::signal WVSignal;
            typedef std::vector< KTWVPair > PairVector;

        public:
            KTWignerVille();
            virtual ~KTWignerVille();

            Bool_t Configure(const KTPStoreNode* node);

            Bool_t GetSaveAAFrequencySpectrum() const;
            Bool_t GetSaveAnalyticAssociate() const;
            Bool_t GetSaveCrossMultipliedTimeSeries() const;

            void SetSaveAAFrequencySpectrum(Bool_t flag);
            void SetSaveAnalyticAssociate(Bool_t flag);
            void SetSaveCrossMultipliedTimeSeries(Bool_t flag);

            void AddPair(const KTWVPair& pair);
            void SetPairVector(const PairVector& pairs);
            const PairVector& GetPairVector() const;
            void ClearPairs();

        protected:
            PairVector fPairs;


        protected:
            KTComplexFFTW* fFullFFT;
            KTSlidingWindowFFTW* fWindowedFFT;

            Bool_t fSaveAAFrequencySpectrum;
            Bool_t fSaveAnalyticAssociate;
            Bool_t fSaveCrossMultipliedTimeSeries;

        public:
            /// Performs the W-V transform on the given time series data.
            /// In the process, the data is FFTed, and then reverse FFTed; if you want to keep the intermediate frequency spectrum, pass a KTFrequencySpectrumDataFFTW** as the second parameter..
            /// @note A frequency spectrum data object can still be returned even if the full W-V transform fails!
            KTSlidingWindowFSDataFFTW* TransformData(const KTTimeSeriesData* data, KTFrequencySpectrumDataFFTW** outputFSData=NULL, KTTimeSeriesData** outputAAData=NULL, KTTimeSeriesData** outputCMTSData=NULL);

            /// Performs the W-V transform on the given time series.
            /// In the process, the data is FFTed, and then reverse FFTed. If you want to keep the intermediate frequency spectrum, pass a KTFrequencySpectrumFFTW** as the second parameter.
            /// @note A frequency spectrum object can still be returned even if the full W-V transform fails.
            //KTTimeSeriesFFTW* Transform(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS=NULL);

            /// Performs the W-V transform on the given frequency spectrum (in place! does NOT create a new FS)
            //Bool_t Transform(KTFrequencySpectrumFFTW* freqSpectrum);

        private:
            /// Calculates the AA and returns the new time series; the intermediate FS is assigned to the given output pointer.
            KTTimeSeriesFFTW* CalculateAnalyticAssociate(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS=NULL);
            /// Calculates the AA in place.
            Bool_t CalculateAnalyticAssociate(KTFrequencySpectrumFFTW* freqSpectrum);

            KTTimeSeriesFFTW* CrossMultiply(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2);


            //***************
             // Signals
             //***************

         private:
             WVSignal fWVSignal;

             //***************
             // Slots
             //***************

         public:
             //void ProcessHeader(const KTEggHeader* header);
             void ProcessEvent(KTEvent* event, const std::string& dataName="");
             //void ProcessTimeSeriesData(const KTTimeSeriesData* tsData);
             //void ProcessFrequencySpectrumData(const KTFrequencySpectrumDataFFTW* fsData);

    };

    inline Bool_t KTWignerVille::GetSaveAAFrequencySpectrum() const
    {
        return fSaveAAFrequencySpectrum;
    }

    inline void KTWignerVille::SetSaveAAFrequencySpectrum(Bool_t flag)
    {
        fSaveAAFrequencySpectrum = flag;
        return;
    }

    inline Bool_t KTWignerVille::GetSaveAnalyticAssociate() const
    {
        return fSaveAnalyticAssociate;
    }

    inline void KTWignerVille::SetSaveAnalyticAssociate(Bool_t flag)
    {
        fSaveAnalyticAssociate = flag;
        return;
    }

    inline Bool_t KTWignerVille::GetSaveCrossMultipliedTimeSeries() const
    {
        return fSaveCrossMultipliedTimeSeries;
    }

    inline void KTWignerVille::SetSaveCrossMultipliedTimeSeries(Bool_t flag)
    {
        fSaveCrossMultipliedTimeSeries = flag;
        return;
    }

    inline void KTWignerVille::AddPair(const KTWVPair& pair)
    {
        fPairs.push_back(pair);
        return;
    }

    inline void KTWignerVille::SetPairVector(const PairVector& pairs)
    {
        fPairs = pairs;
        return;
    }

    inline const KTWignerVille::PairVector& KTWignerVille::GetPairVector() const
    {
        return fPairs;
    }

    inline void KTWignerVille::ClearPairs()
    {
        fPairs.clear();
        return;
    }


} /* namespace Katydid */
#endif /* KTWIGNERVILLE_HH_ */
