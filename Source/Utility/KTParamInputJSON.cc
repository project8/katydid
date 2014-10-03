/*
 * KTParamInputJSON.cc
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#include "KTParamInputJSON.hh"

#include "KTLogger.hh"
#include "KTParam.hh"

#include "filestream.h"

#include <cstdio>
using std::string;



namespace Katydid
{
    KTLOGGER( plog, "KTParamInputJSON" );

    KTParamInputJSON::KTParamInputJSON()
    {
    }
    KTParamInputJSON::~KTParamInputJSON()
    {
    }

    KTParamNode* KTParamInputJSON::ReadFile( const std::string& aFilename )
    {
        FILE* t_config_file = fopen( aFilename.c_str(), "r" );
        if( t_config_file == NULL )
        {
            KTERROR( plog, "file <" << aFilename << "> did not open" );
            return NULL;
        }
        rapidjson::FileStream t_file_stream( t_config_file );

        rapidjson::Document t_config_doc;
        if( t_config_doc.ParseStream<0>( t_file_stream ).HasParseError() )
        {
            unsigned errorPos = t_config_doc.GetErrorOffset();
            rewind( t_config_file );
            unsigned iChar, newlineCount = 1, lastNewlinePos = 0;
            int thisChar;
            for( iChar = 0; iChar != errorPos; ++iChar )
            {
                thisChar = fgetc( t_config_file );
                if( thisChar == EOF )
                {
                    break;
                }
                if( thisChar == '\n' || thisChar == '\r' )
                {
                    newlineCount++;
                    lastNewlinePos = iChar + 1;
                }
            }
            if( iChar == errorPos )
            {
                KTERROR( plog, "error parsing config file :\n" <<
                        '\t' << t_config_doc.GetParseError() << '\n' <<
                        "\tThe error was reported at line " << newlineCount << ", character " << errorPos - lastNewlinePos );
            }
            else
            {
                KTERROR( plog, "error parsing config file :\n" <<
                        '\t' << t_config_doc.GetParseError() <<
                        "\tend of file reached before error location was found" );
            }
            fclose( t_config_file );
            return NULL;
        }
        fclose( t_config_file );

        return KTParamInputJSON::ReadDocument( t_config_doc );
    }

    KTParamNode* KTParamInputJSON::ReadString( const std::string& aJSONString )
    {
        rapidjson::Document t_config_doc;
        if( t_config_doc.Parse<0>( aJSONString.c_str() ).HasParseError() )
        {
            KTERROR( plog, "error parsing string:\n" << t_config_doc.GetParseError() );
            return NULL;
        }
        return KTParamInputJSON::ReadDocument( t_config_doc );
    }

    KTParamNode* KTParamInputJSON::ReadDocument( const rapidjson::Document& aDoc )
    {
        KTParamNode* t_config = new KTParamNode();
        for( rapidjson::Value::ConstMemberIterator jsonIt = aDoc.MemberBegin();
                jsonIt != aDoc.MemberEnd();
                ++jsonIt)
        {
            t_config->Replace( jsonIt->name.GetString(), KTParamInputJSON::ReadValue( jsonIt->value ) );
        }
        return t_config;
    }

    KTParam* KTParamInputJSON::ReadValue( const rapidjson::Value& aValue )
    {
        if( aValue.IsNull() )
        {
            return new KTParam();
        }
        if( aValue.IsObject() )
        {
            KTParamNode* t_config_object = new KTParamNode();
            for( rapidjson::Value::ConstMemberIterator jsonIt = aValue.MemberBegin();
                    jsonIt != aValue.MemberEnd();
                    ++jsonIt)
            {
                t_config_object->Replace( jsonIt->name.GetString(), KTParamInputJSON::ReadValue( jsonIt->value ) );
            }
            return t_config_object;
        }
        if( aValue.IsArray() )
        {
            KTParamArray* t_config_array = new KTParamArray();
            for( rapidjson::Value::ConstValueIterator jsonIt = aValue.Begin();
                    jsonIt != aValue.End();
                    ++jsonIt)
            {
                t_config_array->PushBack( KTParamInputJSON::ReadValue( *jsonIt ) );
            }
            return t_config_array;
        }
        if( aValue.IsString() )
        {
            KTParamValue* t_config_value = new KTParamValue();
            (*t_config_value) << aValue.GetString();
            return t_config_value;
        }
        if( aValue.IsBool() )
        {
            KTParamValue* t_config_value = new KTParamValue();
            (*t_config_value) << aValue.GetBool();
            return t_config_value;
        }
        if( aValue.IsInt() )
        {
            KTParamValue* t_config_value = new KTParamValue();
            (*t_config_value) << aValue.GetInt();
            return t_config_value;
        }
        if( aValue.IsUint() )
        {
            KTParamValue* t_config_value = new KTParamValue();
            (*t_config_value) << aValue.GetUint();
            return t_config_value;
        }
        if( aValue.IsInt64() )
        {
            KTParamValue* t_config_value = new KTParamValue();
            (*t_config_value) << aValue.GetInt64();
            return t_config_value;
        }
        if( aValue.IsUint64() )
        {
            KTParamValue* t_config_value = new KTParamValue();
            (*t_config_value) << aValue.GetUint64();
            return t_config_value;
        }
        if( aValue.IsDouble() )
        {
            KTParamValue* t_config_value = new KTParamValue();
            (*t_config_value) << aValue.GetDouble();
            return t_config_value;
        }
        KTWARN( plog, "(config_reader_json) unknown type; returning null value" );
        return new KTParam();
    }

} /* namespace Katydid */
