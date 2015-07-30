/*
 * KTBasicROOTTypeWriterTime.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTISLICEROOTTYPEWRITERTIME_HH_
#define KTMULTISLICEROOTTYPEWRITERTIME_HH_

#include "KTMultiSliceROOTWriter.hh"

#include "KTData.hh"

class TH1D;

namespace Katydid
{
    using namespace Nymph;
    class KTEggHeader;

    class KTMultiSliceROOTTypeWriterTime : public KTMEROOTTypeWriterBase//, public KTTypeWriterTime
    {
        public:
            KTMultiSliceROOTTypeWriterTime();
            virtual ~KTMultiSliceROOTTypeWriterTime();

            void RegisterSlots();

            virtual void StartNewHistograms();
            virtual void FinishHistograms();

            void OutputHistograms();

            void ClearHistograms();

        public:
            void StartByHeader(KTDataPtr);

            void AddTimeSeriesData(KTDataPtr data);

        private:
            std::vector< TH1D* > fTSHists;

    };


} /* namespace Katydid */
#endif /* KTMULTISLICEROOTTYPEWRITERTIME_HH_ */
