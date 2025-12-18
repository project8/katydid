/*
 * KTProcessedCavityEventData.hh
 *
 *  Created on: Sep 11, 2025
 *      Author: juniorpe
 */

#ifndef KTPROCESSEDCAVITYEVENTDATA_HH_
#define KTPROCESSEDCAVITYEVENTDATA_HH_

#include "KTData.hh"
#include "KTMultiTrackEventData.hh"
#include "KTMemberVariable.hh"

#include <vector>

namespace Katydid
{

    class KTProcessedCavityEventData : public Nymph::KTExtensibleData< KTProcessedCavityEventData >
    {
        public:
            KTProcessedCavityEventData();
            KTProcessedCavityEventData(const KTProcessedCavityEventData& orig);
            virtual ~KTProcessedCavityEventData();

            KTProcessedCavityEventData& operator=(const KTProcessedCavityEventData& rhs);

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(uint64_t, AcquisitionID);
            MEMBERVARIABLE(unsigned, EventID);
            MEMBERVARIABLE(unsigned, TotalEventSequences);

            MEMBERVARIABLE(double, FirstTrackStartCyclotronFrequency);
            MEMBERVARIABLE(double, FirstTrackAxialFrequency);

            MEMBERVARIABLEREF(std::vector<int>, MPTEventSequenceID);
            MEMBERVARIABLEREF(std::vector<double>, MPTStartCyclotronFrequency);
            MEMBERVARIABLEREF(std::vector<double>, MPTAxialFrequency);
            void AddProcessedMPT(int seqID, double startCyclFreq, double axialFreq);
            void ClearProcessedMPT();

        public:
            static const std::string sName;
    };

    inline void KTProcessedCavityEventData::AddProcessedMPT(int seqID, double startCyclFreq, double axialFreq)
    {
        fMPTEventSequenceID.push_back(seqID);
        fMPTStartCyclotronFrequency.push_back(startCyclFreq);
        fMPTAxialFrequency.push_back(axialFreq);
    }

    inline void KTProcessedCavityEventData::ClearProcessedMPT()
    {
        fMPTEventSequenceID.clear();
        fMPTStartCyclotronFrequency.clear();
        fMPTAxialFrequency.clear();
    }

}
#endif

