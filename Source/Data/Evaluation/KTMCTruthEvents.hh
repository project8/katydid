/*
 * KTMCTruthElectrons.hh
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#ifndef KTMCTRUTHELECTRONS_HH_
#define KTMCTRUTHELECTRONS_HH_

#include "KTData.hh"

#include <set>

namespace Katydid
{

    class KTMCTruthElectrons : public KTExtensibleData< KTMCTruthElectrons >
    {
        public:
            struct Electron
            {
                Double_t fStartTime;
                Double_t fEndTime;

                Electron(Double_t startTime, double_t endTime)
                {
                    fStartTime = startTime;
                    fEndTime = endTime;
                }
            };

        public:
            KTMCTruthElectrons();
            virtual ~KTMCTruthElectrons();

            const std::set< Electron >& GetElectrons() const;
            void AddElectron(const Electron& electron);
            void ClearElectrons();

        protected:
            std::set< Electron > fElectrons;
    };

    inline const std::set< KTMCTruthElectrons::Electron >& KTMCTruthElectrons::GetElectrons() const
    {
        return fElectrons;
    }

    inline void KTMCTruthElectrons::AddElectron(const Electron& electron)
    {
        fElectrons.insert(electron);
        return;
    }

    inline void KTMCTruthElectrons::ClearElectrons()
    {
        fElectrons.clear();
        return;
    }

} /* namespace Katydid */
#endif /* KTMCTRUTHELECTRONS_HH_ */
