/*
 * KTCutable.hh
 *
 *  Created on: Dec 21, 2011
 *      Author: nsoblath
 */

#ifndef KTCUTABLE_HH_
#define KTCUTABLE_HH_

//#include <boost/unordered_map.hpp>
//#include <boost/functional/hash.hpp>

//#include <boost/bimap/bimap.hpp>
//#include <boost/bimap/unconstrained_set_of.hpp>
//#include <boost/bimap/unordered_set_of.hpp>
//#include <boost/bimap/vector_of.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/tag.hpp>

#include <cstddef>
#include <functional>
#include <set>
#include <utility>

using namespace boost;
using namespace boost::multi_index;

namespace Katydid
{
    template< class XObjectType, class XRangeIteratorType >
    class KTCutable
    {
            //***********************
            // Range iterator wrapper
            //***********************

        public:
            class RangeIteratorWrapper
            {
                public:
                    RangeIteratorWrapper(const XRangeIteratorType& original, int position);
                    virtual ~RangeIteratorWrapper();

                    bool operator<(RangeIteratorWrapper& rhs);
                    bool operator<(const RangeIteratorWrapper& rhs) const;

                    const XRangeIteratorType& Iterator() const;

                    int GetPosition() const;
                    void SetPosition(int position);

                private:
                    XRangeIteratorType fIterator;

                    int fPosition;

                    RangeIteratorWrapper();
            };

        private:
            struct RangeIteratorEqualTo : std::binary_function< XRangeIteratorType, XRangeIteratorType, bool >
            {
                bool operator() (const XRangeIteratorType& lhs, const XRangeIteratorType& rhs) const
                {
                    return lhs==rhs;
                }
            };

            struct RangeIteratorHash : std::unary_function< XRangeIteratorType, std::size_t >
            {
                std::size_t operator()(const XRangeIteratorType& it) const
                {
                    return boost::hash<long unsigned int>()((long unsigned int)&(*it));
                }
            };

        private:
            //typedef boost::unordered_map< XRangeIteratorType, int, RangeIteratorHash, RangeIteratorEqualTo > RangeIteratorMap;
            //typedef boost::bimaps::bimap< boost::bimaps::vector_of< unsigned int >,
            //                              boost::bimaps::unconstrained_set_of< XRangeIteratorType >,
            //                              boost::bimaps::unordered_set_of_relation< RangeIteratorHash< boost::bimaps::_relation >, RangeIteratorEqualTo > > RangeIteratorMap;
            typedef multi_index_container< RangeIteratorWrapper,
                                           indexed_by< ordered_unique< const_mem_fun< RangeIteratorWrapper, int, &RangeIteratorWrapper::GetPosition > >,
                                                       hashed_unique<  tag<XRangeIteratorType>,
                                                                       const_mem_fun< RangeIteratorWrapper, const XRangeIteratorType&, &RangeIteratorWrapper::Iterator >,
                                                                       RangeIteratorHash,
                                                                       RangeIteratorEqualTo >
                                                     >
                                         > RangeIteratorMap;
            //typedef typename RangeIteratorMap::const_iterator RangeIteratorMapCIt;
            typedef typename RangeIteratorMap::template index< XRangeIteratorType >::type RangeIteratorMapByRI;
            typedef typename RangeIteratorMapByRI::const_iterator RangeIteratorMapCIt;


            //***********************
            // value_type
            //***********************
        public:
            typedef XObjectType value_type;

            //**************************
            // value_type iterator class
            //**************************
        public:
            typedef XRangeIteratorType value_type_range_iterator;


        public:
            struct RangeIteratorWrapperComp {
                bool operator() (const RangeIteratorWrapper& lhs, const RangeIteratorWrapper& rhs) const
                {return lhs<rhs;}
            };


        private:
            typedef std::set< RangeIteratorWrapper > RangeIteratorWrapperSet;
            typedef typename RangeIteratorWrapperSet::iterator RangeIteratorWrapperSetIt;


