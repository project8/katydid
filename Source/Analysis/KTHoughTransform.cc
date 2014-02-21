/*
 * KTHoughTransform.cc
 *
 *  Created on: Nov 14, 2012
 *      Author: nsoblath
 */

#include "KTHoughTransform.hh"

#include "KTNOFactory.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTHoughData.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTPStoreNode.hh"
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

    static KTNORegistrar< KTProcessor, KTHoughTransform > sHTRegistrar("hough-transform");

    KTHoughTransform::KTHoughTransform(const std::string& name) :
            KTProcessor(name),
            fNThetaPoints(1),
            fNRPoints(1),
            fHTSignal("hough-transform", this),
            fDiscPts2DSlot("disc", this, &KTHoughTransform::TransformData, &fHTSignal)
    {
    }

    KTHoughTransform::~KTHoughTransform()
    {
    }

    bool KTHoughTransform::Configure(const KTPStoreNode* node)
    {
        SetNThetaPoints(node->GetData< unsigned >("n-theta-points", fNThetaPoints));
        SetNRPoints(node->GetData< unsigned >("n-r-points", fNRPoints));

        return true;
    }
/*
    bool KTHoughTransform::TransformData(KTSlidingWindowFSDataFFTW& data)
    {
        KTHoughData* newData = new KTHoughData(data->GetNComponents());

        for (unsigned iComponent=0; iComponent<data->GetNComponents(); iComponent++)
        {
            const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* inputSpectrum = data->GetSpectra(iComponent);

            KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* freqSpectra = RemoveNegativeFrequencies(inputSpectrum);

            KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* newTransform = TransformSpectrum(freqSpectra);
            if (newTransform == NULL)
            {
                KTERROR(htlog, "Something went wrong in transform " << iComponent);
            }
            else
            {
                newData->SetTransform(newTransform, iComponent);
            }

            for (unsigned iPS = 0; iPS < freqSpectra->size(); iPS++)
            {
                delete (*freqSpectra)(iPS);
            }
            delete freqSpectra;
        }

        return newData;
    }

    KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* KTHoughTransform::TransformSpectrum(const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* powerSpectrum)
    {
        unsigned nTimeBins = powerSpectrum->size();
        unsigned nFreqBins = (*powerSpectrum)(0)->size();

        KTINFO(htlog, "time info: " << nTimeBins << "  " << powerSpectrum->GetRangeMin() << "  " << powerSpectrum->GetRangeMax() << "  " << powerSpectrum->GetBinWidth());
        KTINFO(htlog, "freq info: " << nFreqBins << "  " << (*powerSpectrum)(0)->GetRangeMin() << "  " << (*powerSpectrum)(0)->GetRangeMax() << "  " << (*powerSpectrum)(0)->GetBinWidth());

        double maxR = sqrt(double(nTimeBins*nTimeBins + nFreqBins*nFreqBins));

        KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* newTransform = new KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >(fNThetaPoints, 0., KTMath::Pi());

        double deltaTheta = KTMath::Pi() / (double)fNThetaPoints;

        // initial loop over theta bins to create the KTPhysicalArrays and to cache cosTheta and sinTheta values
        vector< double > cosTheta(fNThetaPoints);
        vector< double > sinTheta(fNThetaPoints);
        double theta = newTransform->GetBinCenter(0);
        for (unsigned iTheta = 0; iTheta < fNThetaPoints; iTheta++)
        {
            (*newTransform)(iTheta) = new KTPhysicalArray< 1, double >(fNRPoints, -maxR, maxR);
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
                value = (*(*powerSpectrum)(iTime))(iFreq).abs();
                if (value < 1.e-4) continue; // HARD CODED THRESHOLD

                freqVal = double(iFreq);

                for (unsigned iTheta = 0; iTheta < fNThetaPoints; iTheta++)
                {
                    radius = timeVal * cosTheta[iTheta] + freqVal * sinTheta[iTheta];

                    iRadius = (*newTransform)(iTheta)->FindBin(radius);

                    (*(*newTransform)(iTheta))(iRadius) = (*(*newTransform)(iTheta))(iRadius) + value;
                }
            }
        }

        return newTransform;
    }
*/

    bool KTHoughTransform::TransformData(KTDiscriminatedPoints2DData& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTHoughData& newData = data.Of< KTHoughData >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTDiscriminatedPoints2DData::SetOfPoints inputPoints = data.GetSetOfPoints(iComponent);

            KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* newTransform = TransformSetOfPoints(inputPoints, data.GetNBinsX(), data.GetNBinsY());
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

    KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* KTHoughTransform::TransformSetOfPoints(const SetOfPoints& points, unsigned nTimeBins, unsigned nFreqBins)
    {
        KTINFO(htlog, "Number of time/frequency points: " << points.size());

        double maxR = sqrt(double(nTimeBins*nTimeBins + nFreqBins*nFreqBins));

        KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* newTransform = new KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >(fNThetaPoints, 0., KTMath::Pi());

        double deltaTheta = KTMath::Pi() / (double)fNThetaPoints;

        // initial loop over theta bins to create the KTPhysicalArrays and to cache cosTheta and sinTheta values
        vector< double > cosTheta(fNThetaPoints);
        vector< double > sinTheta(fNThetaPoints);
        double theta = newTransform->GetBinCenter(0);
        for (unsigned iTheta = 0; iTheta < fNThetaPoints; iTheta++)
        {
            (*newTransform)(iTheta) = new KTPhysicalArray< 1, double >(fNRPoints, -maxR, maxR);
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

                iRadius = (*newTransform)(iTheta)->FindBin(radius);

                (*(*newTransform)(iTheta))(iRadius) = (*(*newTransform)(iTheta))(iRadius) + value;
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
