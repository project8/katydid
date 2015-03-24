/*
 * KTVarTypePhysicalArray.hh
 *
 *  Created on: Mar 23, 2015
 *      Author: nsoblath
 */

#ifndef KTVARTYPEPHYSICALARRAY_HH_
#define KTVARTYPEPHYSICALARRAY_HH_

#include "KTPhysicalArray.hh"
#include "KTException.hh"

#ifdef USE_MONARCH
#include "M3Constants.hh"
#endif

#include <cstring> // for memcpy
#include <inttypes.h>


namespace Katydid
{
    //*************************
    // Data formats
    //*************************
    // Data format types (values matched to Monarch3's data format types; incompatible with Monarch2's data format constants)
    static const uint32_t sInvalidFormat = UINT32_MAX;
    static const uint32_t sDigitizedUS = 0;
    static const uint32_t sDigitizedS = 1;
    static const uint32_t sAnalog = 2;

#ifdef USE_MONARCH
    uint32_t ConvertMonarch3DataFormat( uint32_t m3DataFormat );

    // Monarch2 data is all sDigitizedS
#endif


    //*************************
    // Type information
    // Specializations for valid types
    // Default implementation for non-used types
    //*************************

    template< typename XType >
    struct KTVTPATypeInfo
    {
            static bool IsUsable() {return false;}
            static size_t Size() {return sizeof( XType );}
            static bool IsSigned() {return false;}
            static uint32_t DataFormat() {return sInvalidFormat;}
    };

    template<>
    struct KTVTPATypeInfo< uint8_t >
    {
            static bool IsUsable() {return true;}
            static size_t Size() {return 1;}
            static bool IsSigned() {return false;}
            static uint32_t DataFormat() {return sDigitizedUS;}
    };

    template<>
    struct KTVTPATypeInfo< uint16_t >
    {
            static bool IsUsable() {return true;}
            static size_t Size() {return 2;}
            static bool IsSigned() {return false;}
            static uint32_t DataFormat() {return sDigitizedUS;}
    };

    template<>
    struct KTVTPATypeInfo< uint32_t >
    {
            static bool IsUsable() {return true;}
            static size_t Size() {return 4;}
            static bool IsSigned() {return false;}
            static uint32_t DataFormat() {return sDigitizedUS;}
    };

    template<>
    struct KTVTPATypeInfo< uint64_t >
    {
            static bool IsUsable() {return true;}
            static size_t Size() {return 8;}
            static bool IsSigned() {return false;}
            static uint32_t DataFormat() {return sDigitizedUS;}
    };

    template<>
    struct KTVTPATypeInfo< int8_t >
    {
            static bool IsUsable() {return true;}
            static size_t Size() {return 1;}
            static bool IsSigned() {return true;}
            static uint32_t DataFormat() {return sDigitizedS;}
    };

    template<>
    struct KTVTPATypeInfo< int16_t >
    {
            static bool IsUsable() {return true;}
            static size_t Size() {return 2;}
            static bool IsSigned() {return true;}
            static uint32_t DataFormat() {return sDigitizedS;}
    };

    template<>
    struct KTVTPATypeInfo< int32_t >
    {
            static bool IsUsable() {return true;}
            static size_t Size() {return 4;}
            static bool IsSigned() {return true;}
            static uint32_t DataFormat() {return sDigitizedS;}
    };

    template<>
    struct KTVTPATypeInfo< int64_t >
    {
            static bool IsUsable() {return true;}
            static size_t Size() {return 8;}
            static bool IsSigned() {return true;}
            static uint32_t DataFormat() {return sDigitizedS;}
    };

    template<>
    struct KTVTPATypeInfo< float >
    {
            static bool IsUsable() {return true;}
            static size_t Size() {return 4;}
            static bool IsSigned() {return true;}
            static uint32_t DataFormat() {return sAnalog;}
    };

    template<>
    struct KTVTPATypeInfo< double >
    {
            static bool IsUsable() {return true;}
            static size_t Size() {return 8;}
            static bool IsSigned() {return true;}
            static uint32_t DataFormat() {return sAnalog;}
    };



    //*************************
    // 1-D array
    //*************************

