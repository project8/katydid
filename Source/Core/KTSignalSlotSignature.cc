/*
 * KTSignalSlotSignature.cc
 *
 *  Created on: Sep 6, 2012
 *      Author: nsoblath
 */

#include "KTSignalSlotSignature.hh"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

namespace Katydid
{

    KTSignalSlotSignature::KTSignalSlotSignature() :
            fSignature(),
            fReturn(),
            fParameters()
    {
    }

    KTSignalSlotSignature::KTSignalSlotSignature(const std::string& signature) :
            fSignature(),
            fReturn(),
            fParameters()
    {
        if (! ParseSignature(signature))
        {
            throw std::invalid_argument("Signature <" + signature + "> did not parse correctly");
        }
        fSignature = signature;
    }

    KTSignalSlotSignature::~KTSignalSlotSignature()
    {
    }

    Bool_t KTSignalSlotSignature::ParseSignature(const std::string& signature)
    {
        /*
        using boost::spirit::qi::phrase_parse;
        using boost::spirit::qi::_1;
        using boost::spirit::ascii::space;
        using boost::spirit::str_;
        using boost::phoenix::push_back;



        Bool_t parsed = phrase_parse(signature.begin(), signature.end(),
                (string_[] >> '(' >> -(string_[]) >> *(',' >> string_[]) >> ')'),
                space
        );

        */
        return true;
    }


} /* namespace Katydid */
