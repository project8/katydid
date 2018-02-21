/*
 * KTConvolution2D.cc
 *
 *  Created on: Oct 20, 2017
 *      Author: ezayas
 */

#include "KTConvolution2D.hh"

#include "KTConvolvedSpectrumData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTPowerSpectrum.hh"

#include "param_codec.hh"
#include "param_json.hh"
#include "param.hh"
#include "KTConfigurator.hh"

using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(sdlog, "KTConvolution2D");

    KT_REGISTER_PROCESSOR(KTConvolution2D, "convolution2D");

    KTConvolution2D::KTConvolution2D(const std::string& name) :
            KTProcessor(name),
            fKernel("placeholder.json"),
            fBlockSizeTime(0),
            fBlockSizeFreq(0),
            fNormalizeKernel(false),
            fTransformType("convolution"),
            fTransformFlag("ESTIMATE"),
            fRegularSizeTime(0),
            fRegularSizeFreq(0),
            fShortSizeTime(0),
            fShortSizeFreq(0),
            fTransformFlagMap(),
            fComplexToRealPlan(),
            fRealToComplexPlan(),
            fC2CForwardPlan(),
            fC2CReversePlan(),
            fComplexToRealPlanShortTime(),
            fRealToComplexPlanShortTime(),
            fC2CForwardPlanShortTime(),
            fC2CReversePlanShortTime(),
            fComplexToRealPlanShortFreq(),
            fRealToComplexPlanShortFreq(),
            fC2CForwardPlanShortFreq(),
            fC2CReversePlanShortFreq(),
            fComplexToRealPlanShortTimeShortFreq(),
            fRealToComplexPlanShortTimeShortFreq(),
            fC2CForwardPlanShortTimeShortFreq(),
            fC2CReversePlanShortTimeShortFreq(),
            fInputArrayReal(NULL),
            fOutputArrayReal(NULL),
            fInputArrayComplex(NULL),
            fOutputArrayComplex(NULL),
            fTransformedInputArray(NULL),
            fTransformedOutputArray(NULL),
            fTransformedInputArrayFromReal(NULL),
            fTransformedOutputArrayFromReal(NULL),
            fTransformedKernelXYAsReal(NULL),
            fTransformedKernelXYAsComplex(NULL),
            fGeneralTransformedInputArray(NULL),
            fGeneralTransformedOutputArray(NULL),
            fGeneralTransformedKernelArray(NULL),
            nBinLimitRegularTime(0),
            nBinLimitRegularFreq(0),
            nBinLimitShortTime(0),
            nBinLimitShortFreq(0),
            fGeneralForwardPlan(),
            fGeneralReversePlan(),
            fGeneralForwardPlanShortTime(),
            fGeneralReversePlanShortTime(),
            fGeneralForwardPlanShortFreq(),
            fGeneralReversePlanShortFreq(),
            fGeneralForwardPlanShortTimeShortFreq(),
            fGeneralReversePlanShortTimeShortFreq(),
            fTransformFlagUnsigned(FFTW_ESTIMATE),
            fKernelSizeX(0),
            fKernelSizeY(0),
            fInitialized(false),
            fMultiPSSignal("conv-multi-ps", this),
            fMultiFSFFTWSignal("conv-multi-fs-fftw", this),
            fMultiFSPolarSignal("conv-multi-fs-polar", this),
            fMultiPSSlot("multi-ps", this, &KTConvolution2D::Convolve2D, &fMultiPSSignal),
            fMultiFSFFTWSlot("multi-fs-fftw", this, &KTConvolution2D::Convolve2D, &fMultiFSFFTWSignal),
            fMultiFSPolarSlot("multi-fs-polar", this, &KTConvolution2D::Convolve2D, &fMultiFSPolarSignal)
    {
        SetupInternalMaps();
    }

    KTConvolution2D::~KTConvolution2D()
    {
        FreeArrays();
    }

    bool KTConvolution2D::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetKernel(node->get_value< std::string >("kernel", GetKernel()));
        SetBlockSizeTime(node->get_value< unsigned >("block-size-time", GetBlockSizeTime()));
        SetBlockSizeFreq(node->get_value< unsigned >("block-size-freq", GetBlockSizeFreq()));
        SetTransformType(node->get_value< std::string >("transform-type", GetTransformType()));
        SetTransformFlag(node->get_value< std::string >("transform-flag", GetTransformFlag()));

        return FinishSetup();
    }

    bool KTConvolution2D::FinishSetup()
    {
        // Find transform type
        if( GetTransformType() == "convolution" )
        {
            KTINFO(sdlog, "Transform type is ordinary convolution. Proceeding with kernel");
        }
        else if( GetTransformType() == "cross-correlation" )
        {
            KTINFO(sdlog, "Transform type is cross-correlation. Will conjugate and reverse input data first");
        }
        else
        {
            KTWARN(sdlog, "Transform type not recognized. Please make sure you've typed it correctly. Aborting");
            return false;
        }

        // Convert transform flag string to unsigned
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find( fTransformFlag );
        fTransformFlagUnsigned = iter->second;

        // Read in the kernel
        if( ! ParseKernel() )
        {
            KTERROR(sdlog, "Failed to parse kernel json. Aborting");
            return false;
        }

        return true;
    }

    void KTConvolution2D::AllocateArrays( int nSizeRegularX, int nSizeShortX, int nSizeRegularY, int nSizeShortY )
    {
        KTDEBUG(sdlog, "DFT initialization started");
        KTDEBUG(sdlog, "Regular size X = " << nSizeRegularX);
        KTDEBUG(sdlog, "Regular size Y = " << nSizeRegularY);
        KTDEBUG(sdlog, "Short size X = " << nSizeShortX);
        KTDEBUG(sdlog, "Short size Y = " << nSizeShortY);

        if( nSizeShortX > nSizeRegularX || nSizeShortY > nSizeRegularY )
        {
            KTWARN(sdlog, "Short size is larger than regular size; something weird happened. Aborting DFT initialization");
            return;
        }

        if( fInitialized )
        {
            KTDEBUG(sdlog, "Already initialized! Freeing arrays first");
            FreeArrays();
        }

        // Input/Output arrays
        fInputArrayReal = (double*) fftw_malloc( sizeof( double ) * nSizeRegularX * nSizeRegularY );
        fOutputArrayReal = (double*) fftw_malloc( sizeof( double ) * nSizeRegularX * nSizeRegularY );
        fInputArrayComplex = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * nSizeRegularY );
        fOutputArrayComplex = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * nSizeRegularY );

        // Intermediate (fourier space) arrays
        fTransformedInputArray = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * nSizeRegularY );
        fTransformedOutputArray = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * nSizeRegularY );
        fTransformedInputArrayFromReal = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * (nSizeRegularY/2 + 1) );
        fTransformedOutputArrayFromReal = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * (nSizeRegularY/2 + 1) );

        // DFT plans
        fRealToComplexPlan = fftw_plan_dft_r2c_2d( nSizeRegularX, nSizeRegularY, fInputArrayReal, fTransformedInputArrayFromReal, fTransformFlagUnsigned );
        fComplexToRealPlan = fftw_plan_dft_c2r_2d( nSizeRegularX, nSizeRegularY, fTransformedOutputArrayFromReal, fOutputArrayReal, fTransformFlagUnsigned );
        fC2CForwardPlan = fftw_plan_dft_2d( nSizeRegularX, nSizeRegularY, fInputArrayComplex, fTransformedInputArray, FFTW_FORWARD, fTransformFlagUnsigned );
        fC2CReversePlan = fftw_plan_dft_2d( nSizeRegularX, nSizeRegularY, fTransformedOutputArray, fOutputArrayComplex, FFTW_BACKWARD, fTransformFlagUnsigned );

        fRealToComplexPlanShortTime = fftw_plan_dft_r2c_2d( nSizeShortX, nSizeRegularY, fInputArrayReal, fTransformedInputArrayFromReal, fTransformFlagUnsigned );
        fComplexToRealPlanShortTime = fftw_plan_dft_c2r_2d( nSizeShortX, nSizeRegularY, fTransformedOutputArrayFromReal, fOutputArrayReal, fTransformFlagUnsigned );
        fC2CForwardPlanShortTime = fftw_plan_dft_2d( nSizeShortX, nSizeRegularY, fInputArrayComplex, fTransformedInputArray, FFTW_FORWARD, fTransformFlagUnsigned );
        fC2CReversePlanShortTime = fftw_plan_dft_2d( nSizeShortX, nSizeRegularY, fTransformedOutputArray, fOutputArrayComplex, FFTW_BACKWARD, fTransformFlagUnsigned );

        fRealToComplexPlanShortFreq = fftw_plan_dft_r2c_2d( nSizeRegularX, nSizeShortY, fInputArrayReal, fTransformedInputArrayFromReal, fTransformFlagUnsigned );
        fComplexToRealPlanShortFreq = fftw_plan_dft_c2r_2d( nSizeRegularX, nSizeShortY, fTransformedOutputArrayFromReal, fOutputArrayReal, fTransformFlagUnsigned );
        fC2CForwardPlanShortFreq = fftw_plan_dft_2d( nSizeRegularX, nSizeShortY, fInputArrayComplex, fTransformedInputArray, FFTW_FORWARD, fTransformFlagUnsigned );
        fC2CReversePlanShortFreq = fftw_plan_dft_2d( nSizeRegularX, nSizeShortY, fTransformedOutputArray, fOutputArrayComplex, FFTW_BACKWARD, fTransformFlagUnsigned );

        fRealToComplexPlanShortTimeShortFreq = fftw_plan_dft_r2c_2d( nSizeShortX, nSizeShortY, fInputArrayReal, fTransformedInputArrayFromReal, fTransformFlagUnsigned );
        fComplexToRealPlanShortTimeShortFreq = fftw_plan_dft_c2r_2d( nSizeShortX, nSizeShortY, fTransformedOutputArrayFromReal, fOutputArrayReal, fTransformFlagUnsigned );
        fC2CForwardPlanShortTimeShortFreq = fftw_plan_dft_2d( nSizeShortX, nSizeShortY, fInputArrayComplex, fTransformedInputArray, FFTW_FORWARD, fTransformFlagUnsigned );
        fC2CReversePlanShortTimeShortFreq = fftw_plan_dft_2d( nSizeShortX, nSizeShortY, fTransformedOutputArray, fOutputArrayComplex, FFTW_BACKWARD, fTransformFlagUnsigned );

        // Store these guys
        fRegularSizeTime = nSizeRegularX;
        fRegularSizeFreq = nSizeRegularY;
        fShortSizeTime = nSizeShortX;
        fShortSizeFreq = nSizeShortY;

        // Make sure we don't come back here unless something weird happens
        fInitialized = true;

        return;
    }  

    void KTConvolution2D::FreeArrays()
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

        if( fRealToComplexPlanShortTime != nullptr )
        {
            fftw_destroy_plan( fRealToComplexPlanShortTime );
            fRealToComplexPlanShortTime = nullptr;
        }
        if( fComplexToRealPlanShortTime != nullptr )
        {
            fftw_destroy_plan( fComplexToRealPlanShortTime );
            fComplexToRealPlanShortTime = nullptr;
        }
        if( fC2CForwardPlanShortTime != nullptr )
        {
            fftw_destroy_plan( fC2CForwardPlanShortTime );
            fC2CForwardPlanShortTime = nullptr;
        }
        if( fC2CReversePlanShortTime != nullptr )
        {
            fftw_destroy_plan( fC2CReversePlanShortTime );
            fC2CReversePlanShortTime = nullptr;
        }

        if( fRealToComplexPlanShortFreq != nullptr )
        {
            fftw_destroy_plan( fRealToComplexPlanShortFreq );
            fRealToComplexPlanShortFreq = nullptr;
        }
        if( fComplexToRealPlanShortFreq != nullptr )
        {
            fftw_destroy_plan( fComplexToRealPlanShortFreq );
            fComplexToRealPlanShortFreq = nullptr;
        }
        if( fC2CForwardPlanShortFreq != nullptr )
        {
            fftw_destroy_plan( fC2CForwardPlanShortFreq );
            fC2CForwardPlanShortFreq = nullptr;
        }
        if( fC2CReversePlanShortFreq != nullptr )
        {
            fftw_destroy_plan( fC2CReversePlanShortFreq );
            fC2CReversePlanShortFreq = nullptr;
        }

        if( fRealToComplexPlanShortTimeShortFreq != nullptr )
        {
            fftw_destroy_plan( fRealToComplexPlanShortTimeShortFreq );
            fRealToComplexPlanShortTimeShortFreq = nullptr;
        }
        if( fComplexToRealPlanShortTimeShortFreq != nullptr )
        {
            fftw_destroy_plan( fComplexToRealPlanShortTimeShortFreq );
            fComplexToRealPlanShortTimeShortFreq = nullptr;
        }
        if( fC2CForwardPlanShortTimeShortFreq != nullptr )
        {
            fftw_destroy_plan( fC2CForwardPlanShortTimeShortFreq );
            fC2CForwardPlanShortTimeShortFreq = nullptr;
        }
        if( fC2CReversePlanShortTimeShortFreq != nullptr )
        {
            fftw_destroy_plan( fC2CReversePlanShortTimeShortFreq );
            fC2CReversePlanShortTimeShortFreq = nullptr;
        }

        fInitialized = false;

        return;
    }

    bool KTConvolution2D::ParseKernel()
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
        scarab::param_array& kernel2DArray = kernelNode["kernel"].as_array();
        KTDEBUG(sdlog, "Obtained kernel as array");

        // We need to store this because we're about to periodically extend it
        fKernelSizeY = kernel2DArray.size();
        fKernelSizeX = sizeof( kernel2DArray.get_value< double* >(0) ) / sizeof( double );
        KTDEBUG(sdlog, "Kernel size X = " << fKernelSizeX);
        KTDEBUG(sdlog, "Kernel size Y = " << fKernelSizeY);

        // Fill kernel vector
        // Also calculate the norm in case we need that

        double norm = 0.;
        std::vector< double > kernelRow;
        for( int iValue = 0; iValue < fKernelSizeY; ++iValue )
        {
            kernelRow.clear();
            for( int jValue = 0; jValue < fKernelSizeX; ++jValue )
            {
                kernelRow.push_back( (kernel2DArray[iValue]).as_array().get_value< double >(jValue) );
                norm += kernelRow[jValue];
            }
            
            kernelXY.push_back( kernelRow );
        }
        KTDEBUG(sdlog, "Kernel norm = " << norm);

        // Here is where we set block size if left unspecified
        // We need it to periodically extend the kernel
        if( GetBlockSizeTime() == 0 )
        {
            SetBlockSizeTime( 8 * fKernelSizeX );
            int power = log2( GetBlockSizeTime() ); // int will take the floor of the log
            SetBlockSizeTime( pow( 2, power ) );    // largest power of 2 which is <= 8 * kernel size
        }
        KTDEBUG(sdlog, "Set time block size: " << GetBlockSizeTime());

        // Check that kernel size is not more than block size
        if( GetBlockSizeTime() < fKernelSizeX )
        {
            KTERROR( sdlog, "Block size is smaller than kernel length. Aborting." );
            return false;
        }

        if( GetBlockSizeFreq() == 0 )
        {
            SetBlockSizeFreq( 8 * fKernelSizeY );
            int power = log2( GetBlockSizeFreq() ); // int will take the floor of the log
            SetBlockSizeFreq( pow( 2, power ) );    // largest power of 2 which is <= 8 * kernel size
        }
        KTDEBUG(sdlog, "Set freq block size: " << GetBlockSizeFreq());

        // Check that kernel size is not more than block size
        if( GetBlockSizeFreq() < fKernelSizeY )
        {
            KTERROR( sdlog, "Block size is smaller than kernel length. Aborting." );
            return false;
        }

        // Periodically extend kernel up to block size
        for( int iPosition = 0; iPosition < GetBlockSizeTime(); ++iPosition )
        {
            if( iPosition < fKernelSizeX )
            {
                for( int jPosition = fKernelSizeY; jPosition < GetBlockSizeFreq(); ++jPosition )
                {
                    kernelXY[iPosition].push_back( 0.0 );
                }
            }
            else
            {
                kernelRow.clear();
                for( int jPosition = 0; jPosition < GetBlockSizeFreq(); ++jPosition )
                {
                    kernelRow.push_back( 0.0 );
                }

                kernelXY.push_back( kernelRow );
            }
        }

        if( GetNormalizeKernel() )
        {
            KTDEBUG(sdlog, "Normalizing kernel");
            for( int iPosition = 0; iPosition < GetBlockSizeTime(); ++iPosition )
            {
                for( int jPosition = 0; jPosition < GetBlockSizeFreq(); ++jPosition )
                {
                    kernelXY[iPosition][jPosition] = kernelXY[iPosition][jPosition] / norm;
                }
            }
        }

        KTINFO(sdlog, "Successfully parsed kernel!");
        return true;
    }

    bool KTConvolution2D::Convolve2D( KTMultiPSData& data )
    {
        KTINFO(sdlog, "Received multi power spectrum. Performing 2D convolution");
        // New data object
        KTConvolvedMultiPSData& newData = data.Of< KTConvolvedMultiPSData >();
        
        return CoreConvolve2D( static_cast< KTMultiPSDataCore& >(data), newData );
    }

    bool KTConvolution2D::Convolve2D( KTMultiFSDataFFTW& data )
    {
        KTINFO(sdlog, "Received multi frequency spectrum. Performing 2D convolution");
        // New data object
        KTConvolvedMultiFSDataFFTW& newData = data.Of< KTConvolvedMultiFSDataFFTW >();
        
        return CoreConvolve2D( static_cast< KTMultiFSDataFFTWCore& >(data), newData );
    }

    bool KTConvolution2D::Convolve2D( KTMultiFSDataPolar& data )
    {
        KTINFO(sdlog, "Received multi frequency spectrum. Performing 1D convolution");
        // New data object
        KTConvolvedMultiFSDataPolar& newData = data.Of< KTConvolvedMultiFSDataPolar >();
        
        return CoreConvolve2D( static_cast< KTMultiFSDataPolarCore& >(data), newData );
    }

    template< class XMultiSpectrumDataCore, class XConvolvedMultiSpectrumTypeData >
    bool KTConvolution2D::CoreConvolve2D( XMultiSpectrumDataCore& data, XConvolvedMultiSpectrumTypeData& newData )
    {
        newData.SetNComponents( data.GetNComponents() );

        // Set overlap-and-save method parameters
        // These do not change, but I want to group them together like this so it's easy to follow
        int blockX = GetBlockSizeTime();
        int blockY = GetBlockSizeFreq();
        int overlapX = fKernelSizeX - 1;
        int overlapY = fKernelSizeY - 1;
        int stepX = blockX - overlapX;
        int stepY = blockY - overlapY;

        KTINFO(sdlog, "Block size: (" << blockX << ", " << blockY << ")");
        KTINFO(sdlog, "Overlap: (" << overlapX << ", " << overlapY << ")");
        KTINFO(sdlog, "Step size: (" << stepX << ", " << stepY << ")");

        int nBinsTotalX = GetSpectra( data, 0 )->size();
        int nBinsTotalY = (*GetSpectra( data, 0 ))(0)->GetNFrequencyBins();
        KTINFO(sdlog, "nBinsTotal = (" << nBinsTotalX << ", " << nBinsTotalY << ")");

        // Now that nBinsTotal is determined, we can initialize the DFTs
        // The following conditional should only be true on the first slice
        if( ! fInitialized )
        {
            Initialize( nBinsTotalX, nBinsTotalY, blockX, blockY, stepX, stepY, overlapX, overlapY );
        }

        // First loop over components
        for( unsigned iComponent = 0; iComponent < data.GetNComponents(); ++iComponent )
        {
            KTINFO(sdlog, "Starting component: " << iComponent);

            // Get power spectrum and initialize convolved spectrum for this component
            KTPhysicalArray< 1, typename XMultiSpectrumDataCore::spectrum_type* >* transformedSpectra = DoConvolution( GetSpectra( data, iComponent ), blockX, blockY, stepX, stepY, overlapX, overlapY );

            if( transformedSpectra == nullptr )
            {
                KTERROR( sdlog, "Convolution was unsuccessful. Aborting." );
                return false;
            }

            // Set power spectrum
            newData.SetSpectra( transformedSpectra, iComponent );
            KTDEBUG(sdlog, "Filled new spectra");
        }

        KTINFO(sdlog, "All components finished successfully!");

        return true;
    }

    template< class XSpectraType >
    KTPhysicalArray< 1, XSpectraType* >* KTConvolution2D::DoConvolution( const KTPhysicalArray< 1, XSpectraType* >* myInitialSpectrum, const int blockX, const int blockY, const int stepX, const int stepY, const int overlapX, const int overlapY )
    {
        // Block loop parameters
        int nBinX = 0;
        int nBinY = 0;
        int nBinsTotalX = myInitialSpectrum->size();
        int nBinsTotalY = (*myInitialSpectrum)(0)->GetNFrequencyBins();
        int blockNumberX = 0;
        int blockNumberY = 0;
        int positionX = 0;
        int positionY = 0;

        // non-const version
        KTPhysicalArray< 1, XSpectraType* >* initialSpectrum = new KTPhysicalArray< 1, XSpectraType* >( *myInitialSpectrum );

        // If we're doing cross-correlation, first we need to conjugate and reverse the input spectrum
        if( fTransformType == "cross-correlation" )
        {
            ConjugateAndReverse( *initialSpectrum );
        }
        
        KTPhysicalArray< 1, XSpectraType* >* transformedSpectrum = new KTPhysicalArray< 1, XSpectraType* >( nBinsTotalX, initialSpectrum->GetRangeMin(), initialSpectrum->GetRangeMax() );

        if( ! SetUpGeneralVars< XSpectraType* >() )
        {
            KTERROR(sdlog, "Spectrum type unknown. Returning blank spectrum");
            return transformedSpectrum;
        }

        // Loop over block numbers
        while( (blockNumberX+1) * stepX <= nBinsTotalX )
        {
            blockNumberY = 0;
            positionY = 0;

            while( (blockNumberY+1) * stepY <= nBinsTotalY )
            {
                KTDEBUG(sdlog, "Block number: (" << blockNumberX << ", " << blockNumberY << ")");
                KTDEBUG(sdlog, "Starting position: (" << blockNumberX * stepX - overlapX << ", " << blockNumberY * stepY - overlapY << ")");
                KTDEBUG(sdlog, "nBinsTotal: (" << nBinsTotalX << ", " << nBinsTotalY << ")");

                // Fill input array
                for( nBinX = 0; nBinX < blockX; ++nBinX )
                {
                    positionX = nBinX + blockNumberX * stepX - overlapX;
                    for( nBinY = 0; nBinY < blockY; ++nBinY )
                    {
                        positionY = nBinY + blockNumberY * stepY - overlapY;
                        SetInputArray( positionX, positionY, nBinX * blockY + nBinY, initialSpectrum );
                    }
                }

                // FFT of input block
                KTDEBUG(sdlog, "Performing DFT");
                fftw_execute( fGeneralForwardPlan );

                // Bin multiplication in fourier space
                KTDEBUG(sdlog, "Multiplying arrays in fourier space");

                for( int nBinX = 0; nBinX < nBinLimitRegularTime; ++nBinX )
                {
                    for( int nBinY = 0; nBinY < nBinLimitRegularFreq; ++nBinY )
                    {
                        fGeneralTransformedOutputArray[nBinX * nBinLimitRegularFreq + nBinY][0] = fGeneralTransformedInputArray[nBinX * nBinLimitRegularFreq + nBinY][0] * fGeneralTransformedKernelArray[nBinX * nBinLimitRegularFreq + nBinY][0] - fGeneralTransformedInputArray[nBinX * nBinLimitRegularFreq + nBinY][1] * fGeneralTransformedKernelArray[nBinX * nBinLimitRegularFreq + nBinY][1];
                        fGeneralTransformedOutputArray[nBinX * nBinLimitRegularFreq + nBinY][1] = fGeneralTransformedInputArray[nBinX * nBinLimitRegularFreq + nBinY][0] * fGeneralTransformedKernelArray[nBinX * nBinLimitRegularFreq + nBinY][1] + fGeneralTransformedInputArray[nBinX * nBinLimitRegularFreq + nBinY][1] * fGeneralTransformedKernelArray[nBinX * nBinLimitRegularFreq + nBinY][0];
                    }
                }

                // Reverse FFT of output block
                KTDEBUG(sdlog, "Performing reverse DFT");
                fftw_execute( fGeneralReversePlan );
                
                // Loop over bins in the output block and fill the convolved spectrum
                for( nBinX = overlapX; nBinX < blockX; ++nBinX )
                {
                    for( nBinY = overlapY; nBinY < blockY; ++nBinY )
                    {
                        SetOutputArray( nBinX - overlapX + blockNumberX * stepX, nBinY - overlapY + blockNumberY * stepY, nBinX * blockY + nBinY, *transformedSpectrum, blockX, blockY );
                    }
                }

                // Increment block number
                ++blockNumberY;
            }

            if( blockNumberY * stepY == nBinsTotalY )
            {
                KTINFO(sdlog, "Reached end of input data");

                ++blockNumberX;
                continue;
            }

            KTINFO(sdlog, "Reached final block");
            KTINFO(sdlog, "Starting position: " << blockNumberY * stepY - overlapY);

            // Same procedure as above, this time with a shorter final block

            for( nBinX = 0; nBinX < blockX; ++nBinX )
            {
                positionX = nBinX + blockNumberX * stepX - overlapX; 
                for( nBinY = 0; positionY+1 < nBinsTotalY; ++nBinY )
                {
                    positionY = nBinY + blockNumberY * stepY - overlapY;
                    SetInputArray( positionX, positionY, nBinX * fShortSizeFreq + nBinY, initialSpectrum );
                }
            }

            KTINFO(sdlog, "Short array length = " << nBinY);
            KTDEBUG(sdlog, "Initialized short array length = " << fShortSizeFreq);

            // FFT of input block
            fftw_execute( fGeneralForwardPlanShortFreq );

            for( int nBinX = 0; nBinX < nBinLimitRegularTime; ++nBinX )
            {
                for( int nBinY = 0; nBinY < nBinLimitShortFreq; ++nBinY )
                {
                    fGeneralTransformedOutputArray[nBinX * nBinLimitShortFreq + nBinY][0] = fGeneralTransformedInputArray[nBinX * nBinLimitShortFreq + nBinY][0] * fGeneralTransformedKernelArray[nBinX * nBinLimitShortFreq + nBinY][0] - fGeneralTransformedInputArray[nBinX * nBinLimitShortFreq + nBinY][1] * fGeneralTransformedKernelArray[nBinX * nBinLimitShortFreq + nBinY][1];
                    fGeneralTransformedOutputArray[nBinX * nBinLimitShortFreq + nBinY][1] = fGeneralTransformedInputArray[nBinX * nBinLimitShortFreq + nBinY][0] * fGeneralTransformedKernelArray[nBinX * nBinLimitShortFreq + nBinY][1] + fGeneralTransformedInputArray[nBinX * nBinLimitShortFreq + nBinY][1] * fGeneralTransformedKernelArray[nBinX * nBinLimitShortFreq + nBinY][0];
                }
            }

            // Reverse FFT of output block
            fftw_execute( fGeneralReversePlanShortFreq );
            
            // Loop over bins in the output block and fill the convolved spectrum
            for( nBinX = overlapX; nBinX < blockX; ++nBinX )
            {
                for( nBinY = overlapY; nBinY < fShortSizeFreq; ++nBinY )
                {
                    SetOutputArray( nBinX - overlapX + blockNumberX * stepX, nBinY - overlapY + blockNumberY * stepY, nBinX * fShortSizeFreq + nBinY, *transformedSpectrum, fShortSizeTime, fShortSizeFreq );
                }
            }

            ++blockNumberX;
        }

        if( blockNumberX * stepX == nBinsTotalX )
        {
            KTINFO(sdlog, "Reached end of input data");
            return transformedSpectrum;
        }

        KTINFO(sdlog, "Reached final block");
        KTINFO(sdlog, "Starting position: " << blockNumberX * stepX - overlapX);

        // Same procedure as above, this time with a shorter final block

        blockNumberY = 0;
        positionY = 0;

        while( (blockNumberY+1) * stepY <= nBinsTotalY )
        {
            for( nBinX = 0; positionX+1 < nBinsTotalX; ++nBinX )
            {
                positionX = nBinX + blockNumberX * stepX - overlapX;
                for( nBinY = 0; nBinY < blockY; ++nBinY )
                {
                    positionY = nBinY + blockNumberY * stepY - overlapY;
                    SetInputArray( positionX, positionY, nBinX * blockY + nBinY, initialSpectrum );
                }
            }

            KTINFO(sdlog, "Short array length = " << nBinX);
            KTDEBUG(sdlog, "Initialized short array length = " << fShortSizeTime);

            // FFT of input block
            fftw_execute( fGeneralForwardPlanShortTime );

            for( int nBinX = 0; nBinX < nBinLimitShortTime; ++nBinX )
            {
                for( int nBinY = 0; nBinY < nBinLimitRegularFreq; ++nBinY )
                {
                    fGeneralTransformedOutputArray[nBinX * nBinLimitRegularFreq + nBinY][0] = fGeneralTransformedInputArray[nBinX * nBinLimitRegularFreq + nBinY][0] * fGeneralTransformedKernelArray[nBinX * nBinLimitRegularFreq + nBinY][0] - fGeneralTransformedInputArray[nBinX * nBinLimitRegularFreq + nBinY][1] * fGeneralTransformedKernelArray[nBinX * nBinLimitRegularFreq + nBinY][1];
                    fGeneralTransformedOutputArray[nBinX * nBinLimitRegularFreq + nBinY][1] = fGeneralTransformedInputArray[nBinX * nBinLimitRegularFreq + nBinY][0] * fGeneralTransformedKernelArray[nBinX * nBinLimitRegularFreq + nBinY][1] + fGeneralTransformedInputArray[nBinX * nBinLimitRegularFreq + nBinY][1] * fGeneralTransformedKernelArray[nBinX * nBinLimitRegularFreq + nBinY][0];
                }
            }

            // Reverse FFT of output block
            fftw_execute( fGeneralReversePlanShortTime );
            
            // Loop over bins in the output block and fill the convolved spectrum
            for( nBinX = overlapX; nBinX < fShortSizeTime; ++nBinX )
            {
                for( nBinY = overlapY; nBinY < blockY; ++nBinY )
                {
                    SetOutputArray( nBinX - overlapX + blockNumberX * stepX, nBinY - overlapY + blockNumberY * stepY, nBinX * blockY + nBinY, *transformedSpectrum, fShortSizeTime, fShortSizeFreq );
                }
            }

            blockNumberY++;
        }

        if( blockNumberX * stepX == nBinsTotalX )
        {
            KTINFO(sdlog, "Reached end of input data");
            return transformedSpectrum;
        }

        KTINFO(sdlog, "Reached final block");
        KTINFO(sdlog, "Starting position: " << blockNumberX * stepX - overlapX);

        // Same procedure as above, this time with a shorter final block

        for( nBinX = 0; positionX+1 < nBinsTotalX; ++nBinX )
        {
            positionX = nBinX + blockNumberX * stepX - overlapX; 
            for( nBinY = 0; positionY+1 < nBinsTotalY; ++nBinY )
            {
                positionY = nBinY + blockNumberY * stepY - overlapY;
                SetInputArray( positionX, positionY, nBinX * fShortSizeFreq + nBinY, initialSpectrum );
            }
        }

        KTINFO(sdlog, "Short array length = " << nBinX);
        KTDEBUG(sdlog, "Initialized short array length = " << fShortSizeTime);

        // FFT of input block
        fftw_execute( fGeneralForwardPlanShortTimeShortFreq );

        for( int nBinX = 0; nBinX < nBinLimitShortTime; ++nBinX )
        {
            for( int nBinY = 0; nBinY < nBinLimitShortFreq; ++nBinY )
            {
                fGeneralTransformedOutputArray[nBinX * nBinLimitShortFreq + nBinY][0] = fGeneralTransformedInputArray[nBinX * nBinLimitShortFreq + nBinY][0] * fGeneralTransformedKernelArray[nBinX * nBinLimitShortFreq + nBinY][0] - fGeneralTransformedInputArray[nBinX * nBinLimitShortFreq + nBinY][1] * fGeneralTransformedKernelArray[nBinX * nBinLimitShortFreq + nBinY][1];
                fGeneralTransformedOutputArray[nBinX * nBinLimitShortFreq + nBinY][1] = fGeneralTransformedInputArray[nBinX * nBinLimitShortFreq + nBinY][0] * fGeneralTransformedKernelArray[nBinX * nBinLimitShortFreq + nBinY][1] + fGeneralTransformedInputArray[nBinX * nBinLimitShortFreq + nBinY][1] * fGeneralTransformedKernelArray[nBinX * nBinLimitShortFreq + nBinY][0];
            }
        }

        // Reverse FFT of output block
        fftw_execute( fGeneralReversePlanShortTimeShortFreq );
        
        // Loop over bins in the output block and fill the convolved spectrum
        for( nBinX = overlapX; nBinX < fShortSizeTime; ++nBinX )
        {
            for( nBinY = overlapY; nBinY < fShortSizeFreq; ++nBinY )
            {
                SetOutputArray( nBinX - overlapX + blockNumberX * stepX, nBinY - overlapY + blockNumberY * stepY, nBinX * fShortSizeFreq + nBinY, *transformedSpectrum, fShortSizeTime, fShortSizeFreq );
            }
        }

        KTINFO(sdlog, "Component finished!");

        return transformedSpectrum;
    }

    template< class XSpectraType >
    bool KTConvolution2D::SetUpGeneralVars()
    {
        return false;
    }

    template<>
    bool KTConvolution2D::SetUpGeneralVars< KTPowerSpectrum* >()
    {
        fGeneralTransformedInputArray = fTransformedInputArrayFromReal;
        fGeneralTransformedOutputArray = fTransformedOutputArrayFromReal;
        fGeneralTransformedKernelArray = fTransformedKernelXYAsReal;
        fGeneralForwardPlan = fC2CForwardPlan;
        fGeneralReversePlan = fC2CReversePlan;
        fGeneralForwardPlanShortTime = fC2CForwardPlanShortTime;
        fGeneralReversePlanShortTime = fC2CReversePlanShortTime;
        fGeneralForwardPlanShortFreq = fC2CForwardPlanShortFreq;
        fGeneralReversePlanShortFreq = fC2CReversePlanShortFreq;
        fGeneralForwardPlanShortTimeShortFreq = fC2CForwardPlanShortTimeShortFreq;
        fGeneralReversePlanShortTimeShortFreq = fC2CReversePlanShortTimeShortFreq;
        nBinLimitRegularTime = fRegularSizeTime/2 + 1;
        nBinLimitRegularFreq = fRegularSizeFreq/2 + 1;
        nBinLimitShortTime = fShortSizeTime/2 + 1;
        nBinLimitShortFreq = fShortSizeFreq/2 + 1;

        return true;
    }

    template<>
    bool KTConvolution2D::SetUpGeneralVars< KTFrequencySpectrumFFTW* >()
    {
        fGeneralTransformedInputArray = fTransformedInputArray;
        fGeneralTransformedOutputArray = fTransformedOutputArray;
        fGeneralTransformedKernelArray = fTransformedKernelXYAsComplex;
        fGeneralForwardPlan = fC2CForwardPlan;
        fGeneralReversePlan = fC2CReversePlan;
        fGeneralForwardPlanShortTime = fC2CForwardPlanShortTime;
        fGeneralReversePlanShortTime = fC2CReversePlanShortTime;
        fGeneralForwardPlanShortFreq = fC2CForwardPlanShortFreq;
        fGeneralReversePlanShortFreq = fC2CReversePlanShortFreq;
        fGeneralForwardPlanShortTimeShortFreq = fC2CForwardPlanShortTimeShortFreq;
        fGeneralReversePlanShortTimeShortFreq = fC2CReversePlanShortTimeShortFreq;
        nBinLimitRegularTime = fRegularSizeTime;
        nBinLimitRegularFreq = fRegularSizeFreq;
        nBinLimitShortTime = fShortSizeTime;
        nBinLimitShortFreq = fShortSizeFreq;

        return true;
    }

    template<>
    bool KTConvolution2D::SetUpGeneralVars< KTFrequencySpectrumPolar* >()
    {
        fGeneralTransformedInputArray = fTransformedInputArray;
        fGeneralTransformedOutputArray = fTransformedOutputArray;
        fGeneralTransformedKernelArray = fTransformedKernelXYAsComplex;
        fGeneralForwardPlan = fC2CForwardPlan;
        fGeneralReversePlan = fC2CReversePlan;
        fGeneralForwardPlanShortTime = fC2CForwardPlanShortTime;
        fGeneralReversePlanShortTime = fC2CReversePlanShortTime;
        fGeneralForwardPlanShortFreq = fC2CForwardPlanShortFreq;
        fGeneralReversePlanShortFreq = fC2CReversePlanShortFreq;
        fGeneralForwardPlanShortTimeShortFreq = fC2CForwardPlanShortTimeShortFreq;
        fGeneralReversePlanShortTimeShortFreq = fC2CReversePlanShortTimeShortFreq;
        nBinLimitRegularTime = fRegularSizeTime;
        nBinLimitRegularFreq = fRegularSizeFreq;
        nBinLimitShortTime = fShortSizeTime;
        nBinLimitShortFreq = fShortSizeFreq;

        return true;
    }

    void KTConvolution2D::ConjugateAndReverse( KTPhysicalArray< 1, KTPowerSpectrum* >& spectrum )
    {
        double temp;
        int nBins;

        for( int i = 0; i < spectrum.size(); ++i )
        {
            nBins = spectrum(i)->GetNFrequencyBins();
            for( int iBin = 0; iBin < (nBins+1)/2; ++iBin )
            {
                temp = (*spectrum(i))(iBin);
                (*spectrum(i))(iBin) = (*spectrum(i))(nBins - iBin - 1);
                (*spectrum(i))(nBins - iBin - 1) = temp;
            }
        }

        return;
    }

    void KTConvolution2D::ConjugateAndReverse( KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >& spectrum )
    {
        fftw_complex temp;
        int nBins;

        for( int i = 0; i < spectrum.size(); ++i )
        {
            nBins = spectrum(i)->GetNFrequencyBins();
            for( int iBin = 0; iBin < (nBins+1)/2; ++iBin )
            {
                temp[0] = (*(spectrum(i)))(iBin)[0];
                temp[1] = (*(spectrum(i)))(iBin)[1];
                (*(spectrum(i)))(iBin)[0] = (*(spectrum(i)))(nBins - iBin - 1)[0];
                (*(spectrum(i)))(iBin)[1] = -1. * (*(spectrum(i)))(nBins - iBin - 1)[1];
                (*(spectrum(i)))(nBins - iBin - 1)[0] = temp[0];
                (*(spectrum(i)))(nBins - iBin - 1)[1] = -1. * temp[1];
            }
        }

        return;
    }

    void KTConvolution2D::ConjugateAndReverse( KTPhysicalArray< 1, KTFrequencySpectrumPolar* >& spectrum )
    {
        fftw_complex temp;
        int nBins;

        for( int i = 0; i < spectrum.size(); ++i )
        {
            nBins = spectrum(i)->GetNFrequencyBins();
            for( int iBin = 0; iBin < (nBins+1)/2; ++iBin )
            {
                temp[0] = spectrum(i)->GetReal( iBin );
                temp[1] = spectrum(i)->GetImag( iBin );
                spectrum(i)->SetRect( iBin, spectrum(i)->GetReal(nBins - iBin - 1), -1. * spectrum(i)->GetImag(nBins - iBin - 1) );
                spectrum(i)->SetRect( nBins - iBin - 1, temp[0], -1. * temp[1] );
            }
        }

        return;
    }

    void KTConvolution2D::SetInputArray( int positionX, int positionY, int nBin, const KTPhysicalArray< 1, KTPowerSpectrum* >* initialSpectrum )
    {
        if( positionX < 0 || positionY < 0 ) { fInputArrayReal[nBin] = 0.0; }
        else { fInputArrayReal[nBin] = (*(*initialSpectrum)(positionX))(positionY); }

        return;
    }

    void KTConvolution2D::SetInputArray( int positionX, int positionY, int nBin, const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* initialSpectrum )
    {
        if( positionX < 0 || positionY < 0 )
        {
            fInputArrayComplex[nBin][0] = 0.0;
            fInputArrayComplex[nBin][1] = 0.0;
        }
        else
        {
            fInputArrayComplex[nBin][0] = (*(*initialSpectrum)(positionX))(positionY)[0];
            fInputArrayComplex[nBin][1] = (*(*initialSpectrum)(positionX))(positionY)[1];
        }

        return;
    }

    void KTConvolution2D::SetInputArray( int positionX, int positionY, int nBin, const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* initialSpectrum )
    {
        if( positionX < 0 || positionY < 0 )
        {
            fInputArrayComplex[nBin][0] = 0.0;
            fInputArrayComplex[nBin][1] = 0.0;
        }
        else
        {
            fInputArrayComplex[nBin][0] = (*initialSpectrum)(positionX)->GetReal(positionY);
            fInputArrayComplex[nBin][1] = (*initialSpectrum)(positionX)->GetImag(positionY);
        }

        return;
    }

    void KTConvolution2D::SetOutputArray( int positionX, int positionY, int nBin, KTPhysicalArray< 1, KTPowerSpectrum* >& transformedPS, double normX, double normY )
    {
        (*(transformedPS(positionX)))(positionY) = fOutputArrayReal[nBin] / (double)(normX * normY);

        return;
    }

    void KTConvolution2D::SetOutputArray( int positionX, int positionY, int nBin, KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >& transformedFSFFTW, double normX, double normY )
    {
        (*(transformedFSFFTW(positionX)))(positionY)[0] = fOutputArrayComplex[nBin][0] / (double)(normX * normY);
        (*(transformedFSFFTW(positionX)))(positionY)[1] = fOutputArrayComplex[nBin][1] / (double)(normX * normY);

        return;
    }

    void KTConvolution2D::SetOutputArray( int positionX, int positionY, int nBin, KTPhysicalArray< 1, KTFrequencySpectrumPolar* >& transformedFSPolar, double normX, double normY )
    {
        transformedFSPolar(positionX)->SetRect( positionY, fOutputArrayComplex[nBin][0] / (double)(normX * normY), fOutputArrayComplex[nBin][1] / (double)(normX * normY) );

        return;
    }

    void KTConvolution2D::SetupInternalMaps()
    {
        KTDEBUG(sdlog, "Setting up internal maps");

        // transform flag map
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }

    void KTConvolution2D::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(sdlog, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }

        fTransformFlag = flag;

        return;
    }

    void KTConvolution2D::Initialize( int nBinsTotalX, int blockX, int stepX, int overlapX, int nBinsTotalY, int blockY, int stepY, int overlapY  )
    {
        KTINFO(sdlog, "DFTs are not yet initialized; doing so now");

        // Slightly obnoxious conditionals to determine the sizes
        // Ordinary block is given, but the shorter block at the end is harder
        // Maybe there is a simpler way to determine this, I'm not sure

        if( (nBinsTotalX + overlapX) % stepX >= overlapX )
        {
            if( (nBinsTotalY + overlapY) % stepY >= overlapY )
            {
                AllocateArrays( blockX, (nBinsTotalX + overlapX) % stepX, blockY, (nBinsTotalY + overlapY) % stepY );
            }
            else
            {
                AllocateArrays( blockX, (nBinsTotalX + overlapX) % stepX, blockY, ((nBinsTotalY + overlapY) % stepY) + stepY );
            }
        }
        else
        {
            if( (nBinsTotalY + overlapY) % stepY >= overlapY )
            {
                AllocateArrays( blockX, ((nBinsTotalX + overlapX) % stepX) + stepX, blockY, (nBinsTotalY + overlapY) % stepY );
            }
            else
            {
                AllocateArrays( blockX, ((nBinsTotalX + overlapX) % stepX) + stepX, blockY, ((nBinsTotalY + overlapY) % stepY) + stepY );
            }
        }

        // Also transform the kernel, we only need to do this once
        KTDEBUG(sdlog, "Transforming kernel");

        for( int iBin = 0; iBin < blockX; ++iBin )
        {
            for( int jBin = 0; jBin < blockY; ++jBin )
            {
                fInputArrayReal[iBin * blockY + jBin] = kernelXY[iBin][jBin];
                fInputArrayComplex[iBin * blockY + jBin][0] = kernelXY[iBin][jBin];
                fInputArrayComplex[iBin * blockY + jBin][1] = 0.;
            }
        }

        fftw_execute( fRealToComplexPlan );
        fftw_execute( fC2CForwardPlan );
        
        fTransformedKernelXYAsReal = new fftw_complex[blockX * blockY];
        fTransformedKernelXYAsComplex = new fftw_complex[blockX * blockY];
        for( int iBin = 0; iBin < blockX; ++iBin )
        {
            for( int jBin = 0; jBin < blockY; ++jBin )
            {
                fTransformedKernelXYAsReal[iBin * blockY + jBin][0] = fTransformedInputArrayFromReal[iBin * blockY + jBin][0];
                fTransformedKernelXYAsReal[iBin * blockY + jBin][1] = fTransformedInputArrayFromReal[iBin * blockY + jBin][1];
                fTransformedKernelXYAsComplex[iBin * blockY + jBin][0] = fTransformedInputArray[iBin * blockY + jBin][0];
                fTransformedKernelXYAsComplex[iBin * blockY + jBin][1] = fTransformedInputArray[iBin * blockY + jBin][1];
            }
        }
    }


} /* namespace Katydid */
