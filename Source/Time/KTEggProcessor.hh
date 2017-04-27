/**
 @file KTEggProcessor.hh
 @brief Contains KTEggProcessor
 @details Iterates over slices in an Egg file.
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTEGGPROCESSOR_HH_
#define KTEGGPROCESSOR_HH_

#include "KTPrimaryProcessor.hh"

#include "KTData.hh"
#include "KTEggHeader.hh"
#include "KTEggReader.hh"
#include "KTSlot.hh"


namespace Katydid
{
    
    class KTDAC;
    class KTProcSummary;
    class KTTimeSeriesData;

    /*!
     @class KTEggProcessor
     @author N. S. Oblath

     @brief Iterates over the slices in an Egg file.

     @details
     Iterates over slices in an egg file; slices are extracted until fNSlices is reached.

     Configuration name: "egg-processor"

     Available configuration options:
     - "number-of-slices": unsigned -- Number of slices to process
     - "progress-report-interval": unsigned -- Interval (# of slices) between 
        reporting progress (mainly relevant for RELEASE builds); turn off by setting to 0
     - "filename": string -- Egg filename to use (will take priority over \"filenames\")
     - "filenames": array of strings -- Egg filenames to use (\"filename\" will take priority over this)
     - "egg-reader": string -- Egg reader to use.
        Options: "egg2" [default], "egg1", "rsamat"
        - "egg2" [default] - Uses the monarch2 library to read Egg files
        - "egg1" - uses the old style Egg reader, which opens and reads the
           contents of the Egg file directly
        - "rsamat" - reads matlab (mat) files generated by the Tektronix RSA,
           and stores the header and data into Egg classes as if it was an Egg
           file, allowing for seamless processing of the data
     - "slice-size": unsigned -- Specify the size of the time series (required)
     - "stride": unsigned -- Specify how many bins to advance between slices
        (leave unset to make stride == slice-size; i.e. no overlap or skipping
         between slices)
     - "start-time": double -- Specify how far into the file to start (in seconds)
     - "normalize-voltages": bool -- Flag to toggle the normalization of ADC
        values from the egg file (default: true)
     - "dac": object -- configure the DAC

     Command-line options defined
     - -n (n-slices): Number of slices to process
     - -e (egg-file): Egg filename to use
     - -z (--use-2011-egg-reader): Use the 2011 egg reader

     Signals:
     - "header": void (Nymph::KTDataPtr) -- emitted when the file header is parsed.
     - "raw-ts" void (Nymph::KTDataPtr) -- emitted when a new raw time series is 
        produced; guarantees KTRawTimeSeriesData
     - "ts": void (Nymph::KTDataPtr) -- emitted when the new calibrated time series is
        produced; Guarantees KTTimeSeriesData
     - "egg-done": void () --  emitted when a file is finished.
     - "summary": void (const KTProcSummary*) -- emitted when a file is 
        finished (after "egg-done")

     Additional Notes:
     - To use the "rsamat" egg-reader, the user must have installed the Matlab 
       Compiler Runtime (MCR) of version 2014a, and configured Katydid to use 
       Matlab at compilation.  Earlier versions might work but have not been
       tested.  R2013a is known to not work for Katydid on Macs, and R2012b
       to not work with Katydid on Linux systems.
    */
    class KTEggProcessor : public Nymph::KTPrimaryProcessor
    {
        public:
            KTEggProcessor(const std::string& name = "egg-processor");
            virtual ~KTEggProcessor();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(unsigned, NSlices);
            MEMBERVARIABLE(unsigned, ProgressReportInterval);

            MEMBERVARIABLEREF(KTEggReader::path_vec, Filenames);
            MEMBERVARIABLEREF(std::string, EggReaderType);

            MEMBERVARIABLE(unsigned, SliceSize);
            MEMBERVARIABLE(unsigned, Stride);
            MEMBERVARIABLE(double, StartTime);

            MEMBERVARIABLE(bool, NormalizeVoltages);

        private:
            KTDAC* fDAC;

        public:
            bool Run();

            bool ProcessEgg();

            bool HatchNextSlice(KTEggReader* reader, Nymph::KTDataPtr& data) const;
            void NormalizeData(Nymph::KTDataPtr& data);

        private:
            void UnlimitedLoop(KTEggReader* reader);
            void LimitedLoop(KTEggReader* reader);


            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fHeaderSignal;
            Nymph::KTSignalData fRawDataSignal;
            Nymph::KTSignalData fDataSignal;
            Nymph::KTSignalDone fEggDoneSignal;
            Nymph::KTSignalOneArg< const KTProcSummary* > fSummarySignal;

    };

    inline bool KTEggProcessor::Run()
    {
        return ProcessEgg();
    }

    inline bool KTEggProcessor::HatchNextSlice(KTEggReader* reader, Nymph::KTDataPtr& data) const
    {
        data = reader->HatchNextSlice();
        if (data) return true;
        return false;
    }

} /* namespace Katydid */

#endif /* KTEGGPROCESSOR_HH_ */
