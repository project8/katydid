/**
 @file KTFFTEHunt.hh
 @brief Contains KTFFTEHunt
 @details Performs the FFT-based electron hunt
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTFFTEHUNT_HH_
#define KTFFTEHUNT_HH_

#include "KTProcessor.hh"
#include "KTConfigurable.hh"

#include "KTSimpleFFT.hh"
#include "KTSlidingWindowFFT.hh"
#include "KTGainNormalization.hh"
#include "KTSimpleClustering.hh"

#include "TFile.h"

#include <fstream>
#include <list>
#include <map>
#include <utility>
#include <vector>

namespace Katydid
{
    class KTPStoreNode;
    class KTEggHeader;
    class KTTimeSeriesData;

    /*!
     @class KTFFTEHunt
     @author N. S. Oblath

     @brief Performs an FFT-based electron hunt.

     @details
     Uses a windows FFT of Egg events to search for clusters of high-peaked bins moving up in frequency.

     Available configuration values:
     \li \c output_filename_base --
     \li \c threshold_multiplier --
     \li \c write_text_file --
     \li \c draw_waterfall --
     \li \c group_bins_margin_high --
     \li \c group_bins_margin_low --
     \li \c group_bins_margin_same_time --

    */

    class KTFFTEHunt : public KTProcessor, public KTConfigurable
    {
        private:
            typedef std::list< std::multimap< Int_t, Int_t >* > EventPeakBinsList;
            typedef std::pair< Double_t, Double_t > CutRange;

        public:
            KTFFTEHunt();
            virtual ~KTFFTEHunt();

            Bool_t Configure(const KTPStoreNode* node);

            void ProcessHeader(const KTEggHeader* header);

            void ProcessEvent(UInt_t iEvent, const KTTimeSeriesData* tsData);

            void FinishHunt();

        private:
            void EmptyEventPeakBins();

        public:
            const std::string& GetTextFilename() const;
            const std::string& GetROOTFilename() const;
            Bool_t GetWriteTextFileFlag() const;
            Bool_t GetWriteROOTFileFlag() const;

            Double_t GetFrequencyMultiplier() const;
            Int_t GetTotalCandidates() const;

            void SetTextFilename(const std::string& name);
            void SetROOTFilename(const std::string& name);
            void SetWriteTextFileFlag(Bool_t flag);
            void SetWriteROOTFileFlag(Bool_t flag);

            void SetFrequencyMultiplier(Double_t mult);

        private:
            EventPeakBinsList fEventPeakBins;

            UInt_t fMinimumGroupSize;

            std::vector< CutRange > fCutRanges;

            KTSimpleFFT fSimpleFFT;
            KTSlidingWindowFFT fWindowFFT;
            KTGainNormalization fGainNorm;
            KTSimpleClustering fClustering;

            std::string fTextFilename;
            std::string fROOTFilename;
            Bool_t fWriteTextFileFlag;
            Bool_t fWriteROOTFileFlag;
            std::ofstream fTextFile;
            TFile fROOTFile;

            Double_t fFrequencyMultiplier;
            Int_t fTotalCandidates;

    };


    const std::string& KTFFTEHunt::GetTextFilename() const
    {
        return fTextFilename;
    }

    const std::string& KTFFTEHunt::GetROOTFilename() const
    {
        return fROOTFilename;
    }

    Bool_t KTFFTEHunt::GetWriteTextFileFlag() const
    {
        return fWriteTextFileFlag;
    }

    Bool_t KTFFTEHunt::GetWriteROOTFileFlag() const
    {
        return fWriteROOTFileFlag;
    }


    Double_t KTFFTEHunt::GetFrequencyMultiplier() const
    {
        return fFrequencyMultiplier;
    }

    Int_t KTFFTEHunt::GetTotalCandidates() const
    {
        return fTotalCandidates;
    }


    void KTFFTEHunt::SetTextFilename(const std::string& name)
    {
        fTextFilename = name;
        return;
    }

    void KTFFTEHunt::SetROOTFilename(const std::string& name)
    {
        fROOTFilename = name;
        return;
    }

    void KTFFTEHunt::SetWriteTextFileFlag(Bool_t flag)
    {
        fWriteTextFileFlag = flag;
        return;
    }

    void KTFFTEHunt::SetWriteROOTFileFlag(Bool_t flag)
    {
        fWriteROOTFileFlag = flag;
        return;
    }


    void KTFFTEHunt::SetFrequencyMultiplier(Double_t mult)
    {
        fFrequencyMultiplier = mult;
        return;
    }



} /* namespace Katydid */
#endif /* KTFFTEHUNT_HH_ */
