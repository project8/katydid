/*
 * TestROOTWritingToSameFile.cc
 *
 *  Created on: Jan 5, 2017
 *      Author: obla999
 */

#include "KTBasicROOTFileWriter.hh"
#include "KTBasicROOTTypeWriterTransform.hh"
#include "KTData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTROOTTreeTypeWriterSpectrumAnalysis.hh"
#include "KTROOTTreeWriter.hh"
#include "KTSliceHeader.hh"
#include "complexpolar.hh"

#include <iostream>


using namespace Katydid;
using namespace std;

int main()
{
    // Set up the data
    Nymph::KTDataPtr data(new Nymph::KTData);

    KTSliceHeader& header = data->Of< KTSliceHeader >();
    header.SetSliceNumber(1);

    KTFrequencySpectrumDataPolar& fsData = data->Of< KTFrequencySpectrumDataPolar >().SetNComponents(2);

    KTFrequencySpectrumPolar* spectrum1 = new KTFrequencySpectrumPolar(10, -0.5, 9.5);
    (*spectrum1)(3).set_polar(5., 1.);
    fsData.SetSpectrum(spectrum1, 0);

    KTFrequencySpectrumPolar* spectrum2 = new KTFrequencySpectrumPolar(10, -0.5, 9.5);
    (*spectrum2)(8).set_polar(3., 2.);
    fsData.SetSpectrum(spectrum2, 1);

    KTDiscriminatedPoints1DData& dpData = data->Of< KTDiscriminatedPoints1DData >().SetNComponents(1);
    dpData.AddPoint(0, KTDiscriminatedPoints1DData::Point(1., 2., 3.));
    dpData.AddPoint(5, KTDiscriminatedPoints1DData::Point(4., 5., 6.));

    // Set up the Basic ROOT writer
    KTBasicROOTFileWriter brWriter;
    brWriter.SetFilename("test_one_file_writing.root");
    brWriter.SetFileFlag("recreate");

    // Set up the ROOT Tree writer
    KTROOTTreeWriter rtWriter;
    rtWriter.SetFilename("test_one_file_writing.root");
    rtWriter.SetFileFlag("recreate");

    // Write the data
    brWriter.GetTypeWriter< KTBasicROOTTypeWriterTransform >()->WriteFrequencySpectrumDataPolar(data);
    rtWriter.GetTypeWriter< KTROOTTreeTypeWriterSpectrumAnalysis >()->WriteDiscriminatedPoints1D(data);

    // Set up next data
    (*spectrum1)(3).set_polar(10., .5);
    (*spectrum2)(8).set_polar(12., 2.1);
    header.SetSliceNumber(2);

    // Write the data
    brWriter.GetTypeWriter< KTBasicROOTTypeWriterTransform >()->WriteFrequencySpectrumDataPolar(data);

    cout << "Test complete; see histograms in test_writer.root" << endl;

    return 0;

}


