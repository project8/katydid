/**
 @file KTMatrixAggregator.hh
 @brief Contains KTMatrixAggregator
 @details Aggregates data from several egg files and components to a single matrix
 @author: F. Thomas
 @date: Apr 29, 2021
 */

#ifndef KTMATRIXAGGREGATOR_HH_
#define KTMATRIXAGGREGATOR_HH_

#include "KTProcessor.hh"

#include "KTMemberVariable.hh"
#include "KTSlot.hh"

#include "KTAggregatedTSMatrixData.hh"

#include <cmath>
#include <eigen3/Eigen/Dense>

namespace scarab
{
    class param_node;
}

namespace Katydid
{

	class KTTimeSeriesData;

    /*!
     @class KTMatrixAggregator
     @author F. Thomas

     @brief Aggregates data from several egg files and components to a single matrix

     @details
     Frequency-domain implementation of a single-pole RC low-pass filter.  
     This is no-doubt a non-ideal implementation, but demonstrates the features of a processor very well.

     The relationship between the cutoff frequency, f_c, and the RC constant is:
     ```
     RC = 1 / 2*pi*f_c
     ```

     Configuration name: "low-pass-filter"

     Available configuration values:
     - "rc": double -- RC time constant of the filter

     Slots:
     - "ts-fftw": void (Nymph::KTDataPtr) -- Combine multiple multi-channel complex time series to a matrix; Requires KTTimeSeriesData; Adds KTAggregatedTSMatrixData; Emits signal "matrix"
     Signals:
     - "matrix": void (Nymph::KTDataPtr) -- Emitted upon matrix conversion; Guarantees KTLowPassFilteredPSData.
    */
    class KTMatrixAggregator : public Nymph::KTProcessor
    {
        public:
            KTMatrixAggregator(const std::string& name = "matrix-aggregator");

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(int, MaxCols);

            void PrintBuffer();

            void SlotFunction(Nymph::KTDataPtr data);

        private:
            bool AddCol(KTTimeSeriesData& tsData);
            void ShrinkMatrix();

            Eigen::ArrayXXcd fBufferMat;
            unsigned fNRows;

            unsigned fSignalCount;
            
            //***************
            // Signals
            //***************
            Nymph::KTSignalData fMatrixSignal;

            //***************
            // Slots
            //***************
            //Nymph::KTSlotDataOneType< Nymph::KTDataPtr > fTSSlot;
    };

} /* namespace Katydid */
#endif /* KTMATRIXAGGREGATOR_HH_ */
