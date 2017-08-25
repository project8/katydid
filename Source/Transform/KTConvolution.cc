/*
 * KTConvolution.cc
 *
 *  Created on: Aug 25, 2017
 *      Author: ezayas
 */

#include "KTConvolution.hh"

#include "KTConvolvedSpectrumData.hh"
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
        KTConvolvedPowerSpectrumData& newData = data.Of< KTConvolvedPowerSpectrumData >();

        int block = GetBlockSize();
        int overlap = kernelX.size() - 1;
        int step = block - overlap;

        std::vector< double > inputPiece;   // length <= block
        std::vector< double > outputPiece;  // length <= step

        std::vector< double > transformedInput;
        std::vector< double > transformedOutput;

        int blockNumber = 0;
        int nBins;
        unsigned nBin = 0;
        
        std::vector< double > transformedKernelX = DFT( kernelX );

        KTPowerSpectrum* ps;
        KTPowerSpectrum* transformedPS;

        // First loop over components
        for( unsigned iComponent = 0; iComponent < data.GetNComponents(); ++iComponent )
        {
            // Get power spectrum and initialize convolved spectrum for this component
            ps = data.GetSpectrum( iComponent );
            nBins = ps->GetNFrequencyBins();
            transformedPS = new KTPowerSpectrum( nBins, ps->GetRangeMin(), ps->GetRangeMax() );
            
            // Loop over block numbers
            while( blockNumber * step < nBins )
            {
                // The above conditional only guarantees that at least one bin is still needed before the end of the input
                // If we are near the end, we may not need a full block

                // Loop over bins in this block
                // The second conditional (after the &&) will return false if we reach the end of the input before the end of the block

                inputPiece.clear();
                for( nBin = 0; nBin < block && nBin + blockNumber * step < nBins; ++nBin )
                {
                    inputPiece.push_back( (*ps)(nBin + blockNumber * step) );
                }

                // FFT of input block
                transformedInput = DFT( inputPiece );

                transformedOutput.clear();
                for( nBin = 0; nBin < block && nBin + blockNumber * step < nBins; ++nBin )
                {
                    transformedOutput.push_back( transformedInput[nBin] * transformedKernelX[nBin] );
                }

                // Reverse FFT of output block
                outputPiece = RDFT( transformedOutput );
                
                // Loop over bins in the output block and fill the convolved spectrum
                for( nBin = overlap; nBin < block && nBin + blockNumber * step < nBins; ++nBin )
                {
                    (*transformedPS)(nBin + blockNumber * step) = outputPiece[nBin];
                }

                // Increment block number
                ++blockNumber;
            }

            // Set power spectrum
            newData.SetSpectrum( transformedPS, iComponent );
        }

        return true;
    }

    std::vector< double > KTConvolution::DFT( std::vector< double > in )
    {
        // Use FFTW
        return in;
    }

    std::vector< double > KTConvolution::RDFT( std::vector< double > in )
    {
        // Use FFTW
        return in;
    }


} /* namespace Katydid */
