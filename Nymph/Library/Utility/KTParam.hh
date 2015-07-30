/*
 * KTParam.hh
 *
 *  CreAted on: Jan 14, 2014
 *      Author: nsoblAth
 */

#ifndef KTPARAM_HH_
#define KTPARAM_HH_

#include "KTException.hh"

#include <deque>
#include <map>
#include <sstream>
#include <string>

namespace Nymph
{
    class KTParamValue;
    class KTParamArray;
    class KTParamNode;

    class KTParam
    {
        public:
            KTParam();
            KTParam(const KTParam& orig);
            virtual ~KTParam();

            virtual KTParam* Clone() const;

            virtual bool IsNull() const;
            virtual bool IsValue() const;
            virtual bool IsArray() const;
            virtual bool IsNode() const;

            KTParamValue& AsValue();
            KTParamArray& AsArray();
            KTParamNode& AsNode();

            const KTParamValue& AsValue() const;
            const KTParamArray& AsArray() const;
            const KTParamNode& AsNode() const;

            /// Assumes that the parameter is a value, and returns a reference to itself.
            const KTParamValue& operator()() const;
            /// Assumes that the parameter is a value, and returns a reference to itself.
            KTParamValue& operator()();

            /// Assumes that the parameter is an array, and returns a reference to the KTParam at aIndex.
            /// Behavior is undefined if aIndex is out-of-range.
            const KTParam& operator[]( unsigned aIndex ) const;
            /// Assumes that the parameter is an array, and returns a reference to the KTParam at aIndex.
            /// Behavior is undefined if aIndex is out-of-range.
            KTParam& operator[]( unsigned aIndex );

            /// Assumes that the parameter is a node, and returns a reference to the KTParam corresponding to aName.
            /// Throws an KTException if aName is not present.
            const KTParam& operator[]( const std::string& aName ) const;
            /// Assumes that the parameter is a node, and returns a reference to the KTParam corresponding to aName.
            /// Adds a new Value if aName is not present.
            KTParam& operator[]( const std::string& aName );

            virtual std::string ToString() const;

            static unsigned sIndentLevel;
    };

    class KTParamValue : public KTParam
    {
        public:
            KTParamValue();
            template< typename XStreamableType >
            KTParamValue( XStreamableType aStreamable );
            KTParamValue(const KTParamValue& orig);
            virtual ~KTParamValue();

            virtual KTParam* Clone() const;

            bool Empty() const;

            virtual bool IsNull() const;
            virtual bool IsValue() const;

            const std::string& Get() const;
            template< typename XValType >
            XValType Get() const;

            template< typename XStreamableType >
            KTParamValue& operator<<( const XStreamableType& aStreamable );

            virtual std::string ToString() const;

        protected:
            std::string fValue;

    };

    template< typename XStreamableType >
    KTParamValue::KTParamValue( XStreamableType aStreamable ) :
            KTParam(),
            fValue()
    {
        (*this) << aStreamable;
    }

    template< typename XValType >
    XValType KTParamValue::Get() const
    {
        XValType tReturn;
        std::stringstream tBuffer;
        tBuffer << fValue;
        tBuffer >> tReturn;
        return tReturn;
    }

    template< typename XStreamableType >
    KTParamValue& KTParamValue::operator<<( const XStreamableType& a_streamable )
    {
        std::stringstream tBuffer;
        tBuffer << a_streamable;
        fValue = tBuffer.str();
        return *this;
    }


    class KTParamArray : public KTParam
    {
        public:
            typedef std::deque< KTParam* > contents;
            typedef contents::iterator iterator;
            typedef contents::const_iterator const_iterator;
            typedef contents::reverse_iterator reverse_iterator;
            typedef contents::const_reverse_iterator const_reverse_iterator;
            typedef contents::value_type contents_type;

        public:
            KTParamArray();
            KTParamArray( const KTParamArray& orig );
            virtual ~KTParamArray();

            virtual KTParam* Clone() const;

            virtual bool IsNull() const;
            virtual bool IsArray() const;

            unsigned Size() const;
            bool Empty() const;

            /// sets the size of the array
            /// if smaller than the current size, extra elements are deleted
            void Resize( unsigned a_size );

            /// Returns the result of ParamValue::Get if aName is present and is of type ParamValue
            /// Throws a KTException if aName is not present or is not of type ParamValue
            std::string GetValue( unsigned aIndex ) const;
            /// Returns the result of ParamValue::Get if aName is present and is of type ParamValue
            /// Throws a KTException if aName is not present or is not of type ParamValue
            template< typename XValType >
            XValType GetValue( unsigned aIndex ) const;

