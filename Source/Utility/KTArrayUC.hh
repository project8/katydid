/*
 * KKTArrayUC.hh
 *
 *  Created on: Sep 12, 2011
 *      Author: Rene Brun
 *      Copied from ROOT's TArrayC.h by N. Oblath
 */

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_KTArrayUC
#define ROOT_KTArrayUC


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// KTArrayUC                                                            //
//                                                                      //
// Array of unsigned chars or bytes (8 bits per element).               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TArray
#include "TArray.h"
#endif

//namespace Katydid
//{



    class KTArrayUC : public TArray {

    public:
       UChar_t    *fArray;       //[fN] Array of fN unsigned chars

       KTArrayUC();
       KTArrayUC(Int_t n);
       KTArrayUC(Int_t n, const UChar_t *array);
       KTArrayUC(const KTArrayUC &array);
       KTArrayUC    &operator=(const KTArrayUC &rhs);
       virtual    ~KTArrayUC();

       void          Adopt(Int_t n, UChar_t *array);
       void          AddAt(UChar_t c, Int_t i);
       UChar_t        At(Int_t i) const ;
       void          Copy(KTArrayUC &array) const {array.Set(fN,fArray);}
       const UChar_t *GetArray() const { return fArray; }
       UChar_t       *GetArray() { return fArray; }
       Double_t      GetAt(Int_t i) const { return At(i); }
       Stat_t        GetSum() const {Stat_t sum=0; for (Int_t i=0;i<fN;i++) sum+=fArray[i]; return sum;}
       void          Reset(UChar_t val=0)  {memset(fArray,val,fN*sizeof(UChar_t));}
       void          Set(Int_t n);
       void          Set(Int_t n, const UChar_t *array);
       void          SetAt(Double_t v, Int_t i) { AddAt((UChar_t)v, i); }
       UChar_t       &operator[](Int_t i);
       UChar_t        operator[](Int_t i) const;

       ClassDef(KTArrayUC,1)  //Array of unsigned chars
    };


    #if defined R__TEMPLATE_OVERLOAD_BUG
    template <>
    #endif
    inline TBuffer &operator>>(TBuffer &buf, KTArrayUC *&obj)
    {
       // Read KTArrayUC object from buffer.

       obj = (KTArrayUC *) TArray::ReadArray(buf, KTArrayUC::Class());
       return buf;
    }

    #if defined R__TEMPLATE_OVERLOAD_BUG
    template <>
    #endif
    inline TBuffer &operator<<(TBuffer &buf, const KTArrayUC *obj)
    {
       // Write a KTArrayUC object into buffer
       return buf << (TArray*)obj;
    }

    inline UChar_t KTArrayUC::At(Int_t i) const
    {
       if (!BoundsOk("KTArrayUC::At", i)) return 0;
       return fArray[i];
    }

    inline UChar_t &KTArrayUC::operator[](Int_t i)
    {
       if (!BoundsOk("KTArrayUC::operator[]", i))
          i = 0;
       return fArray[i];
    }

    inline UChar_t KTArrayUC::operator[](Int_t i) const
    {
       if (!BoundsOk("KTArrayUC::operator[]", i)) return 0;
       return fArray[i];
    }


//} /* namespace Katydid */

#endif /* ROOT_KTArrayUC */
