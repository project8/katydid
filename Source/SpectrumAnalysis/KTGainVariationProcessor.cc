/*
 * KTGainVariationProcessor.cc
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#include "KTGainVariationProcessor.hh"

#include "KTConvolvedSpectrumData.hh"
#include "KTCorrelationData.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTPowerSpectrumData.hh"
#include "KTSpline.hh"
#include "KTStdComplexFuncs.hh"

#include <cmath>
#include <complex>
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
            fNormalize(false),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fNFitPoints(5),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fVarianceCalcNBins(100),
            fGainVarSignal("gain-var", this),
            //fFSPolarSlot("fs-polar", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            //fFSFFTWSlot("fs-fftw", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            //fCorrSlot("corr", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            //fPSSlot("ps", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            //fConvFSPolarSlot("conv-fs-polar", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            //fConvFSFFTWSlot("conv-fs-fftw", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            //fConvPSSlot("conv-ps", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            fFSPolarWithVarSlot("fs-polar-var", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            fFSFFTWWithVarSlot("fs-fftw-var", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            fPSWithVarSlot("ps-var", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            fConvFSPolarWithVarSlot("conv-fs-polar-var", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            fConvFSFFTWWithVarSlot("conv-fs-fftw-var", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal),
            fConvPSWithVarSlot("conv-ps-var", this, &KTGainVariationProcessor::CalculateGainVariation, &fGainVarSignal)
    {
    }

    KTGainVariationProcessor::~KTGainVariationProcessor()
    {
    }

    bool KTGainVariationProcessor::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetNormalize(node->get_value< bool >("normalize", fNormalize));

        // The if(has) pattern is used here so that Set[whatever] is only called if the particular parameter is present.
        // These Set[whatever] functions also set the flags to calculate the min/max bin, so we only want to call them if we are setting the value, and not just keeping the existing value.
        if (node->has("min-frequency"))
        {
            SetMinFrequency(node->get_value< double >("min-frequency"));
        }
        if (node->has("max-frequency"))
        {
            SetMaxFrequency(node->get_value< double >("max-frequency"));
        }

        // The if(has) pattern is used here so that Set[whatever] is only called if the particular parameter is present.
        // These Set[whatever] functions also set the flags to calculate the min/max bin, so we only want to call them if we are setting the value, and not just keeping the existing value.
        if (node->has("min-bin"))
        {
            SetMinBin(node->get_value< unsigned >("min-bin"));
        }
        if (node->has("max-bin"))
        {
            SetMaxBin(node->get_value< unsigned >("max-bin"));
        }

        SetNFitPoints(node->get_value< unsigned >("fit-points", fNFitPoints));
        SetVarianceCalcNBins(node->get_value< unsigned >("variance-n-bins", fVarianceCalcNBins));

        return true;
    }

    // Commented-out functions have been removed because the variance is not calculated correctly here
    // See GitHub issue #159
/*
    bool KTGainVariationProcessor::CalculateGainVariation(KTFrequencySpectrumDataPolar& data)
    {
        KTFrequencySpectrumVarianceDataPolar newVarData;
        newVarData.SetNComponents(data.GetNComponents());
        if (! CoreVarianceCalc(data, newVarData))
        {
            KTERROR(gvlog, "Something went wrong in calculating the variance for polar frequency spectrum!");
            return false;
        }
        
        return CalculateGainVariation(data, newVarData);
    }
*/
    bool KTGainVariationProcessor::CalculateGainVariation(KTFrequencySpectrumDataPolar& data, KTFrequencySpectrumVarianceDataPolar& varData)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());

        if( ! CoreGainVarCalc( data, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for polar frequency spectrum!" );
            return false;
        }

        if( ! CoreGainVarCalc( varData, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for polar frequency spectrum variance!" );
            return false;
        }

        return true;
    }
/*
    bool KTGainVariationProcessor::CalculateGainVariation(KTFrequencySpectrumDataFFTW& data)
    {
        KTFrequencySpectrumVarianceDataFFTW newVarData;
        newVarData.SetNComponents(data.GetNComponents());
        if (! CoreVarianceCalc(data, newVarData))
        {
            KTERROR(gvlog, "Something went wrong in calculating the variance for FFTW frequency spectrum!");
            return false;
        }

        return CalculateGainVariation(data, newVarData);
    }
*/
    bool KTGainVariationProcessor::CalculateGainVariation(KTFrequencySpectrumDataFFTW& data, KTFrequencySpectrumVarianceDataFFTW& varData)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());

        if( ! CoreGainVarCalc( data, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for FFTW frequency spectrum!" );
            return false;
        }

        if( ! CoreGainVarCalc( varData, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for FFTW frequency spectrum variance!" );
            return false;
        }

        return true;
    }
/*
    bool KTGainVariationProcessor::CalculateGainVariation(KTCorrelationData& data)
    {
        KTFrequencySpectrumVarianceDataPolar newVarData;
        newVarData.SetNComponents(data.GetNComponents());
        if (! CoreVarianceCalc(data, newVarData))
        {
            KTERROR(gvlog, "Something went wrong in calculating the variance for correlation spectrum!");
            return false;
        }

        return CalculateGainVariation(data, newVarData);
    }
*/
    bool KTGainVariationProcessor::CalculateGainVariation(KTCorrelationData& data, KTFrequencySpectrumVarianceDataPolar& varData)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());

        if( ! CoreGainVarCalc( data, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for correlation spectrum!" );
            return false;
        }

        if( ! CoreGainVarCalc( varData, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for correlation spectrum variance!" );
            return false;
        }

        return true;
    }
/*
    bool KTGainVariationProcessor::CalculateGainVariation(KTPowerSpectrumData& data)
    {
        KTPowerSpectrumVarianceData newVarData;
        newVarData.SetNComponents(data.GetNComponents());
        if (! CoreVarianceCalc(data, newVarData))
        {
            KTERROR(gvlog, "Something went wrong in calculating the variance for power spectrum!");
            return false;
        }

        return CalculateGainVariation(data, newVarData);
    }
*/
    bool KTGainVariationProcessor::CalculateGainVariation(KTPowerSpectrumData& data, KTPowerSpectrumVarianceData& varData)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());

        if( ! CoreGainVarCalc( data, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for power spectrum!" );
            return false;
        }

        if( ! CoreGainVarCalc( varData, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for power spectrum variance!" );
            return false;
        }

        return true;
    }
/*
    bool KTGainVariationProcessor::CalculateGainVariation(KTConvolvedFrequencySpectrumDataPolar& data)
    {
        KTConvolvedFrequencySpectrumVarianceDataPolar newVarData;
        newVarData.SetNComponents(data.GetNComponents());
        if (! CoreVarianceCalc(data, newVarData))
        {
            KTERROR(gvlog, "Something went wrong in calculating the variance for convolved polar frequency spectrum!");
            return false;
        }

        return CalculateGainVariation(data, newVarData);
    }
*/
    bool KTGainVariationProcessor::CalculateGainVariation(KTConvolvedFrequencySpectrumDataPolar& data, KTConvolvedFrequencySpectrumVarianceDataPolar& varData)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());

        if( ! CoreGainVarCalc( data, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for convolved polar frequency spectrum!" );
            return false;
        }

        if( ! CoreGainVarCalc( varData, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for convolved polar frequency spectrum variance!" );
            return false;
        }

        return true;
    }
/*
    bool KTGainVariationProcessor::CalculateGainVariation(KTConvolvedFrequencySpectrumDataFFTW& data)
    {
        KTConvolvedFrequencySpectrumVarianceDataFFTW newVarData;
        newVarData.SetNComponents(data.GetNComponents());
        if (! CoreVarianceCalc(data, newVarData))
        {
            KTERROR(gvlog, "Something went wrong in calculating the variance for convolved FFTW frequency spectrum!");
            return false;
        }

        return CalculateGainVariation(data, newVarData);
    }
*/
    bool KTGainVariationProcessor::CalculateGainVariation(KTConvolvedFrequencySpectrumDataFFTW& data, KTConvolvedFrequencySpectrumVarianceDataFFTW& varData)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());

        if( ! CoreGainVarCalc( data, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for convolved FFTW frequency spectrum!" );
            return false;
        }

        if( ! CoreGainVarCalc( varData, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for convolved FFTW frequency spectrum variance!" );
            return false;
        }

        return true;
    }
/*
    bool KTGainVariationProcessor::CalculateGainVariation(KTConvolvedPowerSpectrumData& data)
    {
        KTConvolvedPowerSpectrumVarianceData newVarData;
        newVarData.SetNComponents(data.GetNComponents());
        if (! CoreVarianceCalc(data, newVarData))
        {
            KTERROR(gvlog, "Something went wrong in calculating the variance for convolved power spectrum!");
            return false;
        }

        return CalculateGainVariation(data, newVarData);
    }
*/
    bool KTGainVariationProcessor::CalculateGainVariation(KTConvolvedPowerSpectrumData& data, KTConvolvedPowerSpectrumVarianceData& varData)
    {
        KTGainVariationData& newData = data.Of< KTGainVariationData >().SetNComponents(data.GetNComponents());

        if( ! CoreGainVarCalc( data, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for convolved power spectrum!" );
            return false;
        }

        if( ! CoreGainVarCalc( varData, newData ) )
        {
            KTERROR( gvlog, "Something went wrong calculating gain variation for convolved power spectrum variance!" );
            return false;
        }

        return true;
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
            for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; ++iFitPoint)
            {
                unsigned fitPointStartBin = iFitPoint * nBinsPerFitPoint + fMinBin;
                unsigned fitPointEndBin = fitPointStartBin + nBinsPerFitPoint;

                double leftEdge = spectrum->GetBinLowEdge(fitPointStartBin);
                double rightEdge = spectrum->GetBinLowEdge(fitPointEndBin);
                xVals[iFitPoint] = leftEdge + 0.5 * (rightEdge - leftEdge);

                double mean = 0.;
                for (unsigned iBin=fitPointStartBin; iBin<fitPointEndBin; ++iBin)
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
                for (unsigned iFitPoint=1; iFitPoint < fNFitPoints; ++iFitPoint)
                {
                    if (yVals[iFitPoint] < minYVal) minYVal = yVals[iFitPoint];
                }
                for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; ++iFitPoint)
                {
                    yVals[iFitPoint] = yVals[iFitPoint] / minYVal;
                }
            }

            KTSpline* spline = new KTSpline(xVals, yVals, fNFitPoints);
            spline->SetXMin(fMinFrequency);
            spline->SetXMax(fMaxFrequency);

            delete [] xVals;
            delete [] yVals;

            newData.SetSpline(spline, iComponent);
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
            for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; ++iFitPoint)
            {
                unsigned fitPointStartBin = iFitPoint * nBinsPerFitPoint + fMinBin;
                unsigned fitPointEndBin = fitPointStartBin + nBinsPerFitPoint;

                double leftEdge = spectrum->GetBinLowEdge(fitPointStartBin);
                double rightEdge = spectrum->GetBinLowEdge(fitPointEndBin);
                xVals[iFitPoint] = leftEdge + 0.5 * (rightEdge - leftEdge);

                double mean = 0.;
                for (unsigned iBin=fitPointStartBin; iBin<fitPointEndBin; ++iBin)
                {
                    mean += spectrum->GetAbs(iBin);
                }
                mean /= (double)nBinsPerFitPoint;
                yVals[iFitPoint] = mean;

                KTDEBUG(gvlog, "Fit point " << iFitPoint << "  " << xVals[iFitPoint] << "  " << yVals[iFitPoint]);
            }

            // Normalize the fit points to 1
            if (fNormalize)
            {
                double minYVal = yVals[0];
                for (unsigned iFitPoint=1; iFitPoint < fNFitPoints; ++iFitPoint)
                {
                    if (yVals[iFitPoint] < minYVal) minYVal = yVals[iFitPoint];
                }
                for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; ++iFitPoint)
                {
                    yVals[iFitPoint] = yVals[iFitPoint] / minYVal;
                }
            }

            KTSpline* spline = new KTSpline(xVals, yVals, fNFitPoints);
            spline->SetXMin(fMinFrequency);
            spline->SetXMax(fMaxFrequency);

            delete [] xVals;
            delete [] yVals;

            newData.SetSpline(spline, iComponent);
        }
        KTINFO(gvlog, "Completed gain variation calculation for " << nComponents);

        return true;
    }

    bool KTGainVariationProcessor::CoreGainVarCalc(KTPowerSpectrumDataCore& data, KTGainVariationData& newData)
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
            for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; ++iFitPoint)
            {
                unsigned fitPointStartBin = iFitPoint * nBinsPerFitPoint + fMinBin;
                unsigned fitPointEndBin = fitPointStartBin + nBinsPerFitPoint;

                double leftEdge = spectrum->GetBinLowEdge(fitPointStartBin);
                double rightEdge = spectrum->GetBinLowEdge(fitPointEndBin);
                xVals[iFitPoint] = leftEdge + 0.5 * (rightEdge - leftEdge);

                double mean = 0.;
                for (unsigned iBin=fitPointStartBin; iBin<fitPointEndBin; ++iBin)
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
                for (unsigned iFitPoint=1; iFitPoint < fNFitPoints; ++iFitPoint)
                {
                    if (yVals[iFitPoint] < minYVal) minYVal = yVals[iFitPoint];
                }
                for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; ++iFitPoint)
                {
                    yVals[iFitPoint] = yVals[iFitPoint] / minYVal;
                }
            }

            KTSpline* spline = new KTSpline(xVals, yVals, fNFitPoints);
            spline->SetXMin(fMinFrequency);
            spline->SetXMax(fMaxFrequency);

            delete [] xVals;
            delete [] yVals;

            newData.SetSpline(spline, iComponent);
        }
        KTINFO(gvlog, "Completed gain variation calculation for " << nComponents);

        return true;
    }

    bool KTGainVariationProcessor::CoreGainVarCalc(KTFrequencySpectrumVarianceDataCore& data, KTGainVariationData& newData)
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
            const KTFrequencySpectrumVariance* spectrum = data.GetSpectrum(iComponent);

            double* xVals = new double[fNFitPoints];
            double* yVals = new double[fNFitPoints];

            // Calculate fit points
