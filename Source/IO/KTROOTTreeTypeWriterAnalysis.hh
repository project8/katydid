/*
 * KTROOTTreeTypeWriterAnalysis.hh
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITERANALYSIS_HH_
#define KTROOTTREETYPEWRITERANALYSIS_HH_

#include "KTROOTTreeWriter.hh"

#include "KTData.hh"

#include "Rtypes.h"

class TH1D;
class TTree;

namespace Katydid
{
    struct TDiscriminatedPoints1DData
    {
        UInt_t fSlice;
        double fTimeInRun;
        UShort_t fComponent;
        UInt_t fBin;
        double fAbscissa;
        double fOrdinate;
        double fThreshold;
    };

    struct TAmplitudeDistributionData
    {
        UShort_t fComponent;
        UInt_t fFreqBin;
        TH1D* fDistribution;
    };


    class KTROOTTreeTypeWriterAnalysis : public KTROOTTreeTypeWriter//, public KTTypeWriterAnalysis
    {
        public:
            KTROOTTreeTypeWriterAnalysis();
            virtual ~KTROOTTreeTypeWriterAnalysis();

            void RegisterSlots();

        public:
            void WriteDiscriminatedPoints1D(KTDataPtr data);
            void WriteAmplitudeDistributions(KTDataPtr data);

        public:
            TTree* GetDiscriminatedPoints1DTree() const;
            TTree* GetAmplitudeDistributionTree() const;

        private:
            bool SetupDiscriminatedPoints1DTree();
            bool SetupAmplitudeDistributionTree();

            TTree* fDiscPoints1DTree;
            TTree* fAmpDistTree;

            TDiscriminatedPoints1DData fDiscPoints1DData;
            TAmplitudeDistributionData fAmpDistData;

    };

    inline TTree* KTROOTTreeTypeWriterAnalysis::GetDiscriminatedPoints1DTree() const
    {
        return fDiscPoints1DTree;
    }

    inline TTree* KTROOTTreeTypeWriterAnalysis::GetAmplitudeDistributionTree() const
    {
        return fAmpDistTree;
    }

} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERANALYSIS_HH_ */
