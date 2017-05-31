/*
 * KTDPTReader.cc
 *
 *  Created on: Oct 13, 2016
 *      Author: obla999
 */

#include "KTDPTReader.hh"

#include "KTPowerSpectrumData.hh"

#include "KTLogger.hh"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

#include <cstdlib>

namespace Katydid
{
    KTLOGGER( inlog, "KTDPTReader" );

    KTDPTReader::KTDPTReader(const std::string& name) :
            Nymph::KTReader(name),
            fFilename(),
            fDPTSignal("ps", this)
    {
    }

    KTDPTReader::~KTDPTReader()
    {
    }

    bool KTDPTReader::Configure(const scarab::param_node* node)
    {
        if (node == nullptr) return false;

        Filename() = node->get_value("filename", Filename());

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
        rapidxml::file<> xmlFile(fFilename.c_str());
        rapidxml::xml_document<> dptDoc;

        try
        {
            dptDoc.parse<0>(xmlFile.data());
        }
        catch (rapidxml::parse_error& e)
        {
            KTERROR(inlog, "Caught exception while parsing DPT file:\n" <<
                    '\t' << e.what() << '\n' <<
                    '\t' << e.where<char>());
            return false;
        }

        /*
<RSAPersist version="1.12">
  <Application>RSA6100</Application>
  <Internal>
    <Composite pid="cells" collection="t">
      <Items>
        <Composite pid="dpsa_results" collection="t">
          <Items>
            <Waveform pid="traceAvg" name="Trace3">
              <TimeSpanStart>-1</TimeSpanStart>
              <TimeSpanLength>-1</TimeSpanLength>
              <InternalYUnits>W</InternalYUnits>
              <InternalXUnits>Hz</InternalXUnits>
              <YUnits>dBm</YUnits>
              <XUnits>Hz</XUnits>
              <MinMax>false</MinMax>
              <Count>801</Count>
              <XStart>557500000</XStart>
              <XStop>642500000</XStop>
              <y>-61.718456268310547</y>
              <y>-61.691680908203125</y>
              <y>-61.706001281738281</y>
              <y>-61.695640563964844</y>
         *
         */

        // get header data
        // hard-coded nesting of nodes
        typedef std::array< std::string, 7 > NodeSeqArray;
        NodeSeqArray nodeSequence = {"RSAPersist", "Internal", "Composite", "Items", "Composite", "Items", "Waveform"};
        NodeSeqArray::const_iterator nodeNameIt = nodeSequence.begin();
        rapidxml::xml_node< char >* parentNode = dptDoc.first_node(nodeNameIt->c_str());
        if (parentNode == nullptr)
        {
            KTERROR(inlog, "Did not find node <" << *nodeNameIt << ">; nesting level: " << nodeNameIt - nodeSequence.begin());
            return false;
        }
        for (++nodeNameIt; nodeNameIt != nodeSequence.end(); ++nodeNameIt)
        {
            parentNode = parentNode->first_node(nodeNameIt->c_str());
            if (parentNode == nullptr)
            {
                KTERROR(inlog, "Did not find node <" << *nodeNameIt << ">; nesting level: " << nodeNameIt - nodeSequence.begin());
                return false;
            }
        }

        // verify units
        rapidxml::xml_node< char >* xUnitsNode = parentNode->first_node("XUnits");
        if (xUnitsNode == nullptr || strcmp(xUnitsNode->value(), "Hz") != 0)
        {
            KTERROR(inlog, "Did not find x-axis units or units were not as expected (Hz)");
            return false;
        }

        rapidxml::xml_node< char >* yUnitsNode = parentNode->first_node("YUnits");
        if (yUnitsNode == nullptr || strcmp(yUnitsNode->value(), "dBm") != 0)
        {
            KTERROR(inlog, "Did not find y-axis units or units were not as expected (dBm)");
            return false;
        }

        // get spectrum information
        rapidxml::xml_node< char >* nBinsNode = parentNode->first_node("Count");
        if (nBinsNode == nullptr)
        {
            KTERROR(inlog, "Did not find <Count> node");
            return false;
        }
        unsigned nBins = strtoul(nBinsNode->value(), NULL, 10);

        rapidxml::xml_node< char >* minFreqNode = parentNode->first_node("XStart");
        if (nBinsNode == nullptr)
        {
            KTERROR(inlog, "Did not find <XStart> node");
            return false;
        }
        double minFreq = strtod(minFreqNode->value(), NULL);

        rapidxml::xml_node< char >* maxFreqNode = parentNode->first_node("XStop");
        if (nBinsNode == nullptr)
        {
            KTERROR(inlog, "Did not find <XStop> node");
            return false;
        }
        double maxFreq = strtod(maxFreqNode->value(), NULL);
        KTDEBUG(inlog, "DPT power spectrum will have " << nBins << " bins and frequency range " << minFreq << " - " << maxFreq << " Hz");

        // create spectrum
        KTPowerSpectrumData& psData = data.Of< KTPowerSpectrumData >().SetNComponents(1);
        KTPowerSpectrum* newSpectrum = new KTPowerSpectrum(nBins, minFreq, maxFreq);

        // loop over bins and fill spectrum
        rapidxml::xml_node< char >* powerNode = parentNode->first_node("y");
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            if (powerNode == nullptr)
            {
                KTERROR(inlog, "Did not find node for bin <" << iBin << ">");
                delete newSpectrum;
                return false;
            }
            // convert dBm to W: P_W = 10^((P_dBm-30)/10)
            (*newSpectrum)(iBin) = pow(10., 0.1 * (strtod(powerNode->value(), NULL) - 30.));
            powerNode = powerNode->next_sibling("y");
        }

        // add spectrum to data
        psData.SetSpectrum(newSpectrum);

        return true;
    }


} /* namespace Katydid */
