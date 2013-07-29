/*
 * KTROOTTreeTypeWriterAnalysis.hh
 *
 *  Created on: Jul 24, 2013
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITERANALYSIS_HH_
#define KTROOTTREETYPEWRITERANALYSIS_HH_

#include "KTROOTTreeWriter.hh"

#include <boost/shared_ptr.hpp>

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

    class KTROOTTreeTypeWriterAnalysis : public KTROOTTreeTypeWriter//, public KTTypeWriterAnalysis
    {
        public:
            KTROOTTreeTypeWriterAnalysis();
            virtual ~KTROOTTreeTypeWriterAnalysis();

            void RegisterSlots();

        public:
            void WriteDiscriminatedPoints1D(boost::shared_ptr< KTData > data);

        public:
            TTree* GetDiscriminatedPoints1DTree() const;

        private:
            Bool_t SetupDiscriminatedPoints1DTree();

            TTree* fDiscPoints1DTree;

            TDiscriminatedPoints1DData fDiscPoints1DData;

    };

    inline TTree* KTROOTTreeTypeWriterAnalysis::GetDiscriminatedPoints1DTree() const
    {
        return fDiscPoints1DTree;
    }

} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERANALYSIS_HH_ */
