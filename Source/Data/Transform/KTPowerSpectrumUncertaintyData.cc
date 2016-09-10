/*
 * KTPowerSpectrumUncertaintyData.cc
 *
 *  Created on: Oct 2, 2015
 *      Author: ezayas
 */

#include "KTPowerSpectrumUncertaintyData.hh"


namespace Katydid
{

    const std::string KTPowerSpectrumUncertaintyData::sName( "power-spectrum-uncertainty" );

    KTPowerSpectrumUncertaintyData::KTPowerSpectrumUncertaintyData() :
            KTPowerSpectrumDataCore(),
            KTExtensibleData()
    {
    }

    KTPowerSpectrumUncertaintyData::~KTPowerSpectrumUncertaintyData()
    {
    }

    KTPowerSpectrumUncertaintyData& KTPowerSpectrumUncertaintyData::SetNComponents( unsigned num )
    {
        unsigned oldSize = fSpectra.size();
        // if num < oldSize
        for( unsigned iComponent = num; iComponent < oldSize; iComponent++ )
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize( num );
        // if num > oldSize
        for( unsigned iComponent = oldSize; iComponent < num; iComponent++ )
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }

} /* namespace Katydid */