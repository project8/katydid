/**
 @file KTConvolution2D.hh
 @brief Contains KTConvolution2D
 @details Performs 2D discrete convolution
 @author: E. Zayas
 @date: Oct 20, 2017
 */

#ifndef KTCONVOLUTION2D_HH_
#define KTCONVOLUTION2D_HH_

#include "KTProcessor.hh"
#include "KTSlot.hh"

#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTPowerSpectrumData.hh"

#include "KTMultiFSDataPolar.hh"
#include "KTMultiFSDataFFTW.hh"
#include "KTMultiPSData.hh"

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

    class KTConvolvedMultiPowerSpectrumData;
    class KTConvolvedMultiFSDataFFTW;
    class KTConvolvedMultiFSDataPolar;


    /*!
     @class KTConvolution2D
     @author E. Zayas

     @brief Performs discrete convolution of 1D spectra

     @details
     Uses the overlap-save method to efficiently calculate the convolution. The input is broken up into blocks of size N, where
     N can be specified at runtime or determined automatically. N must be larger than the size of the kernel, and a power of 2 is recommended
     to maximize the FFT efficiency.
  
     Configuration name: "convolution"

     Available configuration values:
     - "kernel": std::string -- Location of a .json file which contains the kernel
     - "block-size-time": double -- Size of the input blocks in the time axis. 0 will trigger an automatic determination of the ideal block size
     - "block-size-freq": double -- Size of the input blocks in the frequency axis. 0 will trigger an automatic determination of the ideal block size
     - "normalize": bool -- Normalize the kernel. If false, the output will be scaled by the norm of the kernel
     - "transform-type": std::string -- "convolution" or "cross-correlation"
     - "transform-flag": std:string -- Transform flag for FFTW

     Slots:
     - "multi-ps": void (Nymph::KTDataPtr) -- Convolves an array of power spectra; Requires KTMultiPSData; Adds KTConvolvedMultiPSData
     - "multi-fs-fftw": void (Nymph::KTDataPtr) -- Convolves a frequency spectrum; Requires KTMultiFSDataFFTW; Adds KTConvolvedMultiFSDataFFTW
     - "multi-fs-polar": void (Nymph::KTDataPtr) -- Convolves a frequency spectrum; Requires KTMultiFSDataPolar; Adds KTConvolvedMultiFSDataPolar

     Signals:
     - "conv-multi-ps": void (Nymph::KTDataPtr) -- Emitted upon convolution of a power spectrum; Guarantees KTConvolvedMultiPSData
     - "conv-multi-fs-fftw": void (Nymph::KTDataPtr) -- Emitted upon convolution of a frequency spectrum; Guarantees KTConvolvedMultiFSDataFFTW
     - "conv-multi-fs-polar": void (Nymph::KTDataPtr) -- Emitted upon convolution of a frequency spectrum; Guarantees KTConvolvedMultiFSDataPolar
    */

    class KTConvolution2D : public Nymph::KTProcessor
    {

        public:

            KTConvolution2D(const std::string& name = "convolution2D");
            virtual ~KTConvolution2D();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(std::string, Kernel);
            MEMBERVARIABLE(unsigned, BlockSizeTime);
            MEMBERVARIABLE(unsigned, BlockSizeFreq);
            MEMBERVARIABLE(bool, NormalizeKernel);
            MEMBERVARIABLE(std::string, TransformType);
            MEMBERVARIABLE_NOSET(std::string, TransformFlag);

            MEMBERVARIABLE_NOSET(double, RegularSizeTime);
            MEMBERVARIABLE_NOSET(double, RegularSizeFreq);
            MEMBERVARIABLE_NOSET(double, ShortSizeTime);
            MEMBERVARIABLE_NOSET(double, ShortSizeFreq);

            void SetTransformFlag(const std::string& flag);


        private:

            std::vector< std::vector< double > > kernelXY;

            typedef std::map< std::string, unsigned > TransformFlagMap;
            TransformFlagMap fTransformFlagMap;
            
            fftw_plan fComplexToRealPlan;
            fftw_plan fRealToComplexPlan;
            fftw_plan fC2CForwardPlan;
            fftw_plan fC2CReversePlan;

            fftw_plan fComplexToRealPlanShortTime;
            fftw_plan fRealToComplexPlanShortTime;
            fftw_plan fC2CForwardPlanShortTime;
            fftw_plan fC2CReversePlanShortTime;

            fftw_plan fComplexToRealPlanShortFreq;
            fftw_plan fRealToComplexPlanShortFreq;
            fftw_plan fC2CForwardPlanShortFreq;
            fftw_plan fC2CReversePlanShortFreq;

            fftw_plan fComplexToRealPlanShortTimeShortFreq;
            fftw_plan fRealToComplexPlanShortTimeShortFreq;
            fftw_plan fC2CForwardPlanShortTimeShortFreq;
            fftw_plan fC2CReversePlanShortTimeShortFreq;

            double *fInputArrayReal;
            double *fOutputArrayReal;
            fftw_complex *fInputArrayComplex;
            fftw_complex *fOutputArrayComplex;

            fftw_complex *fTransformedInputArray;
            fftw_complex *fTransformedOutputArray;
            fftw_complex *fTransformedInputArrayFromReal;
            fftw_complex *fTransformedOutputArrayFromReal;

            fftw_complex *fTransformedKernelXYAsReal;
            fftw_complex *fTransformedKernelXYAsComplex;

            fftw_complex* fGeneralTransformedInputArray;
            fftw_complex* fGeneralTransformedOutputArray;
            fftw_complex* fGeneralTransformedKernelArray;
            int nBinLimitRegularTime;
            int nBinLimitRegularFreq;
            int nBinLimitShortTime;
            int nBinLimitShortFreq;
            fftw_plan fGeneralForwardPlan;
            fftw_plan fGeneralReversePlan;
            fftw_plan fGeneralForwardPlanShortTime;
            fftw_plan fGeneralReversePlanShortTime;
            fftw_plan fGeneralForwardPlanShortFreq;
            fftw_plan fGeneralReversePlanShortFreq;
            fftw_plan fGeneralForwardPlanShortTimeShortFreq;
            fftw_plan fGeneralReversePlanShortTimeShortFreq;

            unsigned fTransformFlagUnsigned;
            int fKernelSizeX;
            int fKernelSizeY;
            bool fInitialized;

        public:
            
            bool ParseKernel();

            bool Convolve2D( KTMultiPSData& data );
            bool Convolve2D( KTMultiFSDataFFTW& data );
            bool Convolve2D( KTMultiFSDataPolar& data );

            template< class XMultiSpectrumDataCore, class XConvolvedMultiSpectrumTypeData >
            bool CoreConvolve2D( XMultiSpectrumDataCore& data, XConvolvedMultiSpectrumTypeData& newData );

            const KTPhysicalArray< 1, KTPowerSpectrum* >* GetSpectra( KTMultiPSDataCore& data, unsigned iComponent );
            const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* GetSpectra( KTMultiFSDataFFTWCore& data, unsigned iComponent );
            const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* GetSpectra( KTMultiFSDataPolarCore& data, unsigned iComponent );

            template< class XSpectraType >
            KTPhysicalArray< 1, XSpectraType* >* DoConvolution( const KTPhysicalArray< 1, XSpectraType* >* myInitialSpectrum, const int blockX, const int blockY, const int stepX, const int stepY, const int overlapX, const int overlapY );

            template< class XMultiSpectraType >
            bool SetUpGeneralVars();

            void ConjugateAndReverse( KTPhysicalArray< 1, KTPowerSpectrum* >& spectrum );
            void ConjugateAndReverse( KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >& spectrum );
            void ConjugateAndReverse( KTPhysicalArray< 1, KTFrequencySpectrumPolar* >& spectrum );

            void SetInputArray( int positionX, int positionY, int nBin, const KTPhysicalArray< 1, KTPowerSpectrum* >* initialSpectrum );
            void SetInputArray( int positionX, int positionY, int nBin, const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* initialSpectrum );
            void SetInputArray( int positionX, int positionY, int nBin, const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* initialSpectrum );

            void SetOutputArray( int positionX, int positionY, int nBin, KTPhysicalArray< 1, KTPowerSpectrum* >& transformedPS, double normX, double normY );
            void SetOutputArray( int positionX, int positionY, int nBin, KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >& transformedFSFFTW, double normX, double normY );
            void SetOutputArray( int positionX, int positionY, int nBin, KTPhysicalArray< 1, KTFrequencySpectrumPolar* >& transformedFSPolar, double normX, double normY );

            void SetupInternalMaps();

            bool FinishSetup();
            void Initialize( int nBinsTotalX, int blockX, int stepX, int overlapX, int nBinsTotalY, int blockY, int stepY, int overlapY );

            void AllocateArrays( int nSizeRegularX, int nSizeShortX, int nSizeRegularY, int nSizeShortY );
            void FreeArrays();

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fMultiPSSignal;
            Nymph::KTSignalData fMultiFSFFTWSignal;
            Nymph::KTSignalData fMultiFSPolarSignal;

            //***************
            // Slots
            //***************

        private:

            Nymph::KTSlotDataOneType< KTMultiPSData > fMultiPSSlot;
            Nymph::KTSlotDataOneType< KTMultiFSDataFFTW > fMultiFSFFTWSlot;
            Nymph::KTSlotDataOneType< KTMultiFSDataPolar > fMultiFSPolarSlot;            

    };

    inline const KTPhysicalArray< 1, KTPowerSpectrum* >* KTConvolution2D::GetSpectra( KTMultiPSDataCore& data, unsigned iComponent )
    {
        return data.GetSpectra( iComponent );
    }

    inline const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTConvolution2D::GetSpectra( KTMultiFSDataFFTWCore& data, unsigned iComponent )
    {
        return data.GetSpectra( iComponent );
    }

    inline const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* KTConvolution2D::GetSpectra( KTMultiFSDataPolarCore& data, unsigned iComponent )
    {
        return data.GetSpectra( iComponent );
    }

} /* namespace Katydid */

#endif /* KTCONVOLUTION2D_HH_ */
