/*
 * KTMatchedFilter.cc
 *
 *  Created on: May 21, 2018
 *      Author: N. Buzinsky
 */

#include "KTMatchedFilter.hh"

#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTLogger.hh"
//#include "KTMatchedFilteredData.hh"
#include "KTMath.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"

#include "param.hh"


namespace Katydid
{
    KTLOGGER(gclog, "KTMatchedFilter");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTMatchedFilter, "matched-filter");

    KTMatchedFilter::KTMatchedFilter(const std::string& name) :
            KTProcessor(name),
            fHasHypothesis(false),
            fHasData(false),
            fHypothesisCachedInnerProduct(1.),
            fWindowSize(8192),
            fTSHypothesisSlot("ts-hypothesis", this, &KTMatchedFilter::HypothesisSetup, &fTSHypothesisSignal),
            fTSDataSlot("ts-data", this, &KTMatchedFilter::Filter, &fTSDataSignal)
    {
    }

    KTMatchedFilter::~KTMatchedFilter()
    {
    }

    bool KTMatchedFilter::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetWindowSize(node->get_value("window-size", GetWindowSize()));
        //SetDomain(node->get_value("domain", GetDomain()));
        //SetOverlap(node->get_value("overlap", GetOverlap()));

        return true;
    }

    bool KTMatchedFilter::HypothesisSetup(KTTimeSeriesData& tsHypothesis)
    {
        if(tsHypothesis.GetNComponents() > 0 )
        {
            KTTimeSeriesFFTW *ts = dynamic_cast< KTTimeSeriesFFTW* >(tsHypothesis.GetTimeSeries(0));
            SetHypothesis(*ts);
            //Cache inner product of hypothesis with hypothesis ie <h|h>
            SetHypothesisCachedInnerProduct(InnerProduct(ts));
            SetHasHypothesis(true);
        }
        else
        {
            KTERROR(gclog, "Matched Filter hypothesis has zero components! ");
        }
        return true;
    }

    bool KTMatchedFilter::Filter(KTTimeSeriesData& tsData)
    {
        KTDEBUG(gclog, "Receiving data time series for matched filter");

        KTTimeSeriesFFTW* ts;   // time series from data

        // Slice and TS parameters

        //double t = slice.GetTimeInAcq();    // time value of the current bin
        //double dt = slice.GetBinWidth();    // time step
        //KTINFO(gclog, "Set up time series and slice parameters.");
        //KTDEBUG(gclog, "Initial t = " << t);
        //KTDEBUG(gclog, "Time step = " << dt);

        SetHasData(true);

        for( unsigned iComponent = 0; iComponent < tsData.GetNComponents(); ++iComponent )
        {
            if(GetHasHypothesis())
            {
                ts = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries( iComponent ));
                SNR(ts);
            }
            else
            {
                //If the template bank hypothesis is not set, throw error
                KTERROR(gclog, "Matched Filter hypothesis not set! ");
            }

        }
        return true;
    }

    double KTMatchedFilter::SNR(KTTimeSeriesFFTW* tsData)
    {
       return InnerProduct(tsData) / sqrt ( GetHypothesisCachedInnerProduct() );
    }

    //Define as Re ( a . b*)
    double KTMatchedFilter::InnerProduct(KTTimeSeriesFFTW* tsData)
    {
        double total = 0.;
        for(unsigned iBin = 0; iBin < GetWindowSize(); ++iBin)
        {
            total += (*tsData)(iBin)[0] * (fHypothesis)(iBin)[0];
            total += (*tsData)(iBin)[1] * (fHypothesis)(iBin)[1];
        }
    }

} /* namespace Katydid */
