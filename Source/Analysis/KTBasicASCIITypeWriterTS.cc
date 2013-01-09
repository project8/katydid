#include "KTBasicASCIITypeWriterTS.hh"

namespace Katydid 
{
  
  KTLOGGER(ats_log, "katydid.output.ascii.tw.ts");

  static KTDerivedTIRegistrar< KTBasicASCIITypeWriter, KTBasicASCIITypeWriterTS > sBATWReg;

  KTBasicASCIITypeWriterTS::KTBasicASCIITypeWriterTS() :
    KTBasicASCIITypeWriter()
  {/* no-op */}

  KTBasicASCIITypeWriterTS::~KTBasicASCIITypeWriterTS()
  {/* no-op */}

  void KTBasicASCIITypeWriterTS::RegisterSlots() 
  {
    fWriter->RegisterSlot("ts-data", 
			  this,
			  &KTBasicASCIITypeWriterTS::WriteTimeSeriesData,
			  "void (const KTTimeSeriesData*)");
  }

  void KTBasicASCIITypeWriterTS::WriteTimeSeriesData(const KTTimeSeriesData* data)
  {
    KTEvent* event = data->GetEvent();
    UInt_t eventNumber = (event == NULL) ? 0 : event->GetEventNumber();
    UInt_t nCh = data->GetNTimeSeries();

    if( fWriter->CanWrite() == true ) {

      for(unsigned iCh = 0; iCh < nCh; iCh++) {
	std::ofstream* file_ptr = fWriter->GetStream();
	const KTTimeSeries* sCh = data->GetTimeSeries(iCh);
	if(sCh != NULL) {
	  (*file_ptr) << "hi" << std::endl;
	}
	else {
	  KTWARN(ats_log, "Channel #" << iCh << " was missing from event!  Logic error?");
	}
      }

    } // if CanWrite
    else {
      KTWARN(ats_log, "Writer for ASCII TS type-writer cannot write.  No data will be written!");
    } // if cannot write
  }

}; // namespace katydid
