/*
 * KTScoredSpectrumData.cc
 *
 *  Created on: Mar 10, 2016
 *      Author: Christine
 */

#include "KTScoredSpectrumData.hh"


namespace Katydid
{
    KTScoredSpectrumDataCore::KTScoredSpectrumDataCore() :
            KTFrequencyDomainArrayData()
    {
    }

    KTScoredSpectrumDataCore::~KTScoredSpectrumDataCore()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }


    const std::string KTScoredSpectrumData::sName("scored-spectrum");

    KTScoredSpectrumData::KTScoredSpectrumData() :
				KTScoredSpectrumDataCore(),
				KTExtensibleData()
    {
    }

    KTScoredSpectrumData::~KTScoredSpectrumData()
    {
    }

    KTScoredSpectrumData& KTScoredSpectrumData::SetNComponents(unsigned num)
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


