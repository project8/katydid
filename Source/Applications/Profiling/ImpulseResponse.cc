/*
 * ImpulseResponse.cc
 *
 *  Created on: Apr 8, 2013
 *      Author: nsoblath
 */

#include "KTApplication.hh"
#include "KTEggHeader.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPrimaryProcessor.hh"
#include "KTProcessorToolbox.hh"
#include "KTSliceHeader.hh"
#include "KTSlot.hh"
#include "KTTimeSeriesReal.hh"
#include "KTTimeSeriesFFTW.hh"
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
    /*!
     @class KTSineGenerator
     @author N. S. Oblath

     @brief Generates a single sine-wave time series

     @details
     Iterates over slices in an egg file; slices are extracted until fNSlices is reached.

     Available configuration options:
     \li \c "time-series-size": UInt_t -- Specify the size of the time series
     \li \c "bin-width": Double_t -- Specify the bin width
     \li \c "time-series-type": string -- Type of time series to produce (options: real [default], fftw [not available with the 2011 egg reader])
     \li \c "signal-frequency": Double_t -- Frequency of the sinusoid

     Signals:
     \li \c "header": void (const KTEggHeader*) -- emitted when the file header is parsed.
     \li \c "slice": void (boost::shared_ptr<KTData>) -- emitted when the new time series is produced.
     \li \c "done": void () --  emitted when the job is complete.
    */
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

            UInt_t GetSliceSize() const;
            void SetSliceSize(UInt_t size);

            Double_t GetBinWidth() const;
            void SetBinWidth(Double_t bw);

            TimeSeriesType GetTimeSeriesType() const;
            void SetTimeSeriesType(TimeSeriesType type);

            Double_t GetSignalFrequency() const;
            void SetSignalFrequency(Double_t freq);

        private:
            UInt_t fSliceSize;
            Double_t fBinWidth;

            TimeSeriesType fTimeSeriesType;

            Double_t fSignalFreq;

            //***************
            // Signals
            //***************

        private:
            KTSignalOneArg< const KTEggHeader* > fHeaderSignal;
            KTSignalData fDataSignal;
            KTSignalOneArg< void > fDoneSignal;
    };


    class KTImpulseAnalysis : public KTProcessor
    {
        public:
            KTImpulseAnalysis();
            virtual ~KTImpulseAnalysis();

            Bool_t Configure(const KTPStoreNode* node);

            Bool_t Analyze(KTFrequencySpectrumDataPolar& fsData);
            //Bool_t Analyze(KTFrequencySpectrumDataFFTW& fsData);

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSDataPolarSlot;
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
    string appConfigName("impulse-response");
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
        fSliceSize(1024),
        fBinWidth(5.e-9),
        fTimeSeriesType(kRealTimeSeries),
        fSignalFreq(50.e6),
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
        fSliceSize = node->GetData< UInt_t >("time-series-size", fSliceSize);
        fBinWidth = node->GetData< Double_t >("bin-width", fBinWidth);

        // type of time series
        string timeSeriesTypeString = node->GetData< string >("time-series-type", "real");
        if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
        else if (timeSeriesTypeString == "fftw") SetTimeSeriesType(kFFTWTimeSeries);
        else
        {
            KTERROR(irlog, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
            return false;
        }

        // specify the frequency of the signal
        fSignalFreq = node->GetData< Double_t >("signal-frequency", fSignalFreq);
    }

    return true;
}

