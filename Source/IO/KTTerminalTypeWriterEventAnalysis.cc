/*
 * KTTerminalTypeWriterEventAnalysis.cc
 *
 *  Created on: Oct 6, 2016
 *      Author: ezayas
 */

#include "KTTerminalTypeWriterEventAnalysis.hh"

#include "KTLogger.hh"
#include "KTProcessedTrackData.hh"
#include "KTLinearFitResult.hh"

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
        fWriter->RegisterSlot("track", this, &KTTerminalTypeWriterEventAnalysis::WriteProcessedTrackData);
        fWriter->RegisterSlot("fit-result", this, &KTTerminalTypeWriterEventAnalysis::WriteLinearFitData);
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

    //********************
    // Linear Fit Result
    //********************

    void KTTerminalTypeWriterEventAnalysis::WriteLinearFitData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        KTLinearFitResult& fitData = data->Of< KTLinearFitResult >();
        stringstream toTerm;

        toTerm << "Printing Linear Fit Result Info" << '\n';

        toTerm << "Slope: "                     << fitData.GetSlope(0)                  << '\n';
//        toTerm << "SlopeSigma: "                << fitData.GetSlopeSigma(0)             << '\n';
        toTerm << "Sideband Intercept: "        << fitData.GetIntercept(0)              << '\n';
        toTerm << "Signal Intercept: "          << fitData.GetIntercept(1)              << '\n';
//        toTerm << "Intercept_deviation: "       << fitData.GetIntercept_deviation(0)    << '\n';
//        toTerm << "StartingFrequency: "         << fitData.GetStartingFrequency(0)      << '\n';
        toTerm << "TrackDuration: "             << fitData.GetTrackDuration(0)          << '\n';
        toTerm << "SidebandSeparation: "        << fitData.GetSidebandSeparation(0)     << '\n';
//        toTerm << "FineProbe_sigma_1: "         << fitData.GetFineProbe_sigma_1(0)      << '\n';
//        toTerm << "FineProbe_sigma_2: "         << fitData.GetFineProbe_sigma_2(0)      << '\n';
//        toTerm << "FineProbe_SNR_1: "           << fitData.GetFineProbe_SNR_1(0)        << '\n';
//        toTerm << "FineProbe_SNR_2: "           << fitData.GetFineProbe_SNR_2(0)        << '\n';
        toTerm << "FFT_peak: "                  << fitData.GetFFT_peak(0)               << '\n';
//        toTerm << "FFT_peak_uncertainty: "      << fitData.GetFFT_peak_uncertainty(0)   << '\n';
//        toTerm << "FFT_sigma: "                 << fitData.GetFFT_sigma(0)              << '\n';
        toTerm << "FFT_SNR: "                   << fitData.GetFFT_SNR(0)                << '\n';
        toTerm << "Fit_width: "                 << fitData.GetFit_width(0)              << '\n';
//        toTerm << "NPoints: "                   << fitData.GetNPoints(0)                << '\n';
//        toTerm << "ProbeWidth: "                << fitData.GetProbeWidth(0)             << '\n';

        KTPROG(termlog, toTerm.str());

        return;
    }

} /* namespace Katydid */
