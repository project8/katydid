/**
 @file KTSimpleClustering.hh
 @brief Contains KTSimpleClustering
 @details Simple cluster-finding algorithm that works by looking for lines of high-peaked bins increasing in frequency
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTSIMPLECLUSTERING_HH_
#define KTSIMPLECLUSTERING_HH_

#include "KTProcessor.hh"
#include "KTConfigurable.hh"

#include "KTFrequencySpectrum.hh"
#include "KTMaskedArray.hh"

#include <list>
#include <map>

namespace Katydid
{
    class KTSlidingWindowFSData;
    class KTPStoreNode;
    class KTSlidingWindowFFT;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    class KTSimpleClustering : public KTProcessor, public KTConfigurable
    {
        private:
            typedef std::list< std::multimap< Int_t, Int_t >* > epbList;

        public:
            KTSimpleClustering();
            virtual ~KTSimpleClustering();

            Bool_t Configure(const KTPStoreNode* node);

            void ProcessSlidingWindowFFT(KTSlidingWindowFSData* swFSData);
            void ProcessFrequencySpectrum(UInt_t psNum, KTFrequencySpectrum* powerSpectrum);

            void SetEventPeakBinsList(epbList* eventPeakBinsList); /// does NOT take ownership of eventPeakBinsList
            void SetBinCuts(KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >* binCuts); /// takes ownership of binCuts
            void SetMinimumGroupSize(UInt_t size);

        private:
            epbList* fEventPeakBins;
            Double_t fThresholdMult;

            KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >* fBinCuts;

            UInt_t fMinimumGroupSize;

            Int_t fGroupBinsMarginLow;
            Int_t fGroupBinsMarginHigh;
            Int_t fGroupBinsMarginSameTime;

            UInt_t fFirstBinToUse;

            Bool_t fDrawFlag;

    };

    inline void KTSimpleClustering::SetEventPeakBinsList(epbList* list)
    {
        fEventPeakBins = list;
        return;
    }

    inline void KTSimpleClustering::SetBinCuts(KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >* binCuts)
    {
        delete fBinCuts;
        fBinCuts = binCuts;
        return;
    }

    inline void KTSimpleClustering::SetMinimumGroupSize(UInt_t size)
    {
        fMinimumGroupSize = size;
        return;
    }

} /* namespace Katydid */
#endif /* KTSIMPLECLUSTERING_HH_ */
