/*
+
                                                ------------------------------------
  Entry Name:                                   MR_PET.C
                                                ------------------------------------

  Function:    	Choose and evaluate .root file of GATE simulation. 

  Author:      	Uwe Pietrzyk

				Institute of Neurosciences and Medicine (INM)
				Research Center Juelich, Germany

				Department of Mathematics and Natural Sciences
				University of Wuppertal, Germany


------------------------------------------------------------------------------------
  Sources:		ion sources
------------------------------------------------------------------------------------


------------------------------------------------------------------------------------
  Description for Users:
------------------------------------------------------------------------------------

Macro for the evaluation of the MR_PET example.
The user is allowed to choose a specific .root file for evaluation
via a file browser.

input:    	.root file (via file browser)

output:   	canvas with six pads showing
			1) energy spectrum of detected events
			2) 3 dimensional plot of detected events
			3) simulation settings (source type, medium, B0,...)
			4) - 6) projection of detected events onto xy/xz/zy plane.


------------------------------------------------------------------------------------
  Last change: 22.09.2017
------------------------------------------------------------------------------------
  Revisions:
------------------------------------------------------------------------------------
REV #   DATE          	NAME 	REVISIONS MADE
01.00   20.06.2014     	UP     	created
01.01   10.07.2015		UP 		added ion sources
01.02	16.07.2015		ML 		histogram axes scale automatically
02.00   22.09.2017      ML      various bug fixes for ROOT 6 compatibility
------------------------------------------------------------------------------------
-
*/


#include <TGButton.h>
#include <TGFrame.h>
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TF2.h>
#include <RQ_OBJECT.h>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "TROOT.h"
#include "TGClient.h"
#include "TGFileDialog.h"
#include "TGFSContainer.h"
#include "TVirtualPad.h"
#include "TView3D.h"
#include "TAxis3D.h"
#include "TPolyLine3D.h"
#include "TGMsgBox.h"
#include "TColor.h"
#include <TGMsgBox.h>
#include <algorithm>

using namespace std;


//----------------------------------------------------------------------------------
class MyMainFrame : public TGMainFrame {

private:

   TGMainFrame *fMain;
   TGMainFrame *m;
   TGTextButton     *choose, *exit;

    TRootEmbeddedCanvas *fEcanvas;
  
	TH1F *ener;
		
	TH2F *globalPosZ_Y;
	TH2F *globalPosX_Z;
	TH2F *globalPosX_Y;
	TH3F *three_dim_plot;
	TMarker *m1;
	TMarker *m2;
	TMarker *m3;

public:
   MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~MyMainFrame();

   // slots

   void ChooseFile();
   void CloseWindow();
   void MRPET(TString fname);

   ClassDef(MyMainFrame, 0)
};

//----------------------------------------------------------------------------------
MyMainFrame *pmain;


class FileList {
	
	RQ_OBJECT("FileList")
	
protected:
	TGTransientFrame *fMain;
	TGFileContainer  *fContents;
	TGPopupMenu      *fMenu;	
	
	void DisplayFile(const TString &fname);
	void DisplayDirectory(const TString &fname);
	void DisplayObject(const TString& fname,const TString& name);
	
public:
	FileList(const TGWindow *p,const TGWindow *pmain,UInt_t w,UInt_t h);
	virtual ~FileList();
	
	// slots
	void OnDoubleClick(TGLVEntry*,Int_t);
	void DoMenu(Int_t);
	void CloseWindow();
	
};

//----------------------------------------------------------------------------------
void MyMainFrame::CloseWindow()
{
	// got close message for this MainFrame. terminates the application.
	
	gApplication->Terminate(0);
}

