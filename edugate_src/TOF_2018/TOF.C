
// TOF.C
// Macro for the evaluation of the TOF EduGATE example.
// The user is allowed to choose a specific .root file for evaluation
// via a file browser.

#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TF2.h>
#include <RQ_OBJECT.h>

class MyMainFrame : public TGMainFrame {

private:

  TGMainFrame *fMain;
  TGTextButton     *choose, *exit;
  TRootEmbeddedCanvas *fEcanvas;
	TH2F *DetectPos;
	TH2F *SourcePos;
  TH2F *Reco;
  TH2F *Reco_Scattered;
  TH2F *Reco_Random;

public:
   MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~MyMainFrame();

   void CloseWindow();
   void ChooseFile();
   void TOF(TString fname);

   ClassDef(MyMainFrame, 0)
};

MyMainFrame *pmain;


class TestFileList {

	RQ_OBJECT("TestFileList")

protected:
	TGTransientFrame *fMain;
	TGFileContainer  *fContents;
	TGPopupMenu      *fMenu;

	void DisplayFile(const TString &fname);
	void DisplayDirectory(const TString &fname);
	void DisplayObject(const TString& fname,const TString& name);

public:
	TestFileList(const TGWindow *p, const TGWindow *pmain, /*TCanvas *can,*/ UInt_t w, UInt_t h);
	virtual ~TestFileList();

	void OnDoubleClick(TGLVEntry*,Int_t);
	void DoMenu(Int_t);
	void CloseWindow();

};

void MyMainFrame::CloseWindow()
{
	gApplication->Terminate(0);
}

void MyMainFrame::ChooseFile()
{
  choose->SetState(kButtonDown);
  choose->SetText("&Select Root File");
  new TestFileList(gClient->GetRoot(), fMain,  1280, 800);
  choose->SetState(kButtonUp);
}


MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h)
{
	fMain = new TGMainFrame(p,w,h);
	fMain->Connect("CloseWindow()", "MyMainFrame", this, "CloseWindow()");
	fMain->DontCallClose();

	fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,750,750);
	fEcanvas->GetCanvas()->Divide(1,1);
	fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1));
	TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40);
	choose = new TGTextButton(hframe,"&Select Root File");
	choose->Connect("Clicked()","MyMainFrame",this,"ChooseFile()");
	hframe->AddFrame(choose, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)");
	hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
	fMain->SetWindowName("Coincidence Channel");
	fMain->MapSubwindows();
	fMain->Resize(fMain->GetDefaultSize());
	fMain->MapWindow();

  Double_t plot_size = 120;
	DetectPos				= new TH2F("DetectPos","time-of-flight PET",200,-plot_size,plot_size,200,-plot_size,plot_size);
  SourcePos				= new TH2F("SourcePos","Transaxial Source Position",200,-plot_size,plot_size,200,-plot_size,plot_size);
  Reco				    = new TH2F("Reco","Transaxial Reco True Unscattered",200,-plot_size,plot_size,200,-plot_size,plot_size);
  Reco_Scattered	= new TH2F("Reco_Scattered","Transaxial Reco True Scattered",200,-plot_size,plot_size,200,-plot_size,plot_size);
  Reco_Random	    = new TH2F("Reco_Random","Transaxial Reco Random",200,-plot_size,plot_size,200,-plot_size,plot_size);
}

