/*
 * KTDCOffsetGenerator.cc
 *
 *  Created on: May 29, 2013
 *      Author: nsoblath
 */

#include "KTDCOffsetGenerator.hh"

#include "param.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "KTDCOffsetGenerator");

    KT_REGISTER_PROCESSOR(KTDCOffsetGenerator, "dc-offset-generator");

    KTDCOffsetGenerator::KTDCOffsetGenerator(const string& name) :
            KTTSGenerator(name),
            fOffsets()
    {
    }

    KTDCOffsetGenerator::~KTDCOffsetGenerator()
    {
    }

    bool KTDCOffsetGenerator::ConfigureDerivedGenerator(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        const KTParamArray* offsetPairs = node->ArrayAt("offsets");
        if (offsetPairs != NULL)
        {
            for (KTParamArray::const_iterator pairIt = offsetPairs->Begin(); pairIt != offsetPairs->End(); ++pairIt)
            {
                if (! ((*pairIt)->IsArray() && (*pairIt)->AsArray().Size() == 2))
                {
                    KTERROR(genlog, "Invalid pair: " << (*pairIt)->ToString());
                    return false;
                }
                UIntDoublePair pair((*pairIt)->AsArray().GetValue< unsigned >(0), (*pairIt)->AsArray().GetValue< double >(1));
                if (fOffsets.size() <= pair.first) fOffsets.resize(pair.first + 1);
                fOffsets[pair.first] = pair.second;
            }
        }

        return true;
    }

    bool KTDCOffsetGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        const unsigned sliceSize = data.GetTimeSeries(0)->GetNTimeBins();

        unsigned nComponents = data.GetNComponents();
        if (fOffsets.size() <= nComponents) fOffsets.resize(nComponents + 1);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeries* timeSeries = data.GetTimeSeries(iComponent);

            if (timeSeries == NULL)
            {
                KTERROR(genlog, "Time series " << iComponent << " was not present");
                continue;
            }

            for (unsigned iBin = 0; iBin < sliceSize; ++iBin)
            {
                timeSeries->SetValue(iBin, fOffsets[iComponent] + timeSeries->GetValue(iBin));
            }
        }

        return true;
    }


} /* namespace Katydid */
