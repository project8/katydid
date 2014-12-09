/*
 * KTBasicROOTTypeWriterFFT.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterFFT.hh"

#include "KT2ROOT.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTSliceHeader.hh"
#include "KTMultiFSDataPolar.hh"
#include "KTMultiFSDataFFTW.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTTimeFrequencyDataPolar.hh"
#include "KTTimeFrequencyPolar.hh"

#include "TH1.h"
#include "TH2.h"

#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTBasicROOTTypeWriterFFT");


    static KTTIRegistrar< KTBasicROOTTypeWriter, KTBasicROOTTypeWriterFFT > sBRTWFFTRegistrar;

    KTBasicROOTTypeWriterFFT::KTBasicROOTTypeWriterFFT() :
            KTBasicROOTTypeWriter()
    {
    }

    KTBasicROOTTypeWriterFFT::~KTBasicROOTTypeWriterFFT()
    {
    }


    void KTBasicROOTTypeWriterFFT::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("fs-polar-phase", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataPolarPhase);
        fWriter->RegisterSlot("fs-fftw-phase", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTWPhase);
        fWriter->RegisterSlot("fs-polar-power", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataPolarPower);
        fWriter->RegisterSlot("fs-fftw-power", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTWPower);
        fWriter->RegisterSlot("fs-polar-mag-dist", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataPolarMagnitudeDistribution);
        fWriter->RegisterSlot("fs-fftw-mag-dist", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTWMagnitudeDistribution);
        fWriter->RegisterSlot("fs-polar-power-dist", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataPolarPowerDistribution);
        fWriter->RegisterSlot("fs-fftw-power-dist", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTWPowerDistribution);
        fWriter->RegisterSlot("ps", this, &KTBasicROOTTypeWriterFFT::WritePowerSpectrum);
        fWriter->RegisterSlot("psd", this, &KTBasicROOTTypeWriterFFT::WritePowerSpectralDensity);
        fWriter->RegisterSlot("ps-dist", this, &KTBasicROOTTypeWriterFFT::WritePowerSpectrumDistribution);
        fWriter->RegisterSlot("psd-dist", this, &KTBasicROOTTypeWriterFFT::WritePowerSpectralDensityDistribution);
        fWriter->RegisterSlot("tf-polar", this, &KTBasicROOTTypeWriterFFT::WriteTimeFrequencyDataPolar);
        fWriter->RegisterSlot("tf-polar-phase", this, &KTBasicROOTTypeWriterFFT::WriteTimeFrequencyDataPolarPhase);
        fWriter->RegisterSlot("tf-polar-power", this, &KTBasicROOTTypeWriterFFT::WriteTimeFrequencyDataPolarPower);
        fWriter->RegisterSlot("multi-fs-polar", this, &KTBasicROOTTypeWriterFFT::WriteMultiFSDataPolar);
        fWriter->RegisterSlot("multi-fs-fftw", this, &KTBasicROOTTypeWriterFFT::WriteMultiFSDataFFTW);
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataPolar(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTW(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSfftw_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataPolarPhase(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSpolarPhase_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePhaseHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTWPhase(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSfftwPhase_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePhaseHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataPolarPower(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

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
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTWPower(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

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
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataPolarMagnitudeDistribution(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSDistpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreateMagnitudeDistributionHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTWMagnitudeDistribution(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histFSDistfftw_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreateMagnitudeDistributionHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataPolarPowerDistribution(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPSDistpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerDistributionHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTWPowerDistribution(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPSDistfftw_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerDistributionHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }


    //********************
    // Power Spectrum Data
    //********************
    void KTBasicROOTTypeWriterFFT::WritePowerSpectrum(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTPowerSpectrum* spectrum = fsData.GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectrum();
                stringstream conv;
                conv << "histPS_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WritePowerSpectralDensity(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTPowerSpectrum* spectrum = fsData.GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectralDensity();
                stringstream conv;
                conv << "histPSD_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WritePowerSpectrumDistribution(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTPowerSpectrum* spectrum = fsData.GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectrum();
                stringstream conv;
                conv << "histPSdist_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerDistributionHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WritePowerSpectralDensityDistribution(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTPowerSpectrum* spectrum = fsData.GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectralDensity();
                stringstream conv;
                conv << "histPSDdist_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerDistributionHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }



    //************************
    // Time/Frequency Data
    //************************

    void KTBasicROOTTypeWriterFFT::WriteTimeFrequencyDataPolar(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeFrequencyDataPolar& fsData = data->Of<KTTimeFrequencyDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTTimeFrequencyPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histTFpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH2D* powerSpectrum = spectrum->CreateMagnitudeHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteTimeFrequencyDataPolarPhase(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeFrequencyDataPolar& fsData = data->Of<KTTimeFrequencyDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTTimeFrequencyPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histTFPhasepolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH2D* powerSpectrum = spectrum->CreatePhaseHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    void KTBasicROOTTypeWriterFFT::WriteTimeFrequencyDataPolarPower(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeFrequencyDataPolar& fsData = data->Of<KTTimeFrequencyDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

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
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }


    //*****************
    // Multi-FS Data
    //*****************

    void KTBasicROOTTypeWriterFFT::WriteMultiFSDataPolar(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTMultiFSDataPolar& fsData = data->Of<KTMultiFSDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPlot = 0; iPlot < nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histMFSpolar_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* mfsHist = fsData.CreateMagnitudeHistogram(iPlot, histName);
            mfsHist->SetDirectory(fWriter->GetFile());
            mfsHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteMultiFSDataFFTW(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTMultiFSDataPolar& fsData = data->Of<KTMultiFSDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPlot = 0; iPlot < nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histMFSfftwr_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* mfsHist = fsData.CreateMagnitudeHistogram(iPlot, histName);
            mfsHist->SetDirectory(fWriter->GetFile());
            mfsHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

} /* namespace Katydid */
