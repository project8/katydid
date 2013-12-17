/*
 * KTHoughData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTHoughData.hh"

#include "KTLogger.hh"

using std::vector;

namespace Katydid
{
    KTLOGGER(htlog, "katydid.analysis");

    KTHoughData::KTHoughData() :
            KTExtensibleData< KTHoughData >(),
            fTransforms()
    {
    }

    KTHoughData::~KTHoughData()
    {
        while (! fTransforms.empty())
        {
            KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* backTransform = fTransforms.back();
            for (KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >::iterator iter = backTransform->begin(); iter != backTransform->end(); iter++)
            {
                delete *iter;
            }
            delete backTransform;
            fTransforms.pop_back();
        }
    }

    KTHoughData& KTHoughData::SetNComponents(UInt_t components)
    {
        UInt_t oldSize = fTransforms.size();
        // if components < oldSize
        for (UInt_t iComponent = components; iComponent < oldSize; iComponent++)
        {
            for (UInt_t iTransform = 0; iTransform < fTransforms.size(); iTransform++)
            {
                delete (*fTransforms[iComponent])(iTransform);
            }
        }
        fTransforms.resize(components);
        // if components > oldSize
        for (UInt_t iComponent = oldSize; iComponent < components; iComponent++)
        {
            fTransforms[iComponent] = NULL;
        }
        return *this;
    }


#ifdef ROOT_FOUND

    TH2D* KTHoughData::CreateHistogram(UInt_t component, const std::string& name) const
    {
        if (component >= fTransforms.size()) return NULL;
        if (fTransforms[component]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Hough Space",
                fTransforms[component]->size(), fTransforms[component]->GetRangeMin(), fTransforms[component]->GetRangeMax(),
                (*fTransforms[component])(0)->size(), (*fTransforms[component])(0)->GetRangeMin(), (*fTransforms[component])(0)->GetRangeMax());

        KTINFO(htlog, "Radius axis: " << (*fTransforms[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax());
        KTINFO(htlog, "Angle axis: " << fTransforms[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax());

        for (Int_t iBinX=1; iBinX<=(Int_t)fTransforms[component]->size(); iBinX++)
        {
            KTPhysicalArray< 1, double >* fs = (*fTransforms[component])(iBinX-1);
            for (Int_t iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*fs)(iBinY-1));
            }
        }

        hist->SetXTitle("Angle");
        hist->SetYTitle("Radius");
        return hist;
    }

 #endif


} /* namespace Katydid */

