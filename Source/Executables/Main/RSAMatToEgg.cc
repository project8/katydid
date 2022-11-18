/*
 * RSAMatToEgg.cc
 *
 *  Created on: Jan 20, 2015
 *      Author: nsoblath
 *
 *  Usage: RSAMatToEgg -e outputFilename.egg [input files]
 */

#include "KTConstants.hh"
#include "KTData.hh"
#include "KTEggHeader.hh"
#include "KTMath.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTRSAMatReader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include "M3DataInterface.hh"
#include "M3Monarch.hh"

#include <boost/filesystem.hpp>

#include <iostream>
#include <string>
#include <unistd.h>

#include <fftw3.h>


using namespace std;
using namespace Katydid;



KTLOGGER(exelog, "RSAMatToEgg");

void PrintUsage()
{
    KTPROG(exelog, "Usage:\n" <<
            "\t> RSAMatToEgg -e outputFilename.egg [input files]\n\n" <<
            "Parameters:\n" <<
            "\t-e -- Specifies the filename that will be created.\n" <<
            "\t[input files] -- Specifies the Matlab-formatted RSA files to be combined into the egg file; wildcards can be used");
    return;
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        PrintUsage();
        exit(-1);
    }

    boost::filesystem::path outputFilename;

    int arg;
    extern char *optarg;
    extern int optind, optopt;
    while ((arg = getopt(argc, argv, "e:h")) != -1)
    {
        switch (arg)
        {
            case 'e':
                outputFilename = string(optarg);
                break;
            case 'h':
                PrintUsage();
                exit(0);
                break;
            default:
                PrintUsage();
                exit(-1);
                break;
        }
    }

    KTINFO(exelog, "Output file: " << outputFilename.string());

    boost::filesystem::path currentPath(boost::filesystem::current_path());

    std::vector< boost::filesystem::path > matFiles;
    for (int iArg = optind; iArg < argc; ++iArg)
    {
        boost::filesystem::path candidate(argv[iArg]);
        if (candidate.is_relative()) candidate = currentPath / candidate;
        if (! candidate.has_filename())
        {
            KTERROR(exelog, "Output file provided does not specify a file: " << candidate);
            exit(-1);
        }
        matFiles.push_back(candidate);
        KTDEBUG(exelog, "Added input file: " << matFiles.back().string());
    }

    if (matFiles.empty())
    {
        KTERROR(exelog, "No input files were provided");
        exit(-1);
    }

    // Open the first file
    KTRSAMatReader* matReader = new KTRSAMatReader();
    // setting the slice size to 0 will make the slice the size of the record in the matlab file (see KTRSAMatReader::BreakEgg)
    matReader->SetSliceSize(0);
    std::vector< boost::filesystem::path >::const_iterator matIt = matFiles.begin();
    Nymph::KTDataPtr matHeaderPtr = matReader->BreakAnEgg(matIt->string());
    KTEggHeader& eggHeader = matHeaderPtr->Of< KTEggHeader >();
    double acqRate = eggHeader.GetAcquisitionRate();
    unsigned recSize = eggHeader.GetChannelHeader(0)->GetRecordSize();

    // Open and setup the output file via the header
    monarch3::Monarch3* monarch = monarch3::Monarch3::OpenForWriting(outputFilename.string());
    if (monarch == NULL)
    {
        KTERROR(exelog, "Unable to open output file <" << outputFilename.string() << ">");
        exit(-1);
    }

    monarch3::M3Header* header = monarch->GetHeader();
    header->SetFilename(outputFilename.string());
    header->SetRunDuration(eggHeader.GetRunDuration()); // in ms
    header->SetTimestamp(eggHeader.GetTimestamp());
    header->SetDescription(eggHeader.GetDescription());

    header->AddStream("RSA", KTMath::Nint(acqRate), recSize, 2, eggHeader.GetChannelHeader(0)->GetDataTypeSize(), monarch3::sAnalog, 14, sBitsAlignedLeft);

    // Setup the remaining details in the channel header
    monarch3::M3ChannelHeader& chHeader = header->GetChannelHeaders()[0];
    chHeader.SetVoltageOffset(eggHeader.GetChannelHeader(0)->GetVoltageOffset());
    chHeader.SetVoltageRange(eggHeader.GetChannelHeader(0)->GetVoltageRange());
    chHeader.SetFrequencyMin(eggHeader.GetMinimumFrequency());
    chHeader.SetFrequencyRange(eggHeader.GetMaximumFrequency() - eggHeader.GetMinimumFrequency());

    monarch->WriteHeader();

    // Get the stream to use for writing to the file
    monarch3::M3Stream* stream = monarch->GetStream(0);
    monarch3::M3ComplexDataWriter< fftw_complex > writer(stream->GetStreamRecord()->GetData(), recSize, monarch3::sAnalog);

    bool success = true;
    for (; matIt != matFiles.end(); ++matIt)
    {
        if (matReader == NULL)
        {
            // this will not the be the case in the first iteration, since the file is already open, but will be true after that
            matReader = new KTRSAMatReader();
            matReader->SetSliceSize(0);
            Nymph::KTDataPtr newMatHeaderPtr = matReader->BreakAnEgg(matIt->string());
            KTEggHeader& newEggHeader = matHeaderPtr->Of< KTEggHeader >();
            if (newEggHeader.GetAcquisitionRate() != acqRate || newEggHeader.GetChannelHeader(0)->GetRecordSize() != recSize)
            {
                KTERROR(exelog, "File <" << matIt->string() << "> has parameters that don't match the first file:\n" <<
                        "\tAcquisition rate: first = " << acqRate << "\tthis = " << newEggHeader.GetAcquisitionRate() << '\n' <<
                        "\tRecord size:      first = " << recSize << "\tthis = " << newEggHeader.GetChannelHeader(0)->GetRecordSize());
                success = false;
                matReader->CloseEgg();
                delete matReader;
                matReader = NULL;
                break;
            }
        }

        Nymph::KTDataPtr data = matReader->HatchNextSlice();
        bool firstRec = true;
        while (data)
        {
            KTSliceHeader& slHeader = data->Of< KTSliceHeader >();
            KTDEBUG(exelog, "Processing slice " << slHeader.GetSliceNumber());
            KTTimeSeriesData& tsData = data->Of< KTTimeSeriesData >();
            KTTimeSeriesFFTW* timeSeries = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries(0));
            if (timeSeries == NULL)
            {
                KTERROR(exelog, "Time series was not of type KTTimeSeriesFFTW");
                success = false;
                matReader->CloseEgg();
                delete matReader;
                matReader = NULL;
                break;
            }

            // side note: if the time series array type were guaranteed to be contiguous in memory, this could be replaced with a memcpy
            for (unsigned iBin = 0; iBin < recSize; ++iBin)
            {
                fftw_complex val;
                val[0] = timeSeries->GetReal(iBin);
                val[1] = timeSeries->GetImag(iBin);
                writer.set_at(val, iBin);
            }

            stream->WriteRecord(firstRec);
            firstRec = false;

            data = matReader->HatchNextSlice();
        }

        matReader->CloseEgg();
        delete matReader;
        matReader = NULL;
    }

    if (! success)
    {
        KTERROR(exelog, "An error occurred while writing the egg file");
    }

    monarch->FinishWriting();
    delete monarch;

    return 0;
}
