/*
 * KTHDF5TypeWriterEventAnalysis.hh
 *
 *      Created on: 9/13/2014
 *          Author: J.N. Kofron
 */

#ifndef __KTHDF5TYPEWRITEREVENTANALYSIS_HH
#define __KTHDF5TYPEWRITEREVENTANALYSIS_HH

extern "C"
{
    #include "hdf5.h"
    #include "hdf5_hl.h"
}

#include "KTHDF5Writer.hh"
#include "KTData.hh"
#include "KTEggHeader.hh"

namespace Katydid
{

    /*
     * These are the records we will be writing to the 
     * data table.
     */
    typedef struct
    {
        unsigned Component;
        uint64_t AcquisitionID;
        unsigned EventID;
        unsigned TotalEventSequences;
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
        bool UnknownEventTopology;
    } MTEData;


    /*
     * A bunch of calculations we have to do in order to store 
     * candidate data structs in an HDF5 table.  This is all
     * sensitive to the details of the data structures that
     * are defined in KTCandidate.hh.  If the details change, this
     * has to change too.
     */
    const size_t MTENFields = 26;
    size_t MTESize = sizeof(MTEData);
    const char* MTEFieldNames[MTENFields] =
    {
        "Component",
        "AcquisitionID",
        "EventID",
        "TotalEventSequences",
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
        "FirstTrackTotalPower",
        "UnknownEventTopology"
    };
    size_t MTEFieldOffsets[MTENFields] =
    {
        HOFFSET(MTEData, Component),
        HOFFSET(MTEData, AcquisitionID),
        HOFFSET(MTEData, EventID),
        HOFFSET(MTEData, TotalEventSequences),
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
        HOFFSET(MTEData, FirstTrackTotalPower),
        HOFFSET(MTEData, UnknownEventTopology)
    };


    H5::PredType MTEFieldTypes[MTENFields] =
    {
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_UINT64,
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
        H5::PredType::NATIVE_DOUBLE,
        H5::PredType::NATIVE_INT8
    };

    /*
     * Now the same for Processed Track Data
     * Keep an eye on KTProcessedTrackData.hh - if that changes, you need to change the definitions here too
     */ 

