/*
 * KTJSONTypeWriterEventAnalysis.cc
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#include "KTFrequencyCandidate.hh"
#include "KTFrequencyCandidateData.hh"
#include "KTTIFactory.hh"
//#include "logger.hh"

#include <sstream>
#include "KTJSONTypeWriterEventAnalysis.hh"



using std::stringstream;
using std::string;

namespace Katydid
{
    //LOGGER(publog, "KTJSONTypeWriterEventAnalysis");

    static Nymph::KTTIRegistrar< KTJSONTypeWriter, KTJSONTypeWriterEventAnalysis > sJSONTWCRegistrar;

    KTJSONTypeWriterEventAnalysis::KTJSONTypeWriterEventAnalysis() :
            KTJSONTypeWriter()
            //KTTypeWriterEventAnalysis()
    {
    }

    KTJSONTypeWriterEventAnalysis::~KTJSONTypeWriterEventAnalysis()
    {
    }


    void KTJSONTypeWriterEventAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("frequency-candidates", this, &KTJSONTypeWriterEventAnalysis::WriteFrequencyCandidates);
        return;
    }


    //*********************
    // Frequency Candidates
    //*********************

    void KTJSONTypeWriterEventAnalysis::WriteFrequencyCandidates(Nymph::KTDataPtr data)
    {
        using rapidjson::SizeType;

        KTFrequencyCandidateData& fcData = data->Of< KTFrequencyCandidateData >();

        unsigned nComponents = fcData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        stringstream conv;

        KTJSONWriter::JSONMaker* jsonMaker = fWriter->GetJSONMaker();

        jsonMaker->String("candidates");
        jsonMaker->StartObject();

        for (unsigned iGroup=0; iGroup < nComponents; iGroup++)
        {
            const KTFrequencyCandidateData::Candidates& candidates = fcData.GetCandidates(iGroup);
            conv << iGroup;
            jsonMaker->String(conv.str().c_str());
            jsonMaker->StartArray();

            for (KTFrequencyCandidateData::Candidates::const_iterator candIt = candidates.begin(); candIt != candidates.end(); candIt++)
            {
                jsonMaker->StartObject();
                jsonMaker->String("first-bin");
                jsonMaker->Uint(candIt->GetFirstBin());

                jsonMaker->String("last-bin");
                jsonMaker->Uint(candIt->GetLastBin());

                jsonMaker->String("mean-frequency");
                jsonMaker->Double(candIt->GetMeanFrequency());
                jsonMaker->EndObject();
            }

            jsonMaker->EndArray();
        }

        jsonMaker->EndObject();

        return;
    }

} /* namespace Katydid */
