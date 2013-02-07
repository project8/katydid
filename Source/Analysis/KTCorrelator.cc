/*
 * KTCorrelator.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#include "KTCorrelator.hh"

#include "KTCorrelationData.hh"
#include "KTBundle.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumData.hh"
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

        RegisterSignal("correlation", &fCorrSignal, "void (const KTCorrelationData*)");

        RegisterSlot("fft-data", this, &KTCorrelator::ProcessFFTData, "void (const KTFrequencySpectrumData*)");
        RegisterSlot("fftw-data", this, &KTCorrelator::ProcessFFTWData, "void (const KTFrequencySpectrumDataFFTW*)");
        RegisterSlot("bundle", this, &KTCorrelator::ProcessBundle, "void (shared_ptr<KTBundle>)");
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

        SetInputDataName(node->GetData< string >("input-data-name", fInputDataName));
        SetOutputDataName(node->GetData< string >("output-data-name", fOutputDataName));

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
            KTFrequencySpectrumPolar* result = DoCorrelation(data->GetSpectrum(firstChannel), data->GetSpectrum(secondChannel));
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

        newData->SetTimeInRun(data->GetTimeInRun());
        newData->SetSliceNumber(data->GetSliceNumber());
        KTWARN(corrlog, "slice: " << data->GetSliceNumber() << "   " << newData->GetSliceNumber());

        newData->SetName(fOutputDataName);

        KTDEBUG(corrlog, "Correlations complete; " << iPair << " channel-pairs correlated.");
        return newData;
    }

    KTCorrelationData* KTCorrelator::Correlate(const KTFrequencySpectrumDataFFTW* data)
    {
        KTCorrelationData* newData = new KTCorrelationData();
        newData->SetNPairs(fPairs.size());

        UInt_t iPair = 0;
        for (PairVector::const_iterator iter = fPairs.begin(); iter != fPairs.end(); iter++)
        {
            UInt_t firstChannel = (*iter).first;
            UInt_t secondChannel = (*iter).second;
            KTFrequencySpectrumPolar* result = DoCorrelation(data->GetSpectrum(firstChannel), data->GetSpectrum(secondChannel));
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

        newData->SetTimeInRun(data->GetTimeInRun());
        newData->SetSliceNumber(data->GetSliceNumber());

        newData->SetName(fOutputDataName);

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
            KTFrequencySpectrumPolar* result = DoCorrelation(data->GetSpectrum(firstChannel), data->GetSpectrum(secondChannel));
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

    KTCorrelationData* KTCorrelator::Correlate(const KTFrequencySpectrumData* data, const KTCorrelationPair& pair)
    {
        UInt_t firstChannel = pair.first;
        UInt_t secondChannel = pair.second;
        KTFrequencySpectrumPolar* result = DoCorrelation(data->GetSpectrum(firstChannel), data->GetSpectrum(secondChannel));

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

    void KTCorrelator::ProcessFFTData(const KTFrequencySpectrumData* fsData)
    {
        KTCorrelationData* newData = Correlate(fsData);
        if (newData != NULL)
        {
            KTBundle* bundle = fsData->GetBundle();
            newData->SetBundle(bundle);
            if (bundle != NULL)
                bundle->AddData(newData);
            fCorrSignal(newData);
        }
        return;
    }

    void KTCorrelator::ProcessFFTWData(const KTFrequencySpectrumDataFFTW* fsData)
    {
        KTCorrelationData* newData = Correlate(fsData);
        if (newData != NULL)
        {
            KTBundle* bundle = fsData->GetBundle();
            newData->SetBundle(bundle);
            if (bundle != NULL)
                bundle->AddData(newData);
            fCorrSignal(newData);
        }
        return;
    }

    void KTCorrelator::ProcessBundle(shared_ptr<KTBundle> bundle)
    {
        const KTFrequencySpectrumData* fsData = bundle->GetData< KTFrequencySpectrumData >(fInputDataName);
        if (fsData != NULL)
        {
            ProcessFFTData(fsData);
            return;
        }

        const KTFrequencySpectrumDataFFTW* fsDataFFTW = bundle->GetData< KTFrequencySpectrumDataFFTW >(fInputDataName);
        if (fsDataFFTW != NULL)
        {
            ProcessFFTWData(fsDataFFTW);
            return;
        }

        KTWARN(corrlog, "No frequency-spectrum data named <" << fInputDataName << "> was available in the bundle");
        return;

    }



} /* namespace Katydid */
