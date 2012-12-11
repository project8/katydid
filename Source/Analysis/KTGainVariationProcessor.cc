/*
 * KTGainVariation.cc
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#include "KTGainVariation.hh"

#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include <cmath>
#include <vector>

using std::string;
using std::vector;
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(gvlog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTGainVariation > sGainVarRegistrar("gain-variation");

    KTGainVariation::KTGainVariation() :
            KTProcessor(),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fNFitPoints(5),
            fInputDataName("frequency-spectrum"),
            fOutputDataName("gain-variation")
    {
        fConfigName = "gain-variation";

        RegisterSignal("gain-var", &fGainVarSignal, "void (const KTGainVariationData*)");

        RegisterSlot("event", this, &KTGainVariation::ProcessEvent, "void (shared_ptr<KTEvent>)");
        RegisterSlot("fsdata", this, &KTGainVariation::ProcessFrequencySpectrumData, "void (const KTFrequencySpectrumData*)");
        RegisterSlot("fsdata-fftw", this, &KTGainVariation::ProcessFrequencySpectrumDataFFTW, "void (const KTFrequencySpectrumDataFFTW*)");
    }

    KTGainVariation::~KTGainVariation()
    {
    }

    Bool_t KTGainVariation::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("min-frequency"))
        {
            SetMinFrequency(node->GetData< Double_t >("min-frequency"));
        }
        if (node->HasData("max-frequency"))
        {
            SetMaxFrequency(node->GetData< Double_t >("max-frequency"));
        }

        if (node->HasData("min-bin"))
        {
            SetMinBin(node->GetData< UInt_t >("min-bin"));
        }
        if (node->HasData("max-bin"))
        {
            SetMaxBin(node->GetData< UInt_t >("max-bin"));
        }

        SetNFitPoints(node->GetData< UInt_t >("fit-points", fNFitPoints));

        SetInputDataName(node->GetData< string >("input-data-name", fInputDataName));
        SetOutputDataName(node->GetData< string >("output-data-name", fOutputDataName));

        return true;
    }

    KTGainVariationData* KTGainVariation::PerformFit(const KTFrequencySpectrumData* data)
    {
        if (fCalculateMinBin) SetMinBin(data->GetSpectrum(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin(data->GetSpectrum(0)->FindBin(fMaxFrequency));

        UInt_t nTotalBins = fMinBin - fMaxBin + 1;
        UInt_t nBinsPerFitPoint = nTotalBins / fNFitPoints; // integer division rounds down; there may be bins leftover unused

        KTINFO(gvlog, "Performing gain variation fits with " << fNFitPoints << ", and " << nBinsPerFitPoint << " bins averaged per fit point.");

        UInt_t nChannels = data->GetNChannels();

        Double_t sigmaNorm = 1. / Double_t(nBinsPerFitPoint - 1);
        for (UInt_t iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTFrequencySpectrum* spectrum = data->GetSpectrum(iChannel);

            vector< FitPoint > fitPoints(fNFitPoints);

            // Calculate fit points
            for (UInt_t iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
            {
                UInt_t fitPointStartBin = iFitPoint * nBinsPerFitPoint;
                UInt_t fitPointEndBin = fitPointStartBin + nBinsPerFitPoint;
                fitPoints[iFitPoint].fX = Double_t(fitPointStartBin + 0.5 * nBinsPerFitPoint);

                Double_t mean = 0.;
                for (UInt_t iBin=fitPointStartBin; iBin<fitPointEndBin; iBin++)
                {
                    mean += (*spectrum)(iBin).abs();
                }
                mean /= (Double_t)nBinsPerFitPoint;
                fitPoints[iFitPoint].fY = mean;

                Double_t sigma = 0., diff;
                for (UInt_t iBin=fitPointStartBin; iBin<fitPointEndBin; iBin++)
                {
                    diff = (*spectrum)(iBin).abs() - mean;
                    sigma += diff * diff;
                }
                fitPoints[iFitPoint].fSigma = sqrt(sigma * sigmaNorm);
            }

            FitResult results = DoFit(fitPoints);
        }

        return NULL;
    }

    KTGainVariationData* KTGainVariation::PerformFit(const KTFrequencySpectrumDataFFTW* data)
    {
        if (fCalculateMinBin) SetMinBin(data->GetSpectrum(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin(data->GetSpectrum(0)->FindBin(fMaxFrequency));

        UInt_t nTotalBins = fMinBin - fMaxBin + 1;
        UInt_t nBinsPerFitPoint = nTotalBins / fNFitPoints; // integer division rounds down; there may be bins leftover unused

        KTINFO(gvlog, "Performing gain variation fit with " << fNFitPoints << ", and " << nBinsPerFitPoint << " bins averaged per fit point.");

        return NULL;
    }

    KTGainVariation::FitResult KTGainVariation::DoFit(const vector< FitPoint >& fitPoints)
    {
        // I apologize for all of the short, non-standard variable names here.
        // They're this way to match the algebra I worked out offline.
        // -- Noah

        Double_t S=0., S_x=0., S_xx=0., S_xxx=0., S_xxxx=0., S_y=0., S_xy=0., S_xxy=0.;
        Double_t invSigmaSq, xTemp, xMultTemp, yTemp;
        for (UInt_t iPoint=0; iPoint<fitPoints.size(); iPoint++)
        {
            invSigmaSq = 1. / (fitPoints[iPoint].fSigma * fitPoints[iPoint].fSigma);
            xTemp = fitPoints[iPoint].fX;
            yTemp = fitPoints[iPoint].fY;
            xMultTemp = xTemp; // == x

            S += invSigmaSq; // == 1 / sigma^2
            S_y += yTemp * invSigmaSq; // == y / sigma^2
            S_x += xMultTemp * invSigmaSq; // == x / sigma^2
            S_xy += xMultTemp * yTemp * invSigmaSq; // == x * y / sigma^2
            xMultTemp *= xTemp; // == x^2
            S_xx += xMultTemp * invSigmaSq; // == x^2 / sigma^2
            S_xxy += xMultTemp * yTemp * invSigmaSq; // == x^2 * y / sigma^2
            xMultTemp *= xTemp; // == x^3
            S_xxx += xMultTemp * invSigmaSq; // == x^3 / sigma^2
            xMultTemp *= xTemp; // == x^4
            S_xxx += xMultTemp * invSigmaSq; // == x^4 / sigma^2

        }

        Double_t WX_denom = 1. / (S_xx * S_xx - S * S_xxxx);
        Double_t W = (S_xx * S_y - S * S_xxy) * WX_denom;
        Double_t X = (S * S_xxx - S_xx * S_x) * WX_denom;

        Double_t UV_denom = 1. / (S * S - S_x * S_x);
        Double_t U = (S * S_xy - S_x * S_y) * UV_denom;
        Double_t V = (S_xx * S_x - S * S_xxx) * UV_denom;

        FitResult results;
        results.fA = (W + U * X) / (1 - V * X);
        results.fB = U + results.fA * V;
        results.fC = (S_y - results.fA * S_xx - results.fB * S_x) / S;

        return results;
    }



    void KTGainVariation::ProcessEvent(shared_ptr<KTEvent> event)
    {
        const KTFrequencySpectrumData* fsData = dynamic_cast< KTFrequencySpectrumData* >(event->GetData(fInputDataName));
        if (fsData != NULL)
        {
            KTGainVariationData* newData = PerformFit(fsData);
            event->AddData(newData);
            return;
        }

        const KTFrequencySpectrumDataFFTW* fsDataFFTW = dynamic_cast< KTFrequencySpectrumDataFFTW* >(event->GetData(fInputDataName));
        if (fsData != NULL)
        {
            KTGainVariationData* newData = PerformFit(fsData);
            event->AddData(newData);
            return;
        }

        KTWARN(gvlog, "No time series data named <" << fInputDataName << "> was available in the event");
        return;
    }

    void KTGainVariation::ProcessFrequencySpectrumData(const KTFrequencySpectrumData* data)
    {
        KTGainVariationData* newData = PerformFit(data);
        if (data->GetEvent() != NULL)
            data->GetEvent()->AddData(newData);
        return;
    }
    void KTGainVariation::ProcessFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* data)
    {
        KTGainVariationData* newData = PerformFit(data);
        if (data->GetEvent() != NULL)
            data->GetEvent()->AddData(newData);
        return;
    }



} /* namespace Katydid */
