/*
 * KTCorrelator.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#include "KTCorrelator.hh"

#include "KTCorrelationData.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTLogger.hh"

using std::pair;
using std::vector;

namespace Katydid
{
    KTLOGGER(corrlog, "katydid.analysis");

    KTCorrelator::KTCorrelator() :
            KTProcessor(),
            KTCorrelator(),
            fPairs(),
            fCorrSignal()
    {
    }

    KTCorrelator::~KTCorrelator()
    {
    }

    Bool_t KTCorrelator::Configure(const KTPStoreNode* node)
    {
        return true;
    }

    Bool_t KTCorrelator::Correlate(const KTFrequencySpectrumData* data, const KTCorrelationPair& pair)
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
            fCorrSignal(newData);

            return true;
        }

        KTWARN(corrlog, "Something went wrong with the correlation of channels " << firstChannel << " and " << secondChannel);
        return false;

    }

    Bool_t KTCorrelator::Correlate(const KTFrequencySpectrumData* data, const PairVector& pairs)
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

        data->GetEvent()->AddData(newData);
        fCorrSignal(newData);

        KTDEBUG(corrlog, "Correlations complete");
        return true;
    }

    KTFrequencySpectrum* KTCorrelator::DoCorrelation(const KTFrequencySpectrum* firstSpectrum, const KTFrequencySpectrum* secondSpectrum)
    {
        // temporary
        return new KTFrequencySpectrum();
    }

    void KTCorrelator::ProcessFFTData(const KTFrequencySpectrumData* tsData)
    {
        if (! Correlate(tsData, fPairs))
        {
            KTWARN(corrlog, "Correlation failed");
        }
        return;
    }
    /*
    void KTCorrelator::ProcessEvent(const KTEvent* event)
    {

    }
    */


} /* namespace Katydid */
