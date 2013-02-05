#include "KTBasicAsciiWriter.hh"
#include "KTFactory.hh"
#include "KTBundle.hh"
#include "KTPStoreNode.hh"

namespace Katydid {

  KTLOGGER(asciilog, "katydid.output.ascii");
  
  static KTDerivedRegistrar< KTWriter, KTBasicASCIIWriter > 
  sAWR("basic-ascii-writer");

  static KTDerivedRegistrar< KTProcessor, KTBasicASCIIWriter > 
  sAPR("basic-ascii-writer");

  KTBasicASCIIWriter::KTBasicASCIIWriter() :
    KTWriterWithTypists<KTBasicASCIIWriter>(),
    fOutputFilename("basic_ascii_output.txt"),
    fOutputStream(NULL)
  {
    fConfigName = "basic-ascii-writer";
  } // default constructor

  KTBasicASCIIWriter::~KTBasicASCIIWriter() {
    if( this->fOutputStream->is_open() ) this->fOutputStream->close();
    delete this->fOutputStream;
  }

  // Configuration method
  Bool_t KTBasicASCIIWriter::Configure(const KTPStoreNode* n) {
    if (n != NULL) {
      fOutputFilename = n->GetData<std::string>("output-file",fOutputFilename);
      if( this->OpenFile() == false ) {
	KTWARN(asciilog, "ASCII writer couldn't open output file - no data will be written!");
      }
    }

    return true;
  }

  Bool_t KTBasicASCIIWriter::OpenFile() {
    fOutputStream = new std::ofstream(fOutputFilename.c_str(), std::ios_base::out);
    if( fOutputStream->is_open() == false ) {
      delete fOutputStream;
      fOutputStream = NULL;
      KTERROR(asciilog, "Output file " << fOutputFilename << " could not be opened!");
      return false;
    }
    return true;
  }

  bool KTBasicASCIIWriter::CanWrite() {
    return (this->fOutputStream != NULL) && (this->fOutputStream->is_open());
  }

  std::ofstream* KTBasicASCIIWriter::GetStream() {
    return this->fOutputStream;
  }

  // void KTBasicASCIIWriter::WriteFrequencySpectrumData(const KTFrequencySpectrumData* dt) {
  //   KTBundle* ev = dt->GetBundle();
  //   uint64_t evN = (ev == NULL) ? 0 : ev->GetBundleNumber();
  //   uint64_t nCh = dt->GetNChannels();

  //   if( fOutputStream && fOutputStream->is_open() ) {
  //     for( unsigned iCh = 0; iCh < nCh; iCh++ ) {
  // 	const KTFrequencySpectrum* spectrum = dt->GetSpectrum(iCh);
  // 	for( unsigned iB = 0; iB < spectrum->size(); iB++ ) {
	  
  // 	  (*fOutputStream) << evN
  // 			   << ","
  // 			   << iCh
  // 			   << ","
  // 			   << (*spectrum)(iB).abs() 
  // 			   << "," 
  // 			   << (*spectrum)(iB).arg() 
  // 			   << std::endl;
  // 	}
  //     }
  //   }
  //   else {
  //     KTWARN(asciilog, "no file open, no data written!");
  //   }
  // }

  /*
   *  The following methods are unimplemented and just warn!
   */
  void KTBasicASCIIWriter::Publish(const KTWriteableData* dt) {
    KTWARN(asciilog, "stub Publish(const KTWriteableData*) called! no data written!");
  };
  void KTBasicASCIIWriter::Write(const KTWriteableData* dt) {
    KTWARN(asciilog, "stub Write(const KTWriteableData*) called! no data written!");
  };

}; // namespace Katydid
