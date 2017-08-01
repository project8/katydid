/*
 * KTScore.cpp
 *
 *  Created on: 08.03.2016
 *      Author: Christine
 */

#include <KTScore.hh>
/*
 * KTConvertToPower.cc
 *
 *  Created on: nsoblath
 *      Author: Aug 1, 2014
 */


#include <iostream>

#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTParam.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTScoredSpectrum.hh"
#include "KTScoredSpectrumData.hh"

using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(sclog, "katydid.fft");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTScore, "scored-spectrum-creator");

    KTScore::KTScore(const std::string& name) :

            KTProcessor(name),
			fMode('Scores'),
			fThreshold(5.0),
			fScoredSpectrumSignal("scores-1d", this),
            fPSSlot("ps", this, &KTScore::ScoreRatio, &fScoredSpectrumSignal),
            fPreCalcSlot("gv", this, &KTScore::SetPreCalcGainVar),
            fPSPreCalcSlot("ps-pre", this, &KTScore::Ratio, &fScoredSpectrumSignal)
    {
    	std::cout << "KTScore constructor"<<std::endl<<std::endl;
    }

    KTScore::~KTScore()
    {
    }

    bool KTScore::Configure(const scarab::param_node* node)
    {
    	if (node == NULL) return true;

    	if (node->has("Mode"))
    	{
    		fMode(node->get_value< double >("Mode"));
    	}
    	if (node->has("Threshold"))
    	{
    		fThreshold(node->get_value< char >("Threshold"));
    	}
    	return true;
    }

    bool KTScore::SetPreCalcGainVar(KTGainVariationData& gvData)
        {
            fGVData = gvData;
            KTINFO(sclog, "KtScore processor received Gain Variation Data ");
            return true;
        }

    bool KTScore::Ratio(KTPowerSpectrumData& data)
    {
        return ScoreRatio(data, fGVData);
        std::cout << "slice angekommen!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }


    bool KTScore::ScoreRatio(KTPowerSpectrumData& data, KTGainVariationData& gvData)
    {
    	std::cout << "alles angekommen!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        unsigned nComponents = data.GetNComponents();


        KTScoredSpectrumData& scData = data.Of< KTScoredSpectrumData >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {


            KTScoredSpectrum* spectrum = KTScoredSpectrum::CreateScoredSpectrum(*(data.GetSpectrum(iComponent)), gvData);

            spectrum->ConvertToScoredSpectrum();
            scData.SetSpectrum(spectrum, iComponent);
        }
        return true;
    }






} /* namespace Katydid */