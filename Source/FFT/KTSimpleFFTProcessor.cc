/*
 * KTSimpleFFTProcessor.cc
 *
 *  Created on: Jan 5, 2012
 *      Author: nsoblath
 */

#include "KTSimpleFFTProcessor.hh"

#include "KTSimpleFFT.hh"

#include <iostream>
using std::cout;
using std::endl;

using std::string;

namespace Katydid
{

    KTSimpleFFTProcessor::KTSimpleFFTProcessor() :
            fFFT()
    {
    }

    KTSimpleFFTProcessor::~KTSimpleFFTProcessor()
    {
        fHeaderConnection.disconnect();
        fEventConnection.disconnect();
    }

    void KTSimpleFFTProcessor::ProcessHeader(KTEgg::HeaderInfo headerInfo)
    {
        fFFT.SetTimeSize(headerInfo.fRecordSize);
        fFFT.InitializeFFT();
        return;
    }

    void KTSimpleFFTProcessor::ProcessEvent(UInt_t iEvent, const KTEvent* event)
    {
        if (fFFT.TakeData(event))
        {
            cout << "Data transferred; performing transform" << endl;
            fFFT.Transform();
            fFFTSignal(iEvent, &fFFT);
        }
        return;
    }

} /* namespace Katydid */
