/*
 * KTDLIBClassifier.hh
 *
 * Created on: Feb 18, 2018
 *     Author: ezayas, L. Saldana 
 */

#ifndef KTDLIBCLASSIFIER_HH_
#define KTDLIBCLASSIFIER_HH_

#include "KTProcessor.hh"
#include "KTData.hh"

#include "KTProcessedTrackData.hh"
#include "KTPowerFitData.hh"
#include "KTClassifierResultsData.hh"

#include "KTSlot.hh"
#include "logger.hh"

// Undefine to avoid conflict between dlib and scarab logger macros
#undef LINFO
#undef LPROG
#undef LWARN
#undef LERROR
#undef LFATAL
#undef LASSERT
#undef LTRACE
#undef LDEBUG

// dlib stuff
#include <dlib/matrix.h>
#include <dlib/svm_threaded.h>
#include <dlib/svm.h>

namespace Katydid
{
    
    LOGGER(avlog_hh, "KTDLIBClassifier.hh");


    /*
     @class KTDLIBClassifier
     @author E. Zayas, L. Saldana
     @brief Contains DLIB classifier
     @details
     Reads in a decision function from a trained ML algorithm and uses this to assign a classifier value to a track. The file to read is generated by the dlib c++ library 
     and exists at runtime.   
    
     Available configuration values:
     - "df-file": std::string -- location of the dat file produced by dlib containing decision function to read
    
     Slots:
     - "power-fit": void (Nymph::KTDataPtr) -- Performs SVM classification with all classifier features (power, slope, time length, and rotate-project parameters); Requires KTProcessedTrackData and KTPowerFitData; Adds KTClassifierResultsData
    
     Signals:
     - "classified": void (Nymph::KTDataPtr) -- Emitted upon successful classification; Guarantees KTProcessedTrackData, KTPowerFitData and KTClassifierResultsData
    */

    class KTDLIBClassifier : public Nymph::KTProcessor
    {
        public:
            KTDLIBClassifier(const std::string& name = "dlib-classifier");
            virtual ~KTDLIBClassifier();

            bool Configure(const scarab::param_node* node);

            std::string GetDFFile() const;
            void SetDFFile(std::string fileName);

        private:
            std::string fDFFile;
            // Some helpful type definitions from dlib
            typedef dlib::matrix<double,14,1> sample_type;
            typedef dlib::one_vs_all_trainer<dlib::any_trainer< sample_type, double > > ova_trainer;
            typedef dlib::radial_basis_kernel<sample_type> rbf_kernel;
            typedef dlib::one_vs_all_decision_function<ova_trainer,dlib::decision_function<rbf_kernel>> decision_funct_type;
            typedef dlib::normalized_function<decision_funct_type> normalized_decision_funct_type;

            normalized_decision_funct_type fDecisionFunction;
            bool fIsInitialized;

        public:
            bool Initialize();
            bool ClassifyTrack( KTProcessedTrackData& ptData, KTPowerFitData& pfData );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fClassifiedSignal;

            //***************
            // Slots
            //***************
            Nymph::KTSlotDataTwoTypes< KTProcessedTrackData, KTPowerFitData > fPowerFitSlot;

    };

    inline std::string KTDLIBClassifier::GetDFFile() const
    {
        return fDFFile;
    }

    inline void KTDLIBClassifier::SetDFFile(std::string fileName)
    {
        fDFFile = fileName;
        return;
    }

}

#endif /* KTDLIBCLASSIFIER_HH_ */