    /*!
     @class KTVarTypePhysicalArray
     @author N. S. Oblath

     @brief Physical array class with a variable underlying data type and interface type

     @details
     Three independent types are distinguished by this array class:
     - Storage array type: This is the underlying data storage array type; it is always uint8_t.
     - Data type: This is the type of the original data; the size of this type, combined with the number of bins, is used to determine how large the storage array needs to be.
                  It is either specified by type size and format, or by constructor template.
     - Interface type: This is the type by which the user sets and gets bin values.
                       It is the template argument for the class.
                       If you want to change the interface type, you need to create a new interface object with the interface-only constructor (copyData = false).

    */
    template< typename XInterfaceType >
    class KTVarTypePhysicalArray : public KTAxisProperties< 1 >
    {
        public:
            typedef uint8_t storage_value_type;
            typedef uint8_t* storage_type;
            typedef XInterfaceType ifc_value_type;
            /*
            typedef const fftw_complex* const_iterator;
            typedef fftw_complex* iterator;
            typedef const fftw_complex* const_reverse_iterator;
            typedef fftw_complex* reverse_iterator;
            */

        private:
            typedef KTNBinsInArray< 1, FixedSize > XNBinsFunctor;

        public:
            /// Default constructor; not terribly useful
            KTVarTypePhysicalArray();

            /// Own-data constructor w/ data-type template
            template< typename XDataType >
            KTVarTypePhysicalArray(size_t nBins, double rangeMin=0., double rangeMax=1.);

            /// Own-data constructor w/ data type & format specified
            KTVarTypePhysicalArray(size_t dataTypeSize, uint32_t dataFormat, size_t nBins, double rangeMin=0., double rangeMax=1.);

            /// Interface-only (copyData = false) or copy (copyData = true; default) constructor
            template< typename XOrigInterfaceType >
            KTVarTypePhysicalArray(const KTVarTypePhysicalArray< XOrigInterfaceType >& orig, bool copyData = true);

            virtual ~KTVarTypePhysicalArray();

            template< typename XOrigInterfaceType >
            KTVarTypePhysicalArray& operator=(const KTVarTypePhysicalArray< XOrigInterfaceType >& rhs);

        public:
            const storage_type& GetStorage() const;
            storage_type& GetStorage();

            size_t GetNBytes() const;
            size_t GetDataTypeSize() const;
            uint32_t GetDataFormat() const;

        protected:
            bool fOwnsStorage;

            // storage_type is always uint8_t
            union
            {
                uint8_t*  fUByteData;
                uint16_t* fU2BytesData;
                uint32_t* fU4BytesData;
                uint64_t* fU8BytesData;
                int8_t*   fIByteData;
                int16_t*  fI2BytesData;
                int32_t*  fI4BytesData;
                int64_t*  fI8BytesData;
                float*    fF4BytesData;
                double*   fF8BytesData;
            };
            size_t fNBytes;

            // these parameters describe the original data format
            size_t fDataTypeSize;
            uint32_t fDataFormat;

        public:
            ifc_value_type operator()(unsigned i) const;
            void SetAt(ifc_value_type value, unsigned i);

        protected:
            void SetInterfaceFunctions( unsigned aDataTypeSize, uint32_t aDataFormat );

            ifc_value_type AtU1( unsigned index ) const;
            ifc_value_type AtU2( unsigned index ) const;
            ifc_value_type AtU4( unsigned index ) const;
            ifc_value_type AtU8( unsigned index ) const;
            ifc_value_type AtI1( unsigned index ) const;
            ifc_value_type AtI2( unsigned index ) const;
            ifc_value_type AtI4( unsigned index ) const;
            ifc_value_type AtI8( unsigned index ) const;
            ifc_value_type AtF4( unsigned index ) const;
            ifc_value_type AtF8( unsigned index ) const;

            //typedef XInterfaceType (KTVarTypePhysicalArray< XInterfaceType >::*GetFcnPtr)( unsigned ) const;
            XInterfaceType (KTVarTypePhysicalArray< XInterfaceType >::*fArrayGetFcn)( unsigned ) const;

