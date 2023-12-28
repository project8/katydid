/*
 * KT2ROOT.cc
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"

#include "logger.hh"

#include "KTClassifierResultsData.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTHoughData.hh"
#include "KTMultiTrackEventData.hh"
#include "KTPowerFitData.hh"
#include "KTFrequencySpectrumVariance.hh"
#include "KTPowerSpectrum.hh"
#include "KTProcessedMPTData.hh"
#include "KTProcessedTrackData.hh"
#include "KTRawTimeSeries.hh"
#include "KTTimeSeriesDist.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTSequentialLineData.hh"
#include "KTChannelAggregatedData.hh"
#include "KTMath.hh"

#include "CClassifierResultsData.hh"
#include "CMTEWithClassifierResultsData.hh"
#include "CProcessedMPTData.hh"
#include "CROOTData.hh"
#include "KTROOTData.hh"

#include "TClonesArray.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph2D.h"

#include <cfloat>
#include "stdint.h"

#ifndef UINT16_MAX
#define UINT16_MAX (32767)
#endif
//#include <stdint.h>

using std::string;

namespace Katydid
{
    LOGGER(dblog, "KT2ROOT");

    KT2ROOT::KT2ROOT()
    {
    }

    KT2ROOT::~KT2ROOT()
    {
    }

    TH1I* KT2ROOT::CreateHistogram(const KTVarTypePhysicalArray< uint64_t >* ts, const string& histName, int complexSampleIndex)
    {
        unsigned nBins = ts->size();
        TH1I* hist = new TH1I(histName.c_str(), "Raw Time Series", (int) nBins, ts->GetRangeMin(), ts->GetRangeMax());
        if (complexSampleIndex == -1)
        {
            for (unsigned iBin = 0; iBin < nBins; ++iBin)
            {
                hist->SetBinContent((int) iBin + 1, (*ts)(iBin));
            }
        }
        else if (complexSampleIndex == 0 || complexSampleIndex == 1)
        {
            for (unsigned iBin = 0; iBin < nBins; ++iBin)
            {
                hist->SetBinContent((int) iBin + 1, (*ts)(2 * iBin + complexSampleIndex));
            }
        }
        else
        {
            LERROR(dblog, "Invalid parameter for complexSampleIndex: <" << complexSampleIndex << ">; must be -1, 0, or 1");
        }
        //**** DEBUG ****//
        /*
         std::stringstream tsstream, histstream;
         for (unsigned i=0; i<10; ++i)
         {
         tsstream << (*ts)(i) << "  ";
         histstream << hist->GetBinContent((int)i+1) << "  ";
         }
         LWARN( dblog, "ts: " << tsstream.str() );
         LWARN( dblog, "hist: " << histstream.str() );
         */
        //**** DEBUG ****//
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (ADC)");
        return hist;

    }

    TH1I* KT2ROOT::CreateHistogram(const KTVarTypePhysicalArray< int64_t >* ts, const string& histName, int complexSampleIndex)
    {
        unsigned nBins = ts->size();
        TH1I* hist = new TH1I(histName.c_str(), "Raw Time Series", (int) nBins, ts->GetRangeMin(), ts->GetRangeMax());
        if (complexSampleIndex == -1)
        {
            for (unsigned iBin = 0; iBin < nBins; ++iBin)
            {
                hist->SetBinContent((int) iBin + 1, (*ts)(iBin));
            }
        }
        else if (complexSampleIndex == 0 || complexSampleIndex == 1)
        {
            for (unsigned iBin = 0; iBin < nBins; ++iBin)
            {
                hist->SetBinContent((int) iBin + 1, (*ts)(2 * iBin + complexSampleIndex));
            }
        }
        else
        {
            LERROR(dblog, "Invalid parameter for complexSampleIndex: <" << complexSampleIndex << ">; must be -1, 0, or 1");
        }
        //**** DEBUG ****//
        /*
         std::stringstream tsstream, histstream;
         for (unsigned i=0; i<10; ++i)
         {
         tsstream << (*ts)(i) << "  ";
         histstream << hist->GetBinContent((int)i+1) << "  ";
         }
         LWARN( dblog, "ts: " << tsstream.str() );
         LWARN( dblog, "hist: " << histstream.str() );
         */
        //**** DEBUG ****//
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (ADC)");
        return hist;

    }

    TH1I* KT2ROOT::CreateHistogram(const KTTimeSeriesDist* tsDist, const string& histName)
    {
        unsigned nBins = tsDist->size();
        TH1I* hist = new TH1I(histName.c_str(), "Time Series Distribution", (int) nBins, tsDist->GetRangeMin(), tsDist->GetRangeMax());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, (*tsDist)(iBin));
        }
        hist->SetXTitle("Amplitude");
        hist->SetYTitle("Occurrences (#)");
        return hist;
    }

    /*TH1I* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTRawTimeSeries* ts, const string& histName)
     {
     unsigned tMaxMag = 0;
     unsigned tMinMag = UINT16_MAX;//std::numeric_limits<int16_t>::max();
     unsigned nBins = ts->GetNBins();
     unsigned value;
     for (unsigned iBin=0; iBin<nBins; ++iBin)
     {
     value = (*ts)(iBin);
     //value *= value;
     if (value < tMinMag) tMinMag = value;
     if (value > tMaxMag) tMaxMag = value;
     }
     if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
     TH1I* hist = new TH1I(histName.c_str(), "Voltage Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
     for (unsigned iBin=0; iBin<nBins; ++iBin)
     {
     //value = (*this)(iBin);
     //hist->Fill(value*value);
     hist->Fill((*ts)(iBin));
     }
     hist->SetXTitle("Voltage (V)");
     return hist;
     }*/

    TH1D* KT2ROOT::CreateHistogram(const KTTimeSeriesFFTW* ts, const std::string& histName)
    {
        unsigned nBins = ts->GetNBins();
        TH1D* hist = new TH1D(histName.c_str(), "Time Series", (int) nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, ts->GetAbs(iBin));
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    static TH1D* CreateHistogramReal(const KTTimeSeriesFFTW* ts, const std::string& histName = "hTimeSeriesReal")
    {
        unsigned nBins = ts->GetNBins();
        TH1D* hist = new TH1D(histName.c_str(), "Time Series (Real)", (int) nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, ts->GetReal(iBin));
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    static TH1D* CreateHistogramImag(const KTTimeSeriesFFTW* ts, const std::string& histName = "hTimeSeriesImag")
    {
        unsigned nBins = ts->GetNBins();
        TH1D* hist = new TH1D(histName.c_str(), "Time Series (Imag)", (int) nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, ts->GetImag(iBin));
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    /*    TH1D* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTTimeSeriesFFTW* ts, const std::string& histName)
     {
     double tMaxMag = -DBL_MAX;
     double tMinMag = DBL_MAX;
     unsigned nBins = ts->GetNTimeBins();
     double value;
     for (unsigned iBin=0; iBin<nBins; ++iBin)
     {
     value = (*ts)(iBin)[0];
     //value *= value;
     if (value < tMinMag) tMinMag = value;
     if (value > tMaxMag) tMaxMag = value;
     }
     if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
     TH1D* hist = new TH1D(histName.c_str(), "Voltage Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
     for (unsigned iBin=0; iBin<nBins; ++iBin)
     {
     //value = (*this)(iBin);
     //hist->Fill(value*value);
     hist->Fill((*ts)(iBin)[0]);
     }
     hist->SetXTitle("Voltage (V)");
     return hist;
     }
     */

    TH1D* KT2ROOT::CreateHistogram(const KTTimeSeriesReal* ts, const std::string& histName)
    {
        unsigned nBins = ts->GetNBins();
        TH1D* hist = new TH1D(histName.c_str(), "Time Series", (int) nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, (*ts)(iBin));
        }
        //**** DEBUG ****//
        /**/
        std::stringstream tsstream, histstream;
        for (unsigned i = 0; i < 10; ++i)
        {
            tsstream << (*ts)(i) << "  ";
            histstream << hist->GetBinContent((int) i + 1) << "  ";
        }
        LWARN(dblog, "ts: " << tsstream.str());
        LWARN(dblog, "hist: " << histstream.str());
        /**/
        //**** DEBUG ****//
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    /*    TH1D* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTTimeSeriesReal* ts, const std::string& histName)
     {
     double tMaxMag = -DBL_MAX;
     double tMinMag = DBL_MAX;
     unsigned nBins = ts->GetNTimeBins();
     double value;
     for (unsigned iBin=0; iBin<nBins; ++iBin)
     {
     value = (*ts)(iBin);
     //value *= value;
     if (value < tMinMag) tMinMag = value;
     if (value > tMaxMag) tMaxMag = value;
     }
     if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
     TH1D* hist = new TH1D(histName.c_str(), "Voltage Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
     for (unsigned iBin=0; iBin<nBins; ++iBin)
     {
     //value = (*this)(iBin);
     //hist->Fill(value*value);
     hist->Fill((*ts)(iBin));
     }
     hist->SetXTitle("Voltage (V)");
     return hist;
     }
     */

    TH1D* KT2ROOT::CreateMagnitudeHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", (int) nBins, fs->GetRangeMin(), fs->GetRangeMax());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, (*fs)(iBin).abs());
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KT2ROOT::CreatePhaseHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Phase", (int) nBins, fs->GetRangeMin(), fs->GetRangeMax());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, (*fs)(iBin).arg());
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    TH1D* KT2ROOT::CreatePowerHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (int) nBins, fs->GetRangeMin(), fs->GetRangeMax());
        double value;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double) (fs->GetNTimeBins());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            value = (*fs)(iBin).abs();
            hist->SetBinContent((int) iBin + 1, value * value * scaling);
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Power (W)");
        return hist;
    }

    TH1D* KT2ROOT::CreateMagnitudeDistributionHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name)
    {
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        unsigned nBins = fs->size();
        double value;
        // Skip the DC bin: start at bin 1
        for (unsigned iBin = 1; iBin < nBins; ++iBin)
        {
            value = (*fs)(iBin).abs();
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Magnitude Distribution", 100, tMinMag * 0.95, tMaxMag * 1.05);
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->Fill((*fs)(iBin).abs());
        }
        hist->SetXTitle("Voltage (V)");
        return hist;
    }

    TH1D* KT2ROOT::CreatePowerDistributionHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name)
    {
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        unsigned nBins = fs->size();
        double value;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double) fs->GetNTimeBins();
        // Skip the DC bin: start at bin 1
        for (unsigned iBin = 1; iBin < nBins; ++iBin)
        {
            value = (*fs)(iBin).abs();
            value *= value * scaling;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag * 0.95, tMaxMag * 1.05);
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            value = (*fs)(iBin).abs();
            hist->Fill(value * value * scaling);
        }
        hist->SetXTitle("Power (W)");
        return hist;
    }

    TH1D* KT2ROOT::CreateMagnitudeHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", (int) nBins, fs->GetRangeMin(), fs->GetRangeMax());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, fs->GetAbs(iBin));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KT2ROOT::CreatePhaseHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Phase", (int) nBins, fs->GetRangeMin(), fs->GetRangeMax());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, fs->GetArg(iBin));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    TH1D* KT2ROOT::CreatePowerHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (int) nBins, fs->GetRangeMin(), fs->GetRangeMax());
        double value, valueImag, valueReal;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double) fs->GetNTimeBins();

        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, scaling * fs->GetNorm(iBin));
        }

        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Power (W)");
        return hist;
    }

    TH1D* KT2ROOT::CreateMagnitudeDistributionHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        double value;
        // skip the DC bin; start at iBin = 1
        for (unsigned iBin = 1; iBin < nBins; ++iBin)
        {
            value = fs->GetAbs(iBin);
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Magnitude Distribution", 100, tMinMag * 0.95, tMaxMag * 1.05);
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            value = fs->GetAbs(iBin);
            hist->Fill(value);
        }
        hist->SetXTitle("Voltage (V)");
        return hist;
    }

    TH1D* KT2ROOT::CreatePowerDistributionHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        double value;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double) fs->GetNTimeBins();
        // skip the DC bin; start at iBin = 1
        for (unsigned iBin = 1; iBin < nBins; ++iBin)
        {
            value = fs->GetNorm(iBin) * scaling;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag * 0.95, tMaxMag * 1.05);
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            value = fs->GetNorm(iBin);
            hist->Fill(value * scaling);
        }
        hist->SetXTitle("Power (W)");
        return hist;
    }

    std::vector<TGraph2D*> KT2ROOT::CreateGridGraphs(const KTAggregatedFrequencySpectrumDataFFTW& aggfs, const std::string& name)
    {
        unsigned nComponents = aggfs.GetNComponents();
        double fActiveRadius = aggfs.GetActiveRadius();
        unsigned nZ=aggfs.GetNAxialPositions();
        std::vector<TGraph2D*> graphs;
        std::vector<unsigned> pointIndex;
        pointIndex.assign(nZ,0);

        for (unsigned iZ=0; iZ<nZ; ++iZ)
        {
            graphs.push_back(new TGraph2D());
            graphs.at(iZ)->SetName((name+"_"+std::to_string(iZ)).c_str());
            graphs.at(iZ)->GetXaxis()->SetTitle("X Axis (m)");
            graphs.at(iZ)->GetYaxis()->SetTitle("Y Axis (m)");
        }

        for (unsigned iComponents = 0; iComponents < nComponents; ++iComponents)
        {
            double xPos,yPos,zPos;
            aggfs.GetGridPoint(iComponents, xPos, yPos, zPos);
            unsigned zIndex=(int)zPos;
            if(zIndex>nZ) LERROR(dblog, "The z index cannot be more than " << nZ-1);
            graphs[zIndex]->SetPoint(pointIndex[zIndex], xPos, yPos, aggfs.GetSummedGridVoltage(iComponents));
            pointIndex[zIndex]+=1;
        }
        return graphs;
    }

    std::vector<TGraph2D*> KT2ROOT::CreateGridGraphs(const KTAggregatedPowerSpectrumData& aggps, const std::string& name)
    {
        // Currently only assume a square grid
        unsigned nComponents = aggps.GetNComponents();
        double fActiveRadius = aggps.GetActiveRadius();
        unsigned nZ=aggps.GetNAxialPositions();
        std::vector<TGraph2D*> graphs;
        std::vector<unsigned> pointIndex;
        pointIndex.assign(nZ,0);

        for (unsigned iZ=0; iZ<nZ; ++iZ)
        {
            graphs.push_back(new TGraph2D());
            graphs.at(iZ)->SetName((name+"_"+std::to_string(iZ)).c_str());
            graphs.at(iZ)->GetXaxis()->SetTitle("X Axis (m)");
            graphs.at(iZ)->GetYaxis()->SetTitle("Y Axis (m)");
        }

        for (unsigned iComponents = 0; iComponents < nComponents; ++iComponents)
        {
            double xPos,yPos,zPos;
            aggps.GetGridPoint(iComponents, xPos, yPos, zPos);
            unsigned zIndex=(int)zPos;
            if(zIndex>nZ) LERROR(dblog, "The z index cannot be more than " << nZ-1);
            graphs[zIndex]->SetPoint(pointIndex[zIndex],xPos,yPos,aggps.GetSummedGridPower(iComponents));
            pointIndex[zIndex]+=1;
        }
        return graphs;
    }

    TH1D* KT2ROOT::CreateHistogram(const KTFrequencySpectrumVariance* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum Variance", (int) nBins, fs->GetRangeMin(), fs->GetRangeMax());
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            hist->SetBinContent((int) iBin + 1, (*fs)(iBin));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage^{2} (V^{2})");
        return hist;
    }

    TH1D* KT2ROOT::CreatePowerHistogram(const KTPowerSpectrum* ps, const std::string& name)
    {
        unsigned nBins = ps->size();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (int) nBins, ps->GetRangeMin(), ps->GetRangeMax());
        //double value;
        for (unsigned iBin = 0; iBin < nBins; iBin++)
        {
            hist->SetBinContent((int) iBin + 1, (*ps)(iBin));
        }
        hist->SetXTitle(ps->GetAxisLabel().c_str());
        hist->SetYTitle(ps->GetDataLabel().c_str());
        return hist;
    }

    TH1D* KT2ROOT::CreatePowerDistributionHistogram(const KTPowerSpectrum* ps, const std::string& name)
    {
        double value = (*ps)(0);
        double tMaxMag = value;
        double tMinMag = value;
        unsigned nBins = ps->size();
        for (unsigned iBin = 1; iBin < nBins; iBin++)
        {
            value = (*ps)(iBin);
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag * 0.95, tMaxMag * 1.05);
        for (unsigned iBin = 0; iBin < nBins; iBin++)
        {
            hist->Fill((*ps)(iBin));
        }
        hist->SetXTitle(ps->GetDataLabel().c_str());
        return hist;
    }

    TH2D* KT2ROOT::CreateHistogram(const KTPhysicalArray< 2, double >* ht, const std::string& histName)
    {
        TH2D* hist = new TH2D(histName.c_str(), histName.c_str(), ht->size(1), ht->GetRangeMin(1), ht->GetRangeMax(1), ht->size(2), ht->GetRangeMin(2), ht->GetRangeMax(2));

        for (int iBinX = 1; iBinX <= (int) ht->size(1); iBinX++)
        {
            for (int iBinY = 1; iBinY <= hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*ht)(iBinX - 1, iBinY - 1));
            }
        }

        return hist;

    }

    TH2D* KT2ROOT::CreatePowerHistogram(std::map< double, KTPowerSpectrum* > psColl, const std::string& histName)
    {
        std::map< double, KTPowerSpectrum* >::iterator it = psColl.begin();
        unsigned nBinsX = psColl.size(), nBinsY = it->second->size();

        double minTime = 1, maxTime = 0;
        double minFreq, maxFreq;

        KTPowerSpectrum* ps = it->second;
        minFreq = ps->GetRangeMin();
        maxFreq = ps->GetRangeMax();

        for (std::map< double, KTPowerSpectrum* >::iterator iter = psColl.begin(); it != psColl.end(); ++it)
        {
            if (it->first < minTime)
            {
                minTime = it->first;
            }
            if (it->first > maxTime)
            {
                maxTime = it->first;
            }
        }

        TH2D* hist = new TH2D(histName.c_str(), histName.c_str(), (int) nBinsX, minTime, maxTime, (int) nBinsY, minFreq, maxFreq);

        int iBinX = 1, iBinY = 1;
        for (it = psColl.begin(); it != psColl.end(); ++it)
        {
            for (iBinY = 1; iBinY <= nBinsY; iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*it->second)(iBinY - 1));
            }
            iBinX++;
        }

        return hist;

    }

    TH1D* KT2ROOT::CreateMagnitudeHistogram(const KTPowerFitData* pf, const std::string& histName)
    {
        std::map< unsigned, KTPowerFitData::Point >::iterator it;
        std::map< unsigned, KTPowerFitData::Point > SetOfPoints = pf->GetSetOfPoints();

        double minFreq, maxFreq;
        unsigned nBins = SetOfPoints.size();
        minFreq = SetOfPoints.begin()->second.fAbscissa;
        maxFreq = SetOfPoints.rbegin()->second.fAbscissa;

        TH1D* hist = new TH1D(histName.c_str(), histName.c_str(), (int) nBins, minFreq, maxFreq);

        int iBin = 1;
        for (it = SetOfPoints.begin(); it != SetOfPoints.end(); ++it)
        {
            hist->SetBinContent(iBin, it->second.fOrdinate);
            iBin++;
        }

        return hist;

    }

    void KT2ROOT::LoadProcTrackData(const KTProcessedTrackData& ptData, Cicada::TProcessedTrackData& rootPTData)
    {
        rootPTData.SetComponent(ptData.GetComponent());
        rootPTData.SetAcquisitionID(ptData.GetAcquisitionID());
        rootPTData.SetTrackID(ptData.GetTrackID());
        rootPTData.SetEventID(ptData.GetEventID());
        rootPTData.SetEventSequenceID(ptData.GetEventSequenceID());
        rootPTData.SetIsCut(ptData.GetIsCut());
        rootPTData.SetStartTimeInRunC(ptData.GetStartTimeInRunC());
        rootPTData.SetStartTimeInAcq(ptData.GetStartTimeInAcq());
        rootPTData.SetEndTimeInRunC(ptData.GetEndTimeInRunC());
        rootPTData.SetTimeLength(ptData.GetTimeLength());
        rootPTData.SetStartFrequency(ptData.GetStartFrequency());
        rootPTData.SetEndFrequency(ptData.GetEndFrequency());
        rootPTData.SetFrequencyWidth(ptData.GetFrequencyWidth());
        rootPTData.SetSlope(ptData.GetSlope());
        rootPTData.SetIntercept(ptData.GetIntercept());
        rootPTData.SetTotalPower(ptData.GetTotalPower());
        rootPTData.SetNTrackBins(ptData.GetNTrackBins());
        rootPTData.SetTotalTrackSNR(ptData.GetTotalTrackSNR());
        rootPTData.SetMaxTrackSNR(ptData.GetMaxTrackSNR());
        rootPTData.SetTotalWideTrackSNR(ptData.GetTotalWideTrackSNR());
        rootPTData.SetNTrackBins(ptData.GetNTrackBins());
        rootPTData.SetTotalTrackNUP(ptData.GetTotalTrackNUP());
        rootPTData.SetMaxTrackNUP(ptData.GetMaxTrackNUP());
        rootPTData.SetTotalWideTrackNUP(ptData.GetTotalWideTrackNUP());
        rootPTData.SetStartTimeInRunCSigma(ptData.GetStartTimeInRunCSigma());
        rootPTData.SetEndTimeInRunCSigma(ptData.GetEndTimeInRunCSigma());
        rootPTData.SetTimeLengthSigma(ptData.GetTimeLengthSigma());
        rootPTData.SetStartFrequencySigma(ptData.GetStartFrequencySigma());
        rootPTData.SetEndFrequencySigma(ptData.GetEndFrequencySigma());
        rootPTData.SetFrequencyWidthSigma(ptData.GetFrequencyWidthSigma());
        rootPTData.SetSlopeSigma(ptData.GetSlopeSigma());
        rootPTData.SetInterceptSigma(ptData.GetInterceptSigma());
        rootPTData.SetTotalPowerSigma(ptData.GetTotalPowerSigma());
        return;
    }

    void KT2ROOT::UnloadProcTrackData(KTProcessedTrackData& ptData, const Cicada::TProcessedTrackData& rootPTData)
    {
        ptData.SetComponent(rootPTData.GetComponent());
        ptData.SetAcquisitionID(rootPTData.GetAcquisitionID());
        ptData.SetTrackID(rootPTData.GetTrackID());
        ptData.SetEventID(rootPTData.GetEventID());
        ptData.SetEventSequenceID(rootPTData.GetEventSequenceID());
        ptData.SetIsCut(rootPTData.GetIsCut());
        ptData.SetStartTimeInRunC(rootPTData.GetStartTimeInRunC());
        ptData.SetStartTimeInAcq(rootPTData.GetStartTimeInAcq());
        ptData.SetEndTimeInRunC(rootPTData.GetEndTimeInRunC());
        ptData.SetTimeLength(rootPTData.GetTimeLength());
        ptData.SetStartFrequency(rootPTData.GetStartFrequency());
        ptData.SetEndFrequency(rootPTData.GetEndFrequency());
        ptData.SetFrequencyWidth(rootPTData.GetFrequencyWidth());
        ptData.SetSlope(rootPTData.GetSlope());
        ptData.SetIntercept(rootPTData.GetIntercept());
        ptData.SetTotalPower(rootPTData.GetTotalPower());
        ptData.SetNTrackBins(rootPTData.GetNTrackBins());
        ptData.SetTotalTrackSNR(rootPTData.GetTotalTrackSNR());
        ptData.SetMaxTrackSNR(rootPTData.GetMaxTrackSNR());
        ptData.SetTotalWideTrackSNR(rootPTData.GetTotalWideTrackSNR());
        ptData.SetNTrackBins(rootPTData.GetNTrackBins());
        ptData.SetTotalTrackNUP(rootPTData.GetTotalTrackNUP());
        ptData.SetMaxTrackNUP(rootPTData.GetMaxTrackNUP());
        ptData.SetTotalWideTrackNUP(rootPTData.GetTotalWideTrackNUP());
        ptData.SetStartTimeInRunCSigma(rootPTData.GetStartTimeInRunCSigma());
        ptData.SetEndTimeInRunCSigma(rootPTData.GetEndTimeInRunCSigma());
        ptData.SetTimeLengthSigma(rootPTData.GetTimeLengthSigma());
        ptData.SetStartFrequencySigma(rootPTData.GetStartFrequencySigma());
        ptData.SetEndFrequencySigma(rootPTData.GetEndFrequencySigma());
        ptData.SetFrequencyWidthSigma(rootPTData.GetFrequencyWidthSigma());
        ptData.SetSlopeSigma(rootPTData.GetSlopeSigma());
        ptData.SetInterceptSigma(rootPTData.GetInterceptSigma());
        ptData.SetTotalPowerSigma(rootPTData.GetTotalPowerSigma());
    }

    void KT2ROOT::LoadClassifierResultsData(const KTClassifierResultsData& crData, Cicada::TClassifierResultsData& rootCRData)
    {
        rootCRData.SetComponent(crData.GetComponent());
        rootCRData.SetMainCarrierHigh(crData.GetMainCarrierHigh());
        rootCRData.SetMainCarrierLow(crData.GetMainCarrierLow());
        rootCRData.SetSideBand(crData.GetSideBand());
        return;
    }

    void KT2ROOT::UnloadClassifierResultsData(KTClassifierResultsData& crData, const Cicada::TClassifierResultsData& rootCRData)
    {
        crData.SetComponent(rootCRData.GetComponent());
        crData.SetMainCarrierHigh(rootCRData.GetMainCarrierHigh());
        crData.SetMainCarrierLow(rootCRData.GetMainCarrierLow());
        crData.SetSideBand(rootCRData.GetSideBand());
        return;
    }

    void KT2ROOT::LoadMultiTrackEventData(const KTMultiTrackEventData& mteData, Cicada::TMultiTrackEventData& rootMTEData)
    {
        rootMTEData.SetComponent(mteData.GetComponent());
        rootMTEData.SetAcquisitionID(mteData.GetAcquisitionID());
        rootMTEData.SetEventID(mteData.GetEventID());
        rootMTEData.SetTotalEventSequences(mteData.GetTotalEventSequences());
        rootMTEData.SetStartTimeInRunC(mteData.GetStartTimeInRunC());
        rootMTEData.SetStartTimeInAcq(mteData.GetStartTimeInAcq());
        rootMTEData.SetEndTimeInRunC(mteData.GetEndTimeInRunC());
        rootMTEData.SetTimeLength(mteData.GetTimeLength());
        rootMTEData.SetStartFrequency(mteData.GetStartFrequency());
        rootMTEData.SetEndFrequency(mteData.GetEndFrequency());
        rootMTEData.SetMinimumFrequency(mteData.GetMinimumFrequency());
        rootMTEData.SetMaximumFrequency(mteData.GetMaximumFrequency());
        rootMTEData.SetFrequencyWidth(mteData.GetFrequencyWidth());
        rootMTEData.SetStartTimeInRunCSigma(mteData.GetStartTimeInRunCSigma());
        rootMTEData.SetEndTimeInRunCSigma(mteData.GetEndTimeInRunCSigma());
        rootMTEData.SetTimeLengthSigma(mteData.GetTimeLengthSigma());
        rootMTEData.SetStartFrequencySigma(mteData.GetStartFrequencySigma());
        rootMTEData.SetEndFrequencySigma(mteData.GetEndFrequencySigma());
        rootMTEData.SetFrequencyWidthSigma(mteData.GetFrequencyWidthSigma());
        rootMTEData.SetFirstTrackID(mteData.GetFirstTrackID());
        rootMTEData.SetFirstTrackTimeLength(mteData.GetFirstTrackTimeLength());
        rootMTEData.SetFirstTrackFrequencyWidth(mteData.GetFirstTrackFrequencyWidth());
        rootMTEData.SetFirstTrackSlope(mteData.GetFirstTrackSlope());
        rootMTEData.SetFirstTrackIntercept(mteData.GetFirstTrackIntercept());
        rootMTEData.SetFirstTrackTotalPower(mteData.GetFirstTrackTotalPower());
        rootMTEData.SetFirstTrackNTrackBins(mteData.GetFirstTrackNTrackBins());
        rootMTEData.SetFirstTrackTotalSNR(mteData.GetFirstTrackTotalSNR());
        rootMTEData.SetFirstTrackMaxSNR(mteData.GetFirstTrackMaxSNR());
        rootMTEData.SetFirstTrackTotalNUP(mteData.GetFirstTrackTotalNUP());
        rootMTEData.SetFirstTrackMaxNUP(mteData.GetFirstTrackMaxNUP());
        rootMTEData.SetFirstTrackTotalWideSNR(mteData.GetFirstTrackTotalWideSNR());
        rootMTEData.SetFirstTrackTotalWideNUP(mteData.GetFirstTrackTotalWideNUP());
        rootMTEData.SetUnknownEventTopology(mteData.GetUnknownEventTopology());
        Int_t nTracks = (Int_t) mteData.GetNTracks();
        TClonesArray* tracks = rootMTEData.GetTracks();
        tracks->Clear();
        tracks->Expand(nTracks);
        Int_t iTrack = 0;
        for (TrackSetCIt trIt = mteData.GetTracksBegin(); trIt != mteData.GetTracksEnd(); ++trIt)
        {
            Cicada::TProcessedTrackData* track = new ((*tracks)[iTrack]) Cicada::TProcessedTrackData;
            KT2ROOT::LoadProcTrackData(trIt->fProcTrack, *track);
            ++iTrack;
        }
        return;
    }

    void KT2ROOT::UnloadMultiTrackEventData(KTMultiTrackEventData& mteData, const Cicada::TMultiTrackEventData& rootMTEData)
    {
        mteData.ClearTracks(); // do this first, since it clears some of the member variables other than just fTracks
        mteData.SetComponent(rootMTEData.GetComponent());
        mteData.SetAcquisitionID(rootMTEData.GetAcquisitionID());
        mteData.SetEventID(rootMTEData.GetEventID());
        mteData.SetTotalEventSequences(rootMTEData.GetTotalEventSequences());
        mteData.SetStartTimeInRunC(rootMTEData.GetStartTimeInRunC());
        mteData.SetStartTimeInAcq(rootMTEData.GetStartTimeInAcq());
        mteData.SetEndTimeInRunC(rootMTEData.GetEndTimeInRunC());
        mteData.SetTimeLength(rootMTEData.GetTimeLength());
        mteData.SetStartFrequency(rootMTEData.GetStartFrequency());
        mteData.SetEndFrequency(rootMTEData.GetEndFrequency());
        mteData.SetMinimumFrequency(rootMTEData.GetMinimumFrequency());
        mteData.SetMaximumFrequency(rootMTEData.GetMaximumFrequency());
        mteData.SetFrequencyWidth(rootMTEData.GetFrequencyWidth());
        mteData.SetStartTimeInRunCSigma(rootMTEData.GetStartTimeInRunCSigma());
        mteData.SetEndTimeInRunCSigma(rootMTEData.GetEndTimeInRunCSigma());
        mteData.SetTimeLengthSigma(rootMTEData.GetTimeLengthSigma());
        mteData.SetStartFrequencySigma(rootMTEData.GetStartFrequencySigma());
        mteData.SetEndFrequencySigma(rootMTEData.GetEndFrequencySigma());
        mteData.SetFrequencyWidthSigma(rootMTEData.GetFrequencyWidthSigma());
        mteData.SetFirstTrackID(rootMTEData.GetFirstTrackID());
        mteData.SetFirstTrackTimeLength(rootMTEData.GetFirstTrackTimeLength());
        mteData.SetFirstTrackFrequencyWidth(rootMTEData.GetFirstTrackFrequencyWidth());
        mteData.SetFirstTrackSlope(rootMTEData.GetFirstTrackSlope());
        mteData.SetFirstTrackIntercept(rootMTEData.GetFirstTrackIntercept());
        mteData.SetFirstTrackTotalPower(rootMTEData.GetFirstTrackTotalPower());
        mteData.SetFirstTrackNTrackBins(rootMTEData.GetFirstTrackNTrackBins());
        mteData.SetFirstTrackTotalSNR(rootMTEData.GetFirstTrackTotalSNR());
        mteData.SetFirstTrackMaxSNR(rootMTEData.GetFirstTrackMaxSNR());
        mteData.SetFirstTrackTotalNUP(rootMTEData.GetFirstTrackTotalNUP());
        mteData.SetFirstTrackMaxNUP(rootMTEData.GetFirstTrackMaxNUP());
        mteData.SetFirstTrackTotalWideSNR(rootMTEData.GetFirstTrackTotalWideSNR());
        mteData.SetFirstTrackTotalWideNUP(rootMTEData.GetFirstTrackTotalWideNUP());
        mteData.SetUnknownEventTopology(rootMTEData.GetUnknownEventTopology());
        const TClonesArray* tracks = rootMTEData.GetTracks();
        Int_t nTracks = tracks->GetSize();
        KTProcessedTrackData track;
        for (Int_t iTrack = 0; iTrack < nTracks; ++iTrack)
        {
            KT2ROOT::UnloadProcTrackData(track, *(Cicada::TProcessedTrackData*) ((*tracks)[iTrack]));
            mteData.AddTrack(track);
        }
        return;
    }

    void KT2ROOT::LoadProcMPTData(const KTProcessedMPTData& mptData, Cicada::TProcessedMPTData& rootMPTData)
    {
        rootMPTData.SetComponent(mptData.GetComponent());
        KT2ROOT::LoadProcTrackData(mptData.GetMainTrack(), rootMPTData.MainTrack());
        rootMPTData.SetAxialFrequency(mptData.GetAxialFrequency());
        return;
    }
    void KT2ROOT::UnloadProcMPTData(KTProcessedMPTData& mptData, const Cicada::TProcessedMPTData& rootMPTData)
    {
        mptData.SetComponent(rootMPTData.GetComponent());
        KT2ROOT::UnloadProcTrackData(mptData.GetMainTrack(), rootMPTData.MainTrack());
        mptData.SetAxialFrequency(rootMPTData.GetAxialFrequency());
        return;
    }

    void KT2ROOT::LoadMTEWithClassifierResultsData(const KTMultiTrackEventData& mteData, Cicada::TMTEWithClassifierResultsData& rootMTECRData)
    {
        LoadMultiTrackEventData(mteData, rootMTECRData);

        Int_t nTracks = (Int_t) mteData.GetNTracks();
        TClonesArray* classifierResults = rootMTECRData.GetClassifierResults();
        classifierResults->Clear();
        classifierResults->Expand(nTracks);
        Int_t iTrack = 0;
        for (TrackSetCIt trIt = mteData.GetTracksBegin(); trIt != mteData.GetTracksEnd(); ++trIt)
        {
            Cicada::TClassifierResultsData* classifierResult = new ((*classifierResults)[iTrack]) Cicada::TClassifierResultsData;
            // get the classifier results data from the proc track object for loading
            KT2ROOT::LoadClassifierResultsData(trIt->fProcTrack.Of< KTClassifierResultsData >(), *classifierResult);
            ++iTrack;
        }
        return;
    }
    void KT2ROOT::UnloadMTEWithClassifierResultsData(KTMultiTrackEventData& mteData, const Cicada::TMTEWithClassifierResultsData& rootMTECRData)
    {
        UnloadMultiTrackEventData(mteData, rootMTECRData);

        const TClonesArray* classifierResults = rootMTECRData.GetClassifierResults();
        Int_t nTracks = classifierResults->GetSize();
        // loop over tracks that have already been unloaded so we can add to each track
        Int_t iTrack = 0;
        for (TrackSetCIt trIt = mteData.GetTracksBegin(); trIt != mteData.GetTracksEnd(); ++trIt)
        {
            // add the classifier results data to the proc track object
            KTClassifierResultsData& crData = trIt->fProcTrack.Of< KTClassifierResultsData >();
            KT2ROOT::UnloadClassifierResultsData(crData, *(Cicada::TClassifierResultsData*) ((*classifierResults)[iTrack]));
            ++iTrack;
        }
        return;
    }

    void KT2ROOT::LoadSparseWaterfallCandidateData(const KTSparseWaterfallCandidateData& swfData, TSparseWaterfallCandidateData& rootSWfData)
    {
        rootSWfData.SetComponent(swfData.GetComponent());
        rootSWfData.SetAcquisitionID(swfData.GetAcquisitionID());
        rootSWfData.SetCandidateID(swfData.GetCandidateID());
        rootSWfData.SetTimeInRunC(swfData.GetTimeInRunC());
        rootSWfData.SetTimeLength(swfData.GetTimeLength());
        rootSWfData.SetMinFrequency(swfData.GetMinFrequency());
        rootSWfData.SetMaxFrequency(swfData.GetMaxFrequency());
        rootSWfData.SetFrequencyWidth(swfData.GetFrequencyWidth());

        //
        Int_t nPoints = (Int_t) swfData.GetPoints().size();
        TClonesArray* points = rootSWfData.GetPoints();
        points->Clear();
        points->Expand(nPoints);
        Int_t iPoint = 0;
        for (KTDiscriminatedPoints::const_iterator pIt = swfData.GetPoints().begin(); pIt != swfData.GetPoints().end(); ++pIt)
        {
            Katydid::TDiscriminatedPoint* point = new ((*points)[iPoint]) Katydid::TDiscriminatedPoint;
            point->SetTimeInRunC(pIt->fTimeInRunC);
            point->SetFrequency(pIt->fFrequency);
            point->SetAmplitude(pIt->fAmplitude);
            point->SetTimeInAcq(pIt->fTimeInAcq);
            point->SetMean(pIt->fMean);
            point->SetVariance(pIt->fVariance);
            point->SetNeighborhoodAmplitude(pIt->fNeighborhoodAmplitude);
            point->SetBinInSlice(pIt->fBinInSlice);
            ++iPoint;
        }
        return;
    }

    // void KT2ROOT::UnloadSparseWaterfallCandidateData(KTSparseWaterfallCandidateData& swfData, const TSparseWaterfallCandidateData& rootSWfData)
    // {
    // TODO
    // return;
    // }

    void KT2ROOT::LoadSequentialLineData(const KTSequentialLineData& seqData, TSequentialLineData& rootSEQData)
    {
        rootSEQData.SetComponent(seqData.GetComponent());
        rootSEQData.SetAcquisitionID(seqData.GetAcquisitionID());
        rootSEQData.SetCandidateID(seqData.GetCandidateID());

        rootSEQData.SetStartTimeInRunC(seqData.GetStartTimeInRunC());
        rootSEQData.SetEndTimeInRunC(seqData.GetEndTimeInRunC());
        rootSEQData.SetStartTimeInAcq(seqData.GetStartTimeInAcq());
        rootSEQData.SetStartFrequency(seqData.GetStartFrequency());
        rootSEQData.SetEndFrequency(seqData.GetEndFrequency());
        rootSEQData.SetSlope(seqData.GetSlope());

        rootSEQData.SetTotalPower(seqData.GetTotalPower());
        rootSEQData.SetTotalTrackSNR(seqData.GetTotalSNR());
        rootSEQData.SetTotalTrackNUP(seqData.GetTotalNUP());
        rootSEQData.SetTotalWidePower(seqData.GetTotalWidePower());
        rootSEQData.SetTotalWideTrackSNR(seqData.GetTotalWideSNR());
        rootSEQData.SetTotalWideTrackNUP(seqData.GetTotalWideNUP());

        Int_t nPoints = (Int_t) seqData.GetLinePoints().size();
        TClonesArray* points = rootSEQData.GetPoints();
        points->Clear();
        points->Expand(nPoints);
        Int_t iPoint = 0;
        for (KTDiscriminatedPoints::const_iterator pIt = seqData.GetLinePoints().begin(); pIt != seqData.GetLinePoints().end(); ++pIt)
        {
            Katydid::TDiscriminatedPoint* point = new ((*points)[iPoint]) Katydid::TDiscriminatedPoint;
            point->SetTimeInRunC(pIt->fTimeInRunC);
            point->SetFrequency(pIt->fFrequency);
            point->SetAmplitude(pIt->fAmplitude);
            point->SetTimeInAcq(pIt->fTimeInAcq);
            point->SetMean(pIt->fMean);
            point->SetVariance(pIt->fVariance);
            point->SetNeighborhoodAmplitude(pIt->fNeighborhoodAmplitude);
            ++iPoint;
        }
        return;
    }

// void KT2ROOT::UnloadSequentialLineData(KTSequentialLineData& seqData, const TSequentialLineData& rootSEQData)
// {
// TODO
// return;
// }

} /* namespace Katydid */
