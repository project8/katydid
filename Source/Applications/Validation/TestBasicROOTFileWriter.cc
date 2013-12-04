/*
 * TestBasicROOTFileWriter.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTFileWriter.hh"
#include "KTBasicROOTTypeWriterFFT.hh"
#include "KTData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTSliceHeader.hh"
#include "complexpolar.hh"

#include <iostream>

using namespace Katydid;
using namespace std;

int main()
{
    // Set up the data
    KTDataPtr data(new KTData);

    KTSliceHeader& header = data->Of< KTSliceHeader >();
    header.SetSliceNumber(1);

    KTFrequencySpectrumDataPolar& fsData = data->Of< KTFrequencySpectrumDataPolar >().SetNComponents(2);

    KTFrequencySpectrumPolar* spectrum1 = new KTFrequencySpectrumPolar(10, -0.5, 9.5);
    (*spectrum1)(3).set_polar(5., 1.);
    fsData.SetSpectrum(spectrum1, 0);

    KTFrequencySpectrumPolar* spectrum2 = new KTFrequencySpectrumPolar(10, -0.5, 9.5);
    (*spectrum2)(8).set_polar(3., 2.);
    fsData.SetSpectrum(spectrum2, 1);

    // Set up the writer
    KTBasicROOTFileWriter writer;
    writer.SetFilename("test_writer.root");
    writer.SetFileFlag("recreate");

    // Writer the data
    writer.GetTypeWriter< KTBasicROOTTypeWriterFFT >()->WriteFrequencySpectrumDataPolar(data);

    // Set up next data
    (*spectrum1)(3).set_polar(10., .5);
    (*spectrum2)(8).set_polar(12., 2.1);
    header.SetSliceNumber(2);

    // Publish the data
    writer.GetTypeWriter< KTBasicROOTTypeWriterFFT >()->WriteFrequencySpectrumDataPolar(data);

    cout << "Test complete; see histograms in test_writer.root" << endl;

    return 0;

}
