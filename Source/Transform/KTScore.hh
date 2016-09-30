/*
 * KTScore.h
 *
 *  Created on: 08.03.2016
 *      Author: Christine
 */

#ifndef KTSCORE_HH_
#define KTSCORE_HH_
/**
 @file KTConvertToPower.hh
 @brief Contains KTConvertToPower
 @details Converts frequency spectra to power spectra and power spectral densities
 @author: nsoblath
 @date: Aug 1, 2014
 */


#include "KTProcessor.hh"
#include "KTSlot.hh"
#include "KTGainVariationData.hh"



namespace Nymph
{
    class KTParamNode;
}
;

namespace Katydid
{
    using namespace Nymph;

    class KTPowerSpectrumData;
    class KTScoredSpectrum;

    /*!


     Slots:

    */

    class KTScore : public KTProcessor
    {

        public:
            KTScore(const std::string& name = "scored-spectrum-creator");
            virtual ~KTScore();

            bool Configure(const KTParamNode* node);


        private:
            KTGainVariationData fGVData;

            char fMode;
            double fThreshold;

        public:
            bool Configure(const scarab::param_node* node);
            bool SetPreCalcGainVar(KTGainVariationData& gvData);
            bool Ratio(KTPowerSpectrumData& data);
            bool ScoreRatio(KTPowerSpectrumData& data, KTGainVariationData& gvData);

        private:

            //***************
            // Signals
            //***************

        private:
            KTSignalData fScoredSpectrumSignal;


            //***************
            // Slots
            //***************

        private:

            KTSlotDataTwoTypes< KTPowerSpectrumData, KTGainVariationData > fPSSlot;

            KTSlotDataOneType< KTGainVariationData > fPreCalcSlot;

            KTSlotDataOneType< KTPowerSpectrumData > fPSPreCalcSlot;

    };
}
 /* namespace Katydid */



#endif /* KTSCORE_H_ */
