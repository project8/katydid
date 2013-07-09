/*
 * KTGainVariationProcessor.cc
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#include "KTGainVariationProcessor.hh"

#include "KTNOFactory.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTSpline.hh"

#include <cmath>
#include <vector>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::string;
using std::vector;
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(gvlog, "katydid.analysis");

    static KTDerivedNORegistrar< KTProcessor, KTGainVariationProcessor > sGainVarRegistrar("gain-variation");

    KTGainVariationProcessor::KTGainVariationProcessor(const std::string& name) :
            KTProcessor(name),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fNFitPoints(5),
            fGainVarSignal("gain-var", this),
            fFSPolarSlot("fs-polar", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            fFSFFTWSlot("fs-fftw", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal)
    {
    }

    KTGainVariationProcessor::~KTGainVariationProcessor()
    {
    }

    Bool_t KTGainVariationProcessor::Configure(const KTPStoreNode* node)
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

        return true;
    }

    Bool_t KTGainVariationProcessor::CalculateGainVariation(KTFrequencySpectrumDataPolar& data)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrumPolar(0)->FindBin(fMinFrequency));
        }
        else
        {
            fMinFrequency = data.GetSpectrumPolar(0)->GetBinCenter(fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrumPolar(0)->FindBin(fMaxFrequency));
        }
        else
        {
            fMaxFrequency = data.GetSpectrumPolar(0)->GetBinCenter(fMaxBin);
        }
        KTDEBUG(gvlog, "min frequency: " << fMinFrequency << "; max frequency: " << fMaxFrequency << "; min bin: " << fMinBin << "; max bin " << fMaxBin << "; input range max " << data.GetSpectrumPolar(0)->GetRangeMin() << "; input range min: " << data.GetSpectrumPolar(0)->GetRangeMax());

        UInt_t nTotalBins = fMaxBin - fMinBin + 1;
        UInt_t nBinsPerFitPoint = nTotalBins / fNFitPoints; // integer division rounds down; there may be bins leftover unused

        KTDEBUG(gvlog, "Performing gain variation fits with " << fNFitPoints << " points, and " << nBinsPerFitPoint << " bins averaged per fit point.");

        UInt_t nComponents = data.GetNComponents();

        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(nComponents);

        Double_t sigmaNorm = 1. / Double_t(nBinsPerFitPoint - 1);
        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTFrequencySpectrumPolar* spectrum = data.GetSpectrumPolar(iComponent);

            Double_t* xVals = new Double_t[fNFitPoints];
            Double_t* yVals = new Double_t[fNFitPoints];

            // Calculate fit points
#pragma omp parallel for default(shared)
            for (UInt_t iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
            {
                UInt_t fitPointStartBin = iFitPoint * nBinsPerFitPoint + fMinBin;
                UInt_t fitPointEndBin = fitPointStartBin + nBinsPerFitPoint;

                Double_t leftEdge = spectrum->GetBinLowEdge(fitPointStartBin);
                Double_t rightEdge = spectrum->GetBinLowEdge(fitPointEndBin);
                xVals[iFitPoint] = leftEdge + 0.5 * (rightEdge - leftEdge);

                Double_t mean = 0.;
                for (UInt_t iBin=fitPointStartBin; iBin<fitPointEndBin; iBin++)
                {
                    mean += (*spectrum)(iBin).abs();
                }
                mean /= (Double_t)nBinsPerFitPoint;
                yVals[iFitPoint] = mean;

                KTDEBUG(gvlog, "Fit point " << iFitPoint << "  " << xVals[iFitPoint] << "  " << yVals[iFitPoint]);
            }

            // Normalize the fit points to 1
            Double_t minYVal = yVals[0];
            for (UInt_t iFitPoint=1; iFitPoint < fNFitPoints; iFitPoint++)
            {
                if (yVals[iFitPoint] < minYVal) minYVal = yVals[iFitPoint];
            }
            for (UInt_t iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
            {
                yVals[iFitPoint] = yVals[iFitPoint] / minYVal;
            }

            KTSpline* spline = new KTSpline(xVals, yVals, fNFitPoints);
            spline->SetXMin(fMinFrequency);
            spline->SetXMax(fMaxFrequency);
            //GainVariation* gainVarResult = CreateGainVariation(spline, spectrum->GetNBins(), spectrum->GetRangeMin(), spectrum->GetRangeMax());

            delete [] xVals;
            delete [] yVals;

            newData.SetSpline(spline, iComponent);
            //newData->SetGainVariation(gainVarResult, iComponent);
        }
        KTINFO(gvlog, "Completed gain variation calculation for " << nComponents);

        return true;
    }

    Bool_t KTGainVariationProcessor::CalculateGainVariation(KTFrequencySpectrumDataFFTW& data)
    {
        // Frequency spectra include negative and positive frequencies; this algorithm operates only on the positive frequencies.
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrumFFTW(0)->FindBin(fMinFrequency));
        }
        else
        {
            fMinFrequency = data.GetSpectrumFFTW(0)->GetBinCenter(fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrumFFTW(0)->FindBin(fMaxFrequency));
        }
        else
        {
            fMaxFrequency = data.GetSpectrumFFTW(0)->GetBinCenter(fMaxBin);
        }
        KTDEBUG(gvlog, "min frequency: " << fMinFrequency << "; max frequency: " << fMaxFrequency << "; min bin: " << fMinBin << "; max bin " << fMaxBin << "; input range max " << data.GetSpectrumFFTW(0)->GetRangeMin() << "; input range min: " << data.GetSpectrumFFTW(0)->GetRangeMax());

        UInt_t nTotalBins = fMaxBin - fMinBin + 1;
        UInt_t nBinsPerFitPoint = nTotalBins / fNFitPoints; // integer division rounds down; there may be bins leftover unused

        KTDEBUG(gvlog, "Performing gain variation fit with " << fNFitPoints << " points, and " << nBinsPerFitPoint << " bins averaged per fit point.");

        UInt_t nComponents = data.GetNComponents();

        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(nComponents);

        Double_t sigmaNorm = 1. / Double_t(nBinsPerFitPoint - 1);
        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTFrequencySpectrumFFTW* spectrum = data.GetSpectrumFFTW(iComponent);

            Double_t* xVals = new Double_t[fNFitPoints];
            Double_t* yVals = new Double_t[fNFitPoints];

            // Calculate fit points
#pragma omp parallel for default(shared)
            for (UInt_t iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
            {
                UInt_t fitPointStartBin = iFitPoint * nBinsPerFitPoint + fMinBin;
                UInt_t fitPointEndBin = fitPointStartBin + nBinsPerFitPoint;

                Double_t leftEdge = spectrum->GetBinLowEdge(fitPointStartBin);
                Double_t rightEdge = spectrum->GetBinLowEdge(fitPointEndBin);
                xVals[iFitPoint] = leftEdge + 0.5 * (rightEdge - leftEdge);

                Double_t mean = 0.;
                for (UInt_t iBin=fitPointStartBin; iBin<fitPointEndBin; iBin++)
                {
                    mean += sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                }
                mean /= (Double_t)nBinsPerFitPoint;
                yVals[iFitPoint] = mean;

                KTDEBUG(gvlog, "Fit point " << iFitPoint << "  " << xVals[iFitPoint] << "  " << yVals[iFitPoint]);
            }

            // Normalize the fit points to 1
            Double_t minYVal = yVals[0];
            for (UInt_t iFitPoint=1; iFitPoint < fNFitPoints; iFitPoint++)
            {
                if (yVals[iFitPoint] < minYVal) minYVal = yVals[iFitPoint];
            }
            for (UInt_t iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
            {
                yVals[iFitPoint] = yVals[iFitPoint] / minYVal;
            }

            KTSpline* spline = new KTSpline(xVals, yVals, fNFitPoints);
            spline->SetXMin(fMinFrequency);
            spline->SetXMax(fMaxFrequency);
            //GainVariation* gainVarResult = CreateGainVariation(spline, spectrum->GetNBins(), spectrum->GetRangeMin(), spectrum->GetRangeMax());

            delete [] xVals;
            delete [] yVals;

            newData.SetSpline(spline, iComponent);
            //newData->SetGainVariation(gainVarResult, iComponent);
        }
        KTINFO(gvlog, "Completed gain variation calculation for " << nComponents);

        return true;
    }

    /*
    KTGainVariationProcessor::GainVariation* KTGainVariationProcessor::CreateGainVariation(TSpline* spline, UInt_t nBins, Double_t rangeMin, Double_t rangeMax) const
    {
        GainVariation* newGainVar = new GainVariation(nBins, rangeMin, rangeMax);

        // The fit region: [fMinBin, fMaxBin)
        // Keep track of the minimum value so we can shift it down to 1
        Double_t minVal = spline->Eval(newGainVar->GetBinCenter(fMinBin));
        for (UInt_t iBin = fMinBin; iBin < fMaxBin; iBin++)
        {
            (*newGainVar)(iBin) = spline->Eval(newGainVar->GetBinCenter(iBin));
            if ((*newGainVar)(iBin) < minVal)
            {
                minVal = (*newGainVar)(iBin);
            }
        }
        for (UInt_t iBin = fMinBin; iBin < fMaxBin; iBin++)
        {
            (*newGainVar)(iBin) = (*newGainVar)(iBin) / minVal;
        }

        // Before the fit region: [0, fMinBin)
        for (UInt_t iBin = 0; iBin < fMinBin; iBin++)
        {
            (*newGainVar)(iBin) = 1.;
        }

        // After the fit region: [fMaxBin, nBins)
        for (UInt_t iBin = fMaxBin; iBin < nBins; iBin++)
        {
            (*newGainVar)(iBin) = 1.;
        }

        return newGainVar;
    }
    */

} /* namespace Katydid */
