/*
 * KTCorrelator.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#include "KTCorrelator.hh"

#include "KTFactory.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

using std::pair;
using std::string;
using std::vector;

using boost::shared_ptr;

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

    static KTDerivedRegistrar< KTProcessor, KTCorrelator > sCorrelatorRegistrar("correlator");

    KTCorrelator::KTCorrelator() :
            KTProcessor(),
            fPairs(),
            fCorrSignal()
    {
        fConfigName = "correlator";

        RegisterSignal("correlation", &fCorrSignal, "void (shared_ptr<KTData>)");

        RegisterSlot("fft-data", this, &KTCorrelator::ProcessFFTData, "void (shared_ptr<KTData>)");
        RegisterSlot("fftw-data", this, &KTCorrelator::ProcessFFTWData, "void (shared_ptr<KTData>)");
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

    Bool_t KTCorrelator::Correlate(KTFrequencySpectrumDataPolar& data)
    {
        KTCorrelationData& newData = data.Of< KTCorrelationData >().SetNComponents(fPairs.size());

        UInt_t iPair = 0;
        for (PairVector::const_iterator iter = fPairs.begin(); iter != fPairs.end(); iter++)
        {
            UInt_t firstChannel = (*iter).first;
            UInt_t secondChannel = (*iter).second;
            KTFrequencySpectrumPolar* result = DoCorrelation(data.GetSpectrumPolar(firstChannel), data.GetSpectrumPolar(secondChannel));
            if (result == NULL)
            {
                KTWARN(corrlog, "Something went wrong with the correlation of channels " << firstChannel << " and " << secondChannel);
            }
            else
            {
                newData.SetSpectrum(result, iPair);
                newData.SetInputPair(firstChannel, secondChannel, iPair);
            }
            iPair++;
        }

        KTDEBUG(corrlog, "Correlations complete; " << iPair << " channel-pairs correlated.");
        return true;
    }

    Bool_t KTCorrelator::Correlate(KTFrequencySpectrumDataFFTW& data)
    {
        KTCorrelationData& newData = data.Of< KTCorrelationData >().SetNComponents(fPairs.size());

        UInt_t iPair = 0;
        for (PairVector::const_iterator iter = fPairs.begin(); iter != fPairs.end(); iter++)
        {
            UInt_t firstChannel = (*iter).first;
            UInt_t secondChannel = (*iter).second;
            KTFrequencySpectrumPolar* result = DoCorrelation(data.GetSpectrumFFTW(firstChannel), data.GetSpectrumFFTW(secondChannel));
            if (result == NULL)
            {
                KTWARN(corrlog, "Something went wrong with the correlation of channels " << firstChannel << " and " << secondChannel);
            }
            else
            {
                newData.SetSpectrum(result, iPair);
                newData.SetInputPair(firstChannel, secondChannel, iPair);
            }
            iPair++;
        }

        KTDEBUG(corrlog, "Correlations complete; " << iPair << " channel-pairs correlated.");
        return true;
    }
/*
    KTCorrelationData* KTCorrelator::Correlate(const KTFrequencySpectrumDataPolar* data, const PairVector& pairs)
    {
        KTCorrelationData* newData = new KTCorrelationData();

        for (PairVector::const_iterator iter = pairs.begin(); iter != pairs.end(); iter++)
        {
            UInt_t firstChannel = (*iter).first;
            UInt_t secondChannel = (*iter).second;
            KTFrequencySpectrumPolar* result = DoCorrelation(data->GetSpectrumPolar(firstChannel), data->GetSpectrumPolar(secondChannel));
            if (result == NULL)
            {
                KTWARN(corrlog, "Something went wrong with the correlation of channels " << firstChannel << " and " << secondChannel);
            }
            else
            {
                newData->SetCorrelation(result, firstChannel, secondChannel, 0);
            }
        }

        newData->SetBundle(data->GetBundle());

        //data->GetBundle()->AddData(newData);
        fCorrSignal(newData);

        KTDEBUG(corrlog, "Correlations complete");
        return newData;
    }

    KTCorrelationData* KTCorrelator::Correlate(const KTFrequencySpectrumDataPolar* data, const KTCorrelationPair& pair)
    {
        UInt_t firstChannel = pair.first;
        UInt_t secondChannel = pair.second;
        KTFrequencySpectrumPolar* result = DoCorrelation(data->GetSpectrumPolar(firstChannel), data->GetSpectrumPolar(secondChannel));

        if (result != NULL)
        {
            KTDEBUG(corrlog, "Correlation between channels " << firstChannel << " and " << secondChannel << " complete");

            KTCorrelationData* newData = new KTCorrelationData();
            newData->SetCorrelation(result, firstChannel, secondChannel, 0);
            data->GetBundle()->AddData(newData);

            newData->SetBundle(data->GetBundle());

            fCorrSignal(newData);

            return newData;
        }

        KTWARN(corrlog, "Something went wrong with the correlation of channels " << firstChannel << " and " << secondChannel);
        return NULL;

    }

    */
    KTFrequencySpectrumPolar* KTCorrelator::DoCorrelation(const KTFrequencySpectrumPolar* firstSpectrum, const KTFrequencySpectrumPolar* secondSpectrum)
    {
        // Performs cc(firstSpectrum) * secondSpectrum
        KTFrequencySpectrumPolar* newSpect = new KTFrequencySpectrumPolar(*firstSpectrum);
        newSpect->CConjugate();
        (*newSpect) *= (*secondSpectrum);
        return newSpect;
    }

    KTFrequencySpectrumPolar* KTCorrelator::DoCorrelation(const KTFrequencySpectrumFFTW* firstSpectrum, const KTFrequencySpectrumFFTW* secondSpectrum)
    {
        // Performs cc(firstSpectrum) * secondSpectrum
        KTFrequencySpectrumFFTW newSpectFFTW(*firstSpectrum);
        newSpectFFTW.CConjugate();
        newSpectFFTW *= (*secondSpectrum);

        return newSpectFFTW.CreateFrequencySpectrum();
    }

    void KTCorrelator::ProcessFFTData(shared_ptr< KTData > data)
    {
        if (! data->Has< KTFrequencySpectrumDataPolar >())
        {
            KTERROR(corrlog, "No polar frequency spectrum data was present");
            return;
        }
        if (! Correlate(data->Of< KTFrequencySpectrumDataPolar >()))
        {
            KTERROR(corrlog, "Something went wrong while performing the correlation(s)");
            return;
        }
        fCorrSignal(data);
        return;
    }

    void KTCorrelator::ProcessFFTWData(shared_ptr< KTData > data)
    {
        if (! data->Has< KTFrequencySpectrumDataFFTW >())
        {
            KTERROR(corrlog, "No polar frequency spectrum data was present");
            return;
        }
        if (! Correlate(data->Of< KTFrequencySpectrumDataFFTW >()))
        {
            KTERROR(corrlog, "Something went wrong while performing the correlation(s)");
            return;
        }
        fCorrSignal(data);
        return;
    }

} /* namespace Katydid */
