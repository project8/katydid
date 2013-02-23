/*
 * KTWaterfallCandidateData.hh
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#ifndef KTWATERFALLCANDIDATEDATA_HH_
#define KTWATERFALLCANDIDATEDATA_HH_

#include "KTWriteableData.hh"

namespace Katydid
{
    class KTWaterfallCandidate;

    class KTWaterfallCandidateData : public KTWriteableData
    {
        protected:
            struct PerComponentData
            {
                KTWaterfallCandidate fCandidate;
            };

        public:
            KTWaterfallCandidateData();
            virtual ~KTWaterfallCandidateData();

            void Accept(KTWriter* writer) const;

            const KTWaterfallCandidate& GetCandidate(UInt_t groupNum = 0) const;
            UInt_t GetNComponents() const;

            void SetCandidate(const KTWaterfallCandidate& candidate, UInt_t component = 0);
            void SetNComponents(UInt_t channels);

            UInt_t GetNTimeBins() const;
            Double_t GetTimeBinWidth() const;

            UInt_t GetNFreqBins() const;
            Double_t GetFreqBinWidth() const;

            Double_t GetTimeInRun() const;
            ULong64_t GetSliceNumber() const;

            void SetTimeInRun(Double_t tir);
            void SetSliceNumber(ULong64_t slice);

        protected:
            std::vector< PerComponentData > fComponentData;

            Double_t fTimeInRun;
            ULong64_t fSliceNumber;
    };

} /* namespace Katydid */
#endif /* KTWATERFALLCANDIDATEDATA_HH_ */
