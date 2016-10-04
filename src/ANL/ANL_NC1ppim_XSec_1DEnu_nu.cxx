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
#include "ANL_NC1ppim_XSec_1DEnu_nu.h"

/** 
  * M. Derrick et al., "Study of the reaction \nu n \rightarrow \nu p \pi^-", Physics Letters, Volume 92B, Number 3,4, 363, 19 May 1980
*/

ANL_NC1ppim_XSec_1DEnu_nu::ANL_NC1ppim_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  measurementName = "ANL_NC1ppim_XSec_1DEnu_nu";
  plotTitles = "; E_{#nu};#sigma(E_{#nu}) (cm^{2}/nucleon)";
  EnuMin = 0.3;
  EnuMax = 1.5;
  isDiag = true;
  isRawEvents = false;
  isEnu1D = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/ANL/NC1ppim/ANL_NC1ppim_Enu_xsec.csv");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar = StatUtils::GetInvert(fullcovar);

  // Scale to cross-section
  dataHist->Scale(1.E-41);

  this->scaleFactor = this->eventHist->Integral("width")/((nevents+0.)*fluxHist->Integral("width"))*(16./8.);
};

void ANL_NC1ppim_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  // Very simple here!
  double Enu = ((event->PartInfo(0))->fP).E()/1000.;

  this->X_VAR = Enu;

  return;
};

bool ANL_NC1ppim_XSec_1DEnu_nu::isSignal(FitEvent *event) {

  // Incoming particle should be a neutrino
  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false; 

  // Outgoing particle should be a neutrino
  if (((event->PartInfo(2))->fPID != 14) && ((event->PartInfo(3))->fPID != 14)) return false; 

  int pimCnt = 0;
  int protonCnt = 0;

  for (UInt_t j =  2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == -211) {
      pimCnt++;
    } else if (PID == 2212) {
      protonCnt++;
    } else {
      return false; // require only three prong events! (allow photons?)
    }
  }

  // don't think there's away of implementing spectator proton cuts in NEUT?
  // 100 MeV or larger protons

  if (pimCnt != 1) return false;
  if (protonCnt != 1) return false;

  return true;
}