//----------------------------------------------------------------------------------
MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h)
{

    // create a main frame
	fMain = new TGMainFrame(p,w,h);
	fMain->Connect("CloseWindow()", "MyMainFrame", this, "CloseWindow()");
	fMain->DontCallClose(); // to avoid double deletions.
	
	// create canvas widget
	fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,1500, 1000);
	fEcanvas->GetCanvas()->Divide(3,2);
	fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| 
					kLHintsExpandY,10,10,10,1));

	// create a horizontal frame widget with buttons
	TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40); 
	choose = new TGTextButton(hframe,"&Select Root File"); 
	choose->Connect("Clicked()","MyMainFrame",this,"ChooseFile()"); 
	hframe->AddFrame(choose, new TGLayoutHints(kLHintsCenterX,5,5,3,4)); 
	exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)"); 
	hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

	// set a name to the main frame
	fMain->SetWindowName("MR_PET");

	// map all subwindows of main frame
	fMain->MapSubwindows();

	// initialize the layout algorithm
	fMain->Resize(fMain->GetDefaultSize());

	// map main frame
	fMain->MapWindow();

}

//----------------------------------------------------------------------------------
void MyMainFrame::ChooseFile()
{
  // slot connected to the Clicked() signal. 
  // it will open a file browser which allows to choose a file for processing.
  
  choose->SetText("&Select Root File"); 
  choose->SetState(kButtonDisabled); 
  new FileList(gClient->GetRoot(), fMain,  800, 800);

}

