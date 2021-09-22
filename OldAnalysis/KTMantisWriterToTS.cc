#include "KTMantisWriterToTS.hh"

#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTMantis.hh"
#include "KTRawTimeSeries.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTSliceHeader.hh"

#include "mt_configurator.hh"
#include "mt_exception.hh"
#include "mt_factory.hh"

#include "Monarch.hpp"
#include "MonarchException.hpp"

#include <cstring> // for memcpy()
using std::stringstream;

namespace mantis
{
    MT_REGISTER_WRITER_NS( Katydid, KTMantisWriterToTS, "katydid" );
}

namespace Katydid
{
    KTLOGGER( mtlog, "KTMantisWriterToTS" );

    KTMantisWriterToTS::KTMantisWriterToTS() :
                    mantis::writer(),
                    f_client( NULL ),
                    f_dig_params(),
                    f_slice_size(1024),
                    f_stride(0),
                    f_sample_rate_units_in_hz(1.e6),
                    f_read_state(),
                    f_write_state(),
                    f_advance_records(0),
                    f_bytes(),
                    f_records(),
                    f_record_data_interfaces(),
                    f_header(),
                    f_data_ptr(),
                    f_new_time_series()
    {
        // give some reasonable digitizer parameter defaults
        get_calib_params( 8, 1, -0.25, 0.5, &f_dig_params );
    }
    KTMantisWriterToTS::~KTMantisWriterToTS()
    {
        while( ! f_bytes.empty() )
        {
            delete f_records.back();
            delete [] f_bytes.back();
            f_records.pop_back();
            f_bytes.pop_back();
        }
        while( ! f_record_data_interfaces.empty() )
        {
            delete f_record_data_interfaces.back();
            f_record_data_interfaces.pop_back();
        }
        while( ! f_new_time_series.empty() )
        {
            delete f_new_time_series.back();
            f_new_time_series.pop_back();
        }
    }

    void KTMantisWriterToTS::configure( const mantis::param_node* a_config )
    {
        get_calib_params(
                a_config->get_value< unsigned >( "bit-depth",      f_dig_params.bit_depth ),
                a_config->get_value< unsigned >( "data-type-size", f_dig_params.data_type_size ),
                a_config->get_value< double   >( "voltage-min",    f_dig_params.v_min ),
                a_config->get_value< double   >( "voltage-range",  f_dig_params.v_range ),
                &f_dig_params );
        return;
    }

    bool KTMantisWriterToTS::initialize_derived( mantis::request* a_request )
    {
        KTDEBUG( mtlog, "Initializing writer-to-ts" );
        f_slice_size = f_client->GetSliceSize();
        f_stride = f_client->GetStride();
        if( f_stride == 0 )
            f_stride = f_slice_size;
        if( f_stride < f_slice_size )
        {
            KTERROR( mtlog, "Stride cannot be less than the slice size: " << f_stride << " < " << f_slice_size );
            return false;
        }
        // fill in header from request
        f_header.SetFilename( "none" );
        if( a_request->mode() == mantis::request_mode_t_single )
        {
            f_header.SetAcquisitionMode( monarch::sOneChannel );
            f_header.SetNChannels( 1 );
            f_header.SetFormatMode( monarch::sFormatSingle );
        }
        if( a_request->mode() == mantis::request_mode_t_dual_separate )
        {
            f_header.SetAcquisitionMode( monarch::sTwoChannel );
            f_header.SetNChannels( 2 );
            f_header.SetFormatMode( monarch::sFormatMultiSeparate );
        }
        if( a_request->mode() == mantis::request_mode_t_dual_interleaved )
        {
            f_header.SetAcquisitionMode( monarch::sTwoChannel );
            f_header.SetNChannels( 2 );
            f_header.SetFormatMode( monarch::sFormatMultiInterleaved );
        }


        f_header.SetRecordSize( f_buffer->record_size() / f_header.GetNChannels() );
        f_header.SetRunDuration( a_request->duration() );
        f_header.SetAcquisitionRate( a_request->rate() * f_sample_rate_units_in_hz );

        f_header.SetTimestamp( a_request->date() );
        f_header.SetDescription( a_request->description() );
        f_header.SetRunType( monarch::sRunTypeSignal );
        f_header.SetRunSource( monarch::sSourceMantis );

        f_header.SetDataTypeSize( f_dig_params.data_type_size );
        f_header.SetBitDepth( f_dig_params.bit_depth );
        f_header.SetVoltageMin( f_dig_params.v_min );
        f_header.SetVoltageRange( f_dig_params.v_range );

        f_client->EmitHeaderSignal( &f_header );

        // reset statuses and offsets
        f_read_state.fReadPtrOffset = 0;
        f_read_state.fReadPtrRecordOffset = 0;
        f_read_state.fSliceStartPtrOffset = 0;
        f_read_state.fAbsoluteRecordOffset = 0;
        f_read_state.fStatus = MonarchReadState::kAtStartOfRun;

        f_write_state.fSliceNumber = 0;
        f_write_state.fWriteBin = 0;
        f_write_state.fStatus = WriteState::kAtStartOfRun;

        // initialize monarch records
        KTDEBUG( mtlog, "Allocating record arrays for " << f_header.GetNChannels() << " channels" );
        unsigned n_bytes = sizeof(monarch::AcquisitionIdType) + sizeof(monarch::RecordIdType) + sizeof(monarch::TimeType) + f_header.GetRecordSize() * f_header.GetDataTypeSize();
        for( unsigned iChannel = 0; iChannel < f_header.GetNChannels(); ++iChannel )
        {
            byte_type* new_bytes = new byte_type[ n_bytes ];
            monarch::MonarchRecordBytes* new_record = new ( new_bytes ) monarch::MonarchRecordBytes();
            f_bytes.push_back( new_bytes );
            f_records.push_back( new_record );

            f_record_data_interfaces.push_back( new monarch::MonarchRecordDataInterface< uint64_t >(f_records.back()->fData, f_header.GetDataTypeSize()) );
        }

        return true;
    }

