/*
 * KTRandom.cc
 *
 *  Created on: May 6, 2013
 *      Author: nsoblath
 */

#include "KTRandom.hh"

#include "KTFactory.hh"
//#include "KTLogger.hh"

using std::string;

namespace Katydid
{
    //KTLOGGER(rnglog, "katydid.core");

    KTRNGEngine::KTRNGEngine(const string& name) :
            KTSelfConfigurable(name),
            fGenerator()
    {
    }

    KTRNGEngine::~KTRNGEngine()
    {
    }



    /*

    //*********************
    // Global RNG Engine
    //*********************

    //static KTRNGEngine gRNG("global-rng");



    //*****************************
    // Registrars for generators
    //*****************************

    static KTDerivedRegistrar< KTRNGEngine::KTEngineCore, KTMT19937Wrapper > sMT19937Registrar("mersenne-twister");



    //*********************************
    // Implementation of KTRNGEngine
    //*********************************

    KTRNGEngine::KTRNGEngine(const string& name) :
            KTSelfConfigurable(name),
            fCore(NULL)
    {
    }

    KTRNGEngine::~KTRNGEngine()
    {
        delete fCore;
    }

    Bool_t KTRNGEngine::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("generator"))
        {
            fCore = KTFactory< KTEngineCore >::GetInstance()->Create(node->GetData< string >("generator"));
            if (fCore == NULL)
            {
                KTERROR(rnglog, "Unable to create an RNG of type <" << node->GetData< string >("generator") << ">");
                return false;
            }
        }

        if (node->HasData("seed"))
        {
            fCore->SetSeed(node->GetData< UInt_t >("seed"));
        }
        if (node->HasData("seed-64"))
        {
            fCore->SetSeed64(node->GetData< ULong64_t >("seed"));
        }

        return true;
    }

    */


} /* namespace Katydid */
