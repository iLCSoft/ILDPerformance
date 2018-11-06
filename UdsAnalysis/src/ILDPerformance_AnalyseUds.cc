/**
 *  @file   ILDPerformance/UdsAnalysis/src/ILDPerformance_AnalyseUds.cc
 * 
 *  @brief  Analyse ROOT file from PfoAnalysis processor (LCPandoraAnalysis) and do plots from uds events
 * 
 *  $Log: $
 */

#include "TApplication.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TList.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include "TLegend.h"
#include "TSystem.h"
#include "TTree.h"
#include "TStyle.h"
#include "TF1.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Fit/ParameterSettings.h"

#include <cmath>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>


/**
 *  @brief  analysis helper class. Copied from LCPandoraAnalysis package and adapted
 */
class AnalysisHelper
{
public:
    /**
     *  @brief  Calculate performance figures for an energy spectrum provided in form of a root th1f
     * 
     *  @param  pTH1F
     */
    static void CalculatePerformance(const TH1F *const pTH1F);

    /**
     *  @brief  Calculate performance figures for an energy spectrum provided in form of a root th1f
     * 
     *  @param  pTH1F
     *  @param  resolution
     *  @param  resolutionError
     *  @param  print
     */
    static void CalculatePerformance(const TH1F *const pTH1F, float &resolution, float &resolutionError, float &scale, float &scaleError, bool print = true);
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline void AnalysisHelper::CalculatePerformance(const TH1F *const pTH1F)
{
    float sigma(0.f), sigmasigma(0.f), mean(0.f), meansigma(0.f);
    return CalculatePerformance(pTH1F, sigma, sigmasigma, mean, meansigma);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void AnalysisHelper::CalculatePerformance(const TH1F *const pTH1F, float &resolution, float &resolutionError, float &scale, float &scaleError, bool print)
{
    static const float FLOAT_MAX(std::numeric_limits<float>::max());

    if (NULL == pTH1F)
        return;

    if (5 > pTH1F->GetEntries())
    {
        std::cout << pTH1F->GetName() << " (" << pTH1F->GetEntries() << " entries) - skipped" << std::endl;
        return;
    }

    // Calculate raw properties of distribution
    float sum = 0., total = 0.;
    double sx = 0., sxx = 0.;
    const unsigned int nbins(pTH1F->GetNbinsX());

    for (unsigned int i = 0; i <= nbins; ++i)
    {
        const float binx(pTH1F->GetBinLowEdge(i) + (0.5 * pTH1F->GetBinWidth(i)));
        const float yi(pTH1F->GetBinContent(i));
        sx += yi * binx;
        sxx += yi * binx * binx;
        total += yi;
    }

    const float rawMean(sx / total);
    const float rawMeanSquared(sxx / total);
    const float rawRms(std::sqrt(rawMeanSquared - rawMean * rawMean));

    sum = 0.;
    unsigned int is0 = 0;

    for (unsigned int i = 0; (i <= nbins) && (sum < total / 10.); ++i)
    {
        sum += pTH1F->GetBinContent(i);
        is0 = i;
    }

    // Calculate truncated properties
    float rmsmin(FLOAT_MAX), sigma(FLOAT_MAX), sigmasigma(FLOAT_MAX), frac(FLOAT_MAX), efrac(FLOAT_MAX), mean(FLOAT_MAX), low(FLOAT_MAX), rms(FLOAT_MAX);
    float high(0.f);

    for (unsigned int istart = 0; istart <= is0; ++istart)
    {
        double sumn = 0.;
        double csum = 0.;
        double sumx = 0.;
        double sumxx = 0.;
        unsigned int iend = 0;

        for (unsigned int i = istart; (i <= nbins) && (csum < 0.9 * total); ++i)
        {
            const float binx(pTH1F->GetBinLowEdge(i) + (0.5 * pTH1F->GetBinWidth(i)));
            const float yi(pTH1F->GetBinContent(i));
            csum += yi;

            if (sumn < 0.9 * total)
            {
                sumn += yi;
                sumx += yi * binx;
                sumxx+= yi * binx * binx;
                iend = i;
            }
        }

        const float localMean(sumx / sumn);
        const float localMeanSquared(sumxx / sumn);
        const float localRms(std::sqrt(localMeanSquared - localMean * localMean));

        if (localRms < rmsmin)
        {
            mean = localMean;
            rms = localRms;
            low = pTH1F->GetBinLowEdge(istart);
            high = pTH1F->GetBinLowEdge(iend);
            rmsmin = localRms;

            sigma = rms;
            sigmasigma = sigma / std::sqrt(total);
            
            frac = rms / mean * std::sqrt(2) * 100.;
            efrac = frac / std::sqrt(total);
        }
    }

    if (print)
    {
        std::cout << pTH1F->GetName() << " (" << pTH1F->GetEntries() << " entries), rawrms: " << rawRms << ", rms90: " << rmsmin
                  << " (" << low << "-" << high << "), mean: " << mean << ", sigma: " << sigma << "+-" << sigmasigma;
        std::cout << ", sE/E: " << frac << "+-" << efrac << std::endl;
    }

    resolution = frac;
    resolutionError = efrac;
    scale = mean;
    scaleError = sigma;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  UdsAnalysis class
 */
class UdsAnalysis
{
public:
  /**
   *  @brief  Constructor
   */
    UdsAnalysis();
    
    /**
     *  @brief  Destructor
     */
    ~UdsAnalysis();
    
    /**
     *  @brief  Set true energies to read from input files
     *  
     *  @param  energiesStr energy list as string, comma separated (e.g '40:91:200:360:500')
     */
    void SetTrueEnergies(const std::string &energiesStr);
    
    /**
     *  @brief  Run the analysis
     */
    void Process();

    // Inputs Set By Parsing Command Line
    std::string                  m_filePattern;                ///< The global file pattern for reading input data (root file) - uds events
    std::vector<std::string>     m_trueEnergies;               ///< The list of true cms energies
    std::string                  m_trueEnergiesStr;            ///< The list of true cms energies - as string
    std::string                  m_outputFile;                 ///< Output file to write histograms
    std::string                  m_treeName;                   ///< The root tree name
    bool                         m_withoutNeutrinoEnergy;      ///< Whether to do not include MC neutrino energy in total energy distribution 
private:
    /**
     *  @brief  Set the TChain branch addresses
     */
    void SetBranchAddresses();
    
    /**
     *  @brief  Read the root input files - uds events
     */
    void ReadInputFiles();
    
    /**
     *  @brief  Create the monitoring histograms - pre minimization step
     */
    void CreateHistograms();
    
    /**
     *  @brief  Fill the histograms - post minimization step
     */
    void FillHistograms();
    
    /**
     *  @brief  Save the filled histograms in a root file
     */
    void SaveHistograms();

private:    
    UdsAnalysis(const UdsAnalysis &) = delete;
    UdsAnalysis &operator=(const UdsAnalysis &) = delete;
};


//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Parse the command line arguments, setting the application parameters
 * 
 *  @param  argc argument count
 *  @param  argv argument vector
 *  @param  udsAnalysis to receive the application parameters
 * 
 *  @return success
 */
bool ParseCommandLine(int argc, char *argv[], UdsAnalysis &udsAnalysis);
void Tokenize(const std::string &inputString, std::vector<std::string> &tokens, const std::string &delimiter = ":");
std::string &Replace(std::string &str, const std::string &variable, const std::string &value);

//------------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    try
    {
        UdsAnalysis udsAnalysis;

        if (!ParseCommandLine(argc, argv, udsAnalysis))
            return 1;

        int myargc = 0;
        char* myargv = (char *)"";
        TApplication *pApplication = new TApplication("ILDPerformance", &myargc, &myargv);
        pApplication->SetReturnFromRun(kTRUE);

        udsAnalysis.Process();
        
        delete pApplication;
    }
    catch (std::exception &exception)
    {
        std::cout << "Exception caught " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

UdsAnalysis::UdsAnalysis() :
    m_filePattern(""),
    m_trueEnergies(),
    m_trueEnergiesStr(""),
    m_outputFile("ILDPerformance_UdsAnalysis.root"),
    m_treeName("PfoAnalysisTree"),
    m_withoutNeutrinoEnergy(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

UdsAnalysis::~UdsAnalysis()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void UdsAnalysis::SetTrueEnergies(const std::string &energiesStr)
{
    m_trueEnergiesStr = energiesStr;
    m_trueEnergies.clear();
    Tokenize(energiesStr, m_trueEnergies, ":");
}

//------------------------------------------------------------------------------------------------------------------------------------------

void UdsAnalysis::Process()
{  
    TFile *pTFile = new TFile(m_outputFile.c_str(), "RECREATE");
    std::vector<TObject*> deletionPool;
    const std::string lastEnergyStr(m_trueEnergies.back());
    std::stringstream sslast(lastEnergyStr);
    float lastEnergy(0.f);
    
    if( (sslast >> lastEnergy).fail() )
        throw std::runtime_error("UdsAnalysis::Process: Couldn't convert energy string to float !");
    
    // Setup global graphs : JER and JES
    // Jet energy resolution plots
    TGraphErrors *fJer = new TGraphErrors(m_trueEnergies.size()); deletionPool.push_back(fJer);
    fJer->SetMarkerSize(3); fJer->SetLineWidth(2);
    fJer->SetTitle("Jet energy resolution;Jet energy [GeV];RMS_{90}(E_{j}) / Mean_{90}(E_{j}) [%]");
    
    TGraphErrors *fJerL7A = new TGraphErrors(m_trueEnergies.size()); deletionPool.push_back(fJerL7A);
    fJerL7A->SetMarkerSize(3); fJerL7A->SetLineWidth(2);
    fJerL7A->SetTitle("Jet energy resolution, cos(#theta) < 0.7;Jet energy [GeV];RMS_{90}(E_{j}) / Mean_{90}(E_{j}) [%]");
    
    TGraphErrors *fJerH7A = new TGraphErrors(m_trueEnergies.size()); deletionPool.push_back(fJerH7A);
    fJerH7A->SetMarkerSize(3); fJerH7A->SetLineWidth(2);
    fJerH7A->SetTitle("Jet energy resolution, cos(#theta) > 0.7;Jet energy [GeV];RMS_{90}(E_{j}) / Mean_{90}(E_{j}) [%]");
    
    // Jet energy scale plots
    TGraphErrors *fJes = new TGraphErrors(m_trueEnergies.size()); deletionPool.push_back(fJes);
    fJes->SetTitle("Jet energy scale;Jet energy [GeV];Reconstructed jet energy [GeV]");
    fJes->SetMarkerSize(3); fJes->SetLineWidth(1); fJes->SetDrawOption("");
    TF1 *pXEqualY = new TF1("PerfectJes", "x", 0, lastEnergy+10); fJes->GetListOfFunctions()->Add(pXEqualY);
    
    TGraphErrors *fJesL7A = new TGraphErrors(m_trueEnergies.size()); deletionPool.push_back(fJesL7A);
    fJesL7A->SetTitle("Jet energy scale, cos(#theta) < 0.7;Jet energy [GeV];Reconstructed jet energy [GeV]");
    fJesL7A->SetMarkerSize(3); fJesL7A->SetLineWidth(1);
    TF1 *pXEqualYL7A = new TF1("PerfectJesL7A", "x", 0, lastEnergy+10); fJesL7A->GetListOfFunctions()->Add(pXEqualYL7A);
    
    TGraphErrors *fJesH7A = new TGraphErrors(m_trueEnergies.size()); deletionPool.push_back(fJesH7A);
    fJesH7A->SetTitle("Jet energy scale, cos(#theta) > 0.7;Jet energy [GeV];Reconstructed jet energy [GeV]");
    fJesH7A->SetMarkerSize(3); fJesH7A->SetLineWidth(1);
    TF1 *pXEqualYH7A = new TF1("PerfectJesH7A", "x", 0, lastEnergy+10); fJesH7A->GetListOfFunctions()->Add(pXEqualYH7A);
    
    // loop over the different jet energies
    for (unsigned int e = 0 ; e < m_trueEnergies.size() ; e++)
    {
        const std::string energyStr(m_trueEnergies.at(e));
        std::stringstream ss(energyStr);
        float trueEnergy = 0.f;
        std::string fileName(m_filePattern);
        Replace(fileName, "energy", energyStr);
        
        if( (ss >> trueEnergy).fail() )
            throw std::runtime_error("UdsAnalysis::Process: Couldn't convert energy string to float !");
        
        TChain* pTChain = new TChain(m_treeName.c_str());
        pTChain->Add(fileName.c_str());
        
        // Setup branches
        int qPdg(0);
        float pfoEnergyTotal(0.f), mcEnergyENu(0.f), thrust(0.f);
        pTChain->SetBranchAddress("pfoEnergyTotal", &pfoEnergyTotal);
        if(not m_withoutNeutrinoEnergy) {
          pTChain->SetBranchAddress("mcEnergyENu", &mcEnergyENu);
        }
        pTChain->SetBranchAddress("qPdg", &qPdg);
        pTChain->SetBranchAddress("thrust", &thrust);
        
        // Define regions for which to create and investigate pfo energy spectra
        const unsigned int nRegionBins(13);
        float pRegionBinEdges[nRegionBins + 1] = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.925, 0.95, 0.975, 1.0};
        
        const float energyRangeMin(trueEnergy - trueEnergy/2.), energyRangeMax(trueEnergy + trueEnergy/2.);
        const int nEnergyBins(trueEnergy*2 + 1);

        pTFile->mkdir( ("uds" + energyStr).c_str() )->cd();

        // Book histograms
        TH1F *pResVsCosThetaHist = new TH1F(("ResVsCosTheta" + energyStr + "GeV").c_str(), ("RMS_{90}(E_{j}) / Mean_{90}(E_{j}) vs |cos(#theta)| (" + energyStr + "GeV)").c_str(), nRegionBins, pRegionBinEdges);
        pResVsCosThetaHist->SetYTitle("RMS_{90}(E_{j}) / Mean_{90}(E_{j}) [%]");
        pResVsCosThetaHist->SetXTitle("|cos(#theta)|");
        
        TH1F *pRecVsCosThetaHist = new TH1F(("RecVsCosTheta" + energyStr + "GeV").c_str(), ("Mean_{90}(E_{j}) vs |cos(#theta)| (" + energyStr + "GeV)").c_str(), nRegionBins, pRegionBinEdges);
        pRecVsCosThetaHist->SetYTitle("Mean_{90}(E_{j}) [GeV]");
        pRecVsCosThetaHist->SetXTitle("|cos(#theta)|");

        TH1F **pRegionHistograms = new TH1F*[nRegionBins];
        TH1F *pPFA = new TH1F(("fPFA_" + energyStr).c_str(), ("TotalEnergy (" + energyStr + "GeV)").c_str(), nEnergyBins, energyRangeMin, energyRangeMax);
        TH1F *pPFAL7A = new TH1F(("fPFA_L7A_" + energyStr).c_str(), ("TotalEnergy<0.7A (" + energyStr + "GeV)").c_str(), nEnergyBins, energyRangeMin, energyRangeMax);
        TH1F *pPFAH7A = new TH1F(("fPFA_H7A_" + energyStr).c_str(), ("TotalEnergy>0.7A (" + energyStr + "GeV)").c_str(), nEnergyBins, energyRangeMin, energyRangeMax);
        
        for (unsigned int i = 0; i < nRegionBins; ++i)
        {
            std::ostringstream name, title;
            name << "fPFA_A" << i << "_E" << energyStr;
            title << "TotalEnergy_" << pRegionBinEdges[i] << "-" << pRegionBinEdges[i + 1] << " (" << energyStr << " GeV)";
            pRegionHistograms[i] = new TH1F(name.str().c_str(), title.str().c_str(), nEnergyBins, energyRangeMin, energyRangeMax);
        }
        
        for (unsigned int iTree = 0; iTree < pTChain->GetEntries(); ++iTree)
        {
            pTChain->GetEntry(iTree);

            if ((qPdg < 1) || (qPdg > 6))
                continue;

            const double totalEnergy = m_withoutNeutrinoEnergy ? pfoEnergyTotal : pfoEnergyTotal + mcEnergyENu;
            
            pPFA->Fill(totalEnergy, 1.);
            
            if (thrust <= 0.7f)
                pPFAL7A->Fill(totalEnergy, 1.);
                
            if (thrust > 0.7f && thrust <= 0.98f)
                pPFAH7A->Fill(totalEnergy, 1.);
                
            for (unsigned int i = 0; i < nRegionBins; ++i)
            {
                if ((thrust >= pRegionBinEdges[i]) && (thrust < pRegionBinEdges[i + 1]))
                    pRegionHistograms[i]->Fill(totalEnergy, 1.);
            }
        }
        
        // JER and JES for cos(theta) < 0.7
        float resolutionL7A(0.f), resolutionErrorL7A(0.f), scaleL7A(0.f), scaleErrorL7A(0.f);
        AnalysisHelper::CalculatePerformance(pPFAL7A, resolutionL7A, resolutionErrorL7A, scaleL7A, scaleErrorL7A);
        fJesL7A->SetPoint(e, trueEnergy/2., scaleL7A/2.);
        fJesL7A->SetPointError(e, 0, scaleErrorL7A/2.);
        fJerL7A->SetPoint(e, trueEnergy/2., resolutionL7A);
        fJerL7A->SetPointError(e, 0, resolutionErrorL7A);
        
        // JER and JES for cos(theta) > 0.7
        float resolutionH7A(0.f), resolutionErrorH7A(0.f), scaleH7A(0.f), scaleErrorH7A(0.f);
        AnalysisHelper::CalculatePerformance(pPFAH7A, resolutionH7A, resolutionErrorH7A, scaleH7A, scaleErrorH7A);
        fJesH7A->SetPoint(e, trueEnergy/2., scaleH7A/2.);
        fJesH7A->SetPointError(e, 0, scaleErrorH7A/2.);
        fJerH7A->SetPoint(e, trueEnergy/2., resolutionH7A);
        fJerH7A->SetPointError(e, 0, resolutionErrorH7A);

        // JER and JES for the overall detector
        float resolution(0.f), resolutionError(0.f), scale(0.f), scaleError(0.f);
        AnalysisHelper::CalculatePerformance(pPFA, resolution, resolutionError, scale, scaleError);
        fJes->SetPoint(e, trueEnergy/2., scale/2.);
        fJes->SetPointError(e, 0, scaleError/2.);
        fJer->SetPoint(e, trueEnergy/2., resolution);
        fJer->SetPointError(e, 0, resolutionError);

        // JES/JER for different cos(theta) regions
        for (unsigned int i = 0; i < nRegionBins; ++i)
        {
            resolution = 0.f; resolutionError = 0.f; scale = 0.f; scaleError = 0.f;
            AnalysisHelper::CalculatePerformance(pRegionHistograms[i], resolution, resolutionError, scale, scaleError);
            pRecVsCosThetaHist->SetBinContent(i + 1, scale); pRecVsCosThetaHist->SetBinError(i + 1, scaleError);
            pResVsCosThetaHist->SetBinContent(i + 1, resolution); pResVsCosThetaHist->SetBinError(i + 1, resolutionError);
        }
        
        delete pTChain;
        delete [] pRegionHistograms;
    }
    
    pTFile->cd();
    fJer->Write("fJER");
    fJerL7A->Write("fJERL7A");
    fJerH7A->Write("fJERH7A");
    fJes->Write("fJES");
    fJesL7A->Write("fJESL7A");
    fJesH7A->Write("fJESH7A");
    
    // clear memory. All histograms are owned by the TFile and will be deleted on "delete pTFile;"
    for (std::vector<TObject*>::iterator iter = deletionPool.begin(), endIter = deletionPool.end() ; endIter != iter ; ++iter)
        delete *iter;        
    
    pTFile->Write();
    pTFile->Close();
    delete pTFile;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

bool ParseCommandLine(int argc, char *argv[], UdsAnalysis &udsAnalysis)
{
    int c(0);
    bool energiesSet(false), filePatternSet(false);

    while (((c = getopt(argc, argv, "o:e:f:t:")) != -1) || (argc == 1))
    {
        switch (c)
        {
        case 'o':
            udsAnalysis.m_outputFile = optarg;
            break;
        case 'e':
            udsAnalysis.SetTrueEnergies(optarg);
            energiesSet = true;
            break;
        case 'f':
            udsAnalysis.m_filePattern = optarg;
            filePatternSet = true;
            break;
        case 't':
            udsAnalysis.m_treeName = optarg;
            break;
        case 'n':
            udsAnalysis.m_withoutNeutrinoEnergy = true;
            break;
        case 'h':
        default:
            std::cout << std::endl << "Usage: ILDPerformance_UdsAnalysis " << std::endl
                      << "    -o file      (optional,  output file to write results to. Default is 'ILDPerformance_UdsAnalysis.root')          " << std::endl
                      << "    -e energies  (mandatory, input energies column separated, i.e '40:91:200:360:500')                               " << std::endl
                      << "    -f pattern   (mandatory, input file pattern. Should contains %{energy} key, i.e ./PfoAnalysisUds_%{energy}.root) " << std::endl
                      << "    -t treename  (optional,  the tree name to look in root files, usually PfoAnalysisTree)                           " << std::endl
                      << "    -n           (optional,  whether to do not include MC neutrino energy in total energy distribution, default is false)" << std::endl
                      << std::endl;
            return false;
        }
    }
    
    if( !energiesSet || !filePatternSet )
    {
      std::cout << std::endl << "Usage: ILDPerformance_UdsAnalysis " << std::endl
                << "    -o file      (optional,  output file to write results to. Default is 'ILDPerformance_UdsAnalysis.root')          " << std::endl
                << "    -e energies  (mandatory, input energies column separated, i.e '40:91:200:360:500')                               " << std::endl
                << "    -f pattern   (mandatory, input file pattern. Should contains %{energy} key, i.e ./PfoAnalysisUds_%{energy}.root) " << std::endl
                << "    -t treename  (optional,  the tree name to look in root files, usually PfoAnalysisTree)                           " << std::endl
                << "    -n           (optional,  whether to do not include MC neutrino energy in total energy distribution, default is false)   " << std::endl
                << std::endl;
      return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Tokenize(const std::string &inputString, std::vector<std::string> &tokens, const std::string &delimiter)
{
	std::string::size_type lastPos = inputString.find_first_not_of(delimiter, 0);
	std::string::size_type pos     = inputString.find_first_of(delimiter, lastPos);

	while ((std::string::npos != pos) || (std::string::npos != lastPos))
	{
		tokens.push_back(inputString.substr(lastPos, pos - lastPos));
		lastPos = inputString.find_first_not_of(delimiter, pos);
		pos = inputString.find_first_of(delimiter, lastPos);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------

std::string &Replace(std::string &str, const std::string &variable, const std::string &value)
{
	std::string replaceVar = "%{" + variable + "}";
	size_t pos = str.find(replaceVar);

	while( pos != std::string::npos )
	{
		str.replace( pos , replaceVar.size() , value );
		pos = str.find(replaceVar);
	}

	return str;
}
