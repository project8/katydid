/*
 * KTNormalizedFSData.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTNormalizedFSData.hh"

namespace Katydid
{
    const std::string KTNormalizedFSDataPolar::sName("normalized-fs-polar");

    KTNormalizedFSDataPolar::KTNormalizedFSDataPolar() :
            fComponentData()
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
        fComponentData.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }


    const std::string KTNormalizedFSDataFFTW::sName("normalized-fs-fftw");

    KTNormalizedFSDataFFTW::KTNormalizedFSDataFFTW() :
            fComponentData()
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
        fComponentData.resize(components);
        if (components > oldSize)
        {
            for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
            {
                fSpectra[iComponent] = NULL;
            }
        }
        return *this;
    }


    const std::string KTNormalizedPSData::sName("normalized-ps");

    KTNormalizedPSData::KTNormalizedPSData() :
            fComponentData()
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
        fComponentData.resize(components);
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