            //***********************
            // iterator class
            //***********************

        public:
            class iterator
            {
                public:
                    iterator();
                    iterator(const RangeIteratorWrapperSetIt& it);
                    iterator(const iterator& orig); // copy constructor
                    virtual ~iterator();

                    /// Dereferences the iterator
                    XObjectType& operator*();
                    const XObjectType& operator*() const;

                    /// Set the pointer
                    iterator& operator=(const iterator& rhs);

                    /// Increment the pointer
                    iterator& operator++();
                    iterator  operator++(int);
                    iterator& operator--();
                    iterator  operator--(int);

                    /// Test the pointer
                    bool operator==(const iterator& rhs) const;
                    bool operator!=(const iterator& lhs) const;

                private:
                    RangeIteratorWrapperSetIt fValidIter;

            };


            //***********************
            // Cutable class
            //***********************

            // constructors and destructor
        public:
            KTCutable();
            KTCutable(const XRangeIteratorType& begin, const XRangeIteratorType& end);
            KTCutable(const KTCutable< XObjectType, XRangeIteratorType >& orig);
            virtual ~KTCutable();

        public:
            KTCutable< XObjectType, XRangeIteratorType >& operator=(const KTCutable& rhs);

            // add and remove cut elements
        public:
            void Cut(const XRangeIteratorType& toCut);
            void UnCut(const XRangeIteratorType& toUnCut);

            void Cut(const XRangeIteratorType& toBeginCut, const XRangeIteratorType& toEndCut);
            void UnCut(const XRangeIteratorType& toBeginUnCut, const XRangeIteratorType& toEndUnCut);

            // access to member variables
        public:
            const RangeIteratorWrapperSet& GetValid() const;

            const XRangeIteratorType& GetCutableBegin() const;
            const XRangeIteratorType& GetCutableEnd() const;

        private:
            RangeIteratorWrapperSet fValid;

            RangeIteratorMap fAll;

            XRangeIteratorType fBegin;
            XRangeIteratorType fEnd;

            //***********************
            // standard container functions
            //***********************

        public:
            iterator begin() const;
            iterator end() const;




    };

    //***********************
    // RangeIteratorWrapper members
    //***********************

    template< class XObjectType, class XRangeIteratorType >
    KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapper::RangeIteratorWrapper(const XRangeIteratorType& original, int position) :
            fIterator(original),
            fPosition(position)
    {
    }

    template< class XObjectType, class XRangeIteratorType >
    KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapper::~RangeIteratorWrapper()
    {
    }

    template< class XObjectType, class XRangeIteratorType >
    bool KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapper::operator<(KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapper& rhs)
    {
        return fPosition < rhs.GetPosition();
    }

    template< class XObjectType, class XRangeIteratorType >
    bool KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapper::operator<(const KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapper& rhs) const
    {
        return fPosition < rhs.GetPosition();
    }

    template< class XObjectType, class XRangeIteratorType >
    const XRangeIteratorType& KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapper::Iterator() const
    {
        return fIterator;
    }