    typedef struct
    {
        unsigned Component;
        uint64_t AcquisitionID;
        unsigned TrackID;
        unsigned EventID;
        unsigned EventSequenceID;
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

    const size_t PTNFields = 25;
    size_t PTSize = sizeof(PTData);
    const char* PTFieldNames[PTNFields] =
    {
        "Component",
        "AcquisitionID",
        "TrackID",
        "EventID",
        "EventSequenceID",
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
    size_t PTFieldOffsets[PTNFields] =
    {
        HOFFSET(PTData, Component),
        HOFFSET(PTData, AcquisitionID),
        HOFFSET(PTData, TrackID),
        HOFFSET(PTData, EventID),
        HOFFSET(PTData, EventSequenceID),
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
    H5::PredType PTFieldTypes[PTNFields] =
    {
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_UINT64,
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

    // Now the same for KTPowerFitData

    typedef struct 
    {
        int IsValid;
        int NPeaks;
        double Average;
        double RMS;
        double Skewness;
        double Kurtosis;
        double NormCentral;
        double MeanCentral;
        double SigmaCentral;
        double MaximumCentral;
        double RMSAwayFromCentral;
        double CentralPowerFraction;
        unsigned TrackID;
    } PFData;

    const size_t PFNFields = 13;
    size_t PFSize = sizeof(PFData);
    const char* PFFieldNames[PFNFields] = 
    {
        "IsValid",
        "NPeaks",
        "Average",
        "RMS",
        "Skewness",
        "Kurtosis",
        "NormCentral",
        "MeanCentral",
        "SigmaCentral",
        "MaximumCentral",
        "RMSAwayFromCentral",
        "CentralPowerFraction",
        "TrackID"
    };
    size_t PFFieldOffsets[PFNFields] = 
    {
        HOFFSET(PFData, IsValid),
        HOFFSET(PFData, NPeaks),
        HOFFSET(PFData, Average),
        HOFFSET(PFData, RMS),
        HOFFSET(PFData, Skewness),
        HOFFSET(PFData, Kurtosis),
        HOFFSET(PFData, NormCentral),
        HOFFSET(PFData, MeanCentral),
        HOFFSET(PFData, SigmaCentral),
        HOFFSET(PFData, MaximumCentral),
        HOFFSET(PFData, RMSAwayFromCentral),
        HOFFSET(PFData, CentralPowerFraction),
        HOFFSET(PFData, TrackID)
    };
    H5::PredType PFFieldTypes[PFNFields] = 
    {
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
        H5::PredType::NATIVE_UINT
    };

    // Rotate-Project-Track-Data

    typedef struct 
    {
        // processed track stuff

        unsigned Component;
        uint64_t AcquisitionID;
        unsigned TrackID;
        unsigned EventID;
        unsigned EventSequenceID;
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

        // additional power-fit results

        int IsValid;
        int NPeaks;
        double Average;
        double RMS;
        double Skewness;
        double Kurtosis;
        double NormCentral;
        double MeanCentral;
        double SigmaCentral;
        double MaximumCentral;
        double RMSAwayFromCentral;
        double CentralPowerFraction;

    } RPTData;

    const size_t RPTNFields = 37;
    size_t RPTSize = sizeof(RPTData);
    const char* RPTFieldNames[RPTNFields] = 
    {
        "Component",
        "AcquisitionID",
        "TrackID",
        "EventID",
        "EventSequenceID",
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
        "TotalPowerSigma",
        "IsValid",
        "NPeaks",
        "Average",
        "RMS",
        "Skewness",
        "Kurtosis",
        "NormCentral",
        "MeanCentral",
        "SigmaCentral",
        "MaximumCentral",
        "RMSAwayFromCentral",
        "CentralPowerFraction"
    };
    size_t RPTFieldOffsets[RPTNFields] = 
    {
        HOFFSET(RPTData, Component),
        HOFFSET(RPTData, AcquisitionID),
        HOFFSET(RPTData, TrackID),
        HOFFSET(RPTData, EventID),
        HOFFSET(RPTData, EventSequenceID),
        HOFFSET(RPTData, IsCut),
        HOFFSET(RPTData, StartTimeInAcq),
        HOFFSET(RPTData, StartTimeInRunC),
        HOFFSET(RPTData, EndTimeInRunC),
        HOFFSET(RPTData, TimeLength),
        HOFFSET(RPTData, StartFrequency),
        HOFFSET(RPTData, EndFrequency),
        HOFFSET(RPTData, FrequencyWidth),
        HOFFSET(RPTData, Slope),
        HOFFSET(RPTData, Intercept),
        HOFFSET(RPTData, TotalPower),
        HOFFSET(RPTData, StartTimeInRunCSigma),
        HOFFSET(RPTData, EndTimeInRunCSigma),
        HOFFSET(RPTData, TimeLengthSigma),
        HOFFSET(RPTData, StartFrequencySigma),
        HOFFSET(RPTData, EndFrequencySigma),
        HOFFSET(RPTData, FrequencyWidthSigma),
        HOFFSET(RPTData, SlopeSigma),
        HOFFSET(RPTData, InterceptSigma),
        HOFFSET(RPTData, TotalPowerSigma),
        HOFFSET(RPTData, IsValid),
        HOFFSET(RPTData, NPeaks),
        HOFFSET(RPTData, Average),
        HOFFSET(RPTData, RMS),
        HOFFSET(RPTData, Skewness),
        HOFFSET(RPTData, Kurtosis),
        HOFFSET(RPTData, NormCentral),
        HOFFSET(RPTData, MeanCentral),
        HOFFSET(RPTData, SigmaCentral),
        HOFFSET(RPTData, MaximumCentral),
        HOFFSET(RPTData, RMSAwayFromCentral),
        HOFFSET(RPTData, CentralPowerFraction)
    };
    H5::PredType RPTFieldTypes[RPTNFields] = 
    {
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_UINT64,
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
        H5::PredType::NATIVE_DOUBLE,
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
        H5::PredType::NATIVE_DOUBLE
    };

    // Classified Rotate-Project-Track-Data

    typedef struct 
    {
        // processed track stuff

        unsigned Component;
        uint64_t AcquisitionID;
        unsigned TrackID;
        unsigned EventID;
        unsigned EventSequenceID;
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

        // additional power-fit results

        int IsValid;
        int NPeaks;
        double Average;
        double RMS;
        double Skewness;
        double Kurtosis;
        double NormCentral;
        double MeanCentral;
        double SigmaCentral;
        double MaximumCentral;
        double RMSAwayFromCentral;
        double CentralPowerFraction;

        // classification label
        int MCH;
        int MCL;
        int SB; 

    } CRPTData;

    const size_t CRPTNFields = 40;
    size_t CRPTSize = sizeof(CRPTData);
    const char* CRPTFieldNames[CRPTNFields] = 
    {
        "Component",
        "AcquisitionID",
        "TrackID",
        "EventID",
        "EventSequenceID",
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
        "TotalPowerSigma",
        "IsValid",
        "NPeaks",
        "Average",
        "RMS",
        "Skewness",
        "Kurtosis",
        "NormCentral",
        "MeanCentral",
        "SigmaCentral",
        "MaximumCentral",
        "RMSAwayFromCentral",
        "CentralPowerFraction",
        "MCH",
        "MCL",
        "SB"
    };

    size_t CRPTFieldOffsets[CRPTNFields] = 
    {
        HOFFSET(CRPTData, Component),
        HOFFSET(CRPTData, AcquisitionID),
        HOFFSET(CRPTData, TrackID),
        HOFFSET(CRPTData, EventID),
        HOFFSET(CRPTData, EventSequenceID),
        HOFFSET(CRPTData, IsCut),
        HOFFSET(CRPTData, StartTimeInAcq),
        HOFFSET(CRPTData, StartTimeInRunC),
        HOFFSET(CRPTData, EndTimeInRunC),
        HOFFSET(CRPTData, TimeLength),
        HOFFSET(CRPTData, StartFrequency),
        HOFFSET(CRPTData, EndFrequency),
        HOFFSET(CRPTData, FrequencyWidth),
        HOFFSET(CRPTData, Slope),
        HOFFSET(CRPTData, Intercept),
        HOFFSET(CRPTData, TotalPower),
        HOFFSET(CRPTData, StartTimeInRunCSigma),
        HOFFSET(CRPTData, EndTimeInRunCSigma),
        HOFFSET(CRPTData, TimeLengthSigma),
        HOFFSET(CRPTData, StartFrequencySigma),
        HOFFSET(CRPTData, EndFrequencySigma),
        HOFFSET(CRPTData, FrequencyWidthSigma),
        HOFFSET(CRPTData, SlopeSigma),
        HOFFSET(CRPTData, InterceptSigma),
        HOFFSET(CRPTData, TotalPowerSigma),
        HOFFSET(CRPTData, IsValid),
        HOFFSET(CRPTData, NPeaks),
        HOFFSET(CRPTData, Average),
        HOFFSET(CRPTData, RMS),
        HOFFSET(CRPTData, Skewness),
        HOFFSET(CRPTData, Kurtosis),
        HOFFSET(CRPTData, NormCentral),
        HOFFSET(CRPTData, MeanCentral),
        HOFFSET(CRPTData, SigmaCentral),
        HOFFSET(CRPTData, MaximumCentral),
        HOFFSET(CRPTData, RMSAwayFromCentral),
        HOFFSET(CRPTData, CentralPowerFraction),
        HOFFSET(CRPTData, MCH),
        HOFFSET(CRPTData, MCL),
        HOFFSET(CRPTData, SB)
    };
    H5::PredType CRPTFieldTypes[CRPTNFields] = 
    {
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_UINT64,
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
        H5::PredType::NATIVE_DOUBLE,
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
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_UINT,
        H5::PredType::NATIVE_UINT
    };

    class KTHDF5TypeWriterEventAnalysis: public KTHDF5TypeWriter
    {
        /*
        * The usual constructor/destructor/slot boilerplate
        */
    public:
        KTHDF5TypeWriterEventAnalysis();
        virtual ~KTHDF5TypeWriterEventAnalysis();

        void RegisterSlots();

    public:
        void ProcessEggHeader(KTEggHeader* header);

        //void WriteFrequencyCandidates(Nymph::KTDataPtr data);
        //void WriteWaterfallCandidate(Nymph::KTDataPtr data);
        //void WriteSparseWaterfallCandidate(Nymph::KTDataPtr data);
        void WriteProcessedTrack(Nymph::KTDataPtr data);
        void WriteMultiTrackEvent(Nymph::KTDataPtr data);
        void WritePowerFitData(Nymph::KTDataPtr data);
        void WriteRPTrackEventData(Nymph::KTDataPtr data);
        void WriteCRPTrackEventData(Nymph::KTDataPtr data);
        void WriteMTEBuffer();
        void WritePTBuffer();
        void WritePFBuffer();
        void WriteMTERPTracksBuffer();
        void WriteMTECRPTracksBuffer();

    private:
        std::vector<MTEData> fMTEDataBuffer;
        H5::CompType* fMTEType;
        std::vector<PTData> fMTETracksDataBuffer;
        std::vector<PTData> fPTDataBuffer;
        H5::CompType* fPTType;
        std::vector<PFData> fPFDataBuffer;
        H5::CompType* fPFType;
        std::vector<RPTData> fMTERPTracksDataBuffer;
        std::vector<RPTData> fRPTDataBuffer;
        H5::CompType* fRPTType;
        std::vector<CRPTData> fMTECRPTracksDataBuffer;
        H5::CompType* fCRPTType;
        

        unsigned fFlushMTEIdx;
        unsigned fFlushPTIdx;
        unsigned fFlushPFIdx;
        unsigned fFlushRPTIdx;
    };
};

#endif  // __KTHDF5TYPEWRITEREVENTANALYSIS_HH
