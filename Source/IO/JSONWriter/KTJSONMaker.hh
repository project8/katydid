/*
 * KTJSONMaker.hh
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#ifndef KTJSONMAKER_HH_
#define KTJSONMAKER_HH_

#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"

namespace Katydid
{
    
    template< typename Stream >
    class KTJSONMaker
    {
        public:
            typedef typename Stream::Ch Ch;

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

    template< typename Stream >
    class KTJSONMakerCompact : public KTJSONMaker< Stream >
    {
        public:
            typedef typename KTJSONMaker< Stream >::Ch Ch;

        public:
            KTJSONMakerCompact(Stream& stream, size_t levelDepth = 32) :
                KTJSONMaker< Stream >(),
                fImpl(stream, 0, levelDepth)
                {}
            virtual ~KTJSONMakerCompact() {}

        public:
            virtual KTJSONMaker< Stream >& Null() { fImpl.Null(); return *this; }
            virtual KTJSONMaker< Stream >& Bool(bool b) { fImpl.Bool(b); return *this; }
            virtual KTJSONMaker< Stream >& Int(int i) { fImpl.Int(i); return *this; }
            virtual KTJSONMaker< Stream >& Uint(unsigned u) { fImpl.Uint(u); return *this; }
            virtual KTJSONMaker< Stream >& Int64(int64_t i64) { fImpl.Int64(i64); return *this; }
            virtual KTJSONMaker< Stream >& Uint64(uint64_t u64) { fImpl.Uint64(u64); return *this; }
            virtual KTJSONMaker< Stream >& Double(double d) { fImpl.Double(d); return *this; }

            virtual KTJSONMaker< Stream >& String(const Ch* str, rapidjson::SizeType length, bool copy = false) { fImpl.String(str, length, copy); return *this; }

            virtual KTJSONMaker< Stream >& StartObject() { fImpl.StartObject(); return *this; }

            virtual KTJSONMaker< Stream >& EndObject(rapidjson::SizeType memberCount = 0) { fImpl.EndObject(memberCount); return *this; }

            virtual KTJSONMaker< Stream >& StartArray() { fImpl.StartArray(); return *this; }

            virtual KTJSONMaker< Stream >& EndArray(rapidjson::SizeType elementCount = 0) { fImpl.EndArray(elementCount); return *this; }

            virtual KTJSONMaker< Stream >& String(const Ch* str) { fImpl.String(str); return *this; }

        private:
            rapidjson::Writer< Stream > fImpl;

    };

    template< typename Stream >
    class KTJSONMakerPretty : public KTJSONMaker< Stream >
    {
        public:
            typedef typename KTJSONMaker< Stream >::Ch Ch;

        public:
            KTJSONMakerPretty(Stream& stream, size_t levelDepth = 32) :
                KTJSONMaker< Stream >(),
                fImpl(stream, 0, levelDepth)
                {}
            virtual ~KTJSONMakerPretty() {}

        public:
            virtual KTJSONMaker< Stream >& Null() { fImpl.Null(); return *this; }
            virtual KTJSONMaker< Stream >& Bool(bool b) { fImpl.Bool(b); return *this; }
            virtual KTJSONMaker< Stream >& Int(int i) { fImpl.Int(i); return *this; }
            virtual KTJSONMaker< Stream >& Uint(unsigned u) { fImpl.Uint(u); return *this; }
            virtual KTJSONMaker< Stream >& Int64(int64_t i64) { fImpl.Int64(i64); return *this; }
            virtual KTJSONMaker< Stream >& Uint64(uint64_t u64) { fImpl.Uint64(u64); return *this; }
            virtual KTJSONMaker< Stream >& Double(double d) { fImpl.Double(d); return *this; }

            virtual KTJSONMaker< Stream >& String(const Ch* str, rapidjson::SizeType length, bool copy = false) { fImpl.String(str, length, copy); return *this; }

            virtual KTJSONMaker< Stream >& StartObject() { fImpl.StartObject(); return *this; }

            virtual KTJSONMaker< Stream >& EndObject(rapidjson::SizeType memberCount = 0) { fImpl.EndObject(memberCount); return *this; }

            virtual KTJSONMaker< Stream >& StartArray() { fImpl.StartArray(); return *this; }

            virtual KTJSONMaker< Stream >& EndArray(rapidjson::SizeType elementCount = 0) { fImpl.EndArray(elementCount); return *this; }

            virtual KTJSONMaker< Stream >& String(const Ch* str) { fImpl.String(str); return *this; }

        private:
            rapidjson::PrettyWriter< Stream > fImpl;

    };

} /* namespace Katydid */
#endif /* KTJSONMAKER_HH_ */