    void KTMantisWriterToTS::finalize( mantis::response* a_response )
    {
        // delete monarch records
        while( ! f_bytes.empty() )
        {
            delete f_records.back();
            // i was getting a deallocation error when i used the next line
            //delete [] f_bytes.back();
            f_records.pop_back();
            f_bytes.pop_back();

            delete f_record_data_interfaces.back();
            f_record_data_interfaces.pop_back();
        }

        writer::finalize( a_response );

        f_client->EmitMantisDoneSignal();

        return;
    }

    bool KTMantisWriterToTS::write( mantis::block* a_block )
    {
        if (f_read_state.fStatus == MonarchReadState::kAtStartOfRun)
        {
            KTDEBUG(mtlog, "At first record");

            // initial values for read state
            f_read_state.fReadPtrOffset = 0;
            f_read_state.fReadPtrRecordOffset = 0;
            f_read_state.fSliceStartPtrOffset = 0;
            f_read_state.fAbsoluteRecordOffset = 0;
            f_read_state.fIsNewAcquisition = true;
            f_read_state.fAcquisitionID = a_block->get_acquisition_id();
            f_read_state.fStatus = MonarchReadState::kAtStartOfRecord;

            // initial values for write state
            f_write_state.fSliceNumber = 0;
            f_write_state.fWriteBin = 0;
            f_write_state.fStatus = WriteState::kAtStartOfRun;

            // initial values for advance variables
            f_advance_records = 0;
            //f_advance_to_ptr_offset = 0;
        }
        else // start of new record, but not new run; may be mid slice
        {
            f_read_state.fStatus = MonarchReadState::kAtStartOfRecord;

            ++f_read_state.fAbsoluteRecordOffset;
            if (f_write_state.fStatus == WriteState::kMidSlice)
            {
                ++f_read_state.fReadPtrRecordOffset;
            }

            --f_advance_records;
            // skip records if they're not needed
            if( f_advance_records != 0)
            {
                return true;
            }

            // check whether the acquisition id changed
            // this must be done after skipping records
            if (f_read_state.fAcquisitionID != a_block->get_acquisition_id())
            {
                f_read_state.fAcquisitionID = a_block->get_acquisition_id();
                f_read_state.fIsNewAcquisition = true;
            }
            else
            {
                f_read_state.fIsNewAcquisition = false;
            }
        }

        // copy data to records
        if( f_header.GetNChannels() == 1 )
        {
            f_records[0]->fAcquisitionId = (monarch::AcquisitionIdType) (a_block->get_acquisition_id());
            f_records[0]->fRecordId = (monarch::RecordIdType) (a_block->get_record_id());
            f_records[0]->fTime = (monarch::TimeType) (a_block->get_timestamp());
            ::memcpy( f_records[0]->fData, a_block->data_bytes(), a_block->get_data_nbytes() );
        }
        else
        {
            for( unsigned iChannel = 0; iChannel < f_header.GetNChannels(); ++iChannel )
            {
                f_records[iChannel]->fAcquisitionId = (monarch::AcquisitionIdType) (a_block->get_acquisition_id());
                f_records[iChannel]->fRecordId = (monarch::RecordIdType) (a_block->get_record_id());
                f_records[iChannel]->fTime = (monarch::TimeType) (a_block->get_timestamp());
            }
            // unzip records (assumes 2 channels)
            unsigned t_rec_size = f_header.GetRecordSize();
            unsigned t_data_type_size = f_header.GetDataTypeSize();
            byte_type* t_zipped = a_block->data_bytes();
            byte_type* t_unzipped_0 = f_records[0]->fData;
            byte_type* t_unzipped_1 = f_records[1]->fData;
            for( unsigned iBin = 0; iBin < f_header.GetRecordSize(); ++iBin )
            {
                *t_unzipped_0 = *t_zipped;
                t_zipped += t_data_type_size;
                t_unzipped_0 += t_data_type_size;

                *t_unzipped_1 = *t_zipped;
                t_zipped += t_data_type_size;
                t_unzipped_1 += t_data_type_size;
            }
        }

        // create slices
        while( f_read_state.fStatus != MonarchReadState::kReachedEndOfRecord )
        {
            if( ! hatch_next_slice( f_records, f_record_data_interfaces ) ) break;

            if( f_write_state.fStatus == WriteState::kSliceComplete )
            {
                f_write_state.fStatus = WriteState::kAtStartOfSlice;
            }

        }

        // there's no situation in which f_advance_records should be 0 at the end of a record
        if( f_advance_records == 0 && f_read_state.fStatus == MonarchReadState::kReachedEndOfRecord)
        {
            KTERROR(mtlog, "Something is wrong, because we've reached the end of the record without indication to advance records");
            return false;
        }

        // something's wrong with the read state
        if( f_read_state.fStatus == MonarchReadState::kInvalid )
        {
            KTERROR(mtlog, "Read state is <Invalid>");
            return false;
        }

        // something's wrong with the write state
        if( f_write_state.fStatus == WriteState::kInvalid )
        {
            KTERROR(mtlog, "Write state is <Invalid>");
            return false;
        }

        return true;
    }


