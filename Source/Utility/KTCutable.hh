/**
 @file KTCutable.hh
 @brief Contains KTCutable and associated classes
 @details KTCutable is a pseudo-container that allows you to apply cuts to a set of objects using STL-like iterator notation.
 @author: N. S. Oblath
 */

#ifndef KTCUTABLE_HH_
#define KTCUTABLE_HH_

#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>

#include <cstddef>
#include <set>
#include <utility>

namespace Katydid
{
    //
    /*!
     @class KTCutable
     @author N. S. Oblath

     @brief A pseudo-container class that allows you to cut out ranges using STL-like iterator notation.

     @details
     A KTCutable object is not a true container, in that it doesn't own the objects it refers to.  The objects remain
     in their original locations, and KTCutable uses the iterators provided to it to find them.  Please note that
     this means that your code will crash if the original objects move or are deleted without updating the KTCutable!

     The template parameters are:
     \li XObjectType: the type of object that will be referred to.
     \li XRangeIteratorType: the type of the iterator used to refer to the objects.

     The main constructor takes two arguments: the begin and end iterators for the range of objects.

     Cuts can be applied and removed singly or using a sub-range.  An iterator class (KTCutable::iterator) is provided,
     which will iterate over the given range, skipping any objects that have been cut.  This is done in a way that
     avoids overhead during iteration at the cost of additional overhead during creation and cut application/removal, as
     well as memory overhead in the form of one or two copies of XRangeIteratorType for each object.

     KTCutable::iterator is a bi-directional iterator.  However, since the iteration order from XRangeIteratorType is
     maintained, if XRangeIteratorType is a reverse iterator, KTCutable::iterator will act in reverse as well.
     Currently the iterator must be dereference-able with operator*, though hopefully the option to change the
     dereferencing function will be added in the future.
     */
    template< class XObjectType, class XRangeIteratorType >
    class KTCutable
    {
            /*!
             @class RangeIteratorWrapper
             @author N. S. Oblath

             @brief Combines the RangeIterator with its original position and a < operator

             @details
             The RangeIterator is the original type of iterator given to KTCutable.  This class wraps that iterator with
             other information and capabilities:
             \li fPosition is the iterators position in the original range given to KTCutable.
             \li operator< allows a RangeIteratorWrapper to be compared to other RangeIteratorWrappers based on position.
             */
        private:
            class RangeIteratorWrapper
            {
                public:
                    RangeIteratorWrapper(const XRangeIteratorType& original, int position);
                    ~RangeIteratorWrapper();

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
            struct RangeIteratorEqualTo
            {
                bool operator() (const XRangeIteratorType& lhs, const XRangeIteratorType& rhs) const
                {
                    return lhs==rhs;
                }
            };

            struct RangeIteratorHash
            {
                std::size_t operator()(const XRangeIteratorType& it) const
                {
                    return boost::hash<long unsigned int>()((long unsigned int)&(*it));
                }
            };

        private:
            typedef boost::unordered_map< XRangeIteratorType, int, RangeIteratorHash, RangeIteratorEqualTo > RangeIteratorMap;
            typedef typename RangeIteratorMap::const_iterator RangeIteratorMapCIt;
            typedef typename RangeIteratorMap::value_type RangeIteratorMapType;


            /*!
             @typedef value_type
             @brief The type of the object being referred to
             */
        public:
            typedef XObjectType value_type;

            /*!
             @typedef value_type_range_iterator
             @brief The type of iterator referring to the objects and defining the range being used
             */
        public:
            typedef XRangeIteratorType value_type_range_iterator;


        private:
            typedef std::set< RangeIteratorWrapper > RangeIteratorWrapperSet;
            typedef typename RangeIteratorWrapperSet::iterator RangeIteratorWrapperSetIt;