//----------------------------------------------------------------------------------
void MyMainFrame::MRPET(TString fname)
{
	
	int iReturnVal;
	m = new TGMainFrame(gClient->GetRoot(),1500,800);
	new TGMsgBox(gClient->GetRoot(),m, 
                 "3d plot", "Show 3d plot of detected events (extended computing time)?"
                 , kMBIconQuestion, (kMBYes|kMBNo), &iReturnVal);

	// Yes: iReturnVal = 1, No: iReturnVal = 2
	
	int ret = iReturnVal;

	TString RootFileName = fname;
	TFile f(RootFileName);
	TString GraphicFileName = RootFileName.Remove(RootFileName.Length()-5) + ".gif";	

	TH1 *total_nb_primaries = (TH1*)gDirectory->Get("total_nb_primaries");
	TTree *Singles = (TTree*)gDirectory->Get("Singles");
	
	Float_t         energy;
	Float_t			scatter_collim, scatter_shielding, primary_event, scatter_crystal;
	Float_t	        globalPosX, globalPosY, globalPosZ;
	Float_t			globalPosX1, globalPosY1, globalPosZ1;

	Singles->SetBranchAddress("energy",&energy);
	Singles->SetBranchAddress("globalPosX",&globalPosX);
	Singles->SetBranchAddress("globalPosY",&globalPosY);	
	Singles->SetBranchAddress("globalPosZ",&globalPosZ);

//----------------------------------------------------------------------------------
    // estimate plot range for the histograms
   	
	Singles->Draw("energy>>hi_energy","","goff");
	Singles->Draw("globalPosX>>posX","","goff");
	Singles->Draw("globalPosY>>posY","","goff");
	Singles->Draw("globalPosZ>>posZ","","goff");

	TH1F *hi_energy = (TH1F*)gDirectory->Get("hi_energy");
	TH1F *posX = (TH1F*)gDirectory->Get("posX");
	TH1F *posY = (TH1F*)gDirectory->Get("posY");
	TH1F *posZ = (TH1F*)gDirectory->Get("posZ");

	Int_t   nbins     	= hi_energy->GetNbinsX();
	Int_t   nbins_X     = posX->GetNbinsX();
	Int_t   nbins_Y     = posY->GetNbinsX();
	Int_t   nbins_Z     = posZ->GetNbinsX();
	
	Float_t binWidth  = hi_energy->GetBinWidth(1);
	Float_t binWidthX = posX->GetBinWidth(1);
	Float_t binWidthY = posY->GetBinWidth(1);
	Float_t binWidthZ = posZ->GetBinWidth(1);

//----------------------------------------------------------------------------------
	// enery spectrum

	double maximum_energy = Singles->GetMaximum("energy");
	double Emax = maximum_energy + 0.2*maximum_energy;

	// X position of detected events
	Int_t x_first_bin_1 = posX->FindFirstBinAbove(1.0);
	Int_t x_first_bin_0 = posX->FindFirstBinAbove(0.0);
	Int_t x_last_bin_1 = posX->FindLastBinAbove(1.0);
	Int_t x_last_bin_0 = posX->FindLastBinAbove(0.0);
	Int_t x_bin_0 = posX->FindBin(0.0);

	Float_t x_nbins_last = (x_last_bin_0 - x_last_bin_1)/5 + (x_last_bin_1 - x_bin_0);
	Float_t x_nbins_first = x_bin_0 - x_first_bin_1 + (x_first_bin_1 - x_first_bin_0)/5;
	Float_t x_bin_range = max((x_nbins_first),(x_nbins_last)) * binWidthX;

	// Y position of detected events

	Int_t y_first_bin_1 = posY->FindFirstBinAbove(1.0);
	Int_t y_first_bin_0 = posY->FindFirstBinAbove(0.0);
	Int_t y_last_bin_1 = posY->FindLastBinAbove(1.0);
	Int_t y_last_bin_0 = posY->FindLastBinAbove(0.0);
	Int_t y_bin_0 = posY->FindBin(0.0);

	Float_t y_nbins_last = (y_last_bin_0 - y_last_bin_1)/5 + (y_last_bin_1 - y_bin_0);
	Float_t y_nbins_first = y_bin_0 - y_first_bin_1 + (y_first_bin_1 - y_first_bin_0)/5;
	Float_t y_bin_range = max((y_nbins_first),(y_nbins_last)) * binWidthY;

	// Z position of detected events

	Int_t z_first_bin_1 = posZ->FindFirstBinAbove(1.0);
	Int_t z_first_bin_0 = posZ->FindFirstBinAbove(0.0);
	Int_t z_last_bin_1 = posZ->FindLastBinAbove(1.0);
	Int_t z_last_bin_0 = posZ->FindLastBinAbove(0.0);
	Int_t z_bin_0 = posZ->FindBin(0.0);

	Float_t z_nbins_last = (z_last_bin_0 - z_last_bin_1)/5 + (z_last_bin_1 - z_bin_0);
	Float_t z_nbins_first = z_bin_0 - z_first_bin_1 + (z_first_bin_1 - z_first_bin_0)/5;
	Float_t z_bin_range = max((z_nbins_first),(z_nbins_last)) * binWidthZ;

   	// bin ranges 

	Float_t xy_bin_range = max(x_bin_range,y_bin_range);
	Float_t xz_bin_range = max(x_bin_range,z_bin_range);
	Float_t zy_bin_range = max(z_bin_range,y_bin_range);

   	// one global bin range for all histograms

	Float_t bin_range = max(max(xy_bin_range,xz_bin_range),zy_bin_range);

	
//----------------------------------------------------------------------------------
	// create histograms in tile 1,4,5,6

	ener = new TH1F("ener","energy spectrum of the detected events",nbins,0.,Emax);	
	globalPosZ_Y = new TH2F("proj_zy","projection onto zy plane",nbins_Z,-bin_range,
		+bin_range,nbins_Y,-bin_range,+bin_range);
	globalPosX_Z = new TH2F("proj_xz","projection onto xz plane",nbins_X,-bin_range,
		+bin_range,nbins_Z,-bin_range,+bin_range);
	globalPosX_Y = new TH2F("proj_xy","projection onto xy plane",nbins_X,-bin_range,
		+bin_range,nbins_Y,-bin_range,+bin_range);
	three_dim_plot = new TH3F("3d_plot", " ", nbins_X,-bin_range,bin_range,nbins_Y,
		-bin_range,bin_range,nbins_Z,-bin_range,bin_range);

	/* "By default when an histogram is created, it is added to the list
 	   of histogram objects in the current directory in memory.
 	   Remove reference to this histogram from current directory and add
 	   reference to new directory dir. dir can be 0 in which case the
 	   histogram does not belong to any directory."
 	   => Set directory to 0 <=> histogram doesn't disappear when the
 	   canvas gets refreshed. */

	ener->SetDirectory(0);
	globalPosZ_Y->SetDirectory(0);
	globalPosX_Z->SetDirectory(0);
	globalPosX_Y->SetDirectory(0);
	three_dim_plot->SetDirectory(0);
	
	globalPosZ_Y->GetXaxis()->SetLimits(-bin_range,bin_range);
	globalPosZ_Y->GetYaxis()->SetLimits(-bin_range,bin_range);
	globalPosX_Z->GetXaxis()->SetLimits(-bin_range,bin_range);
	globalPosX_Z->GetYaxis()->SetLimits(-bin_range,bin_range);
	globalPosX_Y->GetXaxis()->SetLimits(-bin_range,bin_range);
	globalPosX_Y->GetYaxis()->SetLimits(-bin_range,bin_range);

//----------------------------------------------------------------------------------
	// reset histograms

	ener->Reset();
	ener->SetBins(nbins,0,Emax);
    globalPosZ_Y->Reset();
	globalPosX_Z->Reset();
	globalPosX_Y->Reset();

	cout << endl;
	cout << "##### Reading from file: " << RootFileName << endl;
	cout << endl;
	
	TString input = RootFileName;
	Int_t nentries = Singles->GetEntries();
	Int_t ntotal = total_nb_primaries->GetMean();
	Int_t nbytes = 0;


	cout<<"##### nb of entries: " << nentries <<endl;
	
//----------------------------------------------------------------------------------
    gStyle->SetTitleFontSize(0.1);
    
//----------------------------------------------------------------------------------
    // Fill histograms.
	for (Int_t i=0; i<nentries;i++) 
	{
		nbytes += Singles->GetEntry(i);
		globalPosZ_Y->Fill(globalPosZ,globalPosY);
		globalPosX_Z->Fill(globalPosX,globalPosZ);	
		globalPosX_Y->Fill(globalPosX,globalPosY);	
		three_dim_plot->Fill(globalPosZ,globalPosX,globalPosY);	
		ener->Fill(energy);
	}

//----------------------------------------------------------------------------------
	// create plots 
	
	// create Canvas
		gROOT->Reset();
		gStyle->SetOptStat(0);
		gStyle->SetCanvasPreferGL(kTRUE);
		gStyle->SetPalette(1);
    	TCanvas *C1=fEcanvas->GetCanvas();
    	C1->SetFillColor(0);

   	// first tile: energy histogram
		C1->cd(1);
		
		ener->SetFillColor(kRed+1);
		ener->SetFillStyle(3023);
		gStyle->SetTitleFontSize(0.09);
		ener->GetXaxis()->SetTitle("energy [MeV]");
		ener->GetXaxis()->CenterTitle();
		ener->GetXaxis()->SetTitleOffset(1.2);
		ener->GetXaxis()->SetLabelSize(0.03);
		ener->GetXaxis()->SetLabelOffset(0.015);
		ener->GetYaxis()->SetTitle("# entries ");
		ener->GetYaxis()->CenterTitle();
		ener->GetYaxis()->SetTitleOffset(1.5);
		ener->GetYaxis()->SetLabelSize(0.03);
		ener->GetYaxis()->SetLabelOffset(0.015);
		TGaxis::SetMaxDigits(3);
		ener->Draw();
		

	// third tile (text)
		C1->cd(3);
		
		/* Create variables, text,... in order to read file name and print
		   information like source type or source energy onto canvas.*/

		TPaveText *pt = new TPaveText(.0,.3,.8,.9);
		for (int i=0; i<input.Length(); i++)
			{if (input[i] == '_')
		        input[i] = ' ';}
		vector<string> array;
		stringstream ss;
		ss << input;
		string temp;
		string text;
		string text_add;
		int line_number;
		while (ss >> temp)
		   	array.push_back(temp);

	   	
	   	// read filename and print information in a TPaveText called pt
		for (line_number =1; line_number <9; line_number++)
			{   if (line_number == 1)
			    	{text = "- Ion: ";
			        text += array[5];}
			    if (line_number == 8)
			        {text = "- positron annihilation: ";
			    		if (array[9] == "disabled")
			        		{text_add = "disabled";
			        		text += text_add;}
			        	else
			         		{text_add = "enabled";
			         		text += text_add;}}
			    if (line_number == 2)
			        {text = "- camera type: ";
			        text += array[2];}
			    if (line_number == 3)
			        {text = "- medium: ";
			        text += array[3];}
			    if (line_number == 4)
			        {text = "- source activity: ";
			        text += array[4];
			        text += " Bq";}
			    if (line_number == 5)
			        {text = "- B_{0,x}: ";
			        text += array[6];
			        text += " T";}
			    if (line_number == 6)
			        {text = "- B_{0,y}: ";
			        text += array[7];
			        text += " T";}
			    if (line_number == 7)
			        {text = "- B_{0,z}: ";
			        text += array[8];
			        text += " T";}
		      const char * text_str = text.c_str();
		     	  pt->AddText(text_str);
		     	  pt->SetBorderSize(0);
		     	  pt->SetFillColor(0);
		     	  pt->SetTextAlign(11);
		     	  pt->SetTextFont(42);
		     	  pt->Draw();
		   	}   	

		
		TLatex *legend1 = new TLatex();
		legend1->SetTextAlign(11);
		legend1->SetTextSize(0.08);
		legend1->SetTextColor(kRed+2);
		legend1->DrawLatex(0.05,0.16,"#bullet");
		TLatex *legend2 = new TLatex();
		legend2->SetTextColor(kBlack);
		legend2->SetTextFont(42);
		legend2->SetTextSize(0.04);
		legend2->DrawLatex(0.1,0.175,"source position");		
		
		// show gate logo in the corner of tile three
		{
		    TImage *imglogo = TImage::Open("gatelogo.png");
		    if (!imglogo) {
			    printf("Could not create an image... exit\n");
			    return;}
			TPad *logo = new TPad("logo","logo",0.7,0.7,1.,1.);
			logo->Draw();
			logo->cd();
			imglogo->Draw();
		}

	// second tile: 3d plot
		C1->SetFillColor(0);
		
		if (ret == 1)
			{ 
			C1->cd(2);
					
			three_dim_plot->GetXaxis()->SetTitle(" z [mm]");
			three_dim_plot->GetXaxis()->CenterTitle();
			three_dim_plot->GetXaxis()->SetTitleOffset(2);
			three_dim_plot->GetXaxis()->SetLabelOffset(0.005);
			three_dim_plot->GetXaxis()->SetLabelSize(0.03);
			three_dim_plot->GetYaxis()->SetTitle(" x [mm]");
			three_dim_plot->GetYaxis()->CenterTitle();
			three_dim_plot->GetYaxis()->SetTitleOffset(2);
			three_dim_plot->GetYaxis()->SetLabelOffset(0.005);
			three_dim_plot->GetYaxis()->SetLabelSize(0.03);
			three_dim_plot->GetZaxis()->SetTitle(" y [mm]");
			three_dim_plot->GetZaxis()->CenterTitle();
			three_dim_plot->GetZaxis()->SetTitleOffset(1.6);
			three_dim_plot->GetZaxis()->SetLabelOffset(0.015);
			three_dim_plot->GetZaxis()->SetLabelSize(0.03);
			three_dim_plot->GetXaxis()->SetLimits(-bin_range,bin_range);
			three_dim_plot->GetYaxis()->SetLimits(-bin_range,bin_range);
			three_dim_plot->GetZaxis()->SetLimits(-bin_range,bin_range);

			TColor *col46 = gROOT->GetColor(46);
	    	col46->SetAlpha(0.25);
	    	TPolyMarker3D *pm = new TPolyMarker3D(1,8);
	    	pm->SetPoint(0,0,0,0);
	    	pm->SetMarkerColor(kRed+2);

	    	// cross-hair
				// create line for x
			    TPolyLine3D *pl3dx = new TPolyLine3D(2);
			    pl3dx->SetLineWidth(1);
			    pl3dx->SetLineColor(46);

		    	// create line for y
			    TPolyLine3D *pl3dy = new TPolyLine3D(2);
			    pl3dy->SetLineWidth(1);
			    pl3dy->SetLineColor(46);

				// create line for z
			    TPolyLine3D *pl3dz = new TPolyLine3D(2);
			    pl3dz->SetLineWidth(1);
			    pl3dz->SetLineColor(46);

	    		{	pl3dx->SetPoint(0, -bin_range, 0, 0);
			   		pl3dx->SetPoint(1, bin_range, 0, 0);
					pl3dy->SetPoint(0, 0, -bin_range, 0);
			    	pl3dy->SetPoint(1, 0, bin_range, 0);
			    	pl3dz->SetPoint(0, 0, 0, -bin_range);
				    pl3dz->SetPoint(1, 0, 0, bin_range);}

			gPad->SetTheta( 30.) ; // use these settings to get
		    gPad->SetPhi( -120.) ; // the same perspective as in GATE
		    gPad->Update();
			three_dim_plot->Draw("");
			pl3dx->Draw("SAME");
			pl3dy->Draw("SAME");
			pl3dz->Draw("SAME");
			pm->Draw("SAME");
			gPad->Modified();
			gPad->Update();
			}

	// fourth tile: XY plot

		C1->SetFillColor(0);
		C1->cd(4);
	    globalPosX_Y->GetXaxis()->SetTitle("x [mm]");
	    globalPosX_Y->GetXaxis()->CenterTitle();
	    globalPosX_Y->GetXaxis()->SetTitleOffset(1.2);
	    globalPosX_Y->GetXaxis()->SetLabelOffset(0.015);
	    globalPosX_Y->GetXaxis()->SetLabelSize(0.03);
	    globalPosX_Y->GetYaxis()->SetTitle("y [mm]");
		globalPosX_Y->GetYaxis()->CenterTitle();
		globalPosX_Y->GetYaxis()->SetTitleOffset(1.2);
		globalPosX_Y->GetYaxis()->SetLabelOffset(0.015);
		globalPosX_Y->GetYaxis()->SetLabelSize(0.03);

		TMarker *m1 = new TMarker(0,0,8);
   		m1->SetMarkerStyle(8);
   		m1->SetMarkerColor(kRed+2);
   		m1->SetMarkerSize(1);

		globalPosX_Y->Draw("");
   		m1->Draw("SAME");
		{
   
		   TImage *imgxy = TImage::Open("proj_XY.png");
		   if (!imgxy) {
		      printf("Could not create an image... exit\n");
		      return;
		   }
		   TPad *xy = new TPad("xy","xy",0.1,0.91,0.2,1.);
		   xy->Draw();
		   xy->cd();
		   imgxy->Draw();
		}

	// fifth tile: ZX plot
	
		C1->SetFillColor(0);
		C1->cd(5);
	    globalPosX_Z->GetXaxis()->SetTitle("x [mm]");
	    globalPosX_Z->GetXaxis()->CenterTitle();
	    globalPosX_Z->GetXaxis()->SetTitleOffset(1.2);
	    globalPosX_Z->GetXaxis()->SetLabelOffset(0.015);
	    globalPosX_Z->GetXaxis()->SetLabelSize(0.03);
	    globalPosX_Z->GetYaxis()->SetTitle("z [mm]");
		globalPosX_Z->GetYaxis()->CenterTitle();
		globalPosX_Z->GetYaxis()->SetTitleOffset(1.2);
		globalPosX_Z->GetYaxis()->SetLabelOffset(0.015);
		globalPosX_Z->GetYaxis()->SetLabelSize(0.03);

		TMarker *m2 = new TMarker(0,0,8);
   		m2->SetMarkerStyle(8);
   		m2->SetMarkerColor(kRed+2);
   		m2->SetMarkerSize(1);
		
		globalPosX_Z->Draw("");
		m2->Draw("SAME");
		{
   		   TImage *imgxz = TImage::Open("proj_XZ.png");
		   if (!imgxz) {
		      printf("Could not create an image... exit\n");
		      return;
		   }
		   TPad *xz = new TPad("xz","xz",0.1,0.91,0.2,1.);
		   xz->Draw();
		   xz->cd();
		   imgxz->Draw();
		}

    // sixth tile: ZY plot
    	gStyle->SetTitleFontSize(0.04);
		C1->SetFillColor(0);
		C1->cd(6);
		
		globalPosZ_Y->GetXaxis()->SetTitle("z [mm]");
		globalPosZ_Y->GetXaxis()->CenterTitle();
		globalPosZ_Y->GetXaxis()->SetTitleOffset(1.2);
		globalPosZ_Y->GetXaxis()->SetLabelOffset(0.015);
		globalPosZ_Y->GetXaxis()->SetLabelSize(0.03);
		globalPosZ_Y->GetYaxis()->SetTitle("y [mm]");
		globalPosZ_Y->GetYaxis()->CenterTitle();
		globalPosZ_Y->GetYaxis()->SetTitleOffset(1.2);
		globalPosZ_Y->GetYaxis()->SetLabelOffset(0.015);
		globalPosZ_Y->GetYaxis()->SetLabelSize(0.03);
		
		TMarker *m3 = new TMarker(0,0,8);
   		m3->SetMarkerStyle(8);
   		m3->SetMarkerColor(kRed+2);
   		m3->SetMarkerSize(1);
		
		globalPosZ_Y->Draw("");
		m3->Draw("SAME");

		{
   		   TImage *imgzy = TImage::Open("proj_ZY.png");
		   if (!imgzy) {
		      printf("Could not create an image... exit\n");
		      return;
		   }
		   TPad *zy = new TPad("zy","zy",0.1,0.91,0.2,1.);
		   zy->Draw();
		   zy->cd();
		   imgzy->Draw();
		}
			
	// Update Canvas
		C1->Update();    
		gPad->Modified();
		gPad->Update();
		C1->SaveAs(GraphicFileName);	

//----------------------------------------------------------------------------------
	// summary printout 
	
	cout << " *********************************************************************************** " << endl;
	cout << " *                                                              _   _              * " << endl;
	cout << " *   E d u G a t e  S i m u l a t i o n   A n a l y s i s      (_)_(_)             * " << endl;
	cout << " *   M R _ P E T                                                (o o)              * " << endl;
	cout << " *                                                              =\\o/=              * " << endl;
	cout << " *********************************************************************************** " << endl;
	cout << endl;
	
	cout<<"##### Number of emitted particles    :  "	<<ntotal<<endl;
	cout<<"##### Number of detected events      :  "	<<nentries<<endl;
	cout<<"##### ratio detected/emitted         :  "	<<float(nentries)/float(ntotal)*100<<" %"<<endl;
	cout << endl;

}

