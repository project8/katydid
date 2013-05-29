/*
 * KTDCOffsetGenerator.cc
 *
 *  Created on: May 29, 2013
 *      Author: nsoblath
 */

#include "KTDCOffsetGenerator.hh"

#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "katydid.simulation");

    static KTDerivedNORegistrar< KTProcessor, KTDCOffsetGenerator > sDCOffGenRegistrar("dc-offset-generator");

    KTDCOffsetGenerator::KTDCOffsetGenerator(const string& name) :
            KTTSGenerator(name),
            fOffsets()
    {
    }

    KTDCOffsetGenerator::~KTDCOffsetGenerator()
    {
    }

    Bool_t KTDCOffsetGenerator::ConfigureDerivedGenerator(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        KTPStoreNode::csi_pair itPair = node->EqualRange("offset");
        for (KTPStoreNode::const_sorted_iterator citer = itPair.first; citer != itPair.second; citer++)
        {
            UIntDoublePair pair = ParsePairUIntDouble(citer->second.get_value< string >());
            fOffsets[pair.first] = pair.second;
        }

        return true;
    }

    Bool_t KTDCOffsetGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        const UInt_t sliceSize = data.GetTimeSeries(0)->GetNTimeBins();

        UInt_t nComponents = data.GetNComponents();

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            KTTimeSeries* timeSeries = data.GetTimeSeries(iComponent);

            if (timeSeries == NULL)
            {
                KTERROR(genlog, "Time series " << iComponent << " was not present");
                continue;
            }

            for (UInt_t iBin = 0; iBin < sliceSize; iBin++)
            {
                timeSeries->SetValue(iBin, fOffsets[iComponent] + timeSeries->GetValue(iBin));
            }
        }

        return true;
    }


} /* namespace Katydid */
