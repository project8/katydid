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
#include "KTSinusoidGenerator.hh"
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
    //***********************************
    // Definition of KTImpulseAnalysis
    //***********************************

    class KTImpulseAnalysis : public KTProcessor
    {
        public:
            KTImpulseAnalysis(const std::string& name = "impulse-analysis");
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
    KTPStoreNode node = app.GetNode(appConfigName);
    if (! node.IsValid())
    {
        KTERROR(irlog, "Unable to find config node at <" << appConfigName << ">. Aborting.");
        return -2;
    }
    if (! procTB.ConfigureProcessors(&node))
    {
        KTERROR(irlog, "Unable to configure processors. Aborting.");
        return -3;
    }

    // Execute the run queue!
    Bool_t success = procTB.Run();

    if (! success) return -4;
    return 0;

}

//***************************************
// Implementation of KTImpulseAnalysis
//***************************************

static KTDerivedNORegistrar< KTProcessor, KTImpulseAnalysis > sImpAnalysisRegistrar("impulse-analysis");

KTImpulseAnalysis::KTImpulseAnalysis(const std::string& name) :
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
    Double_t binWidth = fs->GetBinWidth();

    // Loop over all the bins in the FS
    // Calculate the sum and keep the peak bin information
    UInt_t peakBin = 0, previousPeakBin = 0;
    Double_t peakBinValue = -1.;
    Double_t previousPeakValue = -1.;
    Double_t sum = 0.; // sum of squares, since we want to calculate the power
    Double_t value;
    for (UInt_t iBin=0; iBin < size; iBin++)
    {
        value = (*fs)(iBin).abs();
        sum += value * value;
        if (value > peakBinValue)
        {
            previousPeakValue = peakBinValue;
            previousPeakBin = peakBin;
            peakBinValue = value;
            peakBin = iBin;
        }
    }

    Double_t peakFraction = peakBinValue * peakBinValue / sum;
    Double_t leakagePeakBin = 1. - peakFraction;
    Double_t peakThreeBinFraction = (peakBinValue*peakBinValue + (*fs)(peakBin-1).abs()*(*fs)(peakBin-1).abs() + (*fs)(peakBin+1).abs()*(*fs)(peakBin+1).abs()) / sum;
    Double_t leakagePeakThreeBin = 1. - peakThreeBinFraction;
    Double_t secondHighestBinRatio = previousPeakValue / peakBinValue;
    //KTDEBUG(irlog, peakFraction << "  " << leakagePeakBin << "  " << peakThreeBinFraction << "  " << leakagePeakThreeBin << "  " << peakBinValue << "  " << peakThreeBinValue << "  " << sum);

    // Examine fractional peak width
    Double_t fraction = 0.1;
    UInt_t leftSideBin = peakBin, rightSideBin = peakBin;
    if (peakBinValue > 0.)
    {
        Double_t fractionalPowerValue = sqrt(fraction) * peakBinValue;
        while (leftSideBin > 0 && (*fs)(leftSideBin).abs() >= fractionalPowerValue)
        {
            leftSideBin--;
        }
        while (rightSideBin < size-1 && (*fs)(rightSideBin).abs() >= fractionalPowerValue)
        {
            rightSideBin++;
        }
        if (leftSideBin != 0 && (*fs)(leftSideBin).abs() < fractionalPowerValue) leftSideBin++; // if we didn't hit the left edge, we went one beyond the FWHM peak
        if (rightSideBin != size-1 && (*fs)(rightSideBin).abs() < fractionalPowerValue) rightSideBin--; // if we didn't heit the right edge, we went one beyond the FWHM peak
    }

    UInt_t fracWidthBins = rightSideBin - leftSideBin + 1;
    Double_t fracWidth = Double_t(fracWidthBins) * binWidth;

    KTPROG(irlog, "Frequency of peak: " << fs->GetBinCenter(peakBin) << " Hz (bin # " << peakBin << ")");
    KTPROG(irlog, "Leakage fraction (1 bin): " << leakagePeakBin);
    KTPROG(irlog, "Leakage fraction (3 bin): " << leakagePeakThreeBin);
    KTPROG(irlog, "Second-highest-bin ratio: " << secondHighestBinRatio << "  (bin # " << previousPeakBin << ")");
    KTPROG(irlog, fraction * 100. << "% width: " << fracWidth << " Hz (" << fracWidthBins << " bins)");

    return true;
}



