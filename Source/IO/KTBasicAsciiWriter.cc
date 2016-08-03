#include "KTBasicAsciiWriter.hh"
#include "param.hh"

namespace Katydid {

    KTLOGGER(asciilog, "KTBasicAsciiWriter");

    KT_REGISTER_WRITER(KTBasicASCIIWriter, "basic-ascii-writer");

    KT_REGISTER_PROCESSOR(KTBasicASCIIWriter, "basic-ascii-writer");

    KTBasicASCIIWriter::KTBasicASCIIWriter(const std::string& name) :
            KTWriterWithTypists<KTBasicASCIIWriter, KTBasicASCIITypeWriter>(name),
            fOutputStream(NULL),
            fOutputFilename("basic_ascii_output.txt")
    {
    } // default constructor

    KTBasicASCIIWriter::~KTBasicASCIIWriter() {
        if( this->fOutputStream->is_open() ) this->fOutputStream->close();
        delete this->fOutputStream;
    }

    // Configuration method
    bool KTBasicASCIIWriter::Configure(const scarab::param_node* n) {
        if (n != NULL) {
            fOutputFilename = n->get_value< std::string >("output-file",fOutputFilename);
            if( this->OpenFile() == false ) {
                KTWARN(asciilog, "ASCII writer couldn't open output file - no data will be written!");
            }
        }

        return true;
    }

    bool KTBasicASCIIWriter::OpenFile() {
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

    // void KTBasicASCIIWriter::WriteFrequencySpectrumData(const KTFrequencySpectrumDataPolar* dt) {
    //   KTBundle* ev = dt->GetBundle();
    //   uint64_t evN = (ev == NULL) ? 0 : ev->GetBundleNumber();
    //   uint64_t nCh = dt->GetNComponents();

    //   if( fOutputStream && fOutputStream->is_open() ) {
    //     for( unsigned iCh = 0; iCh < nCh; iCh++ ) {
    // 	const KTFrequencySpectrumPolar* spectrum = dt->GetSpectrum(iCh);
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
}; // namespace Katydid
