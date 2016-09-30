/*
 * KTLines.hh
 *
 *  Created on: Sep 28, 2016
 *      Author: cclaessens
 */

#ifndef KTLINES_HH_
#define KTLINES_HH_

#include "KTData.hh"

#include <utility>
#include <vector>
#include <iostream>
namespace Katydid
{


    class KTLines
    {
        public:
            KTLines();
            virtual ~KTLines();

            std::vector<KTSeqLine>& GetLines();

            unsigned GetNLines();

            void AddLine(KTSeqLine& Line);
            void RemoveLine(unsigned& ID);

        private:
            std::vector< KTSeqLine > fLines;

    };

    inline void KTLines::AddLine(KTSeqLine& Line)
    {
        fLines.push_back(Line);
    }
    inline void KTLines::RemoveLine(unsigned& ID)
    {
    	for (unsigned iLines=0; iLines < this->GetNLines(); iLines++)
    	{
    		if (fLines[iLines].Identifier==ID)
    			fLines.erase (fLines.begin()+iLines);
    	}

    }
    inline std::vector<KTSeqLine>& KTLines::GetLines()
    {
    	return fLines;
    }


    inline unsigned KTLines::GetNLines()
    {
        return fLines.size();
    }




} /* namespace Katydid */

#endif /* KTDISCRIMINATEDPOINTS1DDATA_HH_ */
