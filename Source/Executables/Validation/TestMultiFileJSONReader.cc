/*
 * TestMultiFileJSONReader.cc
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#include "KTMultiFileJSONReader.hh"

#include "KTLogger.hh"

using namespace Katydid;
using namespace std;

KTLOGGER(vallog, "TestMultiFileJSONReader");

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

    KTMultiFileJSONReader reader;
    reader.AddFilename(filename);
    reader.AddDataType(fileType);

    Nymph::KTDataPtr parsedData(new KTData);
    if (fileType == "mc-truth-events")
    {
        if (! reader.Append(*(parsedData.get())))
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


