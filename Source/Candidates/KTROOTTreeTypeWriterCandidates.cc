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

    static KTDerivedTIRegistrar< KTJSONTypeWriter, KTROOTTreeTypeWriterCandidates > sRTTWCRegistrar;

    KTROOTTreeTypeWriterCandidates::KTROOTTreeTypeWriterCandidates() :
            KTJSONTypeWriter(),
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

Double_t TFrequencyCandidateData::GetBinWidth() const
{
    return fBinWidth;
}

void TFrequencyCandidateData::SetBinWidth(Double_t binWidth)
{
    fBinWidth = binWidth;
}

const TClonesArray*& TFrequencyCandidateData::GetComponentData() const
{
    return fComponentData;
}

void TFrequencyCandidateData::SetComponentData(const TClonesArray*& componentData)
{
    fComponentData = componentData;
}

UInt_t TFrequencyCandidateData::GetNBins() const
{
    return fNBins;
}

void TFrequencyCandidateData::SetNBins(UInt_t nBins)
{
    fNBins = nBins;
}

Double_t TFrequencyCandidateData::GetTimeInRun() const
{
    return fTimeInRun;
}

void TFrequencyCandidateData::SetTimeInRun(Double_t timeInRun)
{
    fTimeInRun = timeInRun;
}
