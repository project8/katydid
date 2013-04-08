/*
 * ImpulseResponse.cc
 *
 *  Created on: Apr 8, 2013
 *      Author: nsoblath
 */

#include "KTApplication.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPrimaryProcessor.hh"
#include "KTProcessorToolbox.hh"
#include "KTSliceHeader.hh"
#include "KTSlot.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include <boost/shared_ptr.hpp>

#include <string>

using boost::shared_ptr;

using namespace std;
using namespace Katydid;

KTLOGGER(irlog, "katydid.applications.profiling");

//*********************************
// Definition of KTSineGenerator
//*********************************

namespace Katydid
{
    class KTSineGenerator : public KTPrimaryProcessor
    {
        public:
        enum TimeSeriesType
        {
            kRealTimeSeries,
            kFFTWTimeSeries
        };

        public:
            KTSineGenerator(const std::string& name = "sine-generator");
            virtual ~KTSineGenerator();

            Bool_t Configure(const KTPStoreNode* node);

            Bool_t Run();

            UInt_t GetSliceSizeRequest() const;
            void SetSliceSizeRequest(UInt_t size);

            TimeSeriesType GetTimeSeriesType() const;
            void SetTimeSeriesType(TimeSeriesType type);

        private:
            UInt_t fSliceSizeRequest;

            TimeSeriesType fTimeSeriesType;

            //***************
            // Signals
            //***************

        private:
            KTSignalOneArg< const KTEggHeader* > fHeaderSignal;
            KTSignalData fDataSignal;
            KTSignalOneArg< void > fDoneSignal;
    };
}


//*********************
// The main function
//*********************

int main(int argc, char** argv)
{
    KTApplication app(argc, argv);
    app.ReadConfigFile();

    // Create and configure the processor toolbox.
    // This will create all of the requested processors, connect their signals and slots, and fill the run queue.
    string appConfigName("impulse_response");
    KTProcessorToolbox procTB;
    if (! app.Configure(&procTB, appConfigName))
    {
        KTERROR(irlog, "Unable to configure processor toolbox. Aborting.");
        return -1;
    }

    // Configure the processors
    if (! procTB.ConfigureProcessors(app.GetNode(appConfigName)))
    {
        KTERROR(irlog, "Unable to configure processors. Aborting.");
        return -2;
    }

    // Execute the run queue!
    Bool_t success = procTB.Run();

    if (! success) return -3;
    return 0;

}

//*************************************
// Implementation of KTSineGenerator
//*************************************

static KTDerivedRegistrar< KTProcessor, KTSineGenerator > sEggProcRegistrar("sine-generator");

KTSineGenerator::KTSineGenerator(const std::string& name) :
        KTPrimaryProcessor(name),
        fSliceSizeRequest(0),
        fTimeSeriesType(kRealTimeSeries),
        fHeaderSignal("header", this),
        fDataSignal("slice", this),
        fDoneSignal("done", this)
{
}

KTSineGenerator::~KTSineGenerator()
{
}

Bool_t KTSineGenerator::Configure(const KTPStoreNode* node)
{
    // Config-file settings
    if (node != NULL)
    {
        // specify the length of the time series
        fSliceSizeRequest = node->GetData< UInt_t >("time-series-size", fSliceSizeRequest);

        // type of time series
        string timeSeriesTypeString = node->GetData< string >("time-series-type", "real");
        if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
        else if (timeSeriesTypeString == "fftw") SetTimeSeriesType(kFFTWTimeSeries);
        else
        {
            KTERROR(irlog, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
            return false;
        }
    }

    return true;
}

Bool_t KTSineGenerator::Run()
{
    const UInt_t nBins = 1024;
    const Double_t startTime = 0.;
    const Double_t endTime = 10.;

    const Double_t mult = 30.;

    KTINFO(irlog, "Time series characteristics:\n"
           "\tSize: " << nBins << " bins\n"
           "\tRange: " << startTime << " to " << endTime << " s\n"
           "\tSine wave frequency: " << mult / (2.*KTMath::Pi()) << " Hz\n");

    KTTimeSeriesReal* timeSeries = new KTTimeSeriesReal(nBins, startTime, endTime);

    // Fill the time series with a sinusoid.
    // The units are volts.
    for (UInt_t iBin=0; iBin<nBins; iBin++)
    {
        (*timeSeries)(iBin) = sin(timeSeries->GetBinCenter(iBin) * mult);
        //KTDEBUG(vallog, iBin << "  " << (*timeSeries)(iBin));
    }

    boost::shared_ptr<KTData> newData(new KTData());

    KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >().SetNComponents(1);
    sliceHeader.SetSampleRate(fHeader.GetAcquisitionRate());
    sliceHeader.SetSliceSize(nBins);
    sliceHeader.CalculateBinWidthAndSliceLength();
    sliceHeader.SetTimeInRun(0);
    sliceHeader.SetSliceNumber(0);

    KTTimeSeriesData& tsData = newData->Of< KTTimeSeriesData >().SetNComponents(1);
    tsData.SetTimeSeries(timeSeries, 0);

    fDataSignal(newData);
    return true;
}
