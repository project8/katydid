/*
 * KTPowerSpectrumData.cc
 *
 *  Created on: Aug 1, 2014
 *      Author: nsoblath
 */

#include "KTPowerSpectrumData.hh"


namespace Katydid
{
    KTPowerSpectrumDataCore::KTPowerSpectrumDataCore() :
            KTFrequencyDomainArrayData()
    {
    }

    KTPowerSpectrumDataCore::~KTPowerSpectrumDataCore()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }


    KTPowerSpectrumData::KTPowerSpectrumData() :
            KTPowerSpectrumDataCore(),
            Data()
    {
    }

    KTPowerSpectrumData::~KTPowerSpectrumData()
    {
    }

    KTPowerSpectrumData& KTPowerSpectrumData::SetNComponents(unsigned num)
    {
        unsigned oldSize = fSpectra.size();
        // if num < oldSize
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(num);
        // if num > oldSize
        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }


    KTPowerSpectrumVarianceData::KTPowerSpectrumVarianceData() :
            KTFrequencySpectrumVarianceDataCore(),
            Data()
    {
    }

    KTPowerSpectrumVarianceData::~KTPowerSpectrumVarianceData()
    {
    }

    KTPowerSpectrumVarianceData& KTPowerSpectrumVarianceData::SetNComponents(unsigned num)
    {
        unsigned oldSize = fSpectra.size();
        // if num < oldSize
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(num);
        // if num > oldSize
        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }

} /* namespace Katydid */

