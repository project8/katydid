/*
 * KTHoughData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTHoughData.hh"

#include "KTDataMap.hh"
#include "KTTIFactory.hh"
#include "KTWriter.hh"

using std::vector;

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTHoughData > > sHDMRegistrar;

    KTHoughData::KTHoughData(unsigned nTransforms) :
            KTWriteableData(),
            fTransforms(nTransforms)
    {
    }

    KTHoughData::~KTHoughData()
    {
        while (! fTransforms.empty())
        {
            KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* backTransform = fTransforms.back();
            for (KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >::iterator iter = backTransform->begin(); iter != backTransform->end(); iter++)
            {
                delete *iter;
            }
            delete backTransform;
            fTransforms.pop_back();
        }
    }

    void KTHoughData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }

#ifdef ROOT_FOUND

    TH2D* KTHoughData::CreateHistogram(unsigned transformNum, const std::string& name) const
    {
        if (transformNum >= fTransforms.size()) return NULL;
        if (fTransforms[transformNum]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Hough Space",
                fTransforms[transformNum]->size(), fTransforms[transformNum]->GetRangeMin(), fTransforms[transformNum]->GetRangeMax(),
                (*fTransforms[transformNum])(0)->size(), (*fTransforms[transformNum])(0)->GetRangeMin(), (*fTransforms[transformNum])(0)->GetRangeMax());

        KTINFO("Radius axis: " << (*fTransforms[transformNum])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax());
        KTINFO("Angle axis: " << fTransforms[transformNum]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax());

        for (Int_t iBinX=1; iBinX<=(Int_t)fTransforms[transformNum]->size(); iBinX++)
        {
            KTPhysicalArray< 1, Double_t >* fs = (*fTransforms[transformNum])(iBinX-1);
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