void MyMainFrame::TOF(TString fname)
{
	TString RootFileName = fname;
	TFile f(RootFileName);
	TString GraphicFileName = RootFileName.Remove(RootFileName.Length()-5) + ".gif";
	TTree *Coincidences = (TTree*)gDirectory->Get("Coincidences");

	//Declaration of leaves types - TTree Coincidences
	Int_t           RayleighCrystal1;
	Int_t           RayleighCrystal2;
	Int_t           RayleighPhantom1;
	Int_t           RayleighPhantom2;
	Char_t          RayleighVolName1[40];
	Char_t          RayleighVolName2[40];
	Float_t         axialPos;
	Char_t          comptVolName1[40];
	Char_t          comptVolName2[40];
	Int_t           comptonCrystal1;
	Int_t           comptonCrystal2;
	Int_t           crystalID1;
	Int_t           crystalID2;
	Int_t           comptonPhantom1;
	Int_t           comptonPhantom2;
	Float_t         energy1;
	Float_t         energy2;
	Int_t           eventID1;
	Int_t           eventID2;
	Float_t         globalPosX1;
	Float_t         globalPosX2;
	Float_t         globalPosY1;
	Float_t         globalPosY2;
	Float_t         globalPosZ1;
	Float_t         globalPosZ2;
	Int_t           layerID1;
	Int_t           layerID2;
	Int_t           moduleID1;
	Int_t           moduleID2;
	Float_t         rotationAngle;
	Int_t           rsectorID1;
	Int_t           rsectorID2;
	Int_t           runID;
	Float_t         sinogramS;
	Float_t         sinogramTheta;
	Int_t           sourceID1;
	Int_t           sourceID2;
	Float_t         sourcePosX1;
	Float_t         sourcePosX2;
	Float_t         sourcePosY1;
	Float_t         sourcePosY2;
	Float_t         sourcePosZ1;
	Float_t         sourcePosZ2;
	Int_t           submoduleID1;
	Int_t           submoduleID2;
	Double_t         time1;
	Double_t         time2;

	// extra variables

	Float_t  zmin;
  Float_t  zmax;
  Float_t  z;
	Int_t		 crystalx;
  Int_t    crystaly;
  Double_t delta_time;
  Double_t c_speed = 3E11;
  Double_t spat_diff;
  Double_t reco_x;
  Double_t reco_y;

	//Set branch addresses

	Coincidences->SetBranchAddress("RayleighCrystal1",&RayleighCrystal1);
	Coincidences->SetBranchAddress("RayleighCrystal2",&RayleighCrystal2);
	Coincidences->SetBranchAddress("RayleighPhantom1",&RayleighPhantom1);
	Coincidences->SetBranchAddress("RayleighPhantom2",&RayleighPhantom2);
	Coincidences->SetBranchAddress("RayleighVolName1",&RayleighVolName1);
	Coincidences->SetBranchAddress("RayleighVolName2",&RayleighVolName2);
	Coincidences->SetBranchAddress("axialPos",&axialPos);
	Coincidences->SetBranchAddress("comptVolName1",&comptVolName1);
	Coincidences->SetBranchAddress("comptVolName2",&comptVolName2);
	Coincidences->SetBranchAddress("comptonCrystal1",&comptonCrystal1);
	Coincidences->SetBranchAddress("comptonCrystal2",&comptonCrystal2);
	Coincidences->SetBranchAddress("crystalID1",&crystalID1);
	Coincidences->SetBranchAddress("crystalID2",&crystalID2);
	Coincidences->SetBranchAddress("comptonPhantom1",&comptonPhantom1);
	Coincidences->SetBranchAddress("comptonPhantom2",&comptonPhantom2);
	Coincidences->SetBranchAddress("energy1",&energy1);
	Coincidences->SetBranchAddress("energy2",&energy2);
	Coincidences->SetBranchAddress("eventID1",&eventID1);
	Coincidences->SetBranchAddress("eventID2",&eventID2);
	Coincidences->SetBranchAddress("globalPosX1",&globalPosX1);
	Coincidences->SetBranchAddress("globalPosX2",&globalPosX2);
	Coincidences->SetBranchAddress("globalPosY1",&globalPosY1);
	Coincidences->SetBranchAddress("globalPosY2",&globalPosY2);
	Coincidences->SetBranchAddress("globalPosZ1",&globalPosZ1);
	Coincidences->SetBranchAddress("globalPosZ2",&globalPosZ2);
	Coincidences->SetBranchAddress("layerID1",&layerID1);
	Coincidences->SetBranchAddress("layerID2",&layerID2);
	Coincidences->SetBranchAddress("moduleID1",&moduleID1);
	Coincidences->SetBranchAddress("moduleID2",&moduleID2);
	Coincidences->SetBranchAddress("rotationAngle",&rotationAngle);
	Coincidences->SetBranchAddress("rsectorID1",&rsectorID1);
	Coincidences->SetBranchAddress("rsectorID2",&rsectorID2);
	Coincidences->SetBranchAddress("runID",&runID);
	Coincidences->SetBranchAddress("sinogramS",&sinogramS);
	Coincidences->SetBranchAddress("sinogramTheta",&sinogramTheta);
	Coincidences->SetBranchAddress("sourceID1",&sourceID1);
	Coincidences->SetBranchAddress("sourceID2",&sourceID2);
	Coincidences->SetBranchAddress("sourcePosX1",&sourcePosX1);
	Coincidences->SetBranchAddress("sourcePosX2",&sourcePosX2);
	Coincidences->SetBranchAddress("sourcePosY1",&sourcePosY1);
	Coincidences->SetBranchAddress("sourcePosY2",&sourcePosY2);
	Coincidences->SetBranchAddress("sourcePosZ1",&sourcePosZ1);
	Coincidences->SetBranchAddress("sourcePosZ2",&sourcePosZ2);
	Coincidences->SetBranchAddress("submoduleID1",&submoduleID1);
	Coincidences->SetBranchAddress("submoduleID2",&submoduleID2);
	Coincidences->SetBranchAddress("time1",&time1);
	Coincidences->SetBranchAddress("time2",&time2);

  // Stat
	gStyle -> SetStatW(0.28);
	gStyle -> SetStatH(0.3);
	gStyle -> SetStatColor(41);
	gStyle -> SetStatX(1);
	gStyle -> SetStatY(1);
	gStyle -> SetStatFont(42);
	gStyle -> SetOptStat(0);
	gStyle -> SetOptFit(0);

  gStyle -> SetLegendBorderSize(1);
  gStyle -> SetLegendFillColor(0);
  gStyle -> SetLegendFont(42);
  gStyle -> SetLegendTextSize(0.022);

  // Reset

	Int_t nentries = Coincidences->GetEntries();
	Int_t nbytes = 0, nbytesdelay = 0, nrandom = 0, nscatter = 0, ntrue = 0;

	cout << endl;
	cout << "##### Reading from file: " << RootFileName << endl;
	cout << endl;

	// Loop for each event in the TTree Coincidences

	for (Int_t i=0; i<nentries;i++) {
		nbytes += Coincidences->GetEntry(i);
    //std::cout << rsectorID1 << '\n';
    delta_time = time2 - time1;
    //std::cout << delta_time << '\n';
    spat_diff = sqrt(pow((globalPosX1-globalPosX2),2)+pow((globalPosY1-globalPosY2),2));
    reco_x = (globalPosX2 + globalPosX1)/2 + (globalPosX1 - globalPosX2) / spat_diff * delta_time * c_speed / 2;
    reco_y = (globalPosY2 + globalPosY1)/2 + (globalPosY1 - globalPosY2) / spat_diff * delta_time * c_speed / 2;

		if (eventID1 != eventID2) {
      // Random coincidence
      Reco_Random->Fill(reco_x,reco_y);
      //std::cout << "Random: " << delta_time << '\n';

      ++nrandom;

		} else {
      // True coincidence

      DetectPos->Fill(globalPosX1,globalPosY1);
			DetectPos->Fill(globalPosX2,globalPosY2);
			SourcePos->Fill(sourcePosX1,sourcePosY1);
			SourcePos->Fill(sourcePosX1,sourcePosY1);

			if (comptonPhantom1 == 0 && comptonPhantom2 == 0 &&
				  RayleighPhantom1 == 0 && RayleighPhantom2 == 0){
        // true unscattered coincidence
        // without scattering in crystal

        //std::cout << globalPosX1 << ", " << globalPosX2 <<'\n';
        Reco->Fill(reco_x,reco_y);
        //std::cout << "true_uns: " << delta_time << '\n';
				ntrue++;

			} else {
        // true scattered coincidence
        Reco_Scattered->Fill(reco_x,reco_y);

				nscatter++;
			}
		}
	}

	cout << endl << endl;
	cout << " *********************************************************************************** " << endl;
	cout << " *                                                                                 * " << endl;
	cout << " *   E d u G a t e  S i m u l a t i o n   A n a l y s i s                          * " << endl;
	cout << " *   T O F                                                                         * " << endl;
	cout << " *                                                                                 * " << endl;
	cout << " *********************************************************************************** " << endl;
	cout << endl << endl;
	cout << " bytes read                    : " << nbytes << endl;
	cout << " total coincidences            : " << nentries << endl;
	cout << " true unscattered coincidences : " << ntrue << endl;
	cout << " random coincidences           : " << nrandom << endl;
	cout << " scattered coincidences        : " << nscatter << endl;
	cout << " ratio nscatter/ntrue          : " << (float)nscatter/(float)(ntrue) << endl;
	cout << endl << endl;
	cout << " *********************************************************************************** " << endl;
	cout << endl << endl;

	TCanvas *c1=fEcanvas->GetCanvas();
	Int_t pos=1;
	c1->SetFillColor(0);
	c1->SetBorderMode(0);

  c1->cd(pos++);
	DetectPos->GetXaxis()->SetTitle("mm");
	DetectPos->GetYaxis()->SetTitle("mm");
	DetectPos->GetYaxis()->SetTitleOffset(1.2);
  DetectPos->SetFillColor(1);
  DetectPos->SetMarkerColorAlpha(1,0.5);
  DetectPos->SetMarkerSize(1);
  DetectPos->SetMarkerStyle(6);
	TGaxis::SetMaxDigits(3);
	DetectPos->Draw();

  //c1->cd(pos++);
	Reco_Scattered->GetXaxis()->SetTitle("mm");
	Reco_Scattered->GetYaxis()->SetTitle("mm");
	Reco_Scattered->GetYaxis()->SetTitleOffset(1.2);
  Reco_Scattered->SetFillColor(7);
  Reco_Scattered->SetMarkerColorAlpha(7,0.5);
  Reco_Scattered->SetMarkerStyle(2);
  Reco_Scattered->SetMarkerSize(0.5);
	TGaxis::SetMaxDigits(3);
	Reco_Scattered->Draw("same");

  //c1->cd(pos++);
	Reco_Random->GetXaxis()->SetTitle("mm");
	Reco_Random->GetYaxis()->SetTitle("mm");
	Reco_Random->GetYaxis()->SetTitleOffset(1.2);
  Reco_Random->SetFillColor(8);
  Reco_Random->SetMarkerColorAlpha(8,0.5);
  Reco_Random->SetMarkerStyle(5);
  Reco_Random->SetMarkerSize(2);
	TGaxis::SetMaxDigits(3);
	Reco_Random->Draw("same");

  //c1->cd(pos++);
	Reco->GetXaxis()->SetTitle("mm");
	Reco->GetYaxis()->SetTitle("mm");
	Reco->GetYaxis()->SetTitleOffset(1.2);
  Reco->SetFillColor(2);
  Reco->SetMarkerColorAlpha(2,0.5);
  Reco->SetMarkerStyle(6);
  Reco->SetMarkerSize(4);
	TGaxis::SetMaxDigits(3);
	Reco->Draw("same");

  //c1->cd(pos++);
	SourcePos->GetXaxis()->SetTitle("mm");
	SourcePos->GetYaxis()->SetTitle("mm");
	SourcePos->GetYaxis()->SetTitleOffset(1.2);
  SourcePos->SetFillColor(18);
  SourcePos->SetMarkerColorAlpha(18,0.5);
  SourcePos->SetMarkerStyle(6);
  SourcePos->SetMarkerSize(4);
	TGaxis::SetMaxDigits(3);
	SourcePos->Draw("same");

  //legend
  auto legend = new TLegend(0.1,0.7,0.65,0.9);
  legend->SetHeader("Calculation of the origin","C"); // option "C" allows to center the header
  legend->AddEntry(DetectPos,"Position of detected events","f");
  legend->AddEntry(SourcePos,"Position of the source","f");
  legend->AddEntry(Reco,"Calculation (true and unscattered)","f");
  legend->AddEntry(Reco_Scattered,"Calculation (true and scattered)","f");
  legend->AddEntry(Reco_Random,"Calculation (random)","f");
  legend->Draw();

	c1->Update();
	c1->SaveAs(GraphicFileName);
}

