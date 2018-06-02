/*
 * KTROOTTreeTypeWriterSpectrumAnalysis.hh
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITERSPECTRUMANALYSIS_HH_
#define KTROOTTREETYPEWRITERSPECTRUMANALYSIS_HH_

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
        Double_t fMean; //<-
        Double_t fVariance; //<-
        Double_t fNeighborhoodAmplitude; //<-
    };

    struct TKDTreePointData
    {
        UInt_t fComponent;
        ULong64_t fSlice;
        Double_t fTimeInRunC;
        Double_t fFrequency;
        Double_t fAmplitude;
        Double_t fMean; //<-
        Double_t fVariance; //<-
        Double_t fNeighborhoodAmplitude; //<-
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


    class KTROOTTreeTypeWriterSpectrumAnalysis : public KTROOTTreeTypeWriter//, public KTTypeWriterSpectrumAnalysis
    {
        public:
            KTROOTTreeTypeWriterSpectrumAnalysis();
            virtual ~KTROOTTreeTypeWriterSpectrumAnalysis();

            void RegisterSlots();

        public:
            void WriteDiscriminatedPoints1D(Nymph::KTDataPtr data);
            void WriteKDTree(Nymph::KTDataPtr data);
            void WriteAmplitudeDistributions(Nymph::KTDataPtr data);
            void WriteHoughData(Nymph::KTDataPtr data);

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

    inline TTree* KTROOTTreeTypeWriterSpectrumAnalysis::GetDiscriminatedPoints1DTree() const
    {
        return fDiscPoints1DTree;
    }

    inline TTree* KTROOTTreeTypeWriterSpectrumAnalysis::GetKDTreeTree() const
    {
        return fKDTreeTree;
    }

    inline TTree* KTROOTTreeTypeWriterSpectrumAnalysis::GetAmplitudeDistributionTree() const
    {
        return fAmpDistTree;
    }

    inline TTree* KTROOTTreeTypeWriterSpectrumAnalysis::GetHoughTree() const
    {
        return fHoughTree;
    }


} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERSPECTRUMANALYSIS_HH_ */
