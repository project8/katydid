/*
 * KTJSONTypeWriterCandidates.cc
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#include "KTJSONTypeWriterCandidates.hh"

#include "KTFrequencyCandidate.hh"
#include "KTFrequencyCandidateData.hh"
#include "KTTIFactory.hh"
//#include "KTLogger.hh"

#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    //KTLOGGER(publog, "katydid.output");

    static KTDerivedTIRegistrar< KTJSONTypeWriter, KTJSONTypeWriterCandidates > sJSONTWCRegistrar;

    KTJSONTypeWriterCandidates::KTJSONTypeWriterCandidates() :
            KTJSONTypeWriter()
            //KTTypeWriterCandidates()
    {
    }

    KTJSONTypeWriterCandidates::~KTJSONTypeWriterCandidates()
    {
    }


    void KTJSONTypeWriterCandidates::RegisterSlots()
    {
        fWriter->RegisterSlot("frequency-candidates", this, &KTJSONTypeWriterCandidates::WriteFrequencyCandidates);
        return;
    }


    //*********************
    // Frequency Candidates
    //*********************

    void KTJSONTypeWriterCandidates::WriteFrequencyCandidates(KTDataPtr data)
    {
        using rapidjson::SizeType;

        KTFrequencyCandidateData& fcData = data->Of< KTFrequencyCandidateData >();

        UInt_t nComponents = fcData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        stringstream conv;

        KTJSONWriter::JSONMaker* jsonMaker = fWriter->GetJSONMaker();

        jsonMaker->String("candidates");
        jsonMaker->StartObject();

        for (UInt_t iGroup=0; iGroup < nComponents; iGroup++)
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
