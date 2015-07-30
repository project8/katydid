/*
 * KTTestProcessor.hh
 *
 *  Created on: Aug 15, 2012
 *      Author: nsoblath
 */

#ifndef KTTESTPROCESSOR_HH_
#define KTTESTPROCESSOR_HH_

#include "KTProcessor.hh"

namespace Nymph
{

    class KTTestProcessorA : public KTProcessor
    {
        public:
            typedef KTSignalConcept< void (int) >::signal TheSignal;

        public:
            KTTestProcessorA();
            virtual ~KTTestProcessorA();

            bool Configure(const KTParamNode* node);

            void EmitSignals(int);

        //private:
            TheSignal fTheSignal;
    };


    class KTTestProcessorB : public KTProcessor
    {
        public:
            KTTestProcessorB();
            virtual ~KTTestProcessorB();

            bool Configure(const KTParamNode* node);

            void Slot1(int);
            void Slot2(int);
    };


} /* namespace Nymph */
#endif /* KTTESTPROCESSOR_HH_ */
