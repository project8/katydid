/*
* TestDBSCanTrackClustering.cc
*
* Created on: Jun 29, 2014
*     Author: nsoblath
*/

#include "KTKDTreeData.hh"
#include "KTDBScanTrackClustering.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTSparseWaterfallCandidateData.hh"

#include <vector>

#ifdef ROOT_FOUND
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#endif


using namespace std;
using namespace Katydid;

KTLOGGER(testlog, "TestDBScanTrackClustering");

int main()
{
    vector< double > times, freqs, amplitudes;
    times.push_back(0.0701605); freqs.push_back(54155479.); amplitudes.push_back(0.1539345);
    times.push_back(0.0701605); freqs.push_back(57206492.); amplitudes.push_back(0.1813601);
    times.push_back(0.0702588); freqs.push_back(57694654.); amplitudes.push_back(0.1948037);
    times.push_back(0.0702588); freqs.push_back(58091286.); amplitudes.push_back(0.1427045);
    times.push_back(0.0702916); freqs.push_back(54155479.); amplitudes.push_back(0.2071948);
    times.push_back(0.0702916); freqs.push_back(55558945.); amplitudes.push_back(0.2166364);
    times.push_back(0.0703244); freqs.push_back( 55192824); amplitudes.push_back(0.1670219);
    times.push_back(0.0704554); freqs.push_back(56748840.); amplitudes.push_back(0.1490871);
    times.push_back(0.0705210); freqs.push_back(56443739.); amplitudes.push_back(0.1782734);
    times.push_back(0.0705210); freqs.push_back(58304857.); amplitudes.push_back(0.1644187);
    times.push_back(0.0705538); freqs.push_back(58335367.); amplitudes.push_back(0.2042647);
    times.push_back(0.0705865); freqs.push_back(55986087.); amplitudes.push_back(0.1480446);
    times.push_back(0.0706521); freqs.push_back(55314864.); amplitudes.push_back(0.1507039);
    times.push_back(0.0706521); freqs.push_back(56657310.); amplitudes.push_back(0.1486019);
    times.push_back(0.0706521); freqs.push_back(58365877.); amplitudes.push_back(0.1999282);
    times.push_back(0.0706848); freqs.push_back(55558945.); amplitudes.push_back(0.1339484);
    times.push_back(0.0706848); freqs.push_back(58396387.); amplitudes.push_back(0.1685721);
    times.push_back(0.0707176); freqs.push_back(54613131.); amplitudes.push_back(0.2188217);
    times.push_back(0.0707504); freqs.push_back(53850378.); amplitudes.push_back(0.1289901);
    times.push_back(0.0707831); freqs.push_back(53697827.); amplitudes.push_back(0.1929480);
    times.push_back(0.0708159); freqs.push_back(54796192.); amplitudes.push_back(0.1887474);
    times.push_back(0.0708159); freqs.push_back(58670978.); amplitudes.push_back(0.1576355);
    times.push_back(0.0708159); freqs.push_back(59250671.); amplitudes.push_back(0.1382071);
    times.push_back(0.0708487); freqs.push_back(56108127.); amplitudes.push_back(0.1590815);
    times.push_back(0.0708814); freqs.push_back(53789358.); amplitudes.push_back(0.1382211);
    times.push_back(0.0709142); freqs.push_back(55162313.); amplitudes.push_back(0.1794486);
    times.push_back(0.0709470); freqs.push_back(53026604.); amplitudes.push_back(0.1932958);
    times.push_back(0.0709470); freqs.push_back( 56138638); amplitudes.push_back(0.1803242);
    times.push_back(0.0709470); freqs.push_back(57603124.); amplitudes.push_back(0.1508648);
    times.push_back(0.0709797); freqs.push_back(56443739.); amplitudes.push_back(0.1588424);
    times.push_back(0.0710125); freqs.push_back(53667317.); amplitudes.push_back(0.1536725);
    times.push_back(0.0710453); freqs.push_back(54460580.); amplitudes.push_back(0.1362641);
    times.push_back(0.0710780); freqs.push_back(54369050.); amplitudes.push_back(0.1211753);
    times.push_back(0.0710780); freqs.push_back(56199658.); amplitudes.push_back(0.1952479);
    times.push_back(0.0710780); freqs.push_back(57969245.); amplitudes.push_back(0.1817430);
    times.push_back(0.0711108); freqs.push_back(56962411.); amplitudes.push_back(0.1532504);
    times.push_back(0.0711108); freqs.push_back(59860873.); amplitudes.push_back(0.1434256);
    times.push_back(0.0711436); freqs.push_back(56230168.); amplitudes.push_back(0.1762948);
    times.push_back(0.0711763); freqs.push_back(56199658.); amplitudes.push_back(0.2626532);
    times.push_back(0.0712091); freqs.push_back(56260678.); amplitudes.push_back(0.1767040);
    times.push_back(0.0712419); freqs.push_back(59616792.); amplitudes.push_back(0.1531586);
    times.push_back(0.0713074); freqs.push_back(56291188.); amplitudes.push_back(0.1583399);
    times.push_back(0.0713074); freqs.push_back(59098120.); amplitudes.push_back(0.1301671);
    times.push_back(0.0713402); freqs.push_back(53758847.); amplitudes.push_back(0.1441528);
    times.push_back( 0.071373); freqs.push_back(55955577.); amplitudes.push_back(0.2380888);
    times.push_back(0.0714057); freqs.push_back(54918232.); amplitudes.push_back(0.1496170);
    times.push_back(0.0714385); freqs.push_back(56352208.); amplitudes.push_back(0.2014921);
    times.push_back(0.0714385); freqs.push_back(59860873.); amplitudes.push_back(0.1574734);
    times.push_back(0.0714713); freqs.push_back(56352208.); amplitudes.push_back(0.1876973);
    times.push_back(0.0715040); freqs.push_back(56382719.); amplitudes.push_back(0.2104633);
    times.push_back(0.0715368); freqs.push_back(56382719.); amplitudes.push_back(0.2575980);
    times.push_back(0.0716023); freqs.push_back(53514766.); amplitudes.push_back(0.1831124);
    times.push_back(0.0716023); freqs.push_back(55742006.); amplitudes.push_back(0.1554094);
    times.push_back(0.0716679); freqs.push_back(57999755.); amplitudes.push_back(0.1493540);
    times.push_back(0.0717006); freqs.push_back(55131803.); amplitudes.push_back(0.1387128);
    times.push_back(0.0717006); freqs.push_back(55803026.); amplitudes.push_back(0.1470704);
    times.push_back(0.0717334); freqs.push_back(56169148.); amplitudes.push_back(0.1357251);
    times.push_back(0.0717334); freqs.push_back(56474249.); amplitudes.push_back(0.1883933);
    times.push_back(0.0717662); freqs.push_back(58670978.); amplitudes.push_back(0.1871909);
    times.push_back(0.0717662); freqs.push_back( 59433732); amplitudes.push_back(0.1919887);
    times.push_back(0.0717989); freqs.push_back(59586282.); amplitudes.push_back(0.1636799);
    times.push_back(0.0718317); freqs.push_back(56504759.); amplitudes.push_back(0.1529294);
    times.push_back(0.0718972); freqs.push_back(56535269.); amplitudes.push_back(0.3338164);
    times.push_back(0.0718972); freqs.push_back(57298022.); amplitudes.push_back(0.1441025);
    times.push_back(0.0719628); freqs.push_back(53819868.); amplitudes.push_back(0.1268521);
    times.push_back(0.0719628); freqs.push_back(58640468.); amplitudes.push_back(0.1358912);
    times.push_back(0.0719955); freqs.push_back(56565779.); amplitudes.push_back(0.2054396);
    times.push_back(0.0720283); freqs.push_back( 56596290); amplitudes.push_back(0.1651692);
    times.push_back(0.0720611); freqs.push_back( 56596290); amplitudes.push_back(0.2014061);
    times.push_back(0.0720611); freqs.push_back(57999755.); amplitudes.push_back(0.1535717);
    times.push_back(0.0720938); freqs.push_back( 56596290); amplitudes.push_back(0.1453571);
    times.push_back(0.0720938); freqs.push_back(56626800.); amplitudes.push_back(0.1708352);
    times.push_back(0.0721266); freqs.push_back(59952404.); amplitudes.push_back(0.1925413);
    times.push_back(0.0721922); freqs.push_back(56657310.); amplitudes.push_back(0.2277085);
    times.push_back(0.0721922); freqs.push_back(59037100.); amplitudes.push_back(0.1432512);
    times.push_back(0.0722249); freqs.push_back(56657310.); amplitudes.push_back(0.2180663);
    times.push_back(0.0722577); freqs.push_back(54613131.); amplitudes.push_back(0.1594814);
    times.push_back(0.0722905); freqs.push_back(56687820.); amplitudes.push_back(0.1968605);
    times.push_back(0.0723232); freqs.push_back(58152306.); amplitudes.push_back(0.1564495);
    times.push_back(0.0723560); freqs.push_back(53362216.); amplitudes.push_back(0.1697344);
    times.push_back(0.0723888); freqs.push_back(57694654.); amplitudes.push_back(0.1620043);
    times.push_back(0.0724215); freqs.push_back(54613131.); amplitudes.push_back(0.1372566);
    times.push_back(0.0724215); freqs.push_back(56748840.); amplitudes.push_back(0.3924053);
    times.push_back(0.0724543); freqs.push_back(56779350.); amplitudes.push_back(0.1535865);
    times.push_back(0.0725198); freqs.push_back( 53057115); amplitudes.push_back(0.2791286);
    times.push_back(0.0725198); freqs.push_back( 59189651); amplitudes.push_back(0.1481105);
    times.push_back(0.0725526); freqs.push_back(56779350.); amplitudes.push_back(0.1500648);
    times.push_back(0.0726181); freqs.push_back(56809860.); amplitudes.push_back(0.2465691);
    times.push_back(0.0726509); freqs.push_back(53148645.); amplitudes.push_back(0.1268001);
    times.push_back(0.0726509); freqs.push_back( 57084452); amplitudes.push_back(0.1791996);
    times.push_back(0.0727164); freqs.push_back(59006590.); amplitudes.push_back(0.2603624);
    times.push_back(0.0727492); freqs.push_back(59006590.); amplitudes.push_back(0.1536206);
    times.push_back(0.0727820); freqs.push_back(59128630.); amplitudes.push_back(0.1397409);
    times.push_back(0.0727820); freqs.push_back(59555772.); amplitudes.push_back(0.1400452);
    times.push_back(0.0728147); freqs.push_back(54155479.); amplitudes.push_back(0.1697771);
    times.push_back(0.0728147); freqs.push_back(54582621.); amplitudes.push_back(0.1476252);
    times.push_back(0.0728475); freqs.push_back(56779350.); amplitudes.push_back(0.1651140);
    times.push_back(0.0728475); freqs.push_back(59067610.); amplitudes.push_back(0.1929571);
    times.push_back(0.0728803); freqs.push_back(53941908.); amplitudes.push_back(0.1614196);
    times.push_back(0.0728803); freqs.push_back(59769343.); amplitudes.push_back(0.1820659);
    times.push_back(0.0729130); freqs.push_back(53148645.); amplitudes.push_back(0.1250671);
    times.push_back(0.0729458); freqs.push_back(59799853.); amplitudes.push_back(0.3102823);
    times.push_back(0.0729786); freqs.push_back(59799853.); amplitudes.push_back(0.1814997);
    times.push_back(0.0730441); freqs.push_back(56626800.); amplitudes.push_back(0.1688024);
    times.push_back(0.0731752); freqs.push_back(55314864.); amplitudes.push_back(0.1542494);
    times.push_back(0.0732407); freqs.push_back(53911398.); amplitudes.push_back(0.1924951);
    times.push_back(0.0732407); freqs.push_back(59738833.); amplitudes.push_back(0.1461460);
    times.push_back(0.0733063); freqs.push_back(53819868.); amplitudes.push_back(0.1287250);
    times.push_back(0.0733063); freqs.push_back(54887722.); amplitudes.push_back(0.1327613);
    times.push_back(0.0734373); freqs.push_back(58579448.); amplitudes.push_back(0.2902271);
    times.push_back(0.0734701); freqs.push_back(54643641.); amplitudes.push_back(0.1345401);
    times.push_back(0.0734701); freqs.push_back(58579448.); amplitudes.push_back(0.1672659);
    times.push_back(0.0734701); freqs.push_back(58609958.); amplitudes.push_back(0.1621858);
    times.push_back(0.0735684); freqs.push_back(56108127.); amplitudes.push_back(0.1951057);
    times.push_back(0.0735684); freqs.push_back(57633634.); amplitudes.push_back(0.1735953);
    times.push_back(0.0735684); freqs.push_back(58640468.); amplitudes.push_back(0.3399784);
    times.push_back(0.0736012); freqs.push_back(57938735.); amplitudes.push_back(0.1589334);
    times.push_back(0.0736339); freqs.push_back(53789358.); amplitudes.push_back(0.1586491);
    times.push_back(0.0736667); freqs.push_back(56260678.); amplitudes.push_back(0.1355866);
    times.push_back(0.0737322); freqs.push_back(58701488.); amplitudes.push_back(0.2863677);
    times.push_back(0.0737650); freqs.push_back(53148645.); amplitudes.push_back(0.1382554);
    times.push_back(0.0737650); freqs.push_back(57023431.); amplitudes.push_back(0.1344866);
    times.push_back(0.0737978); freqs.push_back(53270685.); amplitudes.push_back(0.1662405);
    times.push_back(0.0738633); freqs.push_back(58762509.); amplitudes.push_back(0.1652083);
    times.push_back(0.0738633); freqs.push_back(59616792.); amplitudes.push_back(0.1473926);
    times.push_back(0.0739289); freqs.push_back(58793019.); amplitudes.push_back(0.2060695);
    times.push_back(0.0739616); freqs.push_back(53819868.); amplitudes.push_back(0.1663566);
    times.push_back(0.0739944); freqs.push_back( 55650476); amplitudes.push_back(0.1408757);
    times.push_back(0.0739944); freqs.push_back(56657310.); amplitudes.push_back( 0.181203);
    times.push_back(0.0739944); freqs.push_back(58823529.); amplitudes.push_back(0.2315397);
    times.push_back(0.0740599); freqs.push_back(54308030.); amplitudes.push_back(0.1619495);
    times.push_back(0.0740599); freqs.push_back(58854039.); amplitudes.push_back(0.3180399);
    times.push_back(0.0740927); freqs.push_back(57114962.); amplitudes.push_back(0.1571361);
    times.push_back(0.0741255); freqs.push_back(58884549.); amplitudes.push_back(0.1762043);
    times.push_back(0.0741910); freqs.push_back(54887722.); amplitudes.push_back(0.1602589);
    times.push_back(0.0742238); freqs.push_back(55772516.); amplitudes.push_back(0.1665436);
    times.push_back(0.0742238); freqs.push_back(58915059.); amplitudes.push_back(0.2341337);
    times.push_back(0.0742565); freqs.push_back(58396387.); amplitudes.push_back(0.1492750);
    times.push_back(0.0742565); freqs.push_back(58915059.); amplitudes.push_back(0.3404939);
    times.push_back(0.0742893); freqs.push_back(54430070.); amplitudes.push_back(0.1406510);
    times.push_back(0.0742893); freqs.push_back(57664144.); amplitudes.push_back( 0.148789);
    times.push_back(0.0742893); freqs.push_back(58945569.); amplitudes.push_back(0.1891409);
    times.push_back(0.0743221); freqs.push_back(58945569.); amplitudes.push_back(0.2121315);
    times.push_back(0.0743876); freqs.push_back(54155479.); amplitudes.push_back(0.1296172);
    times.push_back(0.0744204); freqs.push_back(54124969.); amplitudes.push_back(0.1682502);
    times.push_back(0.0744859); freqs.push_back(58182816.); amplitudes.push_back(0.1285351);
    times.push_back(0.0746170); freqs.push_back(55619965.); amplitudes.push_back(0.1951046);
    times.push_back(0.0747808); freqs.push_back(54277520.); amplitudes.push_back(0.1253208);
    times.push_back(0.0747808); freqs.push_back(58274347.); amplitudes.push_back(0.1427803);
    times.push_back(0.0748136); freqs.push_back(54857212.); amplitudes.push_back(0.1698213);
    times.push_back(0.0749119); freqs.push_back(56962411.); amplitudes.push_back(0.1797228);
    times.push_back(0.0749447); freqs.push_back(55375884.); amplitudes.push_back(0.1501580);

    double timeScale = 0.0005; //sec
    double freqScale = 0.1e6; // Hz

    // scale by sqrt(2) so that the radius comes out to 1
    timeScale *=  KTMath::Sqrt2();
    freqScale *=  KTMath::Sqrt2();

    KTKDTreeData kdTreeData;
    kdTreeData.SetXScaling(timeScale);
    kdTreeData.SetYScaling(freqScale);

    vector< double >::const_iterator fIt = freqs.begin();
    vector< double >::const_iterator aIt = amplitudes.begin();
    KTKDTreeData::Point point;
    for (std::vector< double >::const_iterator tIt = times.begin(); tIt != times.end(); ++tIt)
    {
        KTDEBUG(testlog, "Adding point (" << *tIt << ", " << *fIt << ", " << *aIt << ") --> (" << (*tIt)/timeScale << ", " << (*fIt)/freqScale << ")");
        point.fCoords[0] = (*tIt)/timeScale;
        point.fCoords[1] = (*fIt)/freqScale;
        point.fAmplitude = *aIt;
        kdTreeData.AddPoint(point);
        ++fIt; ++aIt;
    }
    kdTreeData.BuildIndex(KTKDTreeData::kEuclidean);

    KTDBScanTrackClustering clustering;

    clustering.SetMinPoints(5);

    clustering.DoClustering(kdTreeData);

    const std::set< KTDataPtr >& candidates = clustering.GetCandidates();

    KTINFO(testlog, "Candidates found: " << candidates.size())

#ifdef ROOT_FOUND
    TFile file("dbscantrackclustering_test.root", "recreate");
    TCanvas* canv = new TCanvas("cCandidates", "Candidates");

    TGraph* ptsGraph = new TGraph(times.size());
    ptsGraph->SetMarkerStyle(1);
    ptsGraph->SetMarkerColor(1);
    vector< double >::const_iterator tIt = times.begin();
    fIt = freqs.begin();
    unsigned pid = 0;
    for (std::vector< double >::const_iterator tIt = times.begin(); tIt != times.end(); ++tIt)
    {
        ptsGraph->SetPoint(pid, *tIt, *fIt);
        ++pid;
        ++fIt;
    }
    ptsGraph->Draw("ap");
    ptsGraph->Write("Points");

    unsigned firstClusterColor = 2;
#endif
    unsigned iCand = 0;

    typedef KTSparseWaterfallCandidateData::Points Points;

    for (std::set< KTDataPtr >::const_iterator cIt = candidates.begin(); cIt != candidates.end(); ++cIt)
    {
        KTINFO(testlog, "Candidate " << iCand);
        KTSparseWaterfallCandidateData& swcData = (*cIt)->Of< KTSparseWaterfallCandidateData >();
        const Points& candPoints = swcData.GetPoints();

#ifdef ROOT_FOUND
        TGraph* clGraph = new TGraph(candPoints.size());
        clGraph->SetMarkerStyle(4);
        clGraph->SetMarkerColor(firstClusterColor + iCand);
        unsigned iPt = 0;
#endif

        stringstream pointsStr;
        pointsStr << "Points: ";
        for (Points::const_iterator pIt = candPoints.begin(); pIt != candPoints.end(); ++pIt)
        {
            pointsStr << "(" << pIt->fTimeInRunC << ", " << pIt->fFrequency << "), ";
#ifdef ROOT_FOUND
            clGraph->SetPoint(iPt, pIt->fTimeInRunC, pIt->fFrequency);
            ++iPt;
#endif
        }

#ifdef ROOT_FOUND
        std::stringstream nameStream;
        nameStream << "Candidate" << iCand;
        clGraph->Draw("psame");
        clGraph->Write(nameStream.str().c_str());

        ++iCand;
#endif
    }

#ifdef ROOT_FOUND
    canv->Write();

    file.Close();
#endif

    return 0;
}