//----------------------------------------------------------------------------------
MyMainFrame::~MyMainFrame()
{
   // clean up all widgets, frames and layouthints that were used
   fMain->Cleanup();
	delete fMain;
	delete ener;
	delete globalPosZ_Y;
	delete globalPosX_Z;
	delete globalPosX_Y;
	delete three_dim_plot;
	delete m1;
	delete m2;
	delete m3;
	
}

//----------------------------------------------------------------------------------
FileList::FileList(const TGWindow *p, const TGWindow *pmain, UInt_t w, UInt_t h)
{
   // create transient frame containing a filelist widget.

   TGLayoutHints *lo;	

   fMain = new TGTransientFrame(p, pmain, w, h);
   fMain->Connect("CloseWindow()", "FileList", this, "CloseWindow()");
   fMain->DontCallClose(); // to avoid double deletions.

   // use hierarchical cleaning
   fMain->SetCleanup(kDeepCleanup);

   TGMenuBar* mb = new TGMenuBar(fMain);
   lo = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
   fMain->AddFrame(mb, lo);

   fMenu = mb->AddPopup("&View");
   fMenu->AddEntry("Lar&ge Icons",kLVLargeIcons);
   fMenu->AddEntry("S&mall Icons",kLVSmallIcons);
   fMenu->AddEntry("&List",       kLVList);
   fMenu->AddEntry("&Details",    kLVDetails);
   fMenu->AddSeparator();
   fMenu->AddEntry("&Close",      10);
   fMenu->Connect("Activated(Int_t)","FileList",this,"DoMenu(Int_t)");

   TGListView* lv = new TGListView(fMain, w, h);
   lo = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   fMain->AddFrame(lv,lo);

   Pixel_t white;
   gClient->GetColorByName("white", white);
   fContents = new TGFileContainer(lv, kSunkenFrame,white);
   fContents->Connect("DoubleClicked(TGFrame*,Int_t)", "FileList", this,
                      "OnDoubleClick(TGLVEntry*,Int_t)");

   // position relative to the parent's window
   fMain->CenterOnParent();

   fMain->SetWindowName("Choose root file");
   fMain->MapSubwindows();
   fMain->MapWindow();
   fContents->SetFilter("*.root");
   fContents->SetDefaultHeaders();
   fContents->DisplayDirectory();
   fContents->AddFile("..");        // up level directory
   fContents->Sort(kSortByDate);
   fContents->Resize();
   fContents->StopRefreshTimer();   // stop refreshing
   #if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
      fContents->SetViewMode(kLVDetails);
   #else
      fContents->SetViewMode(3);
   #endif   
   // 1: list of file names, 1 column
   // 2: list of file names, 2 columns
   // 3: list with details
   // 4: small icons and file names
   fMain->Resize();
}

