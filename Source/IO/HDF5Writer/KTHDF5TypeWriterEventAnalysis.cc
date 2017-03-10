/*
 * KTHDF5TypeWriterEventAnalysis.cc
 *
 *  Created on: 9/13/2014
 *      Author: J.N. Kofron
 */

#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTMultiTrackEventData.hh"
#include "KTPowerFitData.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTWaterfallCandidateData.hh"

#include <sstream>
#include <string>
#include "KTHDF5TypeWriterEventAnalysis.hh"

namespace Katydid {
    KTLOGGER(publog, "KTHDF5TypeWriterEventAnalysis");

    static Nymph::KTTIRegistrar<KTHDF5TypeWriter, KTHDF5TypeWriterEventAnalysis> sH5CNDrg;
    KTHDF5TypeWriterEventAnalysis::KTHDF5TypeWriterEventAnalysis() :
        KTHDF5TypeWriter(),
        fMTEDataBuffer(),
        fMTETracksDataBuffer(),
        fPTDataBuffer(),
        fPFDataBuffer(),
        fFlushMTEIdx(0),
        fFlushPTIdx(0),
        fFlushPFIdx(0) {
            /*
             * First we build the appropriate compound datatype for MTE events
             */
            this->fMTEType = new H5::CompType(MTESize);
            // Insert fields into the type
            for (int f = 0; f < MTENFields; f++) {
                this->fMTEType->insertMember(
                    MTEFieldNames[f],
                    MTEFieldOffsets[f],
                    MTEFieldTypes[f]);
            }
            this->fPTType = new H5::CompType(PTSize);
            for (int f = 0; f < PTNFields; f++) {
                this->fPTType->insertMember(
                    PTFieldNames[f],
                    PTFieldOffsets[f],
                    PTFieldTypes[f]);
            }
            this->fPFType = new H5::CompType(PFSize);
            for (int f = 0; f < PFNFields; f++) {
                this->fPFType->insertMember(
                    PFFieldNames[f],
                    PFFieldOffsets[f],
                    PFFieldTypes[f]);
            }
        }

    KTHDF5TypeWriterEventAnalysis::~KTHDF5TypeWriterEventAnalysis() {
        if(fMTEType) delete fMTEType;
        if(fPTType) delete fPTType;
        if(fPFType) delete fPFType;
    }

    void KTHDF5TypeWriterEventAnalysis::RegisterSlots() {
        fWriter->RegisterSlot("frequency-candidates", this, &KTHDF5TypeWriterEventAnalysis::WriteFrequencyCandidates);
        fWriter->RegisterSlot("waterfall-candidates", this, &KTHDF5TypeWriterEventAnalysis::WriteWaterfallCandidate);
        fWriter->RegisterSlot("sparse-waterfall-candidates", this, &KTHDF5TypeWriterEventAnalysis::WriteSparseWaterfallCandidate);
        fWriter->RegisterSlot("processed-track", this, &KTHDF5TypeWriterEventAnalysis::WriteProcessedTrack);
        fWriter->RegisterSlot("final-write-tracks", this, &KTHDF5TypeWriterEventAnalysis::WritePTBuffer);
        fWriter->RegisterSlot("multi-track-event", this, &KTHDF5TypeWriterEventAnalysis::WriteMultiTrackEvent);
        fWriter->RegisterSlot("final-write-events", this, &KTHDF5TypeWriterEventAnalysis::WriteMTEBuffer);
        fWriter->RegisterSlot("power-fit", this, &KTHDF5TypeWriterEventAnalysis::WritePowerFitData);
        fWriter->RegisterSlot("final-write-pf", this, &KTHDF5TypeWriterEventAnalysis::WritePFBuffer);
        return;
    }

    void KTHDF5TypeWriterEventAnalysis::WriteFrequencyCandidates(Nymph::KTDataPtr data) {
        KTDEBUG("NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterEventAnalysis::WriteWaterfallCandidate(Nymph::KTDataPtr data) {
        KTDEBUG("NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterEventAnalysis::WriteSparseWaterfallCandidate(Nymph::KTDataPtr data) {
        KTDEBUG("NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterEventAnalysis::WriteProcessedTrack(Nymph::KTDataPtr data) {
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

        (this->fPTDataBuffer).push_back(track);

        KTDEBUG("Done.");
        return;
    }
    void KTHDF5TypeWriterEventAnalysis::WriteMultiTrackEvent(Nymph::KTDataPtr data) {
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
        (this->fMTEDataBuffer).push_back(event);

        // Write the tracks that make up this event
        KTDEBUG(publog, "Event " << event.EventID << " contains " << mteData.GetNTracks() << " tracks ");
        PTData track;
        for (TrackSetIt MTETrackIt = mteData.GetTracksBegin(); MTETrackIt != mteData.GetTracksEnd(); MTETrackIt++)
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
            (this->fMTETracksDataBuffer).push_back(track);
            KTDEBUG(publog, "Added track " << track.TrackID << "(EventID=" << track.EventID << ")");
        }


        KTDEBUG("Done.");
        return;
    }

    void KTHDF5TypeWriterEventAnalysis::WriteMTEBuffer() {
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

    void KTHDF5TypeWriterEventAnalysis::WritePTBuffer() {
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

    void KTHDF5TypeWriterEventAnalysis::WritePowerFitData(Nymph::KTDataPtr data) {
        KTDEBUG(publog, "Processing Power Fit Data");
        KTPowerFitData& pfData = data->Of< KTPowerFitData >();

        PFData powerFit;
        powerFit.Average = pfData.GetAverage();
        powerFit.RMS = pfData.GetRMS();
        powerFit.Skewness = pfData.GetSkewness();
        powerFit.Kurtosis = pfData.GetKurtosis();
        powerFit.NormCentral = pfData.GetNormCentral();
        powerFit.MeanCentral = pfData.GetMeanCentral();
        powerFit.SigmaCentral = pfData.GetSigmaCentral();
        powerFit.MaximumCentral = pfData.GetMaximumCentral();
        powerFit.RMSAwayFromCentral = pfData.GetRMSAwayFromCentral();
        powerFit.CentralPowerRatio = pfData.GetCentralPowerRatio();

        (this->fPFDataBuffer).push_back(powerFit);

        KTDEBUG(publog, "Done.");
        return;
    }

    void KTHDF5TypeWriterEventAnalysis::WritePFBuffer() {
        KTDEBUG(publog, "Writing PF Data Buffer");

        hsize_t* dims = new hsize_t(this->fPFDataBuffer.size());
        H5::DataSpace dspace(1, dims);

        if( !fWriter->OpenAndVerifyFile() ) return;
        H5::Group* pfGroup = fWriter->AddGroup("pf-data");

        std::stringstream namestream;
        std::string dsetname;
        namestream << "pf-data_" << this->fFlushPFIdx;
        namestream >> dsetname;
        H5::DataSet* dset = new H5::DataSet(pfGroup->createDataSet(dsetname.c_str(), *(this->fPFType), dspace));
        dset->write((this->fPFDataBuffer).data(),*(this->fPFType));
        this->fPFDataBuffer.clear();
        this->fFlushPFIdx++;
    }

}  //  namespace Katydid