#pragma omp parallel for default(shared)
            for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; ++iFitPoint)
            {
                unsigned fitPointStartBin = iFitPoint * nBinsPerFitPoint + fMinBin;
                unsigned fitPointEndBin = fitPointStartBin + nBinsPerFitPoint;

                double leftEdge = spectrum->GetBinLowEdge(fitPointStartBin);
                double rightEdge = spectrum->GetBinLowEdge(fitPointEndBin);
                xVals[iFitPoint] = leftEdge + 0.5 * (rightEdge - leftEdge);

                double mean = 0.;
                for (unsigned iBin=fitPointStartBin; iBin<fitPointEndBin; ++iBin)
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
                for (unsigned iFitPoint=1; iFitPoint < fNFitPoints; ++iFitPoint)
                {
                    if (yVals[iFitPoint] < minYVal) minYVal = yVals[iFitPoint];
                }
                for (unsigned iFitPoint=0; iFitPoint < fNFitPoints; ++iFitPoint)
                {
                    yVals[iFitPoint] = yVals[iFitPoint] / minYVal;
                }
            }

            KTSpline* spline = new KTSpline(xVals, yVals, fNFitPoints);
            spline->SetXMin(fMinFrequency);
            spline->SetXMax(fMaxFrequency);

            delete [] xVals;
            delete [] yVals;

            newData.SetVarianceSpline(spline, iComponent);
        }
        KTINFO(gvlog, "Completed gain variation calculation for " << nComponents);

        return true;
    }

    // These functions have been removed because the variance is not calculated correctly here
    // See GitHub issue #159
