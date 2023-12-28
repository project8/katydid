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
#include "KTSparseWaterfallCandidateData.hh"
#include "KTDiscriminatedPoint.hh"

#include <cmath>




using std::string;
using std::vector;

namespace Katydid
{
    LOGGER(htlog, "KTHoughTransform");

    KT_REGISTER_PROCESSOR(KTHoughTransform, "hough-transform");

    KTHoughTransform::KTHoughTransform(const std::string& name) :
            KTProcessor(name),
            fNThetaPoints(1),
            fNRPoints(1),
            fCosTheta(0),
            fSinTheta(0),
            fHTSignal("hough", this),
            fSWFCandSlot("swf-cand", this, &KTHoughTransform::TransformData, &fHTSignal),
            fWFCandSlot("wf-cand", this, &KTHoughTransform::TransformData, &fHTSignal),
            fDiscPts2DSlot("disc", this, &KTHoughTransform::TransformData, &fHTSignal)
    {
    }

    KTHoughTransform::~KTHoughTransform()
    {
    }

    bool KTHoughTransform::Configure(const scarab::param_node* node)
    {
        SetNThetaPoints(node->get_value< unsigned >("n-theta-points", fNThetaPoints));
        SetNRPoints(node->get_value< unsigned >("n-r-points", fNRPoints));

        return true;
    }

    bool KTHoughTransform::TransformData(KTSparseWaterfallCandidateData& data)
    {
        KTHoughData& newData = data.Of< KTHoughData >().SetNComponents(1);

        const KTDiscriminatedPoints& points = data.GetPoints();

        KTPhysicalArray< 2, double >* newTransform = TransformPoints(points, data.GetTimeInRunC(), data.GetTimeLength(), data.GetMinFrequency(), data.GetFrequencyWidth());
        if (newTransform == NULL)
        {
            LERROR(htlog, "Something went wrong in the transform");
        }
        else
        {
            newData.SetTransform(newTransform, data.GetTimeInRunC(), data.GetTimeLength(), data.GetMinFrequency(), data.GetFrequencyWidth(), 0);
        }
        LINFO(htlog, "Completed hough transform");

        return true;
    }

    KTPhysicalArray< 2, double >* KTHoughTransform::TransformPoints(const SWFPoints& points, double minTime, double timeLength, double minFreq, double freqWidth)
    {
        LINFO(htlog, "Number of time/frequency points: " << points.size());

        double maxR = KTMath::Sqrt2();
        double timeScaling = 1. / timeLength;
        double freqScaling = 1. / freqWidth;

        KTPhysicalArray< 2, double >* newTransform = new KTPhysicalArray< 2, double >(fNThetaPoints, 0., KTMath::Pi(), fNRPoints, -maxR, maxR);
        for (unsigned iTheta = 0; iTheta < fNThetaPoints; ++iTheta)
        {
            for (unsigned iRadius = 0; iRadius < fNRPoints; ++iRadius)
            {
                (*newTransform)(iTheta, iRadius) = 0.;
            }
        }

        double deltaTheta = KTMath::Pi() / (double)fNThetaPoints;

        // initial loop over theta bins to create the KTPhysicalArrays and to cache cosTheta and sinTheta values
        if (fCosTheta.size() != fNThetaPoints || fSinTheta.size() != fNThetaPoints)
        {
            fCosTheta.resize(fNThetaPoints);
            fSinTheta.resize(fNThetaPoints);
            double theta = newTransform->GetBinCenter(0, 0);
            for (unsigned iTheta = 0; iTheta < fNThetaPoints; ++iTheta)
            {
                fCosTheta[iTheta] = cos(theta);
                fSinTheta[iTheta] = sin(theta);
                theta += deltaTheta;
            }
        }

        double timeVal, freqVal, value, radius;
        unsigned iRadius;
        for (SWFPoints::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            timeVal = (pIt->fTimeInRunC - minTime) * timeScaling;
            freqVal = (pIt->fFrequency - minFreq) * freqScaling;
            value = pIt->fAmplitude;

            for (unsigned iTheta = 0; iTheta < fNThetaPoints; ++iTheta)
            {
                radius = timeVal * fCosTheta[iTheta] + freqVal * fSinTheta[iTheta];

                iRadius = newTransform->FindBin(2, radius);

                (*newTransform)(iTheta, iRadius) = (*newTransform)(iTheta, iRadius) + value;
            }
        }

        return newTransform;
    }

    bool KTHoughTransform::TransformData(KTWaterfallCandidateData& data)
    {
        KTHoughData& newData = data.Of< KTHoughData >().SetNComponents(1);

        const KTTimeFrequency* candidate = data.GetCandidate();

        KTPhysicalArray< 2, double >* newTransform = TransformSpectrum(candidate);
        if (newTransform == NULL)
        {
            LERROR(htlog, "Something went wrong in the transform");
        }
        else
        {
            newData.SetTransform(newTransform, 0., candidate->GetTimeBinWidth(), 0., candidate->GetFrequencyBinWidth(), 0);
        }
        LINFO(htlog, "Completed hough transform");

        return true;
    }

