// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "MINERvA_SignalDef.h"

#include "MINERvA_CCinc_XSec_1DEnu_nu.h"

//********************************************************************
MINERvA_CCinc_XSec_1DEnu_nu::MINERvA_CCinc_XSec_1DEnu_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type,
						     std::string fakeDataFile){
//********************************************************************

  // Measurement Details
  fName = name;
  fPlotTitles = "; Neutrino energy (GeV); d#sigma/dE_{#nu} (cm^{2}/GeV/nucleon)";
  EnuMin = 2.;
  EnuMax = 20.;
  target = "";
  fIsRawEvents = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  if      (name.find("C12")   != std::string::npos) target =   "C12";
  else if (name.find("Fe56")  != std::string::npos) target =  "Fe56";
  else if (name.find("Pb208") != std::string::npos) target = "Pb208";
  if      (name.find("DEN")   != std::string::npos) target =   "CH";
  if (target == "") ERR(WRN) << "target " << target << " was not found!" << std::endl;

  // Setup the Data Plots
  std::string basedir = FitPar::GetDataBase()+"/MINERvA/CCinc/";
  std::string smearfilename  = "CCinc_"+target+"_x_smear.csv";
  int nbins = 8;
  double bins[9] = {2, 3, 4, 5, 6, 8, 10, 15, 20};

  // Make a dummy fDataHist so it is used to construct other histograms...
  this->fDataHist = new TH1D(name.c_str(),(name+fPlotTitles).c_str(),nbins,bins);

  // Setup Default MC Histograms
  this->SetupDefaultHist();

  // Set Scale Factor (EventHist/nucleons) so I don't need to know what the target is here
  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38/(fNEvents+0.))/this->TotalIntegratedFlux(); // NEUT

};

//********************************************************************
void MINERvA_CCinc_XSec_1DEnu_nu::FillEventVariables(FitEvent *event){
//********************************************************************

  Enu  = (event->PartInfo(0))->fP.E()/1000.0;

  // Get the relevant signal information
  for (UInt_t j = 0; j < event->Npart(); ++j){

    if ((event->PartInfo(j))->fPID != 13) continue;

    ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());
    break;
  }

  fXVar   = Enu;
  return;
}



//********************************************************************
bool MINERvA_CCinc_XSec_1DEnu_nu::isSignal(FitEvent *event){
//*******************************************************************

  // Throw away NC events
  if (event->Mode > 30) return false;

  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Restrict the phase space to theta < 17 degrees
  if (ThetaMu > 0.296706) return false;

  // restrict energy range
  if (Enu < this->EnuMin || Enu > this->EnuMax) return false;

  return true;
};

//********************************************************************
void MINERvA_CCinc_XSec_1DEnu_nu::ScaleEvents(){
//********************************************************************

  // Get rid of this because it causes odd behaviour
  //Measurement1D::ScaleEvents();

  this->fMCHist->Scale(this->fScaleFactor, "width");

  // Proper error scaling - ROOT Freaks out with xsec weights sometimes
  for(int i=0; i<this->fMCStat->GetNbinsX();i++) {

    if (this->fMCStat->GetBinContent(i+1) != 0)
      this->fMCHist->SetBinError(i+1, this->fMCHist->GetBinContent(i+1) * this->fMCStat->GetBinError(i+1) / this->fMCStat->GetBinContent(i+1) );
    else this->fMCHist->SetBinError(i+1, this->fMCHist->Integral());
  }

}
