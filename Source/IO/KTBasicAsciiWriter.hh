/*
 * KTBasicAsciiWriter.hh
 *
 *  Created on: Dec 17, 2012
 *      Author: kofron
 */

#ifndef __KT_ASCII_WRITER_HH
#define __KT_ASCII_WRITER_HH

#include "KTWriter.hh"

#include <fstream>

namespace Katydid
{
    

    class KTBasicASCIIWriter;

    typedef Nymph::KTDerivedTypeWriter< KTBasicASCIIWriter > KTBasicASCIITypeWriter;

    class KTBasicASCIIWriter : public Nymph::KTWriterWithTypists< KTBasicASCIIWriter, KTBasicASCIITypeWriter > {
            // Constructors/destructors
        public:
            KTBasicASCIIWriter(const std::string& name = "basic-ascii-writer");
            virtual ~KTBasicASCIIWriter();

            // Configuration
        public:
            bool Configure(const scarab::param_node* node);
        protected:
            std::ofstream* fOutputStream;
            std::string fOutputFilename;

        public:
            bool OpenFile();

            // Basic publish and write
        public:
            bool CanWrite();
            std::ofstream* GetStream();

    }; // class KTBasicASCIIWriter

}; // namespace Katydid


#endif // __KT_ASCII_WRITER_HH
