{
cout << endl << "Welcome to the ATLAS rootlogon.C" << endl;
//
// based on a style file from BaBar
//

//..BABAR style from RooLogon.C in workdir

// use plain black on white colors
Int_t icol=0;
gStyle->SetFrameBorderMode(icol);
gStyle->SetCanvasBorderMode(icol);
gStyle->SetPadBorderMode(icol);
gStyle->SetPadColor(icol);
gStyle->SetCanvasColor(icol);
gStyle->SetStatColor(icol);
//gStyle->SetFillColor(icol);

// set the paper & margin sizes
gStyle->SetPaperSize(20,26);
gStyle->SetPadTopMargin(0.1);
gStyle->SetPadRightMargin(0.15);
gStyle->SetPadBottomMargin(0.15);
gStyle->SetPadLeftMargin(0.15);

// use large fonts
//Int_t font=72;
Int_t font=132;
Double_t tsize=0.05;
gStyle->SetTextFont(font);


gStyle->SetTextSize(tsize);
gStyle->SetLabelFont(font,"x");
gStyle->SetTitleFont(font,"x");
 gStyle->SetTitleOffset(1,"y"); //1.5

gStyle->SetLabelFont(font,"y");
gStyle->SetTitleFont(font,"y");
gStyle->SetLabelFont(font,"z");
gStyle->SetTitleFont(font,"z");

gStyle->SetLabelSize(tsize,"x");
gStyle->SetTitleSize(tsize,"x");
gStyle->SetLabelSize(tsize,"y");
gStyle->SetTitleSize(tsize,"y");
gStyle->SetLabelSize(tsize,"z");
gStyle->SetTitleSize(tsize,"z");

 
//use bold lines and markers
gStyle->SetMarkerStyle(1);
gStyle->SetMarkerSize(1.0);
gStyle->SetHistLineWidth(3.);
gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

//get rid of X error bars and y error bar caps
//gStyle->SetErrorX(0.001);

//do not display any of the standard histogram decorations
//gStyle->SetOptTitle(0);
//gStyle->SetOptStat(1111);
//gStyle->SetOptStat(0);
//gStyle->SetOptFit(1111);
gStyle->SetOptFit(0);

// put tick marks on top and RHS of plots
gStyle->SetPadTickX(1);
gStyle->SetPadTickY(1);

//gROOT->SetStyle("Plain");

//gStyle->SetPadTickX(1);
//gStyle->SetPadTickY(1);
}

