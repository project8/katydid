/*
 * KTHDF5TypeWriterEgg.hh
 *
 *  Created on: Sept 9, 2014
 *      Author: J.N. Kofron, N.S. Oblath
 */

#ifndef KTHDF5TYPEWRITEREGG_HH_
#define KTHDF5TYPEWRITEREGG_HH_

#include "KTHDF5Writer.hh"
#include "KTData.hh"

namespace Katydid {
    class KTEggHeader;


    class KTHDF5TypeWriterEgg : public KTHDF5TypeWriter {
        public:
            KTHDF5TypeWriterEgg();
            virtual ~KTHDF5TypeWriterEgg();

            void RegisterSlots();

            void WriteRawTimeSeriesData(KTDataPtr data);

            void WriteRealTimeSeriesData(KTDataPtr data);

        private:
            H5::DataSpace* fRawTSliceDSpace; //raw_time_slice_dspace
            H5::DataSpace* fRealTSliceDSpace; // real_time_slice_dspace
            unsigned fSliceSize; // slice_size
            unsigned fRawSliceSize;
            unsigned fNComponents;

            void CreateDataspaces();
            H5::DataSet* CreateRawTSDSet(const std::string &name);
            H5::DataSet* CreateRealTSDSet(const std::string &name);
            H5::Group* fRawDataGroup;
            H5::Group* fRealDataGroup;


            unsigned* fRawTimeBuffer;
            double* fRealTimeBuffer;

            void ProcessEggHeader();
            bool fHeaderProcessed;

    };

} /* namespace Katydid */


#endif /* KTHDF5TYPEWRITEREGG_HH_ */
