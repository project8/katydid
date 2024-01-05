/*
 * KTConfigurable.hh
 *
 *  Created on: Aug 21, 2012
 *      Author: nsoblath
 */

#ifndef KTCONFIGURABLE_HH_
#define KTCONFIGURABLE_HH_

#include "param.hh"

#include <string>

namespace Nymph
{
    class KTCommandLineHandler;


    //******************
    // KTConfigurable
    //******************

    class KTConfigurable
    {
        public:
            KTConfigurable(const std::string& name="default-name");
            virtual ~KTConfigurable();

            /// Should perform parameter store and command-line configurations
            virtual bool Configure(const scarab::param_node& node) = 0;
            /// Implement the option for calling Configure without passing a parameter store node.
            //virtual bool Configure();
            /// Configure from a JSON string
            bool Configure(const std::string& config);

        protected:
            KTCommandLineHandler* fCLHandler;

        public:
            const std::string& GetConfigName() const;
            void SetConfigName(const std::string& name);

        protected:
            std::string fConfigName;

    };
/*
    inline bool KTConfigurable::Configure()
    {
        return Configure(NULL);
    }
*/
    inline const std::string& KTConfigurable::GetConfigName() const
    {
        return fConfigName;
    }

    inline void KTConfigurable::SetConfigName(const std::string& name)
    {
        fConfigName = name;
    }


    //**********************
    // KTSelfConfigurable
    //**********************

    class KTSelfConfigurable : public KTConfigurable
    {
        public:
            KTSelfConfigurable(const std::string& name="default-self-name");
            virtual ~KTSelfConfigurable();

            /// Should perform parameter store and command-line configurations
            virtual bool Configure(const scarab::param_node& node) = 0;
            /// Configure by getting the top-level node from the parameter store
            virtual bool Configure();

            /// Check if everything has been correctly configured
            virtual bool IsReady() const = 0;

            bool GetIsConfigured();

        private:
            bool fIsConfigured;

    };

    inline bool KTSelfConfigurable::GetIsConfigured()
    {
        return fIsConfigured;
    }

} /* namespace Nymph */
#endif /* KTCONFIGURABLE_HH_ */
