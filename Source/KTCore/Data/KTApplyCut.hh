/**
 @file KTApplyCut.hh
 @brief Contains KTApplyCut
 @details Applies a cut to data
 @author: N. S. Oblath
 @date: Oct 6, 2014
 */

#ifndef KTAPPLYCUT_HH_
#define KTAPPLYCUT_HH_

#include "KTProcessor.hh"

#include "KTLogger.hh"
#include "KTSlot.hh"

#include <string>

namespace Katydid
{
    class KTCut;
    class KTParamNode;

    /*!
     @class KTApplyCut
     @author N. S. Oblath

     @brief Applies a cut to data.

     @details
     KTApplyCut applies a cut to data -- the type of cut and its parameters are specified at runtime
     from the set of cuts registered.

     Configuration name: "apply-cut"

     Available configuration values:
     - "[cut name]": subtree -- sets the type of window function to be used; parent node for the cut configuration

     Slots:
     - "apply-cut": void (KTDataPtr) -- Applies the cut to the received data; Requirements are set by the cut; No data is added.

     Signals:
     - "after-cut": void (KTDataPtr) -- Emitted upon application of the cut.
    */

    class KTApplyCut : public KTProcessor
    {
        public:
            KTApplyCut(const std::string& name = "apply-cut");
            virtual ~KTApplyCut();

            bool Configure(const KTParamNode* node);

            KTCut* GetCut() const;
            void SetCut(KTCut* cut);
            bool SelectCut(const std::string& cutName);

        private:
            KTCut* fCut;

        public:
            void ApplyCut(KTDataPtr);


            //***************
            // Signals
            //***************

        private:
            KTSignalData fAfterCutSignal;

            //***************
            // Slots
            //***************

        private:

    };


    inline KTCut* KTApplyCut::GetCut() const
    {
        return fCut;
    }

} /* namespace Katydid */
#endif /* KTAPPLYCUT_HH_ */
