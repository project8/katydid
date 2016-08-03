/*
 * KTRandom.hh
 *
 *  Created on: May 6, 2013
 *      Author: nsoblath
 */

#ifndef KTRANDOM_HH_
#define KTRANDOM_HH_

#include "KTConfigurable.hh"
#include "KTSingleton.hh"

#include "KTLogger.hh"

#include "param.hh"

// the generator that will be used
#include <boost/random/mersenne_twister.hpp>

// for definitions of distributions
#include <boost/random/chi_squared_distribution.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_real_distribution.hpp>

namespace Katydid
{
    using namespace Nymph;
    KTLOGGER(rnglog, "KTRandom");

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

            virtual bool Configure(const scarab::param_node* node);
            virtual bool IsReady() const;

            virtual void SetSeed(unsigned seed);

            generator_type& GetGenerator();

        private:
            generator_type fGenerator;
    };

    inline bool KTRNGEngine::IsReady() const
    {
        return true;
    }

    inline void KTRNGEngine::SetSeed(unsigned seed)
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
            virtual bool Configure(const scarab::param_node* node);
            virtual bool ConfigureDistribution(const scarab::param_node* node) = 0;

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
    inline bool KTRNGDistribution< Engine >::Configure(const scarab::param_node* node)
    {
        return this->ConfigureDistribution(node);
    }



    //*********************
    // RNG distributions
    //*********************

    /*!
     @class KTRNGUniform01
     @author N. S. Oblath

     @brief Continuous uniform distribution on the range [0, 1)

     @details
     Returns a floating point number on the range [0, 1)

     Available configuration options:
       N/A
    */
    template< typename Engine = KTGlobalRNGEngine, typename RealType = double >
    struct KTRNGUniform01 : KTRNGDistribution< Engine >, boost::random::uniform_01<RealType>
    {
        typedef boost::random::uniform_01<RealType> dist_type;
        typedef typename dist_type::result_type result_type;

        KTRNGUniform01(Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "uniform-01") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        virtual ~KTRNGUniform01() {}

        inline result_type operator()() {return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator());}

        inline virtual bool ConfigureDistribution(const scarab::param_node*)
        {
            return true;
        }
    };

    /*!
     @class KTRNGUniform
     @author N. S. Oblath

     @brief Continuous uniform distribution on the range [min, max)

     @details
     Returns a floating point value distributed in the range [min, max)

     Available configuration options:
       - "min": double -- Minimum for the uniform distribution range (inclusive)
       - "max": double -- Maximum for the uniform distribution range (exclusive)
    */
    template< typename Engine = KTGlobalRNGEngine, typename RealType = double >
    struct KTRNGUniform : KTRNGDistribution< Engine >, boost::random::uniform_real_distribution<RealType>
    {
        typedef boost::random::uniform_real_distribution<RealType> dist_type;
        typedef typename dist_type::input_type input_type;
        typedef typename dist_type::result_type result_type;
        typedef typename dist_type::param_type param_type;

        KTRNGUniform(Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "uniform") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        KTRNGUniform(input_type min, input_type max, Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "uniform") :
            KTRNGDistribution< Engine >(rng, name),
            dist_type(min, max)
        {}
        virtual ~KTRNGUniform() {}

        inline result_type operator()()
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator());
        }
        inline result_type operator()(input_type min, input_type max)
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator(), param_type(min, max));
        }

        inline virtual bool ConfigureDistribution(const scarab::param_node* node)
        {
            input_type min = node->get_value< input_type >("min", this->a());
            input_type max = node->get_value< input_type >("max", this->b());
            this->param(param_type(min, max));
            return true;
        }
    };

    /*!
     @class KTRNGGaussian
     @author N. S. Oblath

     @brief Gaussian distribution with parameters "mean" and "sigma"

     @details
     Returns a floating point value distributed according to a Gaussian distribution with parameters \f$\mu \equiv\f$ "mean" and \f$\sigma \equiv\f$ "sigma."

     The PDF for the distribution is: \f$ p(x) = \frac{1}{\sqrt{2\pi\sigma}} \exp{-\frac{(x-\mu)^2}{2\sigma^2}} \f$

     Available configuration options:
       - "mean": double -- Mean of the Gaussian distribution
       - "sigma": double -- Standard deviation of the Gaussian distribution
    */
    template< typename Engine = KTGlobalRNGEngine, typename RealType = double >
    struct KTRNGGaussian : KTRNGDistribution< Engine >, boost::random::normal_distribution<RealType>
    {
        typedef boost::random::normal_distribution<RealType> dist_type;
        typedef typename dist_type::input_type input_type;
        typedef typename dist_type::result_type result_type;
        typedef typename dist_type::param_type param_type;

        KTRNGGaussian(Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "gaussian") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        KTRNGGaussian(input_type mean, input_type sigma, Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "gaussian") :
            KTRNGDistribution< Engine >(rng, name),
            dist_type(mean, sigma)
        {}
        virtual ~KTRNGGaussian() {}

        inline result_type operator()()
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator());
        }
        inline result_type operator()(input_type mean, input_type sigma)
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator(), param_type(mean, sigma));
        }

        inline virtual bool ConfigureDistribution(const scarab::param_node* node)
        {
            input_type mean = node->get_value< input_type >("mean", this->mean());
            input_type sigma = node->get_value< input_type >("sigma", this->sigma());
            this->param(param_type(mean, sigma));
            return true;
        }
    };

    /*!
     @class KTRNGPoisson
     @author N. S. Oblath

     @brief Poisson distribution with parameter "mean"

     @details
     Returns an integer value distributed according to a Poisson distribution with parameter \f$\lambda \equiv\f$ "mean."

     The PDF for the distribution is: \f$ p(i) = \frac{exp{-\lambda}\lambda^i}{i!} \f$

     Available configuration options:
       - "mean": double -- Mean of the Poisson distribution
    */
    template< typename Engine = KTGlobalRNGEngine, typename IntType = int, typename RealType = double >
    struct KTRNGPoisson : KTRNGDistribution< Engine >, boost::random::poisson_distribution<IntType, RealType>
    {
        typedef boost::random::poisson_distribution<IntType, RealType> dist_type;
        typedef typename dist_type::input_type input_type;
        typedef typename dist_type::result_type result_type;
        typedef typename dist_type::param_type param_type;

        KTRNGPoisson(Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "poisson") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        KTRNGPoisson(input_type mean, Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "poisson") :
            KTRNGDistribution< Engine >(rng, name),
            dist_type(mean)
        {}
        virtual ~KTRNGPoisson() {}

        inline result_type operator()()
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator());
        }
        inline result_type operator()(input_type mean)
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator(), param_type(mean));
        }

        inline virtual bool ConfigureDistribution(const scarab::param_node* node)
        {
            input_type mean = node->get_value< input_type >("mean", this->mean());
            this->param(param_type(mean));
            return true;
        }
    };

    /*!
     @class KTRNGExponential
     @author N. S. Oblath

     @brief Exponential distribution with parameter "lambda"

     @details
     Returns a floating point value distributed according to an exponential distribution with parameter \f$\lambda \equiv\f$ "lambda."

     The PDF for the distribution is: \f$ p(x) = \lambda\exp{-\lambda{}x} \f$

     Available configuration options:
       - "lambda": double -- rate parameter of the exponential distribution
    */
    template< typename Engine = KTGlobalRNGEngine, typename RealType = double >
    struct KTRNGExponential : KTRNGDistribution< Engine >, boost::random::exponential_distribution<RealType>
    {
        typedef boost::random::exponential_distribution<RealType> dist_type;
        typedef typename dist_type::input_type input_type;
        typedef typename dist_type::result_type result_type;
        typedef typename dist_type::param_type param_type;

        KTRNGExponential(Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "exponential") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        KTRNGExponential(input_type lambda, Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "exponential") :
            KTRNGDistribution< Engine >(rng, name),
            dist_type(lambda)
        {}
        virtual ~KTRNGExponential() {}

        inline result_type operator()()
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator());
        }
        inline result_type operator()(input_type lambda)
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator(), param_type(lambda));
        }

        inline virtual bool ConfigureDistribution(const scarab::param_node* node)
        {
            input_type lambda = node->get_value< input_type >("lambda", this->lambda());
            this->param(param_type(lambda));
            return true;
        }
    };

    /*!
     @class KTRNGChiSquared
     @author N. S. Oblath

     @brief Chi-squared distribution with parameter "n"

     @details
     Returns a floating point value distributed according to an chi-squared distribution with parameter \f$n\f$.

     The PDF for the distribution is: \f$ p(x) = \frac{x^{n/2-1}\exp{-x/2}}{\Gamma(n/2)2^{n/2}} \f$

     Available configuration options:
       - "lambda": double -- rate parameter of the exponential distribution
    */
    template< typename Engine = KTGlobalRNGEngine, typename RealType = double >
    struct KTRNGChiSquared : KTRNGDistribution< Engine >, boost::random::chi_squared_distribution<RealType>
    {
        typedef boost::random::chi_squared_distribution<RealType> dist_type;
        typedef typename dist_type::input_type input_type;
        typedef typename dist_type::result_type result_type;
        typedef typename dist_type::param_type param_type;

        KTRNGChiSquared(Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "chi-squared") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        KTRNGChiSquared(input_type n, Engine* rng = KTGlobalRNGEngine::GetInstance(), const std::string& name = "chi-squared") :
            KTRNGDistribution< Engine >(rng, name),
            dist_type(n)
        {}
        virtual ~KTRNGChiSquared() {}

        inline result_type operator()()
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator());
        }
        inline result_type operator()(input_type n)
        {
            return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator(), param_type(n));
        }

        inline virtual bool ConfigureDistribution(const scarab::param_node* node)
        {
            input_type n = node->get_value< input_type >("n", this->n());
            this->param(param_type(n));
            return true;
        }
    };

} /* namespace Katydid */
#endif /* KTRANDOM_HH_ */
