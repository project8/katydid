/*
 * KTGainVariationProcessor.cc
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#include "KTGainVariationProcessor.hh"

#include "KTCorrelationData.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTPowerSpectrumData.hh"
#include "KTSpline.hh"

#include <cmath>
#include <vector>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::string;
using std::vector;


namespace Katydid
{
    KTLOGGER(gvlog, "KTGainVariationProcessor");

    KT_REGISTER_PROCESSOR(KTGainVariationProcessor, "gain-variation");

    KTGainVariationProcessor::KTGainVariationProcessor(const std::string& name) :
            KTProcessor(name),
            fNormalize(true),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fNFitPoints(5),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fGainVarSignal("gain-var", this),
            fFSPolarSlot("fs-polar", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            fFSFFTWSlot("fs-fftw", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            fCorrSlot("corr", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            fPSSlot("ps", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal)
    {
    }

    KTGainVariationProcessor::~KTGainVariationProcessor()
    {
    }

    bool KTGainVariationProcessor::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetNormalize(node->get_value< bool >("normalize", fNormalize));

        if (node->has("min-frequency"))
        {
            SetMinFrequency(node->get_value< double >("min-frequency"));
        }
        if (node->has("max-frequency"))
        {
            SetMaxFrequency(node->get_value< double >("max-frequency"));
        }

        if (node->has("min-bin"))
        {
            SetMinBin(node->get_value< unsigned >("min-bin"));
        }
        if (node->has("max-bin"))
        {
            SetMaxBin(node->get_value< unsigned >("max-bin"));
        }

        SetNFitPoints(node->get_value< unsigned >("fit-points", fNFitPoints));

        return true;
    }

    bool KTGainVariationProcessor::CalculateGainVariation(KTFrequencySpectrumDataPolar& data)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());
        return CoreGainVarCalc(data, newData);
    }

    bool KTGainVariationProcessor::CalculateGainVariation(KTFrequencySpectrumDataFFTW& data)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());
        return CoreGainVarCalc(data, newData);
    }

    bool KTGainVariationProcessor::CalculateGainVariation(KTCorrelationData& data)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());
        return CoreGainVarCalc(data, newData);
    }

    bool KTGainVariationProcessor::CalculateGainVariation(KTPowerSpectrumData& data)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());
        return CoreGainVarCalc(data, newData);
    }

    bool KTGainVariationProcessor::CoreGainVarCalc(KTFrequencySpectrumDataPolarCore& data, KTGainVariationData& newData)
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

        unsigned nTotalBins = fMaxBin - fMinBin + 1;
        unsigned nBinsPerFitPoint = nTotalBins / fNFitPoints; // integer division rounds down; there may be bins leftover unused

        KTDEBUG(gvlog, "Performing gain variation fits with " << fNFitPoints << " points, and " << nBinsPerFitPoint << " bins averaged per fit point.");

        unsigned nComponents = data.GetNComponents();

        //double sigmaNorm = 1. / double(nBinsPerFitPoint - 1);
        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTFrequencySpectrumPolar* spectrum = data.GetSpectrumPolar(iComponent);

            double* xVals = new double[fNFitPoints];
            double* yVals = new double[fNFitPoints];

            // Calculate fit points
#pragma omp parallel for default(shared)
            for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
            {
                unsigned fitPointStartBin = iFitPoint * nBinsPerFitPoint + fMinBin;
                unsigned fitPointEndBin = fitPointStartBin + nBinsPerFitPoint;

                double leftEdge = spectrum->GetBinLowEdge(fitPointStartBin);
                double rightEdge = spectrum->GetBinLowEdge(fitPointEndBin);
                xVals[iFitPoint] = leftEdge + 0.5 * (rightEdge - leftEdge);

                double mean = 0.;
                for (unsigned iBin=fitPointStartBin; iBin<fitPointEndBin; iBin++)
                {
                    mean += (*spectrum)(iBin).abs();
                }
                mean /= (double)nBinsPerFitPoint;
                yVals[iFitPoint] = mean;

                KTDEBUG(gvlog, "Fit point " << iFitPoint << "  " << xVals[iFitPoint] << "  " << yVals[iFitPoint]);
            }

            if (fNormalize)
            {
                // Normalize the fit points to 1
                double minYVal = yVals[0];
                for (unsigned iFitPoint=1; iFitPoint < fNFitPoints; iFitPoint++)
                {
                    if (yVals[iFitPoint] < minYVal) minYVal = yVals[iFitPoint];
                }
                for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
                {
                    yVals[iFitPoint] = yVals[iFitPoint] / minYVal;
                }
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

    bool KTGainVariationProcessor::CoreGainVarCalc(KTFrequencySpectrumDataFFTWCore& data, KTGainVariationData& newData)
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

        unsigned nTotalBins = fMaxBin - fMinBin + 1;
        unsigned nBinsPerFitPoint = nTotalBins / fNFitPoints; // integer division rounds down; there may be bins leftover unused

        KTDEBUG(gvlog, "Performing gain variation fit with " << fNFitPoints << " points, and " << nBinsPerFitPoint << " bins averaged per fit point.");

        unsigned nComponents = data.GetNComponents();

        //double sigmaNorm = 1. / double(nBinsPerFitPoint - 1);
        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTFrequencySpectrumFFTW* spectrum = data.GetSpectrumFFTW(iComponent);

            double* xVals = new double[fNFitPoints];
            double* yVals = new double[fNFitPoints];

            // Calculate fit points
#pragma omp parallel for default(shared)
            for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
            {
                unsigned fitPointStartBin = iFitPoint * nBinsPerFitPoint + fMinBin;
                unsigned fitPointEndBin = fitPointStartBin + nBinsPerFitPoint;

                double leftEdge = spectrum->GetBinLowEdge(fitPointStartBin);
                double rightEdge = spectrum->GetBinLowEdge(fitPointEndBin);
                xVals[iFitPoint] = leftEdge + 0.5 * (rightEdge - leftEdge);

                double mean = 0.;
                for (unsigned iBin=fitPointStartBin; iBin<fitPointEndBin; iBin++)
                {
                    mean += sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                }
                mean /= (double)nBinsPerFitPoint;
                yVals[iFitPoint] = mean;

                KTDEBUG(gvlog, "Fit point " << iFitPoint << "  " << xVals[iFitPoint] << "  " << yVals[iFitPoint]);
            }

            // Normalize the fit points to 1
            if (fNormalize)
            {
                double minYVal = yVals[0];
                for (unsigned iFitPoint=1; iFitPoint < fNFitPoints; iFitPoint++)
                {
                    if (yVals[iFitPoint] < minYVal) minYVal = yVals[iFitPoint];
                }
                for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
                {
                    yVals[iFitPoint] = yVals[iFitPoint] / minYVal;
                }
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

    bool KTGainVariationProcessor::CoreGainVarCalc(KTPowerSpectrumData& data, KTGainVariationData& newData)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrum(0)->FindBin(fMinFrequency));
        }
        else
        {
            fMinFrequency = data.GetSpectrum(0)->GetBinCenter(fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrum(0)->FindBin(fMaxFrequency));
        }
        else
        {
            fMaxFrequency = data.GetSpectrum(0)->GetBinCenter(fMaxBin);
        }
        KTDEBUG(gvlog, "min frequency: " << fMinFrequency << "; max frequency: " << fMaxFrequency << "; min bin: " << fMinBin << "; max bin " << fMaxBin << "; input range max " << data.GetSpectrum(0)->GetRangeMin() << "; input range min: " << data.GetSpectrum(0)->GetRangeMax());

        unsigned nTotalBins = fMaxBin - fMinBin + 1;
        unsigned nBinsPerFitPoint = nTotalBins / fNFitPoints; // integer division rounds down; there may be bins leftover unused

        KTDEBUG(gvlog, "Performing gain variation fits with " << fNFitPoints << " points, and " << nBinsPerFitPoint << " bins averaged per fit point.");

        unsigned nComponents = data.GetNComponents();

        //double sigmaNorm = 1. / double(nBinsPerFitPoint - 1);
        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTPowerSpectrum* spectrum = data.GetSpectrum(iComponent);

            double* xVals = new double[fNFitPoints];
            double* yVals = new double[fNFitPoints];

            // Calculate fit points
#pragma omp parallel for default(shared)
            for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
            {
                unsigned fitPointStartBin = iFitPoint * nBinsPerFitPoint + fMinBin;
                unsigned fitPointEndBin = fitPointStartBin + nBinsPerFitPoint;

                double leftEdge = spectrum->GetBinLowEdge(fitPointStartBin);
                double rightEdge = spectrum->GetBinLowEdge(fitPointEndBin);
                xVals[iFitPoint] = leftEdge + 0.5 * (rightEdge - leftEdge);

                double mean = 0.;
                for (unsigned iBin=fitPointStartBin; iBin<fitPointEndBin; iBin++)
                {
                    mean += (*spectrum)(iBin);
                }
                mean /= (double)nBinsPerFitPoint;
                yVals[iFitPoint] = mean;

                KTDEBUG(gvlog, "Fit point " << iFitPoint << "  " << xVals[iFitPoint] << "  " << yVals[iFitPoint]);
            }

            if (fNormalize)
            {
                // Normalize the fit points to 1
                double minYVal = yVals[0];
                for (unsigned iFitPoint=1; iFitPoint < fNFitPoints; iFitPoint++)
                {
                    if (yVals[iFitPoint] < minYVal) minYVal = yVals[iFitPoint];
                }
                for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; iFitPoint++)
                {
                    yVals[iFitPoint] = yVals[iFitPoint] / minYVal;
                }
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
    KTGainVariationProcessor::GainVariation* KTGainVariationProcessor::CreateGainVariation(TSpline* spline, unsigned nBins, double rangeMin, double rangeMax) const
    {
        GainVariation* newGainVar = new GainVariation(nBins, rangeMin, rangeMax);

        // The fit region: [fMinBin, fMaxBin)
        // Keep track of the minimum value so we can shift it down to 1
        double minVal = spline->Eval(newGainVar->GetBinCenter(fMinBin));
        for (unsigned iBin = fMinBin; iBin < fMaxBin; iBin++)
        {
            (*newGainVar)(iBin) = spline->Eval(newGainVar->GetBinCenter(iBin));
            if ((*newGainVar)(iBin) < minVal)
            {
                minVal = (*newGainVar)(iBin);
            }
        }
        for (unsigned iBin = fMinBin; iBin < fMaxBin; iBin++)
        {
            (*newGainVar)(iBin) = (*newGainVar)(iBin) / minVal;
        }

        // Before the fit region: [0, fMinBin)
        for (unsigned iBin = 0; iBin < fMinBin; iBin++)
        {
            (*newGainVar)(iBin) = 1.;
        }

        // After the fit region: [fMaxBin, nBins)
        for (unsigned iBin = fMaxBin; iBin < nBins; iBin++)
        {
            (*newGainVar)(iBin) = 1.;
        }

        return newGainVar;
    }
    */

} /* namespace Katydid */
