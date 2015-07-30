/*
 * KTWriter.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTWRITER_HH_
#define KTWRITER_HH_

#include "KTProcessor.hh"

#include "KTTIFactory.hh"

#include <map>
#include <typeinfo>

namespace Nymph
{
    class KTTypeWriter
    {
        public:
            KTTypeWriter();
            virtual ~KTTypeWriter();

            virtual void RegisterSlots() = 0;
    };


    template< class XWriter >
    class KTDerivedTypeWriter : public KTTypeWriter
    {
        public:
            KTDerivedTypeWriter();
            virtual ~KTDerivedTypeWriter();

            void SetWriter(XWriter* writer);

        protected:
            XWriter* fWriter;
    };


    template< class XWriter >
    KTDerivedTypeWriter< XWriter >::KTDerivedTypeWriter() :
            KTTypeWriter(),
            fWriter(NULL)
    {
    }

    template< class XWriter >
    KTDerivedTypeWriter< XWriter >::~KTDerivedTypeWriter()
    {
    }

    template< class XWriter >
    void KTDerivedTypeWriter< XWriter >::SetWriter(XWriter* writer)
    {
        fWriter = writer;
        return;
    }




    class KTWriter : public KTProcessor
    {
        public:
            KTWriter(const std::string& name = "default-writer-name");
            virtual ~KTWriter();

    };

    template< class XWriter, class XTypist >
    class KTWriterWithTypists : public KTWriter
    {
        protected:
            typedef std::map< const std::type_info*, XTypist* > TypeWriterMap;
        public:
            KTWriterWithTypists(const std::string& name = "default-writer-with-typists-name");
            virtual ~KTWriterWithTypists();

            template< class XTypeWriter >
            XTypeWriter* GetTypeWriter();

        protected:
            TypeWriterMap fTypeWriters;

    };


    template< class XWriter, class XTypist >
    KTWriterWithTypists< XWriter, XTypist >::KTWriterWithTypists(const std::string& name) :
            KTWriter(name),
            fTypeWriters()
    {
        KTTIFactory< XTypist >* twFactory = KTTIFactory< XTypist >::GetInstance();
        for (typename KTTIFactory< XTypist >::FactoryCIt factoryIt = twFactory->GetFactoryMapBegin();
                factoryIt != twFactory->GetFactoryMapEnd();
                factoryIt++)
        {
            XTypist* newTypeWriter = twFactory->Create(factoryIt);
            newTypeWriter->SetWriter(static_cast< XWriter* >(this));
            newTypeWriter->RegisterSlots();
            fTypeWriters.insert(typename TypeWriterMap::value_type(factoryIt->first, newTypeWriter));
        }
    }

    template< class XWriter, class XTypist >
    KTWriterWithTypists< XWriter, XTypist >::~KTWriterWithTypists()
    {
        while (! fTypeWriters.empty())
        {
            delete fTypeWriters.begin()->second;
            fTypeWriters.erase(fTypeWriters.begin());
        }
    }

    template< class XWriter, class XTypist >
    template< class XTypeWriter >
    XTypeWriter* KTWriterWithTypists< XWriter, XTypist >::GetTypeWriter()
    {
        typename TypeWriterMap::const_iterator it = fTypeWriters.find(&typeid(XTypeWriter));
        if (it == fTypeWriters.end())
        {
            return NULL;
        }
        return static_cast< XTypeWriter* >(it->second);
    }


#define KT_REGISTER_WRITER(writer_class, writer_name) \
        static ::Nymph::KTNORegistrar< ::Nymph::KTWriter, writer_class > s##writer_class##WriterRegistrar(writer_name);

} /* namespace Nymph */
#endif /* KTWRITER_HH_ */
