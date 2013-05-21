/*
 * TestOneShotJSONReader.cc
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#include "KTOneShotJSONReader.hh"

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

    KTOneShotJSONReader reader;
    reader.SetFilename(filename);

    boost::shared_ptr<KTData> parsedData;
    if (fileType == "mc-truth-events")
    {
        if (! reader.ReadMCTruthEventsFile(*(parsedData.get())))
        {
            KTERROR(vallog, "Something went wrong while reading the mc truth file");
        }
    }
    else
    {
        KTERROR(vallog, "This program isn't setup to test files of type <" << fileType << ">");
    }

    return 0;
}


