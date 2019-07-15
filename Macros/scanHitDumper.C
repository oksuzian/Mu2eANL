// Macro to scan the output N-Tuple of the HitDumper module for pdgId stats
// Ben Barton
// 07/15/19



void scanHitDumper(string nTuplePath)
{
  
  TFile inFile(nTuplePath.c_str());
  TNtuple *tree = (TNtuple*) inFile.Get("HitDumper/nt");

  cout << "\nNumber of neutron events: " << tree->GetEntries("hits.pdgId==2112") << endl;
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
}
