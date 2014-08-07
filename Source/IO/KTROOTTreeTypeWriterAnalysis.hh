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
class TH2D;
class TTree;

namespace Katydid
{
    struct TDiscriminatedPoints1DData
    {
        UInt_t fSlice;
        double fTimeInRunC; // bin center on the time axis
        UInt_t fComponent;
        UInt_t fBin;
        double fAbscissa;
        double fOrdinate;
        double fThreshold;
    };

    struct TAmplitudeDistributionData
    {
        UInt_t fComponent;
        UInt_t fFreqBin;
        TH1D* fDistribution;
    };

    struct THoughData
    {
        UInt_t fComponent;
        TH2D* fTransform;
        double fXOffset;
        double fXScale;
        double fYOffset;
        double fYScale;
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
            void WriteHoughData(KTDataPtr data);

        public:
            TTree* GetDiscriminatedPoints1DTree() const;
            TTree* GetAmplitudeDistributionTree() const;
            TTree* GetHoughTree() const;

        private:
            bool SetupDiscriminatedPoints1DTree();
            bool SetupAmplitudeDistributionTree();
            bool SetupHoughTree();

            TTree* fDiscPoints1DTree;
            TTree* fAmpDistTree;
            TTree* fHoughTree;

            TDiscriminatedPoints1DData fDiscPoints1DData;
            TAmplitudeDistributionData fAmpDistData;
            THoughData fHoughData;

    };

    inline TTree* KTROOTTreeTypeWriterAnalysis::GetDiscriminatedPoints1DTree() const
    {
        return fDiscPoints1DTree;
    }

    inline TTree* KTROOTTreeTypeWriterAnalysis::GetAmplitudeDistributionTree() const
    {
        return fAmpDistTree;
    }

    inline TTree* KTROOTTreeTypeWriterAnalysis::GetHoughTree() const
    {
        return fHoughTree;
    }

} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERANALYSIS_HH_ */
