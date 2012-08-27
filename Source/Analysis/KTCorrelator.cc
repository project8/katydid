/*
 * KTCorrelator.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#include "KTCorrelator.hh"

#include "KTCorrelationData.hh"
#include "KTFFTTypes.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(corrlog, "katydid.analysis");

    KTCorrelator::KTCorrelator()
    {
    }

    KTCorrelator::~KTCorrelator()
    {
    }

    Bool_t KTCorrelator::Correlate(const KTFrequencySpectrumData* data, KTCorrelationPair pair)
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
            fCorrelationSignal(newData);

            return true;
        }

        KTWARN(corrlog, "Something went wrong with the correlation of channels " << firstChannel << " and " << secondChannel);
        return false;

    }

    Bool_t KTCorrelator::Correlate(const KTFrequencySpectrumData* data, std::vector< KTCorrelationPair > pairs)
    {
        typedef std::vector< KTCorrelationPair > pairVector;

        KTCorrelationData* newData = new KTCorrelationData();

        for (pairVector::const_iterator iter = pairs.begin(); iter != pairs.end(); iter++)
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
        fCorrelationSignal(newData);

        KTDEBUG(corrlog, "Correlations complete");
        return result;
    }

    KTFrequencySpectrum* KTCorrelator::DoCorrelation(const KTFrequencySpectrum* firstSpectrum, const KTFrequencySpectrum* secondSpectrum)
    {

    }


} /* namespace Katydid */
