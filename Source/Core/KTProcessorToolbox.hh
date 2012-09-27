/*
 * KTProcessorToolbox.hh
 *
 *  Created on: Sep 27, 2012
 *      Author: nsoblath
 */

#ifndef KTPROCESSORTOOLBOX_HH_
#define KTPROCESSORTOOLBOX_HH_

#include "KTConfigurable.hh"
#include "KTFactory.hh"

#include "KTProcessor.hh"

namespace Katydid
{

    class KTProcessorToolbox : public KTConfigurable, public KTFactory< KTProcessor >
    {
        public:
            KTProcessorToolbox();
            virtual ~KTProcessorToolbox();

        public:
            Bool_t Configure(const KTPStoreNode* node);

        protected:
            typedef std::map< std::string, KTProcessor* > ProcessorMap;
            typedef ProcessorMap::iterator ProcMapIt;
            typedef ProcessorMap::const_iterator ProcMapCIt;
            typedef ProcessorMap::value_type ProcMapValue;

        public:
            KTProcessor* GetProcessor(const std::string& procName);
            const KTProcessor* GetProcessor(const std::string& procName) const;
            Bool_t AddProcessor(const std::string& procName, KTProcessor* proc);
            Bool_t RemoveProcessor(const std::string& procName);
            KTProcessor* ReleaseProcessor(const std::string& procName);
            void ClearProcessors();

        protected:
            ProcessorMap fProcMap;

    };

} /* namespace Katydid */
#endif /* KTPROCESSORTOOLBOX_HH_ */
