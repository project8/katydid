/*
 * KTHDF5TypeWriterEventAnalysis.hh
 *
 *      Created on: 9/13/2014
 *          Author: J.N. Kofron
 */

#ifndef __KTHDF5TYPEWRITEREVENTANALYSIS_HH
#define __KTHDF5TYPEWRITEREVENTANALYSIS_HH

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
        double StartTimeInAcq;
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
    size_t MTENFields = 23;
    size_t MTESize = sizeof(MTEData);
    const char* MTEFieldNames[23] = {
        "Component",
        "EventID",
        "StartTimeInAcq",
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
    size_t MTEFieldOffsets[23] = {
        HOFFSET(MTEData, Component),
        HOFFSET(MTEData, EventID),
        HOFFSET(MTEData, StartTimeInAcq),
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
    size_t FreqCanSizes[23] = {
        sizeof(MTEData.Component),
        sizeof(MTEData.EventID),
        sizeof(MTEData.StartTimeInAcq),
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

    H5::PredType MTEFieldTypes[23] = {
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
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE
    };

    /*
     * Now the same for Processed Track Data
     * Keep an eye on KTProcessedTrackData.hh - if that changes, you need to change the definitions here too
     */ 

    typedef struct {
        unsigned Component;
        unsigned TrackID;
        unsigned EventID;
        bool IsCut;
        double StartTimeInAcq;
        double StartTimeInRunC;
        double EndTimeInRunC;
        double TimeLength;
        double StartFrequency;
        double EndFrequency;
        double FrequencyWidth;
        double Slope;
        double Intercept;
        double TotalPower;
        double StartTimeInRunCSigma;
        double EndTimeInRunCSigma;
        double TimeLengthSigma;
        double StartFrequencySigma;
        double EndFrequencySigma;
        double FrequencyWidthSigma;
        double SlopeSigma;
        double InterceptSigma;
        double TotalPowerSigma;
    } PTData;

    size_t PTNFields = 23;
    size_t PTSize = sizeof(PTData);
    const char* PTFieldNames[23] = {
        "Component",
        "TrackID",
        "EventID",
        "IsCut",
        "StartTimeInAcq",
        "StartTimeInRunC",
        "EndTimeInRunC",
        "TimeLength",
        "StartFrequency",
        "EndFrequency",
        "FrequencyWidth",
        "Slope",
        "Intercept",
        "TotalPower",
        "StartTimeInRunCSigma",
        "EndTimeInRunCSigma",
        "TimeLengthSigma",
        "StartFrequencySigma",
        "EndFrequencySigma",
        "FrequencyWidthSigma",
        "SlopeSigma",
        "InterceptSigma",
        "TotalPowerSigma"
    };
    size_t PTFieldOffsets[23] = {
        HOFFSET(PTData, Component),
        HOFFSET(PTData, TrackID),
        HOFFSET(PTData, EventID),
        HOFFSET(PTData, IsCut),
        HOFFSET(PTData, StartTimeInAcq),
        HOFFSET(PTData, StartTimeInRunC),
        HOFFSET(PTData, EndTimeInRunC),
        HOFFSET(PTData, TimeLength),
        HOFFSET(PTData, StartFrequency),
        HOFFSET(PTData, EndFrequency),
        HOFFSET(PTData, FrequencyWidth),
        HOFFSET(PTData, Slope),
        HOFFSET(PTData, Intercept),
        HOFFSET(PTData, TotalPower),
        HOFFSET(PTData, StartTimeInRunCSigma),
        HOFFSET(PTData, EndTimeInRunCSigma),
        HOFFSET(PTData, TimeLengthSigma),
        HOFFSET(PTData, StartFrequencySigma),
        HOFFSET(PTData, EndFrequencySigma),
        HOFFSET(PTData, FrequencyWidthSigma),
        HOFFSET(PTData, SlopeSigma),
        HOFFSET(PTData, InterceptSigma),
        HOFFSET(PTData, TotalPowerSigma)
    };
    H5::PredType PTFieldTypes[23] = {
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_INT8,
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
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_DOUBLE
    };

    class KTHDF5TypeWriterEventAnalysis: public KTHDF5TypeWriter {
        /*
        * The usual constructor/destructor/slot boilerplate
        */
    public:
        KTHDF5TypeWriterEventAnalysis();
        virtual ~KTHDF5TypeWriterEventAnalysis();

        void RegisterSlots();

    public:
        void ProcessEggHeader(KTEggHeader* header);

        void WriteFrequencyCandidates(KTDataPtr data);
        void WriteWaterfallCandidate(KTDataPtr data);
        void WriteSparseWaterfallCandidate(KTDataPtr data);
        void WriteProcessedTrack(KTDataPtr data);
        void WriteMultiTrackEvent(KTDataPtr data);
        void WriteMTEBuffer();
        void WritePTBuffer();

    private:
        std::vector<MTEData> fMTEDataBuffer;
        H5::CompType* fMTEType;
        std::vector<PTData> fMTETracksDataBuffer;
        std::vector<PTData> fPTDataBuffer;
        H5::CompType* fPTType;

        unsigned fFlushMTEIdx;
        unsigned fFlushPTIdx;
    };
};

#endif  // __KTHDF5TYPEWRITEREVENTANALYSIS_HH
