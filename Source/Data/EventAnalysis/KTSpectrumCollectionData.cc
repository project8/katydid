/*
 * KTSpectrumCollectionData.cc
 *
 *  Created on: Oct 9, 2015
 *      Author: ezayas
 */

#include "KTSpectrumCollectionData.hh"

#include "KTPowerSpectrum.hh"

namespace Katydid
{
    const std::string KTPSCollectionData::sName("ps-collection");

    KTPSCollectionData::KTPSCollectionData() :
            KTExtensibleData< KTPSCollectionData >(),
            fSpectra(),
            fStartTime(0.),
            fEndTime(0.001),
            fDeltaT(1e-6),
            fStartFreq(50e6),
            fEndFreq(150e6),
            fFilling(false)
    {
    }

    KTPSCollectionData::KTPSCollectionData(const KTPSCollectionData& orig) :
            KTExtensibleData< KTPSCollectionData >(orig),
            fSpectra(),
            fStartTime(orig.fStartTime),
            fEndTime(orig.fEndTime),
            fDeltaT(orig.fDeltaT),
            fStartFreq(orig.fStartFreq),
            fEndFreq(orig.fEndFreq),
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
        fStartFreq = rhs.fStartFreq;
        fEndFreq = rhs.fEndFreq;
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
        // note that nBins will automatically take the floor of the expression because it is an integer
        int nBins = (GetEndFreq() - GetStartFreq()) / spectrum->GetFrequencyBinWidth();
        if( nBins <= 0 )
        {
            return;
        }

        // calculate frequency bounds for new spectrum
        double midFreq = 0.5 * (GetStartFreq() + GetEndFreq());
        double minFreq = midFreq - (0.5 * nBins * spectrum->GetFrequencyBinWidth());
        double maxFreq = midFreq + (0.5 * nBins * spectrum->GetFrequencyBinWidth());

        // initialize new spectrum
        KTPowerSpectrum* newSpectrum = new KTPowerSpectrum( nBins, minFreq, maxFreq );
        for( int i = 0; i < nBins; i++ )
        {
            (*newSpectrum)(i) = 0.;
        }

        // fill new spectrum
        int minBin = spectrum->FindBin( minFreq );
        for( int i = minBin; i < minBin + nBins; i++ )
        {
            (*newSpectrum)(i - minBin) = (*spectrum)(i);
        }

        // add new spectrum to fSpectra
        fSpectra.erase(t);
        fSpectra[t] = new KTPowerSpectrum(*newSpectrum);
        return;
    }

} /* namespace Katydid */
