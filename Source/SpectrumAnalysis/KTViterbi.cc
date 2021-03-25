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

        SetMinBin(node->get_value("min-bin", GetMinBin()));
        SetMaxBin(node->get_value("max-bin", GetMaxBin()));
        SetP0(node->get_value("p0", GetP0()));
        SetP1(node->get_value("p1", GetP1()));
        SetTauTrack(node->get_value("tau-track", GetTauTrack()));
        SetTauEvent(node->get_value("tau-event", GetTauEvent()));
        SetEventRate(node->get_value("event-rate", GetEventRate()));
        SetKScatter(node->get_value("k-scatter", GetKScatter()));

        SetNBins( fMaxBin - fMinBin + 1 );
        SetNStates( fNBins + 1 );

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
        vector<double> vZeros(fNStates, 0);

        if(fT1.empty())
        {
            KTWARN(vittylog, "Initializing Transition Matrix with "<<fNStates<<" states....");
            InitializeTransitionMatrix();
            vector<double> vPrior(fNStates, 0);
            vPrior[0] = 1.;
            fT1.push_back(truncated_log(vPrior));
            fT2.push_back(vZeros);
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            uint64_t acqID = slHeader.GetAcquisitionID(iComponent);

            KTWARN(vittylog, "Found Slice! Adding column of length "<< fNStates << " to Viterbi matrices!");

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
        std::vector<unsigned> highPowerStates;

        for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            highPowerStates.push_back(BinToStateID(pIt->first)) ;
            KTWARN(vittylog, "discriminated point: bin = " <<pIt->first<< ", frequency = "<<pIt->second.fAbscissa<< ", amplitude = "<<pIt->second.fOrdinate);
        }

        if(!std::is_sorted(highPowerStates.begin(), highPowerStates.end()))
        {  
            KTWARN(vittylog, "Sorting discriminated points by bin number. Why aren't they sorted already?");
            std::sort(highPowerStates.begin(), highPowerStates.end());
        }

        vector<double> log_B = GetEmissionVector(highPowerStates);
        unsigned iTimeSlice = fT1.size() - 1;
        
        KTWARN(vittylog, "Looping over discriminated points in time Slice: "<<iTimeSlice);

        MostProbablePreviousState(iTimeSlice, 0, true, log_B[0]);

        for(auto itState = highPowerStates.begin(); itState!=highPowerStates.end(); ++itState)
        {
            MostProbablePreviousState(iTimeSlice, *itState, true, log_B[*itState]);
        }

        vector<unsigned> nStates(fNStates);
        vector<unsigned> lowPowerStates(fNStates);

        std::iota(nStates.begin(), nStates.end(), 0);
        auto it = std::set_difference(nStates.begin(), nStates.end(), highPowerStates.begin(), highPowerStates.end(), lowPowerStates.begin());
        lowPowerStates.resize(it-lowPowerStates.begin());

        for(auto itState = lowPowerStates.begin(); itState!=lowPowerStates.end(); ++itState)
        {
            MostProbablePreviousState(iTimeSlice, *itState, false, log_B[*itState]);
        }

        return true;
    }


    vector<double> KTViterbi::GetEmissionVector(vector<unsigned> highPowerStates)
    {
        unsigned nHPStates = highPowerStates.size();
        double normalization = nHPStates * log(fP0) +  (fNBins - nHPStates) * log(fP1);
        double emission_H0 = normalization + log(1. - fP1) - log(1. - fP0);
        double emission_H1 = normalization + log(fP1) - log(fP0);

        vector<double> emissionVector(fNStates, emission_H0);

        emissionVector[0] = normalization;

        for(auto it=highPowerStates.begin();it!=highPowerStates.end();++it)
            emissionVector[*it] = emission_H1;

        return truncated_log(emissionVector);
    }



    std::pair<unsigned, double> KTViterbi::FindBestState(vector<unsigned> checkStates, unsigned iState, unsigned iTimeSlice)
    {
        vector<double> markovProbs;
        for(unsigned i=0; i<checkStates.size();++i)
        {
            markovProbs.push_back( fT1[iTimeSlice-1][checkStates[i]] + flog_A[checkStates[i]][iState] );
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
            fT2[iTimeSlice][iState] = bestState.first;
            fT1[iTimeSlice][iState] = bestState.second + log_B;
        }
        else if(highPower)
        {
            unsigned kChecks = fmin(3 * fKScatter, iState - 1);
            checkStates.resize(kChecks + 1);
            std::iota(checkStates.begin(), checkStates.end(), iState - kChecks);
            std::pair<unsigned, double> bestState = FindBestState(checkStates, iState, iTimeSlice);
            fT2[iTimeSlice][iState] = bestState.first;
            fT1[iTimeSlice][iState] = bestState.second;
        }
        else
        {
            double markov_prob = fT1[iTimeSlice-1][iState]+ flog_A[iState][iState] + log_B; 
            fT2[iTimeSlice][iState] = iState;
            fT1[iTimeSlice][iState] = markov_prob;
        }
        
        return true;
    }

    bool KTViterbi::BacktrackBestPath()
    {
        unsigned nWindow = fT1.size();
        vector<unsigned> xBestPath(nWindow);

        vector<double> finalColumn = fT1.back();
        auto maxPointer = std::max_element(finalColumn.begin(), finalColumn.end());
        xBestPath[nWindow-1] = maxPointer - finalColumn.begin();

        //Recursively backtrack to find most probable path
        for(unsigned iTimeSlice=nWindow-1; iTimeSlice > 0; --iTimeSlice)
            xBestPath[iTimeSlice-1] = fT2[iTimeSlice] [xBestPath[iTimeSlice]]; 

        return true;
    }


    bool KTViterbi::InitializeTransitionMatrix()
    {
        //Initialize transition matrix to all zeros: nStates x nStates
        vector<vector<double>> transitionMatrix(fNStates, vector<double>(fNStates, 0));

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

        //Fill out scattering profile: here assumed to be uniform. Generalize?
        for(unsigned iBin=1; iBin<fNStates; ++iBin)
        {
            transitionMatrix[iBin][iBin] = pTrackToTrack;
        }

        for(unsigned k=1; k<fKScatter; ++k)
        {
            for(unsigned iBin =1; iBin<fNStates-fKScatter; ++iBin)
            {
                transitionMatrix[iBin+k][iBin] = pNewTrack / fKScatter;
            }
        }

        //Normalize transition matrix so sum of probabilities equals 1 
        //Optimized for Uniform scattering dist. Generalize/ separate out?
        for(unsigned xBin = 1; xBin < fKScatter+1; ++ xBin)
            transitionMatrix[xBin][0] += xBin * pNewTrack / fKScatter;

        flog_A = truncated_log(transitionMatrix);
        return true;
    }

    const unsigned KTViterbi::BinToStateID(const unsigned &binID)
    {
        return (binID - fMinBin) + 1;

    }

    //Prevent infinities by truncating log(x) as x-> 0
    const double KTViterbi::truncated_log(const double &aDouble) const
    {
        return ( aDouble > 0  ? log(aDouble) : -1000.); //assuming 1000 is approximately infinite
    }

    vector<double> KTViterbi::truncated_log(vector<double> aVector) const
    {
        for(unsigned i=0; i<aVector.size(); ++i) aVector[i] = truncated_log(aVector[i]);
        return aVector;
    }

    //I am sure there is a better way to do this with STL. Lambda not acceptable. Better solutions seems dependent on C++11
    vector<vector<double>> KTViterbi::truncated_log(vector<vector<double>> aMatrix) const
    {
        for(unsigned i=0; i<aMatrix.size(); ++i) aMatrix[i] = truncated_log(aMatrix[i]);
        return aMatrix;
    }


    void KTViterbi::AcquisitionIsOver()
    {
        //BacktrackBestPath();
        KTINFO(vittylog, "Got egg-done signal. Pls work.");
    }

} /* namespace Katydid */
