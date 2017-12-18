# initialize python/root environment before running this macro:
#  export PYTHONPATH=${ROOTSYS}/lib

import ROOT
ROOT.gROOT.SetBatch()
from array import array
from math import log10

filename = './photons_validate_ILD_l5_o1_v02_nobg.root'
rfile = ROOT.TFile(filename,'read')

photonEnergies=[]
allvars=[]

tt=ROOT.TLatex()

hh = rfile.GetListOfKeys()
for h in hh:
    hname=h.GetName()
    if 'photonEn' in hname:
        df = hname.split('_')
        energylab = int( df[0].replace('photonEn','') )
        if energylab not in photonEnergies:
            photonEnergies.append(energylab)
        if df[1] not in allvars:
            allvars.append(df[1])

photonEnergies.sort()
print photonEnergies
print allvars

kky=filename.split('/')
planme=kky[len(kky)-1]
planme=planme.replace('.root','.ps')

cc = ROOT.TCanvas()
cc.Print(planme+'[')

nen=len(photonEnergies)
ny=int( float(nen)**0.5 )
nx=nen/ny
if nx*ny<nen:
    nx=nx+1

for vv in allvars:
    cc.Clear()
    cc.Divide(nx, ny)
    ic=1
    for pen in photonEnergies:
        cc.cd(ic)
        ic=ic+1
        hh=rfile.Get('photonEn'+str(pen)+'_'+vv)
        if hh.InheritsFrom('TH2'):
            hh.Draw('col')
        else:
            hh.Draw()

    cc.Print(planme)

#--------------------

realen=array('d')
logen=array('d')
zero=array('d')
invsqren=array('d')

for pen in photonEnergies:
    realen.append(float(pen)/10.)
    invsqren.append(1./(float(pen)/10.)**0.5)
    logen.append(log10(float(pen)/10.))
    zero.append(0)

#--------------------

effs=array('d')
effs22=array('d')
effsOnePhoton=array('d')


if 'nPFO' in allvars:
    for pen in photonEnergies:
        hh=rfile.Get('photonEn'+str(pen)+'_nPFO')
        ntot=hh.GetEntries()
        n0 = hh.GetBinContent(1)
        eff = float ( ntot-n0 ) / ntot
        effs.append(eff)

if 'nPFO22' in allvars:
    for pen in photonEnergies:
        hh=rfile.Get('photonEn'+str(pen)+'_nPFO22')
        ntot=hh.GetEntries()
        n0 = hh.GetBinContent(1)
        eff = float ( ntot-n0 ) / ntot
        effs22.append(eff)

        n1=hh.GetBinContent(2)
        eff = float ( n1 ) / ntot
        effsOnePhoton.append(eff)

nn=int(len(realen))


tg = ROOT.TGraph( nn, logen, effs )
tg22 = ROOT.TGraph( nn, logen, effs22 )
tg22.SetLineColor(4)

tgOnePhoton = ROOT.TGraph( nn, logen, effsOnePhoton )
tgOnePhoton.SetLineColor(2)

cc.Clear()
tg.Draw('apl')
tg.GetHistogram().SetTitle('')
tg.GetHistogram().GetXaxis().SetTitle('log10 ( photon energy [GeV] )')
tg.GetHistogram().GetYaxis().SetTitle('efficiency')
tg22.Draw('same;pl')
tgOnePhoton.Draw('same;pl')


tt.SetTextColor(1)
tt.DrawLatexNDC(0.52, 0.5, '>0 PFO')
tt.SetTextColor(4)
tt.DrawLatexNDC(0.52, 0.4, '>0 photon PFO')
tt.SetTextColor(2)
tt.DrawLatexNDC(0.52, 0.3, 'exactly 1 photon PFO')


cc.Print(planme)

#--------------------

mean=array('d')
meanDiff=array('d')
meanRelDiff=array('d')
meanErr=array('d')
meanRelErr=array('d')
width=array('d')
widthErr=array('d')
relWidth=array('d')
relWidthErr=array('d')

bar_mean=array('d')
bar_meanDiff=array('d')
bar_meanRelDiff=array('d')
bar_meanErr=array('d')
bar_meanRelErr=array('d')
bar_width=array('d')
bar_widthErr=array('d')
bar_relWidth=array('d')
bar_relWidthErr=array('d')

