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

#include <string>



using namespace std;
using namespace Katydid;

KTLOGGER(irlog, "ImpulseResponse");

//*********************************
// Definition of KTSineGenerator
//*********************************

namespace Katydid
{
    using namespace Nymph;
    //***********************************
    // Definition of KTImpulseAnalysis
    //***********************************

    class KTImpulseAnalysis : public KTProcessor
    {
        public:
            KTImpulseAnalysis(const std::string& name = "impulse-analysis");
            virtual ~KTImpulseAnalysis();

            bool Configure(const scarab::param_node* node);

            bool Analyze(KTFrequencySpectrumDataPolar& fsData);
            //bool Analyze(KTFrequencySpectrumDataFFTW& fsData);

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
    Nymph::KTApplication* app = NULL;
    try
    {
        app = new Nymph::KTApplication(argc, argv);
    }
    catch( std::exception& e )
    {
        KTERROR( irlog, "Something went wrong while processing the command line:\n" << e.what() );
        return -1;
    }

    const scarab::param_node* parentConfigNode = app->GetConfigurator()->Config();

    // Create and configure the processor toolbox.
    // This will create all of the requested processors, connect their signals and slots, and fill the run queue.
    KTProcessorToolbox procTB;
    if ( ! procTB.Configure( parentConfigNode->NodeAt( procTB.GetConfigName() ) ) )
    {
        KTERROR(irlog, "Unable to configure processor toolbox. Aborting.");
        return -2;
    }

    // Configure the processors
    if ( ! procTB.ConfigureProcessors( parentConfigNode ) )
    {
        KTERROR(irlog, "Unable to configure processors. Aborting.");
        return -4;
    }

    // Execute the run queue!
    bool success = procTB.Run();

    delete app;

    if (! success) return -4;
    return 0;

}

//***************************************
// Implementation of KTImpulseAnalysis
//***************************************

static Nymph::KTNORegistrar< Nymph::KTProcessor, KTImpulseAnalysis > sImpAnalysisRegistrar("impulse-analysis");

KTImpulseAnalysis::KTImpulseAnalysis(const std::string& name) :
        KTProcessor(name),
        fFSDataPolarSlot("fs-polar", this, &KTImpulseAnalysis::Analyze)
{
}

KTImpulseAnalysis::~KTImpulseAnalysis()
{
}

bool KTImpulseAnalysis::Configure(const scarab::param_node*)
{
    return true;
}

bool KTImpulseAnalysis::Analyze(KTFrequencySpectrumDataPolar& fsData)
{
    KTFrequencySpectrumPolar* fs = fsData.GetSpectrumPolar(0);
    unsigned size = fs->size();
    double binWidth = fs->GetBinWidth();

    // Loop over all the bins in the FS
    // Calculate the sum and keep the peak bin information
    unsigned peakBin = 0, previousPeakBin = 0;
    double peakBinValue = -1.;
    double previousPeakValue = -1.;
    double sum = 0.; // sum of squares, since we want to calculate the power
    double value;
    for (unsigned iBin=0; iBin < size; iBin++)
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

    double peakFraction = peakBinValue * peakBinValue / sum;
    double leakagePeakBin = 1. - peakFraction;
    double peakThreeBinFraction = (peakBinValue*peakBinValue + (*fs)(peakBin-1).abs()*(*fs)(peakBin-1).abs() + (*fs)(peakBin+1).abs()*(*fs)(peakBin+1).abs()) / sum;
    double leakagePeakThreeBin = 1. - peakThreeBinFraction;
    double secondHighestBinRatio = previousPeakValue / peakBinValue;
    //KTDEBUG(irlog, peakFraction << "  " << leakagePeakBin << "  " << peakThreeBinFraction << "  " << leakagePeakThreeBin << "  " << peakBinValue << "  " << peakThreeBinValue << "  " << sum);

    // Examine fractional peak width
    double fraction = 0.1;
    unsigned leftSideBin = peakBin, rightSideBin = peakBin;
    if (peakBinValue > 0.)
    {
        double fractionalPowerValue = sqrt(fraction) * peakBinValue;
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

    unsigned fracWidthBins = rightSideBin - leftSideBin + 1;
    double fracWidth = double(fracWidthBins) * binWidth;

    KTPROG(irlog, "Frequency of peak: " << fs->GetBinCenter(peakBin) << " Hz (bin # " << peakBin << ")");
    KTPROG(irlog, "Leakage fraction (1 bin): " << leakagePeakBin);
    KTPROG(irlog, "Leakage fraction (3 bin): " << leakagePeakThreeBin);
    KTPROG(irlog, "Second-highest-bin ratio: " << secondHighestBinRatio << "  (bin # " << previousPeakBin << ")");
    KTPROG(irlog, fraction * 100. << "% width: " << fracWidth << " Hz (" << fracWidthBins << " bins)");

    return true;
}



