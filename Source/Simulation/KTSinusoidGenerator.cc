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

    bool KTSinusoidGenerator::ConfigureDerivedGenerator(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        fFrequency = node->GetData< double >("frequency", fFrequency);
        fPhase = node->GetData< double >("phase", fPhase);
        fAmplitude = node->GetData< double >("amplitude", fAmplitude);

        return true;
    }

    bool KTSinusoidGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        const double mult = 2. * KTMath::Pi() * fFrequency;
        const double binWidth = data.GetTimeSeries(0)->GetTimeBinWidth();
        const unsigned sliceSize = data.GetTimeSeries(0)->GetNTimeBins();

        unsigned nComponents = data.GetNComponents();

        for (unsigned iComponent = 0; iComponent < nComponents; iComponent++)
        {
            KTTimeSeries* timeSeries = data.GetTimeSeries(iComponent);

            if (timeSeries == NULL)
            {
                KTERROR(genlog, "Time series " << iComponent << " was not present");
                continue;
            }

            double binCenter = 0.5 * binWidth;
            for (unsigned iBin = 0; iBin < sliceSize; iBin++)
            {
                timeSeries->SetValue(iBin, fAmplitude * sin(binCenter * mult + fPhase) + timeSeries->GetValue(iBin));
                binCenter += binWidth;
                //KTDEBUG(genlog, iBin << "  " << (*timeSeries)(iBin));
            }
        }

        return true;
    }


} /* namespace Katydid */