fwd_mean=array('d')
fwd_meanDiff=array('d')
fwd_meanRelDiff=array('d')
fwd_meanErr=array('d')
fwd_meanRelErr=array('d')
fwd_width=array('d')
fwd_widthErr=array('d')
fwd_relWidth=array('d')
fwd_relWidthErr=array('d')

if 'totenCosth' in allvars:
    cc.Clear()
    cc.Divide(nx, ny)
    ic=1
    for pen in photonEnergies:
        re = float(pen)/10

        cc.cd(ic)
        ic=ic+1
        hh=rfile.Get('photonEn'+str(pen)+'_totenCosth')
        dd = hh.ProjectionY()
        dd.GetXaxis().SetRange(2, dd.GetNbinsX() )
        dd.Fit('gaus','q','',0.01,dd.GetXaxis().GetXmax())
        ff=dd.GetFunction('gaus')
        dd.SetLineColor(1)
        ff.SetLineColor(1)

        mean.append(ff.GetParameter(1))
        meanErr.append(ff.GetParError(1))
        meanRelErr.append(ff.GetParError(1)/re)
        meanDiff.append( ff.GetParameter(1) - re) 
        meanRelDiff.append( (ff.GetParameter(1) - re)/re ) 
        width.append(ff.GetParameter(2))
        widthErr.append(ff.GetParError(2))
        relWidth.append(ff.GetParameter(2)/ff.GetParameter(1))
        relWidthErr.append(ff.GetParError(2)/ff.GetParameter(1))

        ibBar = hh.GetXaxis().FindBin(0.7)
        ddB = hh.ProjectionY('_pyBar'+str(pen),0,ibBar)
        ddB.Fit('gaus','q','',0.01,ddB.GetXaxis().GetXmax())
        ffB=ddB.GetFunction('gaus')
        ddB.SetLineColor(4)
        ffB.SetLineColor(4)

        bar_mean.append(ffB.GetParameter(1))
        bar_meanErr.append(ffB.GetParError(1))
        bar_meanRelErr.append(ffB.GetParError(1)/re)
        bar_meanDiff.append( ffB.GetParameter(1) - re) 
        bar_meanRelDiff.append( (ffB.GetParameter(1) - re)/re ) 
        bar_width.append(ffB.GetParameter(2))
        bar_widthErr.append(ffB.GetParError(2))
        bar_relWidth.append(ffB.GetParameter(2)/ffB.GetParameter(1))
        bar_relWidthErr.append(ffB.GetParError(2)/ffB.GetParameter(1))

        ddF = hh.ProjectionY('_pyFwd'+str(pen),ibBar)
        ddF.Fit('gaus','q','',0.01,ddF.GetXaxis().GetXmax())
        ffF=ddF.GetFunction('gaus')
        ddF.SetLineColor(8)
        ffF.SetLineColor(8)

        fwd_mean.append(ffF.GetParameter(1))
        fwd_meanErr.append(ffF.GetParError(1))
        fwd_meanRelErr.append(ffF.GetParError(1)/re)
        fwd_meanDiff.append( ffF.GetParameter(1) - re) 
        fwd_meanRelDiff.append( (ffF.GetParameter(1) - re)/re ) 
        fwd_width.append(ffF.GetParameter(2))
        fwd_widthErr.append(ffF.GetParError(2))
        fwd_relWidth.append(ffF.GetParameter(2)/ffF.GetParameter(1))
        fwd_relWidthErr.append(ffF.GetParError(2)/ffF.GetParameter(1))

        dd.Draw()
        ddB.Draw('same')
        ddF.Draw('same')


    cc.Print(planme)



tgeMean = ROOT.TGraphErrors(nn, realen, mean, zero, meanErr)
tgeMean.SetMarkerStyle(24)
bar_tgeMean = ROOT.TGraphErrors(nn, realen, bar_mean, zero, bar_meanErr)
bar_tgeMean.SetMarkerStyle(25)
bar_tgeMean.SetLineColor(4)
bar_tgeMean.SetMarkerColor(4)
fwd_tgeMean = ROOT.TGraphErrors(nn, realen, fwd_mean, zero, fwd_meanErr)
fwd_tgeMean.SetMarkerStyle(26)
fwd_tgeMean.SetLineColor(8)
fwd_tgeMean.SetMarkerColor(8)

