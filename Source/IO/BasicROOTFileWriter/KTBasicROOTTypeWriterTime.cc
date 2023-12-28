/*
 * KTBasicROOTTypeWriterTime.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"
#include "KTTIFactory.hh"
#include "logger.hh"
#include "KTSliceHeader.hh"
#include "KTRawTimeSeries.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesDist.hh"
#include "KTTimeSeriesDistData.hh"
#include "KTTimeSeriesFFTW.hh"

#include "TH1.h"

#include <sstream>
#include "KTBasicROOTTypeWriterTime.hh"



using std::stringstream;
using std::string;

namespace Katydid
{
    LOGGER(publog, "KTBasicROOTTypeWriterTime");

    static Nymph::KTTIRegistrar< KTBasicROOTTypeWriter, KTBasicROOTTypeWriterTime > sBRTWERegistrar;

    KTBasicROOTTypeWriterTime::KTBasicROOTTypeWriterTime() :
            KTBasicROOTTypeWriter()
            //KTTypeWriterTime()
    {
    }

    KTBasicROOTTypeWriterTime::~KTBasicROOTTypeWriterTime()
    {
    }


    void KTBasicROOTTypeWriterTime::RegisterSlots()
    {
        fWriter->RegisterSlot("raw-ts", this, &KTBasicROOTTypeWriterTime::WriteRawTimeSeriesData);
        //fWriter->RegisterSlot("raw-ts-dist", this, &KTBasicROOTTypeWriterTime::WriteRawTimeSeriesDataDistribution);
        fWriter->RegisterSlot("ts", this, &KTBasicROOTTypeWriterTime::WriteTimeSeriesData);
        fWriter->RegisterSlot("ts-dist", this, &KTBasicROOTTypeWriterTime::WriteTimeSeriesDataDistribution);
        fWriter->RegisterSlot("ts-fftw", this, &KTBasicROOTTypeWriterTime::WriteTimeSeriesFFTWData);
        return;
    }


    //*****************
    // Raw Time Series Data
    //*****************

    void KTBasicROOTTypeWriterTime::WriteRawTimeSeriesData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        KTSliceHeader& slHeader = data->Of<KTSliceHeader>();
        uint64_t sliceNumber = slHeader.GetSliceNumber();

        KTRawTimeSeriesData& tsData = data->Of<KTRawTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTRawTimeSeries* ts = tsData.GetTimeSeries(iComponent);
            if (ts != NULL)
            {
                if (ts->GetSampleSize() == 1)
                {
                    stringstream conv;
                    conv << "histRawTS_" << sliceNumber << "_" << iComponent;
                    string histName;
                    conv >> histName;
                    TH1I* tsHist = NULL;
                    if (slHeader.GetRawDataFormatType(iComponent) == sDigitizedUS)
                    {
                        tsHist = KT2ROOT::CreateHistogram(ts, histName);
                    }
                    else if(slHeader.GetRawDataFormatType(iComponent) == sDigitizedS)
                    {
                        KTVarTypePhysicalArray< int64_t > array(*ts, false);
                        tsHist = KT2ROOT::CreateHistogram(&array, histName);
                    }
                    tsHist->SetDirectory(fWriter->GetFile());
                    tsHist->Write();
                    LDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
                }
                else if (ts->GetSampleSize() == 2)
                {
                    stringstream convReal;
                    convReal << "histRawTSReal_" << sliceNumber << "_" << iComponent;
                    string histNameReal;
                    convReal >> histNameReal;
                    TH1I* tsHistReal = NULL;
                    if (slHeader.GetRawDataFormatType(iComponent) == sDigitizedUS)
                    {
                        tsHistReal = KT2ROOT::CreateHistogram(ts, histNameReal);
                    }
                    else if(slHeader.GetRawDataFormatType(iComponent) == sDigitizedS)
                    {
                        KTVarTypePhysicalArray< int64_t > array(*ts, false);
                        tsHistReal = KT2ROOT::CreateHistogram(&array, histNameReal);
                    }
                    tsHistReal->SetDirectory(fWriter->GetFile());
                    tsHistReal->Write();
                    LDEBUG(publog, "Histogram <" << histNameReal << "> written to ROOT file");

                    stringstream convImag;
                    convImag << "histRawTSImag_" << sliceNumber << "_" << iComponent;
                    string histNameImag;
                    convImag >> histNameImag;
                    TH1I* tsHistImag = NULL;
                    if (slHeader.GetRawDataFormatType(iComponent) == sDigitizedUS)
                    {
                        tsHistImag = KT2ROOT::CreateHistogram(ts, histNameImag);
                    }
                    else if(slHeader.GetRawDataFormatType(iComponent) == sDigitizedS)
                    {
                        KTVarTypePhysicalArray< int64_t > array(*ts, false);
                        tsHistImag = KT2ROOT::CreateHistogram(&array, histNameImag);
                    }
                    tsHistImag->SetDirectory(fWriter->GetFile());
                    tsHistImag->Write();
                    LDEBUG(publog, "Histogram <" << histNameImag << "> written to ROOT file");
                }
                else
                {
                    LWARN(publog, "Invalid sample size: " << ts->GetSampleSize());
                }
            }
        }
        return;
    }

    /*void KTBasicROOTTypeWriterTime::WriteRawTimeSeriesDataDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTRawTimeSeriesData& tsData = data->Of<KTRawTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTRawTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histRawTSDist_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                TH1I* powerSpectrum = KT2ROOT::CreateAmplitudeDistributionHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                LDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    */

    //*****************
    // Time Series Data
    //*****************

    void KTBasicROOTTypeWriterTime::WriteTimeSeriesData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* ts = tsData.GetTimeSeries(iComponent);
            if (ts != NULL)
            {
                stringstream conv;
                conv << "histTS_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                TH1D* tsHist = ts->CreateHistogram(histName);
                tsHist->SetDirectory(fWriter->GetFile());
                tsHist->Write();
                LDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterTime::WriteTimeSeriesDataDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesDistData& tsDistData = data->Of<KTTimeSeriesDistData>();
        unsigned nComponents = tsDistData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeriesDist* distribution = tsDistData.GetTimeSeriesDist(iComponent);
            if (distribution != NULL)
            {
                stringstream conv;
                conv << "histTSDist_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                TH1I* amplitudeSpectrum = KT2ROOT::CreateHistogram(distribution, histName);
                amplitudeSpectrum->SetDirectory(fWriter->GetFile());
                amplitudeSpectrum->Write();
                LDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }


    void KTBasicROOTTypeWriterTime::WriteTimeSeriesFFTWData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* ts = tsData.GetTimeSeries(iComponent);
            const KTTimeSeriesFFTW* tsFFTW = NULL;
            if (ts != NULL) tsFFTW = dynamic_cast< const KTTimeSeriesFFTW* >( ts );
            if (tsFFTW != NULL)
            {
                // Real component
                stringstream convReal;
                convReal << "histTSReal_" << sliceNumber << "_" << iComponent;
                string histNameReal;
                convReal >> histNameReal;
                TH1D* tsHistReal = ts->CreateHistogram(histNameReal);
                tsHistReal->SetDirectory(fWriter->GetFile());
                tsHistReal->Write();
                LDEBUG(publog, "Histogram <" << histNameReal << "> written to ROOT file");

                // Imaginary component
                stringstream convImag;
                convImag << "histTSImag_" << sliceNumber << "_" << iComponent;
                string histNameImag;
                convImag >> histNameImag;
                TH1D* tsHistImag = ts->CreateHistogram(histNameImag);
                tsHistImag->SetDirectory(fWriter->GetFile());
                tsHistImag->Write();
                LDEBUG(publog, "Histogram <" << histNameImag << "> written to ROOT file");
            }
        }
        return;
    }


} /* namespace Katydid */
