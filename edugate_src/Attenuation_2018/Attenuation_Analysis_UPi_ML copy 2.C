
// Attenuation_Analysis_UPi_ML
// Macro for the evaluation of the Attenuation_Analysis_UPi_ML EduGATE example.
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
  
	TH1F *primary;
	TH1F *sc_ph;
	TH1F *sc_cry;
	TH1F *sc_com;
	TH1F *sc_ta ;
	TH1F *sc_col;
	TH1F *ener;
	
	TH1F *o1;
	TH1F *o2;
	TH1F *o3;
	TH1F *o4;
    TH1F *o5;
	TH1F *osup;
	
	TH2F *globalPosZ_Y;

public:
   MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~MyMainFrame();

   // slots

   void CloseWindow();
   void ChooseFile();
   void GammaCamera(TString fname);

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
	TestFileList(const TGWindow *p, const TGWindow *pmain,/* TCanvas *can,*/ UInt_t w, UInt_t h);
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
  
  choose->SetText("&Select Root File"); 
  choose->SetState(kButtonDisabled); 
  new TestFileList(gClient->GetRoot(), fMain,  1280, 600);
}


MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h)

{

	// Create a main frame
	fMain = new TGMainFrame(p,w,h);
	fMain->Connect("CloseWindow()", "MyMainFrame", this, "CloseWindow()");
	fMain->DontCallClose(); // to avoid double deletions.
	
	// Create canvas widget
	fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,1050, 750); 
	fEcanvas->GetCanvas()->Divide(2,2);
	fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1));
	
	// Create a horizontal frame widget with buttons
	TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40); 
	choose = new TGTextButton(hframe,"&Select Root File"); 
	choose->Connect("Clicked()","MyMainFrame",this,"ChooseFile()"); 
	hframe->AddFrame(choose, new TGLayoutHints(kLHintsCenterX,5,5,3,4)); 
	exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)"); 
	hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
	
	// Set a name to the main frame
	fMain->SetWindowName("Gamma Camera");
	
	// Map all subwindows of main frame
	fMain->MapSubwindows();
	
	// Initialize the layout algorithm
	fMain->Resize(fMain->GetDefaultSize());
	
	// Map main frame
	fMain->MapWindow();
	
	
	primary = new TH1F("primary","",100,0,1);
	sc_ph   = new TH1F("sc_ph","",100,0,1);
	sc_cry  = new TH1F("sc_mo","",100,0,1);
	sc_com  = new TH1F("sc_com","",100,0,1);
	sc_ta   = new TH1F("sc_ta","",100,0,1);
	sc_col  = new TH1F("sc_col","",100,0,1);
	ener    = new TH1F("ener","",100,0,1);
    
    //cout<<"Runig"<<endl;
	
	o1   = new TH1F("o1","",100,0,1);
	o2   = new TH1F("o2","",100,0,1);
	o3   = new TH1F("o3","",100,0,1);
	o4   = new TH1F("o4","",100,0,1);
    o5   = new TH1F("o5","",100,0,1);
    //osup = new TH1F("osup","",100,0,1);
	
	globalPosZ_Y = new TH2F("globalPosZ","globalPosY ",600,-150.0,+150.,400,-100.0,+100.0);

}

