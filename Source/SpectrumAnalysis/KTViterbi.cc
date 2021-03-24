/*
 * KTViterbi.cc
 *
 *  Created on: Mar 1, 2021
 *      Author: nbuzinsky
 */

#include "KTViterbi.hh"

#include "KTLogger.hh"

#include "KTEggHeader.hh"
#include "KTSliceHeader.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTGainVariationData.hh"
#include "KTDiscriminatedPoints1DData.hh"

#include <numeric>
#include <cmath>

using std::vector;


namespace Katydid
{
    KTLOGGER(vittylog, "KTViterbi");

    KT_REGISTER_PROCESSOR(KTViterbi, "viterbi");

    KTViterbi::KTViterbi(const std::string& name) :
                    KTProcessor(name),
                    fMinFrequency(0.),
                    fMaxFrequency(1.),
                    fCalculateMinBin(true),
                    fCalculateMaxBin(true),
                    fHeaderSlot("header", this, &KTViterbi::InitializeWithHeader),
                    fDiscrimSlot("disc-1d", this, &KTViterbi::CollectDiscrimPointsFromSlice),
                    fDoneSlot("done", this, &KTViterbi::AcquisitionIsOver, &fClusterDoneSignal)
    {
    }

    KTViterbi::~KTViterbi()
    {
    }

    bool KTViterbi::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMinFrequency(node->get_value("min-frequency", GetMinFrequency()));
        SetMaxFrequency(node->get_value("max-frequency", GetMaxFrequency()));
        SetP0(node->get_value("p0", GetP0()));
        SetP1(node->get_value("p1", GetP1()));
        SetTauTrack(node->get_value("tau-track", GetTauTrack()));
        SetTauEvent(node->get_value("tau-event", GetTauEvent()));
        SetEventRate(node->get_value("event-rate", GetEventRate()));

        if (node->has("min-bin"))
        {
            SetMinBin(node->get_value< unsigned >("min-bin"));
            SetCalculateMinBin(false);
        }
        if (node->has("max-bin"))
        {
            SetMaxBin(node->get_value< unsigned >("max-bin"));
            SetCalculateMaxBin(false);
        }

