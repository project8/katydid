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
        fFlushIdx(0) {
            /*
             * First we build the appropriate compound datatype for MTE events
             */
            this->fMTEType = new H5::CompType(MTESize);
            // Insert fields into the type
            for (int f = 0; f < 22; f++) {
                this->fMTEType->insertMember(
                    MTEFieldNames[f],
                    MTEFieldOffsets[f],
                    MTEFieldTypes[f]);
            }
        }

    KTHDF5TypeWriterCandidates::~KTHDF5TypeWriterCandidates() {}

    void KTHDF5TypeWriterCandidates::RegisterSlots() {
        fWriter->RegisterSlot("frequency-candidates", this, &KTHDF5TypeWriterCandidates::WriteFrequencyCandidates);
        fWriter->RegisterSlot("waterfall-candidates", this, &KTHDF5TypeWriterCandidates::WriteWaterfallCandidate);
        fWriter->RegisterSlot("sparse-waterfall-candidates", this, &KTHDF5TypeWriterCandidates::WriteSparseWaterfallCandidate);
        fWriter->RegisterSlot("processed-track", this, &KTHDF5TypeWriterCandidates::WriteProcessedTrack);
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
        KTDEBUG("NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterCandidates::WriteMultiTrackEvent(KTDataPtr data) {
        KTDEBUG(publog, "Processing MTE");
        KTMultiTrackEventData& mteData = data->Of< KTMultiTrackEventData >();

        MTEData event;
        event.Component = mteData.GetComponent();
        event.EventID = mteData.GetEventID();
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

        KTDEBUG("Done.");
        return;
    }

    void KTHDF5TypeWriterCandidates::WriteMTEBuffer() {
        KTDEBUG("writing MTE buffer.");
        // Now create the dataspace we need
        hsize_t* dims = new hsize_t(this->fMTEDataBuffer.size());
        H5::DataSpace dspace(1, dims);

        if( !fWriter->OpenAndVerifyFile() ) return;
        // Make a group
        H5::Group* candidatesGroup = fWriter->AddGroup("candidates");

        // OK, create the dataset and write it down.
        std::stringstream namestream;
        std::string dsetname;
        namestream << "candidates_" << this->fFlushIdx;
        namestream >> dsetname;
        H5::DataSet* dset = new H5::DataSet(candidatesGroup->createDataSet(dsetname.c_str(),
                                                                           *(this->fMTEType),
                                                                           dspace));
        dset->write((this->fMTEDataBuffer).data(),*(this->fMTEType));
        this->fMTEDataBuffer.clear();
        this->fFlushIdx++;
    }

}  //  namespace Katydid
