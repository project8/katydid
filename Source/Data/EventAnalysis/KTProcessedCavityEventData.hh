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
#include <set>

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
            MEMBERVARIABLE(std::vector<int>, FirstTrackBandIDs);
            MEMBERVARIABLE(std::vector<int>, FirstTrackBandClassifications);

            MEMBERVARIABLEREF(std::vector<int>, MPTEventSequenceID);
            MEMBERVARIABLEREF(std::vector<double>, MPTStartCyclotronFrequency);
            MEMBERVARIABLEREF(std::vector<double>, MPTAxialFrequency);

            MEMBERVARIABLEREF(std::vector<int>, AllTrackIDs);
            MEMBERVARIABLEREF(std::vector<int>, AllTrackEventSequenceIDs);
            MEMBERVARIABLEREF(std::vector<int>, AllTrackBandClassifications);

            void ClearProcessedEvent();

            void AddFirstTrackBandInfo(int trackID, int classification);
            void ClearFirstTrackBandInfo();

            void AddProcessedMPT(int seqID, double startCyclFreq, double axialFreq);

            void AddClassificationData(int trackID, int seqID, int bandClassification);
            void AddClassificationData(const std::vector<AllTrackData>& tracksInMPT, int seqID, int bandClassification);

        public:
            static const std::string sName;
    };

    inline void KTProcessedCavityEventData::ClearProcessedEvent()
    {
        fFirstTrackBandIDs.clear();
        fFirstTrackBandClassifications.clear();

        fMPTEventSequenceID.clear();
        fMPTStartCyclotronFrequency.clear();
        fMPTAxialFrequency.clear();

        fAllTrackIDs.clear();
        fAllTrackEventSequenceIDs.clear();
        fAllTrackBandClassifications.clear();
    }

    inline void KTProcessedCavityEventData::AddFirstTrackBandInfo(int trackID, int classification)
    {
        fFirstTrackBandIDs.push_back(trackID);
        fFirstTrackBandClassifications.push_back(classification);
    }

    inline void KTProcessedCavityEventData::ClearFirstTrackBandInfo()
    {
        fFirstTrackBandIDs.clear();
        fFirstTrackBandClassifications.clear();
    }

    inline void KTProcessedCavityEventData::AddProcessedMPT(int seqID, double startCyclFreq, double axialFreq)
    {
        fMPTEventSequenceID.push_back(seqID);
        fMPTStartCyclotronFrequency.push_back(startCyclFreq);
        fMPTAxialFrequency.push_back(axialFreq);
    }

    inline void KTProcessedCavityEventData::AddClassificationData(int trackID, int seqID, int bandClassification)
    {
        fAllTrackIDs.push_back(trackID);
        fAllTrackEventSequenceIDs.push_back(seqID);
        fAllTrackBandClassifications.push_back(bandClassification);
    }

    inline void KTProcessedCavityEventData::AddClassificationData(const std::vector<AllTrackData>& tracksInMPT, int seqID, int bandClassification)
    {
        for (const auto& trk : tracksInMPT)
        {
            AddClassificationData(trk.fProcTrack.GetTrackID(), seqID, bandClassification);
        }
    }

}
#endif