tgeMeanDiff = ROOT.TGraphErrors(nn, logen, meanDiff, zero, meanErr)
tgeMeanDiff.SetMarkerStyle(24)
bar_tgeMeanDiff = ROOT.TGraphErrors(nn, logen, bar_meanDiff, zero, bar_meanErr)
bar_tgeMeanDiff.SetMarkerStyle(25)
bar_tgeMeanDiff.SetLineColor(4)
bar_tgeMeanDiff.SetMarkerColor(4)
fwd_tgeMeanDiff = ROOT.TGraphErrors(nn, logen, fwd_meanDiff, zero, fwd_meanErr)
fwd_tgeMeanDiff.SetMarkerStyle(26)
fwd_tgeMeanDiff.SetLineColor(8)
fwd_tgeMeanDiff.SetMarkerColor(8)

tgeMeanRelDiff = ROOT.TGraphErrors(nn, logen, meanRelDiff, zero, meanRelErr)
tgeMeanRelDiff.SetMarkerStyle(24)
bar_tgeMeanRelDiff = ROOT.TGraphErrors(nn, logen, bar_meanRelDiff, zero, bar_meanRelErr)
bar_tgeMeanRelDiff.SetMarkerStyle(25)
bar_tgeMeanRelDiff.SetLineColor(4)
bar_tgeMeanRelDiff.SetMarkerColor(4)
fwd_tgeMeanRelDiff = ROOT.TGraphErrors(nn, logen, fwd_meanRelDiff, zero, fwd_meanRelErr)
fwd_tgeMeanRelDiff.SetMarkerStyle(26)
fwd_tgeMeanRelDiff.SetLineColor(8)
fwd_tgeMeanRelDiff.SetMarkerColor(8)

tgeWidth = ROOT.TGraphErrors(nn, realen, width, zero, widthErr)
tgeWidth.SetMarkerStyle(24)
bar_tgeWidth = ROOT.TGraphErrors(nn, realen, bar_width, zero, bar_widthErr)
bar_tgeWidth.SetMarkerStyle(25)
bar_tgeWidth.SetLineColor(4)
bar_tgeWidth.SetMarkerColor(4)
fwd_tgeWidth = ROOT.TGraphErrors(nn, realen, fwd_width, zero, fwd_widthErr)
fwd_tgeWidth.SetMarkerStyle(26)
fwd_tgeWidth.SetLineColor(8)
fwd_tgeWidth.SetMarkerColor(8)

tgeRelWidth = ROOT.TGraphErrors(nn, realen, relWidth, zero, relWidthErr)
tgeRelWidth.SetMarkerStyle(24)
bar_tgeRelWidth = ROOT.TGraphErrors(nn, realen, bar_relWidth, zero, bar_relWidthErr)
bar_tgeRelWidth.SetMarkerStyle(25)
bar_tgeRelWidth.SetLineColor(4)
bar_tgeRelWidth.SetMarkerColor(4)
fwd_tgeRelWidth = ROOT.TGraphErrors(nn, realen, fwd_relWidth, zero, fwd_relWidthErr)
fwd_tgeRelWidth.SetMarkerStyle(26)
fwd_tgeRelWidth.SetLineColor(8)
fwd_tgeRelWidth.SetMarkerColor(8)

tgeRelWidthSq = ROOT.TGraphErrors(nn, invsqren, relWidth, zero, relWidthErr)
tgeRelWidthSq.SetMarkerStyle(24)
bar_tgeRelWidthSq = ROOT.TGraphErrors(nn, invsqren, bar_relWidth, zero, bar_relWidthErr)
bar_tgeRelWidthSq.SetMarkerStyle(25)
bar_tgeRelWidthSq.SetLineColor(4)
bar_tgeRelWidthSq.SetMarkerColor(4)
fwd_tgeRelWidthSq = ROOT.TGraphErrors(nn, invsqren, fwd_relWidth, zero, fwd_relWidthErr)
fwd_tgeRelWidthSq.SetMarkerStyle(26)
fwd_tgeRelWidthSq.SetLineColor(8)
fwd_tgeRelWidthSq.SetMarkerColor(8)