            void SetAtU1( ifc_value_type value, unsigned index );
            void SetAtU2( ifc_value_type value, unsigned index );
            void SetAtU4( ifc_value_type value, unsigned index );
            void SetAtU8( ifc_value_type value, unsigned index );
            void SetAtI1( ifc_value_type value, unsigned index );
            void SetAtI2( ifc_value_type value, unsigned index );
            void SetAtI4( ifc_value_type value, unsigned index );
            void SetAtI8( ifc_value_type value, unsigned index );
            void SetAtF4( ifc_value_type value, unsigned index );
            void SetAtF8( ifc_value_type value, unsigned index );

            void (KTVarTypePhysicalArray< XInterfaceType >::*fArraySetFcn)( XInterfaceType, unsigned );




            /*
        public:
            bool IsCompatibleWith(const KTPhysicalArray< 1, fftw_complex >& rhs) const;

            KTPhysicalArray< 1, fftw_complex >& operator=(const KTPhysicalArray< 1, fftw_complex >& rhs);

            KTPhysicalArray< 1, fftw_complex >& operator+=(const KTPhysicalArray< 1, fftw_complex >& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator-=(const KTPhysicalArray< 1, fftw_complex >& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator*=(const KTPhysicalArray< 1, fftw_complex >& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator/=(const KTPhysicalArray< 1, fftw_complex >& rhs);

            KTPhysicalArray< 1, fftw_complex >& operator+=(const fftw_complex& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator-=(const fftw_complex& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator*=(const fftw_complex& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator/=(const fftw_complex& rhs);

            KTPhysicalArray< 1, fftw_complex >& operator*=(double rhs);

        public:
            const_iterator begin() const;
            const_iterator end() const;
            iterator begin();
            iterator end();

            const_reverse_iterator rbegin() const;
            const_reverse_iterator rend() const;
            reverse_iterator rbegin();
            reverse_iterator rend();
            */

    };

