/*
 * KTFilenameParsers.cc
 *
 *  Created on: May 31, 2013
 *      Author: nsoblath
 */

#include "KTFilenameParsers.hh"

#include "KTLogger.hh"

#include <boost/property_tree/ptree.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

// I can't just use boost::spirit::qi because of naming conflicts with std
using boost::spirit::qi::int_;
using boost::spirit::qi::double_;
// I had to take this out because of a naming conflict with boost::bind
//using boost::spirit::qi::_1;
using boost::spirit::qi::phrase_parse;
using boost::spirit::ascii::space;
using boost::phoenix::ref;

using std::string;

namespace Katydid
{
    KTLOGGER(fnplog, "KTFilenameParsers");


    KTLocustMCFilename::KTLocustMCFilename(const string& filename)
    {
        bool parsed = phrase_parse(filename.begin(), filename.end(),
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


}
