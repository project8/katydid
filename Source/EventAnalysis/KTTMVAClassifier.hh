/*
 * KTTMVAClassifier.hh
 *
 *  Created on: Dec 13, 2016
 *      Author: ezayas
 */

#ifndef KTTMVACLASSIFIER_HH_
#define KTTMVACLASSIFIER_HH_

#include "KTProcessor.hh"
#include "KTData.hh"

#include "KTSlot.hh"

#include "KTProcessedTrackData.hh"

#include "TMVA/Reader.h"

namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTTMVAClassifier.hh");

    /*
     @class KTTMVAClassifier
     @author E. Zayas
     @brief Conatins TMVA classifier
     @details
     Reads the output of a machine-learning training algorithm to assign a classifier value to a track. The file to read is generated by TMVA and must exist at runtime.

     Available configuration values:
     - "mva-file": std::string -- location of the XML file produced by TMVA to read
     - "algortihm": std::string -- machine-learning algorithm used to generated the XML file
     - "mva-cut": double -- threshold of the classifier value to label events as a signal

     Slots:
     - "power-fit": void (Nymph::KTDataPtr) -- Performs MVA analysis with the rotated-and-projected parameters; Requires KTProcessedTrackData and KTPowerFitData; Adds nothing

     Signals:
     - "classify": void (Nymph::KTDataPtr) -- Emitted upon successful classification; Guarantees KTProcessedTrackData and KTPowerFitData
    */

    class KTTMVAClassifier : public Nymph::KTProcessor
    {
        public:
            KTTMVAClassifier(const std::string& name = "tmva-classifier");
            virtual ~KTTMVAClassifier();

            bool Configure(const scarab::param_node* node);

            std::string GetMVAFile() const;
            void SetMVAFile(std::string fileName);

            std::string GetAlgorithm() const;
            void SetAlgorithm(std::string alg);

            double GetMVACut() const;
            void SetMVACut(double value);

        private:
            std::string fMVAFile;
            std::string fAlgorithm;
            double fMVACut;

            // MVA Reader
            TMVA::Reader* reader;

            // Variables for power-fit slot
            float fAverage;
            float fRMS;
            float fSkewness;
            float fKurtosis;
            float fNormCentral;
            float fMeanCentral;
            float fSigmaCentral;
            float fNPeaks;
            float fCentralPowerFraction;
            float fRMSAwayFromCentral;

        public:
            bool ClassifyTrack( KTProcessedTrackData& trackData, double mva );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fClassifySignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionPowerFitData( Nymph::KTDataPtr data );

    };

    inline std::string KTTMVAClassifier::GetMVAFile() const
    {
        return fMVAFile;
    }

    inline void KTTMVAClassifier::SetMVAFile(std::string fileName)
    {
        fMVAFile = fileName;
        return;
    }

    inline std::string KTTMVAClassifier::GetAlgorithm() const
    {
        return fAlgorithm;
    }

    inline void KTTMVAClassifier::SetAlgorithm(std::string alg)
    {
        fAlgorithm = alg;
        return;
    }

    inline double KTTMVAClassifier::GetMVACut() const
    {
        return fMVACut;
    }

    inline void KTTMVAClassifier::SetMVACut(double value)
    {
        fMVACut = value;
        return;
    }
}

#endif /* KTTMVACLASSIFIER_HH_ */
