/*
 * KTBasicAsciiWriter.hh
 *
 *  Created on: Dec 17, 2012
 *      Author: kofron
 */

#ifndef __KT_ASCII_WRITER_HH
#define __KT_ASCII_WRITER_HH

#include "KTBundle.hh"
#include "KTWriter.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"

#include <fstream>

namespace Katydid {

  class KTBasicASCIIWriter : public KTWriterWithTypists< KTBasicASCIIWriter > {
    // Constructors/destructors
  public:
    KTBasicASCIIWriter();
    virtual ~KTBasicASCIIWriter();

    // Configuration
  public:
    Bool_t Configure(const KTPStoreNode* node);
  protected:
    std::ofstream* fOutputStream;
    std::string fOutputFilename;

  public:
    Bool_t OpenFile();

    // Basic publish and write
  public:
    bool CanWrite();
    std::ofstream* GetStream();   

    void Publish(const KTWriteableData* dt);
    void Write(const KTWriteableData* dt);

  }; // class KTBasicASCIIWriter 

  typedef KTDerivedTypeWriter< KTBasicASCIIWriter > KTBasicASCIITypeWriter;
}; // namespace Katydid


#endif // __KT_ASCII_WRITER_HH
