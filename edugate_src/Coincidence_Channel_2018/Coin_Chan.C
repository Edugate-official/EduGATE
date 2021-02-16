
// Coin_Chan.C
// Macro for the evaluation of the Coin_Chan EduGATE example.
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
    TH2F *Sinogram;
	TH1F *SourcePos;
	TH1F *EnergyDstnTrue;
	TH1F *EnergyDstnTrueUnsc;
	TH1F *EnergyDstnTrueSc;
	TH1F *DetectPosAxial;
	TH1F *AxialSensitivityDet;
	TH1F *AxialScattersDet;
	TH1F *AxialProfileDet;
	TH1F *ScatterFractionAxialDet;

public:
   MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~MyMainFrame();

   // slots

   void CloseWindow();
   void ChooseFile();
   void CoinChan(TString fname);

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
	
	// slots
	void OnDoubleClick(TGLVEntry*,Int_t);
	void DoMenu(Int_t);
	void CloseWindow();
	
};


void MyMainFrame::CloseWindow()
{
	// Got close message for this MainFrame. Terminates the application.
	
	gApplication->Terminate(0);
}

void MyMainFrame::ChooseFile()
{
  // Slot connected to the Clicked() signal. 
  // It will open a File Browser which allows to choose a file for processing.
  
  choose->SetState(kButtonDown);
  choose->SetText("&Select Root File");  
  new TestFileList(gClient->GetRoot(), fMain,  1280, 800);
  choose->SetState(kButtonUp);
}


MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h) 
{

	// Create a pmain frame
	fMain = new TGMainFrame(p,w,h);
	fMain->Connect("CloseWindow()", "MyMainFrame", this, "CloseWindow()");
	fMain->DontCallClose(); // to avoid double deletions.
	
	// Create canvas widget
	fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,750,750); 
	fEcanvas->GetCanvas()->Divide(3,3);
	fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1));
	// Create a horizontal frame widget with buttons
	TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40); 
	choose = new TGTextButton(hframe,"&Select Root File"); 
	choose->Connect("Clicked()","MyMainFrame",this,"ChooseFile()"); 
	hframe->AddFrame(choose, new TGLayoutHints(kLHintsCenterX,5,5,3,4)); 
	exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)"); 
	hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
	// Set a name to the pmain frame
	fMain->SetWindowName("Coincidence Channel");
	// Map all subwindows of pmain frame
	fMain->MapSubwindows();
	// Initialize the layout algorithm
	fMain->Resize(fMain->GetDefaultSize());
	// Map pmain frame
	fMain->MapWindow();
	
	Float_t	E_min=0.0, E_max=800.0;
	
	DetectPos				= new TH2F("DetectPos","Transaxial Detection Position ",200,-150.,+150.,200,-150.,+150.);
    Sinogram				= new TH2F("Sinoram","Sinogram (S vs Theta) ",150,-75.,+75.,100,0.,+3.2);	
	SourcePos				= new TH1F("SourcePos","SourcePos ",100,-4.,+4.);
	EnergyDstnTrue			= new TH1F("EnergyDstnTrue","Energy Distribution True",200,E_min,E_max);
	EnergyDstnTrueUnsc		= new TH1F("EnergyDstnTrueUnsc","Energy Distribution True Unscattered",200,E_min,E_max);
	EnergyDstnTrueSc		= new TH1F("EnergyDstnTrueSc","Energy Distribution True Scattered",200,E_min,E_max);
	DetectPosAxial			= new TH1F("DetectPosAxial","Axial Detection Position",100,-20.,+20.);
	AxialSensitivityDet		= new TH1F("AxialSensitivityDet","Axial Sensitivity",10,-20.,+20.);
	AxialScattersDet		= new TH1F("AxialScattersDet","Axial Scatters Distribution",100,-20.,+20.);
	AxialProfileDet			= new TH1F("AxialProfileDet","",100,-20.,+20.);
	ScatterFractionAxialDet	= new TH1F("ScatterFractionAxialDet","Axial Scatter Fraction",100,-20.,+20.);
}

