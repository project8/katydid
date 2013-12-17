/*
 * KTDCOffsetGenerator.hh
 *
 *  Created on: May 29, 2013
 *      Author: nsoblath
 */

#ifndef KTDCOFFSETGENERATOR_HH_
#define KTDCOFFSETGENERATOR_HH_

#include "KTTSGenerator.hh"

#include <vector>

namespace Katydid
{

    /*!
     @class KTDCOffsetGenerator
     @author N. S. Oblath

     @brief Generates a DC-offset time series

     @details
     Can create a new time series and drive processing, or can add a DC-offset signal to an existing time series.

     The value of the offset can be varied independently for each channel.

     Basic time series formation is dealt with in KTTSGenerator.

     Available configuration options:
     - Inherited from KTTSGenerator
       - "n-slices": UInt_t -- Number of slices to create (used only if creating new slices)
       - "n-channels": UInt_t -- Number of channels per slice to create (used only if creating new slices)
       - "time-series-size": UInt_t -- Specify the size of the time series (used only if creating new slices)
       - "bin-width": double -- Specify the bin width
       - "time-series-type": string -- Type of time series to produce (options: real [default], fftw)
       - "record-size": UInt_t -- Size of the imaginary record that this slice came from (only used to fill in the egg header; does not affect the simulation at all)
     - From KTDCOffsetGenerator
       - "offset": string -- (channel, offset) pair; may be repeated

     Slots: (inherited from KTTSGenerator)
     - "slice": void (KTDataPtr) -- Add a signal to an existing time series; Requires KTTimeSeriesData; Emits signal "slice" when done.

     Signals: (inherited from KTTSGenerator)
     - "header": void (const KTEggHeader*) -- emitted when the egg header is created.
     - "slice": void (KTDataPtr) -- emitted when the new time series is produced or processed.
     - "done": void () --  emitted when the job is complete.
    */
    class KTDCOffsetGenerator : public KTTSGenerator
    {
        public:
            KTDCOffsetGenerator(const std::string& name = "dc-offset-generator");
            virtual ~KTDCOffsetGenerator();

            virtual Bool_t ConfigureDerivedGenerator(const KTPStoreNode* node);

            const std::vector< double >& GetOffsets() const;
            void SetOffset(UInt_t component, double freq);

        private:
            std::vector< double > fOffsets;

        public:
            virtual Bool_t GenerateTS(KTTimeSeriesData& data);

    };

    inline const std::vector< double >& KTDCOffsetGenerator::GetOffsets() const
    {
        return fOffsets;
    }

    inline void KTDCOffsetGenerator::SetOffset(UInt_t component, double offset)
    {
        fOffsets[component] = offset;
        return;
    }

} /* namespace Katydid */
#endif /* KTDCOFFSETGENERATOR_HH_ */
