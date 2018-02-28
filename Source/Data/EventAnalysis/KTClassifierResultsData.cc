/*
 * KTClassifierResultsData.cc
 *
 *  Created on: Feb 16, 2018
 *      Author: ezayas, L. Saldana
 */

#include "KTClassifierResultsData.hh"

#include <ostream>

namespace Katydid
{
    const std::string KTClassifierResultsData::sName("classifier-results");

    KTClassifierResultsData::KTClassifierResultsData() :
            KTExtensibleData< KTClassifierResultsData >(),
            fMCH(false),
            fMCL(false),
            fSB(false)    
    {
    }

    KTClassifierResultsData::KTClassifierResultsData(const KTClassifierResultsData& orig) :
            KTExtensibleData< KTClassifierResultsData >(orig),            
            fMCH(orig.fMCH),
            fMCL(orig.fMCL),
            fSB(orig.fSB)
    {
    }

    KTClassifierResultsData::~KTClassifierResultsData()
    {
    }

    KTClassifierResultsData& KTClassifierResultsData::operator=(const KTClassifierResultsData& rhs)
    {
        KTExtensibleData< KTClassifierResultsData >::operator=(rhs);
        fMCH = rhs.fMCH;
        fMCL = rhs.fMCL;
        fSB = rhs.fSB;
        return *this;
    }

} /* namespace Katydid */
