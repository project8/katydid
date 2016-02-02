/*
 * KTPSCollectionData.cc
 *
 *  Created on: July 22, 2014
 *      Author: nsoblath
 */

#include "KTSpectrumCollectionData.hh"

#include "KTPowerSpectrum.hh"

namespace Katydid
{
    const std::string KTPSCollectionData::sName("ps-collection");

    KTPSCollectionData::KTPSCollectionData() :
            KTExtensibleData< KTPSCollectionData >(),
            fSpectra()
    {
    }

    KTPSCollectionData::KTPSCollectionData(const KTPSCollectionData& orig) :
            KTExtensibleData< KTPSCollectionData >(orig),
            fSpectra()
    {
        for (collection::const_iterator it = orig.fSpectra.begin(); it != orig.fSpectra.end(); ++it)
        {
            fSpectra.push_back(new KTPowerSpectrum(**it));
        }
    }

    KTPSCollectionData::~KTPSCollectionData()
    {
        for (collection::iterator it = fSpectra.begin(); it != fSpectra.end(); ++it)
        {
            delete *it;
        }
    }

    KTPSCollectionData& KTPSCollectionData::operator=(const KTPSCollectionData& rhs)
    {
        for (collection::const_iterator it = rhs.fSpectra.begin(); it != rhs.fSpectra.end(); ++it)
        {
            fSpectra.push_back(new KTPowerSpectrum(**it));
        }
        return *this;
    }

    void KTPSCollectionData::AddSpectrum(KTPowerSpectrum* spectrum)
    {
        fSpectra.push_back(new KTPowerSpectrum(*spectrum));
        return;
    }

} /* namespace Katydid */
