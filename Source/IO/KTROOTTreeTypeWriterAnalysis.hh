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
        Double_t fTimeInRunC; // bin center on the time axis
        UInt_t fComponent;
        UInt_t fBin;
        Double_t fAbscissa;
        Double_t fOrdinate;
        Double_t fThreshold;
    };

    struct TKDTreePointData
    {
        UInt_t fComponent;
        ULong64_t fSlice;
        Double_t fTimeInRunC;
        Double_t fFrequency;
        Double_t fAmplitude;
        Bool_t fNoiseFlag;
        Double_t fNNDistance;
        UInt_t fKNNWithin0p22;
        UInt_t fKNNWithin0p32;
        UInt_t fKNNWithin0p45;
        UInt_t fKNNWithin0p7;
        UInt_t fKNNWithin1p0;
        UInt_t fKNNWithin1p4;
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
        Double_t fXOffset;
        Double_t fXScale;
        Double_t fYOffset;
        Double_t fYScale;
    };


    class KTROOTTreeTypeWriterAnalysis : public KTROOTTreeTypeWriter//, public KTTypeWriterAnalysis
    {
        public:
            KTROOTTreeTypeWriterAnalysis();
            virtual ~KTROOTTreeTypeWriterAnalysis();

            void RegisterSlots();

        public:
            void WriteDiscriminatedPoints1D(KTDataPtr data);
            void WriteKDTree(KTDataPtr data);
            void WriteAmplitudeDistributions(KTDataPtr data);
            void WriteHoughData(KTDataPtr data);

        public:
            TTree* GetDiscriminatedPoints1DTree() const;
            TTree* GetKDTreeTree() const;
            TTree* GetAmplitudeDistributionTree() const;
            TTree* GetHoughTree() const;

        private:
            bool SetupDiscriminatedPoints1DTree();
            bool SetupKDTreeTree();
            bool SetupAmplitudeDistributionTree();
            bool SetupHoughTree();

            TTree* fDiscPoints1DTree;
            TTree* fKDTreeTree;
            TTree* fAmpDistTree;
            TTree* fHoughTree;

            TDiscriminatedPoints1DData fDiscPoints1DData;
            TKDTreePointData fKDTreePointData;
            TAmplitudeDistributionData fAmpDistData;
            THoughData fHoughData;

    };

    inline TTree* KTROOTTreeTypeWriterAnalysis::GetDiscriminatedPoints1DTree() const
    {
        return fDiscPoints1DTree;
    }

    inline TTree* KTROOTTreeTypeWriterAnalysis::GetKDTreeTree() const
    {
        return fKDTreeTree;
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
