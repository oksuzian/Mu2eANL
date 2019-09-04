//Scan a TrkAna tree to determine information about events which pass cuts


//// Signal Cuts
string momentum_cut = "deent.mom>100 && deent.mom<110 && ue.nhits<0"; 
string trk_cuts_MDC = "dequal.TrkQualDeM>0.8"; //For original CRY1 analysis this was 0.4
string trk_cut_pid = "dequal.TrkPIDDeM>0.5";
string pitch_angle  = "deent.td>0.57735027 && deent.td<1"; //  Excludes beam particles
string min_trans_R  = "deent.d0>-80 && deent.d0<105"; //  Consistent with coming from the target
string max_trans_R  = "(demcent.d0+2.0/demcent.om)>450. && (demcent.d0+2.0/demcent.om)<680."; //  Inconsistent with hitting the proton absorber
string timing_cut   = "de.t0>700 && de.t0<1600"; //unnecessary for unmixed samples
string all_cuts_MDC = momentum_cut + "&&" + trk_cuts_MDC + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R;
string all_cuts_MDC_pid = momentum_cut + "&&" + trk_cuts_MDC + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R + "&&" + trk_cut_pid;
string all_cuts_MDC_mixed = all_cuts_MDC + "&&" + timing_cut;
string all_cuts_MDC_pid_mixed = all_cuts_MDC_pid + "&&" + timing_cut;



string infoDesired = "evtinfo.subrunid:evtinfo.eventid:demcgen.pdg";
TCut cuts = (all_cuts_MDC_pid).c_str();


//Plot all standardized histograms
void scanForEventTypes(string treePath, bool neg)
{
  
  //Read tree from file
  TFile inFile(treePath.c_str());
  TTree *tree;
 
  if (neg) 
    tree = (TTree*) inFile.Get("TrkAnaNeg/trkana");
  else
    tree = (TTree*) inFile.Get("TrkAnaPos/trkana");



  tree->Scan(infoDesired.c_str(), cuts,"");
}
