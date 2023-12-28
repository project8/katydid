/*
 * KTTSGenerator.cc
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#include "KTTSGenerator.hh"

#include "KTConstants.hh"
#include "KTEggHeader.hh"
#include "logger.hh"
#include "KTProcSummary.hh"
#include "param.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include "thorax.hh"

#include <cmath>



using std::string;

namespace Katydid
{
    LOGGER(genlog, "KTTSGenerator");

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

    bool KTTSGenerator::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        // set the number of slices to create
        fNSlices = node->get_value< unsigned >("number-of-slices", fNSlices);

        // number of slices
        fNChannels = node->get_value< unsigned >("n-channels", fNChannels);

        // specify the length of the time series
        fSliceSize = node->get_value< unsigned >("slice-size", fSliceSize);
        // record size, after slice size
        fRecordSize = node->get_value< unsigned >("record-size", fSliceSize);

        fBinWidth = node->get_value< double >("bin-width", fBinWidth);

        // type of time series
        string timeSeriesTypeString = node->get_value("time-series-type", "real");
        if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
        else if (timeSeriesTypeString == "fftw") SetTimeSeriesType(kFFTWTimeSeries);
        else
        {
            LERROR(genlog, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
            return false;
        }

        ConfigureDerivedGenerator(node);

        return true;
    }

    bool KTTSGenerator::Run()
    {
        // Create, signal, and destroy the egg header
        KTEggHeader* newHeader = CreateEggHeader();

        LDEBUG(genlog, "Created header:\n" << *newHeader);

        fHeaderSignal(newHeader);
        delete newHeader;

        // Loop over slices
        // The local copy of the data shared pointer is created and destroyed in each iteration of the loop
        for (fSliceCounter = 0; fSliceCounter < fNSlices; ++fSliceCounter)
        {
            Nymph::KTDataPtr newData = CreateNewData();

            if (! AddSliceHeader(*newData.get()))
            {
                LERROR(genlog, "Something went wrong while adding the slice header");
                return false;
            }

            if (! AddEmptySlice(*newData.get()))
            {
                LERROR(genlog, "Something went wrong while adding the empty slices");
                return false;
            }

            if (! newData->Has< KTTimeSeriesData >())
            {
                LERROR(genlog, "New data does not contain time-series data!");
                return false;
            }

            GenerateTS(newData->Of< KTTimeSeriesData >());

            fDataSignal(newData);
        }

        fDoneSignal();

        KTProcSummary* summary = new KTProcSummary();
        summary->SetNSlicesProcessed(fSliceCounter);
        summary->SetNRecordsProcessed((unsigned)ceil(double(fSliceCounter * fSliceSize) / fRecordSize));
        summary->SetIntegratedTime(double(fSliceCounter * fSliceSize) * fBinWidth);
        fSummarySignal(summary);
        delete summary;

        return true;
    }

    KTEggHeader* KTTSGenerator::CreateEggHeader() const
    {
        KTEggHeader* newHeader = new KTEggHeader();

        newHeader->SetFilename(fConfigName);
        newHeader->SetAcquisitionMode(fNChannels);
        newHeader->SetNChannels(fNChannels);
        newHeader->SetRunDuration(fSliceSize * fNSlices * fBinWidth * 1000); /// the factor of 1000 is to convert from s to ms; fBinWidth is in seconds, but RunDuration is supposed to be in ms
        newHeader->SetAcquisitionRate(1. / fBinWidth);
        //newHeader->SetDescription();
        //newHeader->SetRunType();

        for (unsigned iChannel = 0; iChannel < fNChannels; ++iChannel)
        {
            KTChannelHeader* newChanHeader = new KTChannelHeader();
            newChanHeader->SetNumber(iChannel);
            newChanHeader->SetSource("KTTSGenerator");
            newChanHeader->SetRawSliceSize(fSliceSize);
            newChanHeader->SetSliceSize(fSliceSize);
            newChanHeader->SetSliceStride(fSliceSize);
            newChanHeader->SetRecordSize(fRecordSize);
            newChanHeader->SetSampleSize(1);
            newChanHeader->SetDataTypeSize(1);
            newChanHeader->SetDataFormat(sDigitizedUS);
            newChanHeader->SetBitDepth(8);
            newChanHeader->SetVoltageOffset(-0.25);
            newChanHeader->SetVoltageRange(0.5);
            newChanHeader->SetDACGain(newChanHeader->GetVoltageRange() / (double)(1 << newChanHeader->GetBitDepth()));
            newHeader->SetChannelHeader(newChanHeader, iChannel);

        }

        newHeader->SetTimestamp(get_absolute_time_string());

        return newHeader;
    }

    Nymph::KTDataPtr KTTSGenerator::CreateNewData() const
    {
        Nymph::KTDataPtr newData(new Nymph::KTData());

        newData->SetCounter(fSliceCounter);

        if (fSliceCounter == fNSlices - 1)
            newData->SetLastData(true);

        return newData;
    }

    bool KTTSGenerator::AddSliceHeader(Nymph::KTData& data) const
    {
        KTSliceHeader& sliceHeader = data.Of< KTSliceHeader >().SetNComponents(1);
        sliceHeader.SetSampleRate(1. / fBinWidth);
        sliceHeader.SetSliceSize(fSliceSize);
        sliceHeader.SetRawSliceSize(fSliceSize);
        sliceHeader.CalculateBinWidthAndSliceLength();
        sliceHeader.SetTimeInRun(double(fSliceCounter * fSliceSize) * fBinWidth);
        sliceHeader.SetSliceNumber(fSliceCounter);

        for (unsigned iComponent = 0; iComponent < fNChannels; ++iComponent)
        {
            sliceHeader.SetTimeStamp((uint64_t)(sliceHeader.GetTimeInRun() * (double)NSEC_PER_SEC), iComponent); // TODO: change this to 1e3 when switch to usec is made
            sliceHeader.SetAcquisitionID(0);
            sliceHeader.SetRecordID(0);
        }

        LDEBUG(genlog, "Filled out slice header:\n"
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

    bool KTTSGenerator::AddEmptySlice(Nymph::KTData& data) const
    {
        KTTimeSeriesData& tsData = data.Of< KTTimeSeriesData >().SetNComponents(fNChannels);

        for (unsigned iChannel = 0; iChannel < fNChannels; ++iChannel)
        {
            if (fTimeSeriesType == kRealTimeSeries)
            {
                tsData.SetTimeSeries(new KTTimeSeriesReal(0., fSliceSize, 0., double(fSliceSize) * fBinWidth), iChannel);
            }
            else
            {
                tsData.SetTimeSeries(new KTTimeSeriesFFTW({0., 0.}, fSliceSize, 0., double(fSliceSize) * fBinWidth), iChannel);
            }
        }

        return true;
    }


} /* namespace Katydid */
