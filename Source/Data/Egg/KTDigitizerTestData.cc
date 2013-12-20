/*
 * KTDigitizerTestData.cc
 *
 *  Created on: Dec 18, 2013
 *      Author: nsoblath
 */

#include "KTDigitizerTestData.hh"

using std::vector;

namespace Katydid
{

    KTDigitizerTestData::KTDigitizerTestData() :
            KTExtensibleData< KTDigitizerTestData >(),
            fNBits(8),
            fBitOccupancyFlag(false),
            fBitOccupancyData(),
            fClippingFlag(false),
            fClippingData()
    {
    }

    KTDigitizerTestData::~KTDigitizerTestData()
    {
        while (! fBitOccupancyData.empty())
        {
            delete fBitOccupancyData.back().fBitHistogram;
            fBitOccupancyData.pop_back();
        }
    }

    unsigned KTDigitizerTestData::GetNComponents() const
    {
        return fClippingData.size();
    }

    KTDigitizerTestData& KTDigitizerTestData::SetNComponents(unsigned num)
    {
        fBitOccupancyData.resize(num);
        if (fNBits != 0)
        {
            for (unsigned iComponent = 0; iComponent < num; iComponent++)
            {
                fBitOccupancyData[iComponent].fBitHistogram = new KTCountHistogram(fNBits, -0.5, (double)fNBits - 0.5);
            }
        }
        fClippingData.resize(num);
        return *this;
    }

    void KTDigitizerTestData::SetNBits(unsigned nBits)
    {
        fNBits = nBits;

        // reset bit occupancy histograms
        for (vector< BitOccupancyData >::iterator it = fBitOccupancyData.begin(); it != fBitOccupancyData.end(); ++it)
        {
            delete (*it).fBitHistogram;
            (*it).fBitHistogram = new KTCountHistogram(fNBits, -0.5, (double)fNBits - 0.5);
        }

        return;
    }

    void KTDigitizerTestData::AddBits(unsigned value, unsigned component)
    {
        if (fBitOccupancyData[component].fBitHistogram == NULL)
            fBitOccupancyData[component].fBitHistogram = new KTCountHistogram(fNBits, -0.5, (double)fNBits - 0.5);
        unsigned bit = 0;
        while (value)
        {
            if (value & 1) fBitOccupancyData[component].fBitHistogram->Increment(bit);
            value >>= 1;
            ++bit;
        }
        return;
    }

    void KTDigitizerTestData::SetClippingData(unsigned nClipTop, unsigned nClipBottom, double topClipFrac, double bottomClipFrac, unsigned component)
    {
        if (component >= fClippingData.size())
            SetNComponents(component + 1);
        fClippingData[component].fNClipTop = nClipTop;
        fClippingData[component].fNClipBottom = nClipBottom;
        fClippingData[component].fTopClipFrac = topClipFrac;
        fClippingData[component].fBottomClipFrac = bottomClipFrac;
        return;
    }



} /* namespace Katydid */