MyMainFrame::~MyMainFrame()
{
  fMain->Cleanup();
	delete fMain;
}

TestFileList::TestFileList(const TGWindow *p, const TGWindow *pmain, UInt_t w, UInt_t h)
{
   TGLayoutHints *lo;

   fMain = new TGTransientFrame(p, pmain, w, h);
   fMain->Connect("CloseWindow()", "TestFileList", this, "CloseWindow()");
   fMain->DontCallClose(); // to avoid double deletions.

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
   fMenu->Connect("Activated(Int_t)","TestFileList",this,"DoMenu(Int_t)");

   TGListView* lv = new TGListView(fMain, w, h);
   lo = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   fMain->AddFrame(lv,lo);

   Pixel_t white;
   gClient->GetColorByName("white", white);
   fContents = new TGFileContainer(lv, kSunkenFrame,white);
   fContents->Connect("DoubleClicked(TGFrame*,Int_t)", "TestFileList", this,
                      "OnDoubleClick(TGLVEntry*,Int_t)");

   fMain->CenterOnParent();
   fMain->SetWindowName("Select Root File");
   fMain->MapSubwindows();
   fMain->MapWindow();
   fContents->SetFilter("*.root");
   fContents->SetDefaultHeaders();
   fContents->DisplayDirectory();
   fContents->AddFile("..");
   fContents->Sort(kSortByDate);
   fContents->Resize();
   fContents->StopRefreshTimer();
   #if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
      fContents->SetViewMode(kLVDetails);
   #else
      fContents->SetViewMode(3);
   #endif
   fMain->Resize();
}

