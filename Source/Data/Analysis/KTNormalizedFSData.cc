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

    KTNormalizedFSDataPolar& KTNormalizedFSDataPolar::SetNComponents(UInt_t components)
    {
        UInt_t oldSize = fSpectra.size();
        fSpectra.resize(components);
        if (components > oldSize)
        {
            for (UInt_t iComponent = oldSize; iComponent < components; iComponent++)
            {
                fSpectra[iComponent] = NULL;
            }
        }
        return *this;
    }


    KTNormalizedFSDataFFTW::KTNormalizedFSDataFFTW()
    {}
    KTNormalizedFSDataFFTW::~KTNormalizedFSDataFFTW()
    {}

    KTNormalizedFSDataFFTW& KTNormalizedFSDataFFTW::SetNComponents(UInt_t components)
    {
        UInt_t oldSize = fSpectra.size();
        fSpectra.resize(components);
        if (components > oldSize)
        {
            for (UInt_t iComponent = oldSize; iComponent < components; iComponent++)
            {
                fSpectra[iComponent] = NULL;
            }
        }
        return *this;
    }


} /* namespace Katydid */
