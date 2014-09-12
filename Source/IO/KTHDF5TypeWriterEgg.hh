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

class TTree;

namespace Katydid {
    class KTEggHeader;


    class KTHDF5TypeWriterEgg : public KTHDF5TypeWriter {
        public:
            KTHDF5TypeWriterEgg();
            virtual ~KTHDF5TypeWriterEgg();

            void RegisterSlots();

        public:
            void ProcessEggHeader(KTEggHeader* header);

            void WriteRawTimeSeriesData(KTDataPtr data);

            void WriteTimeSeriesData(KTDataPtr data);

        private:
            H5::DataSpace* raw_time_slice_dspace;
            H5::DataSpace* time_slice_dspace;
            unsigned slice_size;
            unsigned raw_slice_size;
            unsigned n_components;

            void CreateDataspaces();
            H5::DataSet* CreateRawTSDSet(const std::string &name);
            H5::Group* working_group;

    };

} /* namespace Katydid */


#endif /* KTHDF5TYPEWRITEREGG_HH_ */
