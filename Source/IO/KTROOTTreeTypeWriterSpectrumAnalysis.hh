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
    using namespace Nymph;
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

    struct TLinearFitResultData
    {
        UInt_t fComponent;
        Double_t fSlope;
        Double_t fSlopeSigma;
        Double_t fIntercept;
        Double_t fIntercept_deviation;
        Double_t fStartingFrequency;
        Double_t fTrackDuration;
        Double_t fSidebandSeparation;
        Double_t fFineProbe_sigma_1;
        Double_t fFineProbe_sigma_2;
        Double_t fFineProbe_SNR_1;
        Double_t fFineProbe_SNR_2;
        Double_t fFFT_peak;
        Double_t fFFT_SNR;
        Double_t fFit_width;
        UInt_t fNPoints;
        Double_t fProbeWidth;
    };


    class KTROOTTreeTypeWriterSpectrumAnalysis : public KTROOTTreeTypeWriter//, public KTTypeWriterSpectrumAnalysis
    {
        public:
            KTROOTTreeTypeWriterSpectrumAnalysis();
            virtual ~KTROOTTreeTypeWriterSpectrumAnalysis();

            void RegisterSlots();

        public:
            void WriteDiscriminatedPoints1D(KTDataPtr data);
            void WriteKDTree(KTDataPtr data);
            void WriteAmplitudeDistributions(KTDataPtr data);
            void WriteHoughData(KTDataPtr data);
            void WriteLinearFitResultData(KTDataPtr data);

        public:
            TTree* GetDiscriminatedPoints1DTree() const;
            TTree* GetKDTreeTree() const;
            TTree* GetAmplitudeDistributionTree() const;
            TTree* GetHoughTree() const;
            TTree* GetLinearFitResultTree() const;

        private:
            bool SetupDiscriminatedPoints1DTree();
            bool SetupKDTreeTree();
            bool SetupAmplitudeDistributionTree();
            bool SetupHoughTree();
            bool SetupLinearFitResultTree();

            TTree* fDiscPoints1DTree;
            TTree* fKDTreeTree;
            TTree* fAmpDistTree;
            TTree* fHoughTree;
            TTree* fLinearFitResultTree;

            TDiscriminatedPoints1DData fDiscPoints1DData;
            TKDTreePointData fKDTreePointData;
            TAmplitudeDistributionData fAmpDistData;
            THoughData fHoughData;
            TLinearFitResultData fLineFitData;

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

    inline TTree* KTROOTTreeTypeWriterSpectrumAnalysis::GetLinearFitResultTree() const
    {
        return fLinearFitResultTree;
    }

} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERSPECTRUMANALYSIS_HH_ */
