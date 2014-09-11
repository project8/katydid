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

        protected:
            H5::H5File* fFile;

    };


} /* namespace Katydid */
#endif /* KTHDF5WRITER_HH_ */
