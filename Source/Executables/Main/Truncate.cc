/*
 * Truncate.cc
 *
 *  Created on: June 29, 2012
 *      Author: nsoblath
 *
 *      Truncates a file to a given size (bytes).
 *      Unless otherwise specified on the command line, you will be asked to verify that you want to perform the truncation before the file is modified.
 *
 *      WARNING: This routine will permanently delete part of a file!
 *               Only use this if you are ABSOLUTELY 100% SURE that you know what you're doing!
 *
 *      If you specify an output file, the input file will be copied to the output file before truncating.
 *      In this case:
 *        -- the input file will not be truncated.
 *        -- the file system must be able to temporarily hold a full copy of the input file.
 *
 *
 *      Command-line options
 *        -f --- Input file; will be truncated if output file is not specified.
 *        -o --- (optional) Output file
 *        -s --- (optional) Truncated file size in bytes; default is 1,000,000 bytes.
 *        -w --- (optional) Skips the warning message and opportunity to abort.
 */

#include "logger.hh"

#include <cstdlib>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
using namespace Nymph;

LOGGER(tclog, "Truncate");

int main(int argc, char** argv)
{
    // variables that can be set by the command line arguments
    string inputFileName("");
    string outputFileName("");
    off_t newFileSize = 1000000; // default size is 1 MB
    bool skipWarning = false;

    // parse the command line
    int arg;
    extern char *optarg;
    while ((arg = getopt(argc, argv, "f:o:s:w")) != -1)
        switch (arg)
        {
            case 'f':
                inputFileName = string(optarg);
                break;
            case 'o':
                outputFileName = string(optarg);
                break;
            case 's':
                newFileSize = (off_t)atol(optarg);
                break;
            case 'w':
                skipWarning = true;
                break;
        }

    // the input file name must be provided; exit if it isn't
    if (inputFileName.empty())
    {
        LERROR(tclog, "Please provide a file name using '-f [filename]'");
        return -1;
    }
    bool newOutputFile = (! outputFileName.empty() && outputFileName != inputFileName);

    // get information about the input file; this is primarily used for the file size
    // exit if the function to get the file status fails
    // exit if the current file size is smaller than the requested truncation size
    struct stat fileStatus;
    int result = stat(inputFileName.c_str(), &fileStatus);
    if (result != 0)
    {
        LERROR(tclog, "Unable to get file information; aborting!");
        return -2;
    }
    if (fileStatus.st_size < newFileSize)
    {
        LERROR(tclog, "Current file size (" << fileStatus.st_size << " bytes) is smaller than the requested truncated size (" << newFileSize << " bytes); process aborted!");
        return -3;
    }

    // print out useful information so the user knows what's about to happen
    LINFO(tclog, "File to truncate: " << inputFileName);
    if (newOutputFile)
    {
        LINFO(tclog, "New filename will be: " << outputFileName);
    }
    LINFO(tclog, "Original file size: " << fileStatus.st_size << " bytes");
    LINFO(tclog, "Truncated file size: " << newFileSize << " bytes\n");

    // if the user specified to skip the warning, than do so
    if (! skipWarning)
    {
        // print the warning message and ask the user if s/he wants to continue
        LWARN(tclog, "File truncation will result in data loss! Are you sure you want to continue? (y/n)");
        char response;
        cin >> response;
        if (response != 'y')
        {
            LINFO(tclog, "Truncation ABORTED");
            return 0;
        }
    }

    // if the user specified an output file name, copy the input file to the new file name
    if (newOutputFile)
    {
        // copy file to outputFileName
        LINFO(tclog, "Copying the input file to the output file");
        ifstream inputFileStream(inputFileName.c_str(), ios::binary);
        ofstream outputFileStream(outputFileName.c_str(), ios::binary);
        outputFileStream << inputFileStream.rdbuf();
        inputFileStream.close();
        outputFileStream.close();
    }
    else
    {
        outputFileName = inputFileName;
    }

    // perform the truncation
    LINFO(tclog, "Truncating the file");
    result = truncate(outputFileName.c_str(), newFileSize);

    // print the results of the truncation
    if (result == 0)
    {
        LINFO(tclog, "File truncated successfully")
    }
    else
    {
        LERROR(tclog, "An error occurred during file truncation!");
    }

    return result;
}
