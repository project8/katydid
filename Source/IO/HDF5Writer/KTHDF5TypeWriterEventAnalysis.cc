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
#include "KTRPTrackData.hh"
#include "KTSliceHeader.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTWaterfallCandidateData.hh"
#include "KTClassifierResultsData.hh"

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
            fPFDataBuffer(),
            fMTERPTracksDataBuffer(),
            fRPTDataBuffer(),
            fMTECRPTracksDataBuffer(),
            fFlushMTEIdx(0),
            fFlushPTIdx(0),
            fFlushPFIdx(0),
            fFlushRPTIdx(0)

    {
        /*
         * First we build the appropriate compound datatype for MTE events
         */
        this->fMTEType = new H5::CompType(MTESize);
        // Insert fields into the type
        for (int f = 0; f < MTENFields; f++) 
        {
            this->fMTEType->insertMember(
                MTEFieldNames[f],
                MTEFieldOffsets[f],
                MTEFieldTypes[f]);
        }
        this->fPTType = new H5::CompType(PTSize);
        for (int f = 0; f < PTNFields; f++) 
        {
            this->fPTType->insertMember(
                PTFieldNames[f],
                PTFieldOffsets[f],
                PTFieldTypes[f]);
        }
        this->fPFType = new H5::CompType(PFSize);
        for (int f = 0; f < PFNFields; f++) 
        {
            this->fPFType->insertMember(
                PFFieldNames[f],
                PFFieldOffsets[f],
                PFFieldTypes[f]);
        }
        this->fRPTType = new H5::CompType(RPTSize);
        for (int f = 0; f < RPTNFields; f++) 
        {
            this->fRPTType->insertMember(
                RPTFieldNames[f],
                RPTFieldOffsets[f],
                RPTFieldTypes[f]);
        }
        this->fCRPTType = new H5::CompType(CRPTSize);
        for (int f = 0; f < CRPTNFields; f++)
        {
            this->fCRPTType->insertMember(
                CRPTFieldNames[f],
                CRPTFieldOffsets[f],
                CRPTFieldTypes[f]);
        }
    }

    KTHDF5TypeWriterEventAnalysis::~KTHDF5TypeWriterEventAnalysis()
    {
        if(fMTEType) delete fMTEType;
        if(fPTType) delete fPTType;
        if(fPFType) delete fPFType;
        if(fRPTType) delete fRPTType;
        if(fCRPTType) delete fCRPTType;
    }

    void KTHDF5TypeWriterEventAnalysis::RegisterSlots() 
    {
        //fWriter->RegisterSlot("frequency-candidates", this, &KTHDF5TypeWriterEventAnalysis::WriteFrequencyCandidates);
        //fWriter->RegisterSlot("waterfall-candidates", this, &KTHDF5TypeWriterEventAnalysis::WriteWaterfallCandidate);
        //fWriter->RegisterSlot("sparse-waterfall-candidates", this, &KTHDF5TypeWriterEventAnalysis::WriteSparseWaterfallCandidate);
        fWriter->RegisterSlot("proc-track", this, &KTHDF5TypeWriterEventAnalysis::WriteProcessedTrack);
        fWriter->RegisterSlot("final-write-tracks", this, &KTHDF5TypeWriterEventAnalysis::WritePTBuffer);
        fWriter->RegisterSlot("mt-event", this, &KTHDF5TypeWriterEventAnalysis::WriteMultiTrackEvent);
        fWriter->RegisterSlot("final-write-events", this, &KTHDF5TypeWriterEventAnalysis::WriteMTEBuffer);
        fWriter->RegisterSlot("power-fit", this, &KTHDF5TypeWriterEventAnalysis::WritePowerFitData);
        fWriter->RegisterSlot("final-write-pf", this, &KTHDF5TypeWriterEventAnalysis::WritePFBuffer);
        fWriter->RegisterSlot("rp-track", this, &KTHDF5TypeWriterEventAnalysis::WriteRPTrackEventData);
        fWriter->RegisterSlot("final-write-rp-tracks", this, &KTHDF5TypeWriterEventAnalysis::WriteMTERPTracksBuffer);
        fWriter->RegisterSlot("classified-track", this, &KTHDF5TypeWriterEventAnalysis::WriteCRPTrackEventData);
        fWriter->RegisterSlot("final-write-classified-tracks", this, &KTHDF5TypeWriterEventAnalysis::WriteMTECRPTracksBuffer);

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
            track.Component = MTETrackIt->fProcTrack.GetComponent();
            track.AcquisitionID = MTETrackIt->fProcTrack.GetAcquisitionID();
            track.TrackID = MTETrackIt->fProcTrack.GetTrackID();
            track.EventID = event.EventID; // Get the Event ID from the Event, not from the Track
            track.EventSequenceID = MTETrackIt->fProcTrack.GetEventSequenceID();
            track.IsCut = MTETrackIt->fProcTrack.GetIsCut();
            track.StartTimeInAcq = MTETrackIt->fProcTrack.GetStartTimeInAcq();
            track.StartTimeInRunC = MTETrackIt->fProcTrack.GetStartTimeInRunC();
            track.EndTimeInRunC = MTETrackIt->fProcTrack.GetEndTimeInRunC();
            track.TimeLength = MTETrackIt->fProcTrack.GetTimeLength();
            track.StartFrequency = MTETrackIt->fProcTrack.GetStartFrequency();
            track.EndFrequency = MTETrackIt->fProcTrack.GetEndFrequency();
            track.FrequencyWidth = MTETrackIt->fProcTrack.GetFrequencyWidth();
            track.Slope = MTETrackIt->fProcTrack.GetSlope();
            track.Intercept = MTETrackIt->fProcTrack.GetIntercept();
            track.TotalPower = MTETrackIt->fProcTrack.GetTotalPower();
            track.StartTimeInRunCSigma = MTETrackIt->fProcTrack.GetStartTimeInRunCSigma();
            track.EndTimeInRunCSigma = MTETrackIt->fProcTrack.GetEndTimeInRunCSigma();
            track.TimeLengthSigma = MTETrackIt->fProcTrack.GetTimeLengthSigma();
            track.StartFrequencySigma = MTETrackIt->fProcTrack.GetStartFrequencySigma();
            track.EndFrequencySigma = MTETrackIt->fProcTrack.GetEndFrequencySigma();
            track.FrequencyWidthSigma = MTETrackIt->fProcTrack.GetFrequencyWidthSigma();
            track.SlopeSigma = MTETrackIt->fProcTrack.GetSlopeSigma();
            track.InterceptSigma = MTETrackIt->fProcTrack.GetInterceptSigma();
            track.TotalPowerSigma = MTETrackIt->fProcTrack.GetTotalPowerSigma();
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

    void KTHDF5TypeWriterEventAnalysis::WritePowerFitData(Nymph::KTDataPtr data) 
    {
        KTDEBUG(publog, "Processing Power Fit Data");
        KTPowerFitData& pfData = data->Of< KTPowerFitData >();

        PFData powerFit;
        powerFit.IsValid = pfData.GetIsValid();
        powerFit.NPeaks = pfData.GetNPeaks();
        powerFit.Average = pfData.GetAverage();
        powerFit.RMS = pfData.GetRMS();
        powerFit.Skewness = pfData.GetSkewness();
        powerFit.Kurtosis = pfData.GetKurtosis();
        powerFit.NormCentral = pfData.GetNormCentral();
        powerFit.MeanCentral = pfData.GetMeanCentral();
        powerFit.SigmaCentral = pfData.GetSigmaCentral();
        powerFit.MaximumCentral = pfData.GetMaximumCentral();
        powerFit.RMSAwayFromCentral = pfData.GetRMSAwayFromCentral();
        powerFit.CentralPowerFraction = pfData.GetCentralPowerFraction();
        powerFit.TrackID = pfData.GetTrackID();

        (this->fPFDataBuffer).push_back(powerFit);

        KTDEBUG(publog, "Done.");
        return;
    }

    void KTHDF5TypeWriterEventAnalysis::WritePFBuffer() 
    {
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

    void KTHDF5TypeWriterEventAnalysis::WriteRPTrackEventData(Nymph::KTDataPtr data) 
    {
        KTDEBUG(publog, "Processing RP-Track Event");
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
        RPTData track;
        for (std::set< AllTrackData , TrackTimeComp >::iterator RPTrackIt = mteData.GetTracksBegin(); RPTrackIt != mteData.GetTracksEnd(); ++RPTrackIt)
        {
            //KTRPTrackData RPTrackIt = dynamic_cast< KTRPTrackData >( *ProcessedTrackIt );
            
            track.Component = RPTrackIt->fProcTrack.GetComponent();
            track.AcquisitionID = RPTrackIt->fProcTrack.GetAcquisitionID();
            track.TrackID = RPTrackIt->fProcTrack.GetTrackID();
            track.EventID = event.EventID; // Get the Event ID from the Event, not from the Track
            track.EventSequenceID = RPTrackIt->fProcTrack.GetEventSequenceID();
            track.IsCut = RPTrackIt->fProcTrack.GetIsCut();
            track.StartTimeInAcq = RPTrackIt->fProcTrack.GetStartTimeInAcq();
            track.StartTimeInRunC = RPTrackIt->fProcTrack.GetStartTimeInRunC();
            track.EndTimeInRunC = RPTrackIt->fProcTrack.GetEndTimeInRunC();
            track.TimeLength = RPTrackIt->fProcTrack.GetTimeLength();
            track.StartFrequency = RPTrackIt->fProcTrack.GetStartFrequency();
            track.EndFrequency = RPTrackIt->fProcTrack.GetEndFrequency();
            track.FrequencyWidth = RPTrackIt->fProcTrack.GetFrequencyWidth();
            track.Slope = RPTrackIt->fProcTrack.GetSlope();
            track.Intercept = RPTrackIt->fProcTrack.GetIntercept();
            track.TotalPower = RPTrackIt->fProcTrack.GetTotalPower();
            track.StartTimeInRunCSigma = RPTrackIt->fProcTrack.GetStartTimeInRunCSigma();
            track.EndTimeInRunCSigma = RPTrackIt->fProcTrack.GetEndTimeInRunCSigma();
            track.TimeLengthSigma = RPTrackIt->fProcTrack.GetTimeLengthSigma();
            track.StartFrequencySigma = RPTrackIt->fProcTrack.GetStartFrequencySigma();
            track.EndFrequencySigma = RPTrackIt->fProcTrack.GetEndFrequencySigma();
            track.FrequencyWidthSigma = RPTrackIt->fProcTrack.GetFrequencyWidthSigma();
            track.SlopeSigma = RPTrackIt->fProcTrack.GetSlopeSigma();
            track.InterceptSigma = RPTrackIt->fProcTrack.GetInterceptSigma();
            track.TotalPowerSigma = RPTrackIt->fProcTrack.GetTotalPowerSigma();

            track.IsValid = RPTrackIt->fData->Of< KTPowerFitData >().GetIsValid();
            track.NPeaks = RPTrackIt->fData->Of< KTPowerFitData >().GetNPeaks();
            track.Average = RPTrackIt->fData->Of< KTPowerFitData >().GetAverage();
            track.RMS = RPTrackIt->fData->Of< KTPowerFitData >().GetRMS();
            track.Skewness = RPTrackIt->fData->Of< KTPowerFitData >().GetSkewness();
            track.Kurtosis = RPTrackIt->fData->Of< KTPowerFitData >().GetKurtosis();
            track.NormCentral = RPTrackIt->fData->Of< KTPowerFitData >().GetNormCentral();
            track.MeanCentral = RPTrackIt->fData->Of< KTPowerFitData >().GetMeanCentral();
            track.SigmaCentral = RPTrackIt->fData->Of< KTPowerFitData >().GetSigmaCentral();
            track.MaximumCentral = RPTrackIt->fData->Of< KTPowerFitData >().GetMaximumCentral();
            track.RMSAwayFromCentral = RPTrackIt->fData->Of< KTPowerFitData >().GetRMSAwayFromCentral();
            track.CentralPowerFraction = RPTrackIt->fData->Of< KTPowerFitData >().GetCentralPowerFraction();

            fMTERPTracksDataBuffer.push_back(track);
            KTDEBUG(publog, "Added track " << track.TrackID << "(EventID=" << track.EventID << ")");
        }


        KTDEBUG("Done.");
        return;
    }

    void KTHDF5TypeWriterEventAnalysis::WriteMTERPTracksBuffer()
    {
        if (fMTEDataBuffer.empty())
        {
            KTDEBUG("MTE buffer is empty; no multi-track events written");
            return;
        }

        KTDEBUG("Writing MTE buffer.");
        // Now create the dataspace we need
        hsize_t* dims_cands = new hsize_t(fMTEDataBuffer.size());
        hsize_t* dims_tracks = new hsize_t(fMTERPTracksDataBuffer.size());
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
        H5::DataSet* dset_tracks = new H5::DataSet(candidateTracksGroup->createDataSet(dsetname2.c_str(), *fRPTType, dspace_tracks));
        dset_tracks->write(fMTERPTracksDataBuffer.data(), *fRPTType);

        fMTERPTracksDataBuffer.clear();
        fMTEDataBuffer.clear();
        fFlushMTEIdx++;
    }

    void KTHDF5TypeWriterEventAnalysis::WriteCRPTrackEventData(Nymph::KTDataPtr data) 
    {
        KTDEBUG(publog, "Processing Classified-Track Event");
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
        CRPTData track;
        for (std::set< AllTrackData , TrackTimeComp >::iterator CRPTrackIt = mteData.GetTracksBegin(); CRPTrackIt != mteData.GetTracksEnd(); ++CRPTrackIt)
        {   
            track.Component = CRPTrackIt->fProcTrack.GetComponent();
            track.AcquisitionID = CRPTrackIt->fProcTrack.GetAcquisitionID();
            track.TrackID = CRPTrackIt->fProcTrack.GetTrackID();
            track.EventID = event.EventID; // Get the Event ID from the Event, not from the Track
            track.EventSequenceID = CRPTrackIt->fProcTrack.GetEventSequenceID();
            track.IsCut = CRPTrackIt->fProcTrack.GetIsCut();
            track.StartTimeInAcq = CRPTrackIt->fProcTrack.GetStartTimeInAcq();
            track.StartTimeInRunC = CRPTrackIt->fProcTrack.GetStartTimeInRunC();
            track.EndTimeInRunC = CRPTrackIt->fProcTrack.GetEndTimeInRunC();
            track.TimeLength = CRPTrackIt->fProcTrack.GetTimeLength();
            track.StartFrequency = CRPTrackIt->fProcTrack.GetStartFrequency();
            track.EndFrequency = CRPTrackIt->fProcTrack.GetEndFrequency();
            track.FrequencyWidth = CRPTrackIt->fProcTrack.GetFrequencyWidth();
            track.Slope = CRPTrackIt->fProcTrack.GetSlope();
            track.Intercept = CRPTrackIt->fProcTrack.GetIntercept();
            track.TotalPower = CRPTrackIt->fProcTrack.GetTotalPower();
            track.StartTimeInRunCSigma = CRPTrackIt->fProcTrack.GetStartTimeInRunCSigma();
            track.EndTimeInRunCSigma = CRPTrackIt->fProcTrack.GetEndTimeInRunCSigma();
            track.TimeLengthSigma = CRPTrackIt->fProcTrack.GetTimeLengthSigma();
            track.StartFrequencySigma = CRPTrackIt->fProcTrack.GetStartFrequencySigma();
            track.EndFrequencySigma = CRPTrackIt->fProcTrack.GetEndFrequencySigma();
            track.FrequencyWidthSigma = CRPTrackIt->fProcTrack.GetFrequencyWidthSigma();
            track.SlopeSigma = CRPTrackIt->fProcTrack.GetSlopeSigma();
            track.InterceptSigma = CRPTrackIt->fProcTrack.GetInterceptSigma();
            track.TotalPowerSigma = CRPTrackIt->fProcTrack.GetTotalPowerSigma();

            track.IsValid = CRPTrackIt->fData->Of< KTPowerFitData >().GetIsValid();
            track.NPeaks = CRPTrackIt->fData->Of< KTPowerFitData >().GetNPeaks();
            track.Average = CRPTrackIt->fData->Of< KTPowerFitData >().GetAverage();
            track.RMS = CRPTrackIt->fData->Of< KTPowerFitData >().GetRMS();
            track.Skewness = CRPTrackIt->fData->Of< KTPowerFitData >().GetSkewness();
            track.Kurtosis = CRPTrackIt->fData->Of< KTPowerFitData >().GetKurtosis();
            track.NormCentral = CRPTrackIt->fData->Of< KTPowerFitData >().GetNormCentral();
            track.MeanCentral = CRPTrackIt->fData->Of< KTPowerFitData >().GetMeanCentral();
            track.SigmaCentral = CRPTrackIt->fData->Of< KTPowerFitData >().GetSigmaCentral();
            track.MaximumCentral = CRPTrackIt->fData->Of< KTPowerFitData >().GetMaximumCentral();
            track.RMSAwayFromCentral = CRPTrackIt->fData->Of< KTPowerFitData >().GetRMSAwayFromCentral();
            track.CentralPowerFraction = CRPTrackIt->fData->Of< KTPowerFitData >().GetCentralPowerFraction();

            track.MCH = CRPTrackIt->fData->Of< KTClassifierResultsData >().GetMCH();
            track.MCL = CRPTrackIt->fData->Of< KTClassifierResultsData >().GetMCL();
            track.SB = CRPTrackIt->fData->Of< KTClassifierResultsData >().GetSB();            

            fMTECRPTracksDataBuffer.push_back(track);
            KTDEBUG(publog, "Added track " << track.TrackID << "(EventID=" << track.EventID << ")");
        }


        KTDEBUG("Done.");
        return;
    }

    void KTHDF5TypeWriterEventAnalysis::WriteMTECRPTracksBuffer()
    {
        if (fMTEDataBuffer.empty())
        {
            KTDEBUG("MTE buffer is empty; no multi-track events written");
            return;
        }

        KTDEBUG("Writing MTE buffer.");
        // Now create the dataspace we need
        hsize_t* dims_cands = new hsize_t(fMTEDataBuffer.size());
        hsize_t* dims_tracks = new hsize_t(fMTECRPTracksDataBuffer.size());
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
        H5::DataSet* dset_tracks = new H5::DataSet(candidateTracksGroup->createDataSet(dsetname2.c_str(), *fCRPTType, dspace_tracks));
        dset_tracks->write(fMTECRPTracksDataBuffer.data(), *fCRPTType);

        fMTECRPTracksDataBuffer.clear();
        fMTEDataBuffer.clear();
        fFlushMTEIdx++;
    }

}  //  namespace Katydid
