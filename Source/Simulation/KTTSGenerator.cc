/*
 * KTTSGenerator.cc
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#include "KTTSGenerator.hh"

#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTProcSummary.hh"
#include "KTPStoreNode.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include <cmath>

using boost::shared_ptr;

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "katydid.simulation");

    KTTSGenerator::KTTSGenerator(const string& name) :
            KTPrimaryProcessor(name),
            fNSlices(1),
            fNChannels(2),
            fSliceSize(1024),
            fBinWidth(5.e-9),
            fRecordSize(4194304),
            fTimeSeriesType(kRealTimeSeries),
            fSliceCounter(0),
            fDataSlot("slice", this, &KTTSGenerator::GenerateTS, &fDataSignal),
            fHeaderSignal("header", this),
            fDataSignal("slice", this),
            fDoneSignal("done", this),
            fSummarySignal("summary", this)
    {
    }

    KTTSGenerator::~KTTSGenerator()
    {
    }

    Bool_t KTTSGenerator::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        // set the number of slices to create
        fNSlices = node->GetData< UInt_t >("n-slices", fNSlices);

        // number of slices
        fNChannels = node->GetData< UInt_t >("n-channels", fNChannels);

        // specify the length of the time series
        fSliceSize = node->GetData< UInt_t >("time-series-size", fSliceSize);
        fBinWidth = node->GetData< Double_t >("bin-width", fBinWidth);

        // type of time series
        string timeSeriesTypeString = node->GetData< string >("time-series-type", "real");
        if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
        else if (timeSeriesTypeString == "fftw") SetTimeSeriesType(kFFTWTimeSeries);
        else
        {
            KTERROR(genlog, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
            return false;
        }

        // record size
        fRecordSize = node->GetData< UInt_t >("record-size", fRecordSize);

        ConfigureDerivedGenerator(node);

        return true;
    }

    Bool_t KTTSGenerator::Run()
    {
        // Create, signal, and destroy the egg header
        KTEggHeader* newHeader = CreateEggHeader();

        KTDEBUG(genlog, "Created header:\n"
             << "\tFilename: " << newHeader->GetFilename() << '\n'
             << "\tAcuisition Mode: " << newHeader->GetAcquisitionMode() << '\n'
             << "\tNumber of Channels: " << newHeader->GetNChannels() << '\n'
             << "\tSlice Size: " << newHeader->GetSliceSize() << '\n'
             << "\tRecord Size: " << newHeader->GetRecordSize() << '\n'
             << "\tRun Duration: " << newHeader->GetRunDuration() << " ms" << '\n'
             << "\tAcquisition Rate: " << newHeader->GetAcquisitionRate() << " Hz \n"
             << "\tTimestamp: " << newHeader->GetTimestamp() << '\n'
             << "\tDescription: " << newHeader->GetDescription() << '\n'
             << "\tRun Type: " << newHeader->GetRunType() << '\n'
             << "\tRun Source: " << newHeader->GetRunSource() << '\n'
             << "\tFormat Mode: " << newHeader->GetFormatMode());

        fHeaderSignal(newHeader);
        delete newHeader;

        // Loop over slices
        // The local copy of the data shared pointer is created and destroyed in each iteration of the loop
        for (fSliceCounter = 0; fSliceCounter < fNSlices; fSliceCounter++)
        {
            shared_ptr< KTData > newData = CreateNewData();

            if (! AddSliceHeader(*newData.get()))
            {
                KTERROR(genlog, "Something went wrong while adding the slice header");
                return false;
            }

            if (! AddEmptySlice(*newData.get()))
            {
                KTERROR(genlog, "Something went wrong while adding the empty slices");
                return false;
            }

            if (! newData->Has< KTTimeSeriesData >())
            {
                KTERROR(genlog, "New data does not contain time-series data!");
                return false;
            }

            GenerateTS(newData->Of< KTTimeSeriesData >());

            fDataSignal(newData);
        }

        fDoneSignal();

        KTProcSummary* summary = new KTProcSummary();
        summary->SetNSlicesProcessed(fSliceCounter);
        summary->SetNRecordsProcessed((UInt_t)ceil(Double_t(fSliceCounter * fSliceSize) / fRecordSize));
        summary->SetIntegratedTime(Double_t(fSliceCounter * fSliceSize) * fBinWidth);
        fSummarySignal(summary);
        delete summary;

        return true;
    }

    KTEggHeader* KTTSGenerator::CreateEggHeader() const
    {
        KTEggHeader* newHeader = new KTEggHeader();

        newHeader->SetFilename(fConfigName);
        //newHeader->SetAcquisitionMode();
        newHeader->SetNChannels(fNChannels);
        newHeader->SetSliceSize(fSliceSize);
        newHeader->SetRecordSize(fRecordSize);
        //newHeader->SetAcquisitionTime();
        newHeader->SetAcquisitionRate(1. / fBinWidth);

        return newHeader;
    }

    shared_ptr< KTData > KTTSGenerator::CreateNewData() const
    {
        shared_ptr<KTData> newData(new KTData());

        newData->fCounter = fSliceCounter;

        if (fSliceCounter == fNSlices - 1)
            newData->fLastData = true;

        return newData;
    }

    Bool_t KTTSGenerator::AddSliceHeader(KTData& data) const
    {
        KTSliceHeader& sliceHeader = data.Of< KTSliceHeader >().SetNComponents(1);
        sliceHeader.SetSampleRate(1. / fBinWidth);
        sliceHeader.SetSliceSize(fSliceSize);
        sliceHeader.CalculateBinWidthAndSliceLength();
        sliceHeader.SetTimeInRun(Double_t(fSliceCounter * fSliceSize) * fBinWidth);
        sliceHeader.SetSliceNumber(fSliceCounter);

        KTDEBUG(genlog, "Filled out slice header:\n"
                << "\tSample rate: " << sliceHeader.GetSampleRate() << " Hz\n"
                << "\tSlice size: " << sliceHeader.GetSliceSize() << '\n'
                << "\tBin width: " << sliceHeader.GetBinWidth() << " s\n"
                << "\tSlice length: " << sliceHeader.GetSliceLength() << " s\n"
                << "\tTime in run: " << sliceHeader.GetTimeInRun() << " s\n"
                << "\tSlice number: " << sliceHeader.GetSliceNumber() << '\n'
                << "\tStart record number: " << sliceHeader.GetStartRecordNumber() << '\n'
                << "\tStart sample number: " << sliceHeader.GetStartSampleNumber());

        return true;
    }

    Bool_t KTTSGenerator::AddEmptySlice(KTData& data) const
    {
        KTTimeSeriesData& tsData = data.Of< KTTimeSeriesData >().SetNComponents(fNChannels);

        for (UInt_t iChannel = 0; iChannel < fNChannels; iChannel++)
        {
            if (fTimeSeriesType == kRealTimeSeries)
            {
                tsData.SetTimeSeries(new KTTimeSeriesReal(fSliceSize, 0., Double_t(fSliceSize) * fBinWidth), iChannel);
            }
            else
            {
                tsData.SetTimeSeries(new KTTimeSeriesFFTW(fSliceSize, 0., Double_t(fSliceSize) * fBinWidth), iChannel);
            }
        }

        return true;
    }


} /* namespace Katydid */
