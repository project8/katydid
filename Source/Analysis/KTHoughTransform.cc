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

using boost::shared_ptr;

using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(htlog, "katydid.analysis");

    static KTDerivedNORegistrar< KTProcessor, KTHoughTransform > sHTRegistrar("hough-transform");

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

    Bool_t KTHoughTransform::Configure(const KTPStoreNode* node)
    {
        SetNThetaPoints(node->GetData< UInt_t >("n-theta-points", fNThetaPoints));
        SetNRPoints(node->GetData< UInt_t >("n-r-points", fNRPoints));

        return true;
    }
/*
    Bool_t KTHoughTransform::TransformData(KTSlidingWindowFSDataFFTW& data)
    {
        KTHoughData* newData = new KTHoughData(data->GetNComponents());

        for (UInt_t iComponent=0; iComponent<data->GetNComponents(); iComponent++)
        {
            const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* inputSpectrum = data->GetSpectra(iComponent);

            KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* freqSpectra = RemoveNegativeFrequencies(inputSpectrum);

            KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* newTransform = TransformSpectrum(freqSpectra);
            if (newTransform == NULL)
            {
                KTERROR(htlog, "Something went wrong in transform " << iComponent);
            }
            else
            {
                newData->SetTransform(newTransform, iComponent);
            }

            for (UInt_t iPS = 0; iPS < freqSpectra->size(); iPS++)
            {
                delete (*freqSpectra)(iPS);
            }
            delete freqSpectra;
        }

        return newData;
    }

    KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* KTHoughTransform::TransformSpectrum(const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* powerSpectrum)
    {
        UInt_t nTimeBins = powerSpectrum->size();
        UInt_t nFreqBins = (*powerSpectrum)(0)->size();

        KTINFO(htlog, "time info: " << nTimeBins << "  " << powerSpectrum->GetRangeMin() << "  " << powerSpectrum->GetRangeMax() << "  " << powerSpectrum->GetBinWidth());
        KTINFO(htlog, "freq info: " << nFreqBins << "  " << (*powerSpectrum)(0)->GetRangeMin() << "  " << (*powerSpectrum)(0)->GetRangeMax() << "  " << (*powerSpectrum)(0)->GetBinWidth());

        Double_t maxR = sqrt(Double_t(nTimeBins*nTimeBins + nFreqBins*nFreqBins));

        KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* newTransform = new KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >(fNThetaPoints, 0., KTMath::Pi());

        Double_t deltaTheta = KTMath::Pi() / (Double_t)fNThetaPoints;

        // initial loop over theta bins to create the KTPhysicalArrays and to cache cosTheta and sinTheta values
        vector< Double_t > cosTheta(fNThetaPoints);
        vector< Double_t > sinTheta(fNThetaPoints);
        Double_t theta = newTransform->GetBinCenter(0);
        for (UInt_t iTheta = 0; iTheta < fNThetaPoints; iTheta++)
        {
            (*newTransform)(iTheta) = new KTPhysicalArray< 1, Double_t >(fNRPoints, -maxR, maxR);
            cosTheta[iTheta] = cos(theta);
            sinTheta[iTheta] = sin(theta);
            theta += deltaTheta;
        }

        Double_t timeVal, freqVal, value, radius;
        UInt_t iRadius;
        for (UInt_t iTime = 0; iTime < nTimeBins; iTime++)
        {
            timeVal = Double_t(iTime);

            for (UInt_t iFreq = 0; iFreq < nFreqBins; iFreq++)
            {
                value = (*(*powerSpectrum)(iTime))(iFreq).abs();
                if (value < 1.e-4) continue; // HARD CODED THRESHOLD

                freqVal = Double_t(iFreq);

                for (UInt_t iTheta = 0; iTheta < fNThetaPoints; iTheta++)
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

    Bool_t KTHoughTransform::TransformData(KTDiscriminatedPoints2DData& data)
    {
        UInt_t nComponents = data.GetNComponents();
        KTHoughData& newData = data.Of< KTHoughData >().SetNComponents(nComponents);

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTDiscriminatedPoints2DData::SetOfPoints inputPoints = data.GetSetOfPoints(iComponent);

            KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* newTransform = TransformSetOfPoints(inputPoints, data.GetNBinsX(), data.GetNBinsY());
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

    KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* KTHoughTransform::TransformSetOfPoints(const SetOfPoints& points, UInt_t nTimeBins, UInt_t nFreqBins)
    {
        KTINFO(htlog, "Number of time/frequency points: " << points.size());

        Double_t maxR = sqrt(Double_t(nTimeBins*nTimeBins + nFreqBins*nFreqBins));

        KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* newTransform = new KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >(fNThetaPoints, 0., KTMath::Pi());

        Double_t deltaTheta = KTMath::Pi() / (Double_t)fNThetaPoints;

        // initial loop over theta bins to create the KTPhysicalArrays and to cache cosTheta and sinTheta values
        vector< Double_t > cosTheta(fNThetaPoints);
        vector< Double_t > sinTheta(fNThetaPoints);
        Double_t theta = newTransform->GetBinCenter(0);
        for (UInt_t iTheta = 0; iTheta < fNThetaPoints; iTheta++)
        {
            (*newTransform)(iTheta) = new KTPhysicalArray< 1, Double_t >(fNRPoints, -maxR, maxR);
            cosTheta[iTheta] = cos(theta);
            sinTheta[iTheta] = sin(theta);
            theta += deltaTheta;
        }

        Double_t timeVal, freqVal, value, radius;
        UInt_t iRadius;
        for (SetOfPoints::const_iterator pIt = points.begin(); pIt != points.end(); pIt++)
        {
            timeVal = pIt->first.first;
            freqVal = pIt->first.second;
            value = pIt->second;

            for (UInt_t iTheta = 0; iTheta < fNThetaPoints; iTheta++)
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
        UInt_t nTimeBins = inputSpectrum->size();
        KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* newFrequencySpectra = new KTPhysicalArray< 1, KTFrequencySpectrumPolar* >(nTimeBins, inputSpectrum->GetRangeMin(), inputSpectrum->GetRangeMax());

        for (Int_t iTimeBin=0; iTimeBin<nTimeBins; iTimeBin++)
        {
            KTFrequencySpectrumPolar* newSpectrum = (*inputSpectrum)(iTimeBin)->CreateFrequencySpectrum();
            (*newFrequencySpectra)(iTimeBin) = newSpectrum;
        }
        return newFrequencySpectra;
    }
*/

} /* namespace Katydid */