Bool_t KTSineGenerator::Run()
{
    const Double_t startTime = 0.;
    const Double_t endTime = fSliceSize * fBinWidth;

    const Double_t mult = 2. * KTMath::Pi() * fSignalFreq;

    KTINFO(irlog, "Time series characteristics:\n" <<
           "\tSize: " << fSliceSize << " bins\n" <<
           "\tBin width: " << fBinWidth << " s\n" <<
           "\tSample rate: " << 1. / fBinWidth << " Hz\n" <<
           "\tRange: " << startTime << " to " << endTime << " s\n" <<
           "\tSine wave frequency: " << fSignalFreq << " Hz\n");

    boost::shared_ptr<KTData> newData(new KTData());

    KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >().SetNComponents(1);
    sliceHeader.SetSampleRate(1. / fBinWidth);
    sliceHeader.SetSliceSize(fSliceSize);
    sliceHeader.CalculateBinWidthAndSliceLength();
    sliceHeader.SetTimeInRun(0);
    sliceHeader.SetSliceNumber(0);

    KTTimeSeries* timeSeries = NULL;
    if (fTimeSeriesType == kRealTimeSeries)
    {
        timeSeries = new KTTimeSeriesReal(fSliceSize, startTime, endTime);
    }
    else
    {
        timeSeries = new KTTimeSeriesFFTW(fSliceSize, startTime, endTime);
    }

    // Fill the time series with a sinusoid.
    // The units are volts.
    Double_t binCenter = 0.5 * fBinWidth;
    for (UInt_t iBin=0; iBin<fSliceSize; iBin++)
    {
        timeSeries->SetValue(iBin, sin(binCenter * mult));
        binCenter += fBinWidth;
        //KTDEBUG(irlog, iBin << "  " << (*timeSeries)(iBin));
    }

    KTTimeSeriesData& tsData = newData->Of< KTTimeSeriesData >().SetNComponents(1);
    tsData.SetTimeSeries(timeSeries, 0);

    fDataSignal(newData);
    return true;
}


UInt_t KTSineGenerator::GetSliceSize() const
{
    return fSliceSize;
}

void KTSineGenerator::SetSliceSize(UInt_t size)
{
    fSliceSize = size;
    return;
}


Double_t KTSineGenerator::GetBinWidth() const
{
    return fBinWidth;
}

void KTSineGenerator::SetBinWidth(Double_t bw)
{
    fBinWidth = bw;
    return;
}


KTSineGenerator::TimeSeriesType KTSineGenerator::GetTimeSeriesType() const
{
    return fTimeSeriesType;
}

void KTSineGenerator::SetTimeSeriesType(TimeSeriesType type)
{
    fTimeSeriesType = type;
    return;
}


Double_t KTSineGenerator::GetSignalFrequency() const
{
    return fSignalFreq;
}

void KTSineGenerator::SetSignalFrequency(Double_t freq)
{
    fSignalFreq = freq;
    return;
}


KTImpulseAnalysis::KTImpulseAnalysis() :
        KTProcessor(),
        fFSDataPolarSlot("fs-polar", this, &KTImpulseAnalysis::Analyze)
{
}

KTImpulseAnalysis::~KTImpulseAnalysis()
{
}

Bool_t KTImpulseAnalysis::Configure(const KTPStoreNode* node)
{
    return true;
}

Bool_t KTImpulseAnalysis::Analyze(KTFrequencySpectrumDataPolar& fsData)
{
    KTFrequencySpectrumPolar* fs = fsData.GetSpectrumPolar(0);
    UInt_t size = fs->size();

    // Loop over all the bins in the FS
    // Calculate the sum and keep the peak bin information
    UInt_t peakBin;
    Double_t peakBinValue = 0.;
    Double_t previousPeakValue;
    Double_t sum = 0.; // sum of squares, since we want to calculate the power
    Double_t value;
    for (UInt_t iBin=0; iBin < size; iBin++)
    {
        value = (*fs)(iBin).abs();
        sum += value * value;
        if (value > peakBinValue)
        {
            previousPeakValue = peakBinValue;
            peakBinValue = value;
            peakBin = iBin;
        }
    }

    // Subtract the peak bin from the sum
    //sum -= peakBinValue;

    Double_t peakFraction = peakBinValue * peakBinValue / sum;
    Double_t leakage = 1. - peakFraction;
    Double_t secondHighestBinRatio = previousPeakValue / peakBinValue;

    KTPROG(irlog, "Leakage fraction: " << leakage);
    KTPROG(irlog, "Second-highest-bin ratio: " << secondHighestBinRatio);

    return true;
}



