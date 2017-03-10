/*
 * KTHDF5TypeWriterEventAnalysis.cc
 *
 *  Created on: 9/13/2014
 *      Author: J.N. Kofron
 */

#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTMultiTrackEventData.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTWaterfallCandidateData.hh"

#include <sstream>
#include <string>
#include "KTHDF5TypeWriterEventAnalysis.hh"

namespace Katydid
{
    KTLOGGER(publog, "KTHDF5TypeWriterEventAnalysis");

    static Nymph::KTTIRegistrar<KTHDF5TypeWriter, KTHDF5TypeWriterEventAnalysis> sH5CNDrg;
    KTHDF5TypeWriterEventAnalysis::KTHDF5TypeWriterEventAnalysis() :
            KTHDF5TypeWriter(),
            fMTEDataBuffer(),
            fMTETracksDataBuffer(),
            fPTDataBuffer(),
            fFlushMTEIdx(0),
            fFlushPTIdx(0)
    {
        /*
         * First we build the appropriate compound datatype for MTE events
         */
        fMTEType = new H5::CompType(MTESize);
        // Insert fields into the type
        for (int f = 0; f < MTENFields; ++f)
        {
            fMTEType->insertMember( MTEFieldNames[f], MTEFieldOffsets[f], MTEFieldTypes[f]);
        }
        fPTType = new H5::CompType(PTSize);
        for (int f = 0; f < PTNFields; ++f)
        {
            fPTType->insertMember(PTFieldNames[f], PTFieldOffsets[f], PTFieldTypes[f]);
        }
        }

    KTHDF5TypeWriterEventAnalysis::~KTHDF5TypeWriterEventAnalysis()
    {
        if(fMTEType) delete fMTEType;
        if(fPTType) delete fPTType;
    }

    void KTHDF5TypeWriterEventAnalysis::RegisterSlots()
    {
        //fWriter->RegisterSlot("frequency-candidates", this, &KTHDF5TypeWriterEventAnalysis::WriteFrequencyCandidates);
        //fWriter->RegisterSlot("waterfall-candidates", this, &KTHDF5TypeWriterEventAnalysis::WriteWaterfallCandidate);
        //fWriter->RegisterSlot("swfc", this, &KTHDF5TypeWriterEventAnalysis::WriteSparseWaterfallCandidate);
        fWriter->RegisterSlot("proc-track", this, &KTHDF5TypeWriterEventAnalysis::WriteProcessedTrack);
        fWriter->RegisterSlot("final-write-tracks",this, &KTHDF5TypeWriterEventAnalysis::WritePTBuffer);
        fWriter->RegisterSlot("mt-event", this, &KTHDF5TypeWriterEventAnalysis::WriteMultiTrackEvent);
        fWriter->RegisterSlot("final-write-events",this, &KTHDF5TypeWriterEventAnalysis::WriteMTEBuffer);
        return;
    }

