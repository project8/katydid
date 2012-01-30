/**
 @file KTSimpleClusteringProcessor.hh
 @brief Contains KTSimpleClusteringProcessor
 @details Simple cluster-finding algorithm that works by looking for lines of high-peaked bins increasing in frequency
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTSIMPLECLUSTERINGPROCESSOR_HH_
#define KTSIMPLECLUSTERINGPROCESSOR_HH_

#include "KTProcessor.hh"

#include "boost/signals2.hpp"

#include <list>
#include <map>

namespace Katydid
{
    class KTPowerSpectrum;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    class KTSimpleClusteringProcessor : public KTProcessor
    {
        private:
            typedef std::list< std::multimap< Int_t, Int_t >* > epbList;

        public:
            KTSimpleClusteringProcessor();
            virtual ~KTSimpleClusteringProcessor();

            Bool_t ApplySetting(const KTSetting* setting);

            void ProcessPowerSpectrum(UInt_t psNum, KTPowerSpectrum* powerSpectrum);

            void SetEventPeakBinsList(epbList* eventPeakBinsList);
            void SetMinimumGroupSize(UInt_t size);

        private:
            epbList* fEventPeakBins;
            Double_t fThresholdMult;

            UInt_t fMinimumGroupSize;

            Int_t fGroupBinsMarginLow;
            Int_t fGroupBinsMarginHigh;
            Int_t fGroupBinsMarginSameTime;

            //****************
            // Slot connection
            //****************

        public:
            //void ConnectToPowerSpectrumSignalFrom(KTSignalEmitter* sigEmit);
            void SetPowerSpectrumSlotConnection(boost::signals2::connection psConn);

        private:
            boost::signals2::connection fPowerSpectrumConnection;

    };

    inline void KTSimpleClusteringProcessor::SetEventPeakBinsList(epbList* list)
    {
        fEventPeakBins = list;
        return;
    }

    inline void KTSimpleClusteringProcessor::SetMinimumGroupSize(UInt_t size)
    {
        fMinimumGroupSize = size;
        return;
    }

    inline void KTSimpleClusteringProcessor::SetPowerSpectrumSlotConnection(boost::signals2::connection psConn)
    {
        fPowerSpectrumConnection = psConn;
        return;
    }

} /* namespace Katydid */
#endif /* KTSIMPLECLUSTERINGPROCESSOR_HH_ */
