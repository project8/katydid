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
#include "KTPStoreNode.hh"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

using std::pair;
using std::string;
using std::vector;

// I can't just use boost::spirit::qi because of naming conflicts with std
using boost::spirit::qi::int_;
// I had to take this out because of a naming conflict with boost::bind
//using boost::spirit::qi::_1;
using boost::spirit::qi::phrase_parse;
using boost::spirit::ascii::space;
using boost::phoenix::ref;

namespace Katydid
{
    KTLOGGER(corrlog, "katydid.analysis");

    KTCorrelator::KTCorrelator() :
            KTProcessor(),
            KTConfigurable(),
            fPairs(),
            fCorrSignal()
    {
        fConfigName = "correlator";

        RegisterSignal("correlation", &fCorrSignal, "void (const KTWriteableData*)");

        RegisterSlot("fft-data", this, &KTCorrelator::ProcessFFTData, "void (const KTFrequencySpectrumData*)");
        RegisterSlot("event", this, &KTCorrelator::ProcessEvent, "void (KTEvent*)");
    }

    KTCorrelator::~KTCorrelator()
    {
    }

    Bool_t KTCorrelator::Configure(const KTPStoreNode* node)
    {
        KTPStoreNode::csi_pair itPair = node->EqualRange("corr-pair");
        for (KTPStoreNode::const_sorted_iterator citer = itPair.first; citer != itPair.second; citer++)
        {
            string pairString(citer->second.get_value< string >());
            UInt_t first = 0, second = 0;
            Bool_t parsed = phrase_parse(pairString.begin(), pairString.end(),
                    (int_[ref(first)=boost::spirit::qi::_1] >> ',' >> int_[ref(second) = boost::spirit::qi::_1]),
                    space);
            if (! parsed)
            {
                KTWARN(corrlog, "Unable to parse correlation pair: " << pairString);
                continue;
            }
            KTINFO(corrlog, "Adding correlation pair " << first << ", " << second);
            this->AddPair(KTCorrelationPair(first, second));
        }

        return true;
    }

    KTCorrelationData* KTCorrelator::Correlate(const KTFrequencySpectrumData* data)
    {
        KTCorrelationData* newData = new KTCorrelationData();
        newData->SetNPairs(fPairs.size());

        UInt_t iPair = 0;
        for (PairVector::const_iterator iter = fPairs.begin(); iter != fPairs.end(); iter++)
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
                newData->SetCorrelation(result, firstChannel, secondChannel, iPair);
            }
            iPair++;
        }

        newData->SetEvent(data->GetEvent());

        //data->GetEvent()->AddData(newData);
        fCorrSignal(newData);

        KTDEBUG(corrlog, "Correlations complete; " << iPair << " channel-pairs correlated.");
        return newData;
    }
    /*
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

    */
    KTFrequencySpectrum* KTCorrelator::DoCorrelation(const KTFrequencySpectrum* firstSpectrum, const KTFrequencySpectrum* secondSpectrum)
    {
        // Performs cc(firstSpectrum) * secondSpectrum
        KTFrequencySpectrum* newSpect = new KTFrequencySpectrum(*firstSpectrum);
        newSpect->CConjugate();
        (*newSpect) *= (*secondSpectrum);
        return newSpect;
    }

    void KTCorrelator::ProcessFFTData(const KTFrequencySpectrumData* tsData)
    {
        KTCorrelationData* newData = Correlate(tsData);
        tsData->GetEvent()->AddData(newData);
        return;
    }

    void KTCorrelator::ProcessEvent(KTEvent* event)
    {
        const KTFrequencySpectrumData* fsData = dynamic_cast< KTFrequencySpectrumData* >(event->GetData(KTFrequencySpectrumData::StaticGetName()));
        if (fsData == NULL)
        {
            KTWARN(corrlog, "No frequency-spectrum data was available in the event");
            return;
        }
        KTCorrelationData* newData = Correlate(fsData);
        event->AddData(newData);
        return;
    }



} /* namespace Katydid */
