/*
 * KTWignerVille.cc
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#include "KTWignerVille.hh"

#include "KTComplexFFTW.hh"
#include "KTEggHeader.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
//#include "KTSlidingWindowFFTW.hh"
//#include "KTSlidingWindowFSData.hh"
//#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <set>

#include <iostream>

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
    KTLOGGER(wvlog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTWignerVille > sWVRegistrar("wigner-ville");

    KTWignerVille::KTWignerVille() :
            KTProcessor(),
            fFFT(new KTComplexFFTW()),
            fInputArray(new KTTimeSeriesFFTW(1,0.,1.)),
            fWVSignal()
    {
        fConfigName = "wigner-ville";

        RegisterSignal("wigner-ville", &fWVSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTWignerVille::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTWignerVille::ProcessTimeSeriesData, "void (shared_ptr< KTData >)");
    }

    KTWignerVille::~KTWignerVille()
    {
        delete fFFT;
    }

    Bool_t KTWignerVille::Configure(const KTPStoreNode* node)
    {
        const KTPStoreNode* childNode = node->GetChild("complex-fftw");
        if (childNode != NULL)
        {
            fFFT->Configure(childNode);
        }

        KTPStoreNode::csi_pair itPair = node->EqualRange("wv-pair");
        for (KTPStoreNode::const_sorted_iterator citer = itPair.first; citer != itPair.second; citer++)
        {
            string pairString(citer->second.get_value< string >());
            UInt_t first = 0, second = 0;
            Bool_t parsed = phrase_parse(pairString.begin(), pairString.end(),
                    (int_[ref(first)=boost::spirit::qi::_1] >> ',' >> int_[ref(second) = boost::spirit::qi::_1]),
                    space);
            if (! parsed)
            {
                KTWARN(wvlog, "Unable to parse WV pair: " << pairString);
                continue;
            }
            KTINFO(wvlog, "Adding WV pair " << first << ", " << second);
            this->AddPair(KTWVPair(first, second));
        }

        return true;
    }

    Bool_t KTWignerVille::TransformData(KTTimeSeriesData& data)
    {
        if (fPairs.empty())
        {
            KTWARN(wvlog, "No Wigner-Ville pairs specified; no transforms performed.");
            return false;
        }

        UInt_t nComponents = data.GetNComponents();

        // cast all time series into KTTimeSeriesFFTW
        vector< const KTTimeSeriesFFTW* > timeSeries(nComponents);
        for (UInt_t iTS=0; iTS < nComponents; iTS++)
        {
            timeSeries[iTS] = dynamic_cast< const KTTimeSeriesFFTW* >(data.GetTimeSeries(iTS));
            if (timeSeries[iTS] == NULL)
            {
                KTERROR(wvlog, "Time series " << iTS << " did not cast to a const KTTimeSeriesFFTW*. No transforms performed.");
                return false;
            }
        }

        KTWignerVilleData& newData = data.Of< KTWignerVilleData >().SetNComponents(nComponents);

        // Do WV transform for each pair
        UInt_t iPair = 0;
        for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
        {
            UInt_t firstChannel = (*pairIt).first;
            UInt_t secondChannel = (*pairIt).second;

            CrossMultiplyToInputArray(timeSeries[firstChannel], timeSeries[secondChannel], 0);

            KTFrequencySpectrumFFTW* newSpectrum = fFFT->Transform(fInputArray);
            newSpectrum->SetRange(0.5 * newSpectrum->GetRangeMin(), 0.5 * newSpectrum->GetRangeMax());

            newData.SetSpectrum(newSpectrum, iPair);
            newData.SetInputPair(firstChannel, secondChannel, iPair);
            iPair++;
        }

        return newData;
    }

    void KTWignerVille::CrossMultiplyToInputArray(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset)
    {
        UInt_t size = data1->size();
        if (fInputArray->size() != /*2*/size)
        {
            delete fInputArray;
            fInputArray = new KTTimeSeriesFFTW(/*2*/size, data1->GetRangeMin(), data1->GetRangeMax() + 0.5 * data1->GetBinWidth());
            KTWARN(wvlog, "Setting the input array size to " << 2 * size);
        }
        else
        {
            fInputArray->SetRange(data1->GetRangeMin(), data1->GetRangeMax());
        }
        /*  // For non-interpolating version
        UInt_t iPoint1 = offset;
        UInt_t iPoint2 = size - 1 + offset;
        */
        /**/ // For the interpolating version
        Double_t real1, real2, imag1, imag2;

        /*
        real1 = (*data1)(offset)[0];
        imag1 = (*data1)(offset)[1];
        real2 = (*data2)(size - 1 + offset)[0] + 0.5 * ((*data2)(size - 1 + offset)[0] - (*data2)(size - 2 + offset)[0]);
        imag2 = (*data2)(size - 1 + offset)[1] + 0.5 * ((*data2)(size - 1 + offset)[1] - (*data2)(size - 2 + offset)[1]);
        (*fInputArray)(0)[0] = real1 * real2 + imag1 * imag2;
        (*fInputArray)(0)[1] = imag1 * real2 - real1 * imag2;
        */
        UInt_t iPoint1 = offset + 1;
        UInt_t iPoint2 = size - 1 + offset;
        /**/
        //for (UInt_t inPoint = 2; inPoint < 2*size; inPoint+=2)
        for (UInt_t inPoint = 0; inPoint < size; inPoint++)
        {
            /**/  // Non-interpolating
            (*fInputArray)(inPoint)[0] = (*data1)(iPoint1)[0] * (*data2)(iPoint2)[0] + (*data1)(iPoint1)[1] * (*data2)(iPoint2)[1];
            (*fInputArray)(inPoint)[1] = (*data1)(iPoint1)[1] * (*data2)(iPoint2)[0] - (*data1)(iPoint1)[0] * (*data2)(iPoint2)[1];
            /**/
            /*
            fInputArray[inPoint][0] = (*data1)(iPoint1)[0] * (*data2)(iPoint2)[0] + (*data1)(iPoint1)[1] * (*data2)(iPoint2)[1];
            fInputArray[inPoint][1] = (*data1)(iPoint1)[1] * (*data2)(iPoint2)[0] - (*data1)(iPoint1)[0] * (*data2)(iPoint2)[1];
            fInputArray[inPoint-1][0] = 0.5 * (fInputArray[inPoint-2][0] + fInputArray[inPoint][0]);
            fInputArray[inPoint-1][1] = 0.5 * (fInputArray[inPoint-2][1] + fInputArray[inPoint][1]);
            */
            /* // Interpolating
            real1 = (*data1)(iPoint1)[0];
            imag1 = (*data1)(iPoint1)[1];
            real2 = 0.5 * ((*data2)(iPoint2-1)[0] + (*data2)(iPoint2)[0]);
            imag2 = 0.5 * ((*data2)(iPoint2-1)[1] + (*data2)(iPoint2)[1]);
            (*fInputArray)(inPoint)[0] = real1 * real2 + imag1 * imag2;
            (*fInputArray)(inPoint)[1] = imag1 * real2 - real1 * imag2;
            real1 = 0.5 * ((*data1)(iPoint1-1)[0] + (*data1)(iPoint1)[0]);
            imag1 = 0.5 * ((*data1)(iPoint1-1)[1] + (*data1)(iPoint1)[1]);
            real2 = (*data2)(iPoint2)[0];
            imag2 = (*data2)(iPoint2)[1];
            (*fInputArray)(inPoint-1)[0] = real1 * real2 + imag1 * imag2;
            (*fInputArray)(inPoint-1)[1] = imag1 * real2 - real1 * imag2;
            */
            iPoint1++;
            iPoint2--;
        }
        /*
        real1 = (*data1)(size - 1 + offset)[0] + 0.5 * ((*data1)(size - 1 + offset)[0] - (*data1)(size - 2 + offset)[0]);
        imag1 = (*data1)(size - 1 + offset)[1] + 0.5 * ((*data1)(size - 1 + offset)[1] - (*data1)(size - 2 + offset)[1]);
        real2 = (*data2)(offset)[0];
        imag2 = (*data2)(offset)[1];
        (*fInputArray)(2*size - 1)[0] = real1 * real2 + imag1 * imag2;
        (*fInputArray)(2*size - 1)[1] = imag1 * real2 - real1 * imag2;
        */
        return;
    }


    void KTWignerVille::ProcessHeader(const KTEggHeader* header)
    {
        UInt_t nBins = /*2 */ header->GetSliceSize();
        fFFT->SetSize(nBins);
        fFFT->InitializeFFT();
        delete fInputArray;
        // the min/max range for the input array don't matter, so just use 0 and 1
        fInputArray = new KTTimeSeriesFFTW(nBins, 0., 1.);
        return;
    }

    void KTWignerVille::ProcessTimeSeriesData(shared_ptr< KTData > data)
    {
        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(wvlog, "No time series data was present");
            return;
        }
        if (! TransformData(data->Of< KTTimeSeriesData >()))
        {
            KTERROR(wvlog, "Something went wrong while performing a Wigner-Ville transform");
            return;
        }
        fWVSignal(data);
        return;
    }

} /* namespace Katydid */
