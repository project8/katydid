/*
 * TestBasicROOTFilePublisher.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTFilePublisher.hh"
#include "KTEvent.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumData.hh"
#include "complexpolar.hh"

#include <iostream>

using namespace Katydid;
using namespace std;

int main()
{
    // Set up a dummy event
    KTEvent* event = new KTEvent();

    // Set up the data
    KTFrequencySpectrumData* data = new KTFrequencySpectrumData(2);
    data->SetEvent(event);
    event->SetEventNumber(0);

    KTFrequencySpectrum* spectrum1 = new KTFrequencySpectrum(10, -0.5, 9.5);
    (*spectrum1)[3].set_polar(5., 1.);
    data->SetSpectrum(spectrum1, 0);

    KTFrequencySpectrum* spectrum2 = new KTFrequencySpectrum(10, -0.5, 9.5);
    (*spectrum2)[8].set_polar(3., 2.);
    data->SetSpectrum(spectrum2, 1);

    // Set up the publisher
    KTBasicROOTFilePublisher* publisher = new KTBasicROOTFilePublisher();
    publisher->SetFilename("test_publisher.root");
    publisher->SetFileFlag("recreate");

    // Publisher the data
    publisher->Publish(data);

    // Set up next data
    (*spectrum1)[3].set_polar(10., .5);
    (*spectrum2)[8].set_polar(12., 2.1);
    event->SetEventNumber(1);

    // Publish the data
    publisher->Publish(data);

    // Clean up
    delete data;
    delete publisher;

    cout << "Test complete; see histograms in test_publisher.root" << endl;

    return 0;

}
