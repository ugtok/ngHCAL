#include <iostream>
#include <sstream>
#include <array>
#include <string>
#include <fstream>

#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"

#include "../../src/draw_map_full.h"
#include "../../src/ActiveChannels.h"
#include "../../src/DETcoords2FEcoords.h"
#include "./ManageCoords.h"
#include "./HistogramTools.h"

// using namespace std;


void ped_test(Int_t run_num, Int_t SUITE_CODE, const char *Folder_NAME) {

  //////// DECLARATIONS AND INITIALIZATIONS
  char root_file_name[512];
  TFile *file0 =  new TFile();
  sprintf(root_file_name,"../../dat/QIE10testing_%i_%d.root",run_num, SUITE_CODE);
  file0 = TFile::Open(root_file_name);
  char dir_name[512];
  sprintf(dir_name,"mkdir -p ../../img/%i/%s/rootFiles",run_num,Folder_NAME);
  system(dir_name);
  char hist0_name[512];
  histData hist0;
  string sideName;
  int* coords;
  
  float ped_mean_low = 2.0;
  float ped_mean_high = 6.5;
  float ped_rms_low = 0.1;
  float ped_rms_high = 2.0;

  //////// EV PLOTS
  sprintf(hist0_name,"%s","ADC_spectrum");
  printEV(hist0_name,run_num,file0,Folder_NAME);
  
  sprintf(hist0_name,"%s","QSum");
  printEV(hist0_name,run_num,file0,Folder_NAME);
  
  sprintf(hist0_name,"%s","Qav");
  printEV(hist0_name,run_num,file0,Folder_NAME);
  
  sprintf(hist0_name,"%s","T_abs");
  printEV(hist0_name,run_num,file0,Folder_NAME);

  TCanvas *canv = new TCanvas("canv","canv",100,100,1024,768);


  /////// INITIALIZE GENERAL ERROR MAP
  int**** lv2_err_map_gen = create_error_map();
  for (int i=0; i<numActiveChannels; i++) {
    coords = setCoords(i);
    lv2_err_map_gen[coords[0]][coords[4]-1][coords[5]-1][coords[6]-1] = 1;
  }

  ////// PER TEST ERROR MAPS
  int**** lv2_err_map_mean = create_error_map();
  int**** lv2_err_map_rms = create_error_map();


  ///////// CH PLOTS
  for (int i=0; i<numActiveChannels; i++) {

    ///// GET COORDINATES
    coords = setCoords(i);

    ///// Mean Rms Check
    sprintf(hist0_name,"%s","ADC_spectrum");
    hist0 = processCH(hist0_name,run_num,coords,file0);
    if ((hist0.exists == 0) ||  ( hist0.hist->GetMean() < ped_mean_low ) || ( hist0.hist->GetMean() > ped_mean_high) || ( hist0.hist->GetEntries() < 10)) {
      lv2_err_map_mean[coords[0]][coords[4]-1][coords[5]-1][coords[6]-1] = 0;
      lv2_err_map_gen[coords[0]][coords[4]-1][coords[5]-1][coords[6]-1] = 0;
      if (hist0.exists == 1) {
      cout << "MEAN ERROR !!!!!! "<< endl;
      cout << "SIDE:"<< coords[0]  << " Crate:"<< coords[4] <<" Slot:"<< coords[5] <<" Channel:"<< coords[6] << "  -------->" << " Mean Value: " << hist0.hist->GetMean() << endl;
      cout << "SIDE:"<< coords[0]  << " Eta:"<< coords[1] <<" Phi:"<< coords[2] <<" Depth:"<< coords[3] << "      -------->" << " Mean Value: " << hist0.hist->GetMean() << endl;
      cout << "----------------------------------------------------------------ooo------------------------------------------------------------------------------------ "<< endl;

      canv->cd();
      hist0.hist->Draw();
      canv->SetLogy();
      sideName="M";
      if (coords[0]>0){sideName="P";}
      sprintf(hist0_name,"../../img/%i/%s/%s_HF%s0%i_slot%i_channel%i.png",run_num,"ped_test","ADC_spectrum",sideName.c_str(),coords[4],coords[5],coords[6]);
      canv->SaveAs(hist0_name);

      }
    } else {
      lv2_err_map_mean[coords[0]][coords[4]-1][coords[5]-1][coords[6]-1] = 1;    
      hist0.hist->Delete();
    }
    
    sprintf(hist0_name,"%s","ADC_spectrum");
    hist0 = processCH(hist0_name,run_num,coords,file0);	
    if ((hist0.exists == 0) ||  ( hist0.hist->GetRMS() < ped_rms_low ) || ( hist0.hist->GetRMS() > ped_rms_high) || ( hist0.hist->GetEntries() < 10)) {
      lv2_err_map_rms[coords[0]][coords[4]-1][coords[5]-1][coords[6]-1] = 0;
      lv2_err_map_gen[coords[0]][coords[4]-1][coords[5]-1][coords[6]-1] = 0;
      if (hist0.exists == 1) {
      cout << "RMS ERROR !!!!!! "<< endl;
      cout << "SIDE:"<< coords[0] << " Crate:"<< coords[4] <<" Slot:"<< coords[5] <<" Channel:"<< coords[6] << " -------->" << " RMS Value: " << hist0.hist->GetRMS() << endl;
      cout << "SIDE:"<< coords[0]  << " Eta:"<< coords[1] <<" Phi:"<< coords[2] <<" Depth:"<< coords[3] << " -------->" << " RMS Value: " << hist0.hist->GetRMS() << endl;
      cout << "----------------------------------------------------------------ooo------------------------------------------------------------------------------------ "<< endl;
      canv->cd();
      hist0.hist->Draw();
      canv->SetLogy();
      sideName="M";
      if (coords[0]>0){sideName="P";}
      sprintf(hist0_name,"../../img/%i/%s/%s_HF%s0%i_slot%i_channel%i.png",run_num,"ped_test","ADC_spectrum",sideName.c_str(),coords[4],coords[5],coords[6]);
      canv->SaveAs(hist0_name);

      }
    } else {
      lv2_err_map_rms[coords[0]][coords[4]-1][coords[5]-1][coords[6]-1] = 1;    
      hist0.hist->Delete();
    }
    
    
  } // close ch plots
  
  

  ///// DRAW ERROR MAPS
  draw_map(lv2_err_map_gen, run_num, Folder_NAME, "Pedestal Test" );
  draw_map(lv2_err_map_mean, run_num, Folder_NAME, "Pedestal Mean" );
  draw_map(lv2_err_map_rms, run_num, Folder_NAME, "Pedestal RMS" );


} // close function
