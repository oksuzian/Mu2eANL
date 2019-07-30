// Macro to scan the output N-Tuple of the HitDumper module for pdgId stats
// Ben Barton
// 07/15/19



void scanHitDumper(string nTuplePath)
{
  
  TFile inFile(nTuplePath.c_str());
  TNtuple *tree = (TNtuple*) inFile.Get("HitDumper/nt");

  cout << "\nTotal number of events: " << tree->GetEntries() << endl; 
  cout << "Number of neutron events: " << tree->GetEntries("hits.pdgId==2112") << endl;
  cout << "Number of proton events: " << tree->GetEntries("hits.pdgId==2212") << endl;
  cout << "Number of K-long events: " << tree->GetEntries("hits.pdgId==130") << endl;
  cout << "Number of K-short events: " << tree->GetEntries("hits.pdgId==310") << endl;
  cout << "Number of K^+ events: " << tree->GetEntries("hits.pdgId==321") << endl;
  cout << "Number of anti-K^+ events: " << tree->GetEntries("hits.pdgId==-321") << endl;
  cout << "Number of Lambda events: " << tree->GetEntries("hits.pdgId==3122") << endl;
  cout << "Number of Sigma^0 events: " << tree->GetEntries("hits.pdgId==3212") << endl;
  cout << "Number of Sigma^- events: " << tree->GetEntries("hits.pdgId==3112") << endl;
  cout << "Number of Sigma^+ events: " << tree->GetEntries("hits.pdgId==3222") << endl;
  cout << "Number of gamma events: " << tree->GetEntries("hits.pdgId==22") << endl;
  cout << "Number of electron events: " << tree->GetEntries("hits.pdgId==11") << endl;
  cout << "Number of positron events: " << tree->GetEntries("hits.pdgId==-11") << endl;
  cout << "Number of mu- events: " << tree->GetEntries("hits.pdgId==13") << endl;
  cout << "Number of mu+ events: " << tree->GetEntries("hits.pdgId==-13") << endl;
  cout << "Number of pi^- events: " << tree->GetEntries("hits.pdgId==-211") << endl;
  cout << "Number of pi^+ events: " << tree->GetEntries("hits.pdgId==211") << endl;
  cout << "Number of pi^0 events: " << tree->GetEntries("hits.pdgId==111") << endl;
  cout << "Number of \"nuclear\" events: " << tree->GetEntries("hits.pdgId>=100000") << endl;
  cout << "Number of other events: " << tree->GetEntries("hits.pdgId!=2112 && abs(hits.pdgId)!=211 && hits.pdgId!=130 && hits.pdgId!=22 && abs(hits.pdgId)!=11 && hits.pdgId!=2212 && abs(hits.pdgId)!=13 && hits.pdgId<100000 && hits.pdgId!=111 && hits.pdgId!=310 && abs(hits.pdgId)!=321 && hits.pdgId!=3122 &&hits.pdgId!=3212 && hits.pdgId!=3222 &&hits.pdgId!=3112") << endl;

  cout << "\n#################  Scans:  #######################" << endl;
  cout << "\"Other\" events" << endl;
  tree->Scan("hits.pdgId","hits.pdgId!=2112&&abs(hits.pdgId)!=211&&hits.pdgId!=130&&hits.pdgId!=22&&abs(hits.pdgId)!=11&&pdgId!=2212&&abs(hits.pdgId)!=13&&hits.pdgId<100000 && hits.pdgId!=111 && hits.pdgId!=310 && abs(hits.pdgId)!=321 && hits.pdgId!=3122 && hits.pdgId!=3212 && hits.pdgId!=3222 && hits.pdgId!=3112","");

  gStyle->SetOptStat(111111);
  TCanvas *canv = new TCanvas("canv","Kinetic Energy Plot", 800, 600);
  TH1F *kinEnergy = new TH1F("kinEnergy", "Kinetic Energy of K_L's",100, 0, 500);
  kinEnergy->SetXTitle("Kinetic Energy (MeV)");
  tree->Draw("(sqrt((hits.pmag*hits.pmag)+(497.6*497.6))-497.6)>>+kinEnergy","hits.pdgId==130","goff");
  kinEnergy = (TH1F*) gDirectory->Get("kinEnergy");
  canv->cd();
  kinEnergy->Draw("hist");
  canv->SaveAs("k-long_kineticEnergy.png");

  TH1F *kinEnergyNeut = new TH1F("kinEnergyNeut", "Kinetic Energy of Neutrons",100, 0, 500);
  kinEnergyNeut->SetXTitle("Kinetic Energy (MeV)");
  tree->Draw("(sqrt((hits.pmag*hits.pmag)+(939*939))-939)>>+kinEnergy","hits.pdgId==2112","goff");
  kinEnergyNeut = (TH1F*) gDirectory->Get("kinEnergyNeut");
  canv->cd();
  kinEnergyNeut->Draw("hist");
  canv->SaveAs("neutron_kineticEnergy.png");

}
