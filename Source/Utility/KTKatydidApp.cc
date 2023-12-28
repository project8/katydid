/*
 * KTApplication.cc
 *
 *  Created on: Aug 5, 2012
 *      Author: nsoblath
 */

#include "KTKatydidApp.hh"


namespace Katydid
{
    LOGGER(applog, "KTKatydidApp");

    KTKatydidApp::KTKatydidApp(bool makeTApp) :
            KTApplication()
    {

#ifdef ROOT_FOUND
        fTApp = NULL;
        if (makeTApp)
        {
            fTApp = new TApplication("", 0, 0);
        }
#else
        if (makeTApp)
        {
            LWARN(applog, "TApplication requested, but Nymph has been built without ROOT dependence.");
        }
#endif
    }

    KTKatydidApp::KTKatydidApp(int argC, char** argV, bool makeTApp, bool requireArgs, scarab::param_node* defaultConfig) :
            KTApplication(argC, argV, requireArgs, defaultConfig)
    {
#ifdef ROOT_FOUND
        fTApp = NULL;
        if (makeTApp)
        {
            StartTApplication();
        }
#else
        if (makeTApp)
        {
            LWARN(applog, "TApplication requested, but Nymph has been built without ROOT dependence.");
        }
#endif
    }

    KTKatydidApp::~KTKatydidApp()
    {
#ifdef ROOT_FOUND
        delete fTApp;
#endif
    }



    bool KTKatydidApp::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        if (! KTApplication::Configure(node)) return false;

        if (node->get_value("root-app", false))
        {
#ifdef ROOT_FOUND
            StartTApplication();
#else
            LWARN(applog, "TApplication requested, but Nymph has been built without ROOT dependence.");
#endif
        }
        return true;
    }

#ifdef ROOT_FOUND
    bool KTKatydidApp::StartTApplication()
    {
        if (fTApp != NULL)
            return true;

        fTApp = new TApplication("", 0, 0);

        if (fTApp != NULL)
        {
            return true;
        }
        return false;
    }
#endif

} /* namespace Katydid */
