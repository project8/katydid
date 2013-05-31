/*
 * KTFilenameParsers.hh
 *
 *  Created on: May 31, 2013
 *      Author: nsoblath
 */

#ifndef KTFILENAMEPARSERS_HH_
#define KTFILENAMEPARSES_HH_

#include "KTLogger.hh"

#include "Rtypes.h"

#include <boost/property_tree/ptree.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <string>

// I can't just use boost::spirit::qi because of naming conflicts with std
using boost::spirit::qi::int_;
using boost::spirit::qi::double_;
// I had to take this out because of a naming conflict with boost::bind
//using boost::spirit::qi::_1;
using boost::spirit::qi::phrase_parse;
using boost::spirit::ascii::space;
using boost::phoenix::ref;


namespace Katydid
{
    KTLOGGER(fnplog, "katydid.core");

    struct KTLocustMCFilename
    {
            UInt_t fNEvents;
            Double_t fEventLength; // sec
            Double_t fdfdt; // Hz/sec ?
            Double_t fSignalPower; // Watts
            KTLocustMCFilename(const std::string& filename)
            {
                Bool_t parsed = phrase_parse(filename.begin(), filename.end(),
                        (int_[ref(fNEvents)=boost::spirit::qi::_1] >> "events_" >>
                         double_[ref(fEventLength) = boost::spirit::qi::_1] >> "dur_" >>
                         double_[ref(fdfdt) = boost::spirit::qi::_1] >> "dfdt_" >>
                         double_[ref(fSignalPower) = boost::spirit::qi::_1] >> "power.egg"
                        ), space);
                if (! parsed)
                {
                    KTERROR(fnplog, "Filename was not parsed correctly: " << filename);
                }
            }
    };


} /* namespace Katydid */

#endif /* KTFILENAMEPARSERS_HH_*/
