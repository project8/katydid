/**
 @file KTConvolution1D.hh
 @brief Contains KTConvolution1D
 @details Performs 1D discrete convolution
 @author: E. Zayas
 @date: Aug 25, 2017
 */

#ifndef KTCONVOLUTION1D_HH_
#define KTCONVOLUTION1D_HH_

#include "KTProcessor.hh"
#include "KTSlot.hh"

#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTPowerSpectrumData.hh"

#include "KTLogger.hh"

#include <vector>
#include <cmath>
#include <fftw3.h>
#include <map>
#include <iostream>

namespace Katydid
{
    
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTPowerSpectrum;
    class KTPowerSpectrumData;
    class KTPowerSpectrumDataCore;

    class KTConvolvedPowerSpectrumData;
    class KTConvolvedFrequencySpectrumDataFFTW;
    class KTConvolvedFrequencySpectrumDataPolar;


    /*!
     @class KTConvolution1D
     @author E. Zayas

     @brief Performs discrete convolution of 1D spectra

     @details
     Uses the overlap-save method to efficiently calculate the convolution. The input is broken up into blocks of size N, where
     N can be specified at runtime or determined automatically. N must be larger than the size of the kernel, and a power of 2 is recommended
     to maximize the FFT efficiency.
  
     Configuration name: "convolution"

     Available configuration values:
     - "kernel": std::string -- Location of a .json file which contains the kernel
     - "block-size": double -- Size of the input blocks. 0 will trigger an automatic determination of the ideal block size
     - "normalize": bool -- Normalize the kernel. If false, the output will be scaled by the norm of the kernel
     - "transform-type": std::string -- "convolution" or "cross-correlation"
     - "transform-flag": std:string -- Transform flag for FFTW

     Slots:
     - "ps": void (Nymph::KTDataPtr) -- Convolves a power spectrum; Requires KTPowerSpectrumData; Adds KTConvolvedPowerSpectrumData
     - "fs-fftw": void (Nymph::KTDataPtr) -- Convolves a frequency spectrum; Requires KTFrequencySpectrumDataFFTW; Adds KTConvolvedFrequencySpectrumDataFFTW
     - "fs-polar": void (Nymph::KTDataPtr) -- Convolves a frequency spectrum; Requires KTFrequencySpectrumDataPolar; Adds KTConvolvedFrequencySpectrumDataPolar

     Signals:
     - "ps": void (Nymph::KTDataPtr) -- Emitted upon convolution of a power spectrum; Guarantees KTConvolvedPowerSpectrumData
     - "fs-fftw": void (Nymph::KTDataPtr) -- Emitted upon convolution of a frequency spectrum; Guarantees KTConvolvedFrequencySpectrumDataFFTW
     - "fs-polar": void (Nymph::KTDataPtr) -- Emitted upon convolution of a frequency spectrum; Guarantees KTConvolvedFrequencySpectrumDataPolar
    */

    KTLOGGER(convlog_hh, "KTConvolution.hh");

    class KTConvolution1D : public Nymph::KTProcessor
    {

        public:

            KTConvolution1D(const std::string& name = "convolution");
            virtual ~KTConvolution1D();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(std::string, Kernel);
            MEMBERVARIABLE(unsigned, BlockSize);
            MEMBERVARIABLE(bool, NormalizeKernel);
            MEMBERVARIABLE(std::string, TransformType);
            MEMBERVARIABLE_NOSET(std::string, TransformFlag);

            MEMBERVARIABLE_NOSET(double, RegularSize);
            MEMBERVARIABLE_NOSET(double, ShortSize);

            void SetTransformFlag(const std::string& flag);


        private:

            std::vector< double > kernelX;

            typedef std::map< std::string, unsigned > TransformFlagMap;
            TransformFlagMap fTransformFlagMap;
            
            fftw_plan fComplexToRealPlan;
            fftw_plan fRealToComplexPlan;
            fftw_plan fC2CForwardPlan;
            fftw_plan fC2CReversePlan;

            fftw_plan fComplexToRealPlanShort;
            fftw_plan fRealToComplexPlanShort;
            fftw_plan fC2CForwardPlanShort;
            fftw_plan fC2CReversePlanShort;

            double *fInputArrayReal;
            double *fOutputArrayReal;
            fftw_complex *fInputArrayComplex;
            fftw_complex *fOutputArrayComplex;

            fftw_complex *fTransformedInputArray;
            fftw_complex *fTransformedOutputArray;
            fftw_complex *fTransformedInputArrayFromReal;
            fftw_complex *fTransformedOutputArrayFromReal;

            fftw_complex *fTransformedKernelXAsReal;
            fftw_complex *fTransformedKernelXAsComplex;

            fftw_complex* fGeneralTransformedInputArray;
            fftw_complex* fGeneralTransformedOutputArray;
            fftw_complex* fGeneralTransformedKernelArray;
            int nBinLimitRegular;
            int nBinLimitShort;
            fftw_plan fGeneralForwardPlan;
            fftw_plan fGeneralReversePlan;
            fftw_plan fGeneralForwardPlanShort;
            fftw_plan fGeneralReversePlanShort;

