/*
 * TestJSONReader.cc
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#include "KTJSONreader.hh"

#include "KTLogger.hh"

using namespace Katydid;
using namespace std;

KTLOGGER(vallog, "katydid.applications.validation");

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        KTINFO(vallog, "Usage:\n" <<
                "\tTestJSONReader [filename] [file type]");
        return -1;
    }

    string filename = argv[1];
    string fileType = argv[2];

    KTJSONReader reader;
    reader.SetFilename(filename);

    boost::shared_ptr<KTData> parsedData;
    if (fileType == "mc-truth-events")
    {
        parsedData = reader.ReadMCTruthEventsFile();
    }
    else
    {
        KTERROR(vallog, "This program isn't setup to test files of type <" << fileType << ">");
    }

    return 0;
}


