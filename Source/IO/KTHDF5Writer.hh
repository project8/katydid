/*
 * KTHDF5Writer.hh
 *
 *  Created on: Sept 5, 2014
 *      Author: J. Kofron, N.S. Oblath
 */

#ifndef KTHDF5WRITER_HH_
#define KTHDF5WRITER_HH_

#include <string>
#include "KTWriter.hh"
#include "KTEggHeader.hh"


#include "KTMemberVariable.hh"

#include <set>

#include "H5Cpp.h"

namespace Katydid
{
    class KTHDF5Writer;

    typedef KTDerivedTypeWriter< KTHDF5Writer > KTHDF5TypeWriter;

    class KTHDF5Writer : public KTWriterWithTypists< KTHDF5Writer >//public KTWriter
    {
        public:
            KTHDF5Writer(const std::string& name = "hdf5-writer");
            virtual ~KTHDF5Writer();

            bool Configure(const KTParamNode* node);
            void WriteEggHeader(KTEggHeader* header);

        public:
            H5::H5File* OpenFile(const std::string& filename, const std::string& flag);
            void CloseFile();

            MEMBERVARIABLEREF(std::string, Filename);

            bool OpenAndVerifyFile();

            /*
             Adds a new group to the HDF5 file.  If the group already exists,
             this is a no-op.
            */ 
            H5::Group* AddGroup(const std::string& groupname);

            /*
             Set component number.
            */
            void SetComponents(const unsigned n_channels);

            /*
             Set slice size and raw slice size.
            */
            unsigned GetSliceSize();
            void SetSliceSize(const unsigned slice_size);
            unsigned GetRawSliceSize();
            void SetRawSliceSize(const unsigned raw_slice_size);

            /*
             Functions relevant to the creation and retrieval of the
             dataspaces for slices.
            */
            void CreateDataspaces();
            H5::DataSpace* GetRawTimeDataspace();
            H5::DataSpace* GetTimeDataspace();

            /*
             Create datasets on demand for time series data.
            */
            H5::DataSet* CreateRawTimeSeriesDataSet(const std::string &name);

        protected:
            H5::H5File* fFile;
            unsigned slice_size;
            unsigned raw_slice_size;
            unsigned n_channels;
            H5::DataSpace* raw_time_slice_dspace;
            H5::DataSpace* time_slice_dspace;

    };


} /* namespace Katydid */
#endif /* KTHDF5WRITER_HH_ */
