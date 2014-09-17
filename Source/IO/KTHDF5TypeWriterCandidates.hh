/*
 * KTHDF5TypeWriterCandidates.hh
 *
 *      Created on: 9/13/2014
 *          Author: J.N. Kofron
 */

#ifndef __KTHDF5TWC_HH
#define __KTHDF5TWC_HH

extern "C" {
    #include "hdf5.h"
    #include "hdf5_hl.h"
}

#include "KTHDF5Writer.hh"
#include "KTData.hh"
#include "KTEggHeader.hh"

namespace Katydid {

    /*
     * These are the records we will be writing to the 
     * data table.
     */
    typedef struct {
        unsigned Component;
        unsigned EventID;
        double StartTimeInRunC;
        double EndTimeInRunC;
        double TimeLength;
        double StartFrequency;
        double EndFrequency;
        double MinimumFrequency;
        double MaximumFrequency;
        double FrequencyWidth;
        double StartTimeInRunCSigma;
        double EndTimeInRunCSigma;
        double TimeLengthSigma;
        double StartFrequencySigma;
        double EndFrequencySigma;
        double FrequencyWidthSigma;
        unsigned FirstTrackID;
        double FirstTrackTimeLength;
        double FirstTrackFrequencyWidth;
        double FirstTrackSlope;
        double FirstTrackIntercept;
        double FirstTrackTotalPower;
    } MTEData;

    /*
     * A bunch of calculations we have to do in order to store 
     * candidate data structs in an HDF5 table.  This is all
     * sensitive to the details of the data structures that
     * are defined in KTCandidate.hh.  If the details change, this
     * has to change too.
     */
    size_t MTENFields = 22;
    size_t MTESize = sizeof(MTEData);
    const char* MTEFieldNames[22] = {
        "Component",
        "EventID",
        "StartTimeInRunC",
        "EndTimeInRunC",
        "TimeLength",
        "StartFrequency",
        "EndFrequency",
        "MinimumFrequency",
        "MaximumFrequency",
        "FrequencyWidth",
        "StartTimeInRunCSigma",
        "EndTimeInRunCSigma",
        "TimeLengthSigma",
        "StartFrequencySigma",
        "EndFrequencySigma",
        "FrequencyWidthSigma",
        "FirstTrackID",
        "FirstTrackTimeLength",
        "FirstTrackFrequencyWidth",
        "FirstTrackSlope",
        "FirstTrackIntercept",
        "FirstTrackTotalPower"
    };
    size_t MTEFieldOffsets[22] = {
        HOFFSET(MTEData, Component),
        HOFFSET(MTEData, EventID),
        HOFFSET(MTEData, StartTimeInRunC),
        HOFFSET(MTEData, EndTimeInRunC),
        HOFFSET(MTEData, TimeLength),
        HOFFSET(MTEData, StartFrequency),
        HOFFSET(MTEData, EndFrequency),
        HOFFSET(MTEData, MinimumFrequency),
        HOFFSET(MTEData, MaximumFrequency),
        HOFFSET(MTEData, FrequencyWidth),
        HOFFSET(MTEData, StartTimeInRunCSigma),
        HOFFSET(MTEData, EndTimeInRunCSigma),
        HOFFSET(MTEData, TimeLengthSigma),
        HOFFSET(MTEData, StartFrequencySigma),
        HOFFSET(MTEData, EndFrequencySigma),
        HOFFSET(MTEData, FrequencyWidthSigma),
        HOFFSET(MTEData, FirstTrackID),
        HOFFSET(MTEData, FirstTrackTimeLength),
        HOFFSET(MTEData, FirstTrackFrequencyWidth),
        HOFFSET(MTEData, FirstTrackSlope),
        HOFFSET(MTEData, FirstTrackIntercept),
        HOFFSET(MTEData, FirstTrackTotalPower)
    };

/*
    size_t FreqCanSizes[22] = {
        sizeof(MTEData.Component),
        sizeof(MTEData.EventID),
        sizeof(MTEData.StartTimeInRunC),
        sizeof(MTEData.EndTimeInRunC),
        sizeof(MTEData.TimeLength),
        sizeof(MTEData.StartFrequency),
        sizeof(MTEData.EndFrequency),
        sizeof(MTEData.MinimumFrequency),
        sizeof(MTEData.MaximumFrequency),
        sizeof(MTEData.FrequencyWidth),
        sizeof(MTEData.StartTimeInRunCSigma),
        sizeof(MTEData.EndTimeInRunCSigma),
        sizeof(MTEData.TimeLengthSigma),
        sizeof(MTEData.StartFrequencySigma),
        sizeof(MTEData.EndFrequencySigma),
        sizeof(MTEData.FrequencyWidthSigma),
        sizeof(MTEData.FirstTrackID),
        sizeof(MTEData.FirstTrackTimeLength),
        sizeof(MTEData.FirstTrackFrequencyWidth),
        sizeof(MTEData.FirstTrackSlope),
        sizeof(MTEData.FirstTrackIntercept),
        sizeof(MTEData.FirstTrackTotalPower)
    };
*/

    H5::PredType MTEFieldTypes[22] = {
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE
    };

    class KTHDF5TypeWriterCandidates: public KTHDF5TypeWriter {
        /*
        * The usual constructor/destructor/slot boilerplate
        */
    public:
        KTHDF5TypeWriterCandidates();
        virtual ~KTHDF5TypeWriterCandidates();

        void RegisterSlots();

    public:
        void ProcessEggHeader(KTEggHeader* header);

        void WriteFrequencyCandidates(KTDataPtr data);
        void WriteWaterfallCandidate(KTDataPtr data);
        void WriteSparseWaterfallCandidate(KTDataPtr data);
        void WriteProcessedTrack(KTDataPtr data);
        void WriteMultiTrackEvent(KTDataPtr data);
        void WriteMTEBuffer();

    private:
        std::vector<MTEData> fMTEDataBuffer;
        H5::CompType* fMTEType;

        unsigned fFlushIdx;
    };
};

#endif  // __KTHDF5TWC_HH
