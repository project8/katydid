/*
 * KTROOTTreeTypeWriterAnalysis.hh
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITERANALYSIS_HH_
#define KTROOTTREETYPEWRITERANALYSIS_HH_

#include "KTROOTTreeWriter.hh"

#include <boost/shared_ptr.hpp>

class TH1D;
class TTree;

namespace Katydid
{
    class KTData;

    struct TDiscriminatedPoints1DData
    {
        UShort_t fComponent;
        UInt_t fPoint;
        Double_t fValue;
        Double_t fThreshold;
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
            void WriteDiscriminatedPoints1D(boost::shared_ptr< KTData > data);
            void WriteAmplitudeDistributions(boost::shared_ptr< KTData > data);

        public:
            TTree* GetDiscriminatedPoints1DTree() const;
            TTree* GetAmplitudeDistributionTree() const;

        private:
            Bool_t SetupDiscriminatedPoints1DTree();
            Bool_t SetupAmplitudeDistributionTree();

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
