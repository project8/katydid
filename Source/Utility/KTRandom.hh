/*
 * KTRandom.hh
 *
 *  Created on: May 6, 2013
 *      Author: nsoblath
 */

#ifndef KTRANDOM_HH_
#define KTRANDOM_HH_

#include "Service.hh"

#include "logger.hh"

#include "param.hh"
#include "singleton.hh"

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
    
    LOGGER(rnglog, "KTRandom");

    //**************************************
    // Definition of the RNG engine class
    //**************************************

    class KTRNGEngine : public Nymph::Service
    {
        public:
            typedef boost::random::mt19937 generator_type;

        public:
            KTRNGEngine(const std::string& name = "rng-engine");
            virtual ~KTRNGEngine();

        public:
            virtual void Configure(const scarab::param_node& node);

            virtual void SetSeed(unsigned seed);

            generator_type& GetGenerator();

        private:
            generator_type fGenerator;
    };

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

    class KTGlobalRNGEngine : public KTRNGEngine, public scarab::singleton< KTGlobalRNGEngine >
    {
        protected:
            friend class scarab::singleton< KTGlobalRNGEngine >;
            friend class scarab::destroyer< KTGlobalRNGEngine >;
            KTGlobalRNGEngine(const std::string& name = "global-rng-engine") :
                KTRNGEngine(name)
            {}
            virtual ~KTGlobalRNGEngine() {}
    };




    //*********************************************
    // Base class for configurable distributions
    //*********************************************

    template< class Engine >
    class KTRNGDistribution
    {
        public:
            KTRNGDistribution(Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "rng") :
                    fEngine(rng)
                {}
            virtual ~KTRNGDistribution() {}

            Engine* GetEngine() const;
            void SetEngine(Engine* rng);

        protected:
            Engine* fEngine;

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
    template< typename Engine = KTRNGEngine, typename RealType = double >
    struct KTRNGUniform01 : KTRNGDistribution< Engine >, boost::random::uniform_01<RealType>
    {
        typedef boost::random::uniform_01<RealType> dist_type;
        typedef typename dist_type::result_type result_type;

        KTRNGUniform01(Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "uniform-01") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        virtual ~KTRNGUniform01() {}

        inline result_type operator()() {return dist_type::operator()(KTRNGDistribution< Engine >::fEngine->GetGenerator());}
    };

    /*!
     @class KTRNGUniform
     @author N. S. Oblath

     @brief Continuous uniform distribution on the range [min, max)

     @details
     Returns a floating point value distributed in the range [min, max)

     Distribution parameters:
       - Minimum for the uniform distribution range (inclusive)
       - Maximum for the uniform distribution range (exclusive)

     Set the parameters with: `this->param(param_type([min], [max]))`
    */
    template< typename Engine = KTRNGEngine, typename RealType = double >
    struct KTRNGUniform : KTRNGDistribution< Engine >, boost::random::uniform_real_distribution<RealType>
    {
        typedef boost::random::uniform_real_distribution<RealType> dist_type;
        typedef typename dist_type::input_type input_type;
        typedef typename dist_type::result_type result_type;
        typedef typename dist_type::param_type param_type;

        KTRNGUniform(Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "uniform") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        KTRNGUniform(input_type min, input_type max, Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "uniform") :
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
    };

    /*!
     @class KTRNGGaussian
     @author N. S. Oblath

     @brief Gaussian distribution with parameters "mean" and "sigma"

     @details
     Returns a floating point value distributed according to a Gaussian distribution with parameters \f$\mu \equiv\f$ "mean" and \f$\sigma \equiv\f$ "sigma."

     The PDF for the distribution is: \f$ p(x) = \frac{1}{\sqrt{2\pi\sigma}} \exp{-\frac{(x-\mu)^2}{2\sigma^2}} \f$

     Distribution parameters
       - Mean of the Gaussian distribution, mu
       - Standard deviation of the Gaussian distribution, sigma

     Set the parameters with: `this->param(param_type([mean], [stddev]))`
    */
    template< typename Engine = KTRNGEngine, typename RealType = double >
    struct KTRNGGaussian : KTRNGDistribution< Engine >, boost::random::normal_distribution<RealType>
    {
        typedef boost::random::normal_distribution<RealType> dist_type;
        typedef typename dist_type::input_type input_type;
        typedef typename dist_type::result_type result_type;
        typedef typename dist_type::param_type param_type;

        KTRNGGaussian(Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "gaussian") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        KTRNGGaussian(input_type mean, input_type sigma, Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "gaussian") :
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
    };

    /*!
     @class KTRNGPoisson
     @author N. S. Oblath

     @brief Poisson distribution with parameter "mean"

     @details
     Returns an integer value distributed according to a Poisson distribution with parameter \f$\lambda \equiv\f$ "mean."

     The PDF for the distribution is: \f$ p(i) = \frac{exp{-\lambda}\lambda^i}{i!} \f$

     Distribution parameter:
       - Mean of the Poisson distribution, lambda
    
     Set the parameter with: `this->param(param_type([lambda]))`
    */
    template< typename Engine = KTRNGEngine, typename IntType = int, typename RealType = double >
    struct KTRNGPoisson : KTRNGDistribution< Engine >, boost::random::poisson_distribution<IntType, RealType>
    {
        typedef boost::random::poisson_distribution<IntType, RealType> dist_type;
        typedef typename dist_type::input_type input_type;
        typedef typename dist_type::result_type result_type;
        typedef typename dist_type::param_type param_type;

        KTRNGPoisson(Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "poisson") :
            KTRNGDistribution< Engine >(KTGlobalRNGEngine::get_instance())
        {}
        KTRNGPoisson(input_type mean, Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "poisson") :
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
    };

    /*!
     @class KTRNGExponential
     @author N. S. Oblath

     @brief Exponential distribution with parameter "lambda"

     @details
     Returns a floating point value distributed according to an exponential distribution with parameter \f$\lambda \equiv\f$ "lambda."

     The PDF for the distribution is: \f$ p(x) = \lambda\exp{-\lambda{}x} \f$

     Distribution parameter:
       - Rate parameter of the exponential distribution, lambda

     Set the parameter with: `this->param(param_type([lambda]))`
    */
    template< typename Engine = KTRNGEngine, typename RealType = double >
    struct KTRNGExponential : KTRNGDistribution< Engine >, boost::random::exponential_distribution<RealType>
    {
        typedef boost::random::exponential_distribution<RealType> dist_type;
        typedef typename dist_type::input_type input_type;
        typedef typename dist_type::result_type result_type;
        typedef typename dist_type::param_type param_type;

        KTRNGExponential(Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "exponential") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        KTRNGExponential(input_type lambda, Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "exponential") :
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
    };

    /*!
     @class KTRNGChiSquared
     @author N. S. Oblath

     @brief Chi-squared distribution with parameter "n"

     @details
     Returns a floating point value distributed according to an chi-squared distribution with parameter \f$n\f$.

     The PDF for the distribution is: \f$ p(x) = \frac{x^{n/2-1}\exp{-x/2}}{\Gamma(n/2)2^{n/2}} \f$

     Distribution parameter:
       - Degree of the chi-squared function, n

     Set the parameter with: `this->param(param_type([n]))`
    */
    template< typename Engine = KTRNGEngine, typename RealType = double >
    struct KTRNGChiSquared : KTRNGDistribution< Engine >, boost::random::chi_squared_distribution<RealType>
    {
        typedef boost::random::chi_squared_distribution<RealType> dist_type;
        typedef typename dist_type::input_type input_type;
        typedef typename dist_type::result_type result_type;
        typedef typename dist_type::param_type param_type;

        KTRNGChiSquared(Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "chi-squared") :
            KTRNGDistribution< Engine >(rng, name)
        {}
        KTRNGChiSquared(input_type n, Engine* rng = KTGlobalRNGEngine::get_instance(), const std::string& name = "chi-squared") :
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

        inline virtual void ConfigureDistribution(const scarab::param_node& node)
        {
            input_type n = node.get_value("n", this->n());
            this->param(param_type(n));
            return;
        }
    };

} /* namespace Katydid */
#endif /* KTRANDOM_HH_ */
