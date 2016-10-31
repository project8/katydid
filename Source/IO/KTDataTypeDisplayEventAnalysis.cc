/*
 * KTDataTypeDisplayEventAnalysis.cc
 *
 *  Created on: Oct 3, 2016
 *      Author: ezayas
 */

#include "KT2ROOT.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTIFactory.hh"
#include "KTSpectrumCollectionData.hh"
#include "KTPowerSpectrum.hh"

#include "TH1.h"
#include "TH2.h"

#include <sstream>
#include <map>
#include "KTDataTypeDisplayEventAnalysis.hh"



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


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
        fWriter->RegisterSlot("psColl-data", this, &KTDataTypeDisplayEventAnalysis::DrawPSCollectionData);
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

        const std::map< double, KTPowerSpectrum* > spectra = psColl.GetSpectra();

        //KTDEBUG(publog, "2");
        if (!spectra.empty())
        {
            stringstream conv;
            conv << "histPSColl_" << sliceNumber;
            string histName;
            conv >> histName;
            TH2D* psCollection = KT2ROOT::CreatePowerHistogram(spectra, histName);
            fWriter->Draw(psCollection);
        }

        return;
    }

} /* namespace Katydid */