            /// Returns the result of ParamValue::Get if aName is present and is of type ParamValue
            /// Returns aDefault if aName is not present or is not of type ParamValue
            std::string GetValue( unsigned aIndex, const std::string& aDefault ) const;
            std::string GetValue( unsigned aIndex, const char* aDefault ) const;
            /// Returns the result of ParamValue::Get if aName is present and is of type ParamValue
            /// Returns aDefault if aName is not present or is not of type ParamValue
            template< typename XValType >
            XValType GetValue( unsigned aIndex, XValType aDefault ) const;

            /// Returns a pointer to the KTParam corresponding to aName.
            /// Returns NULL if aName is not present.
            const KTParam* At( unsigned aIndex ) const;
            /// Returns a pointer to the KTParam corresponding to aName.
            /// Returns NULL if aName is not present.
            KTParam* At( unsigned aIndex );

            /// Returns a pointer to the KTParamValue (dynamic-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present or type is wrong.
            const KTParamValue* ValueAt( unsigned aIndex ) const;
            /// Returns a pointer to the KTParamValue (dynamic-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present or type is wrong.
            KTParamValue* ValueAt( unsigned aIndex );
            /// Returns a pointer to the KTParamValue (static-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present.
            const KTParamValue* ValueAtFast( unsigned aIndex ) const;
            /// Returns a pointer to the KTParamValue (static-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present.
            KTParamValue* ValueAtFast( unsigned aIndex );


            /// Returns a pointer to the KTParamArray (dynamic-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present or type is wrong.
            const KTParamArray* ArrayAt( unsigned aIndex ) const;
            /// Returns a pointer to the KTParamArray (dynamic-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present or type is wrong.
            KTParamArray* ArrayAt( unsigned aIndex );
            /// Returns a pointer to the KTParamArray (static-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present.
            const KTParamArray* ArrayAtFast( unsigned aIndex ) const;
            /// Returns a pointer to the KTParamArray (static-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present.
            KTParamArray* ArrayAtFast( unsigned aIndex );

            /// Returns a pointer to the KTParamNode (dynamic-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present or type is wrong.
            const KTParamNode* NodeAt( unsigned aIndex ) const;
            /// Returns a pointer to the KTParamNode (dynamic-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present or type is wrong.
            KTParamNode* NodeAt( unsigned aIndex );
            /// Returns a pointer to the KTParamNode (static-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present.
            const KTParamNode* NodeAtFast( unsigned aIndex ) const;
            /// Returns a pointer to the KTParamNode (static-ly cast) corresponding to aName.
            /// Returns NULL if aName is not present.
            KTParamNode* NodeAtFast( unsigned aIndex );

            /// Returns a reference to the KTParam At aIndex.
            /// Behavior is undefined if aIndex is out-of-range.
            const KTParam& operator[]( unsigned aIndex ) const;
            /// Returns a reference to the KTParam At aIndex.
            /// Behavior is undefined if aIndex is out-of-range.
            KTParam& operator[]( unsigned aIndex );

            const KTParam* Front() const;
            KTParam* Front();

            const KTParam* Back() const;
            KTParam* Back();

            // assign a copy of aValue to the array at aIndex
            void Assign( unsigned aIndex, const KTParam& aValue );
            // directly assign aValue_ptr to the array at aIndex
            void Assign( unsigned aIndex, KTParam* aValue_ptr );

            void PushBack( const KTParam& aValue );
            void PushBack( KTParam* aValue_ptr );

            void PushFront( const KTParam& aValue );
            void PushFront( KTParam* aValue_ptr );

            void Append( const KTParamArray& anArray );

            void Erase( unsigned aIndex );
            KTParam* Remove( unsigned aIndex );
            void Clear();

            iterator Begin();
            const_iterator Begin() const;

            iterator End();
            const_iterator End() const;

            reverse_iterator RBegin();
            const_reverse_iterator RBegin() const;

            reverse_iterator REnd();
            const_reverse_iterator REnd() const;

            virtual std::string ToString() const;

        protected:
            contents fContents;
    };

    template< typename XValType >
    XValType KTParamArray::GetValue( unsigned aIndex ) const
    {
        const KTParamValue* value = ValueAt( aIndex );
        if( value == NULL ) throw KTException() << "No Value At <" << aIndex << "> is present At this node";
        return value->Get< XValType >();
    }

    template< typename XValType >
    XValType KTParamArray::GetValue( unsigned aIndex, XValType aDefault ) const
    {
        const KTParamValue* value = ValueAt( aIndex );
        if( value == NULL ) return aDefault;
        return value->Get< XValType >();
    }



    class KTParamNode : public KTParam
    {
        public:
            typedef std::map< std::string, KTParam* > contents;
            typedef contents::iterator iterator;
            typedef contents::const_iterator const_iterator;
            typedef contents::value_type contents_type;

