
#include "KTLogger.hh"

#include "KTTrackProcessing.hh"
// #include "KTParameterStore.hh"
// #include "KTTestConfigurable.hh"
// #include "KTPStoreNode.hh"


using namespace Katydid;

KTLOGGER(testlog, "TestTrackProcessing");

int main()
{

    KTINFO(testlog, "Finally, a customer!");

    KTTrackProcessing trackProc;
    trackProc.SetTrackProcAlgorithm("weighted-slope");
    trackProc.SetSlopeMinimum(0);
    trackProc.SetProcTrackMinPoints(1);
    trackProc.SetProcTrackAssignedError(12000);

    // KTINFO(testparamlog, "Testing a configurable");

    // KTTestConfigurable* testObj = new KTTestConfigurable();

    // KTPStoreNode configNode = store->GetNode(testObj->GetConfigName());
    // if (! configNode.IsValid())
    // {
    //     KTERROR(testparamlog, "Top-level node <" << testObj->GetConfigName() << "> was not found");
    //     return -1;
    // }

}