        return true;
    }

    bool KTViterbi::InitializeWithHeader(KTEggHeader& header)
    {
        fTimeBinWidth = 1. / header.GetAcquisitionRate();
        fFreqBinWidth = 1. / (fTimeBinWidth * header.GetChannelHeader(0)->GetSliceSize());

        return true;
    }

    bool KTViterbi::CollectDiscrimPointsFromSlice(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discrimPoints)
    {

        unsigned nComponents = 1;

        fFreqBinWidth = (double) slHeader.GetSampleRate() / (double) slHeader.GetRawSliceSize();
        KTDEBUG(vittylog, "Frequency bin width " << fFreqBinWidth);

        if (fCalculateMinBin)
        {
            SetMinBin((unsigned) ( fMinFrequency / fFreqBinWidth ) );
            KTDEBUG(vittylog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin((unsigned) ( fMaxFrequency / fFreqBinWidth ) );
            KTDEBUG(vittylog, "Maximum bin set to " << fMaxBin);
        }


        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            uint64_t acqID = slHeader.GetAcquisitionID(iComponent);

            unsigned nBins = fMaxBin - fMinBin + 1;

            KTWARN(vittylog, "Found Slice! Adding column of length "<< nBins + 1 << " to Viterbi matrices!");

            vector<double> vZeros(nBins + 1,0);

            fT1.push_back(vZeros);
            fT2.push_back(vZeros);

            double newTimeInAcq = slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength();
            double newTimeInRunC = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();
            KTDEBUG(vittylog, "new_TimeInAcq is " << newTimeInAcq);
            KTDEBUG(vittylog, "new_TimeInRunC is " << newTimeInRunC);

            const KTDiscriminatedPoints1DData::SetOfPoints&  points = discrimPoints.GetSetOfPoints(iComponent);

            KTWARN( vittylog, "Collected "<<points.size()<<" points");

            // Loop over the high power points
            this->LoopOverHighPowerPoints(points, acqID, iComponent);

        }
        return true;
    }

    bool KTViterbi::LoopOverHighPowerPoints(KTDiscriminatedPoints1DData::SetOfPoints points, uint64_t acqID, unsigned component)
    {
        std::vector<unsigned> highPowerBins;

        for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            highPowerBins.push_back(pIt->first + 1);
            //KTWARN(vittylog, "discriminated point: bin = " <<pIt->first<< ", frequency = "<<pIt->second.fAbscissa<< ", amplitude = "<<pIt->second.fOrdinate);
        }

        //auto log_B = GetEmissionVector(highPowerBins);
        //MostProbablePreviousState(iTimeSlice, iState, true, log_B);

        return true;
    }


    bool KTViterbi::BacktrackBestPath()
    {
        vector<unsigned> xBestPath(fNWindow);

        vector<double> finalColumn = fT1.back();
        auto maxPointer = std::max_element(finalColumn.begin(), finalColumn.end());
        xBestPath[fNWindow-1] = maxPointer - finalColumn.begin();

        //Recursively backtrack to find most probable path
        for(unsigned iTimeSlice=fNWindow-1; iTimeSlice > 0; --iTimeSlice)
            xBestPath[iTimeSlice-1] = fT2[ xBestPath[iTimeSlice]] [iTimeSlice];

        return true;
    }


    vector<double> KTViterbi::GetEmissionVector(vector<unsigned> highPowerBins)
    {
        unsigned nHPBins = highPowerBins.size();
        double normalization = nHPBins * log(fP0) +  (fNBins - nHPBins) * log(fP1);
        double emission_H0 = normalization + log(1. - fP1) - log(1. - fP0);
        double emission_H1 = normalization + log(fP1) - log(fP0);

        vector<double> emissionVector(fNBins+1);

        for(unsigned iState = 1; iState<fNStates; ++iState)
            emissionVector[iState] = emission_H0;

        emissionVector[0] = normalization;

        for(auto it=highPowerBins.begin();it!=highPowerBins.end();++it)
            emissionVector[*it] = emission_H1;

        return emissionVector;
    }



    std::pair<unsigned, double> KTViterbi::FindBestState(vector<unsigned> checkStates, unsigned iState, unsigned iTimeSlice)
    {
        vector<double> markovProbs;
        for(unsigned i=0; i<checkStates.size();++i)
        {
            markovProbs.push_back( fT1[checkStates[i]][iTimeSlice-1] + flog_A[checkStates[i]][iState] );
        }
        auto maxPointer = std::max_element(markovProbs.begin(), markovProbs.end());
        unsigned maxInd = maxPointer - markovProbs.begin();
        return std::pair<unsigned, double>(maxInd,*maxPointer);
    }

    bool KTViterbi::MostProbablePreviousState(unsigned iTimeSlice, unsigned iState, bool highPower, double log_B)
    {
        std::vector<unsigned> checkStates;
        if(!iState)
        {
            checkStates.resize(fNStates);
            std::iota(checkStates.begin(), checkStates.end(), 0);
            std::pair<unsigned, double> bestState = FindBestState(checkStates, iState, iTimeSlice);
            fT2[iState][iTimeSlice] = bestState.first;
            fT1[iState][iTimeSlice] = bestState.second + log_B;
        }
        else if(highPower)
        {
            unsigned kChecks = 3 * fKScatter;
            checkStates.resize(kChecks + 1);
            std::iota(checkStates.begin(), checkStates.end(), iState - kChecks);
            std::pair<unsigned, double> bestState = FindBestState(checkStates, iState, iTimeSlice);
            fT2[iState][iTimeSlice] = bestState.first;
            fT1[iState][iTimeSlice] = bestState.second;
        }
        else
        {
            double markov_prob = fT1[iState][iTimeSlice-1]+ flog_A[iState][iState] + log_B; 
            fT2[iState][iTimeSlice] = iState;
            fT1[iState][iTimeSlice] = markov_prob;
        }
        
        return true;
    }


    bool KTViterbi::InitializeTransitionMatrix()
    {
        vector< vector< double> > transitionMatrix;

        double pCreation = fEventRate * fTimeBinWidth  / fNBins;
        double pAnnihilation = 1. - exp(- fTimeBinWidth / fTauEvent);
        double pTrackToTrack = exp(-fTimeBinWidth / fTauTrack);
        double pNewTrack = 1. - pAnnihilation - pTrackToTrack;

        transitionMatrix[0][0] = 1. - fNBins * pCreation;
        for(unsigned iBin=1;iBin<fNStates; ++iBin)
        {
            transitionMatrix[0][iBin] = pCreation;
            transitionMatrix[iBin][0] = pAnnihilation;
        }

        //*transitionMatrix += GetScatteringMatrix(pTrackToTrack, pNewTrack, fKScatter);

        //Normalize transition matrix so sum of probabilities equals 1 
        //Optimized for Uniform scattering dist. Generalize/ separate out?
        for(unsigned xBin = 1; xBin < fKScatter+1; ++ xBin)
            transitionMatrix[xBin][0] += xBin * pNewTrack / fKScatter;

        return true;
    }


   vector< vector <double> > KTViterbi::GetScatteringMatrix(double pTrackToTrack, double pNewTrack, unsigned fKScatter)
    {
        vector< vector< double> > scatteringMatrix;

        for(unsigned iBin =1; iBin<fNStates; ++iBin)
        {
            scatteringMatrix[iBin][iBin] = pTrackToTrack;
        }

        for(unsigned k=1; k<fKScatter; ++k)
        {
            for(unsigned iBin =1; iBin<fNStates-fKScatter; ++iBin)
            {
                scatteringMatrix[iBin+k][iBin] = pNewTrack / fKScatter;
            }
        }

        return scatteringMatrix;
    }

    void KTViterbi::AcquisitionIsOver()
    {
        KTINFO(vittylog, "Got egg-done signal. Pls work.");
    }

} /* namespace Katydid */