            KTParamNode();
            KTParamNode( const KTParamNode& orig );
            virtual ~KTParamNode();

            virtual KTParam* Clone() const;

            virtual bool IsNull() const;
            virtual bool IsNode() const;

            bool Has( const std::string& aName ) const;
            unsigned Count( const std::string& aName ) const;

            unsigned Size() const;
            bool Empty() const;

            /// Returns the result of ParamValue::Get if aName is present and is of type ParamValue
            /// Throws a KTException if aName is not present or is not of type ParamValue
            std::string GetValue( const std::string& aName ) const;
            /// Returns the result of ParamValue::Get if aName is present and is of type ParamValue
            /// Throws a KTException if aName is not present or is not of type ParamValue
            template< typename XValType >
            XValType GetValue( const std::string& aName ) const;

            /// Returns the result of ParamValue::Get if aName is present and is of type ParamValue
            /// Returns a_default if aName is not present or is not of type ParamValue
            std::string GetValue( const std::string& aName, const std::string& aDefault ) const;
            std::string GetValue( const std::string& aName, const char* aDefault ) const;
            /// Returns the result of ParamValue::Get if aName is present and is of type ParamValue
            /// Returns a_default if aName is not present or is not of type ParamValue
            template< typename XValType >
            XValType GetValue( const std::string& aName, XValType aDefault ) const;

            /// Returns a pointer to the KTParam corresponding to aName.
            /// Returns NULL if aName is not present.
            const KTParam* At( const std::string& aName ) const;
            /// Returns a pointer to the KTParam corresponding to aName.
            /// Returns NULL if aName is not present.
            KTParam* At( const std::string& aName );

            const KTParamValue* ValueAt( const std::string& aName ) const;
            KTParamValue* ValueAt( const std::string& aName );
            const KTParamValue* ValueAtFast( const std::string& aName ) const;
            KTParamValue* ValueAtFast( const std::string& aName );

            const KTParamArray* ArrayAt( const std::string& aName ) const;
            KTParamArray* ArrayAt( const std::string& aName );
            const KTParamArray* ArrayAtFast( const std::string& aName ) const;
            KTParamArray* ArrayAtFast( const std::string& aName );

            const KTParamNode* NodeAt( const std::string& aName ) const;
            KTParamNode* NodeAt( const std::string& aName );
            const KTParamNode* NodeAtFast( const std::string& aName ) const;
            KTParamNode* NodeAtFast( const std::string& aName );

            /// Returns a reference to the KTParam corresponding to aName.
            /// Throws an KTException if aName is not present.
            const KTParam& operator[]( const std::string& aName ) const;
            /// Returns a reference to the KTParam corresponding to aName.
            /// Adds a new Value if aName is not present.
            KTParam& operator[]( const std::string& aName );

            /// creates a copy of aValue
            bool Add( const std::string& aName, const KTParam& aValue );
            /// directly adds (without copying) aValuePtr
            bool Add( const std::string& aName, KTParam* aValuePtr );

            /// creates a copy of aValue
            void Replace( const std::string& aName, const KTParam& aValue );
            /// directly adds (without copying) aValuePtr
            void Replace( const std::string& aName, KTParam* aValuePtr );

            /// Merges the contents of a_object into this object.
            /// If names in the contents of aObject exist in this object,
            /// the values in this object corresponding to the matching names will be replaced.
            void Merge( const KTParamNode& aObject );

            void Erase( const std::string& aName );
            KTParam* Remove( const std::string& aName );
            void Clear();

            iterator Begin();
            const_iterator Begin() const;

            iterator End();
            const_iterator End() const;

            virtual std::string ToString() const;

        protected:
            contents fContents;

    };

    template< typename XValType >
    XValType KTParamNode::GetValue( const std::string& aName ) const
    {
        const KTParamValue* value = ValueAt( aName );
        if( value == NULL ) throw KTException() << "No Value with name <" << aName << "> is present At this node";
        return value->Get< XValType >();
    }

    template< typename XValType >
    XValType KTParamNode::GetValue( const std::string& aName, XValType aDefault ) const
    {
        const KTParamValue* value = ValueAt( aName );
        if( value == NULL ) return aDefault;
        return value->Get< XValType >();
    }



    std::ostream& operator<<(std::ostream& out, const KTParam& aValue);
    std::ostream& operator<<(std::ostream& out, const KTParamValue& value);
    std::ostream& operator<<(std::ostream& out, const KTParamArray& value);
    std::ostream& operator<<(std::ostream& out, const KTParamNode& value);

} /* namespace Nymph */

#endif /* KTPARAM_HH_ */
