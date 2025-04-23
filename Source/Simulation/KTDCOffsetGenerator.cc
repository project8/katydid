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

    bool KTDCOffsetGenerator::ConfigureDerivedGenerator(const scarab::param_node* node) //changed scarab::param_node to scarab::param
    {
        if (node == NULL) return false;

        const scarab::param_array* offsetPairs = node-> array_at("offsets"); //changing this based on DevGuide &KTCorrelator
        if (offsetPairs != NULL)
        {
            for (scarab::param_array::const_iterator pairIt = offsetPairs->begin(); pairIt != offsetPairs->end(); ++pairIt)
            {
                if (! ((*pairIt)->is_array() && (*pairIt)->as_array().size() == 2))
                {
                    KTERROR(genlog, "Invalid pair: " << (*pairIt)->to_string());
                    return false;
                }
                UIntDoublePair pair((*pairIt)->as_array().get_value< unsigned >(0), (*pairIt)->as_array().get_value< double >(1));
                if (fOffsets.size() <= pair.first) fOffsets.resize(pair.first + 1); //keeping fOffsets for now bc it is defined in the .hh
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
                timeSeries->SetValue(iBin, fOffsets[iComponent] + timeSeries->GetValue(iBin)); //is this GetValue diff
            }
        }

        return true;
    }


} /* namespace Katydid */
