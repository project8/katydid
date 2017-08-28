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

#include "param_codec.hh"
#include "param_json.hh"
#include "param.hh"
#include "KTConfigurator.hh"

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
            fTransformFlag("ESTIMATE"),
            fTransformFlagMap(),
            fComplexToRealPlan(),
            fRealToComplexPlan(),
            fC2CForwardPlan(),
            fC2CReversePlan(),
            fInputArrayReal(NULL),
            fOutputArrayReal(NULL),
            fInputArrayComplex(NULL),
            fOutputArrayComplex(NULL),
            fTransformFlagUnsigned(0),
            fPSSignal("ps", this),
            fPSSlot("ps", this, &KTConvolution::Convolve1D_PS, &fPSSignal)
    {
        SetupInternalMaps();
    }

    KTConvolution::~KTConvolution()
    {
    }

    bool KTConvolution::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetKernel(node->get_value< std::string >("kernel", GetKernel()));
        SetBlockSize(node->get_value< unsigned >("block-size", GetBlockSize()));
        SetTransformFlag(node->get_value< std::string >("transform-flag", GetTransformFlag()));

        TransformFlagMap::const_iterator iter = fTransformFlagMap.find( fTransformFlag );
        fTransformFlagUnsigned = iter->second;

        if( ! ParseKernel() )
        {
            KTERROR( sdlog, "Failed to parse kernel json. Aborting" );
            return false;
        }

        AllocateArrays( GetBlockSize() );

        return true;
    }

    void KTConvolution::AllocateArrays( int nSize )
    {
        FreeArrays();

        fInputArrayReal = (double*) fftw_malloc( sizeof( double ) * nSize );
        fOutputArrayReal = (double*) fftw_malloc( sizeof( double ) * nSize );
        fInputArrayComplex = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSize );
        fOutputArrayComplex = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSize );

        fRealToComplexPlan = fftw_plan_dft_r2c_1d( nSize, fInputArrayReal, fOutputArrayComplex, fTransformFlagUnsigned );
        fComplexToRealPlan = fftw_plan_dft_c2r_1d( nSize, fInputArrayComplex, fOutputArrayReal, fTransformFlagUnsigned );
        // fC2CForwardPlan
        // fC2CReversePlan
    }

    void KTConvolution::FreeArrays()
    {
        fftw_free( fInputArrayReal );
        fftw_free( fOutputArrayReal );
        fftw_free( fInputArrayComplex );
        fftw_free( fOutputArrayComplex );

        fftw_destroy_plan( fRealToComplexPlan );
        fftw_destroy_plan( fComplexToRealPlan );
        fftw_destroy_plan( fC2CForwardPlan );
        fftw_destroy_plan( fC2CReversePlan );
    }

    bool KTConvolution::ParseKernel()
    {
        // Read in json with scarab::param

        scarab::path kernelFilePath = scarab::expand_path( GetKernel() );
        scarab::param_translator translator;
        scarab::param* kernelFromFile = translator.read_file( kernelFilePath.native() );
        scarab::param_node& kernelNode = kernelFromFile->as_node();

        if( ! kernelNode.has( "kernel" ) )
        {
            KTERROR( sdlog, "Kernel configuration file is not properly written. Aboring" );
            return false;
        }

        scarab::param_array& kernel1DArray = kernelNode["kernel"].as_array();
        int kernelSize = kernel1DArray.size();

        for( int iValue = 0; iValue < kernelSize; ++iValue )
        {
            kernelX.push_back( kernel1DArray.get_value< double >(iValue) );
        }

        // Set block size if left unspecified
        if( GetBlockSize() == 0 )
        {
            SetBlockSize( 8 * kernelX.size() );
            int power = log2( GetBlockSize() ); // int will take the floor of the log
            SetBlockSize( pow( 2, power ) );    // largest power of 2 which is <= 8 * kernel size
        }

        // Check that kernel size is not more than block size
        if( GetBlockSize() < kernelX.size() )
        {
            KTERROR( sdlog, "Block size is smaller than kernel length. Aborting." );
            return false;
        }

        // Periodically continue kernel up to block size
        for( int iPosition = kernelSize; iPosition < GetBlockSize(); ++iPosition )
        {
            kernelX.push_back( kernelX[iPosition - kernelSize] );
        }

        return true;
    }

    bool KTConvolution::Convolve1D_PS( KTPowerSpectrumData& data )
    {
        KTConvolvedPowerSpectrumData& newData = data.Of< KTConvolvedPowerSpectrumData >();

        int block = GetBlockSize();
        int overlap = kernelX.size() - 1;
        int step = block - overlap;

        std::vector< double > inputPiece;   // length <= block
        std::vector< double > outputPiece;  // length <= step

        fftw_complex *transformedInput;
        fftw_complex *transformedOutput;

        int blockNumber = 0;
        int nBins;
        unsigned nBin = 0;
        
        fftw_complex *transformedKernelX = DFT_1D_R2C( kernelX, block );

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
                transformedInput = DFT_1D_R2C( inputPiece, nBin );

                for( nBin = 0; nBin < block && nBin + blockNumber * step < nBins; ++nBin )
                {
                    transformedOutput[nBin][0] = transformedInput[nBin][0] * transformedKernelX[nBin][0] - transformedInput[nBin][1] * transformedKernelX[nBin][1];
                    transformedOutput[nBin][1] = transformedInput[nBin][0] * transformedKernelX[nBin][1] + transformedInput[nBin][1] * transformedKernelX[nBin][0];
                }

                // Reverse FFT of output block
                outputPiece = RDFT_1D_C2R( transformedOutput, nBin );
                
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

    fftw_complex* KTConvolution::DFT_1D_R2C( std::vector< double > in, int n )
    {
        if( in.size() != n )
        {
            KTWARN(sdlog, "Input array size does not match expected value. Re-allocating arrays.");
            AllocateArrays( n );
        }

        fInputArrayReal = &in[0];
        fftw_execute( fRealToComplexPlan );

        return fOutputArrayComplex;
    }

    std::vector< double > KTConvolution::RDFT_1D_C2R( fftw_complex *input, int n )
    {
        if( sizeof( input ) / sizeof( input[0] ) != n )
        {
            KTWARN(sdlog, "Input array size does not match expected value. Re-allocating arrays.");
            AllocateArrays( n );
        }

        fInputArrayComplex = input;
        fftw_execute( fComplexToRealPlan );
        std::vector< double > out( fOutputArrayReal, fOutputArrayReal + sizeof( fOutputArrayReal ) / sizeof( fOutputArrayReal[0] ) );

        return out;
    }

    void KTConvolution::SetupInternalMaps()
    {
        // transform flag map
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }

    void KTConvolution::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(sdlog, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }

        SetTransformFlag( flag );

        return;
    }


} /* namespace Katydid */
