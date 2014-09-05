/*
 * KTHDF5Writer.hh
 *
 *  Created on: Sept 5, 2014
 *      Author: J. Kofron, N.S. Oblath
 */

#ifndef KTHDF5WRITER_HH_
#define KTHDF5WRITER_HH_

#include "KTWriter.hh"

#include "KTMemberVariable.hh"

#include <set>

namespace Katydid
{
    class KTHDF5Writer;

    typedef KTDerivedTypeWriter< KTHDF5Writer > KTROOTTreeTypeWriter;

    class KTHDF5Writer : public KTWriterWithTypists< KTHDF5Writer >//public KTWriter
    {
        public:
            KTHDF5Writer(const std::string& name = "hdf5-writer");
            virtual ~KTHDF5Writer();

            bool Configure(const KTParamNode* node);

        public:
            bool OpenFile(const std::string& filename, const std::string& flag);
            void CloseFile();

            MEMBERVARIABLEREF(std::string, Filename);

            bool OpenAndVerifyFile();


    };


} /* namespace Katydid */
#endif /* KTHDF5WRITER_HH_ */
