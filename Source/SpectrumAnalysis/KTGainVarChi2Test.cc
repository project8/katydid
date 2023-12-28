/*
 * KTGainVarChi2Test.cc
 *
 *  Created on: Oct 8, 2015
 *      Author: ezayas
 */

#include "KTGainVarChi2Test.hh"

#include "KTGainVariationData.hh"
#include "KTGainVarChi2Data.hh"
#include "KTParam.hh"
#include "KTPowerSpectrumData.hh"
#include "KTPowerSpectrumUncertaintyData.hh"
#include "KTSpline.hh"

#include <cmath>
#include <vector>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::string;
using std::vector;


namespace Katydid
{
    LOGGER(sdlog, "KTGainVarChi2Test");

    KT_REGISTER_PROCESSOR(KTGainVarChi2Test, "variable-spectrum-chi2test");

    KTGainVarChi2Test::KTGainVarChi2Test(const std::string& name) :
            KTProcessor(name),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fChi2TestSignal("chi2-test", this),
            fPSSlot("ps", this, &KTGainVarChi2Test::Calculate, &fChi2TestSignal)
    {
    }

    KTGainVarChi2Test::~KTGainVarChi2Test()
    {
    }

    bool KTGainVarChi2Test::Configure(const KTParamNode* node)
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

    bool KTGainVarChi2Test::Calculate(KTPowerSpectrumData& data, KTPowerSpectrumUncertaintyData& sigma, KTGainVariationData& gvData)
    {
        KTGainVarChi2Data& newData = data.Of< KTGainVarChi2Data >().SetNComponents(data.GetNComponents());

        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrum(0)->FindBin(fMinFrequency));
            LDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrum(0)->FindBin(fMaxFrequency));
            LDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }

        unsigned nComponents = data.GetNComponents();

        //newData.SetNBins(data.GetSpectrum(0)->size());
        //newData.SetBinWidth(data.GetSpectrum(0)->GetBinWidth());

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            if (! CalculateSpectrum(data.GetSpectrum(iComponent), sigma.GetSpectrum(iComponent), gvData.GetSpline(iComponent), newData, iComponent))
            {
                LERROR(sdlog, "Chi-squared calculation on spectrum (component " << iComponent << ") failed");
                return false;
            }
        }
        LINFO(sdlog, "Completed chi-squared calculation on " << nComponents << " components");

        return true;
    }

    bool KTGainVarChi2Test::CalculateSpectrum(const KTPowerSpectrum* spectrum, const KTPowerSpectrum* sigma, const KTSpline* spline, KTGainVarChi2Data& newData, unsigned component)
    {
        if (spectrum == NULL)
        {
            LERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        unsigned nBins = fMaxBin - fMinBin + 1;
        double binWidth = spectrum->GetBinWidth();
        double freqMin = spectrum->GetBinLowEdge(fMinBin);
        double freqMax = spectrum->GetBinLowEdge(fMaxBin) + spectrum->GetBinWidth();
        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        double chi2 = 0., dev = 0.;
        uint64_t ndf = 0;
        for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
        {
            dev = ((*spectrum)(iBin) - (*splineImp)(iBin - fMinBin))/(*sigma)(iBin);
            chi2 += dev * dev;
            ndf += 1;
        }

        newData.SetChi2( chi2 );
        newData.SetNDF( ndf );

        spline->AddToCache(splineImp);

        return true;

    }


} /* namespace Katydid */