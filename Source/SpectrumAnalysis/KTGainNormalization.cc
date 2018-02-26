/*
 * KTGainNormalization.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTGainNormalization.hh"

#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTNormalizedFSData.hh"
#include "KTNormalizedFSData.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::string;


namespace Katydid
{
    KTLOGGER(gnlog, "KTGainNormalization");

    KT_REGISTER_PROCESSOR(KTGainNormalization, "gain-normalization");

    KTGainNormalization::KTGainNormalization(const std::string& name) :
            KTProcessor(name),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fFSPolarSignal("norm-fs-polar", this),
            fFSFFTWSignal("norm-fs-fftw", this),
            fPSSignal("norm-ps", this),
            fFSPolarSlot("fs-polar", this, &KTGainNormalization::Normalize, &fFSPolarSignal),
            fFSFFTWSlot("fs-fftw", this, &KTGainNormalization::Normalize, &fFSFFTWSignal),
            fPSSlot("ps", this, &KTGainNormalization::Normalize, &fPSSignal),
            fPreCalcSlot("gv", this, &KTGainNormalization::SetPreCalcGainVar),
            fPSPreCalcSlot("fs-polar-pre", this, &KTGainNormalization::Normalize, &fFSPolarSignal),
            fPSPreCalcSlot("fs-fftw-pre", this, &KTGainNormalization::Normalize, &fFSFFTWSignal),
            fPSPreCalcSlot("ps-pre", this, &KTGainNormalization::Normalize, &fPSSignal)
    {
    }

    KTGainNormalization::~KTGainNormalization()
    {
    }

    bool KTGainNormalization::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

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

        return true;
    }


    bool KTGainNormalization::SetPreCalcGainVar(KTGainVariationData& gvData)
    {
        fGVData = gvData;
        return true;
    }

    bool KTGainNormalization::Normalize(KTFrequencySpectrumDataPolar& fsData)
    {
        return Normalize(fsData, fGVData);
    }

    bool KTGainNormalization::Normalize(KTFrequencySpectrumDataFFTW& fsData)
    {
        return Normalize(fsData, fGVData);
    }

    bool KTGainNormalization::Normalize(KTPowerSpectrumData& psData)
    {
        return Normalize(psData, fGVData);
    }

    bool KTGainNormalization::Normalize(KTFrequencySpectrumDataPolar& fsData, KTGainVariationData& gvData)
    {
        if (fCalculateMinBin) SetMinBin(fsData.GetSpectrumPolar(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin(fsData.GetSpectrumPolar(0)->FindBin(fMaxFrequency));

        unsigned nComponents = fsData.GetNComponents();
        if (nComponents != gvData.GetNComponents())
        {
            KTERROR(gnlog, "Mismatch in the number of channels between the frequency spectrum data and the gain variation data! Aborting.");
            return false;
        }

        KTNormalizedFSDataPolar& newData = fsData.Of< KTNormalizedFSDataPolar >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* newSpectrum = Normalize(fsData.GetSpectrumPolar(iComponent), gvData.GetSpline(iComponent), gvData.GetVarianceSpline(iComponent));
            if (newSpectrum == NULL)
            {
                KTERROR(gnlog, "Normalization of spectrum " << iComponent << " failed for some reason. Continuing processing.");
                continue;
            }
            KTDEBUG(gnlog, "Computed normalization; size: " << newSpectrum->size() << "; range: " << newSpectrum->GetRangeMin() << " - " << newSpectrum->GetRangeMax());
            newData.SetSpectrum(newSpectrum, iComponent);
        }
        KTINFO(gnlog, "Completed gain normalization of " << nComponents << " frequency spectra (polar)");

        return true;
    }

    bool KTGainNormalization::Normalize(KTFrequencySpectrumDataFFTW& fsData, KTGainVariationData& gvData)
    {
        if (fCalculateMinBin) SetMinBin(fsData.GetSpectrumFFTW(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin(fsData.GetSpectrumFFTW(0)->FindBin(fMaxFrequency));

        unsigned nComponents = fsData.GetNComponents();
        if (nComponents != gvData.GetNComponents())
        {
            KTERROR(gnlog, "Mismatch in the number of channels between the frequency spectrum data and the gain variation data! Aborting.");
            return false;
        }

        KTNormalizedFSDataFFTW& newData = fsData.Of< KTNormalizedFSDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newSpectrum = Normalize(fsData.GetSpectrumFFTW(iComponent), gvData.GetSpline(iComponent), gvData.GetVarianceSpline(iComponent));
            if (newSpectrum == NULL)
            {
                KTERROR(gnlog, "Normalization of spectrum " << iComponent << " failed for some reason. Continuing processing.");
                continue;
            }
            KTDEBUG(gnlog, "Computed normalization; size: " << newSpectrum->size() << "; range: " << newSpectrum->GetRangeMin() << " - " << newSpectrum->GetRangeMax());
             newData.SetSpectrum(newSpectrum, iComponent);
        }
        KTINFO(gnlog, "Completed gain normalization of " << nComponents << " frequency spectra (fftw)");

        return true;
    }

    bool KTGainNormalization::Normalize(KTPowerSpectrumData& psData, KTGainVariationData& gvData)
    {
        if (fCalculateMinBin) SetMinBin(psData.GetSpectrum(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin(psData.GetSpectrum(0)->FindBin(fMaxFrequency));

        unsigned nComponents = psData.GetNComponents();
        if (nComponents != gvData.GetNComponents())
        {
            KTERROR(gnlog, "Mismatch in the number of channels between the frequency spectrum data and the gain variation data! Aborting.");
            return false;
        }

        KTNormalizedPSData& newData = psData.Of< KTNormalizedPSData >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTPowerSpectrum* newSpectrum = Normalize(psData.GetSpectrum(iComponent), gvData.GetSpline(iComponent), gvData.GetVarianceSpline(iComponent));
            if (newSpectrum == NULL)
            {
                KTERROR(gnlog, "Normalization of spectrum " << iComponent << " failed for some reason. Continuing processing.");
                continue;
            }
            KTDEBUG(gnlog, "Computed normalization; size: " << newSpectrum->size() << "; range: " << newSpectrum->GetRangeMin() << " - " << newSpectrum->GetRangeMax());
             newData.SetSpectrum(newSpectrum, iComponent);
        }
        KTINFO(gnlog, "Completed gain normalization of " << nComponents << " power spectra");

        return true;
    }

    KTFrequencySpectrumPolar* KTGainNormalization::Normalize(const KTFrequencySpectrumPolar* frequencySpectrum, const KTSpline* spline, const KTSpline* varSpline)
    {
        unsigned nBins = fMaxBin - fMinBin + 1;
        double freqMin = frequencySpectrum->GetBinLowEdge(fMinBin);
        double freqMax = frequencySpectrum->GetBinLowEdge(fMaxBin) + frequencySpectrum->GetBinWidth();

        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);
        KTSpline::Implementation* varSplineImp = varSpline->Implement(nBins, freqMin, freqMax);

        unsigned nSpectrumBins = frequencySpectrum->size();
        double freqSpectrumMin = frequencySpectrum->GetRangeMin();
        double freqSpectrumMax = frequencySpectrum->GetRangeMax();

        // Average of each spline
        double normalizedValue = 0., normalizedSigma = 0.;
        double splineImpValue = 0.;
        for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
        {
            splineImpValue = (*splineImp)(iBin - fMinBin);
            normalizedValue += splineImpValue;
            normalizedSigma += sqrt( (*varSplineImp)(iBin - fMinBin) - splineImpValue * splineImpValue );
        }
        normalizedValue /= (double)nBins;
        normalizedSigma /= (double)nBins;

        KTDEBUG(gnlog, "Creating new FS for normalized data: " << nSpectrumBins << ", " << freqSpectrumMin << ", " << freqSpectrumMax);
        KTFrequencySpectrumPolar* newSpectrum = new KTFrequencySpectrumPolar(nSpectrumBins, freqSpectrumMin, freqSpectrumMax);
        newSpectrum->SetNTimeBins(frequencySpectrum->GetNTimeBins());

        // First directly copy data that's outside the scaling range
        unsigned iBin;
        double mean = 0., sigma = 0., threshold = 0., value = 0.;
#pragma omp parallel default(shared)
        {
#pragma omp for private(iBin)
            for (iBin=0; iBin < fMinBin; ++iBin)
            {
                (*newSpectrum)(iBin).set_polar((*frequencySpectrum)(iBin).abs(), (*frequencySpectrum)(iBin).arg());
            }
#pragma omp for private(iBin)
            for (iBin=fMaxBin+1; iBin < nSpectrumBins; ++iBin)
            {
                (*newSpectrum)(iBin).set_polar((*frequencySpectrum)(iBin).abs(), (*frequencySpectrum)(iBin).arg());
            }

            // Then scale the bins within the scaling range
#pragma omp for private(iBin)
            for (iBin=fMinBin; iBin < fMaxBin+1; ++iBin)
            {
                mean = (*splineImp)(iBin - fMinBin);
                sigma = sqrt( (*varSplineImp)(iBin - fMinBin) - mean * mean );
                value = (*frequencySpectrum)(iBin).abs();

                (*newSpectrum)(iBin).set_polar(normalizedValue + (value - mean) / sigma * normalizedSigma, (*frequencySpectrum)(iBin).arg());
            }
        }

        spline->AddToCache(splineImp);

        return newSpectrum;
    }

    KTFrequencySpectrumFFTW* KTGainNormalization::Normalize(const KTFrequencySpectrumFFTW* frequencySpectrum, const KTSpline* spline, const KTSpline* varSpline)
    {
        unsigned nBins = fMaxBin - fMinBin + 1;
        double freqMin = frequencySpectrum->GetBinLowEdge(fMinBin);
        double freqMax = frequencySpectrum->GetBinLowEdge(fMaxBin) + frequencySpectrum->GetBinWidth();

        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);
        KTSpline::Implementation* varSplineImp = varSpline->Implement(nBins, freqMin, freqMax);

        unsigned nSpectrumBins = frequencySpectrum->size();
        double freqSpectrumMin = frequencySpectrum->GetRangeMin();
        double freqSpectrumMax = frequencySpectrum->GetRangeMax();

        // Average of each spline
        double normalizedValue = 0., normalizedSigma = 0.;
        double splineImpValue = 0.;
        for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
        {
            splineImpValue = (*splineImp)(iBin - fMinBin);
            normalizedValue += splineImpValue;
            normalizedSigma += sqrt( (*varSplineImp)(iBin - fMinBin) - splineImpValue * splineImpValue );
        }
        normalizedValue /= (double)nBins;
        normalizedSigma /= (double)nBins;

        KTDEBUG(gnlog, "Creating new FS for normalized data: " << nSpectrumBins << ", " << freqSpectrumMin << ", " << freqSpectrumMax);
        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(nSpectrumBins, freqSpectrumMin, freqSpectrumMax);
        newSpectrum->SetNTimeBins(frequencySpectrum->GetNTimeBins());

        //KTDEBUG(gnlog, "array range: 0 - " << frequencySpectrum->size());
        //KTDEBUG(gnlog, "new array range: 0 - " << newSpectrum->size());

        // First directly copy data that's outside the scaling range
        unsigned iBin;
        double mean = 0., sigma = 0., threshold = 0., value = 0.;
#pragma omp parallel default(shared)
        {
#pragma omp for private(iBin)
            for (iBin=0; iBin < fMinBin; ++iBin)
            {
                (*newSpectrum)(iBin)[0] = (*frequencySpectrum)(iBin)[0];
                (*newSpectrum)(iBin)[1] = (*frequencySpectrum)(iBin)[1];
            }
#pragma omp for private(iBin)
            for (iBin=fMaxBin+1; iBin < nSpectrumBins; ++iBin)
            {
                (*newSpectrum)(iBin)[0] = (*frequencySpectrum)(iBin)[0];
                (*newSpectrum)(iBin)[1] = (*frequencySpectrum)(iBin)[1];
            }

            // Then scale the bins within the scaling range
            double scaling;
#pragma omp for private(iBin, scaling)
            for (iBin=fMinBin; iBin < fMaxBin+1; ++iBin)
            {
                mean = (*splineImp)(iBin - fMinBin);
                sigma = sqrt( (*varSplineImp)(iBin - fMinBin) - mean * mean );
                value = sqrt( (*frequencySpectrum)(iBin)[0] * (*frequencySpectrum)(iBin)[0] + (*frequencySpectrum)(iBin)[1] * (*frequencySpectrum)(iBin)[1] );

                (*newSpectrum)(iBin)[0] = (normalizedValue + (value - mean) / sigma * normalizedSigma) * (*frequencySpectrum)(iBin)[0] / value;
                (*newSpectrum)(iBin)[1] = (normalizedValue + (value - mean) / sigma * normalizedSigma) * (*frequencySpectrum)(iBin)[1] / value;
            }
        }

        spline->AddToCache(splineImp);

        return newSpectrum;
    }

    KTPowerSpectrum* KTGainNormalization::Normalize(const KTPowerSpectrum* powerSpectrum, const KTSpline* spline, const KTSpline* varSpline)
    {
        unsigned nBins = fMaxBin - fMinBin + 1;
        double freqMin = powerSpectrum->GetBinLowEdge(fMinBin);
        double freqMax = powerSpectrum->GetBinLowEdge(fMaxBin) + powerSpectrum->GetBinWidth();

        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);
        KTSpline::Implementation* varSplineImp = varSpline->Implement(nBins, freqMin, freqMax);

        unsigned nSpectrumBins = powerSpectrum->size();
        double freqSpectrumMin = powerSpectrum->GetRangeMin();
        double freqSpectrumMax = powerSpectrum->GetRangeMax();

        // Average of each spline
        double normalizedValue = 0., normalizedSigma = 0.;
        double splineImpValue = 0.;
        for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
        {
            splineImpValue = (*splineImp)(iBin - fMinBin);
            normalizedValue += splineImpValue;
            normalizedSigma += sqrt( (*varSplineImp)(iBin - fMinBin) - splineImpValue * splineImpValue );
        }
        normalizedValue /= (double)nBins;
        normalizedSigma /= (double)nBins;

        KTDEBUG(gnlog, "Creating new PS for normalized data: " << nSpectrumBins << ", " << freqSpectrumMin << ", " << freqSpectrumMax);
        KTPowerSpectrum* newSpectrum = new KTPowerSpectrum(nSpectrumBins, freqSpectrumMin, freqSpectrumMax);
        newSpectrum->OverrideMode(KTPowerSpectrum::kPower);

        // First directly copy data that's outside the scaling range
        unsigned iBin;
        double mean = 0., sigma = 0., threshold = 0., value = 0.;
#pragma omp parallel default(shared)
        {
#pragma omp for private(iBin)
            for (iBin=0; iBin < fMinBin; ++iBin)
            {
                (*newSpectrum)(iBin) = (*powerSpectrum)(iBin);
            }
#pragma omp for private(iBin)
            for (iBin=fMaxBin+1; iBin < nSpectrumBins; ++iBin)
            {
                (*newSpectrum)(iBin) = (*powerSpectrum)(iBin);
            }

            // Then scale the bins within the scaling range
#pragma omp for private(iBin)
            for (iBin=fMinBin; iBin < fMaxBin+1; ++iBin)
            {
                mean = (*splineImp)(iBin - fMinBin);
                sigma = sqrt( (*varSplineImp)(iBin - fMinBin) - mean * mean );
                value = (*powerSpectrum)(iBin);

                (*newSpectrum)(iBin) = normalizedValue + (value - mean) / sigma * normalizedSigma;
            }
        }

        spline->AddToCache(splineImp);

        return newSpectrum;
    }

} /* namespace Katydid */
