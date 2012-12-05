/*
 * KTHoughTransform.cc
 *
 *  Created on: Nov 14, 2012
 *      Author: nsoblath
 */

#include "KTHoughTransform.hh"

#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTHoughData.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPowerSpectrum.hh"
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
        RegisterSlot("ts-data", this, &KTHoughTransform::ProcessSWFSData, "void (const KTSlidingWindowFSDataFFTW*)");
        RegisterSlot("event", this, &KTHoughTransform::ProcessEvent, "void (KTEvent*)");
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

            KTPhysicalArray< 1, KTPowerSpectrum* >* powerSpectrum = CreatePowerSpectrum(inputSpectrum);

            KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* newTransform = TransformSpectrum(powerSpectrum);
            if (newTransform == NULL)
            {
                KTERROR(htlog, "Something went wrong in transform " << iChannel);
            }
            else
            {
                newData->SetTransform(newTransform, iChannel);
            }

            for (UInt_t iPS = 0; iPS < inputSpectrum->size(); iPS++)
            {
                delete (*inputSpectrum)(iPS);
            }
            delete powerSpectrum;
        }

        newData->SetEvent(data->GetEvent());
        newData->SetName(fOutputDataName);

        fHTSignal(newData);

        return newData;
    }

    KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* KTHoughTransform::TransformSpectrum(const KTPhysicalArray< 1, KTPowerSpectrum* >* powerSpectrum)
    {
        // The time and frequency axes are scaled so that their ranges are both 0-1 (+ half a bin width on either end)

        //Double_t freqMin = fabs((*inputSpectrum)(0)->GetRangeMin());
        //Double_t freqMax = fabs((*inputSpectrum)(0)->GetRangeMax());
        //Double_t maxR = inputSpectrum->GetRangeMax() + (freqMax > freqMin ? freqMax : freqMin);

        UInt_t nTimeBins = powerSpectrum->size();
        UInt_t nFreqBins = (*powerSpectrum)(0)->size();

        KTINFO(htlog, "time info: " << nTimeBins << "  " << powerSpectrum->GetRangeMin() << "  " << powerSpectrum->GetRangeMax() << "  " << powerSpectrum->GetBinWidth());
        KTINFO(htlog, "freq info: " << nFreqBins << "  " << (*powerSpectrum)(0)->GetRangeMin() << "  " << (*powerSpectrum)(0)->GetRangeMax() << "  " << (*powerSpectrum)(0)->GetBinWidth());

        //Double_t timeScaling = 1. / ((Double_t(nTimeBins) - 0.5) * powerSpectrum->GetBinWidth());
        //Double_t freqScaling = 1. / ((Double_t(nFreqBins) - 0.5) * (*powerSpectrum)(0)->GetBinWidth());
        //Double_t maxR = 2.;
        Double_t maxR = sqrt(Double_t(nTimeBins*nTimeBins + nFreqBins*nFreqBins));

        KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* newTransform = new KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >(fNThetaPoints, 0., KTMath::Pi());

        Double_t deltaTheta = KTMath::Pi() / (Double_t)fNThetaPoints;
        //Double_t deltaTime = powerSpectrum->GetBinWidth() * timeScaling;
        //Double_t deltaFreq = (*powerSpectrum)(0)->GetBinWidth() * freqScaling;

        //KTINFO(htlog, deltaTime << "  " << deltaFreq << "  " << timeScaling << "  " << freqScaling);

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
                value = (*(*powerSpectrum)(iTime))(iFreq);
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


    KTPhysicalArray< 1, KTPowerSpectrum* >* KTHoughTransform::CreatePowerSpectrum(const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* inputSpectrum)
    {
        UInt_t nTimeBins = inputSpectrum->size();
        KTPhysicalArray< 1, KTPowerSpectrum* >* newPowerSpectrum = new KTPhysicalArray< 1, KTPowerSpectrum* >(nTimeBins, inputSpectrum->GetRangeMin(), inputSpectrum->GetRangeMax());

        for (Int_t iTimeBin=0; iTimeBin<nTimeBins; iTimeBin++)
        {
            KTPowerSpectrum* newSinglePS = (*inputSpectrum)(iTimeBin)->CreatePowerSpectrum();
            (*newPowerSpectrum)(iTimeBin) = newSinglePS;
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
        return newPowerSpectrum;
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

        KTEvent* event = data->GetEvent();
        if (event != NULL)
        {
            event->AddData(newData);
        }

        return;
    }

    void KTHoughTransform::ProcessEvent(shared_ptr<KTEvent> event)
    {
        const KTSlidingWindowFSDataFFTW* data = dynamic_cast< KTSlidingWindowFSDataFFTW* >(event->GetData(fInputDataName));
        if (data == NULL)
        {
            KTWARN(htlog, "No sliding-window frequency-spectrum data named <" << fInputDataName << "> was available in the event");
            return;
        }

        ProcessSWFSData(data);
        return;
    }

} /* namespace Katydid */
