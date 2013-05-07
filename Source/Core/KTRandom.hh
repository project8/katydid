/*
 * KTRandom.hh
 *
 *  Created on: May 6, 2013
 *      Author: nsoblath
 */

#ifndef KTRANDOM_HH_
#define KTRANDOM_HH_

#include "KTConfigurable.hh"
#include "KTPStoreNode.hh"
#include "KTSingleton.hh"

// the generator that will be used
#include <boost/random/mersenne_twister.hpp>

// for definitions of distributions
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_01.hpp>

namespace Katydid
{
    //**************************************
    // Definition of the RNG engine class
    //**************************************

    class KTRNGEngine : public KTSelfConfigurable
    {
        public:
            typedef boost::random::mt19937 generator_type;

        public:
            KTRNGEngine(const std::string& name = "rng-engine");
            virtual ~KTRNGEngine();

        public:
            using KTSelfConfigurable::Configure;

            virtual Bool_t Configure(const KTPStoreNode* node);
            virtual Bool_t IsReady() const;

            virtual void SetSeed(UInt_t seed);

            generator_type& GetGenerator();

        private:
            generator_type fGenerator;
    };

    inline Bool_t KTRNGEngine::Configure(const KTPStoreNode* node)
    {
        if (node->HasData("seed"))
        {
            SetSeed(node->GetData< UInt_t >("seed"));
        }
        return true;
    }

    inline Bool_t KTRNGEngine::IsReady() const
    {
        return true;
    }

    inline void KTRNGEngine::SetSeed(UInt_t seed)
    {
        fGenerator.seed(seed);
        return;
    }

    inline KTRNGEngine::generator_type& KTRNGEngine::GetGenerator()
    {
        return fGenerator;
    }


    //***************************************
    // Definition of the global RNG engine
    //***************************************

    class KTGlobalRNGEngine : public KTRNGEngine, public KTSingleton< KTGlobalRNGEngine >
    {
        protected:
            friend class KTSingleton< KTGlobalRNGEngine >;
            friend class KTDestroyer< KTGlobalRNGEngine >;
            KTGlobalRNGEngine(const std::string& name = "global-rng-engine") :
                KTRNGEngine(name)
            {}
            virtual ~KTGlobalRNGEngine() {}
    };




    //*********************************************
    // Base class for configurable distributions
    //*********************************************

    template< class Engine >
    class KTRNGDistribution : public KTConfigurable
    {
        public:
            KTRNGDistribution(Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "rng") :
                    KTConfigurable(name),
                    fEngine(rng)
                {}
            virtual ~KTRNGDistribution() {}

            Engine* GetEngine() const;
            void SetEngine(Engine* rng);

        protected:
            Engine* fEngine;

        public:
            virtual Bool_t Configure(const KTPStoreNode* node);
            virtual Bool_t ConfigureDistribution(const KTPStoreNode* node) = 0;

    };

    template< class Engine >
    inline Engine* KTRNGDistribution< Engine >::GetEngine() const
    {
        return fEngine;
    }

    template< class Engine >
    inline void KTRNGDistribution< Engine >::SetEngine(Engine* rng)
    {
        fEngine = rng;
        return;
    }

    template< class Engine >
    inline Bool_t KTRNGDistribution< Engine >::Configure(const KTPStoreNode* node)
    {
        return this->ConfigureDistribution(node);
    }



    //*********************
    // RNG distributions
    //*********************

    template< typename Engine = KTGlobalRNGEngine, typename RealType = Double_t >
    struct KTRNGUniform01 : KTRNGDistribution< Engine >, boost::random::uniform_01<RealType>
    {
        typedef boost::random::uniform_01<RealType> dist_type;
        typedef typename dist_type::result_type result_type;

        KTRNGUniform01(Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "uniform-01") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        virtual ~KTRNGUniform01() {}

        inline result_type operator()() {return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator());}

        inline virtual Bool_t ConfigureDistribution(const KTPStoreNode* node)
        {
            return true;
        }
    };

    template< typename Engine = KTGlobalRNGEngine, typename RealType = Double_t >
    struct KTRNGGaussian : KTRNGDistribution< Engine >, boost::random::normal_distribution<RealType>
    {
        typedef boost::random::uniform_01<RealType> dist_type;
        typedef typename dist_type::result_type result_type;

        KTRNGGaussian(Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "gaussian") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        virtual ~KTRNGGaussian() {}

        inline result_type operator()()
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator());
        }
        inline result_type operator()(RealType mean, RealType sigma)
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator(), param_type(mean, sigma));
        }

        inline virtual Bool_t ConfigureDistribution(const KTPStoreNode* node)
        {
            RealType mean = node->GetData< RealType >("mean", this->mean());
            RealType sigma = node->GetData< RealType >("sigma", this->sigma());
            this->param(param_type(mean, sigma));
            return true;
        }
    };


} /* namespace Katydid */
#endif /* KTRANDOM_HH_ */