            /*!
             @class iterator
             @brief A bi-directional iterator over the interval that skips any cut objects
             @details
             This iterator will traverse the specified range of objects, skipping any that have been cut out.

             This is a bi-directional iterator, but it will adopt some of the characteristics of the original
             iterator passed to KTCutable.  For instance, if XRangeIteratorType is a reverse iterator, then
             KTCutable::iterator will act in reverse.
             */

        public:
            class iterator
            {
                public:
                    iterator();
                    iterator(const RangeIteratorWrapperSetIt& it);
                    iterator(const iterator& orig); /// copy constructor
                    ~iterator();

                    /// Dereferences the iterator
                    XObjectType& operator*();
                    /// Dereferences the iterator (const)
                    const XObjectType& operator*() const;

                    /// Set the pointer (copyable)
                    iterator& operator=(const iterator& rhs);

                    /// Increment the iterator (postfix)
                    iterator& operator++();
                    /// Increment the iterator (prefix)
                    iterator  operator++(int);
                    /// Decrement the iterator (postfix)
                    iterator& operator--();
                    /// Decrement the iterator (prefix)
                    iterator  operator--(int);

                    /// Test for equality
                    bool operator==(const iterator& rhs) const;
                    /// Test for inequality
                    bool operator!=(const iterator& lhs) const;

                private:
                    RangeIteratorWrapperSetIt fValidIter;

            };


            //***********************
            // Cutable class
            //***********************

            // constructors and destructor
        public:
            KTCutable(); /// default constructor
            KTCutable(const XRangeIteratorType& begin, const XRangeIteratorType& end); /// primary constructor
            KTCutable(const KTCutable< XObjectType, XRangeIteratorType >& orig); /// copy constructor
            ~KTCutable();

        public:
            /// Copy another KTCutable's range and cuts
            KTCutable< XObjectType, XRangeIteratorType >& operator=(const KTCutable& rhs);

        public:
            /// Clears all data, including the map of range iterators and the valid iterators
            void Clear();
            /// Sets a new range; all iterators are valid
            void ResetRange(const XRangeIteratorType& begin, const XRangeIteratorType& end);
            /// Copy the range from another KTCutable, but not the cuts; all iterators are valid
            void ResetRange(const KTCutable< XObjectType, XRangeIteratorType >& orig);

        public:
            /// Add a cut to a single iterator position
            void Cut(const XRangeIteratorType& toCut);
            /// Remove a cut to a single iterator position
            void UnCut(const XRangeIteratorType& toUnCut);

            /// Add a cut to a range of iterator positions
            void Cut(const XRangeIteratorType& toBeginCut, const XRangeIteratorType& toEndCut);
            /// Remove a cut to a range of iterator positions
            void UnCut(const XRangeIteratorType& toBeginUnCut, const XRangeIteratorType& toEndUnCut);

            /// Cut all iterator positions
            void CutAll();
            /// Remove all cuts
            void UnCutAll();

        public:
            const XRangeIteratorType& GetCutableBegin() const; /// Get the range iterator to the beginning of the range
            const XRangeIteratorType& GetCutableEnd() const; /// Get the range iterator to the end of the range

        private:
            RangeIteratorWrapperSet fValid;

            RangeIteratorMap fAll;

            XRangeIteratorType fBegin;
            XRangeIteratorType fEnd;

            //***********************
            // standard container functions
            //***********************

        public:
            /// Returns the size of set of valid objects
            std::size_t size() const;
            /// Returns an iterator to the first valid object
            iterator begin() const;
            /// Returns an iterator after the last valid object
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
            fAll.insert(RangeIteratorMapType(iter, index));
            insertionPair = fValid.insert(RangeIteratorWrapper(iter, index));

