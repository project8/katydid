/*
 * TestROOTTreeWritingViaCicada.cc
 *
 *  Created on: Mar 13, 2018
 *      Author: N.S. Oblath
 */

#include "KTMultiTrackEventData.hh"
#include "KTProcessedTrackData.hh"
#include "KTROOTTreeTypeWriterEventAnalysis.hh"

#include "KTLogger.hh"

using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "TestROOTTreeWritingViaCicada");

int main()
{
    string filename("test_root_tree_writing_via_cicada.root");

    KTINFO(testlog, "Creating processors");

    KTROOTTreeWriter writer;
    writer.SetFilename(filename);
    writer.SetFileFlag("recreate");

    KTROOTTreeTypeWriterEventAnalysis typeWriter;
    typeWriter.SetWriter(&writer);

    KTINFO(testlog, "Creating data objects");

    Nymph::KTDataPtr data(new Nymph::KTData());
    KTProcessedTrackData& ptData = data->Of< KTProcessedTrackData >();
    KTMultiTrackEventData& mteData = data->Of< KTMultiTrackEventData >();

    ptData.SetAcquisitionID(10);

    mteData.SetAcquisitionID(11);
    mteData.AddTrack(ptData);

    KTINFO(testlog, "Writing processed track data");
    typeWriter.WriteProcessedTrack(data);

    KTINFO(testlog, "Writing multi-track event data");
    typeWriter.WriteMultiTrackEvent(data);

    KTINFO(testlog, "Done with tests; closing file");

    writer.CloseFile();

    return 0;
}
