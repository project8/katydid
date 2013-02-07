/*
 * TestBasicROOTFileWriter.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTFileWriter.hh"
#include "KTBasicROOTTypeWriterFFT.hh"
#include "KTBundle.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumData.hh"
#include "complexpolar.hh"

#include <iostream>

using namespace Katydid;
using namespace std;

int main()
{
    // Set up a dummy bundle
    KTBundle* bundle = new KTBundle();

    // Set up the data
    KTFrequencySpectrumData* data = new KTFrequencySpectrumData(2);
    data->SetBundle(bundle);
    bundle->SetBundleNumber(0);

    KTFrequencySpectrumPolar* spectrum1 = new KTFrequencySpectrumPolar(10, -0.5, 9.5);
    (*spectrum1)(3).set_polar(5., 1.);
    data->SetSpectrum(spectrum1, 0);

    KTFrequencySpectrumPolar* spectrum2 = new KTFrequencySpectrumPolar(10, -0.5, 9.5);
    (*spectrum2)(8).set_polar(3., 2.);
    data->SetSpectrum(spectrum2, 1);

    // Set up the writer
    KTBasicROOTFileWriter* writer = new KTBasicROOTFileWriter();
    writer->SetFilename("test_writer.root");
    writer->SetFileFlag("recreate");

    // Writer the data
    writer->GetTypeWriter< KTBasicROOTTypeWriterFFT >()->WriteFrequencySpectrumData(data);

    // Set up next data
    (*spectrum1)(3).set_polar(10., .5);
    (*spectrum2)(8).set_polar(12., 2.1);
    bundle->SetBundleNumber(1);

    // Publish the data
    writer->GetTypeWriter< KTBasicROOTTypeWriterFFT >()->WriteFrequencySpectrumData(data);

    // Clean up
    delete data;
    delete writer;

    cout << "Test complete; see histograms in test_writer.root" << endl;

    return 0;

}
