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

    bool KTDCOffsetGenerator::ConfigureDerivedGenerator(const scarab::param* node) //changed scarab::param_node to scarab::param
    {
        if (node == NULL) return false;

        const param_array offset_pairs = node->array_at("offsets");
        if (offset_pairs != NULL)
        {
            for (param_array::const_iterator pair_it = offset_pairs->begin(); pair_it != offset_pairs->end(); ++pair_it)
            {
                if (! ((*pair_it)->is_array() && (*pair_it)->as_array().Size() == 2))
                {
                    KTERROR(genlog, "Invalid pair: " << (*pair_it)->ToString());
                    return false;
                }
                UIntDoublePair pair((*pair_it)->as_array().get_value<unsigned>(0), (*pair_it)->as_array().get_value<double>(1));
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
