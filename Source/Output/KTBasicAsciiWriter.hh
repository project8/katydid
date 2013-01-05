/*
 * KTBasicAsciiWriter.hh
 *
 *  Created on: Dec 17, 2012
 *      Author: kofron
 */

#ifndef __KT_ASCII_WRITER_HH
#define __KT_ASCII_WRITER_HH

#include "KTWriter.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"

#include <fstream>

namespace Katydid {

  class KTBasicAsciiWriter : public KTWriterWithTypists< KTBasicAsciiWriter > {
    // Constructors/destructors
  public:
    KTBasicAsciiWriter();
    virtual ~KTBasicAsciiWriter();

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
    void Publish(const KTWriteableData* dt);
    void Write(const KTWriteableData* dt);

    // Unused write methods (for now)
    void Write(const KTCorrelationData* dt);
    void Write(const KTSlidingWindowFSData* dt);
    void Write(const KTSlidingWindowFSDataFFTW* dt);

    // Writing Frequency Spectrum Data
    void WriteFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* dt);
    void WriteFrequencySpectrumData(const KTFrequencySpectrumData* dt);
    void Write(const KTFrequencySpectrumData* dt);
    void Write(const KTFrequencySpectrumDataFFTW* dt);

    // Writing time series Data
    void WriteTimeSeriesData(const KTTimeSeriesData* ts);
    void Write(const KTTimeSeriesData* ts);

  }; // class KTBasicAsciiWriter 
}; // namespace Katydid


#endif // __KT_ASCII_WRITER_HH
