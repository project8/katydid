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

#include "KTForwardFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
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
    
    KTLOGGER(wvlog, "KTWignerVille");

    class KTAnalyticAssociateData;
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
     - "wv-pairs": array of arrays -- channel pairs to be used in the Wigner-Ville transform:
                                      e.g.: "corr-pairs": [ [0, 1], [1, 0], [1, 1] ]
     - "window-size": unsigned -- number of bins to use for the WV window
     - "window-stride": unsigned -- number of bins to skip between WV windows
     - "n-windows-to-average": unsigned -- number of windows to average together into a single WV window

     Slots:
     - "header": void (Nymph::KTDataPtr) -- Initializes the transform using an Egg header; Requires KTEggHeader
     - "ts": void (Nymph::KTDataPtr) -- Perform a WV transform on a time series; Requires KTTimeSeriesData; Adds KTWignerVilleData
     - "aa": void (Nymph::KTDataPtr) -- Perform a WV transform on an analytic associate: Requires KTAnalyticAssociateData; Adds KTWignerVilleData

     Signals:
     - "wv": void (Nymph::KTDataPtr) -- Emitted upon performance of a WV transform; Guarantees KTWignerVilleData
     */

    class KTWignerVille : public Nymph::KTProcessor
    {
        public:
            typedef std::pair< unsigned, unsigned > UIntPair;
            typedef std::vector< UIntPair > PairVector;

            typedef std::complex< double > Complex;
            typedef boost::circular_buffer< Complex > Buffer;

        public:
            KTWignerVille(const std::string& name = "wigner-ville");
            virtual ~KTWignerVille();

            bool Configure(const scarab::param_node* node);

            void AddPair(const UIntPair& pair);
            void SetPairVector(const PairVector& pairs);
            const PairVector& GetPairVector() const;
            void ClearPairs();

            unsigned GetWindowSize() const;
            void SetWindowSize(unsigned size);

            unsigned GetWindowStride() const;
            void SetWindowStride(unsigned stride);

            unsigned GetNWindowsToAverage() const;
            void SetNWindowsToAverage(unsigned nAvg);

            bool GetUseWindowFunction() const;
            void SetUseWindowFunction(bool flag);

            KTWindower* GetWindower();
            const KTWindower* GetWindower() const;

            KTForwardFFTW* GetFFT();
            const KTForwardFFTW* GetFFT() const;

            bool Initialize(double acqRate, unsigned nComponents, unsigned inputSliceSize);
            bool InitializeWithHeader(KTEggHeader& header);

        private:
            PairVector fPairs;

            unsigned fWindowSize;
            unsigned fWindowStride;
            unsigned fNWindowsToAverage;

        public:
            /// Performs the W-V transform on the given time series data.
            bool TransformData(KTTimeSeriesData& data, KTSliceHeader& header);
            /// Performs the WV transform on the given analytic associate data.
            bool TransformData(KTAnalyticAssociateData& data, KTSliceHeader& header);

        private:
            template< class XDataType >
            bool TransformFFTWBasedData(XDataType& data, KTSliceHeader& header);

            //void CrossMultiplyToInputArray(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, unsigned offset);
            // TODO: remove iWindow argument
            void CalculateACF(Buffer::iterator data1It, const Buffer::iterator& data2End, unsigned iWindow);
            void CalculateLaggedACF(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, unsigned offset);

            KTSliceHeader fFirstHeader;
            KTSliceHeader fSecondHeader;

            bool fReceivedLastData;

            std::vector< Buffer > fBuffer;
            unsigned fSliceSampleOffset;
            bool fAdvanceStartIteratorOnNewSlice;

            std::vector< Buffer::iterator > fSliceBreak;

            KTTimeSeriesFFTW* fInputArray;

            bool fUseWindowFunction;
            KTWindower* fWindower;

            KTForwardFFTW* fFFT;

            std::vector< KTFrequencySpectrumFFTW* > fOutputArrays;

            Nymph::KTDataPtr fOutputData;
            KTSliceHeader* fOutputSHData; // pointer to object that is part of fOutputData
            //KTWV2DData* fOutputWVData;
            KTWignerVilleData* fOutputWVData; // pointer to object that is part of fOutputData

            unsigned fWindowAverageCounter;
            unsigned fWindowCounter;
            unsigned fDataOutCounter;
            //unsigned fLeftStartPointer;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fWVSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTEggHeader > fHeaderSlot;
            //Nymph::KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;
            //Nymph::KTSlotDataOneType< KTAnalyticAssociateData > fAnalyticAssociateSlot;

            void ProcessTimeSeries(Nymph::KTDataPtr data);
            void ProcessAnalyticAssociate(Nymph::KTDataPtr data);

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

    inline unsigned KTWignerVille::GetWindowSize() const
    {
        return fWindowSize;
    }

    inline void KTWignerVille::SetWindowSize(unsigned size)
    {
        fWindowSize = size;
        return;
    }

    inline unsigned KTWignerVille::GetWindowStride() const
    {
        return fWindowStride;
    }

    inline void KTWignerVille::SetWindowStride(unsigned stride)
    {
        fWindowStride = stride;
        return;
    }

    inline unsigned KTWignerVille::GetNWindowsToAverage() const
    {
        return fNWindowsToAverage;
    }

    inline void KTWignerVille::SetNWindowsToAverage(unsigned nAvg)
    {
        fNWindowsToAverage = nAvg;
        return;
    }

    inline bool KTWignerVille::GetUseWindowFunction() const
    {
        return fUseWindowFunction;
    }

    inline void KTWignerVille::SetUseWindowFunction(bool flag)
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

    inline KTForwardFFTW* KTWignerVille::GetFFT()
    {
        return fFFT;
    }

    inline const KTForwardFFTW* KTWignerVille::GetFFT() const
    {
        return fFFT;
    }

    template< class XDataType >
    bool KTWignerVille::TransformFFTWBasedData(XDataType& data, KTSliceHeader& header)
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

            unsigned nComponents = data.GetNComponents();
            if (nComponents != fBuffer.size())
            {
                KTERROR(wvlog, "Number of components mismatched between the buffer (" << fBuffer.size() << ") and the data (" << nComponents << ")");
                return false;
            }

            /*
            // cast all time series into KTTimeSeriesFFTW
            std::vector< const KTTimeSeriesFFTW* > timeSeries(nComponents);
            for (unsigned iTS=0; iTS < nComponents; iTS++)
            {
                timeSeries[iTS] = dynamic_cast< const KTTimeSeriesFFTW* >(data.GetTimeSeries(iTS));
                if (timeSeries[iTS] == NULL)
                {
                    KTERROR(wvlog, "Time series " << iTS << " did not cast to a const KTTimeSeriesFFTW*. No transforms performed.");
                    return false;
                }
            }
             */

            unsigned nPairs = fOutputWVData->GetNComponents();

            std::vector< unsigned > preCopyBufferSize(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                preCopyBufferSize[iComponent] = fBuffer[iComponent].size();
                KTDEBUG(wvlog, "Pre-copy buffer " << iComponent << " size: " << preCopyBufferSize[iComponent]);
            }

            // copy the arriving header into fSecondHeader
            fSecondHeader.CopySliceHeaderOnly(header);

            // check if the data that just arrived is from a new acquisition
            bool localIsNewAcquisition = false;
            if (header.GetIsNewAcquisition())
            {
                localIsNewAcquisition = true;
                for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
                {
                    fBuffer[iComponent].clear();
                }
            }

            // iterators to track where to start the next window
            std::vector< Buffer::iterator > windowStartIterator(nComponents);
            // iterators to track where this window ends
            std::vector< Buffer::iterator > endOfCurrentWindow(nComponents);

            // copy the data into the circular buffer
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                const KTTimeSeriesFFTW* ts = static_cast< const KTTimeSeriesFFTW* >(data.GetTimeSeries(iComponent));
                unsigned tsSize = ts->size();
                for (unsigned iBin = 0; iBin < tsSize; ++iBin)
                {
                    fBuffer[iComponent].push_back((*ts)(iBin));
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
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                fSliceBreak[iComponent] = fBuffer[iComponent].begin() + preCopyBufferSize[iComponent];
                KTDEBUG(wvlog, "Slice break " << iComponent << " offset: " << fSliceBreak[iComponent] - fBuffer[iComponent].begin());
            }

            // This is declared outside of the buffer loop so that after the loop we know which slice the last window started in
            bool windowStartInFirstSlice = false;

            // loop over the buffer until we get too close to the end to fit another window
            bool exitBufferLoop = false;
            while (! exitBufferLoop)
            {
                KTDEBUG(wvlog, "Slice sample offset: " << fSliceSampleOffset);

                fOutputWVData->Clear();

                for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
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
                        for (unsigned iPair = 0; iPair < nPairs; ++iPair)
                        {
                            unsigned firstChannel = fPairs[iPair].first;
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
                        for (unsigned iPair = 0; iPair < nPairs; ++iPair)
                        {
                            unsigned firstChannel = fPairs[iPair].first;
                            fOutputSHData->SetTimeStamp(fSecondHeader.GetTimeStampAtSample(fSliceSampleOffset, firstChannel), iPair);
                            fOutputSHData->SetAcquisitionID(fSecondHeader.GetAcquisitionID(firstChannel), iPair);
                            fOutputSHData->SetRecordID(fSecondHeader.GetRecordID(firstChannel), iPair);
                        }
                      }
                }



                // analyze the data in the buffer
                for (unsigned iPair = 0; iPair < nPairs; ++iPair)
                {
                    unsigned firstChannel = fPairs[iPair].first;
                    unsigned secondChannel =  fPairs[iPair].second;

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
                    fOutputData->SetLastData(fReceivedLastData);
                    fOutputData->SetCounter(fDataOutCounter);

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
                    for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
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
                    for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
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
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
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
            unsigned nPairs = fPairs.size();

            //KTWV2DData& newData = data.template Of< KTWV2DData >().SetNComponents(nPairs);
            ////KTWignerVilleData& newData = data.template Of< KTWignerVilleData >().SetNComponents(nPairs);

            // Do WV transform for each pair
            unsigned iPair = 0;
            for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
            {
                unsigned firstChannel = (*pairIt).first;
                unsigned secondChannel = (*pairIt).second;

                //unsigned nOffsets = timeSeries[firstChannel]->size();
                unsigned nOffsets = 896;
                double timeBW = timeSeries[firstChannel]->GetBinWidth();

                newData.SetInputPair(firstChannel, secondChannel, iPair);
                KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* newSpectra = new KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >(nOffsets, -0.5 * timeBW, timeBW * (double(nOffsets) - 0.5));
                for (unsigned iSpectrum = 0; iSpectrum < nOffsets; iSpectrum++)
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

                //for(unsigned offset = 0; offset < nOffsets; offset++)
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
