/*
 * TestROOTTreeWritingViaCicada.cc
 *
 *  Created on: Mar 13, 2018
 *      Author: N.S. Oblath
 */

#include "KTMultiTrackEventData.hh"
#include "KTProcessedTrackData.hh"
#include "KTROOTTreeTypeWriterEventAnalysis.hh"

#include "logger.hh"

using namespace Katydid;
using namespace std;

LOGGER(testlog, "TestROOTTreeWritingViaCicada");

int main()
{
    string filename("test_root_tree_writing_via_cicada.root");

    LINFO(testlog, "Creating processors");

    KTROOTTreeWriter writer;
    writer.SetFilename(filename);
    writer.SetFileFlag("recreate");

    KTROOTTreeTypeWriterEventAnalysis typeWriter;
    typeWriter.SetWriter(&writer);

    LINFO(testlog, "Creating data objects");

    Nymph::KTDataPtr data(new Nymph::KTData());
    KTProcessedTrackData& ptData = data->Of< KTProcessedTrackData >();
    KTMultiTrackEventData& mteData = data->Of< KTMultiTrackEventData >();

    ptData.SetAcquisitionID(10);

    mteData.SetAcquisitionID(11);
    mteData.AddTrack(ptData);

    LINFO(testlog, "Writing processed track data");
    typeWriter.WriteProcessedTrack(data);

    LINFO(testlog, "Writing multi-track event data");
    typeWriter.WriteMultiTrackEvent(data);

    LINFO(testlog, "Done with tests; closing file");

    writer.CloseFile();

    return 0;
}
