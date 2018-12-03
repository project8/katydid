/*
 * KTSpectrumCollectionData.cc
 *
 *  Created on: Oct 9, 2015
 *      Author: ezayas
 */

#include "KTSpectrumCollectionData.hh"

#include "KTPowerSpectrum.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTSliceHeader.hh"

namespace Katydid
{
    const std::string KTPSCollectionData::sName("ps-collection");

    KTPSCollectionData::KTPSCollectionData() :
            KTMultiPSData(),
            fSpectra(),
            fStartTime(0.),
            fEndTime(0.001),
            fDeltaT(1e-6),
            fMinFreq(50e6),
            fMaxFreq(150e6),
            fMinBin(0),
            fMaxBin(1),
            fFilling(false)
    {
    }

    KTPSCollectionData::KTPSCollectionData(const KTPSCollectionData& orig) :
            KTMultiPSData(orig),
            fSpectra(),
            fStartTime(orig.fStartTime),
            fEndTime(orig.fEndTime),
            fDeltaT(orig.fDeltaT),
            fMinFreq(orig.fMinFreq),
            fMaxFreq(orig.fMaxFreq),
            fMinBin(orig.fMinBin),
            fMaxBin(orig.fMaxBin),
            fFilling(orig.fFilling)
    {
        for (collection::const_iterator it = orig.fSpectra.begin(); it != orig.fSpectra.end(); ++it)
        {
            fSpectra[it->first] = new KTPowerSpectrum(*it->second);
        }
    }

    KTPSCollectionData::~KTPSCollectionData()
    {
        for (collection::iterator it = fSpectra.begin(); it != fSpectra.end(); ++it)
        {
            delete it->second;
        }
    }

    KTPSCollectionData& KTPSCollectionData::operator=(const KTPSCollectionData& rhs)
    {
        fStartTime = rhs.fStartTime;
        fEndTime = rhs.fEndTime;
        fDeltaT = rhs.fDeltaT;
        fMinFreq = rhs.fMinFreq;
        fMaxFreq = rhs.fMaxFreq;
        fMinBin = rhs.fMinBin;
        fMaxBin = rhs.fMaxBin;
        fFilling = rhs.fFilling;
        
        for (collection::iterator it = fSpectra.begin(); it != fSpectra.end(); ++it)
        {
            delete it->second;
        }

        fSpectra.clear();

        for (collection::const_iterator it = rhs.fSpectra.begin(); it != rhs.fSpectra.end(); ++it)
        {
            fSpectra[it->first] = new KTPowerSpectrum(*it->second);
        }
        return *this;
    }

    void KTPSCollectionData::AddSpectrum(double t, KTPowerSpectrum* spectrum)
    {
        // If fSpectra is empty then this is the first spectrum received
        // We must compute the min and max bin, and the number of bins
        if( fSpectra.empty() )
        {
            SetMinBin( spectrum->FindBin( GetMinFreq() ) );
            SetMaxBin( spectrum->FindBin( GetMaxFreq() ) );

            if( GetMinBin() > GetMaxBin() )
            {
                return;
            }

            // midFreq is the midpoint of start and end frequencies
            // minFreq is below this by exactly half the number of bins times the frequency step
            // maxFreq is above this by exactly half the number of bins times the frequency step
            double midFreq = 0.5 * (GetMinFreq() + GetMaxFreq());
            double minFreq = midFreq - (0.5 * (GetMaxBin() - GetMinBin() + 1) * spectrum->GetFrequencyBinWidth());
            double maxFreq = midFreq + (0.5 * (GetMaxBin() - GetMinBin() + 1) * spectrum->GetFrequencyBinWidth());

            // This way the center frequency is preserved but the precise bounds are adjusted to match the bin width
            SetMinFreq( minFreq );
            SetMaxFreq( maxFreq );
        }

        // When fSpectra is not empty, no 'Set' commands are used, only 'Get' for frequency and bin info
        // This ensures all spectra have the same frequency bounds and number of bins

        // nBins is the number of bins in the new spectrum
        int nBins = GetMaxBin() - GetMinBin() + 1;

        // initialize new spectrum
        KTPowerSpectrum* newSpectrum = new KTPowerSpectrum( nBins, GetMinFreq(), GetMaxFreq() );
        for( int i = 0; i < nBins; i++ )
        {
            (*newSpectrum)(i) = 0.;
        }

        // fill new spectrum
        for( int i = GetMinBin(); i <= GetMaxBin(); ++i )
        {
            (*newSpectrum)(i - GetMinBin()) = (*spectrum)(i);
        }

        // add new spectrum to fSpectra
        fSpectra.erase(t);
        fSpectra[t] = new KTPowerSpectrum(*newSpectrum);

        return;
    }
/*
    const std::string KTTSCollectionData::sName("ts-collection");

    KTTSCollectionData::KTTSCollectionData() :
            KTExtensibleData< KTTSCollectionData >(),
            fSeries(),
            fStartTime(0.),
            fEndTime(0.001),
            fDeltaT(1e-6),
            fFilling(false)
    {
    }

    KTTSCollectionData::KTTSCollectionData(const KTTSCollectionData& orig) :
            KTExtensibleData< KTTSCollectionData >(orig),
            fSeries(),
            fStartTime(orig.fStartTime),
            fEndTime(orig.fEndTime),
            fDeltaT(orig.fDeltaT),
            fFilling(orig.fFilling)
    {
        for (collection::const_iterator it = orig.fSeries.begin(); it != orig.fSeries.end(); ++it)
        {
            fSeries.push_back( std::make_pair( new KTSliceHeader(*it->first), new KTTimeSeriesFFTW(*it->second) ) );
        }
    }

    KTTSCollectionData::~KTTSCollectionData()
    {
        for (collection::iterator it = fSeries.begin(); it != fSeries.end(); ++it)
        {
            delete it->second;
        }
    }

    KTTSCollectionData& KTTSCollectionData::operator=(const KTTSCollectionData& rhs)
    {
        fStartTime = rhs.fStartTime;
        fEndTime = rhs.fEndTime;
        fDeltaT = rhs.fDeltaT;
        fFilling = rhs.fFilling;
        
        for (collection::iterator it = fSeries.begin(); it != fSeries.end(); ++it)
        {
            delete it->second;
        }

        fSeries.clear();

        for (collection::const_iterator it = rhs.fSeries.begin(); it != rhs.fSeries.end(); ++it)
        {
            fSeries.push_back( std::make_pair( new KTSliceHeader(*it->first), new KTTimeSeriesFFTW(*it->second) ) );
        }
        return *this;
    }

    void KTTSCollectionData::AddTimeSeries(KTSliceHeader* slice, KTTimeSeriesFFTW* ts)
    {
        // nBins is the number of bins in the new time series
        int nBins = ts->GetNTimeBins();

        // initialize new time series
        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW( nBins, slice->GetTimeInAcq(), slice->GetTimeInAcq() + slice->GetSliceLength() );
        for( int i = 0; i < nBins; i++ )
        {
            (*newTS)(i)[0] = 0.;
            (*newTS)(i)[1] = 0.;
        }

        // fill new time series
        for( int i = 0; i <= nBins; ++i )
        {
            (*newTS)(i)[0] = (*ts)(i)[0];
            (*newTS)(i)[1] = (*ts)(i)[1];
        }

        // add new time series to fSeries
        fSeries.push_back( std::make_pair( slice, newTS ) );

        return;
    }*/

} /* namespace Katydid */
