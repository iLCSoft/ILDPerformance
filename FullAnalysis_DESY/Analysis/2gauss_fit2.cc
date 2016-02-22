#include <cstdio>
#include <string.h>
#include <iostream>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include "TH1.h"
#include <TH1F.h>
#include <TF1.h>
#include "TMath.h"
#include "TLegend.h"
#include <TMinuit.h>
//#include <TFormula>

#ifdef __CINT__
#pragma link C++ class std::vector<float>+;
#endif
//#include "TROOT.h"
//#include "TClonesArray.h"

using namespace std;

struct event {
        float rcene[10000]; //E
        float rcmox[100000];
        float rcmoy[100000];
        float rcmoz[100000];
        int rctyp[1100];
	float em1;
	float em2[10000];
	//Z
	float en;
	float mass;
	};

//#define k 122

Double_t gauss_left(Double_t *x, Double_t *par)
{
        return par[0]*exp(-pow((x[0]-par[1])/par[2],2)/2);
}

Double_t gauss_right(Double_t *x, Double_t *par)
{
  //int  k=130;
  return par[0]*par[3]*exp(-pow((x[0]-par[1])/par[2],2)/2)+(1-par[3])*exp((x[0]-par[1])*128./par[2]+(128*128)/2);
}

Double_t fill_fit(Double_t *x, Double_t *par)
{
	return gauss_left(x, par) + gauss_right(x, &par[3]); 
}

double gauss(double *x, double *par)
{
        double f;
        f = (par[0]/(sqrt(2*M_PI)*par[2]))*exp(-pow((x[0]-par[1])/par[2],2)/2)+par[3]+par[4]*x[0];
	//+par[5]*x[0]*x[0];
	//+par[6]*x[0]*x[0]*x[0];
        return f;

}

/*void p_fit(double *par)
{
        int i, ires;
        double err;
        TMinuit *mn;
        mn = new TMinuit(6);
        mn->SetFCN(gauss);
        mn->mnparm(0, "MTHRESH", 5.29, 0.001, 0, 0, ires);
        mn->mnparm(1, "A", 0.1, 0.001, 0, 0, ires);
        mn->mnparm(2, "A0", 1000., 0.001, 0, 0, ires);
        mn->mnparm(3, "A1", 0.0, 0.001, 0, 0, ires);
        mn->mnparm(4, "A2", 0.0, 0.001, 0, 0, ires);
        mn->mnparm(5, "N_SIG", 0, 0.001, 0, 0, ires);
        mn->mnparm(6, "sigma 1", 0.003, 0.001, 0, 0, ires);
        mn->mncomd("FIX 1 2 4 5 6", ires);
        mn->mncomd("MIGRAD", ires);
        mn->mncomd("REL 2", ires);
        mn->mncomd("MIGRAD", ires);
        mn->mncomd("REL 6", ires);
        mn->mncomd("MIGRAD", ires);
        //mn->mncomd("REL 7 8 10 11 4 5 12", ires);
        mn->mncomd("REL 1", ires);
        mn->mncomd("MIGRAD", ires);
        for (i = 0; i < 6; i++)
                mn->GetParameter(i, par[i], err);
        delete mn;
}*/


