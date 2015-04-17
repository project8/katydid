/*
 * KTParam.cc
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#include "KTParam.hh"

#include "KTLogger.hh"

#include <sstream>
using std::string;
using std::stringstream;


// for the config maker
#include "filestream.h"

#include <cstdio>
using std::string;



namespace Katydid
{
    KTLOGGER( plog, "KTParam" );

    unsigned KTParam::sIndentLevel = 0;

    KTParam::KTParam()
    {
    }

    KTParam::KTParam( const KTParam& )
    {
    }

    KTParam::~KTParam()
    {
    }

    KTParam* KTParam::Clone() const
    {
        return new KTParam( *this );
    }

    bool KTParam::IsNull() const
    {
        return true;
    }

    bool KTParam::IsValue() const
    {
        return false;
    }

    bool KTParam::IsArray() const
    {
        return false;
    }

    bool KTParam::IsNode() const
    {
        return false;
    }

    KTParamValue& KTParam::AsValue()
    {
        KTParamValue* t_cast_ptr = static_cast< KTParamValue* >( this );
        return *t_cast_ptr;
    }

    KTParamArray& KTParam::AsArray()
    {
        KTParamArray* t_cast_ptr = static_cast< KTParamArray* >( this );
        return *t_cast_ptr;
    }

    KTParamNode& KTParam::AsNode()
    {
        KTParamNode* t_cast_ptr = static_cast< KTParamNode* >( this );
        return *t_cast_ptr;
    }

    const KTParamValue& KTParam::AsValue() const
    {
        const KTParamValue* t_cast_ptr = static_cast< const KTParamValue* >( this );
        return *t_cast_ptr;
    }

    const KTParamArray& KTParam::AsArray() const
    {
        const KTParamArray* t_cast_ptr = static_cast< const KTParamArray* >( this );
        return *t_cast_ptr;
    }

    const KTParamNode& KTParam::AsNode() const
    {
        const KTParamNode* t_cast_ptr = static_cast< const KTParamNode* >( this );
        return *t_cast_ptr;
    }

    const KTParamValue& KTParam::operator()() const
    {
        return AsValue();
    }

    KTParamValue& KTParam::operator()()
    {
        return AsValue();
    }

    const KTParam& KTParam::operator[]( unsigned aIndex ) const
    {
        return AsArray()[aIndex];
    }

    KTParam& KTParam::operator[]( unsigned aIndex )
    {
        return AsArray()[aIndex];
    }

    const KTParam& KTParam::operator[]( const std::string& aName ) const
    {
        return AsNode()[aName];
    }

    KTParam& KTParam::operator[]( const std::string& aName )
    {
        return AsNode()[aName];
    }

    std::string KTParam::ToString() const
    {
        return string();
    }

    //************************************
    //***********  DATA  *****************
    //************************************

    KTParamValue::KTParamValue() :
            KTParam(),
            fValue()
    {
    }

    KTParamValue::KTParamValue( const KTParamValue& orig ) :
            KTParam( orig ),
            fValue( orig.fValue )
    {
    }

    KTParamValue::~KTParamValue()
    {
    }

    KTParam* KTParamValue::Clone() const
    {
        return new KTParamValue( *this );
    }

    bool KTParamValue::IsNull() const
    {
        return false;
    }

    bool KTParamValue::Empty() const
    {
        return fValue.empty();
    }

    bool KTParamValue::IsValue() const
    {
        return true;
    }

    const string& KTParamValue::Get() const
    {
         return fValue;
    }

    std::string KTParamValue::ToString() const
    {
        return string( fValue );
    }

    //************************************
    //***********  ARRAY  ****************
    //************************************

    KTParamArray::KTParamArray() :
            KTParam(),
            fContents()
    {
    }

    KTParamArray::KTParamArray( const KTParamArray& orig ) :
            KTParam( orig ),
            fContents( orig.fContents.size() )
    {
        for( unsigned ind = 0; ind < fContents.size(); ++ind )
        {
            this->Assign( ind, orig[ ind ].Clone() );
        }
    }

    KTParamArray::~KTParamArray()
    {
        for( unsigned ind = 0; ind < fContents.size(); ++ind )
        {
            delete fContents[ ind ];
        }
    }

    KTParam* KTParamArray::Clone() const
    {
        return new KTParamArray( *this );
    }

    bool KTParamArray::IsNull() const
    {
        return false;
    }

    bool KTParamArray::IsArray() const
    {
        return true;
    }

    unsigned KTParamArray::Size() const
    {
        return fContents.size();
    }
    bool KTParamArray::Empty() const
    {
        return fContents.empty();
    }

    void KTParamArray::Resize( unsigned a_size )
    {
        unsigned curr_size = fContents.size();
        for( unsigned ind = a_size; ind < curr_size; ++ind )
        {
            delete fContents[ ind ];
        }
        fContents.resize( a_size );
        return;
    }

    std::string KTParamArray::GetValue( unsigned aIndex ) const
    {
        const KTParamValue* value = ValueAt( aIndex );
        if( value == NULL ) throw KTException() << "No value at <" << aIndex << "> is present at this node";
        return value->Get();
    }

    std::string KTParamArray::GetValue( unsigned aIndex, const std::string& aDefault ) const
    {
        const KTParamValue* value = ValueAt( aIndex );
        if( value == NULL ) return aDefault;
        return value->Get();
    }

    std::string KTParamArray::GetValue( unsigned aIndex, const char* aDefault ) const
    {
        return GetValue( aIndex, string( aDefault ) );
    }

    const KTParam* KTParamArray::At( unsigned aIndex ) const
    {
        if( aIndex >= fContents.size() ) return NULL;
        return fContents[ aIndex ];
    }
    KTParam* KTParamArray::At( unsigned aIndex )
    {
        if( aIndex >= fContents.size() ) return NULL;
        return fContents[ aIndex ];
    }

    const KTParamValue* KTParamArray::ValueAt( unsigned aIndex ) const
    {
        if( aIndex >= fContents.size() ) return NULL;
        return dynamic_cast< KTParamValue* >( fContents[ aIndex ] );
    }
    KTParamValue* KTParamArray::ValueAt( unsigned aIndex )
    {
        if( aIndex >= fContents.size() ) return NULL;
        return dynamic_cast< KTParamValue* >( fContents[ aIndex ] );
    }

    const KTParamValue* KTParamArray::ValueAtFast( unsigned aIndex ) const
    {
        if( aIndex >= fContents.size() ) return NULL;
        return static_cast< KTParamValue* >( fContents[ aIndex ] );
    }
    KTParamValue* KTParamArray::ValueAtFast( unsigned aIndex )
    {
        if( aIndex >= fContents.size() ) return NULL;
        return static_cast< KTParamValue* >( fContents[ aIndex ] );
    }

    const KTParamArray* KTParamArray::ArrayAt( unsigned aIndex ) const
    {
        if( aIndex >= fContents.size() ) return NULL;
        return dynamic_cast< KTParamArray* >( fContents[ aIndex ] );
    }
    KTParamArray* KTParamArray::ArrayAt( unsigned aIndex )
    {
        if( aIndex >= fContents.size() ) return NULL;
        return dynamic_cast< KTParamArray* >( fContents[ aIndex ] );
    }

    const KTParamArray* KTParamArray::ArrayAtFast( unsigned aIndex ) const
    {
        if( aIndex >= fContents.size() ) return NULL;
        return static_cast< KTParamArray* >( fContents[ aIndex ] );
    }
    KTParamArray* KTParamArray::ArrayAtFast( unsigned aIndex )
    {
        if( aIndex >= fContents.size() ) return NULL;
        return static_cast< KTParamArray* >( fContents[ aIndex ] );
    }

    const KTParamNode* KTParamArray::NodeAt( unsigned aIndex ) const
    {
        if( aIndex >= fContents.size() ) return NULL;
        return dynamic_cast< KTParamNode* >( fContents[ aIndex ] );
    }
    KTParamNode* KTParamArray::NodeAt( unsigned aIndex )
    {
        if( aIndex >= fContents.size() ) return NULL;
        return dynamic_cast< KTParamNode* >( fContents[ aIndex ] );
    }

    const KTParamNode* KTParamArray::NodeAtFast( unsigned aIndex ) const
    {
        if( aIndex >= fContents.size() ) return NULL;
        return static_cast< KTParamNode* >( fContents[ aIndex ] );
    }
    KTParamNode* KTParamArray::NodeAtFast( unsigned aIndex )
    {
        if( aIndex >= fContents.size() ) return NULL;
        return static_cast< KTParamNode* >( fContents[ aIndex ] );
    }

    const KTParam& KTParamArray::operator[]( unsigned aIndex ) const
    {
        return *fContents[ aIndex ];
    }
    KTParam& KTParamArray::operator[]( unsigned aIndex )
    {
        return *fContents[ aIndex ];
    }

    const KTParam* KTParamArray::Front() const
    {
        return fContents.front();
    }
    KTParam* KTParamArray::Front()
    {
        return fContents.front();
    }

    const KTParam* KTParamArray::Back() const
    {
        return fContents.back();
    }
    KTParam* KTParamArray::Back()
    {
        return fContents.back();
    }

    // assign a copy of aValue to the array at aIndex
    void KTParamArray::Assign( unsigned aIndex, const KTParam& aValue )
    {
        Erase( aIndex );
        fContents[ aIndex ] = aValue.Clone();
        return;
    }
    // directly assign aValue_ptr to the array at aIndex
    void KTParamArray::Assign( unsigned aIndex, KTParam* aValue_ptr )
    {
        Erase( aIndex );
        fContents[ aIndex ] = aValue_ptr;
        return;
    }

    void KTParamArray::PushBack( const KTParam& aValue )
    {
        fContents.push_back( aValue.Clone() );
        return;
    }
    void KTParamArray::PushBack( KTParam* aValue_ptr )
    {
        fContents.push_back( aValue_ptr );
        return;
    }

    void KTParamArray::PushFront( const KTParam& aValue )
    {
        fContents.push_front( aValue.Clone() );
        return;
    }
    void KTParamArray::PushFront( KTParam* aValue_ptr )
    {
        fContents.push_front( aValue_ptr );
        return;
    }

    void KTParamArray::Append( const KTParamArray& anArray )
    {
        for( KTParamArray::const_iterator it = anArray.Begin(); it != anArray.End(); ++it )
        {
            PushBack( *(*it) );
        }
        return;
    }

    void KTParamArray::Erase( unsigned aIndex )
    {
        delete fContents[ aIndex ];
        return;
    }
    KTParam* KTParamArray::Remove( unsigned aIndex )
    {
        KTParam* tCurrent = fContents[ aIndex ];
        fContents[ aIndex ] = NULL;
        return tCurrent;
    }
    void KTParamArray::Clear()
    {
        for( unsigned ind = 0; ind < fContents.size(); ++ind )
        {
            delete fContents[ ind ];
        }
        fContents.clear();
        return;
    }

    KTParamArray::iterator KTParamArray::Begin()
    {
        return fContents.begin();
    }
    KTParamArray::const_iterator KTParamArray::Begin() const
    {
        return fContents.begin();
    }

    KTParamArray::iterator KTParamArray::End()
    {
        return fContents.end();
    }
    KTParamArray::const_iterator KTParamArray::End() const
    {
        return fContents.end();
    }

    KTParamArray::reverse_iterator KTParamArray::RBegin()
    {
        return fContents.rbegin();
    }
    KTParamArray::const_reverse_iterator KTParamArray::RBegin() const
    {
        return fContents.rbegin();
    }

    KTParamArray::reverse_iterator KTParamArray::REnd()
    {
        return fContents.rend();
    }
    KTParamArray::const_reverse_iterator KTParamArray::REnd() const
    {
        return fContents.rend();
    }

    std::string KTParamArray::ToString() const
    {
        stringstream out;
        string indentation;
        for ( unsigned i=0; i<KTParam::sIndentLevel; ++i )
            indentation += "    ";
        out << '\n' << indentation << "[\n";
        KTParam::sIndentLevel++;
        for( const_iterator it = Begin(); it != End(); ++it )
        {
            out << indentation << "    " << **it << '\n';
        }
        KTParam::sIndentLevel--;
        out << indentation << "]";
        return out.str();
    }


    //************************************
    //***********  NODE  *****************
    //************************************

    KTParamNode::KTParamNode() :
            KTParam(),
            fContents()
    {
    }

    KTParamNode::KTParamNode( const KTParamNode& orig ) :
            KTParam( orig ),
            fContents()
    {
        for( const_iterator it = orig.fContents.begin(); it != orig.fContents.end(); ++it )
        {
            this->Replace( it->first, *it->second );
        }
    }

    KTParamNode::~KTParamNode()
    {
        for( iterator it = fContents.begin(); it != fContents.end(); ++it )
        {
            delete it->second;
        }
    }

    KTParam* KTParamNode::Clone() const
    {
        return new KTParamNode( *this );
    }

    bool KTParamNode::IsNull() const
    {
        return false;
    }

    bool KTParamNode::IsNode() const
    {
        return true;
    }

    bool KTParamNode::Has( const std::string& aName ) const
    {
        return fContents.count( aName ) > 0;
    }

    unsigned KTParamNode::Count( const std::string& aName ) const
    {
        return fContents.count( aName );
    }

    unsigned KTParamNode::Size() const
    {
        return fContents.size();
    }
    bool KTParamNode::Empty() const
    {
        return fContents.empty();
    }

    std::string KTParamNode::GetValue( const std::string& aName ) const
    {
        const KTParamValue* value = ValueAt( aName );
        if( value == NULL ) throw KTException() << "No value with name <" << aName << "> is present at this node";
        return value->Get();
    }

    std::string KTParamNode::GetValue( const std::string& aName, const std::string& aDefault ) const
    {
        const KTParamValue* value = ValueAt( aName );
        if( value == NULL ) return aDefault;
        return value->Get();
    }

    std::string KTParamNode::GetValue( const std::string& aName, const char* aDefault ) const
    {
        return GetValue( aName, string( aDefault ) );
    }

    const KTParam* KTParamNode::At( const std::string& aName ) const
    {
        const_iterator it = fContents.find( aName );
        if( it == fContents.end() )
        {
            return NULL;
        }
        return it->second;
    }

    KTParam* KTParamNode::At( const std::string& aName )
    {
        iterator it = fContents.find( aName );
        if( it == fContents.end() )
        {
            return NULL;
        }
        return it->second;
    }

    const KTParamValue* KTParamNode::ValueAt( const std::string& aName ) const
    {
        const_iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return dynamic_cast< KTParamValue* >( it->second );
    }

    KTParamValue* KTParamNode::ValueAt( const std::string& aName )
    {
        iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return dynamic_cast< KTParamValue* >( it->second );
    }

    const KTParamValue* KTParamNode::ValueAtFast( const std::string& aName ) const
    {
        const_iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return static_cast< KTParamValue* >( it->second );
    }

    KTParamValue* KTParamNode::ValueAtFast( const std::string& aName )
    {
        iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return static_cast< KTParamValue* >( it->second );
    }

    const KTParamArray* KTParamNode::ArrayAt( const std::string& aName ) const
    {
        const_iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return dynamic_cast< KTParamArray* >( it->second );
    }

    KTParamArray* KTParamNode::ArrayAt( const std::string& aName )
    {
        iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return dynamic_cast< KTParamArray* >( it->second );
    }

    const KTParamArray* KTParamNode::ArrayAtFast( const std::string& aName ) const
    {
        const_iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return static_cast< KTParamArray* >( it->second );
    }

    KTParamArray* KTParamNode::ArrayAtFast( const std::string& aName )
    {
        iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return static_cast< KTParamArray* >( it->second );
    }

    const KTParamNode* KTParamNode::NodeAt( const std::string& aName ) const
    {
        const_iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return dynamic_cast< KTParamNode* >( it->second );
    }

    KTParamNode* KTParamNode::NodeAt( const std::string& aName )
    {
        iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return dynamic_cast< KTParamNode* >( it->second );
    }

    const KTParamNode* KTParamNode::NodeAtFast( const std::string& aName ) const
    {
        const_iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return static_cast< KTParamNode* >( it->second );
    }

    KTParamNode* KTParamNode::NodeAtFast( const std::string& aName )
    {
        iterator it = fContents.find( aName );
        if( it == fContents.end() ) return NULL;
        return static_cast< KTParamNode* >( it->second );
    }

    const KTParam& KTParamNode::operator[]( const std::string& aName ) const
    {
        const_iterator it = fContents.find( aName );
        if( it == fContents.end() )
        {
            throw KTException() << "No value present corresponding to name <" << aName << ">\n";
        }
        return *(it->second);
    }

    KTParam& KTParamNode::operator[]( const std::string& aName )
    {
        return *fContents[ aName ];
    }

    bool KTParamNode::Add( const std::string& aName, const KTParam& aValue )
    {
        iterator it = fContents.find( aName );
        if( it == fContents.end() )
        {
            fContents.insert( contents_type( aName, aValue.Clone() ) );
            return true;
        }
        return false;
    }

    bool KTParamNode::Add( const std::string& aName, KTParam* aValue )
    {
        iterator it = fContents.find( aName );
        if( it == fContents.end() )
        {
            fContents.insert( contents_type( aName, aValue ) );
            return true;
        }
        return false;
    }

    void KTParamNode::Replace( const std::string& aName, const KTParam& aValue )
    {
        Erase( aName );
        fContents[ aName ] = aValue.Clone();
        return;
    }

    void KTParamNode::Replace( const std::string& aName, KTParam* aValue )
    {
        Erase( aName );
        fContents[ aName ] = aValue;
        return;
    }

    void KTParamNode::Merge( const KTParamNode& aObject )
    {
        for( const_iterator it = aObject.fContents.begin(); it != aObject.fContents.end(); ++it )
        {
            if( this->Has( it->first ) )
            {
                KTParam* toBeMergedInto = this->At( it->first);
                if( toBeMergedInto->IsNode() && it->second->IsNode() )
                {
                    toBeMergedInto->AsNode().Merge( it->second->AsNode() );
                }
                else
                {
                    this->Replace( it->first, *it->second );
                }
            }
            else
            {
                this->Add( it->first, *it->second );
            }
        }
        return;
    }

    void KTParamNode::Erase( const std::string& aName )
    {
        iterator it = fContents.find( aName );
        if( it != fContents.end() )
        {
            delete it->second;
            fContents.erase( it );
        }
        return;
    }

    KTParam* KTParamNode::Remove( const std::string& aName )
    {
        iterator it = fContents.find( aName );
        if( it != fContents.end() )
        {
            KTParam* removed = it->second;
            fContents.erase( it );
            return removed;
        }
        return NULL;
    }

    void KTParamNode::Clear()
    {
        for( iterator it = fContents.begin(); it != fContents.end(); ++it )
        {
            delete it->second;
        }
        fContents.clear();
        return;
    }

    KTParamNode::iterator KTParamNode::Begin()
    {
        return fContents.begin();
    }

    KTParamNode::const_iterator KTParamNode::Begin() const
    {
        return fContents.begin();
    }

    KTParamNode::iterator KTParamNode::End()
    {
        return fContents.end();
    }

    KTParamNode::const_iterator KTParamNode::End() const
    {
        return fContents.end();
    }

    std::string KTParamNode::ToString() const
    {
        stringstream out;
        string indentation;
        for ( unsigned i=0; i<KTParam::sIndentLevel; ++i )
            indentation += "    ";
        out << '\n' << indentation << "{\n";
        KTParam::sIndentLevel++;
        for( const_iterator it = Begin(); it != End(); ++it )
        {
            out << indentation << "    " << it->first << " : " << *(it->second) << '\n';
        }
        KTParam::sIndentLevel--;
        out << indentation << "}";
        return out.str();
    }




    std::ostream& operator<<(std::ostream& out, const KTParam& aValue)
    {
        return out << aValue.ToString();
    }


    std::ostream& operator<<(std::ostream& out, const KTParamValue& aValue)
    {
        return out << aValue.ToString();
    }


    std::ostream& operator<<(std::ostream& out, const KTParamArray& aValue)
    {
        return out << aValue.ToString();
    }


    std::ostream& operator<<(std::ostream& out, const KTParamNode& aValue)
    {
        return out << aValue.ToString();
    }

} /* namespace Katydid */
