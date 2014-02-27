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

namespace Katydid {

  class KTBasicASCIIWriter : public KTWriterWithTypists< KTBasicASCIIWriter > {
    // Constructors/destructors
  public:
    KTBasicASCIIWriter(const std::string& name = "basic-ascii-writer");
    virtual ~KTBasicASCIIWriter();

    // Configuration
  public:
    bool Configure(const KTParamNode* node);
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

  typedef KTDerivedTypeWriter< KTBasicASCIIWriter > KTBasicASCIITypeWriter;
}; // namespace Katydid


#endif // __KT_ASCII_WRITER_HH
