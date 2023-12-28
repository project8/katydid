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
#include "logger.hh"

using namespace Katydid;

LOGGER(corrtestlog, "TestCorrelator");

int main()
{
    // Prepare the input data

    // KTFrequencySpectrumDataPolar option
    /*
    KTFrequencySpectrumDataPolar* dataInput = new KTFrequencySpectrumDataPolar(2);

    KTFrequencySpectrumPolar* spectrum0 = new KTFrequencySpectrumPolar(10, 0, 20);
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

    KTFrequencySpectrumPolar* spectrum1 = new KTFrequencySpectrumPolar(10, 0, 20);
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

    KTFrequencySpectrumFFTW* spectrum0 = new KTFrequencySpectrumFFTW(19, 0, 20);
    spectrum0->SetRect(0, 0., 0.);
    spectrum0->SetRect(1, 0., 0.);
    spectrum0->SetRect(2, 1., 0.);
    spectrum0->SetRect(3, 2., 0.);
    spectrum0->SetRect(4, 3., 0.);
    spectrum0->SetRect(5, 4., 0.);
    spectrum0->SetRect(6, 5., 0.);
    spectrum0->SetRect(7, 6., 0.);
    spectrum0->SetRect(8, 0., 1.);
    spectrum0->SetRect(9, 0., 10.);
    spectrum0->SetRect(10, 0., 1.);
    spectrum0->SetRect(11, 6., 0.);
    spectrum0->SetRect(12, 5., 0.);
    spectrum0->SetRect(13, 4., 0.);
    spectrum0->SetRect(14, 3., 0.);
    spectrum0->SetRect(15, 2., 0.);
    spectrum0->SetRect(16, 1., 0.);
    spectrum0->SetRect(17, 0., 0.);
    spectrum0->SetRect(18, 0., 0.);

    KTFrequencySpectrumFFTW* spectrum1 = new KTFrequencySpectrumFFTW(19, 0, 20);
    spectrum1->SetRect(0, 0., 0.);
    spectrum1->SetRect(1, 0., 0.);
    spectrum1->SetRect(2, 0., 0.);
    spectrum1->SetRect(3, 5., 0.);
    spectrum1->SetRect(4, 5., 0.);
    spectrum1->SetRect(5, 5., 0.);
    spectrum1->SetRect(6, 5., 0.);
    spectrum1->SetRect(7, 0., 0.);
    spectrum1->SetRect(8, 2., 2.);
    spectrum1->SetRect(9, 2., 2.);
    spectrum1->SetRect(10, 2., 2.);
    spectrum1->SetRect(11, 0., 0.);
    spectrum1->SetRect(12, 5., 0.);
    spectrum1->SetRect(13, 5., 0.);
    spectrum1->SetRect(14, 5., 0.);
    spectrum1->SetRect(15, 5., 0.);
    spectrum1->SetRect(16, 0., 0.);
    spectrum1->SetRect(17, 0., 0.);
    spectrum1->SetRect(18, 0., 0.);
    
    /**/

    dataInput->SetSpectrum(spectrum0, 0);
    dataInput->SetSpectrum(spectrum1, 1);

    LINFO(corrtestlog, "Spectrum 0");
    spectrum0->Print(0, 19);

    LINFO(corrtestlog, "Spectrum 1");
    spectrum1->Print(0, 19);

    // Do the correlations
    KTCorrelator* correlator = new KTCorrelator();
    correlator->AddPair(KTCorrelator::UIntPair(0, 0));
    correlator->AddPair(KTCorrelator::UIntPair(0, 1));
    LINFO(corrtestlog, "The correlator has " << correlator->GetPairVector().size() << " correlation pairs");

    if (! correlator->Correlate(*dataInput))
    {
        LERROR(corrtestlog, "Something went wrong during the correlation");
        return -1;
    }
    KTCorrelationData& dataOutput = dataInput->Of< KTCorrelationData >();

    LINFO(corrtestlog, "There are " << dataOutput.GetNComponents() << " output spectra");
    for (unsigned iSpectrum=0; iSpectrum < dataOutput.GetNComponents(); iSpectrum++)
    {
        LINFO(corrtestlog, "Output Spectrum " << iSpectrum << "; "
                "pair (" << dataOutput.GetInputPair().first << ", " <<
                dataOutput.GetInputPair().second << ")");
        LINFO(corrtestlog, "Size of spectrum: " << dataOutput.GetSpectrumPolar(iSpectrum)->size());
        dataOutput.GetSpectrumPolar(iSpectrum)->Print(0, 10);
    }

    // Clean up
    delete dataInput;
    delete correlator;

    return 0;
}


