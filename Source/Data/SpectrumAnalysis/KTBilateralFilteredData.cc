/*
 * KTBilateralFilteredData.cc
 *
 *  Created on: Mar 7, 2018
 *      Author: buzinsky
 */

#include "KTBilateralFilteredData.hh"

namespace Katydid
{
    const std::string KTBilateralFilteredFSDataFFTW::sName("blfed-fs-fftw");

    KTBilateralFilteredFSDataFFTW::KTBilateralFilteredFSDataFFTW()
    {}
    KTBilateralFilteredFSDataFFTW::~KTBilateralFilteredFSDataFFTW()
    {}

    KTBilateralFilteredFSDataFFTW& KTBilateralFilteredFSDataFFTW::SetNComponents(unsigned components)
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