            unsigned fTransformFlagUnsigned;
            int fKernelSize;
            bool fInitialized;

        public:
            
            bool ParseKernel();

            bool Convolve1D( KTPowerSpectrumData& data );
            bool Convolve1D( KTFrequencySpectrumDataFFTW& data );
            bool Convolve1D( KTFrequencySpectrumDataPolar& data );

            template< class XSpectrumDataCore, class XConvolvedSpectrumTypeData >
            bool CoreConvolve1D( XSpectrumDataCore& data, XConvolvedSpectrumTypeData& newData );

            const KTPowerSpectrum* GetSpectrum( KTPowerSpectrumDataCore& data, unsigned iComponent );
            const KTFrequencySpectrumFFTW* GetSpectrum( KTFrequencySpectrumDataFFTWCore& data, unsigned iComponent );
            const KTFrequencySpectrumPolar* GetSpectrum( KTFrequencySpectrumDataPolarCore& data, unsigned iComponent );

            template< class XSpectraType >
            XSpectraType* DoConvolution( const XSpectraType* initialSpectrum, const int block, const int step, const int overlap );

            template< class XSpectraType >
            bool SetUpGeneralVars();

            void ConjugateAndReverse( KTPowerSpectrum& spectrum );
            void ConjugateAndReverse( KTFrequencySpectrumFFTW& spectrum );
            void ConjugateAndReverse( KTFrequencySpectrumPolar& spectrum );

            void SetInputArray( int position, int nBin, const KTPowerSpectrum* initialSpectrum );
            void SetInputArray( int position, int nBin, const KTFrequencySpectrumFFTW* initialSpectrum );
            void SetInputArray( int position, int nBin, const KTFrequencySpectrumPolar* initialSpectrum );

            void SetOutputArray( int position, int nBin, KTPowerSpectrum& transformedPS, double norm );
            void SetOutputArray( int position, int nBin, KTFrequencySpectrumFFTW& transformedFSFFTW, double norm );
            void SetOutputArray( int position, int nBin, KTFrequencySpectrumPolar& transformedFSPolar, double norm );

            void SetupInternalMaps();

            bool FinishSetup();
            void Initialize( int nBinsTotal, int block, int step, int overlap );

            void AllocateArrays( int nSizeRegular, int nSizeShort );
            void FreeArrays();

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fPSSignal;
            Nymph::KTSignalData fFSFFTWSignal;
            Nymph::KTSignalData fFSPolarSignal;

            //***************
            // Slots
            //***************

        private:

            Nymph::KTSlotDataOneType< KTPowerSpectrumData > fPSSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarSlot;            

    };

    template< class XSpectrumDataCore, class XConvolvedSpectrumTypeData >
    bool KTConvolution1D::CoreConvolve1D( XSpectrumDataCore& data, XConvolvedSpectrumTypeData& newData )
    {
        newData.SetNComponents( data.GetNComponents() );

        // Set overlap-and-save method parameters
        // These do not change, but I want to group them together like this so it's easy to follow
        int block = GetBlockSize();
        int overlap = fKernelSize - 1;
        int step = block - overlap;

        KTINFO(convlog_hh, "Block size: " << block);
        KTINFO(convlog_hh, "Overlap: " << overlap);
        KTINFO(convlog_hh, "Step size: " << step);

        int nBinsTotal = GetSpectrum( data, 0 )->GetNFrequencyBins();
        KTINFO(convlog_hh, "nBinsTotal = " << nBinsTotal);

        // Now that nBinsTotal is determined, we can initialize the DFTs
        // The following conditional should only be true on the first slice
        if( ! fInitialized )
        {
            Initialize( nBinsTotal, block, step, overlap );
        }

        // First loop over components
        for( unsigned iComponent = 0; iComponent < data.GetNComponents(); ++iComponent )
        {
            KTINFO(convlog_hh, "Starting component: " << iComponent);

            // Get power spectrum and initialize convolved spectrum for this component
            typename XSpectrumDataCore::spectrum_type* transformedSpectrum = DoConvolution( GetSpectrum( data, iComponent ), block, step, overlap );

            if( transformedSpectrum == nullptr )
            {
                KTERROR( convlog_hh, "Convolution was unsuccessful. Aborting." );
                return false;
            }

            // Set power spectrum
            newData.SetSpectrum( transformedSpectrum, iComponent );
            KTDEBUG(convlog_hh, "Filled new spectrum");
        }

        KTINFO(convlog_hh, "All components finished successfully!");

        return true;
    }

