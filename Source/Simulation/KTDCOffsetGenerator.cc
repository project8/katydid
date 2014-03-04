/*
 * KTDCOffsetGenerator.cc
 *
 *  Created on: May 29, 2013
 *      Author: nsoblath
 */

#include "KTDCOffsetGenerator.hh"

#include "KTPStoreNode.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "KTDCOffsetGenerator");

    KT_REGISTER_PROCESSOR(KTDCOffsetGenerator, "dc-offset-generator");
    static KTNORegistrar< KTProcessor, KTDCOffsetGenerator > sDCOffGenRegistrar("dc-offset-generator");

    KTDCOffsetGenerator::KTDCOffsetGenerator(const string& name) :
            KTTSGenerator(name),
            fOffsets()
    {
    }

    KTDCOffsetGenerator::~KTDCOffsetGenerator()
    {
    }

    bool KTDCOffsetGenerator::ConfigureDerivedGenerator(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        KTPStoreNode::csi_pair itPair = node->EqualRange("offset");
        for (KTPStoreNode::const_sorted_iterator citer = itPair.first; citer != itPair.second; ++citer)
        {
            UIntDoublePair pair = ParsePairUIntDouble(citer->second.get_value< string >());
            if (fOffsets.size() <= pair.first) fOffsets.resize(pair.first + 1);
            fOffsets[pair.first] = pair.second;
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
