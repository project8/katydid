#include "KTBasicASCIITypeWriterTS.hh"

#include "KTSliceHeader.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"



namespace Katydid 
{

    KTLOGGER(ats_log, "KTBasicASCIITypeWriterTS");

    static Nymph::KTTIRegistrar< KTBasicASCIITypeWriter, KTBasicASCIITypeWriterTS > sBATWReg;

    KTBasicASCIITypeWriterTS::KTBasicASCIITypeWriterTS() :
            KTBasicASCIITypeWriter()
    {/* no-op */}

    KTBasicASCIITypeWriterTS::~KTBasicASCIITypeWriterTS()
    {/* no-op */}

    void KTBasicASCIITypeWriterTS::RegisterSlots()
    {
        fWriter->RegisterSlot("ts",
                this,
                &KTBasicASCIITypeWriterTS::WriteTimeSeriesData);
    }

    void KTBasicASCIITypeWriterTS::WriteTimeSeriesData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
        unsigned nCh = tsData.GetNComponents();

        if( fWriter->CanWrite() == true ) {

            for(unsigned iCh = 0; iCh < nCh; iCh++) {
                std::ofstream* file_ptr = fWriter->GetStream();
                const KTTimeSeries* sCh = tsData.GetTimeSeries(iCh);
                if(sCh != NULL) {
                    for(unsigned iB = 0; iB < sCh->GetNTimeBins(); iB++) {
                        (*file_ptr) << sliceNumber
                                << ","
                                << iCh
                                << ","
                                << iB
                                << ","
                                << sCh->GetValue(iB)
                                << std::endl;
                    }
                }
                else {
                    KTWARN(ats_log, "Channel #" << iCh << " was missing from slice!  Logic error?");
                }
            }
        } // if CanWrite
        else {
            KTWARN(ats_log, "Writer for ASCII TS type-writer cannot write.  No data will be written!");
        } // if cannot write
    }

}; // namespace katydid
