/**
 @file KTProcessor.hh
 @brief Contains KTProcessor
 @details KTProcessor is the processor base class
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTPROCESSOR_HH_
#define KTPROCESSOR_HH_

namespace Katydid
{

    class KTProcessor
    {
        public:
            KTProcessor();
            virtual ~KTProcessor();

            //virtual void RunProcess() = 0;
    };

} /* namespace Katydid */
#endif /* KTPROCESSOR_HH_ */