    template< class XObjectType, class XRangeIteratorType >
    int KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapper::GetPosition() const
    {
        return fPosition;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapper::SetPosition(int position)
    {
        fPosition = position;
        return;
    }


    //***********************
    // KTCutable members
    //***********************

    template< class XObjectType, class XRangeIteratorType >
    KTCutable< XObjectType, XRangeIteratorType >::KTCutable() :
            fValid(),
            fAll(),
            fBegin(),
            fEnd()
    {
    }

    template< class XObjectType, class XRangeIteratorType >
    KTCutable< XObjectType, XRangeIteratorType >::KTCutable(const XRangeIteratorType& begin, const XRangeIteratorType& end) :
            fValid(),
            fAll(),
            fBegin(begin),
            fEnd(end)
    {
        int index = 0;
        XRangeIteratorType iter = begin;
        std::pair< RangeIteratorWrapperSetIt, bool > insertionPair;
        if (begin != end)
        {
            // insert the first object
            fAll.insert(RangeIteratorWrapper(iter, index));
            insertionPair = fValid.insert(RangeIteratorWrapper(iter, index));

            // all of the other objects
            iter++;
            index++;
            RangeIteratorWrapperSetIt setIter = insertionPair.first;
            for (; iter != end; iter++)
            {
                fAll.insert(RangeIteratorWrapper(iter, index));
                fValid.insert(setIter, RangeIteratorWrapper(iter, index));
                index++;
            }
        }
    }

    template< class XObjectType, class XRangeIteratorType >
    KTCutable< XObjectType, XRangeIteratorType >::KTCutable(const KTCutable< XObjectType, XRangeIteratorType >& orig) :
            fValid(orig.fValid),
            fAll(orig.fAll),
            fBegin(orig.fBegin),
            fEnd(orig.fEnd)
    {
    }

    template< class XObjectType, class XRangeIteratorType >
    KTCutable< XObjectType, XRangeIteratorType >::~KTCutable()
    {
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::Cut(const XRangeIteratorType& toCut)
    {
        RangeIteratorMapCIt imIter = fAll.get<XRangeIteratorType>().find(toCut);
        if (imIter == fAll.get<XRangeIteratorType>().end())
        {
            return;
        }
        //fValid.erase(RangeIteratorWrapper(toCut, imIter->second));
        fValid.erase(*imIter);
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::UnCut(const XRangeIteratorType& toUnCut)
    {
        RangeIteratorMapCIt imIter = fAll.get<XRangeIteratorType>().find(toUnCut);
        if (imIter == fAll.get<XRangeIteratorType>().end())
        {
            return;
        }
        //fValid.insert(RangeIteratorWrapper(toUnCut, imIter->second));
        fValid.insert(*imIter);
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::Cut(const XRangeIteratorType& toBeginCut, const XRangeIteratorType& toEndCut)
    {
        for (XRangeIteratorType itIter=toBeginCut; itIter != toEndCut; itIter++)
        {
            RangeIteratorMapCIt imIter = fAll.get<XRangeIteratorType>().find(itIter);
            if (imIter == fAll.get<XRangeIteratorType>().end()) break;
            //fValid.erase(RangeIteratorWrapper(itIter, imIter->second));
            fValid.erase(*imIter);
        }
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::UnCut(const XRangeIteratorType& toBeginUnCut, const XRangeIteratorType& toEndUnCut)
    {
        for (XRangeIteratorType itIter=toBeginUnCut; itIter != toEndUnCut; itIter++)
        {
            RangeIteratorMapCIt imIter = fAll.get<XRangeIteratorType>().find(itIter);
            if (imIter == fAll.get<XRangeIteratorType>().end())
            {
                return;
            }
            //fValid.insert(RangeIteratorWrapper(itIter, imIter->second));
            fValid.insert(*imIter);
        }
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    const typename KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapperSet& KTCutable< XObjectType, XRangeIteratorType >::GetValid() const
    {
        return fValid;
    }

    template< class XObjectType, class XRangeIteratorType >
    const XRangeIteratorType& KTCutable< XObjectType, XRangeIteratorType >::GetCutableBegin() const
    {
        return fBegin;
    }

    template< class XObjectType, class XRangeIteratorType >
    const XRangeIteratorType& KTCutable< XObjectType, XRangeIteratorType >::GetCutableEnd() const
    {
        return fEnd;
    }

    template< class XObjectType, class XRangeIteratorType >
    typename KTCutable< XObjectType, XRangeIteratorType >::iterator KTCutable< XObjectType, XRangeIteratorType >::begin() const
    {
        return iterator(fValid.begin());
    }

    template< class XObjectType, class XRangeIteratorType >
    typename KTCutable< XObjectType, XRangeIteratorType >::iterator KTCutable< XObjectType, XRangeIteratorType >::end() const
    {
        return iterator(fValid.end());
    }



    //***********************
    // iterator members
    //***********************

    template< class XObjectType, class XRangeIteratorType >
    KTCutable< XObjectType, XRangeIteratorType >::iterator::iterator() :
            fValidIter()
    {
    }

    template< class XObjectType, class XRangeIteratorType >
    KTCutable< XObjectType, XRangeIteratorType >::iterator::iterator(const KTCutable< XObjectType, XRangeIteratorType >::RangeIteratorWrapperSetIt& it) :
            fValidIter(it)
    {
    }

    template< class XObjectType, class XRangeIteratorType >
    KTCutable< XObjectType, XRangeIteratorType >::iterator::iterator(const KTCutable< XObjectType, XRangeIteratorType >::iterator& orig) :
            fValidIter(orig.fValidIter)
    {
    }

    template< class XObjectType, class XRangeIteratorType >
    KTCutable< XObjectType, XRangeIteratorType >::iterator::~iterator()
    {
    }

    template< class XObjectType, class XRangeIteratorType >
    XObjectType& KTCutable< XObjectType, XRangeIteratorType >::iterator::operator*()
    {
        return *(fValidIter->Iterator());
    }

    template< class XObjectType, class XRangeIteratorType >
    const XObjectType& KTCutable< XObjectType, XRangeIteratorType >::iterator::operator*() const
    {
        return *(fValidIter->Iterator());
    }

    template< class XObjectType, class XRangeIteratorType >
    typename KTCutable< XObjectType, XRangeIteratorType >::iterator& KTCutable< XObjectType, XRangeIteratorType >::iterator::operator=(const KTCutable< XObjectType, XRangeIteratorType >::iterator& rhs)
    {
        fValidIter = rhs.fValidIter;
        return *this;
    }

    template< class XObjectType, class XRangeIteratorType >
    bool KTCutable< XObjectType, XRangeIteratorType >::iterator::operator==(const KTCutable< XObjectType, XRangeIteratorType >::iterator& rhs) const
    {
        return this->fValidIter == rhs.fValidIter;
    }

    template< class XObjectType, class XRangeIteratorType >
    bool KTCutable< XObjectType, XRangeIteratorType >::iterator::operator!=(const KTCutable< XObjectType, XRangeIteratorType >::iterator& rhs) const
    {
        return this->fValidIter != rhs.fValidIter;
    }

    template< class XObjectType, class XRangeIteratorType >
    typename KTCutable< XObjectType, XRangeIteratorType >::iterator& KTCutable< XObjectType, XRangeIteratorType >::iterator::operator++()
    {
        fValidIter++;
        return *this;
    }

    template< class XObjectType, class XRangeIteratorType >
    typename KTCutable< XObjectType, XRangeIteratorType >::iterator  KTCutable< XObjectType, XRangeIteratorType >::iterator::operator++(int)
    {
        typename KTCutable< XObjectType, XRangeIteratorType >::iterator iterSave(*this);
        ++fValidIter;
        return iterSave;
    }

    template< class XObjectType, class XRangeIteratorType >
    typename KTCutable< XObjectType, XRangeIteratorType >::iterator& KTCutable< XObjectType, XRangeIteratorType >::iterator::operator--()
    {
        fValidIter--;
        return *this;
    }

    template< class XObjectType, class XRangeIteratorType >
    typename KTCutable< XObjectType, XRangeIteratorType >::iterator  KTCutable< XObjectType, XRangeIteratorType >::iterator::operator--(int)
    {
        typename KTCutable< XObjectType, XRangeIteratorType >::iterator iterSave(*this);
        --fValidIter;
        return iterSave;
    }

} /* namespace Katydid */
#endif /* KTCUTABLE_HH_ */