void ma()
{
        int n, i;
	Int_t nrec;
        vector<float> rcx;
	vector<float> rcx2;
        vector<float> rcy;
	vector<float> rcy2;
	vector<float> rcz;
	vector<float> rcz2;
	vector<float> rce;
	vector<float> rce2;
	vector<float> rcm;
	vector<float> rcm2;
	vector<float> Mz;
	vector<float> Ez;
	vector<float> Mrec;
	vector<float> Mrec2m;
	vector<float> Mzcut;

	// Reading tree
	TChain *dat = new TChain("MyLCTuple");
        dat->Add("../RootFiles/lc_mokka.root");
	//Determins the number of events
        struct event ev;
	dat->SetBranchAddress ("nrec", &nrec );
        dat->SetBranchAddress ("rcene", ev.rcene );
        dat->SetBranchAddress ("rcmox", ev.rcmox );
        dat->SetBranchAddress ("rcmoy", ev.rcmoy );
        dat->SetBranchAddress ("rcmoz", ev.rcmoz );
        dat->SetBranchAddress ("rctyp", ev.rctyp );
	n=dat->GetEntries();
//	const int nr = nrec;
	
	//dat->Draw("rcmox", "rctyp == -13");
	TFile *f = new TFile("RecoilMass.root", "recreate");
        TTree *t1 = new TTree("t1", "t1");
	TTree *t2 = new TTree("t2", "t2");
	TTree *z = new TTree("z", "z");
	TTree *Mr = new TTree("Mr", "Mr");
	t1->Branch("rce", &rce);
	t2->Branch("rce2", &rce2);
	t1->Branch("rcx", &rcx);
	t2->Branch("rcx2", &rcx2);
	t1->Branch("rcy", &rcy);
	t2->Branch("rcy2", &rcy2);
	t1->Branch("rcz", &rcz);
	t2->Branch("rcz2", &rcz2);
	t1->Branch("rcm", &rcm);
	t2->Branch("rcm2", &rcm2);
	z->Branch("Mz", &Mz);
	z->Branch("Ez", &Ez);
	z->Branch("Mrec", &Mrec);
	z->Branch("Mrec2m", &Mrec2m);
	z->Branch("Mz_with_cut", &Mzcut);
	float rcm_test, rcm_test2, mz, ez, mrec, ez2m, mrec2m;
	Int_t u[n], g[n];
	for (i=0; i<n; i++){
		g[i] = 0;
		u[i] = 0;
	}
	TH1F *h2 = new TH1F("h2","Higgs Recoil Mass with cut",100,100,200);
	for(i=0; i<n; i++){
		rce.clear();
		rce2.clear();
		rcx.clear();
		rcx2.clear();
		rcy.clear();
		rcy2.clear();
		rcz.clear();
		rcz2.clear();
		rcm.clear();
		
		dat->GetEntry(i);
		for(int j=0; j<nrec; j++){
			if(ev.rctyp[j] == 13){
			g[i]=g[i]++;
				}
			if(ev.rctyp[j] == -13){
                        u[i]=u[i]++;
				}
			}
		for(int j=0; j<nrec; j++){
                        if(ev.rctyp[j] == 13){
			rce.push_back(ev.rcene[j]);
			rcx.push_back(ev.rcmox[j]);
			rcy.push_back(ev.rcmoy[j]);
			rcz.push_back(ev.rcmoz[j]);
			rcm_test = sqrt(ev.rcmox[j]*ev.rcmox[j]+ev.rcmoy[j]*ev.rcmoy[j]+ev.rcmoz[j]*ev.rcmoz[j]);
			rcm.push_back(rcm_test);
			}
			if(ev.rctyp[j] == -13){
			rce2.push_back(ev.rcene[j]);
			rcx2.push_back(ev.rcmox[j]);
			rcy2.push_back(ev.rcmoy[j]);
			rcz2.push_back(ev.rcmoz[j]);
			rcm_test2 = sqrt(ev.rcmox[j]*ev.rcmox[j]+ev.rcmoy[j]*ev.rcmoy[j]+ev.rcmoz[j]*ev.rcmoz[j]);
			rcm2.push_back(rcm_test2);
				}
//		printf("%f",g[j]);
			}
		t1->Fill();
		t2->Fill();
	}
	h2->Write();
        t1->Write();
	t2->Write();
	f->Write();

	TH1F *h1 = new TH1F("h1","Higgs Recoil Mass",150,100,250);
	TH1F *h3 = new TH1F("h3","Mz with cut",50,65,115);
	float M2rec;
	// Z boson	
	for(i=0; i<n; i++){
		t1->GetEntry(i);
		t2->GetEntry(i);
		Mz.clear();
		Ez.clear();
		Mrec.clear();
		Mzcut.clear();
		mz = 0;
		ez = 0;
		mrec=0;
//		printf(" g = %d", g[i]);
//		printf(" u = %d", u[i]);
		for(int j=0; j<g[i]; j++){
			for(int h=0; h<u[i]; h++){
				//mz = (rce[j]*rce[j] + rce2[h]*rce2[h] - rcm2[h]*rcm2[h] - rcm[j]*rcm[j] + 2*(abs(rcx[j]*rcx2[h])+abs(rcy[j]*rcy2[h])+abs(rcz[j]*rcz2[h])));
 			mz=sqrt(pow(rce[j]+rce2[h],2)-pow(rcx[j]+rcx2[h],2)-pow(rcy[j]+rcy2[h],2)-pow(rcz[j]+rcz2[h],2));
			ez = rce[j]+rce2[h];
			Mz.push_back(mz);
			Ez.push_back(ez);
			mrec=sqrt(250*250+mz*mz-2*ez*250);
			Mrec.push_back(mrec);
			h1->Fill(mrec);
	//		printf(" rce[j] = %f, rce2[h] = %f", rce[j], rce2[h]);
			}	
		}
		for(int j=0; j<g[i]*u[i]; j++){
//		if(Mz[j] > 70 && Mz[j] < 110)
//				continue;
		if(abs(91.2-Mz[j+1]) < abs(91.2-Mz[j])){
			M2rec = Mz[j+1];
			ez2m = Ez[j+1];
			}
		else {
			M2rec = Mz[j];
			ez2m = Ez[j];
			}
//		printf("M2rec[%d] = %f", i, M2rec);
//		if(i == 998)
//			printf("Mrec[%d] = %f", i, Mrec[j]);
			if( M2rec > 70 && M2rec < 110){
			mrec2m=sqrt(250*250+M2rec*M2rec-2*ez2m*250);
                        Mrec2m.push_back(mrec2m);
			Mzcut.push_back(M2rec);
			h3->Fill(M2rec);
			}	
		}
		h2->Fill(mrec2m);
//		h3->Fill(M2rec);
		z->Fill();
	//	printf("mz = %f\n", mz);
	}
	z->Write();
	f->Write();
//	z->Draw("Mz_with_cut");
	h3->Draw();

	//Fit
	/*	
	Double_t par[6];
        // fit landau
//	TF1 *fitsum = new TF1("fitsum","gaus+expo",128,250);
//	TF1 *fitFnc = new TF1("fitFnc", "gaus", 110, 128);

	TF1 *fitsum = new TF1("fitsum", "expo", 128, 250);
	TF1 *fitgaus = new TF1("fitgaus", "gaus", 115, 128);
	
//	h2->Fit("fitgaus", "R");
//	h2->Draw();	

	h2->Fit("fitsum", "R");
        fitsum->GetParameters(&par[0]);
        h2->Fit("fitgaus", "R");
        fitgaus->GetParameters(&par[2]);
        TF1 *fitall = new TF1("fitall","expo(0)+gaus(2)",115,250);
        fitall->SetParameters(par);
	fitall->FixParameter(0,par[0]);
	fitall->FixParameter(1,par[1]);
	fitall->FixParameter(2,par[2]);
//	fitall->FixParameter(3,par[3]);
//	fitall->FixParameter(4,par[4]);
//	printf("par = %f\n", par[0]);
//	TF1 *fitFnc = new TF1("fitFnc", "gaus", 110, 128);
//	for(i = 0; i < 5; i++)
//		printf("par[%d] = %f\n", par[i], par[i]);
        h2->Fit(fitall, "R");
	h2->Draw();

        double *mass, *mass_err;
        mass = fitall->GetParameters();
        mass_err = fitall->GetParErrors();
        printf("Recoil mass = %f,\n Err = %f\n",mass[1], mass_err[1]);
	*/

	Double_t paokpar[8];
	TF1 *g1    = new TF1("g1","gaus",115.5,121.5);
	TF1 *g2 = new TF1("g2","gaus+expo",121.5,150);
	//TF1 *g2    = new TF1("g2",gauss_right,123,150,4);
	//TF1 *total = new TF1("total","gauss_left(0)+gauss_right(3)",110,150,7);
	TF1 *total = new TF1("total","g1+g2",115.5,150);
	total->SetLineColor(4);
	h1->Fit(g1,"R");
	//h1->Fit(g2,"R+");

	g1->GetParameters(&paokpar[0]);
	g2->GetParameters(&paokpar[3]);

	total->SetParameters(paokpar);
	//h1->Fit(total,"R+");
	h1->Draw();	
}
