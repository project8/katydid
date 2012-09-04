/*
 * KTCorrelator.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#include "KTCorrelator.hh"

#include "KTCorrelationData.hh"
#include "KTEvent.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTLogger.hh"

using std::pair;
using std::vector;

namespace Katydid
{
    KTLOGGER(corrlog, "katydid.analysis");

    KTCorrelator::KTCorrelator() :
            KTProcessor(),
            KTConfigurable(),
            fPairs(),
            fCorrSignal()
    {
        RegisterSignal("correlation", &fCorrSignal);

        RegisterSlot("fft-data", this, &KTCorrelator::ProcessFFTData);
        RegisterSlot("event", this, &KTCorrelator::ProcessEvent);
    }

    KTCorrelator::~KTCorrelator()
    {
    }

    Bool_t KTCorrelator::Configure(const KTPStoreNode* node)
    {
        return true;
    }

    KTCorrelationData* KTCorrelator::Correlate(const KTFrequencySpectrumData* data, const KTCorrelationPair& pair)
    {
        UInt_t firstChannel = pair.first;
        UInt_t secondChannel = pair.second;
        KTFrequencySpectrum* result = DoCorrelation(data->GetSpectrum(firstChannel), data->GetSpectrum(secondChannel));

        if (result != NULL)
        {
            KTDEBUG(corrlog, "Correlation between channels " << firstChannel << " and " << secondChannel << " complete");

            KTCorrelationData* newData = new KTCorrelationData();
            newData->SetCorrelation(result, firstChannel, secondChannel, 0);
            data->GetEvent()->AddData(newData);

            newData->SetEvent(data->GetEvent());

            fCorrSignal(newData);

            return newData;
        }

        KTWARN(corrlog, "Something went wrong with the correlation of channels " << firstChannel << " and " << secondChannel);
        return NULL;

    }

    KTCorrelationData* KTCorrelator::Correlate(const KTFrequencySpectrumData* data, const PairVector& pairs)
    {
        KTCorrelationData* newData = new KTCorrelationData();

        for (PairVector::const_iterator iter = pairs.begin(); iter != pairs.end(); iter++)
        {
            UInt_t firstChannel = (*iter).first;
            UInt_t secondChannel = (*iter).second;
            KTFrequencySpectrum* result = DoCorrelation(data->GetSpectrum(firstChannel), data->GetSpectrum(secondChannel));
            if (result == NULL)
            {
                KTWARN(corrlog, "Something went wrong with the correlation of channels " << firstChannel << " and " << secondChannel);
            }
            else
            {
                newData->SetCorrelation(result, firstChannel, secondChannel, 0);
            }
        }

        newData->SetEvent(data->GetEvent());

        //data->GetEvent()->AddData(newData);
        fCorrSignal(newData);

        KTDEBUG(corrlog, "Correlations complete");
        return newData;
    }

    KTFrequencySpectrum* KTCorrelator::DoCorrelation(const KTFrequencySpectrum* firstSpectrum, const KTFrequencySpectrum* secondSpectrum)
    {
        // Performs cc(firstSpectrum) * secondSpectrum
        KTFrequencySpectrum* newSpect = new KTFrequencySpectrum(*firstSpectrum);
        (*newSpect) *= (*secondSpectrum);
        return newSpect;
    }

    void KTCorrelator::ProcessFFTData(const KTFrequencySpectrumData* tsData)
    {
        KTCorrelationData* newData = Correlate(tsData, fPairs);
        tsData->GetEvent()->AddData(newData);
        return;
    }

    void KTCorrelator::ProcessEvent(KTEvent* event)
    {
        const KTFrequencySpectrumData* fsData = dynamic_cast< KTFrequencySpectrumData* >(event->GetData(KTFrequencySpectrumData::StaticGetName()));
        if (fsData == NULL)
        {
            KTWARN(corrlog, "No time series data was available in the event");
            return;
        }
        KTCorrelationData* newData = Correlate(fsData, fPairs);
        event->AddData(newData);
        return;
    }



} /* namespace Katydid */
