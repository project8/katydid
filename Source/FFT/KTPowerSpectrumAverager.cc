/*
 * KTPowerSpectrumAverager.cc
 *
 *  Created on: Nov 30, 2012
 *      Author: nsoblath
 */

#include "KTPowerSpectrumAverager.hh"

#include "KTEggHeader.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include "TCanvas.h"
#include "TH1.h"
#include "TStyle.h"

#include <sstream>

using boost::shared_ptr;

using std::string;
using std::stringstream;
using std::vector;

namespace Katydid
{
    KTLOGGER(psavglog, "katydid.applications.main");

    static KTDerivedRegistrar< KTProcessor, KTPowerSpectrumAverager > sSimpleFFTRegistrar("power-spectrum-averager");

    KTPowerSpectrumAverager::KTPowerSpectrumAverager() :
            KTProcessor(),
            fOutputFilePath(""),
            fOutputFilenameBase("average_power_spectrum"),
            fOutputFileType("png"),
            fStartNewHistFlag(true),
            fAveragePSHists(),
            fInputDataName("frequency-spectrum")
    {
        fConfigName = "power-spectrum-averager";

        RegisterSlot("header", this, &KTPowerSpectrumAverager::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("event", this, &KTPowerSpectrumAverager::ProcessEvent, "void (shared_ptr<KTEvent>)");
        RegisterSlot("finish", this, &KTPowerSpectrumAverager::Finish, "void ()");
    };

    KTPowerSpectrumAverager::~KTPowerSpectrumAverager()
    {
        for (vector<TH1D*>::iterator it=fAveragePSHists.begin(); it != fAveragePSHists.end(); it++)
        {
            delete *it;
        }
    };

    Bool_t KTPowerSpectrumAverager::Configure(const KTPStoreNode* node)
    {
        SetOutputFilePath(node->GetData< string >("output-file-path", fOutputFilePath));
        SetOutputFilenameBase(node->GetData< string >("output-filename-base", fOutputFilenameBase));
        SetOutputFileType(node->GetData< string >("output-file-type", fOutputFileType));
        SetInputDataName(node->GetData< string >("input-data-name", fInputDataName));

        return true;
    }

    void KTPowerSpectrumAverager::ProcessHeader(const KTEggHeader* header)
    {
        fStartNewHistFlag = true;
    }

    void KTPowerSpectrumAverager::ProcessEvent(shared_ptr<KTEvent> event)
    {
        const KTFrequencySpectrumData* fsData = event->GetData< KTFrequencySpectrumData >(fInputDataName);
        if (fsData == NULL)
        {
            KTWARN(psavglog, "No frequency-spectrum data named <" << fInputDataName << "> was available in the event");
            return;
        }

        if (fStartNewHistFlag)
        {
            fStartNewHistFlag = false;

            for (vector<TH1D*>::iterator it=fAveragePSHists.begin(); it != fAveragePSHists.end(); it++)
            {
                delete *it;
            }
            fAveragePSHists.clear();
            if (fAveragePSHists.size() != fsData->GetNChannels())
                fAveragePSHists.resize(fsData->GetNChannels());

            string histNameBase("PowerSpectrum");
            for (UInt_t iChannel=0; iChannel < fsData->GetNChannels(); iChannel++)
            {
                stringstream conv;
                conv << iChannel;
                string histName = histNameBase + conv.str();
                TH1D* newPS = fsData->GetSpectrum(iChannel)->CreatePowerHistogram(histName);
                fAveragePSHists[iChannel] = newPS;
            }
        }
        else
        {
            for (UInt_t iChannel=0; iChannel < fsData->GetNChannels(); iChannel++)
            {
                TH1D* newPS = fsData->GetSpectrum(iChannel)->CreatePowerHistogram();
                fAveragePSHists[iChannel]->Add(newPS);
                delete newPS;
            }
        }

        return;
    }

    void KTPowerSpectrumAverager::Finish()
    {
        gStyle->SetOptStat(0);
        for (UInt_t iChannel=0; iChannel < fAveragePSHists.size(); iChannel++)
        {
            stringstream conv;
            conv << "_" << iChannel << "." << fOutputFileType;
            string fileName = fOutputFilenameBase + conv.str();
            if (! fOutputFilePath.empty()) fileName = fOutputFilePath + '/' + fileName;

            TCanvas* cPrint = new TCanvas("cPrint", "cPrint");
            cPrint->SetLogy(1);
            fAveragePSHists[iChannel]->Draw();

            cPrint->Print(fileName.c_str(), fOutputFileType.c_str());
            KTINFO(psavglog, "Printed file " << fileName);
            delete cPrint;
        }

        fStartNewHistFlag = true;
        return;
    }

} /* namespace Katydid */
