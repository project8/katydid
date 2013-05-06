/*
 * KTSinusoidGenerator.cc
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#include "KTSinusoidGenerator.hh"

#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTMath.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "katydid.simulation");

    static KTDerivedNORegistrar< KTProcessor, KTSinusoidGenerator > sSinusoidGenRegistrar("sinusoid-generator");

    KTSinusoidGenerator::KTSinusoidGenerator(const string& name) :
            KTTSGenerator(name),
            fFrequency(1.),
            fPhase(0.),
            fAmplitude(1.)
    {
    }

    KTSinusoidGenerator::~KTSinusoidGenerator()
    {
    }

    Bool_t KTSinusoidGenerator::ConfigureDerivedGenerator(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        fFrequency = node->GetData< Double_t >("frequency", fFrequency);
        fPhase = node->GetData< Double_t >("phase", fPhase);
        fAmplitude = node->GetData< Double_t >("amplitude", fAmplitude);

        return true;
    }

    Bool_t KTSinusoidGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        const Double_t mult = 2. * KTMath::Pi() * fFrequency;
        const Double_t binWidth = data.GetTimeSeries(0)->GetTimeBinWidth();
        const UInt_t sliceSize = data.GetTimeSeries(0)->GetNTimeBins();

        UInt_t nComponents = data.GetNComponents();

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            KTTimeSeries* timeSeries = data.GetTimeSeries(iComponent);

            if (timeSeries == NULL)
            {
                KTERROR(genlog, "Time series " << iComponent << " was not present");
                continue;
            }

            Double_t binCenter = 0.5 * binWidth;
            for (UInt_t iBin = 0; iBin < sliceSize; iBin++)
            {
                timeSeries->SetValue(iBin, fAmplitude * sin(binCenter * mult + fPhase));
                binCenter += binWidth;
                //KTDEBUG(genlog, iBin << "  " << (*timeSeries)(iBin));
            }
        }

        return true;
    }


} /* namespace Katydid */
