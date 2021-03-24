/**
 @file KTViterbi.hh
 @brief Contains KTViterbi
 @details Lazy Viterbi algorithm for track-finding + event building
 @author: N. Buzinsky
 @date: Mar 1, 2021
 */

#ifndef KTVITERBI_HH_
#define KTVITERBI_HH_

#include "KTProcessor.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTDiscriminatedPoint.hh"

#include "KTMemberVariable.hh"
//#include "KTPhysicalArray.hh"
#include "KTSlot.hh"

namespace Katydid
{
    class KTEggHeader;
    class KTPowerSpectrum;
    class KTPowerSpectrumData;
    class KTSliceHeader;

    /*!
     @class KTViterbi
     @author N. Buzinsky

     @brief Implementation of "Lazy" Viterbi Algorithm for post-mortem Phase II track-finding + event building
     A python implementation of this code can be found in the scripts repo in: buzinsky_scripts/Viterbi

     @details
     Finds "most probable" hidden state sequence: https://en.wikipedia.org/wiki/Viterbi_algorithm

     Configuration name: "viterbi"

     Available configuration values:
     - "min-frequency": minimum allowed frequency (has to be set)
     - "max-frequency": max allowed frequency (has to be set)
     - "min-bin": can be set instead of min frequency
     - "max-bin": can be set instead of  max frequency

     Slots:
    */


    class KTViterbi : public Nymph::KTProcessor
    {
        public:
            KTViterbi(const std::string& name = "lazy-viterbi");
            virtual ~KTViterbi();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, NoiseTemperature);

            // Others
            MEMBERVARIABLE(bool, CalculateMinBin);
            MEMBERVARIABLE(bool, CalculateMaxBin);
            MEMBERVARIABLE(unsigned, MinBin);
            MEMBERVARIABLE(unsigned, MaxBin);
            MEMBERVARIABLE(double, FreqBinWidth);
            MEMBERVARIABLE(double, TimeBinWidth);
            MEMBERVARIABLE(double, MinFrequency);
            MEMBERVARIABLE(double, MaxFrequency);
            MEMBERVARIABLE(double, P0);
            MEMBERVARIABLE(double, P1);

            MEMBERVARIABLE(double, NBins);
            MEMBERVARIABLE(double, NStates);
            MEMBERVARIABLE(double, NWindow);

            MEMBERVARIABLE(unsigned, KScatter);
            MEMBERVARIABLE(unsigned, TauTrack);
            MEMBERVARIABLE(unsigned, TauEvent);
            MEMBERVARIABLE(unsigned, EventRate);

            std::vector< std::vector<double> > fT1;
            std::vector< std::vector<double> > fT2;
            std::vector< std::vector<double> > flog_A;

            bool LoopOverHighPowerPoints(KTDiscriminatedPoints1DData::SetOfPoints points, uint64_t acqID, unsigned component);

        public:
            bool InitializeWithHeader(KTEggHeader& header);

            bool CollectDiscrimPointsFromSlice(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discrimPoints);
            bool CollectDiscrimPointsFromSlice(KTSliceHeader& slHeader, KTPowerSpectrumData& spectrum, KTDiscriminatedPoints1DData& discrimPoints);
            bool BacktrackBestPath();
            std::vector<double> GetEmissionVector(std::vector<unsigned> highPowerBins);
            std::pair<unsigned, double> FindBestState(std::vector<unsigned> checkStates, unsigned iState, unsigned iTimeSlice);
            bool MostProbablePreviousState(unsigned iTimeSlice, unsigned iState, bool highPower, double log_B);
            bool InitializeTransitionMatrix();
            std::vector< std::vector<double>> GetScatteringMatrix(double pTrackToTrack, double pNewTrack, unsigned fKScatter);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fLineSignal;
            Nymph::KTSignalOneArg< void > fClusterDoneSignal;

            void AcquisitionIsOver();

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTEggHeader > fHeaderSlot;
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTDiscriminatedPoints1DData > fDiscrimSlot;
            Nymph::KTSlotDone fDoneSlot;

    };

} /* namespace Katydid */
#endif /* KTVITERBI_HH_ */
