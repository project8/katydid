/*
 * KTROOTTreeTypeWriterCandidates.hh
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITERCANDIDATES_HH_
#define KTROOTTREETYPEWRITERCANDIDATES_HH_

#include "KTROOTTreeWriter.hh"

class TTree;

namespace Katydid
{
    class KTFrequencyCandidateData;

    struct TFrequencyCandidateData
    {
        UShort_t fComponent;
        ULong64_t fSlice;
        Double_t fTimeInRun;
        Double_t fThreshold;
        UInt_t fFirstBin;
        UInt_t fLastBin;
        Double_t fMeanFrequency;
        Double_t fPeakAmplitude;
        Double_t fAmplitudeSum;
    };

    class KTROOTTreeTypeWriterCandidates : public KTROOTTreeTypeWriter//, public KTTypeWriterCandidates
    {
        public:
            KTROOTTreeTypeWriterCandidates();
            virtual ~KTROOTTreeTypeWriterCandidates();

            void RegisterSlots();

        public:
            void WriteFrequencyCandidates(const KTFrequencyCandidateData* header);

        public:
            TTree* GetFrequencyCandidateTree() const;

        private:
            void SetupFrequencyCandidateTree();

            TTree* fFreqCandidateTree;

            TFrequencyCandidateData fFreqCandidateData;

    };

    TTree* KTROOTTreeTypeWriterCandidates::GetFrequencyCandidateTree() const
    {
        return fFreqCandidateTree;
    }




} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERCANDIDATES_HH_ */
