/**
 @file KTWignerVille.hh
 @brief Contains KTWignerVille
 @details 
 @author: N. S. Oblath
 @date: Oct 19, 2012
 */

#ifndef KTWIGNERVILLE_HH_
#define KTWIGNERVILLE_HH_

#include "KTProcessor.hh"

#include "KTComplexFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPStoreNode.hh"
#include "KTSlot.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTWignerVilleData.hh"
//#include "KTWV2DData.hh"

#include <boost/circular_buffer.hpp>

#include <complex>
#include <utility>


namespace Katydid
{
    KTLOGGER(wvlog, "katydid.analysis");

    class KTAnalyticAssociateData;
    class KTComplexFFTW;
    class KTEggHeader;
    //class KTSliceHeader;
    class KTTimeSeriesData;
    class KTWindower;

    /*!
     @class KTWignerVille
     @author N. S. Oblath

     @brief 

     @details


     Recommendations:
     - There should not be any overlap between the slices produced by the EggProcessor (or whatever source of time series is used).
       Overlap is not checked for when copying the data from the slices to the circular buffer.
     - The slice size produced by the EggProcessor (or other slice source) must be larger than the window size.
     - The Wigner-Ville transform technically has no negative frequency components in the output; This implementation does because
       of the type of DFT that is used. You should follow the WV transform with a switch to polar format that drops the negative
       frequency bins (using processor "switch-fftw-polar," with "use-neg-freqs" set to false)

     Configuration name: "wigner-ville"

     Available configuration values:
     - "complex-fftw": object -- configure the fftw
     - "wv-pair": bool -- channel pair to be used in the Wigner-Ville transform: "[first channel], [second channel]"; e.g. "0, 0" or "0, 1"
     - "window-size": unsigned -- number of bins to use for the WV window
     - "window-stride": unsigned -- number of bins to skip between WV windows
     - "n-windows-to-average": unsigned -- number of windows to average together into a single WV window

     Slots:
     - "header": void (const KTEggHeader*) -- Initializes the transform using an Egg header
     - "ts": void (KTDataPtr) -- Perform a WV transform on a time series; Requires KTTimeSeriesData; Adds KTWignerVilleData
     - "aa": void (KTDataPtr) -- Perform a WV transform on an analytic associate: Requires KTAnalyticAssociateData; Adds KTWignerVilleData

     Signals:
     - "wv": void (KTDataPtr) -- Emitted upon performance of a WV transform; Guarantees KTWignerVilleData
     */

    class KTWignerVille : public KTProcessor
    {
        public:
            typedef std::vector< UIntPair > PairVector;

            typedef std::complex< Double_t > Complex;
            typedef boost::circular_buffer< Complex > Buffer;

        public:
            KTWignerVille(const std::string& name = "wigner-ville");
            virtual ~KTWignerVille();

            Bool_t Configure(const KTPStoreNode* node);

            void AddPair(const UIntPair& pair);
            void SetPairVector(const PairVector& pairs);
            const PairVector& GetPairVector() const;
            void ClearPairs();

            UInt_t GetWindowSize() const;
            void SetWindowSize(UInt_t size);

            UInt_t GetWindowStride() const;
            void SetWindowStride(UInt_t stride);

            UInt_t GetNWindowsToAverage() const;
            void SetNWindowsToAverage(UInt_t nAvg);

            Bool_t GetUseWindowFunction() const;
            void SetUseWindowFunction(Bool_t flag);

            KTWindower* GetWindower();
            const KTWindower* GetWindower() const;

            KTComplexFFTW* GetFFT();
            const KTComplexFFTW* GetFFT() const;

            void Initialize(Double_t acqRate, UInt_t nComponents, UInt_t inputSliceSize);
            void InitializeWithHeader(const KTEggHeader* header);

        private:
            PairVector fPairs;

            UInt_t fWindowSize;
            UInt_t fWindowStride;
            UInt_t fNWindowsToAverage;

        public:
            /// Performs the W-V transform on the given time series data.
            Bool_t TransformData(KTTimeSeriesData& data, KTSliceHeader& header);
            /// Performs the WV transform on the given analytic associate data.
            Bool_t TransformData(KTAnalyticAssociateData& data, KTSliceHeader& header);

        private:
            template< class XDataType >
            Bool_t TransformFFTWBasedData(XDataType& data, KTSliceHeader& header);