    /*
    void KTHDF5TypeWriterEventAnalysis::WriteFrequencyCandidates(Nymph::KTDataPtr data)
    {
        KTDEBUG("NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterEventAnalysis::WriteWaterfallCandidate(Nymph::KTDataPtr data)
    {
        KTDEBUG("NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterEventAnalysis::WriteSparseWaterfallCandidate(Nymph::KTDataPtr data)
    {
        KTDEBUG("NOT IMPLEMENTED");
    }
    */
    void KTHDF5TypeWriterEventAnalysis::WriteProcessedTrack(Nymph::KTDataPtr data)
    {
        KTDEBUG(publog, "Processing Tracks");
        KTProcessedTrackData& ptData = data->Of< KTProcessedTrackData >();

        PTData track;
        track.Component = ptData.GetComponent();
        track.AcquisitionID = ptData.GetAcquisitionID();
        track.TrackID = ptData.GetTrackID();
        track.EventID = ptData.GetEventID();
        track.EventSequenceID = ptData.GetEventSequenceID();
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

        (fPTDataBuffer).push_back(track);

        KTDEBUG("Done.");
        return;
    }
    void KTHDF5TypeWriterEventAnalysis::WriteMultiTrackEvent(Nymph::KTDataPtr data)
    {
        KTDEBUG(publog, "Processing MTE");
        KTMultiTrackEventData& mteData = data->Of< KTMultiTrackEventData >();

        // Write the event information
        MTEData event;
        event.Component = mteData.GetComponent();
        event.AcquisitionID = mteData.GetAcquisitionID();
        event.EventID = mteData.GetEventID();
        event.TotalEventSequences = mteData.GetTotalEventSequences();
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
        event.UnknownEventTopology = mteData.GetUnknownEventTopology();
        fMTEDataBuffer.push_back(event);

        // Write the tracks that make up this event
        KTDEBUG(publog, "Event " << event.EventID << " contains " << mteData.GetNTracks() << " tracks ");
        PTData track;
        for (TrackSetIt MTETrackIt = mteData.GetTracksBegin(); MTETrackIt != mteData.GetTracksEnd(); ++MTETrackIt)
        {
            track.Component = MTETrackIt->GetComponent();
            track.AcquisitionID = MTETrackIt->GetAcquisitionID();
            track.TrackID = MTETrackIt->GetTrackID();
            track.EventID = event.EventID; // Get the Event ID from the Event, not from the Track
            track.EventSequenceID = MTETrackIt->GetEventSequenceID();
            track.IsCut = MTETrackIt->GetIsCut();
            track.StartTimeInAcq = MTETrackIt->GetStartTimeInAcq();
            track.StartTimeInRunC = MTETrackIt->GetStartTimeInRunC();
            track.EndTimeInRunC = MTETrackIt->GetEndTimeInRunC();
            track.TimeLength = MTETrackIt->GetTimeLength();
            track.StartFrequency = MTETrackIt->GetStartFrequency();
            track.EndFrequency = MTETrackIt->GetEndFrequency();
            track.FrequencyWidth = MTETrackIt->GetFrequencyWidth();
            track.Slope = MTETrackIt->GetSlope();
            track.Intercept = MTETrackIt->GetIntercept();
            track.TotalPower = MTETrackIt->GetTotalPower();
            track.StartTimeInRunCSigma = MTETrackIt->GetStartTimeInRunCSigma();
            track.EndTimeInRunCSigma = MTETrackIt->GetEndTimeInRunCSigma();
            track.TimeLengthSigma = MTETrackIt->GetTimeLengthSigma();
            track.StartFrequencySigma = MTETrackIt->GetStartFrequencySigma();
            track.EndFrequencySigma = MTETrackIt->GetEndFrequencySigma();
            track.FrequencyWidthSigma = MTETrackIt->GetFrequencyWidthSigma();
            track.SlopeSigma = MTETrackIt->GetSlopeSigma();
            track.InterceptSigma = MTETrackIt->GetInterceptSigma();
            track.TotalPowerSigma = MTETrackIt->GetTotalPowerSigma();
            fMTETracksDataBuffer.push_back(track);
            KTDEBUG(publog, "Added track " << track.TrackID << "(EventID=" << track.EventID << ")");
        }


        KTDEBUG("Done.");
        return;
    }

    void KTHDF5TypeWriterEventAnalysis::WriteMTEBuffer()
    {
        if (fMTEDataBuffer.empty())
        {
            KTDEBUG("MTE buffer is empty; no multi-track events written");
            return;
        }

        KTDEBUG("Writing MTE buffer.");
        // Now create the dataspace we need
        hsize_t* dims_cands = new hsize_t(fMTEDataBuffer.size());
        hsize_t* dims_tracks = new hsize_t(fMTETracksDataBuffer.size());
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
        namestream << "candidates_" << fFlushMTEIdx;
        namestream >> dsetname;
        H5::DataSet* dset_cands = new H5::DataSet(candidatesGroup->createDataSet(dsetname.c_str(), *fMTEType, dspace_cands));
        dset_cands->write(fMTEDataBuffer.data(), *fMTEType);

        // Write the tracks that belong to the current events
        //dsetname.clear();
        //namestream.str(std::string());
        std::stringstream namestream2;
        std::string dsetname2;
        namestream2 << "candidate_tracks_" << fFlushMTEIdx;
        namestream2 >> dsetname2;
        H5::DataSet* dset_tracks = new H5::DataSet(candidateTracksGroup->createDataSet(dsetname2.c_str(), *fPTType, dspace_tracks));
        dset_tracks->write(fMTETracksDataBuffer.data(), *fPTType);

        fMTETracksDataBuffer.clear();
        fMTEDataBuffer.clear();
        fFlushMTEIdx++;
    }

    void KTHDF5TypeWriterEventAnalysis::WritePTBuffer()
    {
        if (fPTDataBuffer.empty())
        {
            KTDEBUG("PT buffer is empty; no tracks written");
            return;
        }

        KTDEBUG("Writing PT buffer.");
        // Now create the dataspace we need
        hsize_t* dims = new hsize_t(fPTDataBuffer.size());
        H5::DataSpace dspace(1, dims);

        if( !fWriter->OpenAndVerifyFile() ) return;
        // Make a group
        H5::Group* tracksGroup = fWriter->AddGroup("tracks");

        // OK, create the dataset and write it down.
        std::stringstream namestream;
        std::string dsetname;
        namestream << "tracks_" << fFlushPTIdx;
        namestream >> dsetname;
        H5::DataSet* dset = new H5::DataSet(tracksGroup->createDataSet(dsetname.c_str(), *fPTType, dspace));
        dset->write(fPTDataBuffer.data(), *fPTType);
        fPTDataBuffer.clear();
        fFlushPTIdx++;
    }

}  //  namespace Katydid
