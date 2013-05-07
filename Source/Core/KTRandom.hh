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




    /*
    Below is the attempt to build the RNG system with a configurable engine type

    class KTRNGEngine : public KTSelfConfigurable
    {
        public:
            struct KTEngineCore
            {
                virtual ~KTEngineCore() {}
                virtual void SetSeed(UInt_t seed) = 0;
                virtual void SetSeed64(ULong64_t seed) = 0;
            };

            template< class EngineType >
            struct KTEngineCoreWrapper : KTEngineCore
            {
                KTEngineCoreWrapper() :
                    KTEngineCore(),
                    fEngine()
                {}
                template< typename SeedType >
                KTEngineCoreWrapper(SeedType seed) :
                    KTEngineCore(),
                    fEngine(seed)
                {}
                virtual ~KTEngineCoreWrapper() {}

                virtual void SetSeed(UInt_t seed) {fEngine.seed(seed);}
                virtual void SetSeed64(ULong64_t seed) {fEngine.seed(seed);}

                EngineType fEngine;
            };


        public:
            KTRNGEngine(const std::string& name = "rng-engine");
            virtual ~KTRNGEngine();

        public:
            using KTSelfConfigurable::Configure;

            virtual Bool_t Configure(const KTPStoreNode* node);
            virtual Bool_t IsReady() const;

            virtual void SetSeed(UInt_t seed);
            virtual void SetSeed64(ULong64_t seed);

            KTEngineCore* GetEngineCore() const;
            void SetEngineCore(KTEngineCore* core);

        private:
            KTEngineCore* fCore;

        public:
            template< typename Distribution >
            typename Distribution::result_type operator()(const Distribution& dist);
    };

    inline Bool_t KTRNGEngine::IsReady() const
    {
        if (fCore == NULL) return false;
        return true;
    }

    inline KTRNGEngine::KTEngineCore* KTRNGEngine::GetEngineCore() const
    {
        return fCore;
    }

    inline void KTRNGEngine::SetEngineCore(KTEngineCore* core)
    {
        fCore = core;
        return;
    }

    inline void KTRNGEngine::SetSeed(UInt_t seed)
    {
        fCore->SetSeed(seed);
        return;
    }

    inline void KTRNGEngine::SetSeed64(ULong64_t seed)
    {
        fCore->SetSeed64(seed);
        return;
    }


    //**********************************
    // Definitions of engine wrappers
    //**********************************

    struct KTMT19937Wrapper : KTRNGEngine::KTEngineCoreWrapper< boost::random::mt19937 >
    {
        KTMT19937Wrapper() {}
        KTMT19937Wrapper(boost::random::mt19937::result_type seed) :
            KTRNGEngine::KTEngineCoreWrapper< boost::random::mt19937 >(seed) {}
        virtual ~KTMT19937Wrapper() {}
    };


    //****************************************
    // Definitions of the global RNG engine
    //****************************************

    class KTGlobalRNGEngine : public KTRNGEngine, public KTSingleton< KTGlobalRNGEngine >
    {
        protected:
            friend class KTSingleton< KTGlobalRNGEngine >;
            friend class KTDestroyer< KTGlobalRNGEngine >;
            KTGlobalRNGEngine(const std::string& name = "global-rng-engine");
            virtual ~KTGlobalRNGEngine();
    };




    template< class RNG >
    class KTRNGConfigurator : public KTConfigurable
    {
        public:
            KTRNGConfigurator(const std::string& name = "rng", RNG* rng = KTGlobalRNGEngine::GetInstance()) :
                KTConfigurable(name),
                fRNG(rng)
            {}
            virtual ~KTRNGConfigurator() {}

            RNG* GetRNG() const;

        private:
            RNG* fRNG;

        public:
            Bool_t Configure(const KTPStoreNode* node) {return true;}

    };


    template< typename RealType = Double_t >
    struct KTRndmUniform01 : boost::random::uniform_01<RealType>, KTRNGConfigurator< >
    {
        typedef typename boost::random::uniform_01<RealType>::result_type result_type;

        KTRndmUniform01(const std::string& name = "uniform-01", RNG* rng = KTGlobalRNGEngine::GetInstance()) :
            KTRNGConfigurator(name)

        result_type operator()() {return *this(*KTGlobalRNGEngine::GetInstance());}
    };


    template< class RNG >
    RNG* KTRNGConfigurator< RNG >::GetRNG() const
    {
        return fRNG;
    }

     */

} /* namespace Katydid */
#endif /* KTRANDOM_HH_ */
