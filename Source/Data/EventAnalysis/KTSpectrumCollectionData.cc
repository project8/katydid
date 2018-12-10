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
            KTMultiPSDataCore(),
            KTExtensibleData< KTPSCollectionData >(),
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

    void KTPSCollectionData::AddSpectrum(double timeStamp, KTPowerSpectrum* spectrum, unsigned iComponent)
    {
        std::cout << "Adding spectrum" << std::endl;

        if( fSpectra.size() <= iComponent )
        {
            SetNComponents( iComponent + 1 );
            std::cout << "Set N components: " << fSpectra.size() << std::endl;
        }

        // If fSpectra is empty then this is the first spectrum received
        // We must compute the min and max bin, and the number of bins
        if( fSpectra[iComponent] == NULL )
        {
            std::cout << "Proceeding with spectrum" << std::endl;
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

            unsigned iSpectra = (int)((fEndTime - fStartTime) / (double)fDeltaT) + 1;
            std::cout << "iSpectra = " << iSpectra << std::endl;

            fSpectra[iComponent] = new KTMultiPS(iSpectra, fStartTime, fEndTime);
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

        std::cout << "Setting spectrum" << std::endl;

        // add new spectrum to fSpectra
        std::cout << "Timestamp = " << timeStamp << std::endl;
        std::cout << "fStartTime = " << fStartTime << std::endl;
        std::cout << "DeltaT = " << fDeltaT << std::endl;
        unsigned iSpectrum = (int)((timeStamp - fStartTime) / (double)fDeltaT) + 1;
        std::cout << "iSpectrum = " << iSpectrum << std::endl;

        SetSpectrum( newSpectrum, iSpectrum, iComponent );

        std::cout << "Finished setting spectrum" << std::endl;

        return;
    }

} /* namespace Katydid */
