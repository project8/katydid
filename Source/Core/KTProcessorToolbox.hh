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

#include <deque>

namespace Katydid
{

    class KTProcessorToolbox : public KTConfigurable
    {
        public:
            KTProcessorToolbox();
            virtual ~KTProcessorToolbox();

        protected:
            KTFactory< KTProcessor >* fProcFactory; // singleton; not owned by KTProcessorToolbox

        public:
            Bool_t Configure(const KTPStoreNode* node);

            Bool_t ConfigureProcessors(const KTPStoreNode* node);

        public:
            Bool_t Run();

        protected:
            std::deque< KTProcessor* > fRunQueue;

        protected:
            struct ProcessorInfo
            {
                KTProcessor* fProc;
                Bool_t fIsTopLevel;
            };
            typedef std::map< std::string, ProcessorInfo > ProcessorMap;
            typedef ProcessorMap::iterator ProcMapIt;
            typedef ProcessorMap::const_iterator ProcMapCIt;
            typedef ProcessorMap::value_type ProcMapValue;

        public:
            KTProcessor* GetProcessor(const std::string& procName);
            const KTProcessor* GetProcessor(const std::string& procName) const;
            Bool_t AddProcessor(const std::string& procName, KTProcessor* proc, Bool_t isTopLevel=false);
            Bool_t RemoveProcessor(const std::string& procName);
            KTProcessor* ReleaseProcessor(const std::string& procName);
            void ClearProcessors();

        protected:
            ProcessorMap fProcMap;

    };

} /* namespace Katydid */
#endif /* KTPROCESSORTOOLBOX_HH_ */
