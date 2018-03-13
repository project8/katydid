/*
 * TestROOTTreeWritingViaCicada.cc
 *
 *  Created on: Mar 13, 2018
 *      Author: obla999
 */

#include "KTROOTTreeTypeWriterEventAnalysis.hh"

#include "KTLogger.hh"

using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "TestROOTTreeWritingViaCicada");

int main()
{
    string filename("test_root_tree_writing_via_cicada.root");

    KTROOTTreeWriter writer;
    writer.SetFilename(filename);
    writer.SetFileFlag("recreate");

    KTROOTTreeTypeWriterEventAnalysis typeWriter;
    typeWriter.SetWriter(&writer);




    return 0;
}
