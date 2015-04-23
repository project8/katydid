/* 
 * KTHDF5TypeWriterCandidates.cc
 *
 *  Created on: 9/13/2014
 *      Author: J.N. Kofron
 */

#include "KTHDF5TypeWriterCandidates.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTMultiTrackEventData.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTWaterfallCandidateData.hh"

#include <sstream>
#include <string>

namespace Katydid {
    KTLOGGER(publog, "KTHDF5TypeWriterCandidates");

    static KTTIRegistrar<KTHDF5TypeWriter, KTHDF5TypeWriterCandidates> sH5CNDrg;
    KTHDF5TypeWriterCandidates::KTHDF5TypeWriterCandidates() :
        KTHDF5TypeWriter(),
        fMTEDataBuffer(),
        fMTETracksDataBuffer(),
        fPTDataBuffer(),
        fFlushMTEIdx(0),
        fFlushPTIdx(0) {
            /*
             * First we build the appropriate compound datatype for MTE events
             */
            this->fMTEType = new H5::CompType(MTESize);
            // Insert fields into the type
            for (int f = 0; f < 23; f++) {
                this->fMTEType->insertMember(
                    MTEFieldNames[f],
                    MTEFieldOffsets[f],
                    MTEFieldTypes[f]);
            }
            this->fPTType = new H5::CompType(PTSize);
            for (int f = 0; f < 23; f++) {
                this->fPTType->insertMember(
                    PTFieldNames[f],
                    PTFieldOffsets[f],
                    PTFieldTypes[f]);
            }
        }

    KTHDF5TypeWriterCandidates::~KTHDF5TypeWriterCandidates() {
        if(fMTEType) delete fMTEType;
        if(fPTType) delete fPTType;
    }

    void KTHDF5TypeWriterCandidates::RegisterSlots() {
        fWriter->RegisterSlot("frequency-candidates", this, &KTHDF5TypeWriterCandidates::WriteFrequencyCandidates);
        fWriter->RegisterSlot("waterfall-candidates", this, &KTHDF5TypeWriterCandidates::WriteWaterfallCandidate);
        fWriter->RegisterSlot("sparse-waterfall-candidates", this, &KTHDF5TypeWriterCandidates::WriteSparseWaterfallCandidate);
        fWriter->RegisterSlot("processed-track", this, &KTHDF5TypeWriterCandidates::WriteProcessedTrack);
        fWriter->RegisterSlot("final-write-tracks",this, &KTHDF5TypeWriterCandidates::WritePTBuffer);
        fWriter->RegisterSlot("multi-track-event", this, &KTHDF5TypeWriterCandidates::WriteMultiTrackEvent);
        fWriter->RegisterSlot("final-write-events",this, &KTHDF5TypeWriterCandidates::WriteMTEBuffer);
        return;
    }