            // all of the other objects
            iter++;
            index++;
            RangeIteratorWrapperSetIt setIter = insertionPair.first;
            for (; iter != end; iter++)
            {
                fAll.insert(RangeIteratorMapType(iter, index));
                fValid.insert(setIter, RangeIteratorWrapper(iter, index));
                index++;
                setIter++;
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
    void KTCutable< XObjectType, XRangeIteratorType >::Clear()
    {
        fValid.clear();
        fAll.clear();
        fBegin = XRangeIteratorType();
        fEnd = XRangeIteratorType();
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::ResetRange(const XRangeIteratorType& begin, const XRangeIteratorType& end)
    {
        fValid.clear();
        fAll.clear();
        fBegin = begin;
        fEnd = end;

        int index = 0;
        XRangeIteratorType iter = fBegin;
        std::pair< RangeIteratorWrapperSetIt, bool > insertionPair;
        if (fBegin != fEnd)
        {
            // insert the first object
            fAll.insert(RangeIteratorMapType(iter, index));
            insertionPair = fValid.insert(RangeIteratorWrapper(iter, index));

            // all of the other objects
            iter++;
            index++;
            RangeIteratorWrapperSetIt setIter = insertionPair.first;
            for (; iter != fEnd; iter++)
            {
                fAll.insert(RangeIteratorMapType(iter, index));
                fValid.insert(setIter, RangeIteratorWrapper(iter, index));
                index++;
                setIter++;
            }
        }
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::ResetRange(const KTCutable< XObjectType, XRangeIteratorType >& orig)
    {
        fValid.clear();
        fAll = orig.fAll;
        fBegin = orig.fBegin;
        fEnd = orig.fEnd;

        int index = 0;
        XRangeIteratorType iter = fBegin;
        std::pair< RangeIteratorWrapperSetIt, bool > insertionPair;
        if (fBegin != fEnd)
        {
            // insert the first object
            insertionPair = fValid.insert(RangeIteratorWrapper(iter, index));

            // all of the other objects
            iter++;
            index++;
            RangeIteratorWrapperSetIt setIter = insertionPair.first;
            for (; iter != fEnd; iter++)
            {
                fValid.insert(setIter, RangeIteratorWrapper(iter, index));
                index++;
                setIter++;
            }
        }
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::Cut(const XRangeIteratorType& toCut)
    {
        RangeIteratorMapCIt imIter = fAll.find(toCut);
        if (imIter == fAll.end())
        {
            return;
        }
        fValid.erase(RangeIteratorWrapper(toCut, imIter->second));
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::UnCut(const XRangeIteratorType& toUnCut)
    {
        RangeIteratorMapCIt imIter = fAll.find(toUnCut);
        if (imIter == fAll.end())
        {
            return;
        }
        fValid.insert(RangeIteratorWrapper(toUnCut, imIter->second));
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::Cut(const XRangeIteratorType& toBeginCut, const XRangeIteratorType& toEndCut)
    {
        for (XRangeIteratorType itIter=toBeginCut; itIter != toEndCut; itIter++)
        {
            RangeIteratorMapCIt imIter = fAll.find(itIter);
            if (imIter == fAll.end()) break;
            fValid.erase(RangeIteratorWrapper(itIter, imIter->second));
        }
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::UnCut(const XRangeIteratorType& toBeginUnCut, const XRangeIteratorType& toEndUnCut)
    {
        for (XRangeIteratorType itIter=toBeginUnCut; itIter != toEndUnCut; itIter++)
        {
            RangeIteratorMapCIt imIter = fAll.find(itIter);
            if (imIter == fAll.end()) return;
            fValid.insert(RangeIteratorWrapper(itIter, imIter->second));
        }
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::CutAll()
    {
        fValid.clear();
        return;
    }

    template< class XObjectType, class XRangeIteratorType >
    void KTCutable< XObjectType, XRangeIteratorType >::UnCutAll()
    {
        for (RangeIteratorMapCIt imIter = fAll.begin(); imIter != fAll.end(); imIter++)
        {
            fValid.insert(RangeIteratorWrapper(imIter->first, imIter->second));
        }
        return;
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
    std::size_t KTCutable< XObjectType, XRangeIteratorType >::size() const
    {
        return fValid.size();
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
