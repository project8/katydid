/*
 * KTROOTTreeTypeWriterCandidates.cc
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterCandidates.hh"

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

    static KTDerivedTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterCandidates > sRTTWCRegistrar;

    KTROOTTreeTypeWriterCandidates::KTROOTTreeTypeWriterCandidates() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterCandidates()
            fFreqCandidateTree(NULL)
    {
    }

    KTROOTTreeTypeWriterCandidates::~KTROOTTreeTypeWriterCandidates()
    {
    }


    void KTROOTTreeTypeWriterCandidates::RegisterSlots()
    {
        fWriter->RegisterSlot("frequency-candidates", this, &KTROOTTreeTypeWriterCandidates::WriteFrequencyCandidates, "void (const KTFrequencyCandidateData*)");
        return;
    }


    //*********************
    // Frequency Candidates
    //*********************

    void KTROOTTreeTypeWriterCandidates::WriteFrequencyCandidates(const KTFrequencyCandidateData* data)
    {
        if (! fWriter->OpenAndVerifyFile()) return;
        if (fFreqCandidateTree == NULL) SetupFrequencyCandidateTree();
/*
        stringstream conv;

        KTJSONWriter::JSONMaker* jsonMaker = fWriter->GetJSONMaker();

        jsonMaker->String("candidates");
        jsonMaker->StartObject();

        for (UInt_t iGroup=0; iGroup < data->GetNGroups(); iGroup++)
        {
            const KTFrequencyCandidateData::Candidates& candidates = data->GetCandidates(iGroup);
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
*/
        return;
    }

    void KTROOTTreeTypeWriterCandidates::SetupFrequencyCandidateTree()
    {

        return;
    }

} /* namespace Katydid */



