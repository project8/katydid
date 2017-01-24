/*
 * KTJSONMaker.hh
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#ifndef KTJSONMAKER_HH_
#define KTJSONMAKER_HH_

#include "filestream.h"
#include "prettywriter.h"
#include "writer.h"

namespace Katydid
{
    
    template<typename Stream, typename Encoding = rapidjson::UTF8<>, typename Allocator = rapidjson::MemoryPoolAllocator<> >
    class KTJSONMaker
    {
        public:
            typedef typename Encoding::Ch Ch;

        public:
            KTJSONMaker() {}
            virtual ~KTJSONMaker() {}

        public:
            virtual KTJSONMaker& Null() = 0;
            virtual KTJSONMaker& Bool(bool b) = 0;
            virtual KTJSONMaker& Int(int i) = 0;
            virtual KTJSONMaker& Uint(unsigned u) = 0;
            virtual KTJSONMaker& Int64(int64_t i64) = 0;
            virtual KTJSONMaker& Uint64(uint64_t u64) = 0;
            virtual KTJSONMaker& Double(double d) = 0;

            virtual KTJSONMaker& String(const Ch* str, rapidjson::SizeType length, bool copy = false) = 0;

            virtual KTJSONMaker& StartObject() = 0;

            virtual KTJSONMaker& EndObject(rapidjson::SizeType memberCount = 0) = 0;

            virtual KTJSONMaker& StartArray() = 0;

            virtual KTJSONMaker& EndArray(rapidjson::SizeType elementCount = 0) = 0;

            virtual KTJSONMaker& String(const Ch* str) = 0;

    };

    template<typename Stream, typename Encoding = rapidjson::UTF8<>, typename Allocator = rapidjson::MemoryPoolAllocator<> >
    class KTJSONMakerCompact : public KTJSONMaker< Stream, Encoding, Allocator >
    {
        public:
            typedef typename KTJSONMaker< Stream, Encoding, Allocator >::Ch Ch;

        public:
            KTJSONMakerCompact(Stream& stream, Allocator* allocator = 0, size_t levelDepth = 32) :
                KTJSONMaker< Stream, Encoding, Allocator >(),
                fImpl(stream, allocator, levelDepth)
                {}
            virtual ~KTJSONMakerCompact() {}

        public:
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Null() { fImpl.Null(); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Bool(bool b) { fImpl.Bool(b); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Int(int i) { fImpl.Int(i); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Uint(unsigned u) { fImpl.Uint(u); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Int64(int64_t i64) { fImpl.Int64(i64); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Uint64(uint64_t u64) { fImpl.Uint64(u64); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Double(double d) { fImpl.Double(d); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& String(const Ch* str, rapidjson::SizeType length, bool copy = false) { fImpl.String(str, length, copy); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& StartObject() { fImpl.StartObject(); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& EndObject(rapidjson::SizeType memberCount = 0) { fImpl.EndObject(memberCount); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& StartArray() { fImpl.StartArray(); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& EndArray(rapidjson::SizeType elementCount = 0) { fImpl.EndArray(elementCount); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& String(const Ch* str) { fImpl.String(str); return *this; }

        private:
            rapidjson::Writer< Stream, Encoding, Allocator > fImpl;

    };

    template<typename Stream, typename Encoding = rapidjson::UTF8<>, typename Allocator = rapidjson::MemoryPoolAllocator<> >
    class KTJSONMakerPretty : public KTJSONMaker< Stream, Encoding, Allocator >
    {
        public:
            typedef typename KTJSONMaker< Stream, Encoding, Allocator >::Ch Ch;

        public:
            KTJSONMakerPretty(Stream& stream, Allocator* allocator = 0, size_t levelDepth = 32) :
                KTJSONMaker< Stream, Encoding, Allocator >(),
                fImpl(stream, allocator, levelDepth)
                {}
            virtual ~KTJSONMakerPretty() {}

        public:
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Null() { fImpl.Null(); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Bool(bool b) { fImpl.Bool(b); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Int(int i) { fImpl.Int(i); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Uint(unsigned u) { fImpl.Uint(u); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Int64(int64_t i64) { fImpl.Int64(i64); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Uint64(uint64_t u64) { fImpl.Uint64(u64); return *this; }
            virtual KTJSONMaker< Stream, Encoding, Allocator >& Double(double d) { fImpl.Double(d); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& String(const Ch* str, rapidjson::SizeType length, bool copy = false) { fImpl.String(str, length, copy); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& StartObject() { fImpl.StartObject(); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& EndObject(rapidjson::SizeType memberCount = 0) { fImpl.EndObject(memberCount); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& StartArray() { fImpl.StartArray(); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& EndArray(rapidjson::SizeType elementCount = 0) { fImpl.EndArray(elementCount); return *this; }

            virtual KTJSONMaker< Stream, Encoding, Allocator >& String(const Ch* str) { fImpl.String(str); return *this; }

        private:
            rapidjson::PrettyWriter< Stream, Encoding, Allocator > fImpl;

    };

} /* namespace Katydid */
#endif /* KTJSONMAKER_HH_ */
