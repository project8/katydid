/*
 * KTWaterfallTree.hh
 *
 *  Created on: May 4, 2012
 *      Author: nsoblath
 */

#ifndef KTWATERFALLTREE_HH_
#define KTWATERFALLTREE_HH_

#include "KTWaterfallCandidate.hh"

#include "TTree.h"

#include <string>

namespace Katydid
{

    class KTWaterfallTree : public TTree
    {
        public:
            KTWaterfallTree();
            KTWaterfallTree(const std::string& name, const std::string& title);
            virtual ~KTWaterfallTree();

        public:
            /// Writes a new candidate into the tree.
            /// Returns true if write to the tree was successful.
            /// Note: tree assumes ownership of the candidate.
            Bool_t AddCandidate(KTWaterfallCandidate* candidate);

            /// Retrieves a candidate from the tree.
            /// Note: tree maintains ownership of the candidate
            KTWaterfallCandidate* GetCandidate(Long64_t index);

            /// Releases tree ownership of the candidate pointed to by fCandidate
            KTWaterfallCandidate* ReleaseCandidate();

        public:
            KTWaterfallCandidate* GetCandidatePointer() const;

            void SetCandidatePointer(KTWaterfallCandidate* candidate);

        private:
            KTWaterfallCandidate* fCandidate;

            ClassDef(KTWaterfallTree, 1)
    };

    inline KTWaterfallCandidate* KTWaterfallTree::GetCandidatePointer() const
    {
        return fCandidate;
    }

    inline void KTWaterfallTree::SetCandidatePointer(KTWaterfallCandidate* candidate)
    {
        delete fCandidate;
        fCandidate = candidate;
        return;
    }

} /* namespace Katydid */
#endif /* KTWATERFALLTREE_HH_ */
