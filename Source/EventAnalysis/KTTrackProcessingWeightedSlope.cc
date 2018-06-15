/**
 @file KTTrackProcessingWeightedSlope.cc
 @brief Contains KTTrackProcessingWeightedSlope
 @details Extracts physics-relevant information about tracks using a weighted slope algorithm
 @author: N.S. Oblath, B. LaRoque & M. Guigue
 @date: July 22, 2013
 */

#include "KTTrackProcessingWeightedSlope.hh"

#include "KTHoughData.hh"
#include "KTLogger.hh"

#include "KTProcessedTrackData.hh"
#include "KTSmooth.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTSequentialLineData.hh"

#include <limits>
#include <vector>
#include <algorithm>
#include <numeric>

using boost::shared_ptr;
using std::vector;
using std::string;

namespace Katydid
{
    KTLOGGER(tlog, "KTTrackProcessingWeightedSlope");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTTrackProcessingWeightedSlope, "track-proc-ws");

    KTTrackProcessingWeightedSlope::KTTrackProcessingWeightedSlope(const std::string& name) :
            KTProcessor(name),
            fSlopeMinimum(-std::numeric_limits< double >::max()),
            fProcTrackMinPoints(0),
            fProcTrackAssignedError(0.),
            fTrackSignal("track", this),
            fSWFSlot("swfc", this, &KTTrackProcessingWeightedSlope::ProcessTrack<KTSparseWaterfallCandidateData>, &fTrackSignal),
            fSeqSlot("seqc", this, &KTTrackProcessingWeightedSlope::ProcessTrack<KTSequentialLineData>, &fTrackSignal)
    {
    }

    KTTrackProcessingWeightedSlope::~KTTrackProcessingWeightedSlope()
    {
    }

    bool KTTrackProcessingWeightedSlope::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSlopeMinimum(node->get_value("min-slope", GetSlopeMinimum()));
        SetProcTrackMinPoints(node->get_value("min-points", GetProcTrackMinPoints()));
        SetProcTrackAssignedError(node->get_value("assigned-error", GetProcTrackAssignedError()));

        return true;
    }

    template<typename TracklikeCandidate> 
    bool KTTrackProcessingWeightedSlope::ProcessTrack(TracklikeCandidate& tlcData)
    // Method associated with the <SWFC> slot
    {

        TrackID trackID = ExtractTrackID(tlcData);
        Points& points = tlcData.GetPoints();
        // The & is important!!
        KTProcessedTrackData& procTrack = tlcData.template Of< KTProcessedTrackData >();

        KTDEBUG(tlog, "Setting track reconstruction using \"weighted-slope\" algorithm");
        return KTTrackProcessingWeightedSlope::DoWeightedSlopeAlgorithm(points, trackID, &procTrack);
    }

    bool KTTrackProcessingWeightedSlope::DoWeightedSlopeAlgorithm(Points& points, TrackID trackID,KTProcessedTrackData* procTrack)
    {
        vector< double > timeBinInAcq;
        vector< double > timeBinInRunC;
        vector< double > averageFrequency;

        // Makes a first loop over the points to calculate the weighted average in one time slice
        for (Points::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            timeBinInAcq.push_back(pIt->fTimeInAcq);
            timeBinInRunC.push_back(pIt->fTimeInRunC);
        }
        //Make these lists duplicate-free
        sort( timeBinInAcq.begin(), timeBinInAcq.end() );
        timeBinInAcq.erase( unique( timeBinInAcq.begin(), timeBinInAcq.end() ), timeBinInAcq.end() );

        sort( timeBinInRunC.begin(), timeBinInRunC.end() );
        timeBinInRunC.erase( unique( timeBinInRunC.begin(), timeBinInRunC.end() ), timeBinInRunC.end() );

        const int nTimeBins = timeBinInAcq.size();
        //Derived Quantites
        vector< double > mean(nTimeBins);
        vector< double > variance(nTimeBins);
        vector< double > neighborhoodAmplitude(nTimeBins);
        vector< double > sumPf(nTimeBins);
        vector< double > sumP(nTimeBins);
        vector< double > trackSNR(nTimeBins);
        vector< double > trackNUP(nTimeBins);
        vector< double > wideTrackSNR(nTimeBins);
        vector< double > wideTrackNUP(nTimeBins);
        int nTrackBins = 0.;
        // Calculate the averaged points
        for (Points::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            for (int iTimeBin=0; iTimeBin<nTimeBins; ++iTimeBin)
            {
                if (pIt->fTimeInAcq == timeBinInAcq[iTimeBin])
                {
                    sumPf[iTimeBin] += pIt->fFrequency * pIt->fAmplitude;
                    sumP[iTimeBin] += pIt->fAmplitude;
                    mean[iTimeBin] += pIt->fMean;
                    variance[iTimeBin] += pIt->fVariance;
                    neighborhoodAmplitude[iTimeBin] += pIt->fNeighborhoodAmplitude;

                    trackSNR[iTimeBin] += pIt->fAmplitude/pIt->fMean;
                    trackNUP[iTimeBin] += (pIt->fAmplitude - pIt->fMean) / sqrt(pIt->fVariance);

                    wideTrackSNR[iTimeBin] += pIt->fNeighborhoodAmplitude/pIt->fMean;
                    wideTrackNUP[iTimeBin] += (pIt->fNeighborhoodAmplitude - pIt->fMean) / sqrt(pIt->fVariance);
                    
                    ++nTrackBins;
                    break; //Since time bins are unique
                }
            }
        }

        KTDEBUG(tlog, "Averaging");
        for (unsigned iTimeBin = 0; iTimeBin<nTimeBins; ++iTimeBin)
        {
            averageFrequency.push_back(sumPf[iTimeBin]/sumP[iTimeBin]);
            KTDEBUG(tlog, timeBinInAcq[iTimeBin] << '\t' << averageFrequency[iTimeBin]);
        }

        // Determining the slope and intercept from Chi-2 minimization
        double sumXY = 0, sumXX=0, sumX=0, sumY=0, sumOne = 0, amplitudeSum = 0;

        for (unsigned iTimeBin = 0; iTimeBin<nTimeBins; ++iTimeBin)
        {
            sumXY += averageFrequency[iTimeBin] * timeBinInAcq[iTimeBin];
            sumXX += timeBinInAcq[iTimeBin] * timeBinInAcq[iTimeBin];
            sumX += timeBinInAcq[iTimeBin];
            sumY += averageFrequency[iTimeBin];
            sumOne += 1.;
            amplitudeSum += sumP[iTimeBin];
        }
        double slope = (sumXY*sumOne-sumY*sumX)/(sumXX*sumOne-sumX*sumX);
        double intercept = sumY/sumOne-slope*sumX/sumOne;
        double rho = -sumX/sqrt(sumXX*sumOne); // correlation coefficient between slope and intercept
        KTDEBUG(tlog, "Weighted average Frequency results: \n" <<
                      "\tSlope: " << '\t' << slope << '\n' <<
                      "\tIntercept: " << '\t' << intercept);
        KTDEBUG(tlog, "Amplitude of the track: " << amplitudeSum );

        //Calculating Chi^2_min
        double chi2min = 0;
        double residual = 0;
        for (unsigned iTimeBin = 0; iTimeBin<nTimeBins; ++iTimeBin)
        {
            residual = averageFrequency[iTimeBin] - slope*timeBinInAcq[iTimeBin] - intercept;
            chi2min += residual * residual;
            KTDEBUG(tlog, "Residuals : " << residual );
        }
        // Calculate error on slope and intercept for a rescaled Ch^2_min = 1
        double deltaSlope = 0;
        double deltaIntercept = 0;
        double sigmaStartFreq = 0;
        double sigmaEndFreq = 0;

        // need at least 3 points to get a non-zero Ndf
        if (nTimeBins>2)
        {
            KTDEBUG(tlog, "Chi2min : " << chi2min );

            if (chi2min < 0.1)
            {
                KTDEBUG(tlog, "Chi2min too small (points are mostlikely aligned): assigning arbitrary errors to the averaged Frequency points (" << fProcTrackAssignedError << ")");
                deltaSlope = 1.52/(sqrt(sumXX)/fProcTrackAssignedError);
                deltaIntercept = 1.52/(sqrt(sumOne)/fProcTrackAssignedError);
            }
            else
            {
                double ndf = nTimeBins - 2; // 2: two fitting parameters
                deltaSlope = 1.52/sqrt(sumXX*ndf/chi2min);
                deltaIntercept = 1.52/sqrt(sumOne*ndf/chi2min);
            }
            KTDEBUG(tlog, "Error calculations results: \n" <<
                          "\tSlope: " << '\t' << deltaSlope << '\n' <<
                          "\tIntercept: " << '\t' << deltaIntercept << '\n' <<
                          "\tCorrelation coefficifent: " << '\t' << rho);
            //Calculating error on the starting frequency and the end frequency
            double startTime = *std::min_element(timeBinInAcq.begin(), timeBinInAcq.end());
            double endTime = *std::max_element(timeBinInAcq.begin(), timeBinInAcq.end());
            sigmaStartFreq = sqrt( startTime*startTime *  deltaSlope*deltaSlope + deltaIntercept*deltaIntercept + 2 * startTime * rho * deltaSlope * deltaIntercept );
            sigmaEndFreq = sqrt( endTime*endTime *  deltaSlope*deltaSlope + deltaIntercept*deltaIntercept + 2 * endTime * rho * deltaSlope * deltaIntercept );
        }

        // TODO: Calculate distance to track and see for a possible alpha [%] rejection of noise.

        // Adding resuts to ProcessedTrackData object
        procTrack->SetComponent(trackID.fComponent);
        procTrack->SetAcquisitionID(trackID.fAcquisitionID);
        procTrack->SetTrackID(trackID.fCandidateID);

        procTrack->SetStartTimeInAcq(*std::min_element(timeBinInAcq.begin(), timeBinInAcq.end()));
        procTrack->SetStartTimeInRunC(*std::min_element(timeBinInRunC.begin(), timeBinInRunC.end()));
        procTrack->SetEndTimeInRunC(*std::max_element(timeBinInRunC.begin(), timeBinInRunC.end()));
        procTrack->SetTimeLength(procTrack->GetEndTimeInRunC() - procTrack->GetStartTimeInRunC());
        procTrack->SetStartFrequency(procTrack->GetStartTimeInAcq() * slope + intercept);
        procTrack->SetEndFrequency((procTrack->GetStartTimeInAcq() + procTrack->GetTimeLength()) * slope + intercept);
        procTrack->SetFrequencyWidth(std::abs(procTrack->GetEndFrequency() - procTrack->GetStartFrequency()));
        procTrack->SetSlope(slope);
        procTrack->SetIntercept(intercept);
        procTrack->SetTotalPower(amplitudeSum);

        procTrack->SetNTrackBins(nTrackBins);
        procTrack->SetTotalTrackSNR(std::accumulate(trackSNR.begin(),trackSNR.end(),0.));
        procTrack->SetMaxTrackSNR(*std::max_element(trackSNR.begin(), trackSNR.end()));
        procTrack->SetTotalTrackNUP(std::accumulate(trackNUP.begin(),trackNUP.end(),0.));
        procTrack->SetMaxTrackNUP(*std::max_element(trackNUP.begin(), trackNUP.end()));
        procTrack->SetTotalWideTrackSNR(std::accumulate(wideTrackSNR.begin(),wideTrackSNR.end(),0.));
        procTrack->SetTotalWideTrackNUP(std::accumulate(wideTrackNUP.begin(),wideTrackNUP.end(),0.));


        if (!(slope > fSlopeMinimum))
        {
            procTrack->SetIsCut(true);
        }
        procTrack->SetSlopeSigma(deltaSlope);
        procTrack->SetInterceptSigma(deltaIntercept);
        procTrack->SetStartFrequencySigma(sigmaStartFreq);
        procTrack->SetEndFrequencySigma(sigmaEndFreq);

        return true;
    }

    template<typename TracklikeCandidate> 
    KTTrackProcessingWeightedSlope::TrackID KTTrackProcessingWeightedSlope::ExtractTrackID(TracklikeCandidate tlcData)
    {
        TrackID trackID;
        trackID.fComponent = tlcData.GetComponent();
        trackID.fAcquisitionID = tlcData.GetAcquisitionID();
        trackID.fCandidateID = tlcData.GetCandidateID();
        return trackID;
    }

} /* namespace Katydid */
