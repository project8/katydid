/*
 * TFrequencyCandidateData.cc
 *
 *  Created on: Jan 25, 2013
 *      Author: nsoblath
 */

#include "TFrequencyCandidateData.hh"

#include "KTFrequencyCandidate.hh"
#include "KTFrequencyCandidateData.hh"

#include "TClonesArray.h"

#include "TROOT.h"

#include <iostream>


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
            fComponentData(new TClonesArray("Katydid::TFrequencyCandidateDataComponent", 0)),
            fNBins(0),
            fBinWidth(0.),
            fTimeInRun(0.)
    {
        //gROOT->GetListOfClasses()->Print();
    }
    TFrequencyCandidateData::TFrequencyCandidateData(const KTFrequencyCandidateData& data) :
            TObject(),
            fComponentData(new TClonesArray("Katydid::TFrequencyCandidateDataComponent", data.GetNGroups())),
            fNBins(data.GetNBins()),
            fBinWidth(data.GetBinWidth()),
            fTimeInRun(data.GetTimeInRun())
    {
        Load(data);
    }
    TFrequencyCandidateData::~TFrequencyCandidateData()
    {
        Clear();
        delete fComponentData;
    }

    void TFrequencyCandidateData::Load(const KTFrequencyCandidateData& data)
    {
        Clear();

        fComponentData->Expand(data.GetNGroups());

        for (UInt_t iComponent=0; iComponent < data.GetNGroups(); iComponent++)
        {
            new((*fComponentData)[iComponent]) TFrequencyCandidateDataComponent(data.GetCandidates(iComponent), data.GetThreshold(iComponent));
        }
        std::cout << "fComponentData now has " << fComponentData->GetEntries() << " entries (" << data.GetNGroups() << ")" << std::endl;
        //fComponentData->Print("", -1);
        return;
    }

    void TFrequencyCandidateData::Clear()
    {
        fComponentData->Clear("C");
        TObject::Clear();
        return;
    }


    //************************************
    // TFrequencyCandidateDataComponent
    //************************************

    TFrequencyCandidateDataComponent::TFrequencyCandidateDataComponent() :
            TObject(),
            fCandidates(new TClonesArray("Katydid::TFrequencyCandidateDataCandidate", 0)),
            fThreshold(0.)
    {
    }
    TFrequencyCandidateDataComponent::TFrequencyCandidateDataComponent(const KTFrequencyCandidateData::Candidates& candidates, Double_t threshold) :
            TObject(),
            fCandidates(new TClonesArray("Katydid::TFrequencyCandidateDataCandidate", candidates.size())),
            fThreshold(threshold)
    {
        Load(candidates, threshold);
    }
    TFrequencyCandidateDataComponent::~TFrequencyCandidateDataComponent()
    {
        Clear();
        delete fCandidates;
    }

    void TFrequencyCandidateDataComponent::Load(const KTFrequencyCandidateData::Candidates& candidates, Double_t threshold)
    {
        Clear();

        fCandidates->Expand(candidates.size());

        UInt_t iCandidate = 0;
        for (KTFrequencyCandidateData::Candidates::const_iterator it = candidates.begin(); it != candidates.end(); it++)
        {
            new((*fCandidates)[iCandidate]) TFrequencyCandidateDataCandidate(*it);
            iCandidate++;
        }
        std::cout << "fCandidates now has " << fCandidates->GetEntries() << " entries (" << candidates.size() << ")" << std::endl;
        return;
    }

    void TFrequencyCandidateDataComponent::Clear()
    {
        fCandidates->Clear();
        TObject::Clear();
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

    void TFrequencyCandidateDataCandidate::Load(const KTFrequencyCandidate& candidate)
    {
        fFirstBin = candidate.GetFirstBin();
        fLastBin = candidate.GetLastBin();
        fMeanFrequency = candidate.GetMeanFrequency();
        return;
    }



} /* namespace Katydid */

