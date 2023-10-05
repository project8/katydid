/*
 * KTTerminalTypeWriterTime.cc
 *
 *  Created on: Dec 20, 2013
 *      Author: nsoblath
 */

#include "KTTerminalTypeWriterTime.hh"

#include "KTArbitraryMetadata.hh"
#include "KTDigitizerTestData.hh"
#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTProcSummary.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include <algorithm> // for max
#include <cstdio> // for sprintf
#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    static Nymph::KTTIRegistrar< KTTerminalTypeWriter, KTTerminalTypeWriterTime > sTermTWERegistrar;

    KTTerminalTypeWriterTime::KTTerminalTypeWriterTime() :
            KTTerminalTypeWriter()
    {
    }

    KTTerminalTypeWriterTime::~KTTerminalTypeWriterTime()
    {
    }


    void KTTerminalTypeWriterTime::RegisterSlots()
    {
        fWriter->RegisterSlot("header", this, &KTTerminalTypeWriterTime::WriteEggHeader);
        fWriter->RegisterSlot("metadata", this, &KTTerminalTypeWriterTime::WriteArbitraryMetadata);
        fWriter->RegisterSlot("ts", this, &KTTerminalTypeWriterTime::WriteTimeSeriesData);
        fWriter->RegisterSlot("dig-test", this, &KTTerminalTypeWriterTime::WriteDigitizerTestData);
        fWriter->RegisterSlot("summary", this, &KTTerminalTypeWriterTime::WriteProcSummary);
        return;
    }


    //****************
    // Egg Header Data
    //****************

    void KTTerminalTypeWriterTime::WriteEggHeader(Nymph::KTDataPtr data)
    {
        if (! data) return;

        KTPROG(termlog, "Egg header:\n" << data->Of< KTEggHeader >());

        return;
    }


    void KTTerminalTypeWriterTime::WriteArbitraryMetadata(Nymph::KTDataPtr data)
    {
        if (! data) return;

        scarab::param* metadata = data->Of< KTArbitraryMetadata >().GetMetadata();
        if (metadata == nullptr)
        {
            KTPROG(termlog, "Metadata is empty");
            return;
        }

        KTPROG(termlog, "Arbitrary Metadata:\n" << *metadata);
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTTerminalTypeWriterTime::WriteTimeSeriesData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesData& digData = data->Of< KTTimeSeriesData >();
        unsigned nComponents = digData.GetNComponents();

        KTPROG(termlog, "Slice " << sliceNumber << " (" << nComponents << " components)\n" <<
               "-----");

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* spectrum = digData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                stringstream toTerm;
                toTerm << "Component " << iComponent << '\n';
                unsigned nBins = spectrum->GetNTimeBins();
                for (unsigned iBin = 0; iBin < nBins; ++iBin)
                {
                    //toTerm << to_char((data_type)spectrum->GetValue(iBin));
                }
                KTPROG(termlog, toTerm.str());
            }
        }
        return;
    }

    void KTTerminalTypeWriterTime::WriteDigitizerTestData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTDigitizerTestData& digData = data->Of< KTDigitizerTestData >();
        unsigned nComponents = digData.GetNComponents();

        KTPROG(termlog, "Slice " << sliceNumber << " (" << nComponents << " components)\n\n");

        unsigned nBits = digData.GetNBits();

        // Bit Occupancy
        if (digData.GetBitOccupancyFlag())
        {
            string prefixSpc("          |  ");
            string prefixOcc("Occupancy |  ");
            string prefixBit("      Bit |  ");
            unsigned prefixSize = prefixOcc.size();
            unsigned maxHistRows = 10;
            string betweenBins(" | ");
            string filledBin("   #   ");
            string  emptyBin("       ");
            unsigned binWidth = filledBin.size();
            unsigned binPadding = betweenBins.size();
            unsigned bufferSize = (binWidth + binPadding) * (nBits + 1) + prefixSize;
            char* buffer = new char[bufferSize];
            stringstream toTermBitOcc;
            toTermBitOcc << "Bit Occupancy Test\n";
            for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
            {
                toTermBitOcc << "Component " << iComponent << '\n';
                // histogram
                unsigned maxValue = 0;
                for (int bit = (int)nBits-1; bit >= 0; --bit)
                {
                    maxValue = std::max(maxValue, digData.GetBitHistogram()->operator()(bit));
                }
                unsigned histRows = std::min(maxHistRows, maxValue);
                unsigned occPerRow = (unsigned)KTMath::Nint((double)maxValue / (double)histRows);
                for (int row = (int)histRows-1; row >= 0; --row)
                {
                    snprintf(buffer, prefixSpc.size(), "%s", prefixSpc.c_str());
                    for (int bit = (int)nBits-1; bit >= 0; --bit)
                    {
                        int printOffset = (int)nBits - 1 - bit;
                        if (digData.GetBitHistogram()->operator()(bit) > (unsigned)row * occPerRow)
                        {
                            snprintf(buffer + prefixSize + printOffset*binWidth + printOffset*binPadding, filledBin.size(), "%s", filledBin.c_str());
                        }
                        else
                        {
                            snprintf(buffer + prefixSize + printOffset*binWidth + printOffset*binPadding, emptyBin.size(), "%s", emptyBin.c_str());
                        }
                        if (bit != 0)
                        {
                            snprintf(buffer + prefixSize + (printOffset+1)*binWidth + printOffset*binPadding, betweenBins.size(), "%s", betweenBins.c_str());
                        }
                    }
                    toTermBitOcc << buffer << '\n';
                }
                // bit occupancy line
                snprintf(buffer, prefixOcc.size(), "%s", prefixOcc.c_str());
                for (int bit = (int)nBits-1; bit >= 0; --bit)
                {
                    int printOffset = (int)nBits - 1 - bit;
                    snprintf(buffer + prefixSize + printOffset*binWidth + printOffset*binPadding, binWidth, "%*u", binWidth, digData.GetBitHistogram()->operator()(bit));
                    if (bit != 0)
                    {
                        snprintf(buffer + prefixSize + (printOffset+1)*binWidth + printOffset*binPadding, betweenBins.size(), "%s", betweenBins.c_str());
                    }
                }
                toTermBitOcc << buffer << '\n';

                // bit number line
                snprintf(buffer, prefixBit.size(), "%s", prefixBit.c_str());
                for (int bit = (int)nBits-1; bit >= 0; --bit)
                {
                    int printOffset = (int)nBits - 1 - bit;
                    snprintf(buffer + prefixSize + printOffset*binWidth + printOffset*binPadding, binWidth, "%*u", binWidth, bit);
                    if (bit != 0)
                    {
                        snprintf(buffer + prefixSize + (printOffset+1)*binWidth + printOffset*binPadding, betweenBins.size(), "%s", betweenBins.c_str());
                    }
                }
                toTermBitOcc << buffer;
            }
            delete [] buffer;
            KTPROG(termlog, toTermBitOcc.str() << '\n');
        }

        // Clipping
        if (digData.GetClippingFlag())
        {
            stringstream toTermClipping;
            toTermClipping << "Clipping Test\n";
            for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
            {
                toTermClipping << "Component " << iComponent << '\n';
                toTermClipping << "\t\tNumber\tFraction\n";
                toTermClipping << "\tTop   \t" << digData.GetNClipTop(iComponent) << '\t' << digData.GetTopClipFrac(iComponent) << '\n';
                toTermClipping << "\tBottom\t" << digData.GetNClipBottom(iComponent) << '\t' << digData.GetBottomClipFrac(iComponent) << '\n';
                toTermClipping << "\tSequential Top   \t" << digData.GetNMultClipTop(iComponent) << '\t' << digData.GetMultTopClipFrac(iComponent) << '\n';
                toTermClipping << "\tSequential Bottom\t" << digData.GetNMultClipBottom(iComponent) << '\t' << digData.GetMultBottomClipFrac(iComponent) << '\n';
            }
            KTPROG(termlog, toTermClipping.str());
        }
        //Linearity
        if (digData.GetLinearityFlag())
        {
            stringstream toTermLinearity;
            toTermLinearity << "Linearity Test\n";
            for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
            {
                toTermLinearity << "Component " << iComponent << '\n';
                toTermLinearity << "Upslope: "<<'\n';
                toTermLinearity << '\t' << "Average Fractional Max Difference: " << digData.GetMaxDiffAvg(iComponent) << " +/- " <<digData.GetMaxDiffStdev(iComponent)<< '\n';
                toTermLinearity << '\t' << "Average LinReg Slope: "<<digData.GetAvgLinRegSlope(iComponent)<<'\n'<<'\n';
                toTermLinearity << "Downslope: "<<'\n';
                toTermLinearity << '\t' << "Average Fractional Max Difference: " << digData.GetMaxDiffAvgD(iComponent) << " +/- " <<digData.GetMaxDiffStdevD(iComponent)<< '\n';
                toTermLinearity << '\t' << "Average LinReg Slope: "<<digData.GetAvgLinRegSlopeD(iComponent)<<'\n';

            }
            KTPROG(termlog, toTermLinearity.str());
        }

        return;
    }


    //******************
    // Processor Summary
    //******************

    void KTTerminalTypeWriterTime::WriteProcSummary(const KTProcSummary* summary)
    {
        if (summary == NULL) return;

        KTPROG(termlog, "Processing Summary:");
        KTPROG(termlog, "\tNumber of slices processed: " << summary->GetNSlicesProcessed());
        KTPROG(termlog, "\tNumber of records processed: " << summary->GetNRecordsProcessed());
        KTPROG(termlog, "\tIntegrated time processed: " << summary->GetIntegratedTime());

        return;
    }



} /* namespace Katydid */
