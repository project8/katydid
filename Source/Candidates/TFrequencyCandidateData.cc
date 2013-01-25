/*
 * TFrequencyCandidateData.cc
 *
 *  Created on: Jan 25, 2013
 *      Author: nsoblath
 */

#include "TFrequencyCandidateData.hh"

#include "KTFrequencyCandidate.hh"
#include "KTFrequencyCandidateData.hh"



ClassImp(Katydid::TFrequencyCandidateData);
ClassImp(Katydid::TFrequencyCandidateDataComponent);
ClassImp(Katydid::TFrequencyCandidateDataCandidate);

namespace Katydid
{
    //***************************
    // TFrequencyCandidateData
    //***************************

    TFrequencyCandidateData::TFrequencyCandidateData() :
            TObject(),
            fComponentData(new TClonesArray()),
            fNBins(0),
            fBinWidth(0.),
            fTimeInRun(0.)
    {
    }
    TFrequencyCandidateData::TFrequencyCandidateData(const KTFrequencyCandidateData& data) :
            TObject(),
            fComponentData(new TClonesArray("TFrequencyCandidateDataComponent", data.GetNGroups())),
            fNBins(data.GetNBins()),
            fBinWidth(data.GetBinWidth()),
            fTimeInRun(data.GetTimeInRun())
    {
        for (UInt_t iComponent=0; iComponent < data.GetNGroups(); iComponent++)
        {
            new(&fComponentData[iComponent]) TFrequencyCandidateDataComponent(data.GetCandidates(iComponent), data.GetThreshold(iComponent));
        }
    }
    TFrequencyCandidateData::~TFrequencyCandidateData()
    {
        fComponentData->Clear("C");
        delete fComponentData;
    }


    //************************************
    // TFrequencyCandidateDataComponent
    //************************************

    TFrequencyCandidateDataComponent::TFrequencyCandidateDataComponent() :
            TObject(),
            fCandidates(new TClonesArray()),
            fThreshold(0.)
    {
    }
    TFrequencyCandidateDataComponent::TFrequencyCandidateDataComponent(const KTFrequencyCandidateData::Candidates& candidates, Double_t threshold) :
            TObject(),
            fCandidates(new TClonesArray("TFrequencyCandidateDataCandidate", candidates.size())),
            fThreshold(threshold)
    {
        UInt_t iCandidate = 0;
        for (KTFrequencyCandidateData::Candidates::const_iterator it = candidates.begin(); it != candidates.end(); it++)
        {
            new(&fCandidates[iCandidate]) TFrequencyCandidateDataCandidate(*it);
            iCandidate++;
        }
    }
    TFrequencyCandidateDataComponent::~TFrequencyCandidateDataComponent()
    {
        fCandidates->Clear("C");
        delete fCandidates;
    }


    //************************************
    // TFrequencyCandidateDataCandidate
    //************************************

    TFrequencyCandidateDataCandidate::TFrequencyCandidateDataCandidate() :
            TObject(),
            fFirstBin(0),
            fLastBin(0),
            fMeanFrequency(0.)
    {
    }
    TFrequencyCandidateDataCandidate::TFrequencyCandidateDataCandidate(const KTFrequencyCandidate& candidate) :
            TObject(),
            fFirstBin(candidate.GetFirstBin()),
            fLastBin(candidate.GetLastBin()),
            fMeanFrequency(candidate.GetMeanFrequency())
    {
    }
    TFrequencyCandidateDataCandidate::~TFrequencyCandidateDataCandidate()
    {
    }

} /* namespace Katydid */

