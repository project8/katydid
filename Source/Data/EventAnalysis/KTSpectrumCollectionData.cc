/*
 * KTSpectrumCollectionData.cc
 *
 *  Created on: Oct 9, 2015
 *      Author: ezayas
 */

#include "KTSpectrumCollectionData.hh"

#include "KTMath.hh"
#include "KTPowerSpectrum.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTSliceHeader.hh"

#include "logger.hh"

LOGGER(scdlog, "SpectrogramCollectionData");

namespace Katydid
{
    const std::string KTPSCollectionData::sName("ps-collection");

    KTPSCollectionData::KTPSCollectionData() :
            KTMultiPSDataCore(),
            KTExtensibleData< KTPSCollectionData >(),
            fStartTime(0.),
            fEndTime(0.),
            fDeltaT(-1.),
            fMinFreq(0.),
            fMaxFreq(0.),
            fMinBin(0),
            fMaxBin(0),
            fFilling(false),
            fSpectrogramCounter(0)
    {
    }

    KTPSCollectionData::~KTPSCollectionData()
    {
    }

    KTPSCollectionData& KTPSCollectionData::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            DeleteSpectra(iComponent);
        }
        fSpectra.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }

    void KTPSCollectionData::AddSpectrum(double timeStamp, const KTPowerSpectrum& spectrum, unsigned iComponent)
    {
        // timeStamp is the spectrum's time-in-run-c

        if( fSpectra.size() <= iComponent )
        {
            SetNComponents( iComponent + 1 );
        }

        // If fSpectra is empty then this is the first spectrum received
        // We must compute the min and max bin, and the number of bins
        if( fSpectra[iComponent] == NULL )
        {
            SetMinBin( spectrum.FindBin( fMinFreq ) );
            SetMaxBin( spectrum.FindBin( fMaxFreq ) );

            if( fMinBin >= fMaxBin )
            {
                LERROR( scdlog, "Min bin is greater than max bin; Min freq <" << fMinFreq << " is probably greater than max freq <" << fMaxFreq << ">" );
                return;
            }

            if( fDeltaT <= 0. )
            {
                LERROR( scdlog, "DeltaT has not been set or is invalid: " << fDeltaT );
                return;
            }

            // midFreq is the midpoint of start and end frequencies
            // minFreq is below this by exactly half the number of bins times the frequency step
            // maxFreq is above this by exactly half the number of bins times the frequency step
            double midFreq = 0.5 * (fMinFreq + fMaxFreq);
            double minFreq = midFreq - (0.5 * (fMaxBin - fMinBin + 1) * spectrum.GetFrequencyBinWidth());
            double maxFreq = midFreq + (0.5 * (fMaxBin - fMinBin + 1) * spectrum.GetFrequencyBinWidth());

            // This way the center frequency is preserved but the precise bounds are adjusted to match the bin width
            SetMinFreq( minFreq );
            SetMaxFreq( maxFreq );

            unsigned nSpectra = KTMath::Nint((fEndTime - fStartTime) / fDeltaT) + 1;
            LDEBUG(scdlog, "Number of spectra in this new multi-ps: " << nSpectra);
            // fStartTime and fEndTime are times-in-run-c.  the spectrum time boundaries need to be the low and high edges of the bins.
            // So we shift down and up by 0.5*slice length relative to fStartTime and fEndTime for the min and max times, respectively.
            fSpectra[iComponent] = new KTMultiPS(NULL, nSpectra, fStartTime - 0.5 * fDeltaT, fEndTime + 0.5 * fDeltaT);
        }

        // When fSpectra is not empty, no 'Set' commands are used, only 'Get' for frequency and bin info
        // This ensures all spectra have the same frequency bounds and number of bins

        // nBins is the number of bins in the new spectrum
        int nBins = fMaxBin - fMinBin + 1;

        // initialize new spectrum
        KTPowerSpectrum* newSpectrum = new KTPowerSpectrum( nBins, fMinFreq, fMaxFreq );
        for( unsigned i = 0; i < nBins; ++i )
        {
            (*newSpectrum)(i) = 0.;
        }

        // fill new spectrum
        for( int i = fMinBin; i <= fMaxBin; ++i )
        {
            (*newSpectrum)(i - fMinBin) = spectrum(i);
        }

        // add new spectrum to fSpectra
        unsigned iSpectrum = KTMath::Nint((timeStamp - fStartTime) / fDeltaT);
        LDEBUG(scdlog, "Adding spectrum " << iSpectrum);
        SetSpectrum( newSpectrum, iSpectrum, iComponent );

        return;
    }

} /* namespace Katydid */
