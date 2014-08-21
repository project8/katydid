/*
 * KTNormalizedFSData.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTNormalizedFSData.hh"

namespace Katydid
{
    KTNormalizedFSDataPolar::KTNormalizedFSDataPolar()
    {}
    KTNormalizedFSDataPolar::~KTNormalizedFSDataPolar()
    {}

    KTNormalizedFSDataPolar& KTNormalizedFSDataPolar::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }


    KTNormalizedFSDataFFTW::KTNormalizedFSDataFFTW()
    {}
    KTNormalizedFSDataFFTW::~KTNormalizedFSDataFFTW()
    {}

    KTNormalizedFSDataFFTW& KTNormalizedFSDataFFTW::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        if (components < oldSize)
        {
            for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
            {
                delete fSpectra[iComponent];
            }
        }
        fSpectra.resize(components);
        if (components > oldSize)
        {
            for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
            {
                fSpectra[iComponent] = NULL;
            }
        }
        return *this;
    }

    KTNormalizedPSData::KTNormalizedPSData()
    {}
    KTNormalizedPSData::~KTNormalizedPSData()
    {}

    KTNormalizedPSData& KTNormalizedPSData::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        if (components < oldSize)
        {
            for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
            {
                delete fSpectra[iComponent];
            }
        }
        fSpectra.resize(components);
        if (components > oldSize)
        {
            for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
            {
                fSpectra[iComponent] = NULL;
            }
        }
        return *this;
    }


} /* namespace Katydid */
