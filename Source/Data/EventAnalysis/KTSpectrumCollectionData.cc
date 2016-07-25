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
            fFilling(false)
    {
    }

    KTPSCollectionData::KTPSCollectionData(const KTPSCollectionData& orig) :
            KTExtensibleData< KTPSCollectionData >(orig),
            fSpectra(),
            fStartTime(orig.fStartTime),
            fEndTime(orig.fEndTime),
            fDeltaT(orig.fDeltaT),
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
        fSpectra.erase(t);
        fSpectra[t] = new KTPowerSpectrum(*spectrum);
        return;
    }

} /* namespace Katydid */
