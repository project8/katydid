#ifndef __KT_BASIC_ATWTS_HH
#define __KT_BASIC_ATWTS_HH

/*
 *  KTBasicASCIITypeWriterTS.hh
 *    author: kofron
 *   created: 1/9/2013
 *
 *  The TypeWriter which is responsible for writing Time Series data.
 */

#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"
#include "KTBasicASCIIWriter.hh"

namespace Katydid {

  class KTBasicASCIITypeWriterTS : public KTBasicASCIITypeWriter {
    
  public:
    friend class KTTimeSeriesData;

  public:
    KTBasicASCIITypeWriterTS();
    virtual ~KTBasicASCIITypeWriterTS();

    void RegisterSlots();
    
  public:
    void WriteTimeSeriesData(const KTTimeSeriesData* data);

  }; // class definition

};

#endif // __KT_BASIC_ATWTS_HH