void MyMainFrame::CoinChan(TString fname)
{
	
	TString RootFileName = fname;
	TFile f(RootFileName);
	
	TString GraphicFileName = RootFileName.Remove(RootFileName.Length()-5) + ".gif";
	
	TTree *Coincidences = (TTree*)gDirectory->Get("Coincidences");
	// TTree *Hits = (TTree*)gDirectory->Get("Hits");
	// TTree *Singles = (TTree*)gDirectory->Get("Singles");
	
	//
	//Declaration of leaves types - TTree Coincidences
	//  
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
	
	Float_t         zmin,zmax,z;
	Int_t			crystalx, crystaly;

	//   
	//Set branch addresses - TTree Coincicences
	//  

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
	
	
	/////////////STAT////////   
	gStyle -> SetStatW(0.28);
	gStyle -> SetStatH(0.3);
	gStyle -> SetStatColor(41);   
	gStyle -> SetStatX(1);
	gStyle -> SetStatY(1);   
	gStyle -> SetStatFont(42);
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);
	/////////////////////////
	
	//	
	// Reset Histograms
	//

    Sinogram->Reset();
	DetectPos->Reset(); 
	SourcePos->Reset();
	EnergyDstnTrue->Reset();
	EnergyDstnTrueUnsc->Reset();			
	EnergyDstnTrueSc->Reset();
	DetectPosAxial->Reset();		
	AxialSensitivityDet->Reset();	
	AxialScattersDet->Reset();		
	AxialProfileDet->Reset();	
	ScatterFractionAxialDet->Reset();	
  	
	Int_t nentries = Coincidences->GetEntries();
	Int_t nbytes = 0, nbytesdelay = 0, nrandom = 0, nscatter = 0, ntrue = 0;
	
	cout << endl;
	cout << "##### Reading from file: " << RootFileName << endl;
	cout << endl;
	
	//
	// Loop for each event in the TTree Coincidences
	//
	for (Int_t i=0; i<nentries;i++) {
		
		if ((i%250000)  == 0 && i!=0)  printf("... %d ",i);       
		if ((i%1000000) == 0 && i!=0)  printf("... %d\n",i);		
		//		if (fmod((double)i,25000.0) == 0.0) cout << ".";
		
		nbytes += Coincidences->GetEntry(i);
		
		if (eventID1 != eventID2) {									// Random coincidence
			
			++nrandom;
			
		} else {													// True coincidence
			
			DetectPos->Fill(globalPosX1,globalPosY1);
			DetectPos->Fill(globalPosX2,globalPosY2);					
			DetectPos->Fill(sourcePosX1,sourcePosY1);
			DetectPos->Fill(sourcePosX1,sourcePosY1);
			//Sinogram->Fill(sinogramS,sinogramTheta);
			EnergyDstnTrue->Fill(energy1*1000.0);
			EnergyDstnTrue->Fill(energy2*1000.0);
			DetectPosAxial->Fill(globalPosZ1);
			DetectPosAxial->Fill(globalPosZ2);
			SourcePos->Fill(sourcePosZ1);
			SourcePos->Fill(sourcePosZ2);
			
			z = (globalPosZ1+globalPosZ2)/2.;
			
			if (comptonPhantom1 == 0 && comptonPhantom2 == 0 &&
				RayleighPhantom1 == 0 && RayleighPhantom2 == 0) {	// true unscattered coincidence	
				
				Sinogram->Fill(sinogramS,sinogramTheta);
				EnergyDstnTrueUnsc->Fill(energy1*1000.0);
				EnergyDstnTrueUnsc->Fill(energy2*1000.0);
				AxialSensitivityDet->Fill(z);
				ntrue++;
				
			} else {												// true scattered coincidence
				
				//Sinogram->Fill(sinogramS,sinogramTheta);
				EnergyDstnTrueSc->Fill(energy1*1000.0);
				EnergyDstnTrueSc->Fill(energy2*1000.0);
				AxialScattersDet->Fill(z);
				nscatter++;
			}
			
			AxialProfileDet->Fill(z);
		}
	} // for loop
	
	cout << endl << endl;
	cout << " *********************************************************************************** " << endl;
	cout << " *                                                                                 * " << endl;
	cout << " *   E d u G a t e  S i m u l a t i o n   A n a l y s i s                          * " << endl;
	cout << " *   C o i n _ C h a n                                                             * " << endl;
	cout << " *                                                                                 * " << endl;
	cout << " *********************************************************************************** " << endl;
	cout << endl << endl;
	cout << " bytes read                    : " << nbytes << endl;     
	cout << " total coincidences            : " << nentries << endl;
	cout << " true unscattered coincidences : " << ntrue << endl;
	cout << " random coincidences           : " << nrandom << endl;
	cout << " scattered coincidences        : " << nscatter << endl;
	//	cout << "  ==> global scatter fraction = " << (float)nscatter/(float)(nentries-nrandom) << endl;
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
	TGaxis::SetMaxDigits(3);
	DetectPos->Draw();
	
	c1->cd(pos++);
	EnergyDstnTrue->GetXaxis()->SetTitle("MeV");
	EnergyDstnTrue->GetYaxis()->SetTitle("# entries");
	EnergyDstnTrue->GetYaxis()->SetTitleOffset(1.2);
	TGaxis::SetMaxDigits(3);
	EnergyDstnTrue->Draw();
	
	c1->cd(pos++);
	EnergyDstnTrueUnsc->GetXaxis()->SetTitle("MeV");
	EnergyDstnTrueUnsc->GetYaxis()->SetTitle("# entries");
	EnergyDstnTrueUnsc->GetYaxis()->SetTitleOffset(1.2);
	TGaxis::SetMaxDigits(3);
	EnergyDstnTrueUnsc->Draw();
	
	c1->cd(pos++);
	EnergyDstnTrueSc->GetXaxis()->SetTitle("MeV");
	EnergyDstnTrueSc->GetYaxis()->SetTitle("# entries");
	EnergyDstnTrueSc->GetYaxis()->SetTitleOffset(1.2);
	TGaxis::SetMaxDigits(3);
	EnergyDstnTrueSc->Draw();
	
	c1->cd(pos++);
	Sinogram->GetXaxis()->SetTitle("mm");
	Sinogram->GetYaxis()->SetTitle("rad");
	Sinogram->GetYaxis()->SetTitleOffset(1.2);
	TGaxis::SetMaxDigits(3);
	Sinogram->Draw();
	
	c1->cd(pos++);
	SourcePos->GetXaxis()->SetTitle("mm");
	SourcePos->GetYaxis()->SetTitle("# entries");
	SourcePos->GetYaxis()->SetTitleOffset(1.2);
	TGaxis::SetMaxDigits(3);
	SourcePos->Draw();
	
	c1->cd(pos++);
	DetectPosAxial->GetXaxis()->SetTitle("mm");
	DetectPosAxial->GetYaxis()->SetTitle("# entries");
	DetectPosAxial->GetYaxis()->SetTitleOffset(1.2);
	TGaxis::SetMaxDigits(3);
	DetectPosAxial->Draw();
	
	c1->cd(pos++);
	AxialSensitivityDet->GetXaxis()->SetTitle("mm");
	AxialSensitivityDet->GetYaxis()->SetTitle("# entries");
	AxialSensitivityDet->GetYaxis()->SetTitleOffset(1.2);
	TGaxis::SetMaxDigits(3);
	AxialSensitivityDet->Draw();
	
	c1->cd(pos++);
	ScatterFractionAxialDet->GetXaxis()->SetTitle("mm");
	ScatterFractionAxialDet->GetYaxis()->SetTitle("# entries");
	ScatterFractionAxialDet->GetYaxis()->SetTitleOffset(1.2);
	TGaxis::SetMaxDigits(3);
	ScatterFractionAxialDet->Divide(AxialScattersDet,AxialProfileDet,1.,1.,"");  
	ScatterFractionAxialDet->Draw();
	
	c1->Update();    
	c1->SaveAs(GraphicFileName);
	
	
}	

