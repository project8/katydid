#include "KTBasicAsciiWriter.hh"
#include "KTFactory.hh"
#include "KTEvent.hh"
#include "KTPStoreNode.hh"

namespace Katydid {

  KTLOGGER(publog, "katydid.output.ascii");
  
  static KTDerivedRegistrar< KTWriter, KTBasicAsciiWriter > 
  sAWR("basic-ascii-writer");

  static KTDerivedRegistrar< KTProcessor, KTBasicAsciiWriter > 
  sAPR("basic-ascii-writer");

  KTBasicAsciiWriter::KTBasicAsciiWriter() :
    KTWriter(),
    fOutputFilename("basic_ascii_output.txt"),
    fOutputStream(NULL)
  {
    fConfigName = "basic-ascii-writer";

    // This writer so far only knows how to write frequency spectrum data.
    RegisterSlot("write-frequency-spectrum", 
		 this, 
		 &KTBasicAsciiWriter::WriteFrequencySpectrumData);
    RegisterSlot("write-frequency-spectrum-fftw", 
		 this, 
		 &KTBasicAsciiWriter::WriteFrequencySpectrumDataFFTW);
  } // default constructor

  KTBasicAsciiWriter::~KTBasicAsciiWriter() {
    if( this->fOutputStream->is_open() ) this->fOutputStream->close();
    delete this->fOutputStream;
  }

  // Configuration method
  Bool_t KTBasicAsciiWriter::Configure(const KTPStoreNode* n) {
    if (n != NULL) {
      fOutputFilename = n->GetData<std::string>("output-file",fOutputFilename);
      if( this->OpenFile() == false ) {
	KTWARN(publog, "ASCII writer couldn't open output file - no data will be written!");
      }
    }

    return true;
  }

  Bool_t KTBasicAsciiWriter::OpenFile() {
    fOutputStream = new std::ofstream(fOutputFilename.c_str(), std::ios_base::out);
    if( fOutputStream->is_open() == false ) {
      delete fOutputStream;
      fOutputStream = NULL;
      KTERROR(publog, "Output file " << fOutputFilename << " could not be opened!");
      return false;
    }
    return true;
  }

  void KTBasicAsciiWriter::Write(const KTFrequencySpectrumData* dt) {
    return this->WriteFrequencySpectrumData(dt);
  }

  void KTBasicAsciiWriter::WriteFrequencySpectrumData(const KTFrequencySpectrumData* dt) {
    KTEvent* ev = dt->GetEvent();
    uint64_t evN = (ev == NULL) ? 0 : ev->GetEventNumber();
    if( fOutputStream->is_open() ) {
      (*fOutputStream) << "hi" << std::endl;
    }
  }

  void KTBasicAsciiWriter::Write(const KTFrequencySpectrumDataFFTW* dt) {
    return this->WriteFrequencySpectrumDataFFTW(dt);
  }

  void KTBasicAsciiWriter::WriteFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* dt) {
    KTEvent* ev = dt->GetEvent();
    uint64_t evN = (ev == NULL) ? 0 : ev->GetEventNumber();
    if( fOutputStream->is_open() ) {
      (*fOutputStream) << "hi" << std::endl;
    }
  }

}; // namespace Katydid
