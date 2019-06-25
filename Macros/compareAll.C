//Produce overlaid comparison plots of CRY-1, CRY-2, and DBY histograms with the same binning
//Based off old_compareAll.CRY & plotCRY_standardized.C
//Ben Barton
//bbarton@virginia.edu

//Number of cut variations to plot (not including no cuts as a variation)
const int NCUTS = 2;

//Struct to hold parameters for a histograms in a contained unit 
//Arrays should be the length of the number of cut variations to be plotted
typedef struct{
  double xMins[NCUTS + 1];
  double xMaxs[NCUTS + 1];
  int nBins[NCUTS + 1];
  string xTitle;
  string title;
} histParams;


/**********************************************************************************************************************************************************************************/
//////////////////////////////////////////////////////////////    Define Cuts    ///////////////////////////////////////////////////////////////////////////////////////////////////

//Signal cuts
string momentum_cut = "deent.mom>100 && deent.mom<110"; 
string no_upstream = "ue.status<0";
string trk_qual = "dequal.TrkQualDeM>0.8";; //For original CRY1 analysis this was 0.4
string trk_cut_pid = "dequal.TrkPIDDeM>0.5";
string pitch_angle = "deent.td>0.57735027 && deent.td<1"; //  Excludes beam particles
string min_trans_R = "deent.d0>-80 && deent.d0<105"; //  Consistent with coming from the target
string max_trans_R = "(deent.d0+2.0/deent.om)>450. && (deent.d0+2.0/deent.om)<680."; //  Inconsistent with hitting the proton absorber
string timing_cut = "de.t0>700 && de.t0<1600"; // Official cuts 1695 upper threshold
string all_cuts_MDC = momentum_cut + "&&" + no_upstream + "&&" + trk_qual + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R;
string signalCuts = all_cuts_MDC + "&&" + trk_cut_pid + "&&" + timing_cut;
string noMom = no_upstream + "&&" + trk_qual + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R + "&&" + trk_cut_pid + "&&" + timing_cut; 

//Alternative cuts
string d0is0 = "demcent.d0==0";
string noMomNoPID = trk_qual + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R;
string testCut = no_upstream + "&&" + trk_qual + "&&" + pitch_angle + "&&" + min_trans_R;

//Cut lists & identifiers // Add any additional cuts that you want to make to this list
string cuts[NCUTS + 1] = {"", signalCuts, noMom};
string cutIDs[NCUTS + 1] = {"", "sigCuts", "noMomCut"};

/**********************************************************************************************************************************************************************************/
//////////////////////////////////////////////////////    Define Standard Parameters for Each Variable   ///////////////////////////////////////////////////////////////////////////


//This seems clunky but is the most efficient way to define a scalable number of histograms with predefined characteristics. Initializing new Histograms for each would be
//innefficient and messy.To add a new histogram

//Set the parameters for the histograms
vector<histParams> hist_params;

