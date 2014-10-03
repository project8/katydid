/*
 * KTHDF5Writer.hh
 *
 *  Created on: Sept 5, 2014
 *      Author: J.N. Kofron, N.S. Oblath
 */

#ifndef KTHDF5WRITER_HH_
#define KTHDF5WRITER_HH_

#include <string>
#include <set>

#include "KTWriter.hh"
#include "KTEggHeader.hh"
#include "KTMemberVariable.hh"
#include "H5Cpp.h"

namespace Katydid {
    class KTHDF5Writer;

    typedef KTDerivedTypeWriter< KTHDF5Writer > KTHDF5TypeWriter;

    class KTHDF5Writer : public KTWriterWithTypists< KTHDF5Writer > {
        public:
            explicit KTHDF5Writer(const std::string& name = "hdf5-writer");
            virtual ~KTHDF5Writer();

            bool Configure(const KTParamNode* node);
            void WriteEggHeader(KTEggHeader* header);

        public:
            H5::H5File* OpenFile(const std::string& filename, 
                                 const std::string& flag);
            void CloseFile();

            MEMBERVARIABLEREF(std::string, Filename);

            bool OpenAndVerifyFile();

            /*
             Adds a new group to the HDF5 file.  If the group already exists,
             this is a no-op.
            */ 
            H5::Group* AddGroup(const std::string& groupname);

            /*
             Adds metadata to the HDF5 file.  This data goes into the /metadata
             group.  

             TODO: Creating groups within this function can happen if the 
             name contains slashes - e.g. AddMetadata("foo/bar", "baz") will
             create a scalar datatype at /metadata/foo/bar with the value "baz".
            */
            template <typename T> void AddMetadata(std::string name, T value);

            // Header related
            bool DidParseHeader();
            KTEggHeader* GetHeader();

        protected:
            H5::H5File* fFile;

        private:
            KTEggHeader fHeader;
            bool fHeaderParsed;
            std::map<std::string, H5::Group*> fGroups;
            void WriteMetadata(std::string name, H5::DataType type, const void* value);
    };

    template<> inline void KTHDF5Writer::AddMetadata<unsigned>(std::string name, unsigned value) {
        this->WriteMetadata(name, H5::PredType::NATIVE_UINT, &value);
    };

    template<> inline void KTHDF5Writer::AddMetadata<double>(std::string name, double value) {
        this->WriteMetadata(name, H5::PredType::NATIVE_DOUBLE, &value);
    };

    template<> inline void KTHDF5Writer::AddMetadata<unsigned long>(std::string name, unsigned long value) {
        this->WriteMetadata(name, H5::PredType::NATIVE_ULONG, &value);
    };

    template<> inline void KTHDF5Writer::AddMetadata<std::string>(std::string name, std::string value) {
        H5::StrType h5type(H5::PredType::C_S1, value.length() + 1);
        this->WriteMetadata(name, h5type, value.c_str());
    };
} /* namespace Katydid */
#endif /* KTHDF5WRITER_HH_ */