            //void CrossMultiplyToInputArray(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset);
            // TODO: remove iWindow argument
            void CalculateACF(Buffer::iterator data1It, const Buffer::iterator& data2End, UInt_t iWindow);
            void CalculateLaggedACF(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset);

            KTSliceHeader fFirstHeader;
            KTSliceHeader fSecondHeader;

            Bool_t fReceivedLastData;

            std::vector< Buffer > fBuffer;
            UInt_t fSliceSampleOffset;
            Bool_t fAdvanceStartIteratorOnNewSlice;

            std::vector< Buffer::iterator > fSliceBreak;

            KTTimeSeriesFFTW* fInputArray;

            Bool_t fUseWindowFunction;
            KTWindower* fWindower;

            KTComplexFFTW* fFFT;

            std::vector< KTFrequencySpectrumFFTW* > fOutputArrays;

            KTDataPtr fOutputData;
            KTSliceHeader* fOutputSHData; // pointer to object that is part of fOutputData
            //KTWV2DData* fOutputWVData;
            KTWignerVilleData* fOutputWVData; // pointer to object that is part of fOutputData

            UInt_t fWindowAverageCounter;
            UInt_t fWindowCounter;
            UInt_t fDataOutCounter;
            //UInt_t fLeftStartPointer;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fWVSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
            //KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;
            //KTSlotDataOneType< KTAnalyticAssociateData > fAnalyticAssociateSlot;

            void ProcessTimeSeries(KTDataPtr data);
            void ProcessAnalyticAssociate(KTDataPtr data);

    };

    inline void KTWignerVille::AddPair(const UIntPair& pair)
    {
        fPairs.push_back(pair);
        return;
    }

    inline void KTWignerVille::SetPairVector(const PairVector& pairs)
    {
        fPairs = pairs;
        return;
    }

    inline const KTWignerVille::PairVector& KTWignerVille::GetPairVector() const
    {
        return fPairs;
    }

    inline void KTWignerVille::ClearPairs()
    {
        fPairs.clear();
        return;
    }

    inline UInt_t KTWignerVille::GetWindowSize() const
    {
        return fWindowSize;
    }

    inline void KTWignerVille::SetWindowSize(UInt_t size)
    {
        fWindowSize = size;
        return;
    }

    inline UInt_t KTWignerVille::GetWindowStride() const
    {
        return fWindowStride;
    }

    inline void KTWignerVille::SetWindowStride(UInt_t stride)
    {
        fWindowStride = stride;
        return;
    }

    inline UInt_t KTWignerVille::GetNWindowsToAverage() const
    {
        return fNWindowsToAverage;
    }

    inline void KTWignerVille::SetNWindowsToAverage(UInt_t nAvg)
    {
        fNWindowsToAverage = nAvg;
        return;
    }

    inline Bool_t KTWignerVille::GetUseWindowFunction() const
    {
        return fUseWindowFunction;
    }

    inline void KTWignerVille::SetUseWindowFunction(Bool_t flag)
    {
        fUseWindowFunction = flag;
        return;
    }

    inline KTWindower* KTWignerVille::GetWindower()
    {
        return fWindower;
    }

    inline const KTWindower* KTWignerVille::GetWindower() const
    {
        return fWindower;
    }

    inline KTComplexFFTW* KTWignerVille::GetFFT()
    {
        return fFFT;
    }

    inline const KTComplexFFTW* KTWignerVille::GetFFT() const
    {
        return fFFT;
    }

    template< class XDataType >
    Bool_t KTWignerVille::TransformFFTWBasedData(XDataType& data, KTSliceHeader& header)
    {
            //if (fPairs.empty())
            //{
            //    KTWARN(wvlog, "No Wigner-Ville pairs specified; no transforms performed.");
            //    return false;
            //}

            if (fOutputSHData == NULL || fOutputWVData == NULL)
            {
                KTERROR(wvlog, "Output data has not been initialized properly");
                return false;
            }

            fOutputWVData->Clear();

            UInt_t nComponents = data.GetNComponents();
            if (nComponents != fBuffer.size())
            {
                KTERROR(wvlog, "Number of components mismatched between the buffer (" << fBuffer.size() << ") and the data (" << nComponents << ")");
                return false;
            }

            /*
            // cast all time series into KTTimeSeriesFFTW
            std::vector< const KTTimeSeriesFFTW* > timeSeries(nComponents);
            for (UInt_t iTS=0; iTS < nComponents; iTS++)
            {
                timeSeries[iTS] = dynamic_cast< const KTTimeSeriesFFTW* >(data.GetTimeSeries(iTS));
                if (timeSeries[iTS] == NULL)
                {
                    KTERROR(wvlog, "Time series " << iTS << " did not cast to a const KTTimeSeriesFFTW*. No transforms performed.");
                    return false;
                }
            }
             */

            UInt_t nPairs = fOutputWVData->GetNComponents();

            std::vector< UInt_t > preCopyBufferSize(nComponents);
            for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                preCopyBufferSize[iComponent] = fBuffer[iComponent].size();
                KTDEBUG(wvlog, "Pre-copy buffer " << iComponent << " size: " << preCopyBufferSize[iComponent]);
            }