TestFileList::~TestFileList()
{
   delete fContents;
   delete fMenu;
   fMain->DeleteWindow();
}

void TestFileList::DoMenu(Int_t mode)
{
   if (mode<10) {
      fContents->SetViewMode((EListViewMode)mode);
   } else {
      delete this;
   }
}

void TestFileList::DisplayFile(const TString &fname)
{
	cout<<fname<<endl;
	pmain->TOF(fname);
	fMain->CloseWindow();
}
void TestFileList::DisplayDirectory(const TString &fname)
{
   fContents->SetDefaultHeaders();
   gSystem->ChangeDirectory(fname);
   fContents->ChangeDirectory(fname);
   fContents->DisplayDirectory();
   fContents->AddFile("..");
   fMain->Resize();
}
void TestFileList::DisplayObject(const TString& fname,const TString& name)
{
   TDirectory *sav = gDirectory;

   static TFile *file = 0;
   if (file) delete file;
   file = new TFile(fname);

   TObject* obj = file->Get(name);
   if (obj) {
      if (!obj->IsFolder()) {
         obj->Browse(0);
      } else obj->Print();
   }
   gDirectory = sav;
}

void TestFileList::OnDoubleClick(TGLVEntry *f, Int_t btn)
{
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

void TestFileList::CloseWindow()
{
   delete this;
}

void TOF()
{
   pmain = new MyMainFrame(gClient->GetRoot(), 350, 80);
}
