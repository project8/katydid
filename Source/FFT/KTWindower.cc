/*
 * KTWindower.cc
 *
 *  Created on: Apr 18, 2013
 *      Author: nsoblath
 */

#include "KTWindower.hh"

#include "KTEggHeader.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"
#include "KTWindowFunction.hh"

using std::string;
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(windowlog, "katydid.fft");

    static KTDerivedRegistrar< KTProcessor, KTWindower > sWindowerRegistrar("windower");

    KTWindower::KTWindower(const std::string& name) :
            KTProcessor(name),
            fWindowFunction(NULL),
            fWindowed("windowed", this),
            fHeaderSlot("header", this, &KTWindower::InitializeWithHeader),
            fTimeSeriesRealSlot("ts", this, &KTWindower::WindowDataReal, &fWindowed),
            fTimeSeriesFFTWSlot("ts", this, &KTWindower::WindowDataFFTW, &fWindowed)
    {
    }

    KTWindower::~KTWindower()
    {
        delete fWindowFunction;
    }

    Bool_t KTWindower::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            string windowType = node->GetData< string >("window-function-type", "rectangular");
            KTWindowFunction* tempWF = KTFactory< KTWindowFunction >::GetInstance()->Create(windowType);
            if (tempWF == NULL)
            {
                KTERROR(windowlog, "Invalid window function type given: <" << windowType << ">.");
                return false;
            }
            SetWindowFunction(tempWF);
        }

        return true;
    }

    Bool_t KTWindower::InitializeWindow()
    {
        fWindowFunction->RebuildWindowFunction();
        return true;
    }

    void KTWindower::InitializeWithHeader(const KTEggHeader* header)
    {
        fWindowFunction->SetBinWidth(1. / header->GetAcquisitionRate());
        fWindowFunction->SetSize(header->GetSliceSize());
        if (! InitializeWindow())
        {
            KTERROR(windowlog, "Something went wrong while initializing the window function!");
            return;
        }
        return;
    }

    Bool_t KTWindower::WindowDataReal(KTTimeSeriesData& tsData)
    {
        if (tsData.GetTimeSeries(0)->GetNTimeBins() != fWindowFunction->GetSize())
        {
            fWindowFunction->AdaptTo(&tsData); // this call rebuilds the window, so that doesn't need to be done separately
        }

        UInt_t nComponents = tsData.GetNComponents();

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            KTTimeSeriesReal* nextInput = dynamic_cast< KTTimeSeriesReal* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                KTERROR(windowlog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                return false;
            }

            Bool_t result = ApplyWindow(nextInput);

            if (! result)
            {
                KTERROR(windowlog, "Component <" << iComponent << "> did not get windowed correctly.");
                return false;
            }
        }

        KTINFO(fftlog_comp, "Windowing complete");

        return true;
    }

    Bool_t KTWindower::WindowDataFFTW(KTTimeSeriesData& tsData)
    {
        if (tsData.GetTimeSeries(0)->GetNTimeBins() != fWindowFunction->GetSize())
        {
            fWindowFunction->AdaptTo(&tsData); // this call rebuilds the window, so that doesn't need to be done separately
        }

        UInt_t nComponents = tsData.GetNComponents();

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            KTTimeSeriesFFTW* nextInput = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                KTERROR(windowlog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                return false;
            }

            Bool_t result = ApplyWindow(nextInput);

            if (! result)
            {
                KTERROR(windowlog, "Component <" << iComponent << "> did not get windowed correctly.");
                return false;
            }
        }

        KTINFO(fftlog_comp, "Windowing complete");

        return true;
    }

    Bool_t KTWindower::ApplyWindow(KTTimeSeriesReal* data) const
    {
        UInt_t nBins = data->size();
        if (nBins != fWindowFunction->GetSize())
        {
            KTWARN(windowlog, "Number of bins in the data provided does not match the number of bins set for this window\n"
                    << "   Bin expected: " << fWindowFunction->GetSize() << ";   Bins in data: " << nBins);
            return false;
        }

        for (UInt_t iBin=0; iBin < nBins; iBin++)
        {
            (*data)(iBin) = (*data)(iBin) * fWindowFunction->GetWeight(iBin);
        }

        return true;
    }

    Bool_t KTWindower::ApplyWindow(KTTimeSeriesFFTW* data) const
    {
        UInt_t nBins = data->size();
        if (nBins != fWindowFunction->GetSize())
        {
            KTWARN(windowlog, "Number of bins in the data provided does not match the number of bins set for this window\n"
                    << "   Bin expected: " << fWindowFunction->GetSize() << ";   Bins in data: " << nBins);
            return false;
        }

        Double_t weight;
        for (UInt_t iBin=0; iBin < nBins; iBin++)
        {
            weight = fWindowFunction->GetWeight(iBin);
            (*data)(iBin)[0] = (*data)(iBin)[0] * weight;
            (*data)(iBin)[1] = (*data)(iBin)[1] * weight;
        }

        return true;
    }

    void KTWindower::SetWindowFunction(KTWindowFunction* wf)
    {
        delete fWindowFunction;
        fWindowFunction = wf;
        return;
    }



} /* namespace Katydid */
