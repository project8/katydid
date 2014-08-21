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
#include "KTParam.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
//#include "KTSlidingWindowFSData.hh"
//#include "KTSlidingWindowFSDataFFTW.hh"

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
            fPreCalcSlot("pre-calc", this, &KTGainNormalization::SetPreCalcGainVar),
            fPSPreCalcSlot("ps-pre", this, &KTGainNormalization::Normalize, &fPSSignal)
    {
    }

    KTGainNormalization::~KTGainNormalization()
    {
    }

    bool KTGainNormalization::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        if (node->Has("min-frequency"))
        {
            SetMinFrequency(node->GetValue< double >("min-frequency"));
        }
        if (node->Has("max-frequency"))
        {
            SetMaxFrequency(node->GetValue< double >("max-frequency"));
        }

        if (node->Has("min-bin"))
        {
            SetMinBin(node->GetValue< unsigned >("min-bin"));
        }
        if (node->Has("max-bin"))
        {
            SetMaxBin(node->GetValue< unsigned >("max-bin"));
        }

        return true;
    }


    bool KTGainNormalization::SetPreCalcGainVar(KTGainVariationData& gvData)
    {
        fGVData = gvData;
        return true;
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
            KTFrequencySpectrumPolar* newSpectrum = Normalize(fsData.GetSpectrumPolar(iComponent), gvData.GetSpline(iComponent));
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
            KTFrequencySpectrumFFTW* newSpectrum = Normalize(fsData.GetSpectrumFFTW(iComponent), gvData.GetSpline(iComponent));
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
            KTPowerSpectrum* newSpectrum = Normalize(psData.GetSpectrum(iComponent), gvData.GetSpline(iComponent));
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

    KTFrequencySpectrumPolar* KTGainNormalization::Normalize(const KTFrequencySpectrumPolar* frequencySpectrum, const KTSpline* spline)
    {
        unsigned nBins = fMaxBin - fMinBin + 1;
        double freqMin = frequencySpectrum->GetBinLowEdge(fMinBin);
        double freqMax = frequencySpectrum->GetBinLowEdge(fMaxBin) + frequencySpectrum->GetBinWidth();

        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        unsigned nSpectrumBins = frequencySpectrum->size();
        double freqSpectrumMin = frequencySpectrum->GetRangeMin();
        double freqSpectrumMax = frequencySpectrum->GetRangeMax();

        KTDEBUG(gnlog, "Creating new FS for normalized data: " << nSpectrumBins << ", " << freqSpectrumMin << ", " << freqSpectrumMax);
        KTFrequencySpectrumPolar* newSpectrum = new KTFrequencySpectrumPolar(nSpectrumBins, freqSpectrumMin, freqSpectrumMax);
        newSpectrum->SetNTimeBins(frequencySpectrum->GetNTimeBins());

        // First directly copy data that's outside the scaling range
        unsigned iBin;
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
                (*newSpectrum)(iBin).set_polar((*frequencySpectrum)(iBin).abs() / (*splineImp)(iBin - fMinBin), (*frequencySpectrum)(iBin).arg());
            }
        }

        spline->AddToCache(splineImp);

        return newSpectrum;
    }

    KTFrequencySpectrumFFTW* KTGainNormalization::Normalize(const KTFrequencySpectrumFFTW* frequencySpectrum, const KTSpline* spline)
    {
        // PLEASE NOTE: There is on situation in which this normalization function will not operate properly: if the array size
        //              is even, and scaling is requested all the way up to the Nyquist bin, the Nyquist bin will not be scaled.

        unsigned nBins = fMaxBin - fMinBin + 1;
        double freqMin = frequencySpectrum->GetBinLowEdge(fMinBin);
        double freqMax = frequencySpectrum->GetBinLowEdge(fMaxBin) + frequencySpectrum->GetBinWidth();

        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        unsigned nSpectrumBins = frequencySpectrum->size();
        double freqSpectrumMin = frequencySpectrum->GetRangeMin();
        double freqSpectrumMax = frequencySpectrum->GetRangeMax();

        KTDEBUG(gnlog, "Creating new FS for normalized data: " << nSpectrumBins << ", " << freqSpectrumMin << ", " << freqSpectrumMax);
        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(nSpectrumBins, freqSpectrumMin, freqSpectrumMax);
        newSpectrum->SetNTimeBins(frequencySpectrum->GetNTimeBins());

        //KTDEBUG(gnlog, "array range: 0 - " << frequencySpectrum->size());
        //KTDEBUG(gnlog, "new array range: 0 - " << newSpectrum->size());

        // First directly copy data that's outside the scaling range
        // DC bin
        unsigned dcBin = frequencySpectrum->GetDCBin();
        (*newSpectrum)(dcBin)[0] = (*frequencySpectrum)(dcBin)[0];
        (*newSpectrum)(dcBin)[1] = (*frequencySpectrum)(dcBin)[1];
        //KTDEBUG(gnlog, "bin = " << dcBin);

        // Nyquist bin if the array size is even
        unsigned lastBinNeg = 0;
        if (frequencySpectrum->GetIsSizeEven())
        {
            (*newSpectrum)(0)[0] = (*frequencySpectrum)(0)[0];
            (*newSpectrum)(0)[1] = (*frequencySpectrum)(0)[1];
            lastBinNeg = 1;
            //KTDEBUG(gnlog, "bin = 0");
        }

        unsigned minOffsetBin = fMinBin - dcBin;
        unsigned maxOffsetBin = fMaxBin - dcBin;
        unsigned spectrumSizeOffset = frequencySpectrum->size() - dcBin;

        double scaling = 1.;
        unsigned iBinPos, iBinNeg, iBin, iOffsetBin;
#pragma omp parallel default(shared) private(iBinPos, iBinNeg, iBin, iOffsetBin, scaling)
        {
            // All of the other bins outside the scaling range, both positive and negative frequencies
            //for (unsigned iBinPos=dcBin + 1, iBinNeg=dcBin - 1; iBinPos < fMinBin; iBinPos++, iBinNeg--)
//#pragma omp master
            //KTDEBUG(gnlog, "loop: 1 - " << minOffsetBin-1);
#pragma omp for
            for (iOffsetBin=1; iOffsetBin < minOffsetBin; ++iOffsetBin)
            {
                iBinPos = dcBin + iOffsetBin;
                iBinNeg = dcBin - iOffsetBin;
                //KTDEBUG(gnlog, "    offset = " << iOffsetBin << "    binpos = " << iBinPos << "    binneg = " << iBinNeg);
                (*newSpectrum)(iBinPos)[0] = (*frequencySpectrum)(iBinPos)[0];
                (*newSpectrum)(iBinPos)[1] = (*frequencySpectrum)(iBinPos)[1];
                (*newSpectrum)(iBinNeg)[0] = (*frequencySpectrum)(iBinNeg)[0];
                (*newSpectrum)(iBinNeg)[1] = (*frequencySpectrum)(iBinNeg)[1];
            }


            // Then scale the bins within the scaling range
            //for (unsigned iBinPos=fMinBin, iBinNeg=dcBin - (fMinBin-dcBin), iBin=0; iBinPos < fMaxBin+1; iBinPos++, iBinNeg--, ++iBin)
//#pragma omp master
            //KTDEBUG(gnlog, "loop: " << minOffsetBin << " - " << maxOffsetBin);
#pragma omp for
            for (iOffsetBin=minOffsetBin; iOffsetBin <= maxOffsetBin; ++iOffsetBin)
            {
                iBin = iOffsetBin - minOffsetBin;
                iBinPos = dcBin + iOffsetBin;
                iBinNeg = dcBin - iOffsetBin;
                //KTDEBUG(gnlog, "    offset = " << iOffsetBin << "    binpos = " << iBinPos << "    binneg = " << iBinNeg << "    bin = " << iBin);
                scaling = 1. / (*splineImp)(iBin);
                (*newSpectrum)(iBinPos)[0] = (*frequencySpectrum)(iBinPos)[0] * scaling;
                (*newSpectrum)(iBinPos)[1] = (*frequencySpectrum)(iBinPos)[1] * scaling;
                (*newSpectrum)(iBinNeg)[0] = (*frequencySpectrum)(iBinNeg)[0] * scaling;
                (*newSpectrum)(iBinNeg)[1] = (*frequencySpectrum)(iBinNeg)[1] * scaling;
            }




            //for (unsigned iBinPos=fMaxBin+1, iBinNeg=dcBin - (fMaxBin+1-dcBin); iBinPos < nSpectrumBins; iBinPos++, iBinNeg--)
//#pragma omp master
            //KTDEBUG(gnlog, "loop: " << maxOffsetBin+1 << " - " << spectrumSizeOffset);
#pragma omp for
            for (iOffsetBin=maxOffsetBin + 1; iOffsetBin < spectrumSizeOffset; ++iOffsetBin)
            {
                iBinPos = dcBin + iOffsetBin;
                iBinNeg = dcBin - iOffsetBin;
                //KTDEBUG(gnlog, "    offset = " << iOffsetBin << "    binpos = " << iBinPos << "    binneg = " << iBinNeg);
                (*newSpectrum)(iBinPos)[0] = (*frequencySpectrum)(iBinPos)[0];
                (*newSpectrum)(iBinPos)[1] = (*frequencySpectrum)(iBinPos)[1];
                (*newSpectrum)(iBinNeg)[0] = (*frequencySpectrum)(iBinNeg)[0];
                (*newSpectrum)(iBinNeg)[1] = (*frequencySpectrum)(iBinNeg)[1];
            }

        } // end OpenMP parallel block

        spline->AddToCache(splineImp);

        return newSpectrum;
    }

    KTPowerSpectrum* KTGainNormalization::Normalize(const KTPowerSpectrum* powerSpectrum, const KTSpline* spline)
    {
        unsigned nBins = fMaxBin - fMinBin + 1;
        double freqMin = powerSpectrum->GetBinLowEdge(fMinBin);
        double freqMax = powerSpectrum->GetBinLowEdge(fMaxBin) + powerSpectrum->GetBinWidth();

        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        unsigned nSpectrumBins = powerSpectrum->size();
        double freqSpectrumMin = powerSpectrum->GetRangeMin();
        double freqSpectrumMax = powerSpectrum->GetRangeMax();

        KTDEBUG(gnlog, "Creating new PS for normalized data: " << nSpectrumBins << ", " << freqSpectrumMin << ", " << freqSpectrumMax);
        KTPowerSpectrum* newSpectrum = new KTPowerSpectrum(nSpectrumBins, freqSpectrumMin, freqSpectrumMax);
        newSpectrum->OverrideMode(KTPowerSpectrum::kPower);

        // First directly copy data that's outside the scaling range
        unsigned iBin;
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
                (*newSpectrum)(iBin) = (*powerSpectrum)(iBin) / (*splineImp)(iBin - fMinBin);
            }
        }

        spline->AddToCache(splineImp);

        return newSpectrum;
    }

} /* namespace Katydid */
