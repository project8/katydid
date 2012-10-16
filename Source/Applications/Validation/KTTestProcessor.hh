/*
 * KTTestProcessor.hh
 *
 *  Created on: Aug 15, 2012
 *      Author: nsoblath
 */

#ifndef KTTESTPROCESSOR_HH_
#define KTTESTPROCESSOR_HH_

#include "KTProcessor.hh"

namespace Katydid
{

    class KTTestProcessorA : public KTProcessor
    {
        public:
            typedef KTSignal< void (int) >::signal TheSignal;

        public:
            KTTestProcessorA();
            virtual ~KTTestProcessorA();

            Bool_t Configure(const KTPStoreNode* node);

            void EmitSignals(int);

        //private:
            TheSignal fTheSignal;
    };


    class KTTestProcessorB : public KTProcessor
    {
        public:
            KTTestProcessorB();
            virtual ~KTTestProcessorB();

            Bool_t Configure(const KTPStoreNode* node);

            void Slot1(int);
            void Slot2(int);
    };


} /* namespace Katydid */
#endif /* KTTESTPROCESSOR_HH_ */
