/*
 * KTPolyphaseFilterBank.cc
 *
 *  Created on: Apr 18, 2013
 *      Author: nsoblath
 */

#include "KTPolyphaseFilterBank.hh"

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
    KTLOGGER(pfblog, "katydid.fft");

    static KTDerivedRegistrar< KTProcessor, KTPolyphaseFilterBank > sWindowerRegistrar("polyphase-filter-bank");

    KTPolyphaseFilterBank::KTPolyphaseFilterBank(const std::string& name) :
            KTProcessor(name),
            fNSubsets(2),
            fSubsetSize(1),
            fWindowed("windowed", this),
            fHeaderSlot("header", this, &KTPolyphaseFilterBank::InitializeWithHeader),
            fTimeSeriesRealSlot("ts", this, &KTPolyphaseFilterBank::WindowDataReal, &fWindowed),
            fTimeSeriesFFTWSlot("ts", this, &KTPolyphaseFilterBank::WindowDataFFTW, &fWindowed)
    {
    }

    KTPolyphaseFilterBank::~KTPolyphaseFilterBank()
    {
    }

    Bool_t KTPolyphaseFilterBank::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node == NULL)
        {
            return false;
        }

        if (node->HasData("n-subsets"))
        {
            SetNSubsets(node->GetData("n-subsets", fNSubsets));
        }
        if (node->HasData("subset-size"))
        {
            SetSubsetSize(node->GetData("subset-size", fSubsetSize));
        }

        return true;
    }

    Bool_t KTPolyphaseFilterBank::ProcessDataReal(const KTTimeSeriesData& tsData)
    {
        if (dynamic_cast< KTTimeSeriesReal* >(tsData.GetTimeSeries(0)) == NULL)
        {
            KTERROR(pfblog, "Data does not contain real-type time series.");
            return false;
        }

        KTTimeSeriesData* newData = CreateFilteredDataReal(tsData);
        if (newData != NULL)
        {
            return true;
        }
        KTERROR(pfblog, "New data object was not created correctly (real)");
        return false;
    }

    Bool_t KTPolyphaseFilterBank::ProcessDataFFTW(const KTTimeSeriesData& tsData)
    {
        if (dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries(0)) == NULL)
        {
            KTERROR(pfblog, "Data does not contain fftw-type time series.");
            return false;
        }

        KTTimeSeriesData* newData = CreateFilteredDataFFTW(tsData);
        if (newData != NULL)
        {
            return true;
        }
        KTERROR(pfblog, "New data object was not created correctly (fftw)");
        return false;
    }

    KTTimeSeriesData* KTPolyphaseFilterBank::CreateFilteredDataReal(const KTTimeSeriesData& tsData)
    {
        UInt_t nComponents = tsData.GetNComponents();

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            KTTimeSeriesReal* newTS = ApplyPFB(static_cast< const KTTimeSeriesReal* >(tsData.GetTimeSeries(iComponent)));
        }

    }

    KTTimeSeriesData* KTPolyphaseFilterBank::CreateFilteredDataFFTW(const KTTimeSeriesData& tsData)
    {
        UInt_t nComponents = tsData.GetNComponents();

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            KTTimeSeriesFFTW* newTS = ApplyPFB(static_cast< const KTTimeSeriesFFTW* >(tsData.GetTimeSeries(iComponent)));
        }
    }

    KTTimeSeriesReal* KTPolyphaseFilterBank::ApplyPFB(const KTTimeSeriesReal* data) const
    {

    }

    KTTimeSeriesFFTW* KTPolyphaseFilterBank::ApplyPFB(const KTTimeSeriesFFTW* data) const
    {

    }

} /* namespace Katydid */