    void KTHDF5TypeWriterCandidates::WriteFrequencyCandidates(KTDataPtr data) {
        KTDEBUG("NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterCandidates::WriteWaterfallCandidate(KTDataPtr data) {
        KTDEBUG("NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterCandidates::WriteSparseWaterfallCandidate(KTDataPtr data) {
        KTDEBUG("NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterCandidates::WriteProcessedTrack(KTDataPtr data) {
        KTDEBUG(publog, "Processing Tracks");
        KTProcessedTrackData& ptData = data->Of< KTProcessedTrackData >();

        PTData track;
        track.Component = ptData.GetComponent();
        track.TrackID = ptData.GetTrackID();
        track.IsCut = ptData.GetIsCut();
        track.StartTimeInAcq = ptData.GetStartTimeInAcq();
        track.StartTimeInRunC = ptData.GetStartTimeInRunC();
        track.EndTimeInRunC = ptData.GetEndTimeInRunC();
        track.TimeLength = ptData.GetTimeLength();
        track.StartFrequency = ptData.GetStartFrequency();
        track.EndFrequency = ptData.GetEndFrequency();
        track.FrequencyWidth = ptData.GetFrequencyWidth();
        track.Slope = ptData.GetSlope();
        track.Intercept = ptData.GetIntercept();
        track.TotalPower = ptData.GetTotalPower();
        track.StartTimeInRunCSigma = ptData.GetStartTimeInRunCSigma();
        track.EndTimeInRunCSigma = ptData.GetEndTimeInRunCSigma();
        track.TimeLengthSigma = ptData.GetTimeLengthSigma();
        track.StartFrequencySigma = ptData.GetStartFrequencySigma();
        track.EndFrequencySigma = ptData.GetEndFrequencySigma();
        track.FrequencyWidthSigma = ptData.GetFrequencyWidthSigma();
        track.SlopeSigma = ptData.GetSlopeSigma();
        track.InterceptSigma = ptData.GetInterceptSigma();
        track.TotalPowerSigma = ptData.GetTotalPowerSigma();

        (this->fPTDataBuffer).push_back(track);

        KTDEBUG("Done.");
        return;
    }
    void KTHDF5TypeWriterCandidates::WriteMultiTrackEvent(KTDataPtr data) {
        KTDEBUG(publog, "Processing MTE");
        KTMultiTrackEventData& mteData = data->Of< KTMultiTrackEventData >();

        // Write the event information
        MTEData event;
        event.Component = mteData.GetComponent();
        event.EventID = mteData.GetEventID();
        event.StartTimeInAcq = mteData.GetStartTimeInAcq();
        event.StartTimeInRunC = mteData.GetStartTimeInRunC();
        event.EndTimeInRunC = mteData.GetEndTimeInRunC();
        event.TimeLength = mteData.GetTimeLength();
        event.StartFrequency = mteData.GetStartFrequency();
        event.EndFrequency = mteData.GetEndFrequency();
        event.MinimumFrequency = mteData.GetMinimumFrequency();
        event.MaximumFrequency = mteData.GetMaximumFrequency();
        event.FrequencyWidth = mteData.GetFrequencyWidth();
        event.StartTimeInRunCSigma = mteData.GetStartTimeInRunCSigma();
        event.EndTimeInRunCSigma = mteData.GetEndTimeInRunCSigma();
        event.TimeLengthSigma = mteData.GetTimeLengthSigma();
        event.StartFrequencySigma = mteData.GetStartFrequencySigma();
        event.EndFrequencySigma = mteData.GetEndFrequencySigma();
        event.FrequencyWidthSigma = mteData.GetFrequencyWidthSigma();
        event.FirstTrackID = mteData.GetFirstTrackID();
        event.FirstTrackTimeLength = mteData.GetFirstTrackTimeLength();
        event.FirstTrackFrequencyWidth = mteData.GetFirstTrackFrequencyWidth();
        event.FirstTrackSlope = mteData.GetFirstTrackSlope();
        event.FirstTrackIntercept = mteData.GetFirstTrackIntercept();
        event.FirstTrackTotalPower = mteData.GetFirstTrackTotalPower();
        (this->fMTEDataBuffer).push_back(event);

        // Write the tracks that make up this event
        KTDEBUG(publog, "Event " << event.EventID << " contains " << mteData.GetNTracks() << " tracks ");
        PTData track;
        for (KTMultiTrackEventData::TrackIt MTETrackIt = mteData.GetTracksBegin(); MTETrackIt != mteData.GetTracksEnd(); MTETrackIt++)
        {
            KTProcessedTrackData& ptData = mteData.GetTrack(MTETrackIt->first);
            track.Component = ptData.GetComponent();
            track.TrackID = ptData.GetTrackID();
            track.EventID = mteData.GetEventID();
            track.IsCut = ptData.GetIsCut();
            track.StartTimeInAcq = ptData.GetStartTimeInAcq();
            track.StartTimeInRunC = ptData.GetStartTimeInRunC();
            track.EndTimeInRunC = ptData.GetEndTimeInRunC();
            track.TimeLength = ptData.GetTimeLength();
            track.StartFrequency = ptData.GetStartFrequency();
            track.EndFrequency = ptData.GetEndFrequency();
            track.FrequencyWidth = ptData.GetFrequencyWidth();
            track.Slope = ptData.GetSlope();
            track.Intercept = ptData.GetIntercept();
            track.TotalPower = ptData.GetTotalPower();
            track.StartTimeInRunCSigma = ptData.GetStartTimeInRunCSigma();
            track.EndTimeInRunCSigma = ptData.GetEndTimeInRunCSigma();
            track.TimeLengthSigma = ptData.GetTimeLengthSigma();
            track.StartFrequencySigma = ptData.GetStartFrequencySigma();
            track.EndFrequencySigma = ptData.GetEndFrequencySigma();
            track.FrequencyWidthSigma = ptData.GetFrequencyWidthSigma();
            track.SlopeSigma = ptData.GetSlopeSigma();
            track.InterceptSigma = ptData.GetInterceptSigma();
            track.TotalPowerSigma = ptData.GetTotalPowerSigma();
            (this->fMTETracksDataBuffer).push_back(track);
            KTDEBUG(publog, "Added track " << track.TrackID << "(EventID=" << track.EventID << ")");
        }


        KTDEBUG("Done.");
        return;
    }

    void KTHDF5TypeWriterCandidates::WriteMTEBuffer() {
        KTDEBUG("writing MTE buffer.");
        // Now create the dataspace we need
        hsize_t* dims_cands = new hsize_t(this->fMTEDataBuffer.size());
        hsize_t* dims_tracks = new hsize_t(this->fMTETracksDataBuffer.size());
        H5::DataSpace dspace_cands(1, dims_cands);
        H5::DataSpace dspace_tracks(1, dims_tracks);

        if( !fWriter->OpenAndVerifyFile() ) return;
        // Make a group for the events, and a separate group for the tracks belonging to the event
        H5::Group* candidatesGroup = fWriter->AddGroup("candidates");
        H5::Group* candidateTracksGroup = fWriter->AddGroup("candidate_tracks");

        // OK, create the dataset and write it down.
        // Write the event information
        std::stringstream namestream;
        std::string dsetname;
        namestream << "candidates_" << this->fFlushMTEIdx;
        namestream >> dsetname;
        H5::DataSet* dset_cands = new H5::DataSet(candidatesGroup->createDataSet(dsetname.c_str(),
                                                                           *(this->fMTEType),
                                                                           dspace_cands));
        dset_cands->write((this->fMTEDataBuffer).data(),*(this->fMTEType));

        // Write the tracks that belong to the current events
        //dsetname.clear();
        //namestream.str(std::string());
        std::stringstream namestream2;
        std::string dsetname2;
        namestream2 << "candidate_tracks_" << this->fFlushMTEIdx;
        namestream2 >> dsetname2;
        H5::DataSet* dset_tracks = new H5::DataSet(candidateTracksGroup->createDataSet(dsetname2.c_str(),
                                                                           *(this->fPTType),
                                                                           dspace_tracks));
        dset_tracks->write((this->fMTETracksDataBuffer).data(),*(this->fPTType));

        this->fMTETracksDataBuffer.clear();
        this->fMTEDataBuffer.clear();
        this->fFlushMTEIdx++;
    }

    void KTHDF5TypeWriterCandidates::WritePTBuffer() {
        KTDEBUG("writing PT buffer.");
        // Now create the dataspace we need
        hsize_t* dims = new hsize_t(this->fPTDataBuffer.size());
        H5::DataSpace dspace(1, dims);

        if( !fWriter->OpenAndVerifyFile() ) return;
        // Make a group
        H5::Group* tracksGroup = fWriter->AddGroup("tracks");

        // OK, create the dataset and write it down.
        std::stringstream namestream;
        std::string dsetname;
        namestream << "tracks_" << this->fFlushPTIdx;
        namestream >> dsetname;
        H5::DataSet* dset = new H5::DataSet(tracksGroup->createDataSet(dsetname.c_str(),
                                                                           *(this->fPTType),
                                                                           dspace));
        dset->write((this->fPTDataBuffer).data(),*(this->fPTType));
        this->fPTDataBuffer.clear();
        this->fFlushPTIdx++;
    }

}  //  namespace Katydid
