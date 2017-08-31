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
            fRegularSize(0),
            fShortSize(0),
            fTransformFlagMap(),
            fComplexToRealPlan(),
            fRealToComplexPlan(),
            fC2CForwardPlan(),
            fC2CReversePlan(),
            fComplexToRealPlanShort(),
            fRealToComplexPlanShort(),
            fC2CForwardPlanShort(),
            fC2CReversePlanShort(),
            fInputArrayReal(NULL),
            fOutputArrayReal(NULL),
            fInputArrayComplex(NULL),
            fOutputArrayComplex(NULL),
            fInputArrayRealShort(NULL),
            fOutputArrayRealShort(NULL),
            fInputArrayComplexShort(NULL),
            fOutputArrayComplexShort(NULL),
            fTransformedInputArray(NULL),
            fTransformedOutputArray(NULL),
            fTransformedInputArrayShort(NULL),
            fTransformedOutputArrayShort(NULL),
            fTransformedKernelX(NULL),
            fTransformFlagUnsigned(0),
            fKernelSize(0),
            fInitialized(false),
            fPSSignal("ps", this),
            fPSSlot("ps", this, &KTConvolution::Convolve1D_PS, &fPSSignal)
    {
        SetupInternalMaps();
    }

    KTConvolution::~KTConvolution()
    {
        FreeArrays();
    }

    bool KTConvolution::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetKernel(node->get_value< std::string >("kernel", GetKernel()));
        SetBlockSize(node->get_value< unsigned >("block-size", GetBlockSize()));
        SetTransformFlag(node->get_value< std::string >("transform-flag", GetTransformFlag()));

        // Convert transform flag string to unsigned
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find( fTransformFlag );
        fTransformFlagUnsigned = iter->second;

        // Read in the kernel
        if( ! ParseKernel() )
        {
            KTERROR( sdlog, "Failed to parse kernel json. Aborting" );
            return false;
        }

        return true;
    }

    void KTConvolution::AllocateArrays( int nSizeRegular, int nSizeShort )
    {
        KTDEBUG(sdlog, "DFT initialization started");
        KTDEBUG(sdlog, "Regular size = " << nSizeRegular);
        KTDEBUG(sdlog, "Short size = " << nSizeShort);

        if( nSizeShort >= nSizeRegular )
        {
            KTWARN(sdlog, "Short size is not smaller than regular size; something weird happened. Aborting DFT initialization");
            return;
        }

        if( fInitialized )
        {
            KTDEBUG(sdlog, "Already initialized! Freeing arrays first");
            FreeArrays();
        }

        // Input/Output arrays
        fInputArrayReal = (double*) fftw_malloc( sizeof( double ) * nSizeRegular );
        fOutputArrayReal = (double*) fftw_malloc( sizeof( double ) * nSizeRegular );
        fInputArrayComplex = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegular );
        fOutputArrayComplex = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegular );

        // Intermediate (fourier space) arrays
        fTransformedInputArray = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegular );
        fTransformedOutputArray = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegular );

        // DFT plans
        fRealToComplexPlan = fftw_plan_dft_r2c_1d( nSizeRegular, fInputArrayReal, fTransformedInputArray, fTransformFlagUnsigned );
        fComplexToRealPlan = fftw_plan_dft_c2r_1d( nSizeRegular, fTransformedOutputArray, fOutputArrayReal, fTransformFlagUnsigned );
        // fC2CForwardPlan
        // fC2CReversePlan

        // All the same for short size

        fInputArrayRealShort = (double*) fftw_malloc( sizeof( double ) * nSizeShort );
        fOutputArrayRealShort = (double*) fftw_malloc( sizeof( double ) * nSizeShort );
        fInputArrayComplexShort = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeShort );
        fOutputArrayComplexShort = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeShort );

        fTransformedInputArrayShort = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeShort );
        fTransformedOutputArrayShort = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeShort );

        fRealToComplexPlanShort = fftw_plan_dft_r2c_1d( nSizeShort, fInputArrayRealShort, fTransformedInputArrayShort, fTransformFlagUnsigned );
        fComplexToRealPlanShort = fftw_plan_dft_c2r_1d( nSizeShort, fTransformedOutputArrayShort, fOutputArrayRealShort, fTransformFlagUnsigned );
        // fC2CForwardPlan
        // fC2CReversePlan

        // Store these guys
        fRegularSize = nSizeRegular;
        fShortSize = nSizeShort;

        // Make sure we don't come back here unless something weird happens
        fInitialized = true;

        return;
    }  

    void KTConvolution::FreeArrays()
    {
        if( fInputArrayReal != nullptr )
        {
            fftw_free( fInputArrayReal );
            fInputArrayReal = nullptr;
        }
        if( fOutputArrayReal != nullptr )
        {
            fftw_free( fOutputArrayReal );
            fOutputArrayReal = nullptr;
        }
        if( fInputArrayComplex != nullptr )
        {
            fftw_free( fInputArrayComplex );
            fInputArrayComplex = nullptr;
        }
        if( fOutputArrayComplex != nullptr )
        {
            fftw_free( fOutputArrayComplex );
            fOutputArrayComplex = nullptr;
        }

        if( fTransformedInputArray != nullptr )
        {
            fftw_free( fTransformedInputArray );
            fTransformedInputArray = nullptr;
        }

        if( fTransformedOutputArray != nullptr )
        {
            fftw_free( fTransformedOutputArray );
            fTransformedOutputArray = nullptr;
        }

        if( fRealToComplexPlan != nullptr )
        {
            fftw_destroy_plan( fRealToComplexPlan );
            fRealToComplexPlan = nullptr;
        }
        if( fComplexToRealPlan != nullptr )
        {
            fftw_destroy_plan( fComplexToRealPlan );
            fComplexToRealPlan = nullptr;
        }
        if( fC2CForwardPlan != nullptr )
        {
            fftw_destroy_plan( fC2CForwardPlan );
            fC2CForwardPlan = nullptr;
        }
        if( fC2CReversePlan != nullptr )
        {
            fftw_destroy_plan( fC2CReversePlan );
            fC2CReversePlan = nullptr;
        }

        if( fInputArrayRealShort != nullptr )
        {
            fftw_free( fInputArrayRealShort );
            fInputArrayRealShort = nullptr;
        }
        if( fOutputArrayRealShort != nullptr )
        {
            fftw_free( fOutputArrayRealShort );
            fOutputArrayRealShort = nullptr;
        }
        if( fInputArrayComplexShort != nullptr )
        {
            fftw_free( fInputArrayComplexShort );
            fInputArrayComplexShort = nullptr;
        }
        if( fOutputArrayComplexShort != nullptr )
        {
            fftw_free( fOutputArrayComplexShort );
            fOutputArrayComplexShort = nullptr;
        }

        if( fTransformedInputArrayShort != nullptr )
        {
            fftw_free( fTransformedInputArrayShort );
            fTransformedInputArrayShort = nullptr;
        }

        if( fTransformedOutputArrayShort != nullptr )
        {
            fftw_free( fTransformedOutputArrayShort );
            fTransformedOutputArrayShort = nullptr;
        }

        if( fRealToComplexPlanShort != nullptr )
        {
            fftw_destroy_plan( fRealToComplexPlanShort );
            fRealToComplexPlanShort = nullptr;
        }
        if( fComplexToRealPlanShort != nullptr )
        {
            fftw_destroy_plan( fComplexToRealPlanShort );
            fComplexToRealPlanShort = nullptr;
        }
        if( fC2CForwardPlanShort != nullptr )
        {
            fftw_destroy_plan( fC2CForwardPlanShort );
            fC2CForwardPlanShort = nullptr;
        }
        if( fC2CReversePlanShort != nullptr )
        {
            fftw_destroy_plan( fC2CReversePlanShort );
            fC2CReversePlanShort = nullptr;
        }

        fInitialized = false;

        return;
    }

    bool KTConvolution::ParseKernel()
    {
        KTINFO(sdlog, "Attempting to parse kernel");
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

        // Get kernel as array
        scarab::param_array& kernel1DArray = kernelNode["kernel"].as_array();
        KTDEBUG(sdlog, "Obtained kernel as array");

        // We need to store this because we're about to periodically extend it
        fKernelSize = kernel1DArray.size();
        KTDEBUG(sdlog, "Kernel size = " << fKernelSize);

        // Fill kernel vector
        for( int iValue = 0; iValue < fKernelSize; ++iValue )
        {
            kernelX.push_back( kernel1DArray.get_value< double >(iValue) );
        }

        // Here is where we set block size if left unspecified
        // We need it to periodically extend the kernel
        if( GetBlockSize() == 0 )
        {
            SetBlockSize( 8 * kernelX.size() );
            int power = log2( GetBlockSize() ); // int will take the floor of the log
            SetBlockSize( pow( 2, power ) );    // largest power of 2 which is <= 8 * kernel size
        }
        KTDEBUG(sdlog, "Set block size: " << GetBlockSize());

        // Check that kernel size is not more than block size
        if( GetBlockSize() < kernelX.size() )
        {
            KTERROR( sdlog, "Block size is smaller than kernel length. Aborting." );
            return false;
        }

        // Periodically extend kernel up to block size
        for( int iPosition = fKernelSize; iPosition < GetBlockSize(); ++iPosition )
        {
            kernelX.push_back( kernelX[iPosition - fKernelSize] );
        }

        KTINFO(sdlog, "Successfully parsed kernel!");
        return true;
    }

    bool KTConvolution::Convolve1D_PS( KTPowerSpectrumData& data )
    {
        KTINFO(sdlog, "Received power spectrum. Performing 1D convolution");
        // New data object
        KTConvolvedPowerSpectrumData& newData = data.Of< KTConvolvedPowerSpectrumData >();
        newData.SetNComponents( data.GetNComponents() );

        // Set overlap-and-save method parameters
        // These do not change, but I want to group them together like this so it's easy to follow
        int block = GetBlockSize();
        int overlap = fKernelSize - 1;
        int step = block - overlap;

        KTINFO(sdlog, "Block size: " << block);
        KTINFO(sdlog, "Overlap: " << overlap);
        KTINFO(sdlog, "Step size: " << step);

        // Block-loop parameters
        int blockNumber = 0;
        int nBinsTotal = data.GetSpectrum(0)->GetNFrequencyBins();
        unsigned nBin = 0;  // this will be the current bin in the loop

        KTINFO(sdlog, "nBinsTotal = " << nBinsTotal);

        // Now that nBinsTotal is determined, we can initialize the DFTs
        // The following conditional should only be true on the first slice
        if( ! fInitialized )
        {
            KTINFO(sdlog, "DFTs are not yet initialized; doing so now");

            // Slightly obnoxious conditionals to determine the sizes
            // Ordinary block is given, but the shorter block at the end is harder
            // Maybe there is a simpler way to determine this, I'm not sure

            if( nBinsTotal % step > overlap )
            {
                AllocateArrays( block, nBinsTotal % step );
            }
            else
            {
                AllocateArrays( block, (nBinsTotal % step) + step );
            }

            // Also transform the kernel, we only need to do this once
            KTDEBUG(sdlog, "Transforming kernel");

            fInputArrayReal = &kernelX[0];
            DFT_1D_R2C( block );
            fTransformedKernelX = fTransformedInputArray;
        }

        KTPowerSpectrum* ps;
        KTPowerSpectrum* transformedPS;

        // First loop over components
        for( unsigned iComponent = 0; iComponent < data.GetNComponents(); ++iComponent )
        {
            KTINFO(sdlog, "Starting component: " << iComponent);

            // Get power spectrum and initialize convolved spectrum for this component
            ps = data.GetSpectrum( iComponent );
            transformedPS = new KTPowerSpectrum( nBinsTotal, ps->GetRangeMin(), ps->GetRangeMax() );
            
            // Loop over block numbers
            while( blockNumber * step + block < nBinsTotal )
            {
                KTDEBUG(sdlog, "Block number: " << blockNumber);
                KTDEBUG(sdlog, "Starting position: " << blockNumber * step);
                KTDEBUG(sdlog, "nBinsTotal: " << nBinsTotal);

                // Fill input array
                for( nBin = 0; nBin < block; ++nBin )
                {
                    fInputArrayReal[nBin] = (*ps)(nBin + blockNumber * step);
                }

                // FFT of input block
                if( ! DFT_1D_R2C( block ) )
                {
                    return false;
                }

                // Bin multiplication in fourier space
                for( nBin = 0; nBin < block; ++nBin )
                {
                    fTransformedOutputArray[nBin][0] = fTransformedInputArray[nBin][0] * fTransformedKernelX[nBin][0] - fTransformedInputArray[nBin][1] * fTransformedKernelX[nBin][1];
                    fTransformedOutputArray[nBin][1] = fTransformedInputArray[nBin][0] * fTransformedKernelX[nBin][1] + fTransformedInputArray[nBin][1] * fTransformedKernelX[nBin][0];
                }

                // Reverse FFT of output block
                if( ! RDFT_1D_C2R( block ) )
                {
                    return false;
                }
                
                // Loop over bins in the output block and fill the convolved spectrum
                for( nBin = overlap; nBin < block; ++nBin )
                {
                    (*transformedPS)(nBin - overlap + blockNumber * step) = fOutputArrayReal[nBin];
                    KTDEBUG(sdlog, "Filled output bin: " << nBin - overlap + blockNumber * step);
                }

                // Increment block number
                ++blockNumber;
            }

            KTINFO(sdlog, "Reached final block");
            KTINFO(sdlog, "Starting position: " << blockNumber * step);

            // Same procedure as above, this time with a shorter final block

            for( nBin = 0; nBin + blockNumber * step < nBinsTotal; ++nBin )
            {
                fInputArrayRealShort[nBin] = (*ps)(nBin + blockNumber * step);
            }

            KTINFO(sdlog, "Short array length = " << nBin);
            KTDEBUG(sdlog, "Initialized short array length = " << fShortSize);

            // FFT of input block
            if( ! DFT_1D_R2C( nBin ) )
            {
                return false;
            }

            for( nBin = 0; nBin < nBin + blockNumber * step < nBinsTotal; ++nBin )
            {
                fTransformedOutputArrayShort[nBin][0] = fTransformedInputArrayShort[nBin][0] * fTransformedKernelX[nBin][0] - fTransformedInputArrayShort[nBin][1] * fTransformedKernelX[nBin][1];
                fTransformedOutputArrayShort[nBin][1] = fTransformedInputArrayShort[nBin][0] * fTransformedKernelX[nBin][1] + fTransformedInputArrayShort[nBin][1] * fTransformedKernelX[nBin][0];
            }

            // Reverse FFT of output block
            if( ! RDFT_1D_C2R( nBin ) )
            {
                return false;
            }
            
            // Loop over bins in the output block and fill the convolved spectrum
            for( nBin = overlap; nBin + blockNumber * step < nBinsTotal; ++nBin )
            {
                (*transformedPS)(nBin - overlap + blockNumber * step) = fOutputArrayReal[nBin];
                KTDEBUG(sdlog, "Filled output bin: " << nBin - overlap + blockNumber * step);
            }

            KTINFO(sdlog, "Component finished!");

            // Set power spectrum
            newData.SetSpectrum( transformedPS, iComponent );
            KTDEBUG(sdlog, "Filled new power spectrum");
        }

        return true;
    }

    bool KTConvolution::DFT_1D_R2C( int size )
    {
        if( ! fInitialized )
        {
            KTERROR(sdlog, "DFTs are not initialized! Aborting.");
            return false;
        }

        if( size == fRegularSize )
        {
            fftw_execute( fRealToComplexPlan );
        }
        else if( size == fShortSize )
        {
            fftw_execute( fRealToComplexPlanShort );
        }
        else
        {
            KTERROR(sdlog, "DFT size does not match either of those expected. Aborting.");
            return false;
        }

        return true;
    }

    bool KTConvolution::RDFT_1D_C2R( int size )
    {
        if( ! fInitialized )
        {
            KTERROR(sdlog, "DFTs are not initialized! Aborting.");
            return false;
        }

        if( size == fRegularSize )
        {
            fftw_execute( fComplexToRealPlan );
        }
        else if( size == fShortSize )
        {
            fftw_execute( fComplexToRealPlanShort );
        }
        else
        {
            KTERROR(sdlog, "DFT size does not match either of those expected. Aborting.");
            return false;
        }

        return true;
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
