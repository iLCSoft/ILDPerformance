void Jer(){

  TH1F *hNrjPfos = new TH1F("hNrjPfos","Total Energy of PFOs",500,0,500);
  //TFile *f1 = new TFile("PFOAnalysis.root", "read");
  TChain* chain= new TChain("PfoAnalysisTree");
  chain->Add("PFOAnalysis.root");
  chain->Draw("pfoEnergyTotal>>hNrjPfos");

  float xlow;
  float xhigh;
  float rmsmin;
  float frac;
  float efrac;


  CalcRms(hNrjPfos, xlow, xhigh, rmsmin, frac, efrac);


}

void CalcRms(TH1* h, float &xlow, float &xhigh,
        float &rmsmin, float &frac, float &efrac)
{
  if ( h== NULL) return;
 
  float sum   = 0;
  double sx   = 0.;
  double sxx  = 0.;
 
  int nbins = h->GetNbinsX();

  float total = 0;
 
  for(int i = 0; i <= nbins; i++)
    {
      float binx = h->GetBinLowEdge(i)+h->GetBinWidth(i)/2.0;
      float yi   = h->GetBinContent(i);
      sx        += yi*binx;
      sxx      += yi*binx*binx;
      total     +=yi;
    }
 
  float m      = sx/total;
  float msq    = sxx/total;
  float rawrms = sqrt(msq - m*m);
 
  int is0 = 0;
  sum     = 0;
 
  for(int i = 0; i <= nbins && sum < total/10.; i++)
    {
        float yi = h->GetBinContent(i);
        sum      += yi;
        is0      = i;
    }

    rmsmin = 9999.;
    float xmean = 0;
    float xrms  = 0;
    float sigma = 0;
    float sigmasigma = 0;

    for(int istart = 0; istart <= is0; istart++)
    {
        double sumn   = 0.;
        double csum   = 0.;
        double sumx   = 0.;
        double sumxx  = 0.;
        int iend      = 0;

        for(int i = istart; i <= nbins && csum <0.9*total; i++)
        {
            float binx = h->GetBinLowEdge(i)+h->GetBinWidth(i)/2.0;
            float yi   = h->GetBinContent(i);
            csum += yi;

            if(sumn < 0.9*total)
            {
                sumn   += yi;
                sumx   += yi*binx;
                sumxx  += yi*binx*binx;
                iend   = i;
            }
        }

        float mean   = sumx/sumn;
        float meansq = sumxx/sumn;
        float rms    = sqrt(meansq - mean*mean);

        if(rms < rmsmin)
        {
            xmean = mean;
            xrms  = rms;
            xlow  = h->GetBinLowEdge(istart);
            xhigh = h->GetBinLowEdge(iend);

            float e = 91.2;

            if(xmean > 2500 && xmean < 3500) e = 3000.;
            if(xmean > 1500 && xmean < 2500) e = 2000.;
            if(xmean > 700  && xmean < 1500) e = 1000.;
            if(xmean > 400  && xmean < 700)  e = 500.;
            if(xmean > 250  && xmean < 400)  e = 360.;
            if(xmean > 150  && xmean < 250)  e = 200.;
            if(xmean >  0  && xmean < 20)  e = 10.;
            if(xmean > 20  && xmean < 40)  e = 30.;
            if(xmean > 40  && xmean < 60)  e = 50.;
            if(xmean > 60  && xmean < 80)  e = 70.;

            sigma      = xrms/xmean * sqrt(e);
            sigmasigma = sigma/sqrt(total);
            rmsmin     = rms;
            frac       = xrms/xmean * sqrt(2)*100;
            efrac      = frac/sqrt(total);    
        }
    }

    //    cout << h->GetName() << "  " << rawrms <<"  " << rmsmin <<"   " << xmean
    //    << "   " << sigma <<  "   " << sigmasigma
    //    << "    " << frac << "   " << efrac << std::endl;

    cout << " RMS90 = " << frac << "+- " <<  efrac << std::endl ;

}