void defHistParams()
{
  histParams params_crvinfomc_x0;
  params_crvinfomc_x0.title = "crvinfomc._x[0]";
  params_crvinfomc_x0.xTitle = "MC Truth: x at CRV (mm)";
  params_crvinfomc_x0.xMins[0] = -7000; //No cut limits
  params_crvinfomc_x0.xMaxs[0] = 0;
  params_crvinfomc_x0.nBins[0] = 100;
  params_crvinfomc_x0.xMins[1] = -7000; //Signal cut limits
  params_crvinfomc_x0.xMaxs[1] = -800;
  params_crvinfomc_x0.nBins[1] = 100;
  params_crvinfomc_x0.xMins[2] = -7000; //Expanded mom window limits
  params_crvinfomc_x0.xMaxs[2] = -800;
  params_crvinfomc_x0.nBins[2] = 100;
  hist_params.push_back(params_crvinfomc_x0);
  
  histParams params_crvinfomc_y0;
  params_crvinfomc_y0.title = "crvinfomc._y[0]";
  params_crvinfomc_y0.xTitle = "MC Truth: y at CRV (mm)";
  params_crvinfomc_y0.xMins[0] = -2000; //No cut limits
  params_crvinfomc_y0.xMaxs[0] = 3000;
  params_crvinfomc_y0.nBins[0] = 100;
  params_crvinfomc_y0.xMins[1] = -2000; //Signal cut limits
  params_crvinfomc_y0.xMaxs[1] = 3000;
  params_crvinfomc_y0.nBins[1] = 100;
  params_crvinfomc_y0.xMins[2] = -2000; //Expanded mom window limits
  params_crvinfomc_y0.xMaxs[2] = 3000;
  params_crvinfomc_y0.nBins[2] = 100;
  hist_params.push_back(params_crvinfomc_y0);

  histParams params_crvinfomc_z0;
  params_crvinfomc_z0.title = "crvinfomc._z[0]";
  params_crvinfomc_z0.xTitle = "MC Truth: z at CRV (mm)";
  params_crvinfomc_z0.xMins[0] = -3000; //No cut limits
  params_crvinfomc_z0.xMaxs[0] = 20000;
  params_crvinfomc_z0.nBins[0] = 100;
  params_crvinfomc_z0.xMins[1] = 0; //Signal cut limits
  params_crvinfomc_z0.xMaxs[1] = 19000;
  params_crvinfomc_z0.nBins[1] = 100;
  params_crvinfomc_z0.xMins[2] = -2000; //Expanded mom window limits
  params_crvinfomc_z0.xMaxs[2] = 19000;
  params_crvinfomc_z0.nBins[2] = 100;
  hist_params.push_back(params_crvinfomc_z0);

  histParams params_crvinfomc_primaryX;
  params_crvinfomc_primaryX.title = "crvinfomc._primaryX";
  params_crvinfomc_primaryX.xTitle = "Primary Particle x Position (mm)";
  params_crvinfomc_primaryX.xMins[0] = -80000; //No cut limits
  params_crvinfomc_primaryX.xMaxs[0] = 80000;
  params_crvinfomc_primaryX.nBins[0] = 100;
  params_crvinfomc_primaryX.xMins[1] = -80000; //Signal cut limits
  params_crvinfomc_primaryX.xMaxs[1] = 80000;
  params_crvinfomc_primaryX.nBins[1] = 100;
  params_crvinfomc_primaryX.xMins[2] = -80000; //Expanded mom window limits
  params_crvinfomc_primaryX.xMaxs[2] = 80000;
  params_crvinfomc_primaryX.nBins[2] = 100;
  hist_params.push_back(params_crvinfomc_primaryX);

  histParams params_crvinfomc_primaryY;
  params_crvinfomc_primaryY.title = "crvinfomc._primaryY";
  params_crvinfomc_primaryY.xTitle = "Primary Particle y Position (mm)";
  params_crvinfomc_primaryY.xMins[0] = 15364; //No cut limits
  params_crvinfomc_primaryY.xMaxs[0] = 15410;
  params_crvinfomc_primaryY.nBins[0] = 100;
  params_crvinfomc_primaryY.xMins[1] = 15364; //Signal cut limits
  params_crvinfomc_primaryY.xMaxs[1] = 15410;
  params_crvinfomc_primaryY.nBins[1] = 100;
  params_crvinfomc_primaryY.xMins[2] = 15365; //Expanded mom window limits
  params_crvinfomc_primaryY.xMaxs[2] = 15410;
  params_crvinfomc_primaryY.nBins[2] = 100;
  hist_params.push_back(params_crvinfomc_primaryY);

  histParams params_crvinfomc_primaryZ;
  params_crvinfomc_primaryZ.title = "crvinfomc._primaryZ";
  params_crvinfomc_primaryZ.xTitle = "Primary Particle z Position (mm)";
  params_crvinfomc_primaryZ.xMins[0] = -80000; //No cut limits
  params_crvinfomc_primaryZ.xMaxs[0] = 80000;
  params_crvinfomc_primaryZ.nBins[0] = 100;
  params_crvinfomc_primaryZ.xMins[1] = -80000; //Signal cut limits
  params_crvinfomc_primaryZ.xMaxs[1] = 80000;
  params_crvinfomc_primaryZ.nBins[1] = 100;
  params_crvinfomc_primaryZ.xMins[2] = -80000; //Expanded mom window limits
  params_crvinfomc_primaryZ.xMaxs[2] = 80000;
  params_crvinfomc_primaryZ.nBins[2] = 100;
  hist_params.push_back(params_crvinfomc_primaryZ);

  histParams params_crvinfomc_primaryPdgId;
  params_crvinfomc_primaryPdgId.title = "crvinfomc._primaryPdgId";
  params_crvinfomc_primaryPdgId.xTitle = "Primary Particle PDG ID";
  params_crvinfomc_primaryPdgId.xMins[0] = -225; //No cut limits
  params_crvinfomc_primaryPdgId.xMaxs[0] = 2225;
  params_crvinfomc_primaryPdgId.nBins[0] = 100;
  params_crvinfomc_primaryPdgId.xMins[1] = -15; //Signal cut limits
  params_crvinfomc_primaryPdgId.xMaxs[1] = 15;
  params_crvinfomc_primaryPdgId.nBins[1] = 30;
  params_crvinfomc_primaryPdgId.xMins[2] = -15; //Expanded mom window limits
  params_crvinfomc_primaryPdgId.xMaxs[2] = 30;
  params_crvinfomc_primaryPdgId.nBins[2] = 15;
  hist_params.push_back(params_crvinfomc_primaryPdgId);

  histParams params_crvinfomc_primaryE;
  params_crvinfomc_primaryE.title = "crvinfomc._primaryE";
  params_crvinfomc_primaryE.xTitle = "Primary Particle Energy (MeV)";
  params_crvinfomc_primaryE.xMins[0] = 0; //No cut limits
  params_crvinfomc_primaryE.xMaxs[0] = 4300000;
  params_crvinfomc_primaryE.nBins[0] = 100;
  params_crvinfomc_primaryE.xMins[1] = 0; //Signal cut limits
  params_crvinfomc_primaryE.xMaxs[1] = 1000000;
  params_crvinfomc_primaryE.nBins[1] = 100;
  params_crvinfomc_primaryE.xMins[2] = 0; //Expanded mom window limits
  params_crvinfomc_primaryE.xMaxs[2] = 1000000;
  params_crvinfomc_primaryE.nBins[2] = 100;
  hist_params.push_back(params_crvinfomc_primaryE);

  histParams params_deent_mom;
  params_deent_mom.title = "deent.mom";
  params_deent_mom.xTitle = "Downstream e^- : Momentum (MeV/c)";
  params_deent_mom.xMins[0] = 40; //No cut params
  params_deent_mom.xMaxs[0] = 300;
  params_deent_mom.nBins[0] = 100;
  params_deent_mom.xMins[1] = 100; //Signal cut params
  params_deent_mom.xMaxs[1] = 110;
  params_deent_mom.nBins[1] = 20; 
  params_deent_mom.xMins[2] = 60; //Expanded mom window cut params
  params_deent_mom.xMaxs[2] = 160;
  params_deent_mom.nBins[2] = 100;
  hist_params.push_back(params_deent_mom);
  
  histParams params_deent_d0;
  params_deent_d0.title = "deent.d0";
  params_deent_d0.xTitle = "Downstream e^- : Distance to Center (mm)";
  params_deent_d0.xMins[0] = -600; //No cut params
  params_deent_d0.xMaxs[0] = 600;
  params_deent_d0.nBins[0] = 100;
  params_deent_d0.xMins[1] = -80; //Signal cut params
  params_deent_d0.xMaxs[1] = 110;
  params_deent_d0.nBins[1] = 100;
  params_deent_d0.xMins[2] = -80; //Expanded mom window cut params
  params_deent_d0.xMaxs[2] = 110;
  params_deent_d0.nBins[2] = 100;
  hist_params.push_back(params_deent_d0);

  histParams params_deent_td; 
  params_deent_td.title = "deent.td";
  params_deent_td.xTitle = "Downstream e^- : Pitch Angle";
  params_deent_td.xMins[0] = 0; //No cut params
  params_deent_td.xMaxs[0] = 5;
  params_deent_td.nBins[0] = 100;
  params_deent_td.xMins[1] = 0.55; //Signal cut params
  params_deent_td.xMaxs[1] = 1.05;
  params_deent_td.nBins[1] = 100;
  params_deent_td.xMins[2] = 0.55; //Expanded mom window cut params
  params_deent_td.xMaxs[2] = 1.05;
  params_deent_td.nBins[2] = 100;
  hist_params.push_back(params_deent_td);

  histParams params_de_t0;
  params_de_t0.title = "de.t0";
  params_de_t0.xTitle = "Downstream e^- : t_0 (s)";
  params_de_t0.xMins[0] = 400; //No cut params
  params_de_t0.xMaxs[0] = 1700;
  params_de_t0.nBins[0] = 100;
  params_de_t0.xMins[1] = 500; //Signal cut params
  params_de_t0.xMaxs[1] = 1700;
  params_de_t0.nBins[1] = 100;
  params_de_t0.xMins[2] = 500; //Expanded mom window cut params
  params_de_t0.xMaxs[2] = 1700;
  params_de_t0.nBins[2] = 100;
  hist_params.push_back(params_de_t0);

  histParams params_dequal_trkQualDeM;
  params_dequal_trkQualDeM.title = "dequal.TrkQualDeM";
  params_dequal_trkQualDeM.xTitle = "Downstream e^- : Track Quality";
  params_dequal_trkQualDeM.xMins[0] = 0; //No cut params
  params_dequal_trkQualDeM.xMaxs[0] = 1;
  params_dequal_trkQualDeM.nBins[0] = 100;
  params_dequal_trkQualDeM.xMins[1] = 0.5; //Signal cut params
  params_dequal_trkQualDeM.xMaxs[1] = 1;
  params_dequal_trkQualDeM.nBins[1] = 100;
  params_dequal_trkQualDeM.xMins[2] = 0.5; //Expanded mom window cut params
  params_dequal_trkQualDeM.xMaxs[2] = 1;
  params_dequal_trkQualDeM.nBins[2] = 100;
  hist_params.push_back(params_dequal_trkQualDeM);

  histParams params_demcent_d0;
  params_demcent_d0.title = "demcent.d0";
  params_demcent_d0.xTitle = "Downstream e^- : MC Truth Distance to Center (mm)";
  params_demcent_d0.xMins[0] = -800; //No cut params
  params_demcent_d0.xMaxs[0] = 600;
  params_demcent_d0.nBins[0] = 100;
  params_demcent_d0.xMins[1] = -250; //Signal cut params
  params_demcent_d0.xMaxs[1] = 150;
  params_demcent_d0.nBins[1] = 100;
  params_demcent_d0.xMins[2] = -250; //Expanded mom window cut params
  params_demcent_d0.xMaxs[2] = 150;
  params_demcent_d0.nBins[2] = 100;
  hist_params.push_back(params_demcent_d0);

  histParams params_delta_det0_CRVtimeWindowStart;
  params_delta_det0_CRVtimeWindowStart.title = "de.t0 - crvinfo._timeWindowStart";
  params_delta_det0_CRVtimeWindowStart.xTitle = "Time Delta (s)";
  params_delta_det0_CRVtimeWindowStart.xMins[0] = -200; //No cut params
  params_delta_det0_CRVtimeWindowStart.xMaxs[0] = 200;
  params_delta_det0_CRVtimeWindowStart.nBins[0] = 100;
  params_delta_det0_CRVtimeWindowStart.xMins[1] = -200; //Signal cuts params
  params_delta_det0_CRVtimeWindowStart.xMaxs[1] = 200;
  params_delta_det0_CRVtimeWindowStart.nBins[1] = 100;
  params_delta_det0_CRVtimeWindowStart.xMins[2] = -200; //Expanded mom cut params
  params_delta_det0_CRVtimeWindowStart.xMaxs[2] = 200;
  params_delta_det0_CRVtimeWindowStart.nBins[2] = 100;
  hist_params.push_back(params_delta_det0_CRVtimeWindowStart);

  histParams params_pz_p;
  params_pz_p.title = "p_z / p";
  params_pz_p.xTitle = "p_z / p";
  params_pz_p.xMins[0] = -1; //No cuts params
  params_pz_p.xMaxs[0] = 1;
  params_pz_p.nBins[0] = 100;
  params_pz_p.xMins[1] = -1; //Signal cuts params
  params_pz_p.xMaxs[1] = 1;
  params_pz_p.nBins[1] = 100;
  params_pz_p.xMins[2] = -1; //Expanded mom cut params
  params_pz_p.xMaxs[2] = 1;
  params_pz_p.nBins[2] = 100;
  hist_params.push_back(params_pz_p);

  histParams params_noCRV_demc_pdg;
  params_noCRV_demc_pdg.title = "demc.pdg";
  params_noCRV_demc_pdg.xTitle = "Particle PDG";
  params_noCRV_demc_pdg.xMins[0] = -14.5; //No cuts params
  params_noCRV_demc_pdg.xMaxs[0] = 15.5;
  params_noCRV_demc_pdg.nBins[0] = 30;
  params_noCRV_demc_pdg.xMins[1] = -14.5; //Signal cuts params
  params_noCRV_demc_pdg.xMaxs[1] = 15.5;
  params_noCRV_demc_pdg.nBins[1] = 30;
  params_noCRV_demc_pdg.xMins[2] = -14.5; //Expanded mom cut params
  params_noCRV_demc_pdg.xMaxs[2] = 15.5;
  params_noCRV_demc_pdg.nBins[2] = 30;
  hist_params.push_back(params_noCRV_demc_pdg); 

  histParams params_demcpri_posz;
  params_demcpri_posz.title = "demcpri.posz";
  params_demcpri_posz.xTitle = "MC Truth Primary z Position (mm)";
  params_demcpri_posz.xMins[0] = -80000; //No cut params
  params_demcpri_posz.xMaxs[0] = 80000;
  params_demcpri_posz.nBins[0] = 100;
  params_demcpri_posz.xMins[1] = -80000; //Signal cut params
  params_demcpri_posz.xMaxs[1] = 80000;
  params_demcpri_posz.nBins[1] = 100;
  params_demcpri_posz.xMins[2] = -80000; //Expanded mom window cut params
  params_demcpri_posz.xMaxs[2] = 80000;
  params_demcpri_posz.nBins[2] = 100;
  hist_params.push_back(params_demcpri_posz);

  histParams params_demcpri_posx;
  params_demcpri_posx.title = "demcpri.posx";
  params_demcpri_posx.xTitle = "MC Truth Primary x Position (mm)";
  params_demcpri_posx.xMins[0] = -80000; //No cut params
  params_demcpri_posx.xMaxs[0] = 80000;
  params_demcpri_posx.nBins[0] = 100;
  params_demcpri_posx.xMins[1] = -80000; //Signal cut params
  params_demcpri_posx.xMaxs[1] = 80000;
  params_demcpri_posx.nBins[1] = 100;
  params_demcpri_posx.xMins[2] = -80000; //Expanded mom window cut params
  params_demcpri_posx.xMaxs[2] = 80000;
  params_demcpri_posx.nBins[2] = 100;
  hist_params.push_back(params_demcpri_posx);

  histParams params_demcpri_mom;
  params_demcpri_mom.title = "sqrt((demcpri.momx*demcpri.momx)+(demcpri.momy*demcpri.momy)+(demcpri.momz*demcpri.momz))";
  params_demcpri_mom.xTitle = "MC Truth Primary Momentum (MeV/c)";
  params_demcpri_mom.xMins[0] = 0; //No cut params
  params_demcpri_mom.xMaxs[0] = 1000000;
  params_demcpri_mom.nBins[0] = 100;
  params_demcpri_mom.xMins[1] = 0; //Signal cut params
  params_demcpri_mom.xMaxs[1] = 500000;
  params_demcpri_mom.nBins[1] = 100;
  params_demcpri_mom.xMins[2] = 0; //Expanded mom window cut params
  params_demcpri_mom.xMaxs[2] = 500000;
  params_demcpri_mom.nBins[2] = 100;
  hist_params.push_back(params_demcpri_mom);

}