    template< class XSpectraType >
    XSpectraType* KTConvolution1D::DoConvolution( const XSpectraType* myInitialSpectrum, const int block, const int step, const int overlap )
    {
        int nBinsTotal = myInitialSpectrum->GetNFrequencyBins();

        // non-const version
        XSpectraType* initialSpectrum = new XSpectraType( *myInitialSpectrum );

        // If we're doing cross-correlation, first we need to conjugate and reverse the input spectrum
        if( fTransformType == "cross-correlation" )
        {
            ConjugateAndReverse( *initialSpectrum );
        }

        XSpectraType* transformedSpectrum = new XSpectraType( nBinsTotal, initialSpectrum->GetRangeMin(), initialSpectrum->GetRangeMax() );

        if( ! SetUpGeneralVars< XSpectraType* >() )
        {
            KTERROR(convlog_hh, "Spectrum type unknown. Returning blank spectrum");
            return transformedSpectrum;
        }

        // Loop over block numbers
        int blockNumber = 0;
        int position = 0;
        while( (blockNumber+1) * step <= nBinsTotal )
        {
            KTDEBUG(convlog_hh, "Block number: " << blockNumber);
            KTDEBUG(convlog_hh, "Starting position: " << blockNumber * step - overlap);
            KTDEBUG(convlog_hh, "nBinsTotal: " << nBinsTotal);

            // Fill input array
            for( int nBin = 0; nBin < block; ++nBin )
            {
                position = nBin + blockNumber * step - overlap;
                SetInputArray( position, nBin, initialSpectrum );
            }

            // FFT of input block
            KTDEBUG(convlog_hh, "Performing DFT");
            fftw_execute( fGeneralForwardPlan );

            // Bin multiplication in fourier space
            KTDEBUG(convlog_hh, "Multiplying arrays in fourier space");

            for( int nBin = 0; nBin < nBinLimitRegular; ++nBin )
            {
                fGeneralTransformedOutputArray[nBin][0] = fGeneralTransformedInputArray[nBin][0] * fGeneralTransformedKernelArray[nBin][0] - fGeneralTransformedInputArray[nBin][1] * fGeneralTransformedKernelArray[nBin][1];
                fGeneralTransformedOutputArray[nBin][1] = fGeneralTransformedInputArray[nBin][0] * fGeneralTransformedKernelArray[nBin][1] + fGeneralTransformedInputArray[nBin][1] * fGeneralTransformedKernelArray[nBin][0];
            }

            // Reverse FFT of output block
            KTDEBUG(convlog_hh, "Performing reverse DFT");
            fftw_execute( fGeneralReversePlan );

            // Loop over bins in the output block and fill the convolved spectrum
            for( int nBin = overlap; nBin < block; ++nBin )
            {
                SetOutputArray( nBin - overlap + blockNumber * step, nBin, *transformedSpectrum, block );
            }

            // Increment block number
            ++blockNumber;
        }

        if( blockNumber * step == nBinsTotal )
        {
            KTINFO(convlog_hh, "Reached end of input data");
            return transformedSpectrum;
        }

        KTINFO(convlog_hh, "Reached final block");
        KTINFO(convlog_hh, "Starting position: " << blockNumber * step - overlap);

        // Same procedure as above, this time with a shorter final block

#ifndef NDEBUG
        int lastNBin = 0;
#endif
        for( int nBin = 0; position+1 < nBinsTotal; ++nBin )
        {
            position = nBin + blockNumber * step - overlap;
            SetInputArray( position, nBin, initialSpectrum );
#ifndef NDEBUG
            lastNBin = nBin;
#endif
        }

        KTDEBUG(convlog_hh, "Short array length = " << lastNBin);
        KTDEBUG(convlog_hh, "Initialized short array length = " << fShortSize);

        // FFT of input block
        fftw_execute( fGeneralForwardPlanShort );

        for( int nBin = 0; nBin < nBinLimitShort; ++nBin )
        {
            fGeneralTransformedOutputArray[nBin][0] = fGeneralTransformedInputArray[nBin][0] * fGeneralTransformedKernelArray[nBin][0] - fGeneralTransformedInputArray[nBin][1] * fGeneralTransformedKernelArray[nBin][1];
            fGeneralTransformedOutputArray[nBin][1] = fGeneralTransformedInputArray[nBin][0] * fGeneralTransformedKernelArray[nBin][1] + fGeneralTransformedInputArray[nBin][1] * fGeneralTransformedKernelArray[nBin][0];
        }

        // Reverse FFT of output block
        fftw_execute( fGeneralReversePlanShort );

        // Loop over bins in the output block and fill the convolved spectrum
        for( int nBin = overlap; nBin < fShortSize; ++nBin )
        {
            SetOutputArray( nBin - overlap + blockNumber * step, nBin, *transformedSpectrum, fShortSize );
        }

        KTINFO(convlog_hh, "Component finished!");

        return transformedSpectrum;
    }

    inline const KTPowerSpectrum* KTConvolution1D::GetSpectrum( KTPowerSpectrumDataCore& data, unsigned iComponent )
    {
        return data.GetSpectrum( iComponent );
    }

    inline const KTFrequencySpectrumFFTW* KTConvolution1D::GetSpectrum( KTFrequencySpectrumDataFFTWCore& data, unsigned iComponent )
    {
        return data.GetSpectrumFFTW( iComponent );
    }

    inline const KTFrequencySpectrumPolar* KTConvolution1D::GetSpectrum( KTFrequencySpectrumDataPolarCore& data, unsigned iComponent )
    {
        return data.GetSpectrumPolar( iComponent );
    }

} /* namespace Katydid */

#endif /* KTCONVOLUTION1D_HH_ */