void MyMainFrame::GammaCamera(TString fname)
{
	
	TString RootFileName = fname;
	TFile f(RootFileName);
	
	TString GraphicFileName = RootFileName.Remove(RootFileName.Length()-5) + ".gif";	

	TH1 *total_nb_primaries = (TH1*)gDirectory->Get("total_nb_primaries");
	TTree *Singles = (TTree*)gDirectory->Get("Singles");
    TTree *Hits = (TTree*)gDirectory->Get("Hits");
	
	
	Int_t           comptonPhantom;
	Int_t           comptonCrystal;
	Int_t           RayleighCrystal;
	Int_t           RayleighPhantom;
	Float_t         energy;
    Char_t          comptVolName[40];
	Float_t			scatter_phantom, scatter_null, scatter_compartment, scatter_table;
	Float_t			scatter_collim, scatter_shielding, primary_event, scatter_crystal;
	Float_t			order1, order2, order3, order4, order5, ordersup, ordertot;
    
    comptonPhantom      = 0;
    comptonCrystal      = 0;
    RayleighCrystal     = 0;
    energy              = 0.;
    scatter_phantom     = 0.;
    scatter_null        = 0.;
    scatter_compartment = 0.;
    scatter_table       = 0.;
    
    scatter_collim      = 0.;
    scatter_shielding   = 0.;
    primary_event       = 0 ;
    scatter_crystal     = 0.;
    
    order1  = 0.;
    order2  = 0.;
    order3  = 0.;
    order4  = 0.;
    order5  = 0.;
//    ordersup = 0.;
//    ordertot = 0.;
    
	
	Float_t	        globalPosX, globalPosY, globalPosZ, sourcePosX, sourcePosY, sourcePosZ;
    
    globalPosX = 0.;
    globalPosY = 0.;
    globalPosZ = 0.;
    sourcePosX = 0.;
    sourcePosY = 0.;
    sourcePosZ = 0.;
    
	Double_t        time;
    time        = 0.;
    
	Int_t			runID, runID_hits;
    runID       = 0 ;
    runID_hits  = 0 ;
	
    Int_t			eventID, eventID_hits;
    Int_t           sourceID;
    
    eventID         = 0;
    eventID_hits    = 0;
    sourceID        = 0;
    
    Char_t          processName[40];
	
    Int_t			PDGEncoding;
    PDGEncoding     = 0;
	
	Singles->SetBranchAddress("comptonPhantom",&comptonPhantom);
	Singles->SetBranchAddress("comptonCrystal",&comptonCrystal);
	Singles->SetBranchAddress("RayleighCrystal",&RayleighCrystal);
	Singles->SetBranchAddress("RayleighPhantom",&RayleighPhantom);
	Singles->SetBranchAddress("energy",&energy);
	Singles->SetBranchAddress("comptVolName",comptVolName);
	
	Singles->SetBranchAddress("globalPosX",&globalPosX);
	Singles->SetBranchAddress("globalPosY",&globalPosY);	
	Singles->SetBranchAddress("globalPosZ",&globalPosZ);
	
	Singles->SetBranchAddress("sourcePosX",&sourcePosX);
	Singles->SetBranchAddress("sourcePosY",&sourcePosY);
	Singles->SetBranchAddress("sourcePosZ",&sourcePosZ);
    Singles->SetBranchAddress("sourceID",&sourceID);
	Singles->SetBranchAddress("time",&time);
	
	Singles->SetBranchAddress("runID", &runID);
	Singles->SetBranchAddress("eventID", &eventID);
	
	Hits->SetBranchAddress("processName",processName);
	Hits->SetBranchAddress("PDGEncoding",&PDGEncoding);
	Hits->SetBranchAddress("runID", &runID_hits);
	Hits->SetBranchAddress("eventID", &eventID_hits);	
	
    
    cout << "##### Reading from file: " << RootFileName << endl;
    cout << endl;
    
    double maximum_energy = Singles->GetMaximum("energy");
    double Emax = maximum_energy + 0.2*maximum_energy;
	
    cout << endl;
    cout << " maximum_energy " << " " <<  maximum_energy << endl;
    cout << " Emax " << " " <<  Emax << endl;
    cout << endl;
    
	//	
	// Reset Histograms
	//
	
    
	primary->Reset();
	primary->SetBins(100,0,Emax);
	sc_ph->Reset();
	sc_ph->SetBins(100,0,Emax);
	sc_cry->Reset();
	sc_cry->SetBins(100,0,Emax);
	sc_com->Reset();
	sc_com->SetBins(100,0,Emax);
	sc_ta->Reset();
	sc_ta->SetBins(100,0,Emax);
	sc_col->Reset();
	sc_col->SetBins(100,0,Emax);
	ener->Reset();
	ener->SetBins(100,0,Emax);
	
	o1->Reset();
	o1->SetBins(100,0,Emax);
	o2->Reset();
	o2->SetBins(100,0,Emax);
	o3->Reset();
	o3->SetBins(100,0,Emax);
	o4->Reset();
	o4->SetBins(100,0,Emax);
	o5->Reset();
	o5->SetBins(100,0,Emax);

 //   osup->Reset();
 //   osup->SetBins(100,0,Emax);
    
	globalPosZ_Y->Reset();

    
    
    
	Int_t nentries = Singles->GetEntries();
	Int_t ntotal = total_nb_primaries->GetMean();
	Int_t nbytes = 0;
	
	cout<<"##### nb of primaries: " << ntotal <<endl;
	cout<<"##### nb of entries: " << nentries <<endl;
    
 
	for (Int_t i=0; i<nentries;i++) {
        
        
		//for (Int_t i=0; i<10;i++) {	
		nbytes += Singles->GetEntry(i);
		
		globalPosZ_Y->Fill(globalPosZ,globalPosY);
		ener->Fill(energy);
		if (comptonPhantom == 0 && comptonCrystal == 0) {
			primary->Fill(energy);
			primary_event++;
		} // primary
		
		if (comptonCrystal != 0 && comptonPhantom == 0) {
			scatter_crystal++;
			sc_cry->Fill(energy);
		} // scatter in crystal
		
		if (strcmp(comptVolName,"phantom_phys")==  0||strcmp(comptVolName,"source_vol_phys") == 0) {
			scatter_phantom++;
			sc_ph->Fill(energy);
		}	// scatter in phantom
		
		
		if (strcmp(comptVolName,"compartment_phys") == 0) {
			scatter_compartment++;
			sc_com->Fill(energy);
		}
		
		if (strcmp(comptVolName,"table_phys") == 0) {
			scatter_table++;
			sc_ta->Fill(energy);
		}
		
		if (strcmp(comptVolName,"collimator_phys") == 0) {
			scatter_collim++;
			sc_col->Fill(energy);
		}
		
        
        if(sourceID == 0) {
            o1->Fill(energy);
            order1++;
        }
        
        if(sourceID == 1) {
            o2->Fill(energy);
            order2++;
        }
        
        if(sourceID == 2) {
            o3->Fill(energy);
            order3++;
        }
        
        if(sourceID == 3) {
            o4->Fill(energy);
            order4++;
        }
        
        if(sourceID == 4) {
            o5->Fill(energy);
            order5++;
        }
        
        
	} // nentries
	

	// **************************************** Plots **********************************************
	
	gStyle->SetPalette(1);
	gStyle->SetOptStat(0);
	
	// First Canvas
	
	TCanvas *C1=fEcanvas->GetCanvas();
	
	C1->SetFillColor(0);
	C1->cd(1);
	C1->cd(1)->SetLogy();
	
	ener->SetFillColor(2);
	ener->SetFillStyle(3023);
	ener->GetXaxis()->SetTitle("MeV");
	ener->GetYaxis()->SetTitle("# entries");
	ener->SetTitle("Spectrum of the Detected Events");
	ener->Draw();
	
	C1->SetFillColor(0);
	C1->cd(2);
	
	primary->Draw();
	primary->SetLineColor(1);
	primary->GetXaxis()->SetTitle("MeV");
	primary->GetYaxis()->SetTitle("# entries");
	primary->SetTitle("Primary and Scatter Spectra of the Detected Events");
	TGaxis::SetMaxDigits(3);
	sc_ph->Draw("same");
	sc_ph->SetLineColor(6);
	
	sc_ta->Draw("same");
	sc_ta ->SetLineColor(kRed+2);
	
	sc_col->Draw("same");
	sc_col ->SetLineColor(kGreen+1);
	
	sc_com->Draw("same");
	sc_com ->SetLineColor(kBlue+1);
	
	sc_cry->Draw("same");
	sc_cry ->SetLineColor(kOrange-3);
	
	TLegend *leg1 = new TLegend(0.55,0.65,0.9,0.9);
	leg1->SetFillColor(0);
	leg1->SetTextSize(0.03);
	leg1->AddEntry(primary,"primary spectrum                 ","l");
	leg1->AddEntry(sc_ph,  "scatter in phantom           ","l");
	leg1->AddEntry(sc_ta,  "scatter in table             ","l");
	leg1->AddEntry(sc_col, "scatter in collimator        ","l");
	leg1->AddEntry(sc_com, "scatter in backcompartment   ","l");
	leg1->AddEntry(sc_cry, "scatter in crystal           ","l");
	leg1->Draw();

	
	C1->SetFillColor(0);
	C1->cd(3);
	
	o1->Draw();
	o1->GetXaxis()->SetTitle("MeV");
	o1->GetYaxis()->SetTitle("# entries");	
	o1->SetTitle("Scatter Spectra of the Detected Events");
	TGaxis::SetMaxDigits(3);
	o1 ->SetLineColor(1);
	o2->Draw("same");
	o2 ->SetLineColor(kRed+2);
	o3->Draw("same");
	o3 ->SetLineColor(kGreen+2);
	o4->Draw("same");
	o4 ->SetLineColor(kBlue+1);
	o5->Draw("same");
	o5 ->SetLineColor(kOrange-3);
	
	TLegend *leg2 = new TLegend(0.65,0.65,0.9,0.9);
	leg2->SetFillColor(0);
	leg2->SetTextSize(0.03);
//	leg2->AddEntry(o1,"1st order scatter","l");
//	leg2->AddEntry(o2,"2st order scatter","l");
//	leg2->AddEntry(o3,"3st order scatter","l");
//	leg2->AddEntry(o4,"4st order scatter","l");
//	leg2->AddEntry(osup,">4st order scatter","l");

    leg2->AddEntry(o1,"source 1","l");
    leg2->AddEntry(o2,"source 2","l");
    leg2->AddEntry(o3,"source 3","l");
    leg2->AddEntry(o4,"source 4","l");
    leg2->AddEntry(o5,"source 5","l");
	leg2->Draw();

	
	C1->SetFillColor(0);
	C1->cd(4);
	
	globalPosZ_Y->GetXaxis()->SetTitle("position z [mm]");
	globalPosZ_Y->GetYaxis()->SetTitle("position y [mm]");
	globalPosZ_Y->SetTitle("Spatial Distribution of Detected Events");
	globalPosZ_Y->Draw();
	
	// save Plot to disk 
	
	C1->Update();    
	C1->SaveAs(GraphicFileName);	
	
	// **************************************** Summary Printout **********************************************
	
	cout << " *********************************************************************************** " << endl;
	cout << " *                                                                                 * " << endl;
	cout << " *   E d u G a t e  S i m u l a t i o n   A n a l y s i s                          * " << endl;
	cout << " *   G A M M A  C A M E R A                                                        * " << endl;
	cout << " *                                                                                 * " << endl;
	cout << " *********************************************************************************** " << endl;
	cout << endl;
	
    cout<<"##### Number of emitted particles    :  "	<<ntotal<<endl;
    cout<<"##### Number of detected events      :  "	<<nentries<<endl;
    
    cout<<"##### Primary events                 :  "	<<primary_event       <<endl;
    cout<<"##### Scatter in the phantom         :  "	<<scatter_phantom     <<endl;
    cout<<"##### Scatter in the table           :  "	<<scatter_table       <<endl;
    cout<<"##### Scatter in the collimator      :  "	<<scatter_collim      <<endl;
    cout<<"##### Scatter in the crystal         :  "	<<scatter_crystal     <<endl;
    cout<<"##### Scatter in the backcompartment :  "	<<scatter_compartment <<endl;
    cout << endl;
    
	cout<<"##### ratio detected/emitted         :  "	<<nentries            *100/ ntotal   <<" %"<<endl;
	cout<<"##### Primary events                 :  "	<<primary_event       *100/ nentries <<" %"<<endl;
	cout<<"##### Scatter in the phantom         :  "	<<scatter_phantom     *100/ nentries <<" %"<<endl;
	cout<<"##### Scatter in the table           :  "	<<scatter_table       *100/ nentries <<" %"<<endl;
	cout<<"##### Scatter in the collimator      :  "	<<scatter_collim      *100/ nentries <<" %"<<endl;
	cout<<"##### Scatter in the crystal         :  "	<<scatter_crystal     *100/ nentries <<" %"<<endl;
	cout<<"##### Scatter in the backcompartment :  "	<<scatter_compartment *100/ nentries <<" %"<<endl;
	cout << endl;
	if (ntotal > 0) {
        cout<<"##### all sources :      "       <<ntotal<<endl;
		cout<<"##### source 1    :      "       <<order1/ntotal   * 100<<" %"<<endl;
		cout<<"##### source 2    :      "       <<order2/ntotal   * 100<<" %"<<endl;
		cout<<"##### source 3    :      "       <<order3/ntotal   * 100<<" %"<<endl;
		cout<<"##### source 4    :      "       <<order4/ntotal   * 100<<" %"<<endl;
		cout<<"##### source 5    :      "       <<order5/ntotal   * 100<<" %"<<endl;
	}
	cout << endl;
	
}	

MyMainFrame::~MyMainFrame()
{
   // Clean up all widgets, frames and layouthints that were used
   fMain->Cleanup();
	delete fMain;
	delete primary;
	delete sc_ph;
	delete sc_cry;
	delete sc_com;
	delete sc_ta ;
	delete sc_col;
	delete ener;
	
	delete o1;
	delete o2;
	delete o3;
	delete o4;
	delete o5;
	
	delete globalPosZ_Y;
   // cout<<"runing"<<endl;
	
}


TestFileList::TestFileList(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h)
{
   // Create transient frame containing a filelist widget.

   TGLayoutHints *lo;	

   fMain = new TGTransientFrame(p, main, w, h);
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

   fMain->SetWindowName("Choose Root File");
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
   // 4: small icons and file names   fMain->Resize();
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
	pmain->GammaCamera(fname);
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


void Attenuation_Analysis_UPi_ML()
{
   // Popup the GUI...

   pmain = new MyMainFrame(gClient->GetRoot(), 350, 80);
}
