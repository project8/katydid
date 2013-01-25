/*
 * KTROOTTreeTypeWriterCandidates.hh
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITERCANDIDATES_HH_
#define KTROOTTREETYPEWRITERCANDIDATES_HH_

#include "KTROOTTreeWriter.hh"

#include "KTFrequencyCandidate.hh"
#include "KTFrequencyCandidateData.hh"

#include "TClonesArray.h"

namespace Katydid
{
    class KTFrequencyCandidateData;

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

    };

    TTree* KTROOTTreeTypeWriterCandidates::GetFrequencyCandidateTree() const
    {
        return fFreqCandidateTree;
    }

} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERCANDIDATES_HH_ */
