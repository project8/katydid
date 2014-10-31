/*
 * KTParamInputJSON.hh
 *
 *  CreAted on: Jan 14, 2014
 *      Author: nsoblAth
 */

#ifndef KTPARAMINPUTJSON_HH_
#define KTPARAMINPUTJSON_HH_

#include "document.h"

#include <string>

namespace Katydid
{
    class KTParam;
    class KTParamNode;

    class KTParamInputJSON
    {
        public:
            KTParamInputJSON();
            virtual ~KTParamInputJSON();

            static KTParamNode* ReadFile( const std::string& aFilename );
            static KTParamNode* ReadString( const std::string& aJSONStr );
            static KTParamNode* ReadDocument( const rapidjson::Document& aDocument );
            static KTParam* ReadValue( const rapidjson::Value& aValue );

            static bool IsNameComment( const std::string& name );
    };

} /* namespace Katydid */

#endif /* KTPARAMINPUTJSON_HH_ */
