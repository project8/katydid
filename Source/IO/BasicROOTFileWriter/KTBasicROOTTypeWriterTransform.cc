/*
 * KTBasicROOTTypeWriterTransform.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterTransform.hh"

#include "KT2ROOT.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTSliceHeader.hh"
#include "KTMultiFSDataPolar.hh"
#include "KTMultiFSDataFFTW.hh"
#include "KTMultiPSData.hh"
#include "KTSpectrumCollectionData.hh"
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
    KTLOGGER(publog, "KTBasicROOTTypeWriterTransform");


    static Nymph::KTTIRegistrar< KTBasicROOTTypeWriter, KTBasicROOTTypeWriterTransform > sBRTWFFTRegistrar;

    KTBasicROOTTypeWriterTransform::KTBasicROOTTypeWriterTransform() :
            KTBasicROOTTypeWriter()
    {
    }

    KTBasicROOTTypeWriterTransform::~KTBasicROOTTypeWriterTransform()
    {
    }


    void KTBasicROOTTypeWriterTransform::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("fs-polar-phase", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataPolarPhase);
        fWriter->RegisterSlot("fs-fftw-phase", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataFFTWPhase);
        fWriter->RegisterSlot("fs-polar-power", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataPolarPower);
        fWriter->RegisterSlot("fs-fftw-power", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataFFTWPower);
        fWriter->RegisterSlot("fs-polar-mag-dist", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataPolarMagnitudeDistribution);
        fWriter->RegisterSlot("fs-fftw-mag-dist", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataFFTWMagnitudeDistribution);
        fWriter->RegisterSlot("fs-polar-power-dist", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataPolarPowerDistribution);
        fWriter->RegisterSlot("fs-fftw-power-dist", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataFFTWPowerDistribution);
        fWriter->RegisterSlot("fs-polar-var", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumVarianceDataPolar);
        fWriter->RegisterSlot("fs-fftw-var", this, &KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumVarianceDataFFTW);
        fWriter->RegisterSlot("ps", this, &KTBasicROOTTypeWriterTransform::WritePowerSpectrum);
	KTDEBUG(publog, "registering psd slot");
        fWriter->RegisterSlot("psd", this, &KTBasicROOTTypeWriterTransform::WritePowerSpectralDensity);
	KTDEBUG(publog, "psd slot registered");
        fWriter->RegisterSlot("ps-dist", this, &KTBasicROOTTypeWriterTransform::WritePowerSpectrumDistribution);
        fWriter->RegisterSlot("psd-dist", this, &KTBasicROOTTypeWriterTransform::WritePowerSpectralDensityDistribution);
        fWriter->RegisterSlot("ps-var", this, &KTBasicROOTTypeWriterTransform::WritePowerSpectrumVarianceData);
        fWriter->RegisterSlot("tf-polar", this, &KTBasicROOTTypeWriterTransform::WriteTimeFrequencyDataPolar);
        fWriter->RegisterSlot("tf-polar-phase", this, &KTBasicROOTTypeWriterTransform::WriteTimeFrequencyDataPolarPhase);
        fWriter->RegisterSlot("tf-polar-power", this, &KTBasicROOTTypeWriterTransform::WriteTimeFrequencyDataPolarPower);
        fWriter->RegisterSlot("multi-fs-polar", this, &KTBasicROOTTypeWriterTransform::WriteMultiFSDataPolar);
        fWriter->RegisterSlot("multi-fs-fftw", this, &KTBasicROOTTypeWriterTransform::WriteMultiFSDataFFTW);
        fWriter->RegisterSlot("multi-ps", this, &KTBasicROOTTypeWriterTransform::WriteMultiPSData);
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataPolar(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataFFTW(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataPolarPhase(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataFFTWPhase(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataPolarPower(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataFFTWPower(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataPolarMagnitudeDistribution(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataFFTWMagnitudeDistribution(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataPolarPowerDistribution(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumDataFFTWPowerDistribution(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumVarianceDataPolar(Nymph::KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumVarianceDataPolar& fsData = data->Of<KTFrequencySpectrumVarianceDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTFrequencySpectrumVariance* variance = fsData.GetSpectrum(iChannel);
            if (variance != NULL)
            {
                stringstream conv;
                conv << "histFSVarPolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* varianceSpectrum = KT2ROOT::CreateHistogram(variance, histName);
                varianceSpectrum->SetDirectory(fWriter->GetFile());
                varianceSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterTransform::WriteFrequencySpectrumVarianceDataFFTW(Nymph::KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTFrequencySpectrumVarianceDataFFTW& fsData = data->Of<KTFrequencySpectrumVarianceDataFFTW>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTFrequencySpectrumVariance* variance = fsData.GetSpectrum(iChannel);
            if (variance != NULL)
            {
                stringstream conv;
                conv << "histFSVarFFTW_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* varianceSpectrum = KT2ROOT::CreateHistogram(variance, histName);
                varianceSpectrum->SetDirectory(fWriter->GetFile());
                varianceSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }


    //********************
    // Power Spectrum Data
    //********************
    void KTBasicROOTTypeWriterTransform::WritePowerSpectrum(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WritePowerSpectralDensity(Nymph::KTDataPtr data)
    {
	printf("\n\nENTERED WritePowerSpectralDensity\n\n");
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
	printf("\n\nCOMPLETED WritePowerSpectralDensity\n\n");
        return;
    }

    void KTBasicROOTTypeWriterTransform::WritePowerSpectrumDistribution(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WritePowerSpectralDensityDistribution(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WritePowerSpectrumVarianceData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTPowerSpectrumVarianceData& fsData = data->Of<KTPowerSpectrumVarianceData>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTFrequencySpectrumVariance* variance = fsData.GetSpectrum(iChannel);
            if (variance != NULL)
            {
                stringstream conv;
                conv << "histPSVar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* varianceSpectrum = KT2ROOT::CreateHistogram(variance, histName);
                varianceSpectrum->SetDirectory(fWriter->GetFile());
                varianceSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }


    //************************
    // Time/Frequency Data
    //************************

    void KTBasicROOTTypeWriterTransform::WriteTimeFrequencyDataPolar(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteTimeFrequencyDataPolarPhase(Nymph::KTDataPtr data)
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
    void KTBasicROOTTypeWriterTransform::WriteTimeFrequencyDataPolarPower(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteMultiFSDataPolar(Nymph::KTDataPtr data)
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

    void KTBasicROOTTypeWriterTransform::WriteMultiFSDataFFTW(Nymph::KTDataPtr data)
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

    //*****************
    // Multi-PS Data
    //*****************

    void KTBasicROOTTypeWriterTransform::WriteMultiPSData(Nymph::KTDataPtr data)
    {
        KTINFO(publog, "Got multi-ps signal, going to write spectrogram");
        bool hastype = data->Has<KTMultiPSData>();
        KTDEBUG(publog, "Has data: "<<hastype);
        bool hastype1 = data->Has<KTPSCollectionData>();
        KTDEBUG(publog, "Has data: "<<hastype1);
        if (! data) return;

        //uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        //KTDEBUG(publog, "Slice number: "<<sliceNumber);

        KTPSCollectionData& psData = data->Of<KTPSCollectionData>();
        unsigned nComponents = psData.GetNComponents();
        KTDEBUG(publog, "nComponents: "<<nComponents);

        uint64_t spectrogramNumber = psData.GetSpectrogramCounter();
        KTDEBUG(publog, "Slice number: "<<spectrogramNumber);

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPlot = 0; iPlot < nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histMPS_" << spectrogramNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* mpsHist = psData.CreatePowerHistogram(iPlot, histName);
            mpsHist->SetDirectory(fWriter->GetFile());
            mpsHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

} /* namespace Katydid */