    void KTMantisWriterToTS::set_client( KTMantis* a_client )
    {
        f_client = a_client;
        return;
    }

    unsigned KTMantisWriterToTS::hatch_next_slice( const std::vector< monarch::MonarchRecordBytes* >& a_records, const std::vector< monarch::MonarchRecordDataInterface< uint64_t >* >& a_data_ints )
    {
        unsigned nChannels = f_header.GetNChannels();

        if (f_write_state.fStatus == WriteState::kAtStartOfRun ||
                f_write_state.fStatus == WriteState::kAtStartOfSlice ||
                (f_write_state.fStatus == WriteState::kMidSlice &&
                 f_read_state.fStatus == MonarchReadState::kAtStartOfRecord &&
                 f_read_state.fIsNewAcquisition == true))
        {
            if (f_write_state.fStatus == WriteState::kAtStartOfRun)
            {
                f_write_state.fStatus = WriteState::kAtStartOfSlice;
            }
            else
            {
                ++f_write_state.fSliceNumber;
            }

             // space for creating the new time series
            f_new_time_series.resize(nChannels);

           // create the new data object
            f_data_ptr.reset(new Nymph::KTData());

            // Fill out slice header information
            KTSliceHeader& sliceHeader = f_data_ptr->Of< KTSliceHeader >().SetNComponents(f_header.GetNChannels());
            sliceHeader.SetIsNewAcquisition(f_read_state.fIsNewAcquisition);
            sliceHeader.SetSampleRate(f_header.GetAcquisitionRate());
            sliceHeader.SetRawSliceSize(f_slice_size);
            sliceHeader.SetSliceSize(f_slice_size);
            sliceHeader.CalculateBinWidthAndSliceLength();
            sliceHeader.SetNonOverlapFrac((double)f_stride / (double)f_slice_size);
            sliceHeader.SetTimeInRun(double(a_records[0]->fTime * SEC_PER_NSEC) + sliceHeader.GetBinWidth() * double(f_read_state.fReadPtrOffset));
            sliceHeader.SetSliceNumber(f_write_state.fSliceNumber);
            sliceHeader.SetStartRecordNumber(f_read_state.fAbsoluteRecordOffset);
            sliceHeader.SetStartSampleNumber(f_read_state.fReadPtrOffset);
            sliceHeader.SetRecordSize(f_header.GetRecordSize());
            KTDEBUG(mtlog, sliceHeader << "\nNote: some fields may not be filled in correctly yet");

            for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
            {
                sliceHeader.SetAcquisitionID(a_records[iChannel]->fAcquisitionId, iChannel);
                sliceHeader.SetRecordID(a_records[iChannel]->fRecordId, iChannel);
                sliceHeader.SetTimeStamp(a_records[iChannel]->fTime, iChannel);

                //tsData->SetTimeSeries(new vector< DataType >(monarchRecord->fDataPtr, monarchRecord->fDataPtr+header->GetSliceSize()), iChannel);
                f_new_time_series[iChannel] = new KTRawTimeSeries(f_slice_size, 0., double(f_slice_size) * sliceHeader.GetBinWidth());
            }

            // prepare the read state
            //f_read_state.fReadPtrRecordOffset = 0;
            //f_read_state.fSliceStartPtrOffset = f_read_state.fReadPtrOffset;
            f_read_state.fStatus = MonarchReadState::kContinueReading;
            f_read_state.fIsNewAcquisition = false;

            // prepare the write state
            f_write_state.fWriteBin = 0;
            f_write_state.fStatus = WriteState::kMidSlice;

            KTDEBUG(mtlog, "Time in run: " << sliceHeader.GetTimeInRun() << " s\n" <<
                    "\tBin width = " << 1. / f_header.GetAcquisitionRate() << '\n' <<
                    "\tMonarch record size = " << f_header.GetRecordSize() << '\n' <<
                    "\tRecord offset = " << f_read_state.fAbsoluteRecordOffset << '\n' <<
                    "\tSlice start pointer offset = " << f_read_state.fSliceStartPtrOffset << '\n' <<
                    "\tRead pointer record offset = " << f_read_state.fReadPtrRecordOffset << '\n' <<
                    "\tRead pointer offset = " << f_read_state.fReadPtrOffset);
        }



        // Loop over bins
        for (; f_write_state.fWriteBin < f_slice_size; ++f_write_state.fWriteBin)
        {
            if (f_read_state.fStatus == MonarchReadState::kReachedEndOfRecord)
            {
                KTDEBUG(mtlog, "Returning for new record, mid-slice");
                f_advance_records = 1;
                //f_advance_to_ptr_offset = 0;
                return true;
            }

            // Read the data from the records
            for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
            {
                // set the data
                (*f_new_time_series[iChannel])(f_write_state.fWriteBin) = a_data_ints[iChannel]->at(f_read_state.fReadPtrOffset);
            }

            // advance the reading pointer for the next bin
            ++(f_read_state.fReadPtrOffset);

            // check if we've reached the end of a record
            if (f_read_state.fReadPtrOffset >= f_header.GetRecordSize())
            {
                KTDEBUG(mtlog, "End of Monarch record reached.");
                f_read_state.fStatus = MonarchReadState::kReachedEndOfRecord;
            }
        } // end loop over bins

        f_write_state.fStatus = WriteState::kSliceComplete;

        KTSliceHeader& sliceHeader = f_data_ptr->Of< KTSliceHeader >();
        sliceHeader.SetEndRecordNumber(f_read_state.fAbsoluteRecordOffset);
        sliceHeader.SetEndSampleNumber(f_read_state.fReadPtrOffset - 1);

        // finally, set the records in the new data object
        KTRawTimeSeriesData& tsData = f_data_ptr->Of< KTRawTimeSeriesData >().SetNComponents(nChannels);
        for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
        {
            tsData.SetTimeSeries(f_new_time_series[iChannel], iChannel);
        }
        f_new_time_series.clear();

        // tell the client to emit the slice signal
        f_client->EmitSliceSignal( f_data_ptr );



        // calculate how far to skip ahead in the record
        // shift the slice start pointer by the stride
        // note that this pointer refers to the record in which the previous slice started
        f_read_state.fSliceStartPtrOffset += f_stride;
        unsigned sliceStartRecordOffset = 0; // how many records to shift to the start of the slice
        while (f_read_state.fSliceStartPtrOffset >= f_header.GetRecordSize())
        {
            f_read_state.fSliceStartPtrOffset -= f_header.GetRecordSize();
            ++sliceStartRecordOffset;
        }
        // Calculate whether we need to move the read pointer to a different record by subtracting the number
        // of records read in the last slice (fReadPtrRecordOffset)
        // If this is 0, it doesn't need to be moved
        // If it's > 0, we move on to a new record
        f_advance_records = sliceStartRecordOffset - f_read_state.fReadPtrRecordOffset;
        if (f_advance_records > 0)
        {
            f_read_state.fStatus = MonarchReadState::kReachedEndOfRecord;
        }

        // Move the read pointer to the slice start pointer within the record
        f_read_state.fReadPtrRecordOffset = 0;
        f_read_state.fReadPtrOffset = f_read_state.fSliceStartPtrOffset;

        // whether or not the end of a record has been reached, at this point, f_read_state.fSliceStartPtrOffset is correct for the next slice.
        // f_read_state.fReadPtrRecordOffset, and fReadPtrOffset are also correct.

        return true;
    }
}
