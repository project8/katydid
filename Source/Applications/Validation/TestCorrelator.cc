/*
 * TestCorrelator.cc
 *
 *  Created on: Sep 5, 2012
 *      Author: nsoblath
 */

#include "complexpolar.hh"
#include "KTCorrelator.hh"
#include "KTCorrelationData.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTLogger.hh"

using namespace Katydid;

KTLOGGER(corrtestlog, "katydid.applications.validation");

int main(int argc, char** argv)
{
    // Prepare the input data

    // KTFrequencySpectrumData option
    /*
    KTFrequencySpectrumData* dataInput = new KTFrequencySpectrumData(2);

    KTFrequencySpectrum* spectrum0 = new KTFrequencySpectrum(10, 0, 10);
    (*spectrum0)(0) = 0.;
    (*spectrum0)(1) = 0.;
    (*spectrum0)(2) = 1.;
    (*spectrum0)(3) = 2.;
    (*spectrum0)(4) = 3.;
    (*spectrum0)(5) = 4.;
    (*spectrum0)(6) = 5.;
    (*spectrum0)(7) = 6.;
    (*spectrum0)(8) = complexpolar<double>(0., 1.);
    (*spectrum0)(9) = complexpolar<double>(0., 10.);

    KTFrequencySpectrum* spectrum1 = new KTFrequencySpectrum(10, 0, 10);
    (*spectrum1)(0) = 0.;
    (*spectrum1)(1) = 0.;
    (*spectrum1)(2) = 0.;
    (*spectrum1)(3) = 5.;
    (*spectrum1)(4) = 5.;
    (*spectrum1)(5) = 5.;
    (*spectrum1)(6) = 5.;
    (*spectrum1)(7) = 0.;
    (*spectrum1)(8) = complexpolar<double>(0., 2.);
    (*spectrum1)(9) = complexpolar<double>(0., 2.);
    */

    // KTFrequencySpectrumDataFFTW option
    /**/
    KTFrequencySpectrumDataFFTW* dataInput = new KTFrequencySpectrumDataFFTW(2);

    KTFrequencySpectrumFFTW* spectrum0 = new KTFrequencySpectrumFFTW(10, 0, 10);
    (*spectrum0)(0)[0] = 0.; (*spectrum0)(0)[1] = 0.;
    (*spectrum0)(1)[0] = 0.; (*spectrum0)(1)[1] = 0.;
    (*spectrum0)(2)[0] = 1.; (*spectrum0)(2)[1] = 0.;
    (*spectrum0)(3)[0] = 2.; (*spectrum0)(3)[1] = 0.;
    (*spectrum0)(4)[0] = 3.; (*spectrum0)(4)[1] = 0.;
    (*spectrum0)(5)[0] = 4.; (*spectrum0)(5)[1] = 0.;
    (*spectrum0)(6)[0] = 5.; (*spectrum0)(6)[1] = 0.;
    (*spectrum0)(7)[0] = 6.; (*spectrum0)(7)[1] = 0.;
    (*spectrum0)(8)[0] = 0.; (*spectrum0)(8)[1] = 1.;
    (*spectrum0)(9)[0] = 0.; (*spectrum0)(9)[1] = 10.;

    KTFrequencySpectrumFFTW* spectrum1 = new KTFrequencySpectrumFFTW(10, 0, 10);
    (*spectrum1)(0)[0] = 0.; (*spectrum1)(0)[1] = 0.;
    (*spectrum1)(1)[0] = 0.; (*spectrum1)(1)[1] = 0.;
    (*spectrum1)(2)[0] = 0.; (*spectrum1)(2)[1] = 0.;
    (*spectrum1)(3)[0] = 5.; (*spectrum1)(3)[1] = 0.;
    (*spectrum1)(4)[0] = 5.; (*spectrum1)(4)[1] = 0.;
    (*spectrum1)(5)[0] = 5.; (*spectrum1)(5)[1] = 0.;
    (*spectrum1)(6)[0] = 5.; (*spectrum1)(6)[1] = 0.;
    (*spectrum1)(7)[0] = 0.; (*spectrum1)(7)[1] = 0.;
    (*spectrum1)(8)[0] = 2.; (*spectrum1)(8)[1] = 2.;
    (*spectrum1)(9)[0] = 2.; (*spectrum1)(9)[1] = 2.;
    /**/

    dataInput->SetSpectrum(spectrum0, 0);
    dataInput->SetSpectrum(spectrum1, 1);

    KTINFO(corrtestlog, "Spectrum 0");
    spectrum0->Print(0, 10);

    KTINFO(corrtestlog, "Spectrum 1");
    spectrum1->Print(0, 10);

    // Do the correlations
    KTCorrelator* correlator = new KTCorrelator();
    correlator->AddPair(KTCorrelationPair(0, 0));
    correlator->AddPair(KTCorrelationPair(0, 1));
    KTINFO(corrtestlog, "The correlator has " << correlator->GetPairVector().size() << " correlation pairs");

    KTCorrelationData* dataOutput = correlator->Correlate(dataInput);

    KTINFO(corrtestlog, "There are " << dataOutput->GetNPairs() << " ouptut spectra");
    for (unsigned iSpectrum=0; iSpectrum < dataOutput->GetNPairs(); iSpectrum++)
    {
        KTINFO(corrtestlog, "Output Spectrum " << iSpectrum << "; "
                "pair (" << dataOutput->GetFirstChannel(iSpectrum) << ", " <<
                dataOutput->GetSecondChannel(iSpectrum) << ")");
        dataOutput->GetCorrelation(iSpectrum)->Print(0, 10);
    }

    // Clean up
    delete dataInput;
    delete dataOutput;
    delete correlator;

    return 0;
}


