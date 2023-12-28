/*
 * TestBasicROOTFileWriter.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTJSONTypeWriterTime.hh"
#include "KTEggHeader.hh"
#include "KTJSONWriter.hh"
#include "logger.hh"

using namespace Katydid;
using namespace std;

LOGGER(testlog, "TestJSONWriter");

int main()
{
    LINFO(testlog, "Preparing for test");

    // Setup a dummy header to print
    Nymph::KTDataPtr headerPtr(new Nymph::KTData());
    KTEggHeader& header = headerPtr->Of< KTEggHeader >();
    header.SetFilename("awesome_data.egg");
    header.SetAcquisitionMode(1);
    header.SetNChannels(2);
    header.SetRunDuration(203985);
    header.SetAcquisitionRate(500.);

    KTChannelHeader* channelHeader = new KTChannelHeader();
    channelHeader->SetSliceSize(512);
    channelHeader->SetRecordSize(4194304);
    header.SetChannelHeader(channelHeader, 0);

    // Set up the writer
    KTJSONWriter writer;
    writer.SetFilename("test_writer.json");
    writer.SetFileMode("w+");
    writer.SetPrettyJSONFlag(false);

    LINFO(testlog, "Writing to file");

    // Writer the data
    writer.GetTypeWriter< KTJSONTypeWriterTime >()->WriteEggHeader(headerPtr);

    // Close the file
    writer.CloseFile();

    LINFO(testlog, "Writing complete");

    // Switch setup to print to the terminal
    writer.SetFilename("stdout");
    writer.SetPrettyJSONFlag(true);

    LINFO(testlog, "Writing to terminal");

    writer.GetTypeWriter< KTJSONTypeWriterTime >()->WriteEggHeader(headerPtr);

    // Close the file
    writer.CloseFile();

    LINFO(testlog, "Writing complete");

    LINFO(testlog, "Test complete; see file output in test_writer.json");

    return 0;

}
