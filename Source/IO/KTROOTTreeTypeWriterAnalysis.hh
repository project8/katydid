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
            void WriteAmplitudeDistributions(boost::shared_ptr< KTData > data);

        public:
            TTree* GetAmplitudeDistributionTree() const;

        private:
            Bool_t SetupAmplitudeDistributionTree();

            TTree* fAmpDistTree;

            TAmplitudeDistributionData fAmpDistData;

    };

    inline TTree* KTROOTTreeTypeWriterAnalysis::GetAmplitudeDistributionTree() const
    {
        return fAmpDistTree;
    }

} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERANALYSIS_HH_ */
