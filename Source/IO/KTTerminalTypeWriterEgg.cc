/*
 * KTTerminalTypeWriterEgg.cc
 *
 *  Created on: Dec 20, 2013
 *      Author: nsoblath
 */

#include "KTTerminalTypeWriterEgg.hh"

#include "KTDigitizerTestData.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include "thorax.hh"

#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    static KTTIRegistrar< KTTerminalTypeWriter, KTTerminalTypeWriterEgg > sTermTWERegistrar;

    KTTerminalTypeWriterEgg::KTTerminalTypeWriterEgg() :
            KTTerminalTypeWriter()
    {
    }

    KTTerminalTypeWriterEgg::~KTTerminalTypeWriterEgg()
    {
    }


    void KTTerminalTypeWriterEgg::RegisterSlots()
    {
        fWriter->RegisterSlot("ts", this, &KTTerminalTypeWriterEgg::WriteTimeSeriesData);
        fWriter->RegisterSlot("dig", this, &KTTerminalTypeWriterEgg::WriteDigitizerTestData);
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTTerminalTypeWriterEgg::WriteTimeSeriesData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesData& tsData = data->Of< KTTimeSeriesData >();
        unsigned nComponents = tsData.GetNComponents();

        KTPROG(termlog, "Slice " << sliceNumber << " (" << nComponents << " components)\n" <<
                        "-----");

        for (unsigned iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                stringstream toTerm;
                toTerm << "Component " << iComponent << '\n';
                unsigned nBins = spectrum->GetNTimeBins();
                for (unsigned iBin = 0; iBin < nBins; ++iBin)
                {
                    //toTerm << to_char((data_type)spectrum->GetValue(iBin));
                }
                KTPROG(termlog, toTerm.str());
            }
        }
        return;
    }

    void KTTerminalTypeWriterEgg::WriteDigitizerTestData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTDigitizerTestData& tsData = data->Of< KTDigitizerTestData >();
        unsigned nComponents = tsData.GetNComponents();

        KTPROG(termlog, "Slice " << sliceNumber << " (" << nComponents << " components)\n" <<
                        "------");

        // Bit Occupancy

        // Clipping
        stringstream toTermClipping;
        toTermClipping << "Clipping Test\n";
        for (unsigned iComponent=0; iComponent<nComponents; iComponent++)
        {
            toTermClipping << "Component " << iComponent << '\n';
            toTermClipping << "\t\t\tNumber\tFraction\n";
            toTermClipping << "\tTop   \t" << tsData.GetNClipTop(iComponent) << '\t' << tsData.GetTopClipFrac(iComponent) << '\n';
            toTermClipping << "\tBottom\t" << tsData.GetNClipBottom(iComponent) << '\t' << tsData.GetBottomClipFrac(iComponent) << '\n';
            toTermClipping << '\n';
            KTPROG(termlog, toTermClipping.str());
        }
        return;
    }

} /* namespace Katydid */