MyMainFrame::~MyMainFrame()
{
   // Clean up all widgets, frames and layouthints that were used
   fMain->Cleanup();
	delete fMain;
	delete DetectPos;
	delete SourcePos;
    delete Sinogram;
	delete EnergyDstnTrue;
	delete EnergyDstnTrueUnsc;	
	delete EnergyDstnTrueSc;
	delete DetectPosAxial;
	delete AxialSensitivityDet;
	delete AxialScattersDet;
	delete AxialProfileDet;
	delete ScatterFractionAxialDet;
}


TestFileList::TestFileList(const TGWindow *p, const TGWindow *pmain, UInt_t w, UInt_t h)
{
   // Create transient frame containing a filelist widget.

   TGLayoutHints *lo;	

   fMain = new TGTransientFrame(p, pmain, w, h);
   fMain->Connect("CloseWindow()", "TestFileList", this, "CloseWindow()");
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
   fMenu->Connect("Activated(Int_t)","TestFileList",this,"DoMenu(Int_t)");

   TGListView* lv = new TGListView(fMain, w, h);
   lo = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   fMain->AddFrame(lv,lo);

   Pixel_t white;
   gClient->GetColorByName("white", white);
   fContents = new TGFileContainer(lv, kSunkenFrame,white);
   fContents->Connect("DoubleClicked(TGFrame*,Int_t)", "TestFileList", this,
                      "OnDoubleClick(TGLVEntry*,Int_t)");

   // position relative to the parent's window
   fMain->CenterOnParent();

   fMain->SetWindowName("Select Root File");
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

TestFileList::~TestFileList()
{
   // Cleanup.

   delete fContents;
   delete fMenu;
   fMain->DeleteWindow();  // deletes fMain
}

void TestFileList::DoMenu(Int_t mode)
{
   // Switch view mode.

   if (mode<10) {
      fContents->SetViewMode((EListViewMode)mode);
   } else {
      delete this;
   }
}

void TestFileList::DisplayFile(const TString &fname)
{
   // Display content of ROOT file.
	cout<<fname<<endl;
	pmain->CoinChan(fname);
	fMain->CloseWindow();

}

void TestFileList::DisplayDirectory(const TString &fname)
{
   // Display content of directory.

   fContents->SetDefaultHeaders();
   gSystem->ChangeDirectory(fname);
   fContents->ChangeDirectory(fname);
   fContents->DisplayDirectory();
   fContents->AddFile("..");  // up level directory
   fMain->Resize();
}

void TestFileList::DisplayObject(const TString& fname,const TString& name)
{
   // Browse object located in file.

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

void TestFileList::OnDoubleClick(TGLVEntry *f, Int_t btn)
{
   // Handle double click.

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


void Coin_Chan()
{
   // Popup the GUI...

   pmain = new MyMainFrame(gClient->GetRoot(), 350, 80);
}
