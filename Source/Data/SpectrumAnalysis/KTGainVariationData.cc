/*
 * KTGainVariationData.cc
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#include "KTGainVariationData.hh"


namespace Katydid
{
    const std::string KTGainVariationData::sName("gain-variation");

    KTGainVariationData::KTGainVariationData() :
            KTExtensibleData< KTGainVariationData >(),
            fComponentData(1)
    {
        fComponentData[0].fSpline = NULL;
        fComponentData[0].fVarianceSpline = NULL;
    }

    KTGainVariationData::KTGainVariationData(const KTGainVariationData& orig) :
            KTExtensibleData< KTGainVariationData >(orig),
            fComponentData(orig.fComponentData.size())
    {
        unsigned nComponents = fComponentData.size();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            fComponentData[iComponent].fSpline = new KTSpline(*orig.fComponentData[iComponent].fSpline);
            fComponentData[iComponent].fVarianceSpline = new KTSpline(*orig.fComponentData[iComponent].fVarianceSpline);
        }
    }

    KTGainVariationData::~KTGainVariationData()
    {
        while (! fComponentData.empty())
        {
            //delete fChannelData.back().fGainVar;
            delete fComponentData.back().fSpline;
            delete fComponentData.back().fVarianceSpline;
            fComponentData.pop_back();
        }
    }

    KTGainVariationData& KTGainVariationData::operator=(const KTGainVariationData& rhs)
    {
        unsigned nComponents = rhs.GetNComponents();
        SetNComponents(nComponents);
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            delete fComponentData[iComponent].fSpline;
            delete fComponentData[iComponent].fVarianceSpline;
            fComponentData[iComponent].fSpline = new KTSpline(*rhs.fComponentData[iComponent].fSpline);
            fComponentData[iComponent].fVarianceSpline = new KTSpline(*rhs.fComponentData[iComponent].fVarianceSpline);
        }
        return *this;
    }

    KTGainVariationData& KTGainVariationData::SetNComponents(unsigned components)
    {
        unsigned oldSize = fComponentData.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            delete fComponentData[iComponent].fSpline;
            delete fComponentData[iComponent].fVarianceSpline;
        }
        fComponentData.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fComponentData[iComponent].fSpline = NULL;
            fComponentData[iComponent].fVarianceSpline = NULL;
        }
        return *this;
    }

#ifdef ROOT_FOUND
    TH1D* KTGainVariationData::CreateGainVariationHistogram(unsigned nBins, unsigned component, const std::string& name) const
    {
        KTSpline* spline = fComponentData[component].fSpline;
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", nBins, spline->GetXMin(), spline->GetXMax());
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((int)iBin+1, spline->Evaluate(hist->GetBinCenter(iBin+1)));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Gain Variation");
        return hist;
    }
    TH1D* KTGainVariationData::CreateGainVariationVarianceHistogram(unsigned nBins, unsigned component, const std::string& name) const
    {
        KTSpline* spline = fComponentData[component].fVarianceSpline;
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Variance", nBins, spline->GetXMin(), spline->GetXMax());
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((int)iBin+1, spline->Evaluate(hist->GetBinCenter(iBin+1)));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Gain Variation Variance");
        return hist;
    }
#endif


} /* namespace Katydid */

