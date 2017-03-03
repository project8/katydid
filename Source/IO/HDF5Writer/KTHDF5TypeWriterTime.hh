/*
 * KTHDF5TypeWriterTime.hh
 *
 *  Created on: Sept 9, 2014
 *      Author: J.N. Kofron, N.S. Oblath
 */

#ifndef KTHDF5TYPEWRITERTIME_HH_
#define KTHDF5TYPEWRITERTIME_HH_

#include "KTHDF5Writer.hh"
#include "KTData.hh"

// NOTE (NSO): as of 3/2/17, the raw time series and time series functions are only valid for real data, not IQ data!
//             they do not, however, protect against attempting to use the functions on data that has a sample > 1.

namespace Katydid {
    class KTEggHeader;


    class KTHDF5TypeWriterTime : public KTHDF5TypeWriter {
        public:
            KTHDF5TypeWriterTime();
            virtual ~KTHDF5TypeWriterTime();

            void RegisterSlots();

            void WriteRawTimeSeriesData(Nymph::KTDataPtr data);

            void WriteRealTimeSeriesData(Nymph::KTDataPtr data);

        private:
            H5::DataSpace* fRawTSliceDSpace; //raw_time_slice_dspace
            H5::DataSpace* fRealTSliceDSpace; // real_time_slice_dspace

            H5::DataSet* CreateRawTSDSet(const std::string &name);
            H5::DataSet* CreateRealTSDSet(const std::string &name);
            H5::Group* fRawDataGroup;
            H5::Group* fRealDataGroup;


            unsigned* fRawTimeBuffer;
            double* fRealTimeBuffer;

    };

} /* namespace Katydid */


#endif /* KTHDF5TYPEWRITERTIME_HH_ */
