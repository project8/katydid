/*
 * KTHoughData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTHoughData.hh"

#include "logger.hh"

using std::vector;

namespace Katydid
{
    LOGGER(htlog, "KTHoughData");

    const std::string KTHoughData::sName("hough");

    KTHoughData::KTHoughData() :
            KTExtensibleData< KTHoughData >(),
            fTransforms()
    {
    }

    KTHoughData::~KTHoughData()
    {
        while (! fTransforms.empty())
        {
            delete fTransforms.back().fTransform;
            fTransforms.pop_back();
        }
    }

    KTHoughData& KTHoughData::SetNComponents(unsigned components)
    {
        unsigned oldSize = fTransforms.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            delete fTransforms[iComponent].fTransform;
        }
        fTransforms.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fTransforms[iComponent].fTransform = NULL;
        }
        return *this;
    }


} /* namespace Katydid */

