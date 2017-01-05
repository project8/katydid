#ifndef __KT_BASIC_ATWTS_HH
#define __KT_BASIC_ATWTS_HH

/*
 *  KTBasicASCIITypeWriterTS.hh
 *    author: kofron
 *   created: 1/9/2013
 *
 *  The TypeWriter which is responsible for writing Time Series data.
 */

//#include "KTTimeSeries.hh"
//#include "KTTimeSeriesData.hh"
#include "KTBasicAsciiWriter.hh"

#include "KTData.hh"

namespace Katydid {
    class KTBasicASCIITypeWriterTS : public KTBasicASCIITypeWriter
    {

        public:
            KTBasicASCIITypeWriterTS();
            virtual ~KTBasicASCIITypeWriterTS();
    
            void RegisterSlots();
    
        public:
            void WriteTimeSeriesData(Nymph::KTDataPtr);

    }; // class definition

};

#endif // __KT_BASIC_ATWTS_HH