/**********************************************************************************************************************************************************************************/
/////////////////////////////////////////////////////////////////    Make the Plots    /////////////////////////////////////////////////////////////////////////////////////////////

//Makes plots of CRY1, CRY2, & DBY overlaid for each histParam struct in defHistParams()
//Files are saved as pngs/pdfs
void compareAll()
{
  defHistParams(); //Define the histograms to be made

  gStyle->SetOptStat(0);

  TFile file_cry1("/mu2e/data/users/bbarton/CRY1/TrkAnaTrees/cry1_unmixedTrkAna.root");
  TFile file_cry2("/mu2e/data/users/bbarton/CRY2/TrkAnaTrees/cry2_trkana.root");
  TFile file_dby("/mu2e/data/users/bbarton/DBY/TrkAnaTrees/dby_trkana.root");
  TTree *tree_cry1 = (TTree*) file_cry1.Get("TrkAnaNeg/trkana");
  TTree *tree_cry2 = (TTree*) file_cry2.Get("TrkAnaNeg/trkana");
  TTree *tree_dby = (TTree*) file_dby.Get("TrkAnaNeg/trkana");

  TCanvas *canv = new TCanvas("canv","CRY-1, CRY-2, & DBY Comparisons",1800,600);

  for (int cutN = 0; cutN <= NCUTS; cutN++) //For each cut
    {
      for (int v = 0; v < hist_params.size(); v++) //For each variable
	{

	  histParams params = hist_params.at(v);

	  TH1F *hCRY1 = new TH1F("hCRY1", params.title.c_str(), params.nBins[cutN], params.xMins[cutN], params.xMaxs[cutN]);
	  TH1F *hCRY2 = new TH1F("hCRY2", params.title.c_str(), params.nBins[cutN], params.xMins[cutN], params.xMaxs[cutN]);;
	  TH1F *hDBY = new TH1F("hDBY", params.title.c_str(), params.nBins[cutN], params.xMins[cutN], params.xMaxs[cutN]);

	  if (params.title == "p_z / p") //Title of this hist is not the expression for which to fill it so it must be treated as a special case
	    {
	      tree_cry1->Draw(Form("%s>>+hCRY1","demcpri.momz/sqrt((demcpri.momx*demcpri.momx)+(demcpri.momy*demcpri.momy)+(demcpri.momz*demcpri.momz))"),cuts[cutN].c_str(),"goff");
	      tree_cry2->Draw(Form("%s>>+hCRY2","demcpri.momz/sqrt((demcpri.momx*demcpri.momx)+(demcpri.momy*demcpri.momy)+(demcpri.momz*demcpri.momz))"),cuts[cutN].c_str(),"goff");
	      tree_dby->Draw(Form("%s>>+hDBY","demcpri.momz/sqrt((demcpri.momx*demcpri.momx)+(demcpri.momy*demcpri.momy)+(demcpri.momz*demcpri.momz))"),cuts[cutN].c_str(),"goff");
	    }
	  else
	    {
	      //Get data from trees and store in histograms
	      tree_cry1->Draw(Form("%s>>+hCRY1", params.title.c_str()), cuts[cutN].c_str() , "goff");
	      tree_cry2->Draw(Form("%s>>+hCRY2", params.title.c_str()), cuts[cutN].c_str(), "goff");
	      tree_dby->Draw(Form("%s>>+hDBY", params.title.c_str()), cuts[cutN].c_str(), "goff");
	    }
	  
	  
	  hCRY1 = (TH1F*) gDirectory->Get("hCRY1");
	  hCRY2 = (TH1F*) gDirectory->Get("hCRY2");
	  hDBY = (TH1F*) gDirectory->Get("hDBY");

	  //Set plot style
	  hCRY1->SetLineColor(kBlue);
	  hCRY2->SetLineColor(kBlack);
	  hDBY->SetLineColor(kRed);
	  // hCRY1->SetFillColorAlpha(kBlue, 0.1);
	  // hCRY2->SetFillColorAlpha(kBlack, 0.1);
	  // hDBY->SetFillColorAlpha(kRed, 0.1);
	  // hCRY1->SetFillStyle(3004);
	  // hCRY2->SetFillStyle(3005);
	  // hDBY->SetFillStyle(3002);

	  //Scale to unit area
	  hCRY1->Scale(1.0 / hCRY1->Integral());
	  hCRY2->Scale(1.0 / hCRY2->Integral());
	  hDBY->Scale(1.0 / hDBY->Integral());
	  
	  //Label plots
	  hCRY1->SetTitle((params.title + " : " + cutIDs[cutN]).c_str());
	  hCRY2->SetTitle((params.title + " : " + cutIDs[cutN]).c_str());
	  hDBY->SetTitle((params.title + " : " + cutIDs[cutN]).c_str());
	  hCRY1->SetXTitle(params.xTitle.c_str());
	  hCRY2->SetXTitle(params.xTitle.c_str());
	  hDBY->SetXTitle(params.xTitle.c_str());
	 
	  //Reset plot to standard axis
	  canv->SetLogy(0);
	  
	  //Plot in the order s.t. the plot with the highest peak is plotted first in order to guarantee all plots will be completely visible
	  double maxCRY1 = hCRY1->GetBinContent(hCRY1->GetMaximumBin());
	  double maxCRY2 = hCRY2->GetBinContent(hCRY2->GetMaximumBin());
	  double maxDBY = hDBY->GetBinContent(hDBY->GetMaximumBin()); 
	  if (maxDBY > maxCRY1 && maxDBY > maxCRY2)
	    {
	      hDBY->Draw("hist");
	      hCRY1->Draw("hist same");
	      hCRY2->Draw("hist same");
	    }
	  else if (maxCRY2 > maxDBY && maxCRY2 > maxCRY2)
	    {
	      hCRY2->Draw("hist");
	      hCRY1->Draw("hist same");
	      hDBY->Draw("hist same");
	    }  
	  else
	    {
	      hCRY1->Draw("hist");
	      hCRY2->Draw("hist same");
	      hDBY->Draw("hist same");
	    }  
	  

	  //Add legend
	  TLegend *leg = new TLegend(0.75,0.7,0.85,0.9,"Entries");
	  leg->AddEntry(hCRY2,Form("CRY-2: %d", (int)hCRY2->GetEntries()),"l");
	  leg->AddEntry(hCRY1,Form("CRY-1: %d", (int)hCRY1->GetEntries()),"l");
	  leg->AddEntry(hDBY,Form("DBY: %d", (int)hDBY->GetEntries()),"l");
	  leg->Draw("same");

	  //Build a filename and save plots
	  std::string plot_name;
	  plot_name.reserve(params.title.size() + 5);
	  plot_name += "comp_";
	  for(size_t i = 0; i < params.title.size(); ++i)
	    {
	      if(params.title[i] != ']' && params.title[i] != '[' && params.title[i] != '.') 
		plot_name += params.title[i];
	      if(params.title[i] == '.' && params.title[i+1] != '_')
		plot_name += "_";
	    }

	  if (params.title == "p_z / p")
	    plot_name = "comp_pz_p";

	  if (cutN > 0)
	    {
	      canv->SaveAs(Form("standardizedPlots/comparisonPlots/%s_%s.png", plot_name.c_str(), cutIDs[cutN].c_str()));
	      canv->SetLogy();
	      canv->SaveAs(Form("standardizedPlots/comparisonPlots/%s_%s_log.png", plot_name.c_str(), cutIDs[cutN].c_str()));
	    }
	  else
	    {
	      canv->SaveAs(Form("standardizedPlots/comparisonPlots/%s.png", plot_name.c_str()));
	      canv->SetLogy();
	      canv->SaveAs(Form("standardizedPlots/comparisonPlots/%s_log.png", plot_name.c_str()));
	    }

	  //Clean up
	  hCRY1->Delete();
	  hCRY2->Delete();
	  hDBY->Delete();
	  leg->Delete();

	}
    }


}


/**********************************************************************************************************************************************************************************/
