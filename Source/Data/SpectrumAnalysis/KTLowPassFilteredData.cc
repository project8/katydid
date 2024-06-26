/*
 * KTLowPassFilteredData.cc
 *
 *  Created on: Nov 3, 2014
 *      Author: nsoblath
 */

#include "KTLowPassFilteredData.hh"

namespace Katydid
{
    const std::string KTLowPassFilteredFSDataPolar::sName("lpfed-fs-polar");

    KTLowPassFilteredFSDataPolar::KTLowPassFilteredFSDataPolar()
    {}
    KTLowPassFilteredFSDataPolar::~KTLowPassFilteredFSDataPolar()
    {}

    KTLowPassFilteredFSDataPolar& KTLowPassFilteredFSDataPolar::SetNComponents(unsigned components)
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


    const std::string KTLowPassFilteredFSDataFFTW::sName("lpfed-fs-fftw");

    KTLowPassFilteredFSDataFFTW::KTLowPassFilteredFSDataFFTW()
    {}
    KTLowPassFilteredFSDataFFTW::~KTLowPassFilteredFSDataFFTW()
    {}

    KTLowPassFilteredFSDataFFTW& KTLowPassFilteredFSDataFFTW::SetNComponents(unsigned components)
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


    const std::string KTLowPassFilteredPSData::sName("lpfed-ps");

    KTLowPassFilteredPSData::KTLowPassFilteredPSData()
    {}
    KTLowPassFilteredPSData::~KTLowPassFilteredPSData()
    {}

    KTLowPassFilteredPSData& KTLowPassFilteredPSData::SetNComponents(unsigned components)
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
