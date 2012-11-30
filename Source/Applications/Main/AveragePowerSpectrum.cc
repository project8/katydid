/*
 * AveragePowerSpectrum.cc
 *
 *  Created on: Nov 29, 2012
 *      Author: nsoblath
 */


#include "KTApplication.hh"
#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTLogger.hh"
#include "KTProcessor.hh"
#include "KTProcessorToolbox.hh"

#include "TCanvas.h"
#include "TH1.h"

#include <boost/shared_ptr.hpp>

#include <sstream>
#include <string>
#include <vector>

KTLOGGER(katydidlog, "katydid.applications.main");

using namespace std;
using namespace Katydid;

/*!
 @class PowerSpectrumAverager
 @author N. S. Oblath

 @brief Averages multiple power spectra

 @details
 Processes events with KTFrequencySpectrumData-type power spectra, and averages their power spectra together.
 Power spectra are printed as ROOT histograms.

 Available configuration values:
 \li \c "output-filename-base": string -- the output files produced will be of the form [output-filename-base]_[channel].[output-file-type]
 \li \c "output-file-type": string -- see above for filename format. Any of the options in TPad::Print(char*, char*) are acceptable, but keep in mind that this is used as the file extension.

 Slots:
 \li \c void ProcessHeader(const KTEggHeader* header)
 \li \c void ProcessEvent(boost::shared_ptr<KTEvent>)
 \li \c void Finish()

*/
class PowerSpectrumAverager;

static KTDerivedRegistrar< KTProcessor, PowerSpectrumAverager > sSimpleFFTRegistrar("power-spectrum-averager");

int main(int argc, char** argv)
{
    KTApplication app(argc, argv);
    app.ReadConfigFile();

    // Create and configure the processor toolbox.
    // This will create all of the requested processors, connect their signals and slots, and fill the run queue.
    string appConfigName("katydid");
    KTProcessorToolbox procTB;
    if (! app.Configure(&procTB, appConfigName))
    {
        KTERROR(katydidlog, "Unable to configure processor toolbox. Aborting.");
        return -1;
    }

    // Configure the processors
    if (! procTB.ConfigureProcessors(app.GetNode(appConfigName)))
    {
        KTERROR(katydidlog, "Unable to configure processors. Aborting.");
        return -2;
    }

    // Execute the run queue!
    Bool_t success = procTB.Run();

    if (! success) return -3;
    return 0;
}

class PowerSpectrumAverager : public KTProcessor
{
    private:
        string fOutputFilePath;
        string fOutputFilenameBase;
        string fOutputFileType;
        Bool_t fStartNewHistFlag;
        vector< TH1D* > fAveragePSHists;

    public:
        PowerSpectrumAverager() :
                KTProcessor(),
                fOutputFilePath(""),
                fOutputFilenameBase("average_power_spectrum"),
                fOutputFileType("png"),
                fStartNewHistFlag(true),
                fAveragePSHists()
        {
            fConfigName = "power-spectrum-averager";

            RegisterSlot("header", this, &PowerSpectrumAverager::ProcessHeader, "void (const KTEggHeader*)");
            RegisterSlot("event", this, &PowerSpectrumAverager::ProcessEvent, "void (shared_ptr<KTEvent>)");
            RegisterSlot("finish", this, &PowerSpectrumAverager::Finish, "void ()");
        };

        virtual ~PowerSpectrumAverager()
        {
            for (vector<TH1D*>::iterator it=fAveragePSHists.begin(); it != fAveragePSHists.end(); it++)
            {
                delete *it;
            }
        };

        Bool_t Configure(const KTPStoreNode* node)
        {
            fOutputFilePath = node->GetData< string >("output-file-path", fOutputFilePath);
            fOutputFilenameBase = node->GetData< string >("output-filename-base", fOutputFilenameBase);
            fOutputFileType - node->GetData< string >("output-file-type", fOutputFileType);

            return true;
        }

        void ProcessHeader(const KTEggHeader* header)
        {
            fStartNewHistFlag = true;
        }

        void ProcessEvent(boost::shared_ptr<KTEvent> event)
        {
            const KTFrequencySpectrumData* fsData = dynamic_cast< KTFrequencySpectrumData* >(event->GetData("frequency-spectrum"));
            if (fsData == NULL)
            {
                KTWARN(katydidlog, "No frequency-spectrum data named <frequency-spectrum> was available in the event");
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
                    if (! fAveragePSHists[iChannel]->Add(newPS))
                    {
                        KTWARN(katydidlog, "Addition of histograms failed(channel " << iChannel << "); check TH1::Add for failure conditions.");
                    }
                    delete newPS;
                }
            }

            return;
        }

        void Finish()
        {
            for (UInt_t iChannel=0; iChannel < fAveragePSHists.size(); iChannel++)
            {
                stringstream conv;
                conv << "_" << iChannel << "." << fOutputFileType;
                string fileName = fOutputFilenameBase + conv.str();

                TCanvas* cPrint = new TCanvas("cPrint", "cPrint");
                cPrint->SetLogy(1);
                fAveragePSHists[iChannel]->Draw();

                cPrint->Print(fileName.c_str(), fOutputFileType.c_str());
                delete cPrint;
            }

            fStartNewHistFlag = true;
            return;
        }

};


