#ifndef KTMANTISWRITERTOTS_HH_
#define KTMANTISWRITERTOTS_HH_

#include "mt_writer.hh"

#include "KTData.hh"
#include "KTEggHeader.hh"

#include "MonarchRecord.hpp"

#include "thorax.hh"

#include <string>
#include <vector>

namespace Katydid
{
    
    class KTMantis;
    class KTRawTimeSeries;

    class KTMantisWriterToTS :
        public mantis::writer
    {
        private:
            struct MonarchReadState
            {
                enum Status
                {
                    kInvalid,
                    kAtStartOfRun,
                    kAtStartOfRecord, // this means it's the first reading of this record; the slice may start in the middle of the record
                    kContinueReading,
                    kReachedEndOfRecord
                };
                unsigned fReadPtrOffset; // sample offset of the read pointer in the current record
                unsigned fReadPtrRecordOffset; // record offset of the read pointer relative to the start of the slice
                unsigned fSliceStartPtrOffset; // sample offset of the start of the slice in the relevant record
                unsigned fAbsoluteRecordOffset; // number of records read in the run
                monarch::AcquisitionIdType fAcquisitionID;
                bool fIsNewAcquisition; // whether this record is a new acquisition
                Status fStatus;
            };

            struct WriteState
            {
                    enum Status
                    {
                        kInvalid,
                        kAtStartOfRun,
                        kAtStartOfSlice,
                        kMidSlice,
                        kSliceComplete
                    };
                    unsigned fWriteBin;
                    Status fStatus;
                    unsigned fSliceNumber;
            };

        public:
            KTMantisWriterToTS();
            virtual ~KTMantisWriterToTS();

            void configure( const mantis::param_node* );

            bool initialize_derived( mantis::request* a_response );
            void finalize( mantis::response* a_response );

            void set_client( KTMantis* a_client );

        private:
            bool write( mantis::block* a_block );
            unsigned hatch_next_slice( const std::vector< monarch::MonarchRecordBytes* >& a_records, const std::vector< monarch::MonarchRecordDataInterface< uint64_t >* >& a_data_ints );

            KTMantis* f_client;

            struct dig_calib_params f_dig_params;

            unsigned f_slice_size;
            unsigned f_stride;

            double f_sample_rate_units_in_hz;

            MonarchReadState f_read_state;
            WriteState f_write_state;
            unsigned f_advance_records;
            //unsigned f_advance_to_ptr_offset;

            std::vector< byte_type* > f_bytes;
            std::vector< monarch::MonarchRecordBytes* > f_records;
            std::vector< monarch::MonarchRecordDataInterface< uint64_t >* > f_record_data_interfaces;

            KTEggHeader f_header;
            Nymph::KTDataPtr f_data_ptr;
            std::vector< KTRawTimeSeries* > f_new_time_series;

    };

} // namespace Katydid

#endif /* KTMANTISWRITERTOTS_HH_ */