    KTPhysicalArray< 2, double >* KTHoughTransform::TransformSpectrum(const KTTimeFrequency* powerSpectrum)
    {
        unsigned nTimeBins = powerSpectrum->GetNTimeBins();
        unsigned nFreqBins = powerSpectrum->GetNFrequencyBins();

        //LINFO(htlog, "time info: " << nTimeBins << "  " << powerSpectrum->GetRangeMin(0) << "  " << powerSpectrum->GetRangeMax(0) << "  " << powerSpectrum->GetBinWidth(0));
        //LINFO(htlog, "freq info: " << nFreqBins << "  " << powerSpectrum->GetRangeMin(1) << "  " << powerSpectrum->GetRangeMax(1) << "  " << powerSpectrum->GetBinWidth(1));

        double maxR = sqrt(double(nTimeBins*nTimeBins + nFreqBins*nFreqBins));

        KTPhysicalArray< 2, double >* newTransform = new KTPhysicalArray< 2, double >(fNThetaPoints, 0., KTMath::Pi(), fNRPoints, -maxR, maxR);
        for (unsigned iTheta = 0; iTheta < fNThetaPoints; ++iTheta)
        {
            for (unsigned iRadius = 0; iRadius < fNRPoints; ++iRadius)
            {
                (*newTransform)(iTheta, iRadius) = 0.;
            }
        }

        double deltaTheta = KTMath::Pi() / (double)fNThetaPoints;

        // initial loop over theta bins to create the KTPhysicalArrays and to cache cosTheta and sinTheta values
        if (fCosTheta.size() != fNThetaPoints || fSinTheta.size() != fNThetaPoints)
        {
            fCosTheta.resize(fNThetaPoints);
            fSinTheta.resize(fNThetaPoints);
            double theta = newTransform->GetBinCenter(0, 0);
            for (unsigned iTheta = 0; iTheta < fNThetaPoints; iTheta++)
            {
                fCosTheta[iTheta] = cos(theta);
                fSinTheta[iTheta] = sin(theta);
                theta += deltaTheta;
            }
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
                    radius = timeVal * fCosTheta[iTheta] + freqVal * fSinTheta[iTheta];

                    iRadius = newTransform->FindBin(2, radius);

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

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTDiscriminatedPoints2DData::SetOfPoints inputPoints = data.GetSetOfPoints(iComponent);

            KTPhysicalArray< 2, double >* newTransform = TransformSetOfPoints(inputPoints, data.GetNBinsX(), data.GetNBinsY());
            if (newTransform == NULL)
            {
                LERROR(htlog, "Something went wrong in transform " << iComponent);
                return false;
            }
            else
            {
                newData.SetTransform(newTransform, 0., 1., 0., 1., iComponent);
            }
        }
        LINFO(htlog, "Completed hough transform for " << nComponents << " components");

        return true;
    }

    KTPhysicalArray< 2, double >* KTHoughTransform::TransformSetOfPoints(const SetOfPoints& points, unsigned nTimeBins, unsigned nFreqBins)
    {
        LINFO(htlog, "Number of time/frequency points: " << points.size());

        double maxR = sqrt(double(nTimeBins*nTimeBins + nFreqBins*nFreqBins));

        KTPhysicalArray< 2, double >* newTransform = new KTPhysicalArray< 2, double >(fNThetaPoints, 0., KTMath::Pi(), fNRPoints, -maxR, maxR);
        for (unsigned iTheta = 0; iTheta < fNThetaPoints; ++iTheta)
        {
            for (unsigned iRadius = 0; iRadius < fNRPoints; ++iRadius)
            {
                (*newTransform)(iTheta, iRadius) = 0.;
            }
        }

        double deltaTheta = KTMath::Pi() / (double)fNThetaPoints;

        // initial loop over theta bins to create the KTPhysicalArrays and to cache cosTheta and sinTheta values
        if (fCosTheta.size() != fNThetaPoints || fSinTheta.size() != fNThetaPoints)
        {
            fCosTheta.resize(fNThetaPoints);
            fSinTheta.resize(fNThetaPoints);
            double theta = newTransform->GetBinCenter(0, 0);
            for (unsigned iTheta = 0; iTheta < fNThetaPoints; iTheta++)
            {
                fCosTheta[iTheta] = cos(theta);
                fSinTheta[iTheta] = sin(theta);
                theta += deltaTheta;
            }
        }

        double timeVal, freqVal, value, radius;
        unsigned iRadius;
        for (SetOfPoints::const_iterator pIt = points.begin(); pIt != points.end(); pIt++)
        {
            timeVal = pIt->first.first;
            freqVal = pIt->first.second;
            value = pIt->second.fAbscissa;

            for (unsigned iTheta = 0; iTheta < fNThetaPoints; iTheta++)
            {
                radius = timeVal * fCosTheta[iTheta] + freqVal * fSinTheta[iTheta];

                iRadius = newTransform->FindBin(2, radius);

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
