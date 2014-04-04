/*
 * TestCorrelator.cc
 *
 *  Created on: Sep 5, 2012
 *      Author: nsoblath
 */

#include "complexpolar.hh"
#include "KTCorrelationData.hh"
#include "KTCorrelator.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTLogger.hh"

using namespace Katydid;

KTLOGGER(corrtestlog, "TestCorrelator");

int main()
{
    // Prepare the input data

    // KTFrequencySpectrumDataPolar option
    /*
    KTFrequencySpectrumDataPolar* dataInput = new KTFrequencySpectrumDataPolar(2);

    KTFrequencySpectrumPolar* spectrum0 = new KTFrequencySpectrumPolar(10, 0, 10);
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

    KTFrequencySpectrumPolar* spectrum1 = new KTFrequencySpectrumPolar(10, 0, 10);
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
    KTFrequencySpectrumDataFFTW* dataInput = new KTFrequencySpectrumDataFFTW();
    dataInput->SetNComponents(2);

    KTFrequencySpectrumFFTW* spectrum0 = new KTFrequencySpectrumFFTW(19, -10, 10);
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
    (*spectrum0)(10)[0] = 0.; (*spectrum0)(10)[1] = 1.;
    (*spectrum0)(11)[0] = 6.; (*spectrum0)(11)[1] = 0.;
    (*spectrum0)(12)[0] = 5.; (*spectrum0)(12)[1] = 0.;
    (*spectrum0)(13)[0] = 4.; (*spectrum0)(13)[1] = 0.;
    (*spectrum0)(14)[0] = 3.; (*spectrum0)(14)[1] = 0.;
    (*spectrum0)(15)[0] = 2.; (*spectrum0)(15)[1] = 0.;
    (*spectrum0)(16)[0] = 1.; (*spectrum0)(16)[1] = 0.;
    (*spectrum0)(17)[0] = 0.; (*spectrum0)(17)[1] = 0.;
    (*spectrum0)(18)[0] = 0.; (*spectrum0)(18)[1] = 0.;

    KTFrequencySpectrumFFTW* spectrum1 = new KTFrequencySpectrumFFTW(19, -10, 10);
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
    (*spectrum1)(10)[0] = 2.; (*spectrum1)(10)[1] = 2.;
    (*spectrum1)(11)[0] = 0.; (*spectrum1)(11)[1] = 0.;
    (*spectrum1)(12)[0] = 5.; (*spectrum1)(12)[1] = 0.;
    (*spectrum1)(13)[0] = 5.; (*spectrum1)(13)[1] = 0.;
    (*spectrum1)(14)[0] = 5.; (*spectrum1)(14)[1] = 0.;
    (*spectrum1)(15)[0] = 5.; (*spectrum1)(15)[1] = 0.;
    (*spectrum1)(16)[0] = 0.; (*spectrum1)(16)[1] = 0.;
    (*spectrum1)(17)[0] = 0.; (*spectrum1)(17)[1] = 0.;
    (*spectrum1)(18)[0] = 0.; (*spectrum1)(18)[1] = 0.;
    /**/

    dataInput->SetSpectrum(spectrum0, 0);
    dataInput->SetSpectrum(spectrum1, 1);

    KTINFO(corrtestlog, "Spectrum 0");
    spectrum0->Print(0, 19);

    KTINFO(corrtestlog, "Spectrum 1");
    spectrum1->Print(0, 19);

    // Do the correlations
    KTCorrelator* correlator = new KTCorrelator();
    correlator->AddPair(KTCorrelator::UIntPair(0, 0));
    correlator->AddPair(KTCorrelator::UIntPair(0, 1));
    KTINFO(corrtestlog, "The correlator has " << correlator->GetPairVector().size() << " correlation pairs");

    if (! correlator->Correlate(*dataInput))
    {
        KTERROR(corrtestlog, "Something went wrong during the correlation");
        return -1;
    }
    KTCorrelationData& dataOutput = dataInput->Of< KTCorrelationData >();

    KTINFO(corrtestlog, "There are " << dataOutput.GetNComponents() << " output spectra");
    for (unsigned iSpectrum=0; iSpectrum < dataOutput.GetNComponents(); iSpectrum++)
    {
        KTINFO(corrtestlog, "Output Spectrum " << iSpectrum << "; "
                "pair (" << dataOutput.GetInputPair().first << ", " <<
                dataOutput.GetInputPair().second << ")");
        KTINFO(corrtestlog, "Size of spectrum: " << dataOutput.GetSpectrumPolar(iSpectrum)->size());
        dataOutput.GetSpectrumPolar(iSpectrum)->Print(0, 10);
    }

    // Clean up
    delete dataInput;
    delete correlator;

    return 0;
}


