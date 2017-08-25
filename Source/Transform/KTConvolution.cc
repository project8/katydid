/*
 * KTConvolution.cc
 *
 *  Created on: Aug 25, 2017
 *      Author: ezayas
 */

#include "KTConvolution.hh"

#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTNormalizedFSData.hh"
#include "KTPowerSpectrumData.hh"

#include <vector>
#include <cmath>

using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(sdlog, "KTConvolution");

    KT_REGISTER_PROCESSOR(KTConvolution, "convolution");

    KTConvolution::KTConvolution(const std::string& name) :
            KTProcessor(name),
            fKernel("placeholder.json"),
            fBlockSize(0),
            fPSSignal("ps", this),
            fPSSlot("ps", this, &KTConvolution::Convolve1D, &fPSSignal)
    {
    }

    KTConvolution::~KTConvolution()
    {
    }

    bool KTConvolution::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("kernel"))
        {
            SetKernel(node->get_value< std::string >("kernel"));
        }
        if (node->has("block-size"))
        {
            SetBlockSize(node->get_value< unsigned >("block-size"));
        }
        
        if( ! ParseKernel() )
        {
            KTERROR( sdlog, "Failed to parse kernel json. Aborting" );
            return false;
        }

        if( GetBlockSize() == 0 )
        {
            SetBlockSize( 8 * kernelX.size() );
            int power = log2( GetBlockSize() ); // int will take the floor of the log
            SetBlockSize( pow( 2, power ) );    // largest power of 2 which is <= 8 * kernel size
        }

        if( GetBlockSize() < kernelX.size() )
        {
            KTERROR( sdlog, "Block size is smaller than kernel length. Aborting." );
            return false;
        }

        return true;
    }

    bool KTConvolution::ParseKernel()
    {
        // Read in json with scarab::param


        return true;
    }

    bool KTConvolution::Convolve1D( KTPowerSpectrumData& data )
    {
        // Do the thing

        return true;
    }
    

} /* namespace Katydid */
