/*
 * KTDPTReader.cc
 *
 *  Created on: Oct 13, 2016
 *      Author: obla999
 */

#include "KTDPTReader.hh"

namespace Katydid
{

    KTDPTReader::KTDPTReader(const std::string& name) :
            Nymph::KTReader(name),
            fFilename()
    {
    }

    KTDPTReader::~KTDPTReader()
    {
    }

    bool KTDPTReader::Configure(const scarab::param_node* node)
    {
        if (node == nullptr) return false;

        SetFilename(node->get_value("filename", GetFilename()));

        return true;
    }

    bool KTDPTReader::Run()
    {
        Nymph::KTDataPtr newData(new Nymph::KTData());
        return ReadFile(*newData.get());
    }

    bool KTDPTReader::ReadFile(Nymph::KTData& data)
    {
        // open file

        // get header data

        // create spectrum

        // loop over bins and fill spectrum
    }


} /* namespace Katydid */