cc.Clear()
cc.Divide(2,2)
cc.cd(1)
tgeMean.Draw('apl')
tgeMean.GetHistogram().GetXaxis().SetTitle('photon Energy [GeV]')
tgeMean.GetHistogram().GetYaxis().SetTitle('mean sum PFO energies [GeV]')
tgeMean.GetHistogram().SetTitle('single photons')
bar_tgeMean.Draw('same;pl')
fwd_tgeMean.Draw('same;pl')

tt.SetTextColor(1)
tt.DrawLatexNDC(0.2, 0.8, 'all')
tt.SetTextColor(4)
tt.DrawLatexNDC(0.2, 0.7, 'BAR')
tt.SetTextColor(8)
tt.DrawLatexNDC(0.2, 0.6, 'FWD')


cc.cd(2)
tgeMeanDiff.Draw('apl')
tgeMeanDiff.GetHistogram().GetXaxis().SetTitle('log10 (photon Energy [GeV])')
tgeMeanDiff.GetHistogram().GetYaxis().SetTitle('PFOs en - MC en [GeV]')
tgeMeanDiff.GetHistogram().SetTitle('single photons')
bar_tgeMeanDiff.Draw('same;pl')
fwd_tgeMeanDiff.Draw('same;pl')

cc.cd(3)
tgeMeanRelDiff.Draw('apl')
tgeMeanRelDiff.GetHistogram().GetXaxis().SetTitle('log10 (photon Energy [GeV])')
tgeMeanRelDiff.GetHistogram().GetYaxis().SetTitle('(PFOs en - MC en)/MCen')
tgeMeanRelDiff.GetHistogram().SetTitle('single photons')
bar_tgeMeanRelDiff.Draw('same;pl')
fwd_tgeMeanRelDiff.Draw('same;pl')

cc.Print(planme)

cc.Clear()
cc.Divide(1,2)
cc.cd(1)

resfn1=ROOT.TF1('resfun','(0.02**2 + (0.17/(x**0.5))**2)**0.5',0,120)
resfn1.SetLineColor(2)

tgeRelWidth.Draw('apl')
resfn1.Draw('same')
tgeRelWidth.Draw('same;pl')
tgeRelWidth.GetHistogram().GetXaxis().SetTitle('photon Energy [GeV]')
tgeRelWidth.GetHistogram().GetYaxis().SetTitle('PFO en width/mean')
tgeRelWidth.GetHistogram().SetTitle('single photons')
bar_tgeRelWidth.Draw('same;pl')
fwd_tgeRelWidth.Draw('same;pl')

tt.SetTextColor(2)
tt.DrawLatexNDC(0.5, 0.5, '#sigma/#mu = 18%/#sqrt{E} #oplus 2% (for comparison)')

tt.SetTextColor(1)
tt.DrawLatexNDC(0.2, 0.8, 'all')
tt.SetTextColor(4)
tt.DrawLatexNDC(0.2, 0.7, 'BAR')
tt.SetTextColor(8)
tt.DrawLatexNDC(0.2, 0.6, 'FWD')


cc.cd(2)
resfn=ROOT.TF1('resfun','(0.02**2 + (0.17*x)**2)**0.5',0,3)
resfn.SetLineColor(2)

tgeRelWidthSq.Draw('apl')
resfn.Draw('same')
tgeRelWidthSq.Draw('same;pl')
tgeRelWidthSq.GetHistogram().GetXaxis().SetTitle('1/sqrt(MC en [GeV])')
tgeRelWidthSq.GetHistogram().GetYaxis().SetTitle('PFO en width/mean')
tgeRelWidthSq.GetHistogram().SetTitle('single photons')
tt.DrawLatexNDC(0.5, 0.5, '#sigma/#mu = 18%/#sqrt{E} #oplus 2% (for comparison)')
bar_tgeRelWidthSq.Draw('same;pl')
fwd_tgeRelWidthSq.Draw('same;pl')

cc.Print(planme)


cc.Print(planme+']')

rfile.Close()
