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
    KTLOGGER(htlog, "KTHoughData");

    KTHoughData::KTHoughData() :
            KTExtensibleData< KTHoughData >(),
            fTransforms()
    {
    }

    KTHoughData::~KTHoughData()
    {
        while (! fTransforms.empty())
        {
            delete fTransforms.back().fTransform;
            fTransforms.pop_back();
        }
    }

    KTHoughData& KTHoughData::SetNComponents(unsigned components)
    {
        unsigned oldSize = fTransforms.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; iComponent++)
        {
            delete fTransforms[iComponent].fTransform;
        }
        fTransforms.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; iComponent++)
        {
            fTransforms[iComponent].fTransform = NULL;
        }
        return *this;
    }


#ifdef ROOT_FOUND

    TH2D* KTHoughData::CreateHistogram(unsigned component, const std::string& name) const
    {
        if (component >= fTransforms.size()) return NULL;
        if (fTransforms[component].fTransform->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Hough Space",
                fTransforms[component].fTransform->size(1), fTransforms[component].fTransform->GetRangeMin(1), fTransforms[component].fTransform->GetRangeMax(1),
                fTransforms[component].fTransform->size(2), fTransforms[component].fTransform->GetRangeMin(2), fTransforms[component].fTransform->GetRangeMax(2));

        KTINFO(htlog, "Radius axis: " << fTransforms[component].fTransform->size(2) << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax());
        KTINFO(htlog, "Angle axis: " << fTransforms[component].fTransform->size(1) << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax());

        for (int iBinX=1; iBinX<=(int)fTransforms[component].fTransform->size(1); iBinX++)
        {
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*fTransforms[component].fTransform)(iBinX-1, iBinY-1));
            }
        }

        hist->SetXTitle("Angle");
        hist->SetYTitle("Radius");
        return hist;
    }

 #endif


} /* namespace Katydid */