            // copy the arriving header into fSecondHeader
            fSecondHeader.CopySliceHeaderOnly(header);

            // check if the data that just arrived is from a new acquisition
            Bool_t localIsNewAcquisition = false;
            if (header.GetIsNewAcquisition())
            {
                localIsNewAcquisition = true;
                for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
                {
                    fBuffer[iComponent].clear();
                }
            }

            // iterators to track where to start the next window
            std::vector< Buffer::iterator > windowStartIterator(nComponents);
            // iterators to track where this window ends
            std::vector< Buffer::iterator > endOfCurrentWindow(nComponents);

            // copy the data into the circular buffer
            for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                const KTTimeSeriesFFTW* ts = static_cast< const KTTimeSeriesFFTW* >(data.GetTimeSeries(iComponent));
                UInt_t tsSize = ts->size();
                for (UInt_t iBin = 0; iBin < tsSize; ++iBin)
                {
                    fBuffer[iComponent].push_back(std::complex< Double_t >((*ts)(iBin)[0], (*ts)(iBin)[1]));
                }
                // we should only need to advance the start iterator if the start of this window
                // didn't fit in the last slice during the previous iteration
                if (fAdvanceStartIteratorOnNewSlice)
                {
                    windowStartIterator[iComponent] = fBuffer[iComponent].begin() + fSliceSampleOffset;
                    fAdvanceStartIteratorOnNewSlice = false;
                }
                else
                {
                    windowStartIterator[iComponent] = fBuffer[iComponent].begin();
                }
            }

            // set the iterators that point to the break between the slices
            for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                fSliceBreak[iComponent] = fBuffer[iComponent].begin() + preCopyBufferSize[iComponent];
                KTDEBUG(wvlog, "Slice break " << iComponent << " offset: " << fSliceBreak[iComponent] - fBuffer[iComponent].begin());
            }

            // This is declared outside of the buffer loop so that after the loop we know which slice the last window started in
            Bool_t windowStartInFirstSlice = false;

            // loop over the buffer until we get too close to the end to fit another window
            Bool_t exitBufferLoop = false;
            while (! exitBufferLoop)
            {
                KTDEBUG(wvlog, "Slice sample offset: " << fSliceSampleOffset);

                fOutputWVData->Clear();

                for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
                {
                    endOfCurrentWindow[iComponent] = windowStartIterator[iComponent] + (fWindowSize - 1);
                    KTDEBUG(wvlog, "End of current window " << iComponent << " offset: " << endOfCurrentWindow[iComponent] - fBuffer[iComponent].begin());
                }

                // a few things need to be done depending on if the window starts in the first slice or the second slice
                if (windowStartIterator[0] < fSliceBreak[0])
                {
                    windowStartInFirstSlice = true;
                    // set the slice header information if necessary
                    if (fWindowAverageCounter == 0)
                    {
                        fOutputSHData->SetStartRecordAndSample(fFirstHeader.GetRecordSamplePairAtSample(fSliceSampleOffset));
                        fOutputSHData->SetTimeInRun(fFirstHeader.GetTimeInRunAtSample(fSliceSampleOffset));
                        fOutputSHData->SetIsNewAcquisition(false);
                        fOutputSHData->SetRecordSize(fFirstHeader.GetRecordSize());
                        for (UInt_t iPair = 0; iPair < nPairs; ++iPair)
                        {
                            UInt_t firstChannel = fPairs[iPair].first;
                            fOutputSHData->SetTimeStamp(fFirstHeader.GetTimeStampAtSample(fSliceSampleOffset, firstChannel), iPair);
                            fOutputSHData->SetAcquisitionID(fFirstHeader.GetAcquisitionID(firstChannel), iPair);
                            fOutputSHData->SetRecordID(fFirstHeader.GetRecordID(firstChannel), iPair);
                        }
                    }
                }
                else
                {
                    windowStartInFirstSlice = false;
                    // set the slice header information if necessary
                    if (fWindowAverageCounter == 0)
                    {
                        fOutputSHData->SetStartRecordAndSample(fSecondHeader.GetRecordSamplePairAtSample(fSliceSampleOffset));
                        fOutputSHData->SetTimeInRun(fSecondHeader.GetTimeInRunAtSample(fSliceSampleOffset));
                        fOutputSHData->SetIsNewAcquisition(localIsNewAcquisition);
                        fOutputSHData->SetRecordSize(fSecondHeader.GetRecordSize());
                        for (UInt_t iPair = 0; iPair < nPairs; ++iPair)
                        {
                            UInt_t firstChannel = fPairs[iPair].first;
                            fOutputSHData->SetTimeStamp(fSecondHeader.GetTimeStampAtSample(fSliceSampleOffset, firstChannel), iPair);
                            fOutputSHData->SetAcquisitionID(fSecondHeader.GetAcquisitionID(firstChannel), iPair);
                            fOutputSHData->SetRecordID(fSecondHeader.GetRecordID(firstChannel), iPair);
                        }
                      }
                }



                // analyze the data in the buffer
                for (UInt_t iPair = 0; iPair < nPairs; ++iPair)
                {
                    UInt_t firstChannel = fPairs[iPair].first;
                    UInt_t secondChannel =  fPairs[iPair].second;

                    Buffer::iterator startWindowFC = windowStartIterator[firstChannel];
                    Buffer::iterator startWindowSC = windowStartIterator[secondChannel];

                    KTDEBUG(wvlog, "Data remaining in buffer before transform -- ch " << firstChannel << ": " << fBuffer[firstChannel].end() - startWindowFC << "    ch " << secondChannel << ": " << fBuffer[secondChannel].end() - startWindowSC);
                    CalculateACF(startWindowFC, startWindowSC, fWindowCounter);
                    //fOutputWVData->SetSpectrum(fFFT->Transform(fInputArray), iWindowInLoop, iPair);
                    if (fWindowAverageCounter == 0)
                    {
                        fOutputWVData->SetSpectrum(fFFT->Transform(fInputArray), iPair);
                    }
                    else
                    {
                        // this is done here with DoTransform to avoid the repeated allocation of new memory
                        fFFT->DoTransform(fInputArray, fOutputArrays[iPair]);
                        *(fOutputWVData->GetSpectrumFFTW(iPair)) += *(fOutputArrays[iPair]);
                    }
                }

                ++fWindowCounter;
                ++fWindowAverageCounter;


                // time to output new data!
                if (fWindowAverageCounter == fNWindowsToAverage)
                {
                    // if we want an average and not just a sum, this is where the division by the fNWindowsToAverage should go

                    // Finish filling in the output data
                    fOutputData->fLastData = fReceivedLastData;
                    fOutputData->fCounter = fDataOutCounter;

                    fOutputSHData->SetSliceNumber(fDataOutCounter);
                    // window ALWAYS ends in the second slice
                    fOutputSHData->SetEndRecordAndSample(fSecondHeader.GetRecordSamplePairAtSample(endOfCurrentWindow[0] - fSliceBreak[0]));

                    KTDEBUG(wvlog, "Signaling output data;\n" << *fOutputSHData);

                    // Call the signal on the output data
                    fWVSignal(fOutputData);

                    ++fDataOutCounter;
                    fWindowAverageCounter = 0;
                }


                // Move the iterators and sample offset counters
                // if this next if statement is true, then we have enough space to move the start of the next window forward
                // otherwise we'll need to exit (which will happen at the next if statement)
                // it may still be, of course, that the window itself won't fit
                // but that's okay; we want to answer that question separately
                if (fBuffer[0].end() - windowStartIterator[0] > fWindowStride) // (note: this is a comparison to fWindowSTRIDE)
                {
                    // the beginning of the next window fits in the buffer, so just move the iterators up by fWindowStride
                    for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
                    {
                        windowStartIterator[iComponent] += fWindowStride;
                    }
                    // update fSliceSampleOffset based on component 0
                    // if we're still on the first slice in the buffer, advance the offset
                    // if we're on the second slice, set the offset based on distance from the slice break
                    if (windowStartIterator[0] < fSliceBreak[0])
                    {
                        // move fSliceSampleOffset along in the first slice
                        fSliceSampleOffset += fWindowStride;
                    }
                    else
                    {
                        // the offset we will have in the second slice
                        fSliceSampleOffset = windowStartIterator[0] - fSliceBreak[0];
                    }

                }
                else
                {
                    // we are unable to move the start of the next window forward within the buffer.
                    // this offset is how far into the next slice, when its received, we need to start
                    fSliceSampleOffset = fWindowStride - (fBuffer[0].end() - windowStartIterator[0]);
                    fAdvanceStartIteratorOnNewSlice = true;
                    // this change to futureStarWindow will guarantee that the next if statement will cause the buffer loop to exit
                    // we'll need to update windowStartIterator the next time this method is called; we haven't received the next slice yet, so we can't set pointers to the buffer
                    for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
                    {
                        windowStartIterator[iComponent] = fBuffer[iComponent].end();
                    }
                }

                // at this point, windowStartIterator has been updated for the next window

                // determine if we need to exit the loop
                // if this next if statement is true, then we can't fit the next window in what remains of the buffer
                if (fBuffer[0].end() - windowStartIterator[0] < fWindowSize) // (note: this is a comparison to fWindowSIZE)
                    exitBufferLoop = true;

            } // end of the loop over windows in the buffer



            // remove data that has now been analyzed completely
            for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                // just in case the windowStartIterator iterator is beyond the end of the buffer
                if (! (windowStartIterator[iComponent] < fBuffer[iComponent].end()))
                    windowStartIterator[iComponent] = fBuffer[iComponent].end();

                // pop from the front until the front reaches windowStartIterator
                while (fBuffer[iComponent].begin() != windowStartIterator[iComponent])
                {
                    fBuffer[iComponent].pop_front();
                }

                KTDEBUG(wvlog, "Data removed from circular buffer (ch. " << iComponent << "); samples remaining: " << fBuffer[iComponent].end() - fBuffer[iComponent].begin());
            }

            // copy second header data into first header
            fFirstHeader = fSecondHeader;


            /*
            UInt_t nPairs = fPairs.size();

            //KTWV2DData& newData = data.template Of< KTWV2DData >().SetNComponents(nPairs);
            ////KTWignerVilleData& newData = data.template Of< KTWignerVilleData >().SetNComponents(nPairs);

            // Do WV transform for each pair
            UInt_t iPair = 0;
            for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
            {
                UInt_t firstChannel = (*pairIt).first;
                UInt_t secondChannel = (*pairIt).second;

                //UInt_t nOffsets = timeSeries[firstChannel]->size();
                UInt_t nOffsets = 896;
                Double_t timeBW = timeSeries[firstChannel]->GetBinWidth();

                newData.SetInputPair(firstChannel, secondChannel, iPair);
                KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* newSpectra = new KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >(nOffsets, -0.5 * timeBW, timeBW * (Double_t(nOffsets) - 0.5));
                for (UInt_t iSpectrum = 0; iSpectrum < nOffsets; iSpectrum++)
                {
                    (*newSpectra)(iSpectrum) = NULL;
                }
                newData.SetSpectra(newSpectra, iPair);

                for (; fLeftStartPointer + fInputArray->size() < timeSeries[firstChannel]->size(); fLeftStartPointer++)
                {
                    CalculateLaggedACF(timeSeries[firstChannel], timeSeries[secondChannel], 0);
                    newData.SetSpectrum(fFFT->Transform(fInputArray), fLeftStartPointer, iPair);
                }
                KTDEBUG(wvlog, "Left start pointer now at " << fLeftStartPointer);

                //for(UInt_t offset = 0; offset < nOffsets; offset++)
                //{
                //    CalculateLaggedACF(timeSeries[firstChannel], timeSeries[secondChannel], offset);
                //    newData.SetSpectrum(fFFT->Transform(fInputArray), offset, iPair);
                //    ////newData.SetSpectrum(fFFT->Transform(fInputArray), iPair);
                //}

                // why was this put here, cutting the frequency range in half?
                //newSpectrum->SetRange(0.5 * newSpectrum->GetRangeMin(), 0.5 * newSpectrum->GetRangeMax());

                iPair++;
            }
            KTINFO(wvlog, "Completed WV transform of " << iPair << " pairs");
             */

            return true;
    }

} /* namespace Katydid */
#endif /* KTWIGNERVILLE_HH_ */