/*
    bool KTGainVariationProcessor::CoreVarianceCalc(KTFrequencySpectrumDataPolarCore& data, KTFrequencySpectrumVarianceDataCore& newVarData)
    {
        KTDEBUG(gvlog, "Doing local variance calculation");

        unsigned nComponents = data.GetNComponents();

        unsigned nBins = data.GetSpectrumPolar(0)->GetNBins();
        unsigned varCalcNBins = fVarianceCalcNBins;
        if (fVarianceCalcNBins > nBins) varCalcNBins = nBins;
        unsigned varCalcNBinsToLeft = (varCalcNBins - 1) / 2; // this and the next line are for handling odd and even values of varCalcNBins correctly
        unsigned varCalcNBinsToRight = varCalcNBins - 1 - varCalcNBinsToLeft;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTFrequencySpectrumPolar* spectrum = data.GetSpectrumPolar(iComponent);
            KTFrequencySpectrumVariance* varSpect = new KTFrequencySpectrumVariance(spectrum->GetNBins(), spectrum->GetRangeMin(), spectrum->GetRangeMax());

            std::list< std::complex<double> > binValuesInUse;
            std::complex<double> runningSum;
            unsigned nBinsInUse = varCalcNBinsToRight + 1;
            double invNBinsInUse = 1. / (double)nBinsInUse;

            // for the first bin, we'll only have the bin in question and the bins to the right
            // <= used to include both the bin in question plus varCalcNBinsToRight bins following that
            for (unsigned iBin = 0; iBin <= varCalcNBinsToRight; ++iBin)
            {
                binValuesInUse.emplace_back(real((*spectrum)(iBin)), imag((*spectrum)(iBin)));
                runningSum += (*spectrum)(iBin);
            }
            (*varSpect)(0) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);

            std::complex<double> newValue;

            // now calculate the variance for all bins up until and including when we have varCalcNBins available
            for (unsigned iBin = 1; iBin <= varCalcNBinsToLeft; ++iBin)
            {
                Assign(newValue, (*spectrum)(iBin + varCalcNBinsToRight));
                binValuesInUse.push_back(newValue);
                runningSum += newValue;
                ++nBinsInUse;
                invNBinsInUse = 1. / (double)nBinsInUse;

                (*varSpect)(iBin) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);
            }

            // at this point binValuesInUse contains varCalcNBins values, and we're ready to calculate the sliding variance
            for (unsigned iBin = varCalcNBinsToLeft + 1; iBin <= nBins - varCalcNBinsToRight - 1; ++iBin)
            {
                Assign(newValue, (*spectrum)(iBin + varCalcNBinsToRight));
                binValuesInUse.push_back(newValue);
                runningSum += newValue - binValuesInUse.front();
                binValuesInUse.pop_front();

                (*varSpect)(iBin) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);
            }

            // now we reduce back down until iBin reaches the end
            for (unsigned iBin = nBins - varCalcNBinsToRight; iBin < nBins; ++iBin)
            {
                runningSum -= binValuesInUse.front();
                binValuesInUse.pop_front();
                --nBinsInUse;
                invNBinsInUse = 1. / (double)nBinsInUse;

                (*varSpect)(iBin) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);
            }

            newVarData.SetSpectrum(varSpect, iComponent);
        }

        return true;
    }

    bool KTGainVariationProcessor::CoreVarianceCalc(KTFrequencySpectrumDataFFTWCore& data, KTFrequencySpectrumVarianceDataCore& newVarData)
    {
        KTDEBUG(gvlog, "Doing local variance calculation");

        unsigned nComponents = data.GetNComponents();

        unsigned nBins = data.GetSpectrumFFTW(0)->GetNBins();
        unsigned varCalcNBins = fVarianceCalcNBins;
        if (fVarianceCalcNBins > nBins) varCalcNBins = nBins;
        unsigned varCalcNBinsToLeft = (varCalcNBins - 1) / 2; // this and the next line are for handling odd and even values of varCalcNBins correctly
        unsigned varCalcNBinsToRight = varCalcNBins - 1 - varCalcNBinsToLeft;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTFrequencySpectrumFFTW* spectrum = data.GetSpectrumFFTW(iComponent);
            KTFrequencySpectrumVariance* varSpect = new KTFrequencySpectrumVariance(spectrum->GetNBins(), spectrum->GetRangeMin(), spectrum->GetRangeMax());

            std::list< std::complex<double> > binValuesInUse;
            std::complex<double> runningSum;
            unsigned nBinsInUse = varCalcNBinsToRight + 1;
            double invNBinsInUse = 1. / (double)nBinsInUse;

            // for the first bin, we'll only have the bin in question and the bins to the right
            // <= used to include both the bin in question plus varCalcNBinsToRight bins following that
            for (unsigned iBin = 0; iBin <= varCalcNBinsToRight; ++iBin)
            {
                binValuesInUse.emplace_back((*spectrum)(iBin)[0], (*spectrum)(iBin)[1]);
                runningSum += (*spectrum)(iBin);
            }
            (*varSpect)(0) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);

            std::complex<double> newValue;

            // now calculate the variance for all bins up until and including when we have varCalcNBins available
            for (unsigned iBin = 1; iBin <= varCalcNBinsToLeft; ++iBin)
            {
                Assign(newValue, (*spectrum)(iBin + varCalcNBinsToRight));
                binValuesInUse.push_back(newValue);
                runningSum += newValue;
                ++nBinsInUse;
                invNBinsInUse = 1. / (double)nBinsInUse;

                (*varSpect)(iBin) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);
            }

            // at this point binValuesInUse contains varCalcNBins values, and we're ready to calculate the sliding variance
            for (unsigned iBin = varCalcNBinsToLeft + 1; iBin <= nBins - varCalcNBinsToRight - 1; ++iBin)
            {
                Assign(newValue, (*spectrum)(iBin + varCalcNBinsToRight));
                binValuesInUse.push_back(newValue);
                runningSum += newValue - binValuesInUse.front();
                binValuesInUse.pop_front();

                (*varSpect)(iBin) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);
            }

            // now we reduce back down until iBin reaches the end
            for (unsigned iBin = nBins - varCalcNBinsToRight; iBin < nBins; ++iBin)
            {
                runningSum -= binValuesInUse.front();
                binValuesInUse.pop_front();
                --nBinsInUse;
                invNBinsInUse = 1. / (double)nBinsInUse;

                (*varSpect)(iBin) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);
            }

            newVarData.SetSpectrum(varSpect, iComponent);
        }

        return true;
    }

    bool KTGainVariationProcessor::CoreVarianceCalc(KTPowerSpectrumDataCore& data, KTFrequencySpectrumVarianceDataCore& newVarData)
    {
        KTDEBUG(gvlog, "Doing local variance calculation");

        unsigned nComponents = data.GetNComponents();

        unsigned nBins = data.GetSpectrum(0)->GetNBins();
        unsigned varCalcNBins = fVarianceCalcNBins;
        if (fVarianceCalcNBins > nBins) varCalcNBins = nBins;
        unsigned varCalcNBinsToLeft = (varCalcNBins - 1) / 2; // this and the next line are for handling odd and even values of varCalcNBins correctly
        unsigned varCalcNBinsToRight = varCalcNBins - 1 - varCalcNBinsToLeft;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTPowerSpectrum* spectrum = data.GetSpectrum(iComponent);
            KTFrequencySpectrumVariance* varSpect = new KTFrequencySpectrumVariance(spectrum->GetNBins(), spectrum->GetRangeMin(), spectrum->GetRangeMax());

            std::list< double > binValuesInUse;
            double runningSum = 0.;
            unsigned nBinsInUse = varCalcNBinsToRight + 1;
            double invNBinsInUse = 1. / (double)nBinsInUse;

            // for the first bin, we'll only have the bin in question and the bins to the right
            // <= used to include both the bin in question plus varCalcNBinsToRight bins following that
            for (unsigned iBin = 0; iBin <= varCalcNBinsToRight; ++iBin)
            {
                binValuesInUse.push_back((*spectrum)(iBin));
                runningSum += (*spectrum)(iBin);
            }
            (*varSpect)(0) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);

            double newValue = 0.;

            // now calculate the variance for all bins up until and including when we have varCalcNBins available
            for (unsigned iBin = 1; iBin <= varCalcNBinsToLeft; ++iBin)
            {
                newValue = (*spectrum)(iBin + varCalcNBinsToRight);
                binValuesInUse.push_back(newValue);
                runningSum += newValue;
                ++nBinsInUse;
                invNBinsInUse = 1. / (double)nBinsInUse;

                (*varSpect)(iBin) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);
            }

            // at this point binValuesInUse contains varCalcNBins values, and we're ready to calculate the sliding variance
            for (unsigned iBin = varCalcNBinsToLeft + 1; iBin <= nBins - varCalcNBinsToRight - 1; ++iBin)
            {
                newValue = (*spectrum)(iBin + varCalcNBinsToRight);
                binValuesInUse.push_back(newValue);
                runningSum += newValue - binValuesInUse.front();
                binValuesInUse.pop_front();

                (*varSpect)(iBin) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);
            }

            // now we reduce back down until iBin reaches the end
            for (unsigned iBin = nBins - varCalcNBinsToRight; iBin < nBins; ++iBin)
            {
                runningSum -= binValuesInUse.front();
                binValuesInUse.pop_front();
                --nBinsInUse;
                invNBinsInUse = 1. / (double)nBinsInUse;

                (*varSpect)(iBin) = CalculateVariance(binValuesInUse, runningSum, invNBinsInUse);
            }

            newVarData.SetSpectrum(varSpect, iComponent);
        }

        return true;
    }

    double KTGainVariationProcessor::CalculateVariance(const std::list< double >& binValuesInUse, double runningSum, double invNBinsInUse)
    {
        // Var[x] = E[(x - E[x])^2]
        double mean = runningSum * invNBinsInUse;
        double variance = 0.;
        double diff = 0.;

        for (auto&& value : binValuesInUse)
        {
            diff = value - mean;
            variance += diff * diff;
        }

        return variance * invNBinsInUse;
    }

    double KTGainVariationProcessor::CalculateVariance(const std::list< std::complex<double> >& binValuesInUse, std::complex<double> runningSum, double invNBinsInUse)
    {
        // E[Z] = E[R{Z}] + iE[I{Z}]
        // Var[Z] = E[|Z|^2] - |E[Z]|^2
        double meanSq = std::norm(runningSum) * invNBinsInUse * invNBinsInUse;
        double variance = 0.;
        double diff = 0.;

        for (auto&& value : binValuesInUse)
        {
            variance += std::norm(value);
        }

        return variance * invNBinsInUse - meanSq;
    }
*/

} /* namespace Katydid */
