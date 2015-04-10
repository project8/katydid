/*
 * KTDataTypeDisplayFFT.cc
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#include "KTDataTypeDisplayFFT.hh"

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



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    static KTTIRegistrar< KTDataTypeDisplay, KTDataTypeDisplayFFT > sBRTWFFTRegistrar;

    KTDataTypeDisplayFFT::KTDataTypeDisplayFFT() :
            KTDataTypeDisplay()
    {
    }

    KTDataTypeDisplayFFT::~KTDataTypeDisplayFFT()
    {
    }


    void KTDataTypeDisplayFFT::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTDataTypeDisplayFFT::DrawFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTDataTypeDisplayFFT::DrawFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("fs-polar-phase", this, &KTDataTypeDisplayFFT::DrawFrequencySpectrumDataPolarPhase);
        fWriter->RegisterSlot("fs-fftw-phase", this, &KTDataTypeDisplayFFT::DrawFrequencySpectrumDataFFTWPhase);
        fWriter->RegisterSlot("fs-polar-power", this, &KTDataTypeDisplayFFT::DrawFrequencySpectrumDataPolarPower);
        fWriter->RegisterSlot("fs-fftw-power", this, &KTDataTypeDisplayFFT::DrawFrequencySpectrumDataFFTWPower);
        fWriter->RegisterSlot("fs-polar-mag-dist", this, &KTDataTypeDisplayFFT::DrawFrequencySpectrumDataPolarMagnitudeDistribution);
        fWriter->RegisterSlot("fs-fftw-mag-dist", this, &KTDataTypeDisplayFFT::DrawFrequencySpectrumDataFFTWMagnitudeDistribution);
        fWriter->RegisterSlot("fs-polar-power-dist", this, &KTDataTypeDisplayFFT::DrawFrequencySpectrumDataPolarPowerDistribution);
        fWriter->RegisterSlot("fs-fftw-power-dist", this, &KTDataTypeDisplayFFT::DrawFrequencySpectrumDataFFTWPowerDistribution);
        fWriter->RegisterSlot("tf-polar", this, &KTDataTypeDisplayFFT::DrawTimeFrequencyDataPolar);
        fWriter->RegisterSlot("tf-polar-phase", this, &KTDataTypeDisplayFFT::DrawTimeFrequencyDataPolarPhase);
        fWriter->RegisterSlot("tf-polar-power", this, &KTDataTypeDisplayFFT::DrawTimeFrequencyDataPolarPower);
        fWriter->RegisterSlot("multi-fs-polar", this, &KTDataTypeDisplayFFT::DrawMultiFSDataPolar);
        fWriter->RegisterSlot("multi-fs-fftw", this, &KTDataTypeDisplayFFT::DrawMultiFSDataFFTW);
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTDataTypeDisplayFFT::DrawFrequencySpectrumDataPolar(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawFrequencySpectrumDataFFTW(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawFrequencySpectrumDataPolarPhase(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawFrequencySpectrumDataFFTWPhase(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawFrequencySpectrumDataPolarPower(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawFrequencySpectrumDataFFTWPower(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawFrequencySpectrumDataPolarMagnitudeDistribution(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawFrequencySpectrumDataFFTWMagnitudeDistribution(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawFrequencySpectrumDataPolarPowerDistribution(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawFrequencySpectrumDataFFTWPowerDistribution(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawTimeFrequencyDataPolar(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawTimeFrequencyDataPolarPhase(KTDataPtr data)
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
    void KTDataTypeDisplayFFT::DrawTimeFrequencyDataPolarPower(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawMultiFSDataPolar(KTDataPtr data)
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

    void KTDataTypeDisplayFFT::DrawMultiFSDataFFTW(KTDataPtr data)
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
