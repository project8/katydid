/*
 * KTSinusoidGenerator.cc
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#include "KTSinusoidGenerator.hh"

#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "param.hh"
#include "KTMath.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "KTSinusoidGenerator");

    KT_REGISTER_PROCESSOR(KTSinusoidGenerator, "sinusoid-generator");

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

    bool KTSinusoidGenerator::ConfigureDerivedGenerator(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        fFrequency = node->get_value< double >("frequency", fFrequency);
        fPhase = node->get_value< double >("phase", fPhase);
        fAmplitude = node->get_value< double >("amplitude", fAmplitude);

        return true;
    }

    bool KTSinusoidGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        const double mult = 2. * KTMath::Pi() * fFrequency;
        const double binWidth = data.GetTimeSeries(0)->GetTimeBinWidth();
        const unsigned sliceSize = data.GetTimeSeries(0)->GetNTimeBins();

        unsigned nComponents = data.GetNComponents();

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
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
