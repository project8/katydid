/*
 * KTHoughTransform.cc
 *
 *  Created on: Nov 14, 2012
 *      Author: nsoblath
 */

#include "KTHoughTransform.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTHoughData.hh"
#include "KTMath.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTParam.hh"
//#include "KTSlidingWindowFSDataFFTW.hh"

#include <cmath>
#include <vector>

//#include "TFile.h"
//#include "TH2.h"



using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(htlog, "KTHoughTransform");

    KT_REGISTER_PROCESSOR(KTHoughTransform, "hough-transform");

    KTHoughTransform::KTHoughTransform(const std::string& name) :
            KTProcessor(name),
            fNThetaPoints(1),
            fNRPoints(1),
            fHTSignal("hough-transform", this),
            fWFCandSlot("wf-cand", this, &KTHoughTransform::TransformData, &fHTSignal),
            fDiscPts2DSlot("disc", this, &KTHoughTransform::TransformData, &fHTSignal)
    {
    }

    KTHoughTransform::~KTHoughTransform()
    {
    }

    bool KTHoughTransform::Configure(const KTParamNode* node)
    {
        SetNThetaPoints(node->GetValue< unsigned >("n-theta-points", fNThetaPoints));
        SetNRPoints(node->GetValue< unsigned >("n-r-points", fNRPoints));

        return true;
    }

    bool KTHoughTransform::TransformData(KTWaterfallCandidateData& data)
    {
        KTHoughData& newData = data.Of< KTHoughData >().SetNComponents(1);

        const KTTimeFrequency* candidate = data.GetCandidate();

        KTPhysicalArray< 2, double >* newTransform = TransformSpectrum(candidate);
        if (newTransform == NULL)
        {
            KTERROR(htlog, "Something went wrong in the transform");
        }
        else
        {
            newData.SetTransform(newTransform, 0);
        }
        KTINFO(htlog, "Completed hough transform");

        return true;
    }

    KTPhysicalArray< 2, double >* KTHoughTransform::TransformSpectrum(const KTTimeFrequency* powerSpectrum)
    {
        unsigned nTimeBins = powerSpectrum->GetNTimeBins();
        unsigned nFreqBins = powerSpectrum->GetNFrequencyBins();

        //KTINFO(htlog, "time info: " << nTimeBins << "  " << powerSpectrum->GetRangeMin(0) << "  " << powerSpectrum->GetRangeMax(0) << "  " << powerSpectrum->GetBinWidth(0));
        //KTINFO(htlog, "freq info: " << nFreqBins << "  " << powerSpectrum->GetRangeMin(1) << "  " << powerSpectrum->GetRangeMax(1) << "  " << powerSpectrum->GetBinWidth(1));

        double maxR = sqrt(double(nTimeBins*nTimeBins + nFreqBins*nFreqBins));

        KTPhysicalArray< 2, double >* newTransform = new KTPhysicalArray< 2, double >(fNThetaPoints, 0., KTMath::Pi(), fNRPoints, -maxR, maxR);

        double deltaTheta = KTMath::Pi() / (double)fNThetaPoints;

        // initial loop over theta bins to create the KTPhysicalArrays and to cache cosTheta and sinTheta values
        vector< double > cosTheta(fNThetaPoints);
        vector< double > sinTheta(fNThetaPoints);
        double theta = newTransform->GetBinCenter(0, 0);
        for (unsigned iTheta = 0; iTheta < fNThetaPoints; iTheta++)
        {
            cosTheta[iTheta] = cos(theta);
            sinTheta[iTheta] = sin(theta);
            theta += deltaTheta;
        }

        double timeVal, freqVal, value, radius;
        unsigned iRadius;
        for (unsigned iTime = 0; iTime < nTimeBins; iTime++)
        {
            timeVal = double(iTime);

            for (unsigned iFreq = 0; iFreq < nFreqBins; iFreq++)
            {
                value = powerSpectrum->GetAbs(iTime, iFreq);
                //if (value < 1.e-4) continue; // HARD CODED THRESHOLD

                freqVal = double(iFreq);

                for (unsigned iTheta = 0; iTheta < fNThetaPoints; iTheta++)
                {
                    radius = timeVal * cosTheta[iTheta] + freqVal * sinTheta[iTheta];

                    iRadius = newTransform->FindBin(1, radius);

                    (*newTransform)(iTheta, iRadius) = (*newTransform)(iTheta, iRadius) + value;
                }
            }
        }

        return newTransform;
    }


    bool KTHoughTransform::TransformData(KTDiscriminatedPoints2DData& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTHoughData& newData = data.Of< KTHoughData >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTDiscriminatedPoints2DData::SetOfPoints inputPoints = data.GetSetOfPoints(iComponent);

            KTPhysicalArray< 2, double >* newTransform = TransformSetOfPoints(inputPoints, data.GetNBinsX(), data.GetNBinsY());
            if (newTransform == NULL)
            {
                KTERROR(htlog, "Something went wrong in transform " << iComponent);
                return false;
            }
            else
            {
                newData.SetTransform(newTransform, iComponent);
            }
        }
        KTINFO(htlog, "Completed hough transform for " << nComponents << " components");

        return true;
    }

    KTPhysicalArray< 2, double >* KTHoughTransform::TransformSetOfPoints(const SetOfPoints& points, unsigned nTimeBins, unsigned nFreqBins)
    {
        KTINFO(htlog, "Number of time/frequency points: " << points.size());

        double maxR = sqrt(double(nTimeBins*nTimeBins + nFreqBins*nFreqBins));

        KTPhysicalArray< 2, double >* newTransform = new KTPhysicalArray< 2, double >(fNThetaPoints, 0., KTMath::Pi(), fNRPoints, -maxR, maxR);

        double deltaTheta = KTMath::Pi() / (double)fNThetaPoints;

        // initial loop over theta bins to create the KTPhysicalArrays and to cache cosTheta and sinTheta values
        vector< double > cosTheta(fNThetaPoints);
        vector< double > sinTheta(fNThetaPoints);
        double theta = newTransform->GetBinCenter(0, 0);
        for (unsigned iTheta = 0; iTheta < fNThetaPoints; iTheta++)
        {
            cosTheta[iTheta] = cos(theta);
            sinTheta[iTheta] = sin(theta);
            theta += deltaTheta;
        }

        double timeVal, freqVal, value, radius;
        unsigned iRadius;
        for (SetOfPoints::const_iterator pIt = points.begin(); pIt != points.end(); pIt++)
        {
            timeVal = pIt->first.first;
            freqVal = pIt->first.second;
            value = pIt->second;

            for (unsigned iTheta = 0; iTheta < fNThetaPoints; iTheta++)
            {
                radius = timeVal * cosTheta[iTheta] + freqVal * sinTheta[iTheta];

                iRadius = newTransform->FindBin(1, radius);

                (*newTransform)(iTheta, iRadius) = (*newTransform)(iTheta, iRadius) + value;
            }
        }

        return newTransform;
    }

/*
    KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* KTHoughTransform::RemoveNegativeFrequencies(const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* inputSpectrum)
    {
        unsigned nTimeBins = inputSpectrum->size();
        KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* newFrequencySpectra = new KTPhysicalArray< 1, KTFrequencySpectrumPolar* >(nTimeBins, inputSpectrum->GetRangeMin(), inputSpectrum->GetRangeMax());

        for (int iTimeBin=0; iTimeBin<nTimeBins; iTimeBin++)
        {
            KTFrequencySpectrumPolar* newSpectrum = (*inputSpectrum)(iTimeBin)->CreateFrequencySpectrum();
            (*newFrequencySpectra)(iTimeBin) = newSpectrum;
        }
        return newFrequencySpectra;
    }
*/

} /* namespace Katydid */
