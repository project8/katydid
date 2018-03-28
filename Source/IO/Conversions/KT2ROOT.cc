/*
 * KT2ROOT.cc
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"

#include "KTLogger.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTHoughData.hh"
#include "KTMultiTrackEventData.hh"
#include "KTPowerFitData.hh"
#include "KTFrequencySpectrumVariance.hh"
#include "KTPowerSpectrum.hh"
#include "KTProcessedTrackData.hh"
#include "KTRawTimeSeries.hh"
#include "KTTimeSeriesDist.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include "CROOTData.hh"

#include "TClonesArray.h"
#include "TH1.h"
#include "TH2.h"

#include <cfloat>
#include "stdint.h"
#ifndef UINT16_MAX
#define UINT16_MAX (32767)
#endif
//#include <stdint.h>

using std::string;

namespace Katydid
{
    KTLOGGER(dblog, "KT2ROOT");

    KT2ROOT::KT2ROOT()
    {
    }

    KT2ROOT::~KT2ROOT()
    {
    }

    TH1I* KT2ROOT::CreateHistogram(const KTVarTypePhysicalArray< uint64_t >* ts, const string& histName, int complexSampleIndex)
    {
        unsigned nBins = ts->size();
        TH1I* hist = new TH1I(histName.c_str(), "Raw Time Series", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        if (complexSampleIndex == -1)
        {
            for (unsigned iBin=0; iBin<nBins; ++iBin)
            {
                hist->SetBinContent((int)iBin+1, (*ts)(iBin));
            }
        }
        else if (complexSampleIndex == 0 || complexSampleIndex == 1)
        {
            for (unsigned iBin=0; iBin<nBins; ++iBin)
            {
                hist->SetBinContent((int)iBin+1, (*ts)(2 * iBin + complexSampleIndex));
            }
        }
        else
        {
            KTERROR(dblog, "Invalid parameter for complexSampleIndex: <" << complexSampleIndex << ">; must be -1, 0, or 1");
        }
        //**** DEBUG ****//
        /*
        std::stringstream tsstream, histstream;
        for (unsigned i=0; i<10; ++i)
        {
            tsstream << (*ts)(i) << "  ";
            histstream << hist->GetBinContent((int)i+1) << "  ";
        }
        KTWARN( dblog, "ts: " << tsstream.str() );
        KTWARN( dblog, "hist: " << histstream.str() );
        */
        //**** DEBUG ****//
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (ADC)");
        return hist;

    }

    TH1I* KT2ROOT::CreateHistogram(const KTVarTypePhysicalArray< int64_t >* ts, const string& histName, int complexSampleIndex)
    {
        unsigned nBins = ts->size();
        TH1I* hist = new TH1I(histName.c_str(), "Raw Time Series", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        if (complexSampleIndex == -1)
        {
            for (unsigned iBin=0; iBin<nBins; ++iBin)
            {
                hist->SetBinContent((int)iBin+1, (*ts)(iBin));
            }
        }
        else if (complexSampleIndex == 0 || complexSampleIndex == 1)
        {
            for (unsigned iBin=0; iBin<nBins; ++iBin)
            {
                hist->SetBinContent((int)iBin+1, (*ts)(2 * iBin + complexSampleIndex));
            }
        }
        else
        {
            KTERROR(dblog, "Invalid parameter for complexSampleIndex: <" << complexSampleIndex << ">; must be -1, 0, or 1");
        }
        //**** DEBUG ****//
        /*
        std::stringstream tsstream, histstream;
        for (unsigned i=0; i<10; ++i)
        {
            tsstream << (*ts)(i) << "  ";
            histstream << hist->GetBinContent((int)i+1) << "  ";
        }
        KTWARN( dblog, "ts: " << tsstream.str() );
        KTWARN( dblog, "hist: " << histstream.str() );
        */
        //**** DEBUG ****//
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (ADC)");
        return hist;

    }

    TH1I* KT2ROOT::CreateHistogram(const KTTimeSeriesDist* tsDist, const string& histName)
    {
        unsigned nBins = tsDist->size();
        TH1I* hist = new TH1I(histName.c_str(), "Time Series Distribution", (int)nBins, tsDist->GetRangeMin(), tsDist->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*tsDist)(iBin));
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
        TH1D* hist = new TH1D(histName.c_str(), "Time Series", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, ::sqrt((*ts)(iBin)[0]*(*ts)(iBin)[0] + (*ts)(iBin)[1]*(*ts)(iBin)[1]));
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    static TH1D* CreateHistogramReal(const KTTimeSeriesFFTW* ts, const std::string& histName = "hTimeSeriesReal")
    {
        unsigned nBins = ts->GetNBins();
        TH1D* hist = new TH1D(histName.c_str(), "Time Series (Real)", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*ts)(iBin)[0]);
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    static TH1D* CreateHistogramImag(const KTTimeSeriesFFTW* ts, const std::string& histName = "hTimeSeriesImag")
    {
        unsigned nBins = ts->GetNBins();
        TH1D* hist = new TH1D(histName.c_str(), "Time Series (Imag)", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*ts)(iBin)[1]);
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
        TH1D* hist = new TH1D(histName.c_str(), "Time Series", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*ts)(iBin));
        }
        //**** DEBUG ****//
        /**/
        std::stringstream tsstream, histstream;
        for (unsigned i=0; i<10; ++i)
        {
            tsstream << (*ts)(i) << "  ";
            histstream << hist->GetBinContent((int)i+1) << "  ";
        }
        KTWARN( dblog, "ts: " << tsstream.str() );
        KTWARN( dblog, "hist: " << histstream.str() );
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
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", (int)nBins, fs->GetRangeMin(), fs->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*fs)(iBin).abs());
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KT2ROOT::CreatePhaseHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Phase", (int)nBins, fs->GetRangeMin(), fs->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*fs)(iBin).arg());
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    TH1D* KT2ROOT::CreatePowerHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (int)nBins, fs->GetRangeMin(), fs->GetRangeMax());
        double value;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)(fs->GetNTimeBins());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            value = (*fs)(iBin).abs();
            hist->SetBinContent((int)iBin + 1, value * value * scaling);
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
        for (unsigned iBin=1; iBin<nBins; ++iBin)
        {
            value = (*fs)(iBin).abs();
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Magnitude Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
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
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)fs->GetNTimeBins();
        // Skip the DC bin: start at bin 1
        for (unsigned iBin=1; iBin<nBins; ++iBin)
        {
            value = (*fs)(iBin).abs();
            value *= value * scaling;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
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
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", (int)nBins, fs->GetRangeMin(), fs->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, std::sqrt((*fs)(iBin)[0] * (*fs)(iBin)[0] + (*fs)(iBin)[1] * (*fs)(iBin)[1]));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KT2ROOT::CreatePhaseHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Phase", (int)nBins, fs->GetRangeMin(), fs->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, std::atan2((*fs)(iBin)[1], (*fs)(iBin)[0]));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    TH1D* KT2ROOT::CreatePowerHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (int)nBins, fs->GetRangeMin(), fs->GetRangeMax());
        double value, valueImag, valueReal;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)fs->GetNTimeBins();

        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, scaling * ((*fs)(iBin)[0] * (*fs)(iBin)[0] + (*fs)(iBin)[1] * (*fs)(iBin)[1]));
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
        for (unsigned iBin=1; iBin<nBins; ++iBin)
        {
            value = std::sqrt((*fs)(iBin)[0] * (*fs)(iBin)[0] + (*fs)(iBin)[1] * (*fs)(iBin)[1]);
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Magnitude Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            value = sqrt((*fs)(iBin)[0] * (*fs)(iBin)[0] + (*fs)(iBin)[1] * (*fs)(iBin)[1]);
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
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)fs->GetNTimeBins();
        // skip the DC bin; start at iBin = 1
        for (unsigned iBin=1; iBin<nBins; ++iBin)
        {
            value = ((*fs)(iBin)[0] * (*fs)(iBin)[0] + (*fs)(iBin)[1] * (*fs)(iBin)[1]) * scaling;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            value = (*fs)(iBin)[0] * (*fs)(iBin)[0] + (*fs)(iBin)[1] * (*fs)(iBin)[1];
            hist->Fill(value * scaling);
        }
        hist->SetXTitle("Power (W)");
        return hist;
    }

    TH1D* KT2ROOT::CreateHistogram(const KTFrequencySpectrumVariance* fs, const std::string& name)
    {
        unsigned nBins = fs->size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum Variance", (int)nBins, fs->GetRangeMin(), fs->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*fs)(iBin));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage^{2} (V^{2})");
        return hist;
    }

    TH1D* KT2ROOT::CreatePowerHistogram(const KTPowerSpectrum* ps, const std::string& name)
    {
        unsigned nBins = ps->size();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (int)nBins, ps->GetRangeMin(), ps->GetRangeMax());
        //double value;
        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((int)iBin+1, (*ps)(iBin));
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
        for (unsigned iBin=1; iBin<nBins; iBin++)
        {
            value = (*ps)(iBin);
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            hist->Fill((*ps)(iBin));
        }
        hist->SetXTitle(ps->GetDataLabel().c_str());
        return hist;
    }

    TH2D* KT2ROOT::CreateHistogram(const KTPhysicalArray< 2, double >* ht, const std::string& histName)
    {
        TH2D* hist = new TH2D(histName.c_str(), histName.c_str(),
                ht->size(1), ht->GetRangeMin(1), ht->GetRangeMax(1),
                ht->size(2), ht->GetRangeMin(2), ht->GetRangeMax(2));

        for (int iBinX=1; iBinX<=(int)ht->size(1); iBinX++)
        {
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*ht)(iBinX-1, iBinY-1));
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

        for( std::map< double, KTPowerSpectrum* >::iterator iter = psColl.begin(); it != psColl.end(); ++it)
        {
            if( it->first < minTime )
            {
                minTime = it->first;
            }
            if( it->first > maxTime )
            {
                maxTime = it->first;
            }
        }

        TH2D* hist = new TH2D(histName.c_str(), histName.c_str(),
                (int)nBinsX, minTime, maxTime,
                (int)nBinsY, minFreq, maxFreq);

        int iBinX = 1, iBinY = 1;
        for (it = psColl.begin(); it != psColl.end(); ++it)
        {
            for(iBinY = 1; iBinY <= nBinsY; iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*it->second)(iBinY-1));
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

        TH1D* hist = new TH1D(histName.c_str(), histName.c_str(),
                (int)nBins, minFreq, maxFreq);

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
        rootPTData.SetComponent(ptData.GetComponent()); rootPTData.SetAcquisitionID(ptData.GetAcquisitionID()); rootPTData.SetTrackID(ptData.GetTrackID()); rootPTData.SetEventID(ptData.GetEventID()); rootPTData.SetEventSequenceID(ptData.GetEventSequenceID()); rootPTData.SetIsCut(ptData.GetIsCut());
        rootPTData.SetStartTimeInRunC(ptData.GetStartTimeInRunC()); rootPTData.SetStartTimeInAcq(ptData.GetStartTimeInAcq()); rootPTData.SetEndTimeInRunC(ptData.GetEndTimeInRunC());rootPTData.SetTimeLength(ptData.GetTimeLength());
        rootPTData.SetStartFrequency(ptData.GetStartFrequency()); rootPTData.SetEndFrequency(ptData.GetEndFrequency()); rootPTData.SetFrequencyWidth(ptData.GetFrequencyWidth());
        rootPTData.SetSlope(ptData.GetSlope()); rootPTData.SetIntercept(ptData.GetIntercept()); rootPTData.SetTotalPower(ptData.GetTotalPower());
        rootPTData.SetStartTimeInRunCSigma(ptData.GetStartTimeInRunCSigma()); rootPTData.SetEndTimeInRunCSigma(ptData.GetEndTimeInRunCSigma()); rootPTData.SetTimeLengthSigma(ptData.GetTimeLengthSigma());
        rootPTData.SetStartFrequencySigma(ptData.GetStartFrequencySigma()); rootPTData.SetEndFrequencySigma(ptData.GetEndFrequencySigma()); rootPTData.SetFrequencyWidthSigma(ptData.GetFrequencyWidthSigma());
        rootPTData.SetSlopeSigma(ptData.GetSlopeSigma()); rootPTData.SetInterceptSigma(ptData.GetInterceptSigma()); rootPTData.SetTotalPowerSigma(ptData.GetTotalPowerSigma());
        return;
    }

    void KT2ROOT::UnloadProcTrackData(KTProcessedTrackData& ptData, const Cicada::TProcessedTrackData& rootPTData)
    {
        ptData.SetComponent(rootPTData.GetComponent()); ptData.SetAcquisitionID(rootPTData.GetAcquisitionID()); ptData.SetTrackID(rootPTData.GetTrackID()); ptData.SetEventID(rootPTData.GetEventID()); ptData.SetEventSequenceID(rootPTData.GetEventSequenceID()); ptData.SetIsCut(rootPTData.GetIsCut());
        ptData.SetStartTimeInRunC(rootPTData.GetStartTimeInRunC()); ptData.SetStartTimeInAcq(rootPTData.GetStartTimeInAcq()); ptData.SetEndTimeInRunC(rootPTData.GetEndTimeInRunC()); ptData.SetTimeLength(rootPTData.GetTimeLength());
        ptData.SetStartFrequency(rootPTData.GetStartFrequency()); ptData.SetEndFrequency(rootPTData.GetEndFrequency()); ptData.SetFrequencyWidth(rootPTData.GetFrequencyWidth());
        ptData.SetSlope(rootPTData.GetSlope()); ptData.SetIntercept(rootPTData.GetIntercept()); ptData.SetTotalPower(rootPTData.GetTotalPower());
        ptData.SetStartTimeInRunCSigma(rootPTData.GetStartTimeInRunCSigma()); ptData.SetEndTimeInRunCSigma(rootPTData.GetEndTimeInRunCSigma()); ptData.SetTimeLengthSigma(rootPTData.GetTimeLengthSigma());
        ptData.SetStartFrequencySigma(rootPTData.GetStartFrequencySigma()); ptData.SetEndFrequencySigma(rootPTData.GetEndFrequencySigma()); ptData.SetFrequencyWidthSigma(rootPTData.GetFrequencyWidthSigma());
        ptData.SetSlopeSigma(rootPTData.GetSlopeSigma()); ptData.SetInterceptSigma(rootPTData.GetInterceptSigma()); ptData.SetTotalPowerSigma(rootPTData.GetTotalPowerSigma());
    }

    void KT2ROOT::LoadMultiTrackEventData(const KTMultiTrackEventData& mteData, Cicada::TMultiTrackEventData& rootMTEData)
    {
        rootMTEData.SetComponent(mteData.GetComponent()); rootMTEData.SetAcquisitionID(mteData.GetAcquisitionID()); rootMTEData.SetEventID(mteData.GetEventID()); rootMTEData.SetTotalEventSequences(mteData.GetTotalEventSequences());
        rootMTEData.SetStartTimeInRunC(mteData.GetStartTimeInRunC()); rootMTEData.SetStartTimeInAcq(mteData.GetStartTimeInAcq()); rootMTEData.SetEndTimeInRunC(mteData.GetEndTimeInRunC()); rootMTEData.SetTimeLength(mteData.GetTimeLength());
        rootMTEData.SetStartFrequency(mteData.GetStartFrequency()); rootMTEData.SetEndFrequency(mteData.GetEndFrequency()); rootMTEData.SetMinimumFrequency(mteData.GetMinimumFrequency()); rootMTEData.SetMaximumFrequency(mteData.GetMaximumFrequency()); rootMTEData.SetFrequencyWidth(mteData.GetFrequencyWidth());
        rootMTEData.SetStartTimeInRunCSigma(mteData.GetStartTimeInRunCSigma()); rootMTEData.SetEndTimeInRunCSigma(mteData.GetEndTimeInRunCSigma()); rootMTEData.SetTimeLengthSigma(mteData.GetTimeLengthSigma());
        rootMTEData.SetStartFrequencySigma(mteData.GetStartFrequencySigma()); rootMTEData.SetEndFrequencySigma(mteData.GetEndFrequencySigma()); rootMTEData.SetFrequencyWidthSigma(mteData.GetFrequencyWidthSigma());
        rootMTEData.SetFirstTrackID(mteData.GetFirstTrackID()); rootMTEData.SetFirstTrackTimeLength(mteData.GetFirstTrackTimeLength()); rootMTEData.SetFirstTrackFrequencyWidth(mteData.GetFirstTrackFrequencyWidth()); rootMTEData.SetFirstTrackSlope(mteData.GetFirstTrackSlope()); rootMTEData.SetFirstTrackIntercept(mteData.GetFirstTrackIntercept()); rootMTEData.SetFirstTrackTotalPower(mteData.GetFirstTrackTotalPower());
        rootMTEData.SetUnknownEventTopology(mteData.GetUnknownEventTopology());
        Int_t nTracks = (Int_t)mteData.GetNTracks();
        TClonesArray* tracks = rootMTEData.GetTracks();
        tracks->Clear(); tracks->Expand(nTracks);
        Int_t iTrack = 0;
        for (TrackSetCIt trIt = mteData.GetTracksBegin(); trIt != mteData.GetTracksEnd(); ++trIt)
        {
            Cicada::TProcessedTrackData* track = new((*tracks)[iTrack]) Cicada::TProcessedTrackData;
            KT2ROOT::LoadProcTrackData(*trIt, *track);
            ++iTrack;
        }
        return;
    }

    void KT2ROOT::UnloadMultiTrackEventData(KTMultiTrackEventData& mteData, const Cicada::TMultiTrackEventData& rootMTEData)
    {
        mteData.ClearTracks(); // do this first, since it clears some of the member variables other than just fTracks
        mteData.SetComponent(rootMTEData.GetComponent()); mteData.SetAcquisitionID(rootMTEData.GetAcquisitionID()); mteData.SetEventID(rootMTEData.GetEventID()); mteData.SetTotalEventSequences(rootMTEData.GetTotalEventSequences());
        mteData.SetStartTimeInRunC(rootMTEData.GetStartTimeInRunC()); mteData.SetStartTimeInAcq(rootMTEData.GetStartTimeInAcq()); mteData.SetEndTimeInRunC(rootMTEData.GetEndTimeInRunC()); mteData.SetTimeLength(rootMTEData.GetTimeLength());
        mteData.SetStartFrequency(rootMTEData.GetStartFrequency()); mteData.SetEndFrequency(rootMTEData.GetEndFrequency()); mteData.SetMinimumFrequency(rootMTEData.GetMinimumFrequency()); mteData.SetMaximumFrequency(rootMTEData.GetMaximumFrequency()); mteData.SetFrequencyWidth(rootMTEData.GetFrequencyWidth());
        mteData.SetStartTimeInRunCSigma(rootMTEData.GetStartTimeInRunCSigma()); mteData.SetEndTimeInRunCSigma(rootMTEData.GetEndTimeInRunCSigma()); mteData.SetTimeLengthSigma(rootMTEData.GetTimeLengthSigma());
        mteData.SetStartFrequencySigma(rootMTEData.GetStartFrequencySigma()); mteData.SetEndFrequencySigma(rootMTEData.GetEndFrequencySigma()); mteData.SetFrequencyWidthSigma(rootMTEData.GetFrequencyWidthSigma());
        mteData.SetFirstTrackID(rootMTEData.GetFirstTrackID()); mteData.SetFirstTrackTimeLength(rootMTEData.GetFirstTrackTimeLength()); mteData.SetFirstTrackFrequencyWidth(rootMTEData.GetFirstTrackFrequencyWidth()); mteData.SetFirstTrackSlope(rootMTEData.GetFirstTrackSlope()); mteData.SetFirstTrackIntercept(rootMTEData.GetFirstTrackIntercept()); mteData.SetFirstTrackTotalPower(rootMTEData.GetFirstTrackTotalPower());
        mteData.SetUnknownEventTopology(rootMTEData.GetUnknownEventTopology());
        const TClonesArray* tracks = rootMTEData.GetTracks();
        Int_t nTracks = tracks->GetSize();
        KTProcessedTrackData track;
        for (Int_t iTrack = 0; iTrack < nTracks; ++iTrack)
        {
            KT2ROOT::UnloadProcTrackData(track, *(Cicada::TProcessedTrackData*)((*tracks)[iTrack]));
            mteData.AddTrack(track);
        }
        return;
    }

    void KT2ROOT::LoadProcMPTData(const KTProcessedMPTData& ptData, Cicada::TProcessedMPTData& rootPTData)
    {
        fComponent = data.GetComponent(); fTrackID = data.GetMainTrack().GetTrackID(); fEventSequenceID = data.GetMainTrack().GetEventSequenceID(); fIsCut = data.GetMainTrack().GetIsCut();
        fMVAClassifier = data.GetMainTrack().GetMVAClassifier(); fMainband = data.GetMainTrack().GetMainband(); fAxialFrequency = data.GetAxialFrequency();
        fAcquisitionID = data.GetMainTrack().GetAcquisitionID();
        fStartTimeInRunC = data.GetMainTrack().GetStartTimeInRunC(); fStartTimeInAcq = data.GetMainTrack().GetStartTimeInAcq(); fEndTimeInRunC = data.GetMainTrack().GetEndTimeInRunC(); fTimeLength = data.GetMainTrack().GetTimeLength();
        fStartFrequency = data.GetMainTrack().GetStartFrequency(); fEndFrequency = data.GetMainTrack().GetEndFrequency(); fFrequencyWidth = data.GetMainTrack().GetFrequencyWidth();
        fSlope = data.GetMainTrack().GetSlope(); fIntercept = data.GetMainTrack().GetIntercept(); fTotalPower = data.GetMainTrack().GetTotalPower();
        fStartTimeInRunCSigma = data.GetMainTrack().GetStartTimeInRunCSigma(); fEndTimeInRunCSigma = data.GetMainTrack().GetEndTimeInRunCSigma(); fTimeLengthSigma = data.GetMainTrack().GetTimeLengthSigma();
        fStartFrequencySigma = data.GetMainTrack().GetStartFrequencySigma(); fEndFrequencySigma = data.GetMainTrack().GetEndFrequencySigma(); fFrequencyWidthSigma = data.GetMainTrack().GetFrequencyWidthSigma();
        fSlopeSigma = data.GetMainTrack().GetSlopeSigma(); fInterceptSigma = data.GetMainTrack().GetInterceptSigma(); fTotalPowerSigma = data.GetMainTrack().GetTotalPowerSigma();
        return;
    }
    void KT2ROOT::UnloadProcMPTData(KTProcessedMPTData& ptData, const Cicada::TProcessedMPTData& rootPTData)
    {
        KTProcessedTrackData* data = new KTProcessedTrackData();

        data->SetComponent(fComponent); data->SetTrackID(fTrackID); data->SetEventSequenceID(fEventSequenceID); data->SetIsCut(fIsCut);
        data->SetMVAClassifier(fMVAClassifier); data->SetMainband(fMainband);
        data->SetAcquisitionID(fAcquisitionID);
        data->SetStartTimeInRunC(fStartTimeInRunC); data->SetStartTimeInAcq(fStartTimeInAcq); data->SetEndTimeInRunC(fEndTimeInRunC); data->SetTimeLength(fTimeLength);
        data->SetStartFrequency(fStartFrequency); data->SetEndFrequency(fEndFrequency); data->SetFrequencyWidth(fFrequencyWidth);
        data->SetSlope(fSlope); data->SetIntercept(fIntercept); data->SetTotalPower(fTotalPower);
        data->SetStartTimeInRunCSigma(fStartTimeInRunCSigma); data->SetEndTimeInRunCSigma(fEndTimeInRunCSigma); data->SetTimeLengthSigma(fTimeLengthSigma);
        data->SetStartFrequencySigma(fStartFrequencySigma); data->SetEndFrequencySigma(fEndFrequencySigma); data->SetFrequencyWidthSigma(fFrequencyWidthSigma);
        data->SetSlopeSigma(fSlopeSigma); data->SetInterceptSigma(fInterceptSigma); data->SetTotalPowerSigma(fTotalPowerSigma);

        mptData.SetComponent(fComponent);
        mptData.SetMainTrack(*data);
        mptData.SetAxialFrequency(fAxialFrequency);
        return;
    }

    void KT2ROOT::LoadClassifiedEventData(const KTClassifiedEventData& mteData, Cicada::TClassifiedEventData& rootMTEData)
    {
        fComponent = data.GetComponent(); fAcquisitionID = data.GetAcquisitionID(); fEventID = data.GetEventID(); fTotalEventSequences = data.GetTotalEventSequences();
        fStartTimeInRunC = data.GetStartTimeInRunC(); fStartTimeInAcq = data.GetStartTimeInAcq(); fEndTimeInRunC = data.GetEndTimeInRunC();fTimeLength = data.GetTimeLength();
        fStartFrequency = data.GetStartFrequency(); fEndFrequency = data.GetEndFrequency(); fMinimumFrequency = data.GetMinimumFrequency(); fMaximumFrequency = data.GetMaximumFrequency(); fFrequencyWidth = data.GetFrequencyWidth();
        fStartTimeInRunCSigma = data.GetStartTimeInRunCSigma(); fEndTimeInRunCSigma = data.GetEndTimeInRunCSigma(); fTimeLengthSigma = data.GetTimeLengthSigma();
        fStartFrequencySigma = data.GetStartFrequencySigma(); fEndFrequencySigma = data.GetEndFrequencySigma(); fFrequencyWidthSigma = data.GetFrequencyWidthSigma();
        fFirstTrackID = data.GetFirstTrackID(); fFirstTrackTimeLength = data.GetFirstTrackTimeLength(); fFirstTrackFrequencyWidth = data.GetFirstTrackFrequencyWidth(); fFirstTrackSlope = data.GetFirstTrackSlope(); fFirstTrackIntercept = data.GetFirstTrackIntercept(); fFirstTrackTotalPower = data.GetFirstTrackTotalPower();
        fUnknownEventTopology = data.GetUnknownEventTopology();
        Int_t nTracks = (Int_t)data.GetNTracks();
        fTracks->Clear(); fTracks->Expand(nTracks);
        fClassifierResults->Clear(); fClassifierResults->Expand(nTracks);
        Int_t iTrack = 0;
        for (TrackSetCIt trIt = data.GetTracksBegin(); trIt != data.GetTracksEnd(); ++trIt)
        {
            TProcessedTrackData* track = new((*fTracks)[iTrack]) TProcessedTrackData(trIt->fProcTrack);
            TClassifierResultsData* classifier = new((*fClassifierResults)[iTrack]) TClassifierResultsData(trIt->fData->Of< KTClassifierResultsData >());
            ++iTrack;
        }
        return;
    }
    void KT2ROOT::UnloadClassifiedEventData(KTClassifiedEventData& mteData, const Cicada::TClassifiedEventData& rootMTEData)
    {
        data.ClearTracks(); // do this first, since it clears some of the member variables other than just fTracks
        data.SetComponent(fComponent); data.SetAcquisitionID(fAcquisitionID); data.SetEventID(fEventID); data.SetTotalEventSequences(fTotalEventSequences);
        data.SetStartTimeInRunC(fStartTimeInRunC); data.SetStartTimeInAcq(fStartTimeInAcq); data.SetEndTimeInRunC(fEndTimeInRunC); data.SetTimeLength(fTimeLength);
        data.SetStartFrequency(fStartFrequency); data.SetEndFrequency(fEndFrequency); data.SetMinimumFrequency(fMinimumFrequency); data.SetMaximumFrequency(fMaximumFrequency); data.SetFrequencyWidth(fFrequencyWidth);
        data.SetStartTimeInRunCSigma(fStartTimeInRunCSigma); data.SetEndTimeInRunCSigma(fEndTimeInRunCSigma); data.SetTimeLengthSigma(fTimeLengthSigma);
        data.SetStartFrequencySigma(fStartFrequencySigma); data.SetEndFrequencySigma(fEndFrequencySigma); data.SetFrequencyWidthSigma(fFrequencyWidthSigma);
        data.SetFirstTrackID(fFirstTrackID); data.SetFirstTrackTimeLength(fFirstTrackTimeLength); data.SetFirstTrackFrequencyWidth(fFirstTrackFrequencyWidth); data.SetFirstTrackSlope(fFirstTrackSlope); data.SetFirstTrackIntercept(fFirstTrackIntercept); data.SetFirstTrackTotalPower(fFirstTrackTotalPower);
        data.SetUnknownEventTopology(fUnknownEventTopology);

        Int_t nTracks = fTracks->GetSize();
        Nymph::KTDataPtr dummyData;
        KTProcessedTrackData& procTrack = dummyData->Of< KTProcessedTrackData >();
        KTClassifierResultsData& classData = dummyData->Of< KTClassifierResultsData >();
        AllTrackData track( dummyData, procTrack );

        for (Int_t iTrack = 0; iTrack < nTracks; ++iTrack)
        {
            ((TProcessedTrackData*)((*fTracks)[iTrack]))->Unload(procTrack);
            ((TClassifierResultsData*)((*fClassifierResults)[iTrack]))->Unload(classData);
            data.AddTrack(track);
        }
        return;
    }

} /* namespace Katydid */
