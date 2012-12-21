#include "KTBasicAsciiWriter.hh"
#include "KTFactory.hh"
#include "KTEvent.hh"
#include "KTPStoreNode.hh"

namespace Katydid {

  KTLOGGER(asciilog, "katydid.output.ascii");
  
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
	KTWARN(asciilog, "ASCII writer couldn't open output file - no data will be written!");
      }
    }

    return true;
  }

  Bool_t KTBasicAsciiWriter::OpenFile() {
    fOutputStream = new std::ofstream(fOutputFilename.c_str(), std::ios_base::out);
    if( fOutputStream->is_open() == false ) {
      delete fOutputStream;
      fOutputStream = NULL;
      KTERROR(asciilog, "Output file " << fOutputFilename << " could not be opened!");
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
    uint64_t nCh = dt->GetNChannels();

    if( fOutputStream && fOutputStream->is_open() ) {
      for( unsigned iCh = 0; iCh < nCh; iCh++ ) {
	const KTFrequencySpectrum* spectrum = dt->GetSpectrum(iCh);
	for( unsigned iB = 0; iB < spectrum->size(); iB++ ) {
	  
	  (*fOutputStream) << evN
			   << ","
			   << iCh
			   << ","
			   << (*spectrum)(iB).abs() 
			   << "," 
			   << (*spectrum)(iB).arg() 
			   << std::endl;
	}
      }
    }
    else {
      KTWARN(asciilog, "no file open, no data written!");
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

  /*
   *  The following methods are unimplemented and just warn!
   */
  void KTBasicAsciiWriter::Publish(const KTWriteableData* dt) {
    KTWARN(asciilog, "stub Publish(const KTWriteableData*) called! no data written!");
  };
  void KTBasicAsciiWriter::Write(const KTWriteableData* dt) {
    KTWARN(asciilog, "stub Write(const KTWriteableData*) called! no data written!");
  };
  void KTBasicAsciiWriter::Write(const KTCorrelationData* dt) {
    KTWARN(asciilog, "stub Write(const KTCorrelationData*) called! no data written!");
  };
  void KTBasicAsciiWriter::Write(const KTSlidingWindowFSData* dt) {
    KTWARN(asciilog, "stub Write(const KTSlidingWindowFSData*) called! no data written!");
  };
  void KTBasicAsciiWriter::Write(const KTSlidingWindowFSDataFFTW* dt) {
    KTWARN(asciilog, "stub Write(const KTSlidingWindowFSDataFFTW*) called! no data written!");
  };

}; // namespace Katydid