//----------------------------------------------------------------------------------
FileList::~FileList()
{
   // cleanup.

   delete fContents;
   delete fMenu;
   fMain->DeleteWindow();  // deletes fMain
}

//----------------------------------------------------------------------------------
void FileList::DoMenu(Int_t mode)
{
   // switch view mode.

   if (mode<10) {
      fContents->SetViewMode((EListViewMode)mode);
   } else {
      delete this;
   }
}

//----------------------------------------------------------------------------------
void FileList::DisplayFile(const TString &fname)
{
   // display content of ROOT file.
	cout<<fname<<endl;
	pmain->MRPET(fname);
	fMain->CloseWindow();

}

//----------------------------------------------------------------------------------
void FileList::DisplayDirectory(const TString &fname)
{
   // display content of directory.

   fContents->SetDefaultHeaders();
   gSystem->ChangeDirectory(fname);
   fContents->ChangeDirectory(fname);
   fContents->DisplayDirectory();
   fContents->AddFile("..");  // up level directory
   fMain->Resize();
}

//----------------------------------------------------------------------------------
void FileList::DisplayObject(const TString& fname,const TString& name)
{
   // browse object located in file.

   TDirectory *sav = gDirectory;

   static TFile *file = 0;
   if (file) delete file;     // close
   file = new TFile(fname);   // reopen

   TObject* obj = file->Get(name);
   if (obj) {
      if (!obj->IsFolder()) {
         obj->Browse(0);
      } else obj->Print();
   }
   gDirectory = sav;
}

//----------------------------------------------------------------------------------
void FileList::OnDoubleClick(TGLVEntry *f, Int_t btn)
{
   // handle double click.

   if (btn != kButton1) return;

   TString name(f->GetTitle());
   const char* fname = (const char*)f->GetUserData();

   if (fname) {
      DisplayObject(fname, name);
   } else if (name.EndsWith(".root")) {
      DisplayFile(name);
   } else {
      DisplayDirectory(name);
   }
	
}

//----------------------------------------------------------------------------------
void FileList::CloseWindow()
{
   delete this;
}
//----------------------------------------------------------------------------------
void MR_PET()
{
   // popup the GUI...

   pmain = new MyMainFrame(gClient->GetRoot(), 350, 800);

}

//----------------------------------------------------------------------------------