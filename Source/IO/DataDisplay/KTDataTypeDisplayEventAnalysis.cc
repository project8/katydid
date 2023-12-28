/*
 * KTDataTypeDisplayEventAnalysis.cc
 *
 *  Created on: Oct 3, 2016
 *      Author: ezayas
 */

#include "KT2ROOT.hh"
#include "logger.hh"
#include "KTSliceHeader.hh"
#include "KTTIFactory.hh"
#include "KTSpectrumCollectionData.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerFitData.hh"

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"

#include <sstream>
#include <map>
#include "KTDataTypeDisplayEventAnalysis.hh"



using std::stringstream;
using std::string;

namespace Katydid
{
    LOGGER(publog, "katydid.output");


    static Nymph::KTTIRegistrar< KTDataTypeDisplay, KTDataTypeDisplayEventAnalysis > sBRTWAnalysisRegistrar;

    KTDataTypeDisplayEventAnalysis::KTDataTypeDisplayEventAnalysis() :
            KTDataTypeDisplay()
    {
    }

    KTDataTypeDisplayEventAnalysis::~KTDataTypeDisplayEventAnalysis()
    {
    }


    void KTDataTypeDisplayEventAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("ps-coll", this, &KTDataTypeDisplayEventAnalysis::DrawPSCollectionData);
        fWriter->RegisterSlot("power-fit", this, &KTDataTypeDisplayEventAnalysis::DrawPowerFitData);
        return;
    }

    //**************************
    // Spectrum Collection Data
    //**************************

    void KTDataTypeDisplayEventAnalysis::DrawPSCollectionData(Nymph::KTDataPtr data)
    {

        //std::cout << "This is a test!" << std::endl;
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTPSCollectionData& psColl = data->Of<KTPSCollectionData>();

        if (! fWriter->OpenWindow()) return;

        KTPhysicalArray< 1, KTPowerSpectrum* > spectra = *psColl.GetSpectra();

        //LDEBUG(publog, "2");
        if (!spectra.empty())
        {
            stringstream conv;
            conv << "histPSColl_" << sliceNumber;
            string histName;
            conv >> histName;
            for( unsigned i=0; i < psColl.GetNComponents(); ++i)
            {
                TH2D* psCollection = psColl.CreatePowerHistogram(i, histName);
                fWriter->Draw(psCollection);
            }
        }

        return;
    }

    //****************
    // Power Fit Data
    //****************

    void KTDataTypeDisplayEventAnalysis::DrawPowerFitData(Nymph::KTDataPtr data)
    {
        if( !data ) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTPowerFitData& pfData = data->Of<KTPowerFitData>();

        if (! fWriter->OpenWindow()) return;

        const std::map< unsigned, KTPowerFitData::Point > points = pfData.GetSetOfPoints();
        Int_t nPoints = points.size();

        Double_t x[nPoints], y[nPoints];
        int i = 0;
        for( std::map< unsigned, KTPowerFitData::Point >::const_iterator it = points.begin(); it != points.end(); ++it )
        {
            x[i] = it->second.fAbscissa;
            y[i] = it->second.fOrdinate;
            ++i;
        }

        TGraph* g = new TGraph( nPoints, x, y );
        fWriter->Draw(g);

        return;
    }

} /* namespace Katydid */