    template< typename XInterfaceType >
    KTVarTypePhysicalArray< XInterfaceType >::KTVarTypePhysicalArray() :
            KTAxisProperties< 1 >(),
            fOwnsStorage(true),
            fUByteData(new uint8_t [1]),
            fNBytes(0),
            fDataTypeSize(0),
            fDataFormat(sInvalidFormat),
            fArrayGetFcn(NULL),
            fArraySetFcn(NULL)
    {
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(0));
    }


    template< typename XInterfaceType >
    template< typename XDataType >
    KTVarTypePhysicalArray< XInterfaceType >::KTVarTypePhysicalArray(size_t nBins, double rangeMin, double rangeMax) :
            KTAxisProperties< 1 >(rangeMin, rangeMax),
            fOwnsStorage(true),
            fUByteData(reinterpret_cast< uint8_t* >(new XDataType[ nBins ])),
            fNBytes(nBins * sizeof(XDataType)),
            fDataTypeSize(0),
            fDataFormat(sInvalidFormat),
            fArrayGetFcn(NULL),
            fArraySetFcn(NULL)
    {
        try
        {
            SetInterface( KTVTPATypeInfo< XDataType >::Size(), KTVTPATypeInfo< XDataType >::DataFormat() );
        }
        catch( KTException& e ) {throw e;}
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(nBins));
    }

    template< typename XInterfaceType >
    KTVarTypePhysicalArray< XInterfaceType >::KTVarTypePhysicalArray(size_t dataTypeSize, uint32_t dataFormat, size_t nBins, double rangeMin, double rangeMax) :
            KTAxisProperties< 1 >(rangeMin, rangeMax),
            fOwnsStorage(true),
            fUByteData(new uint8_t[ nBins * dataTypeSize ]),
            fNBytes(nBins * dataTypeSize),
            fDataTypeSize(dataTypeSize),
            fDataFormat(dataFormat),
            fArrayGetFcn(NULL),
            fArraySetFcn(NULL)
    {
        try
        {
            SetInterfaceFunctions( dataTypeSize, dataFormat );
        }
        catch( KTException& e ) {throw e;}
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(nBins));
    }


    template< typename XInterfaceType >
    template< typename XOrigInterfaceType >
    KTVarTypePhysicalArray< XInterfaceType >::KTVarTypePhysicalArray(const KTVarTypePhysicalArray< XOrigInterfaceType >& orig, bool copyData) :
            KTAxisProperties< 1 >(orig),
            fOwnsStorage(copyData),
            fUByteData(NULL),
            fNBytes(orig.GetNBytes()),
            fDataTypeSize(orig.GetDataTypeSize()),
            fDataFormat(orig.GetDataFormat()),
            fArrayGetFcn(NULL),
            fArraySetFcn(NULL)
    {
        SetInterfaceFunctions( fDataTypeSize, fDataFormat );
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(orig.size()));
        if (copyData)
        {
            fUByteData = new uint8_t[ fNBytes ];
            memcpy( fUByteData, orig.GetStorage(), fNBytes );
        }
        else
        {
            fUByteData = orig.GetStorage();
        }
    }


    template< typename XInterfaceType >
    KTVarTypePhysicalArray< XInterfaceType >::~KTVarTypePhysicalArray()
    {
        if (fOwnsStorage && fUByteData != NULL)
        {
            delete [] fUByteData;
        }
    }


    template< typename XInterfaceType >
    template< typename XOrigInterfaceType >
    KTVarTypePhysicalArray< XInterfaceType >& KTVarTypePhysicalArray< XInterfaceType >::operator=(const KTVarTypePhysicalArray< XOrigInterfaceType >& rhs)
    {
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(rhs.size()));

        fDataTypeSize = rhs.GetDataTypeSize();
        fDataFormat = rhs.GetDataFormat();
        SetInterfaceFunctions( fDataTypeSize, fDataFormat );

        fOwnsStorage = true;
        fNBytes = rhs.GetNBytes();
        fUByteData = new uint8_t[ fNBytes ];
        memcpy( fUByteData, rhs.GetStorage(), fNBytes );

        return *this;
    }


    template< typename XInterfaceType >
    void KTVarTypePhysicalArray< XInterfaceType >::SetInterfaceFunctions( unsigned aDataTypeSize, uint32_t aDataFormat )
    {
        if( aDataFormat == sDigitizedUS )
        {
            if( aDataTypeSize == 1 )
            {
                fArrayGetFcn = &KTVarTypePhysicalArray< XInterfaceType >::AtU1;
                fArraySetFcn = &KTVarTypePhysicalArray< XInterfaceType >::SetAtU1;
            }
            else if( aDataTypeSize == 2 )
            {
                fArrayGetFcn = &KTVarTypePhysicalArray< XInterfaceType >::AtU2;
                fArraySetFcn = &KTVarTypePhysicalArray< XInterfaceType >::SetAtU2;
            }
            else if( aDataTypeSize == 4 )
            {
                fArrayGetFcn = &KTVarTypePhysicalArray< XInterfaceType >::AtU4;
                fArraySetFcn = &KTVarTypePhysicalArray< XInterfaceType >::SetAtU4;
            }
            else if( aDataTypeSize == 8 )
            {
                fArrayGetFcn = &KTVarTypePhysicalArray< XInterfaceType >::AtU8;
                fArraySetFcn = &KTVarTypePhysicalArray< XInterfaceType >::SetAtU8;
            }
            else
            {
                throw KTException() << "Unable to make a digitized unsigned data interface with data type size " << aDataTypeSize;
            }
        }
        else if( aDataFormat == sDigitizedS )
        {
            if( aDataTypeSize == 1 )
            {
                fArrayGetFcn = &KTVarTypePhysicalArray< XInterfaceType >::AtI1;
                fArraySetFcn = &KTVarTypePhysicalArray< XInterfaceType >::SetAtI1;
            }
            else if( aDataTypeSize == 2 )
            {
                fArrayGetFcn = &KTVarTypePhysicalArray< XInterfaceType >::AtI2;
                fArraySetFcn = &KTVarTypePhysicalArray< XInterfaceType >::SetAtI2;
            }
            else if( aDataTypeSize == 4 )
            {
                fArrayGetFcn = &KTVarTypePhysicalArray< XInterfaceType >::AtI4;
                fArraySetFcn = &KTVarTypePhysicalArray< XInterfaceType >::SetAtI4;
            }
            else if( aDataTypeSize == 8 )
            {
                fArrayGetFcn = &KTVarTypePhysicalArray< XInterfaceType >::AtI8;
                fArraySetFcn = &KTVarTypePhysicalArray< XInterfaceType >::SetAtI8;
            }
            else
            {
                throw KTException() << "Unable to make a digitized signed data interface with data type size " << aDataTypeSize;
            }
        }
        else if( aDataFormat == sAnalog )
        {
            if( aDataTypeSize == 4 )
            {
                fArrayGetFcn = &KTVarTypePhysicalArray< XInterfaceType >::AtF4;
                fArraySetFcn = &KTVarTypePhysicalArray< XInterfaceType >::SetAtF4;
            }
            else if( aDataTypeSize == 8 )
            {
                fArrayGetFcn = &KTVarTypePhysicalArray< XInterfaceType >::AtF8;
                fArraySetFcn = &KTVarTypePhysicalArray< XInterfaceType >::SetAtF8;
            }
            else
            {
                throw KTException() << "Unable to make a analog data interface with data type size " << aDataTypeSize;
            }
        }
        else
        {
            throw KTException() << "Invalid combination of data format <" << aDataFormat << ">, data type size <" << aDataTypeSize << ">";
        }
        return;
    }


    template< typename XInterfaceType >
    inline const typename KTVarTypePhysicalArray< XInterfaceType >::storage_type& KTVarTypePhysicalArray< XInterfaceType >::GetStorage() const
    {
        return fUByteData;
    }


    template< typename XInterfaceType >
    inline typename KTVarTypePhysicalArray< XInterfaceType >::storage_type& KTVarTypePhysicalArray< XInterfaceType >::GetStorage()
    {
        return fUByteData;
    }


    template< typename XInterfaceType >
    inline size_t KTVarTypePhysicalArray< XInterfaceType >::GetNBytes() const
    {
        return fNBytes;
    }

    template< typename XInterfaceType >
    inline size_t KTVarTypePhysicalArray< XInterfaceType >::GetDataTypeSize() const
    {
        return fDataTypeSize;
    }

    template< typename XInterfaceType >
    inline uint32_t KTVarTypePhysicalArray< XInterfaceType >::GetDataFormat() const
    {
        return fDataFormat;
    }


    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::operator()(unsigned i) const
    {
        return (this->*fArrayGetFcn)( i );
    }

    template< typename XInterfaceType >
    inline void KTVarTypePhysicalArray< XInterfaceType >::SetAt(XInterfaceType value, unsigned i)
    {
        (this->*fArraySetFcn)( value, i );
        return;
    }


    // Protected Get functions

    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::AtU1( unsigned index ) const
    {
        return fUByteData[ index ];
    }

    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::AtU2( unsigned index ) const
    {
        return fU2BytesData[ index ];
    }

    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::AtU4( unsigned index ) const
    {
        return fU4BytesData[ index ];
    }

    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::AtU8( unsigned index ) const
    {
        return fU8BytesData[ index ];
    }

    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::AtI1( unsigned index ) const
    {
        return fIByteData[ index ];
    }

    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::AtI2( unsigned index ) const
    {
        return fI2BytesData[ index ];
    }

    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::AtI4( unsigned index ) const
    {
        return fI4BytesData[ index ];
    }

    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::AtI8( unsigned index ) const
    {
        return fI8BytesData[ index ];
    }

    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::AtF4( unsigned index ) const
    {
        return fF4BytesData[ index ];
    }

    template< typename XInterfaceType >
    inline XInterfaceType KTVarTypePhysicalArray< XInterfaceType >::AtF8( unsigned index ) const
    {
        return fF8BytesData[ index ];
    }


    // Protected Set functions

    template< typename XInterfaceType >
    inline void KTVarTypePhysicalArray< XInterfaceType >::SetAtU1( XInterfaceType value, unsigned index )
    {
        fUByteData[ index ] = value;
    }

    template< typename XInterfaceType >
    inline void KTVarTypePhysicalArray< XInterfaceType >::SetAtU2( XInterfaceType value, unsigned index )
    {
        fU2BytesData[ index ] = value;
    }

    template< typename XInterfaceType >
    void KTVarTypePhysicalArray< XInterfaceType >::SetAtU4( XInterfaceType value, unsigned index )
    {
        fU4BytesData[ index ] = value;
    }

    template< typename XInterfaceType >
    inline void KTVarTypePhysicalArray< XInterfaceType >::SetAtU8( XInterfaceType value, unsigned index )
    {
        fU8BytesData[ index ] = value;
    }

    template< typename XInterfaceType >
    inline void KTVarTypePhysicalArray< XInterfaceType >::SetAtI1( XInterfaceType value, unsigned index )
    {
        fIByteData[ index ] = value;
    }

    template< typename XInterfaceType >
    inline void KTVarTypePhysicalArray< XInterfaceType >::SetAtI2( XInterfaceType value, unsigned index )
    {
        fI2BytesData[ index ] = value;
    }

    template< typename XInterfaceType >
    inline void KTVarTypePhysicalArray< XInterfaceType >::SetAtI4( XInterfaceType value, unsigned index )
    {
        fI4BytesData[ index ] = value;
    }

    template< typename XInterfaceType >
    inline void KTVarTypePhysicalArray< XInterfaceType >::SetAtI8( XInterfaceType value, unsigned index )
    {
        fI8BytesData[ index ] = value;
    }

    template< typename XInterfaceType >
    inline void KTVarTypePhysicalArray< XInterfaceType >::SetAtF4( XInterfaceType value, unsigned index )
    {
        fF4BytesData[ index ] = value;
    }

    template< typename XInterfaceType >
    inline void KTVarTypePhysicalArray< XInterfaceType >::SetAtF8( XInterfaceType value, unsigned index )
    {
        fF8BytesData[ index ] = value;
    }



    //****************************************
    // Operator definitions for fftw_complex
    //****************************************

    //std::ostream& operator<< (std::ostream& ostr, const fftw_complex& rhs);



    /*
    bool KTVarTypePhysicalArray< XInterfaceType >::IsCompatibleWith(const KTVarTypePhysicalArray& rhs) const
    {
        //return (this->size() == rhs.size() && this->GetRangeMin() == rhs.GetRangeMin() && this->GetRangeMax() == GetRangeMax());
        return (this->size() == rhs.size());
    }


    KTVarTypePhysicalArray& KTVarTypePhysicalArray< XInterfaceType >::operator=(const KTPhysicalArray< 1, fftw_complex>& rhs)
    {
        if (fData != NULL)
        {
            fftw_free(fData);
        }
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(rhs.size()));
        fData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size());
        memcpy( fData, rhs.fData, rhs.size() * sizeof( fftw_complex ) );
        KTAxisProperties< 1 >::operator=(rhs);
        return *this;
    }


    KTVarTypePhysicalArray& KTVarTypePhysicalArray< XInterfaceType >::operator+=(const KTPhysicalArray< 1, fftw_complex>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBin=0; iBin<rhs.size(); ++iBin)
        {
            fData[iBin][0] = fData[iBin][0] + rhs.fData[iBin][0];
            fData[iBin][1] = fData[iBin][1] + rhs.fData[iBin][1];
        }
        return *this;
    }


    KTVarTypePhysicalArray& KTVarTypePhysicalArray< XInterfaceType >::operator-=(const KTPhysicalArray< 1, fftw_complex>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin][0] = fData[iBin][0] - rhs.fData[iBin][0];
            fData[iBin][1] = fData[iBin][1] - rhs.fData[iBin][1];
        }
        return *this;
    }


    KTVarTypePhysicalArray& KTVarTypePhysicalArray< XInterfaceType >::operator*=(const KTPhysicalArray< 1, fftw_complex>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fTempCache[0] = fData[iBin][0] * rhs.fData[iBin][0] - fData[iBin][1] * rhs.fData[iBin][1];
            fTempCache[1] = fData[iBin][0] * rhs.fData[iBin][1] + fData[iBin][1] * rhs.fData[iBin][0];
            fData[iBin][0] = fTempCache[0];
            fData[iBin][1] = fTempCache[1];
        }
        return *this;
    }


    KTVarTypePhysicalArray& KTVarTypePhysicalArray< XInterfaceType >::operator/=(const KTPhysicalArray< 1, fftw_complex>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        double abs, arg;
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            abs = std::sqrt((fData[iBin][0]*fData[iBin][0] + fData[iBin][1]*fData[iBin][1]) / (rhs.fData[iBin][0]*rhs.fData[iBin][0] + rhs.fData[iBin][1]*rhs.fData[iBin][1]));
            arg = std::atan2(fData[iBin][1], fData[iBin][0]) - std::atan2(rhs.fData[iBin][1], rhs.fData[iBin][0]);
            fTempCache[0] = abs * std::cos(arg);
            fTempCache[1] = abs * std::sin(arg);
            fData[iBin][0] = fTempCache[0];
            fData[iBin][1] = fTempCache[1];
        }
        return *this;
    }


    KTVarTypePhysicalArray& KTVarTypePhysicalArray< XInterfaceType >::operator+=(const fftw_complex& rhs)
    {
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin][0] = fData[iBin][0] + rhs[0];
            fData[iBin][1] = fData[iBin][1] + rhs[1];
        }
        return *this;
    }


    KTVarTypePhysicalArray& KTVarTypePhysicalArray< XInterfaceType >::operator-=(const fftw_complex& rhs)
    {
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin][0] = fData[iBin][0] - rhs[0];
            fData[iBin][1] = fData[iBin][1] - rhs[1];
        }
        return *this;
    }


    KTVarTypePhysicalArray& KTVarTypePhysicalArray< XInterfaceType >::operator*=(const fftw_complex& rhs)
    {
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fTempCache[0] = fData[iBin][0] * rhs[0] - fData[iBin][1] * rhs[1];
            fTempCache[1] = fData[iBin][0] * rhs[1] + fData[iBin][1] * rhs[0];
            fData[iBin][0] = fTempCache[0];
            fData[iBin][1] = fTempCache[1];
        }
        return *this;
    }


    KTVarTypePhysicalArray& KTVarTypePhysicalArray< XInterfaceType >::operator/=(const fftw_complex& rhs)
    {
        double abs, arg;
        double rhsabs = rhs[0]*rhs[0] + rhs[1]*rhs[1];
        double rhsarg = std::atan2(rhs[1], rhs[0]);
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            abs = std::sqrt((fData[iBin][0]*fData[iBin][0] + fData[iBin][1]*fData[iBin][1]) / rhsabs);
            arg = std::atan2(fData[iBin][1], fData[iBin][0]) - rhsarg;
            fTempCache[0] = abs * std::cos(arg);
            fTempCache[1] = abs * std::sin(arg);
            fData[iBin][0] = fTempCache[0];
            fData[iBin][1] = fTempCache[1];
        }
        return *this;
    }


    KTVarTypePhysicalArray& KTVarTypePhysicalArray< XInterfaceType >::operator*=(double rhs)
    {
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin][0] = fData[iBin][0] * rhs;
            fData[iBin][1] = fData[iBin][1] * rhs;
        }
        return *this;
    }



    KTVarTypePhysicalArray< XInterfaceType >::const_iterator KTVarTypePhysicalArray< XInterfaceType >::begin() const
    {
        return fData;
    }


    KTVarTypePhysicalArray< XInterfaceType >::const_iterator KTVarTypePhysicalArray< XInterfaceType >::end() const
    {
        return fData + size();
    }


    KTVarTypePhysicalArray< XInterfaceType >::iterator KTVarTypePhysicalArray< XInterfaceType >::begin()
    {
        return fData;
    }


    KTVarTypePhysicalArray< XInterfaceType >::iterator KTVarTypePhysicalArray< XInterfaceType >::end()
    {
        return fData + size();
    }



    KTVarTypePhysicalArray< XInterfaceType >::const_reverse_iterator KTVarTypePhysicalArray< XInterfaceType >::rbegin() const
    {
        return fData + size() - 1;
    }


    KTVarTypePhysicalArray< XInterfaceType >::const_reverse_iterator KTVarTypePhysicalArray< XInterfaceType >::rend() const
    {
        return fData - 1;
    }


    KTVarTypePhysicalArray< XInterfaceType >::reverse_iterator KTVarTypePhysicalArray< XInterfaceType >::rbegin()
    {
        return fData + size() - 1;
    }


    KTVarTypePhysicalArray< XInterfaceType >::reverse_iterator KTVarTypePhysicalArray< XInterfaceType >::rend()
    {
        return fData - 1;
    }
*/



    //********************************************
    // Operator implementations for fftw_complex
    //********************************************
/*
    std::ostream&
        operator<< (std::ostream& ostr, const fftw_complex& rhs)
    {
        ostr << "(" << rhs[0] << "," << rhs[1] << ")";
        return ostr;
    }
*/

} /* namespace Katydid */
#endif /* KTVARTYPEPHYSICALARRAY_HH_ */
