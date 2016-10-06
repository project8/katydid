/*
 * KTTerminalTypeWriterEventAnalysis.cc
 *
 *  Created on: Oct 6, 2016
 *      Author: ezayas
 */

#include "KTTerminalTypeWriterEventAnalysis.hh"

#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTProcSummary.hh"
#include "KTProcessedTrackData.hh"

#include <algorithm> // for max
#include <cstdio> // for sprintf
#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    static Nymph::KTTIRegistrar< KTTerminalTypeWriter, KTTerminalTypeWriterEventAnalysis > sTermTWERegistrar;

    KTTerminalTypeWriterEventAnalysis::KTTerminalTypeWriterEventAnalysis() :
            KTTerminalTypeWriter()
    {
    }

    KTTerminalTypeWriterEventAnalysis::~KTTerminalTypeWriterEventAnalysis()
    {
    }


    void KTTerminalTypeWriterEventAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("proc-track", this, &KTTerminalTypeWriterEventAnalysis::WriteProcessedTrackData);
        fWriter->RegisterSlot("summary", this, &KTTerminalTypeWriterEventAnalysis::WriteProcSummary);
        return;
    }

    //**********************
    // Processed Track Data
    //**********************

    void KTTerminalTypeWriterEventAnalysis::WriteProcessedTrackData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        KTProcessedTrackData& trackData = data->Of< KTProcessedTrackData >();
        stringstream toTerm;

        toTerm << "Printing Processed Track Data Info" << '\n';

        toTerm << "Component: "             << trackData.GetComponent()             << '\n';
        toTerm << "TrackID: "               << trackData.GetTrackID()               << '\n';
        toTerm << "EventID: "               << trackData.GetEventID()               << '\n';
        toTerm << "EventSequenceID: "       << trackData.GetEventSequenceID()       << '\n';
        toTerm << "IsCut: "                 << trackData.GetIsCut()                 << '\n';
        toTerm << "StartTimeInAcq: "        << trackData.GetStartTimeInAcq()        << '\n';
        toTerm << "StartTimeInRunC: "       << trackData.GetStartTimeInRunC()       << '\n';
        toTerm << "EndTimeInRunC: "         << trackData.GetEndTimeInRunC()         << '\n';
        toTerm << "TimeLength: "            << trackData.GetTimeLength()            << '\n';
        toTerm << "StartFrequency: "        << trackData.GetStartFrequency()        << '\n';
        toTerm << "EndFrequency: "          << trackData.GetEndFrequency()          << '\n';
        toTerm << "FrequencyWidth: "        << trackData.GetFrequencyWidth()        << '\n';
        toTerm << "Slope: "                 << trackData.GetSlope()                 << '\n';
        toTerm << "Intercept: "             << trackData.GetIntercept()             << '\n';
        toTerm << "TotalPower: "            << trackData.GetTotalPower()            << '\n';
//        toTerm << "StartTimeInRunCSigma: "  << trackData.GetStartTimeInRunCSigma()  << '\n';
//        toTerm << "EndTimeInRunCSigma: "    << trackData.GetEndTimeInRunCSigma()    << '\n';
//        toTerm << "TimeLengthSigma: "       << trackData.GetTimeLengthSigma()       << '\n';
//        toTerm << "StartFrequencySigma: "   << trackData.GetStartFrequencySigma()   << '\n';
//        toTerm << "EndFrequencySigma: "     << trackData.GetEndFrequencySigma()     << '\n';
//        toTerm << "FrequencyWidthSigma: "   << trackData.GetFrequencyWidthSigma()   << '\n';
//        toTerm << "SlopeSigma: "            << trackData.GetSlopeSigma()            << '\n';
//        toTerm << "InterceptSigma: "        << trackData.GetInterceptSigma()        << '\n';
//        toTerm << "TotalPowerSigma: "       << trackData.GetTotalPowerSigma()       << '\n';
        
        KTPROG(termlog, toTerm.str());

        return;
    }


    //*******************
    // Processor Summary
    //*******************

    void KTTerminalTypeWriterEventAnalysis::WriteProcSummary(const KTProcSummary* summary)
    {
        if (summary == NULL) return;

        KTPROG(termlog, "Processing Summary:");
        KTPROG(termlog, "\tNumber of slices processed: " << summary->GetNSlicesProcessed());
        KTPROG(termlog, "\tNumber of records processed: " << summary->GetNRecordsProcessed());
        KTPROG(termlog, "\tIntegrated time processed: " << summary->GetIntegratedTime());

        return;
    }



} /* namespace Katydid */
