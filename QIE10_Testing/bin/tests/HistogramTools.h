char hist_name[512];
char dir_name[512];
char hist_full_name[512];
char figure_name[512];
string sideName;
TH1F *histEV = new TH1F();
TDirectory *dir = new TDirectory();
TCanvas *canv = new TCanvas("canv","canv",100,100,1024,768);

struct histData {
  bool exists;
  TH1F *hist;
};

void printEV (char* hist_name_in, int run_num, TFile *file, const char *Folder_NAME) {
  sprintf(hist_name,"%s_EV",hist_name_in);
  sprintf(hist_full_name,"%s/%s",hist_name,hist_name);
  sprintf(figure_name,"../../img/%i/%s/%s.png",run_num,Folder_NAME,hist_name);
  histEV = (TH1F*)file->Get(hist_full_name);
  canv->cd();
  histEV->Draw("box");
  canv->SaveAs(figure_name);
  canv->Clear();
  histEV->Delete();
  canv->Clear();
}

histData processCH (char* hist_name_in, int run_num, int* coords, TFile *file) {

  histData outData;

  sprintf(dir_name,"%s_CH",hist_name_in);
  dir = (TDirectory*)file->Get(dir_name);
  sideName="M";
  if (coords[0]>0){sideName="P";}
  sprintf(hist_name,"%s_depth%i_iphi%i_ieta%i",dir_name,coords[3],coords[2],coords[1]);
  sprintf(hist_name,"%s_HF%s0%i_slot%i_channel%i",dir_name,sideName.c_str(),coords[4],coords[5],coords[6]);
  outData.exists = dir->GetListOfKeys()->Contains(hist_name);
  sprintf(hist_full_name,"%s/%s",dir_name,hist_name);

  if ( outData.exists == 0 ) {
    cout << "iEta: " << coords[1];
    cout << " iPhi: " << coords[2];
    cout << " Depth: " << coords[3];
    cout << " FE_crate: " << coords[4];
    cout << " FE_slot: " << coords[5];
    cout << " FE_channel: " << coords[6];
    cout << "   <<<<<<<<<< MISSING HISTOGRAM";
    cout << endl;
  } else {
    outData.hist = (TH1F*)file->Get(hist_full_name);    
  }

  return outData;

}
