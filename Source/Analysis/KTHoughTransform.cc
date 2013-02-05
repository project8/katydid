/*
 * KTHoughTransform.cc
 *
 *  Created on: Nov 14, 2012
 *      Author: nsoblath
 */

#include "KTHoughTransform.hh"

#include "KTBundle.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTHoughData.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTFrequencySpectrum.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTWriteableData.hh"

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

    static KTDerivedRegistrar< KTProcessor, KTHoughTransform > sHTRegistrar("hough-transform");

    KTHoughTransform::KTHoughTransform() :
            KTProcessor(),
            fNThetaPoints(1),
            fNRPoints(1),
            fInputDataName("sliding-window-fs"),
            fOutputDataName("hough-transform"),
            fHTSignal()
    {
        fConfigName = "hough-transform";

        RegisterSignal("hough-transform", &fHTSignal, "void (const KTWriteableData*)");

        //RegisterSlot("header", this, &KTHoughTransform::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("swfs-data", this, &KTHoughTransform::ProcessSWFSData, "void (const KTSlidingWindowFSDataFFTW*)");
        RegisterSlot("disc-data", this, &KTHoughTransform::ProcessDiscriminatedData, "void (const KTDiscriminatedPoints2DData*)");
        RegisterSlot("bundle", this, &KTHoughTransform::ProcessEvent, "void (shared_ptr<KTBundle>)");
    }

    KTHoughTransform::~KTHoughTransform()
    {
    }

    Bool_t KTHoughTransform::Configure(const KTPStoreNode* node)
    {
        SetNThetaPoints(node->GetData< UInt_t >("n-theta-points", fNThetaPoints));
        SetNRPoints(node->GetData< UInt_t >("n-r-points", fNRPoints));

        SetInputDataName(node->GetData< string >("input-data-name", fInputDataName));
        SetOutputDataName(node->GetData< string >("output-data-name", fOutputDataName));

        return true;
    }

    KTHoughData* KTHoughTransform::TransformData(const KTSlidingWindowFSDataFFTW* data)
    {
        KTHoughData* newData = new KTHoughData(data->GetNChannels());

        for (UInt_t iChannel=0; iChannel<data->GetNChannels(); iChannel++)
        {
            const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* inputSpectrum = data->GetSpectra(iChannel);

            KTPhysicalArray< 1, KTFrequencySpectrum* >* freqSpectra = RemoveNegativeFrequencies(inputSpectrum);

            KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* newTransform = TransformSpectrum(freqSpectra);
            if (newTransform == NULL)
            {
                KTERROR(htlog, "Something went wrong in transform " << iChannel);
            }
            else
            {
                newData->SetTransform(newTransform, iChannel);
            }

            for (UInt_t iPS = 0; iPS < freqSpectra->size(); iPS++)
            {
                delete (*freqSpectra)(iPS);
            }
            delete freqSpectra;
        }

        newData->SetEvent(data->GetEvent());
        newData->SetName(fOutputDataName);

        fHTSignal(newData);

        return newData;
    }

    KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* KTHoughTransform::TransformSpectrum(const KTPhysicalArray< 1, KTFrequencySpectrum* >* powerSpectrum)
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


    KTHoughData* KTHoughTransform::TransformData(const KTDiscriminatedPoints2DData* data)
    {
        KTHoughData* newData = new KTHoughData(data->GetNChannels());

        for (UInt_t iChannel=0; iChannel<data->GetNChannels(); iChannel++)
        {
            const KTDiscriminatedPoints2DData::SetOfPoints inputPoints = data->GetSetOfPoints(iChannel);

            KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* newTransform = TransformSetOfPoints(inputPoints, data->GetNBinsX(), data->GetNBinsY());
            if (newTransform == NULL)
            {
                KTERROR(htlog, "Something went wrong in transform " << iChannel);
            }
            else
            {
                newData->SetTransform(newTransform, iChannel);
            }
        }

        newData->SetEvent(data->GetEvent());
        newData->SetName(fOutputDataName);

        fHTSignal(newData);

        return newData;

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


    KTPhysicalArray< 1, KTFrequencySpectrum* >* KTHoughTransform::RemoveNegativeFrequencies(const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* inputSpectrum)
    {
        UInt_t nTimeBins = inputSpectrum->size();
        KTPhysicalArray< 1, KTFrequencySpectrum* >* newFrequencySpectra = new KTPhysicalArray< 1, KTFrequencySpectrum* >(nTimeBins, inputSpectrum->GetRangeMin(), inputSpectrum->GetRangeMax());

        for (Int_t iTimeBin=0; iTimeBin<nTimeBins; iTimeBin++)
        {
            KTFrequencySpectrum* newSpectrum = (*inputSpectrum)(iTimeBin)->CreateFrequencySpectrum();
            (*newFrequencySpectra)(iTimeBin) = newSpectrum;
        }
/*
        TFile* f = new TFile("htpowerspect.root", "recreate");
        TH2D* hist = new TH2D("powerspect", "PowerSpectrum",
                newPowerSpectrum->size(), newPowerSpectrum->GetRangeMin(), newPowerSpectrum->GetRangeMax(),
                (*newPowerSpectrum)(0)->size(), (*newPowerSpectrum)(0)->GetRangeMin(), (*newPowerSpectrum)(0)->GetRangeMax());

        KTINFO("Frequency axis: " << (*newPowerSpectrum)(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO("Time axis: " << newPowerSpectrum->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (Int_t iBinX=1; iBinX<=(Int_t)newPowerSpectrum->size(); iBinX++)
        {
            KTPowerSpectrum* fs = (*newPowerSpectrum)(iBinX-1);
            for (Int_t iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*fs)(iBinY-1));
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        hist->Write();
        f->Close();
*/
        return newFrequencySpectra;
    }


/*
    void ProcessSWFSData(const KTSlidingWindowFSDataFFTW* data)::ProcessHeader(const KTEggHeader* header)
    {
        return;
    }
*/

    void KTHoughTransform::ProcessSWFSData(const KTSlidingWindowFSDataFFTW* data)
    {
        KTHoughData* newData = TransformData(data);

        if (newData == NULL)
        {
            KTERROR(htlog, "Unable to transform data");
            return;
        }

        KTBundle* bundle = data->GetEvent();
        if (bundle != NULL)
        {
            bundle->AddData(newData);
        }

        return;
    }

    void KTHoughTransform::ProcessDiscriminatedData(const KTDiscriminatedPoints2DData* data)
    {
        KTHoughData* newData = TransformData(data);

        if (newData == NULL)
        {
            KTERROR(htlog, "Unable to transform data");
            return;
        }

        KTBundle* bundle = data->GetEvent();
        if (bundle != NULL)
        {
            bundle->AddData(newData);
        }

        return;
    }


    void KTHoughTransform::ProcessEvent(shared_ptr<KTBundle> bundle)
    {
        const KTDiscriminatedPoints2DData* dpData = bundle->GetData< KTDiscriminatedPoints2DData >(fInputDataName);
        if (dpData != NULL)
        {
            ProcessDiscriminatedData(dpData);
            return;
        }

        const KTSlidingWindowFSDataFFTW* swsfData = bundle->GetData< KTSlidingWindowFSDataFFTW >(fInputDataName);
        if (swsfData != NULL)
        {
            ProcessSWFSData(swsfData);
            return;
        }

        KTWARN(htlog, "No sliding-window frequency-spectrum data named <" << fInputDataName << "> was available in the bundle");
        return;
    }

} /* namespace Katydid */
