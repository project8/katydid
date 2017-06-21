/*
 * KTDataTypeDisplayTransform.cc
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTSliceHeader.hh"
#include "KTMultiFSDataPolar.hh"
#include "KTMultiFSDataFFTW.hh"
#include "KTTimeFrequencyDataPolar.hh"
#include "KTTimeFrequencyPolar.hh"

#include "TH1.h"
#include "TH2.h"

#include <sstream>
#include "KTDataTypeDisplayTransform.hh"



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    static Nymph::KTTIRegistrar< KTDataTypeDisplay, KTDataTypeDisplayTransform > sBRTWFFTRegistrar;

    KTDataTypeDisplayTransform::KTDataTypeDisplayTransform() :
            KTDataTypeDisplay()
    {
    }

    KTDataTypeDisplayTransform::~KTDataTypeDisplayTransform()
    {
    }


    void KTDataTypeDisplayTransform::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTDataTypeDisplayTransform::DrawFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTDataTypeDisplayTransform::DrawFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("fs-polar-phase", this, &KTDataTypeDisplayTransform::DrawFrequencySpectrumDataPolarPhase);
        fWriter->RegisterSlot("fs-fftw-phase", this, &KTDataTypeDisplayTransform::DrawFrequencySpectrumDataFFTWPhase);
        fWriter->RegisterSlot("fs-polar-power", this, &KTDataTypeDisplayTransform::DrawFrequencySpectrumDataPolarPower);
        fWriter->RegisterSlot("fs-fftw-power", this, &KTDataTypeDisplayTransform::DrawFrequencySpectrumDataFFTWPower);
        fWriter->RegisterSlot("fs-polar-mag-dist", this, &KTDataTypeDisplayTransform::DrawFrequencySpectrumDataPolarMagnitudeDistribution);
        fWriter->RegisterSlot("fs-fftw-mag-dist", this, &KTDataTypeDisplayTransform::DrawFrequencySpectrumDataFFTWMagnitudeDistribution);
        fWriter->RegisterSlot("fs-polar-power-dist", this, &KTDataTypeDisplayTransform::DrawFrequencySpectrumDataPolarPowerDistribution);
        fWriter->RegisterSlot("fs-fftw-power-dist", this, &KTDataTypeDisplayTransform::DrawFrequencySpectrumDataFFTWPowerDistribution);
        fWriter->RegisterSlot("tf-polar", this, &KTDataTypeDisplayTransform::DrawTimeFrequencyDataPolar);
        fWriter->RegisterSlot("tf-polar-phase", this, &KTDataTypeDisplayTransform::DrawTimeFrequencyDataPolarPhase);
        fWriter->RegisterSlot("tf-polar-power", this, &KTDataTypeDisplayTransform::DrawTimeFrequencyDataPolarPower);
        fWriter->RegisterSlot("multi-fs-polar", this, &KTDataTypeDisplayTransform::DrawMultiFSDataPolar);
        fWriter->RegisterSlot("multi-fs-fftw", this, &KTDataTypeDisplayTransform::DrawMultiFSDataFFTW);
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTDataTypeDisplayTransform::DrawFrequencySpectrumDataPolar(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* magHist = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                fWriter->Draw(magHist);
            }
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawFrequencySpectrumDataFFTW(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSfftw_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* magHist = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                fWriter->Draw(magHist);
            }
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawFrequencySpectrumDataPolarPhase(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSpolarPhase_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* phaseHist = KT2ROOT::CreatePhaseHistogram(spectrum, histName);
                fWriter->Draw(phaseHist);
            }
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawFrequencySpectrumDataFFTWPhase(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSfftwPhase_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* phaseHist = KT2ROOT::CreatePhaseHistogram(spectrum, histName);
                fWriter->Draw(phaseHist);
            }
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawFrequencySpectrumDataPolarPower(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPSpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerHistogram(spectrum, histName);
                fWriter->Draw(powerSpectrum);
            }
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawFrequencySpectrumDataFFTWPower(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPSfftw_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerHistogram(spectrum, histName);
                fWriter->Draw(powerSpectrum);
            }
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawFrequencySpectrumDataPolarMagnitudeDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSDistpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* magDistHist = KT2ROOT::CreateMagnitudeDistributionHistogram(spectrum, histName);
                fWriter->Draw(magDistHist);
            }
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawFrequencySpectrumDataFFTWMagnitudeDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSDistfftw_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* magDistHist = KT2ROOT::CreateMagnitudeDistributionHistogram(spectrum, histName);
                fWriter->Draw(magDistHist);
            }
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawFrequencySpectrumDataPolarPowerDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPSDistpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerDistHist = KT2ROOT::CreatePowerDistributionHistogram(spectrum, histName);
                fWriter->Draw(powerDistHist);
            }
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawFrequencySpectrumDataFFTWPowerDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPSDistfftw_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerDistHist = KT2ROOT::CreatePowerDistributionHistogram(spectrum, histName);
                fWriter->Draw(powerDistHist);
            }
        }
        return;
    }


    //************************
    // Time/Frequency Data
    //************************

    void KTDataTypeDisplayTransform::DrawTimeFrequencyDataPolar(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeFrequencyDataPolar& fsData = data->Of<KTTimeFrequencyDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTTimeFrequencyPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histTFpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH2D* magHist = spectrum->CreateMagnitudeHistogram(histName);
                fWriter->Draw(magHist);
            }
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawTimeFrequencyDataPolarPhase(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeFrequencyDataPolar& fsData = data->Of<KTTimeFrequencyDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTTimeFrequencyPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histTFPhasepolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH2D* phaseHist = spectrum->CreatePhaseHistogram(histName);
                fWriter->Draw(phaseHist);
            }
        }
        return;
    }
    void KTDataTypeDisplayTransform::DrawTimeFrequencyDataPolarPower(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeFrequencyDataPolar& fsData = data->Of<KTTimeFrequencyDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTTimeFrequencyPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histTFPowerpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH2D* powerSpectrum = spectrum->CreatePowerHistogram(histName);
                fWriter->Draw(powerSpectrum);
            }
        }
        return;
    }


    //*****************
    // Multi-FS Data
    //*****************

    void KTDataTypeDisplayTransform::DrawMultiFSDataPolar(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTMultiFSDataPolar& fsData = data->Of<KTMultiFSDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iPlot = 0; iPlot < nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histMFSpolar_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* mfsHist = fsData.CreateMagnitudeHistogram(iPlot, histName);
            fWriter->Draw(mfsHist);
        }
        return;
    }

    void KTDataTypeDisplayTransform::DrawMultiFSDataFFTW(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTMultiFSDataPolar& fsData = data->Of<KTMultiFSDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iPlot = 0; iPlot < nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histMFSfftwr_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* mfsHist = fsData.CreateMagnitudeHistogram(iPlot, histName);
            fWriter->Draw(mfsHist);
        }
        return;
    }

} /* namespace Katydid */
