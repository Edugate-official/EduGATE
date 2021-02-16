{
//	
// file: Plot_3d.C
//	
//	TChain * T = new TChain("Coincidences");
	gROOT->Reset();
	gStyle->SetOptStat(0);
	gStyle->SetCanvasPreferGL(kTRUE);
	gStyle->SetPalette(1);
	TChain * T = new TChain("Singles");

//	TString RootFileName = "MR_PET_Air_80_e-_1000_0.0.root";

	TString RootFileName;
	cout << " please enter RootFileName: " << endl;	
	cin >> RootFileName;
	cout << "##### Reading from file: " << RootFileName << endl << endl;
	cout << endl << endl;

	//TFile f(RootFileName);	

	T->Add(RootFileName);
	Float_t sourcePosX1;
	Float_t sourcePosY1;
	Float_t sourcePosZ1;
	Float_t globalPosX;
	Float_t globalPosY;
	Float_t globalPosZ;
	
	T->SetBranchAddress("sourcePosX",&sourcePosX1);
	T->SetBranchAddress("sourcePosY",&sourcePosY1);
	T->SetBranchAddress("sourcePosZ",&sourcePosZ1);
	T->SetBranchAddress("globalPosX",&globalPosX);
	T->SetBranchAddress("globalPosY",&globalPosY);
	T->SetBranchAddress("globalPosZ",&globalPosZ);


	int n = T->GetEntries();
	
	float binsize =0.15;
// //	h1 marks source position as a red dot in the plot of global positions
// 	TH3F *h1 = new TH3F("global position1", "global position", 100, -100, 100, 100, -100, 100, 100, -100, 100);
// 	h1->GetXaxis()->SetTitle(" z [mm]");
// 	h1->GetXaxis()->CenterTitle();
// 	h1->GetXaxis()->SetTitleOffset(2);
// 	h1->GetYaxis()->SetTitle(" x [mm]");
// 	h1->GetYaxis()->CenterTitle();
// 	h1->GetYaxis()->SetTitleOffset(2);
// 	h1->GetZaxis()->SetTitle(" y [mm]");
// 	h1->GetZaxis()->CenterTitle();
// 	h1->GetZaxis()->SetTitleOffset(2);

	TH3F *h2 = new TH3F("global position2", "global position", 50, -100, 100, 50, -100, 100, 50, -100, 100);
		h2->GetXaxis()->SetTitle(" z [mm]");
		h2->GetXaxis()->CenterTitle();
		h2->GetXaxis()->SetTitleOffset(2);
		h2->GetXaxis()->SetLabelOffset(0.005);
		h2->GetXaxis()->SetLabelSize(0.03);
		h2->GetYaxis()->SetTitle(" x [mm]");
		h2->GetYaxis()->CenterTitle();
		h2->GetYaxis()->SetTitleOffset(2);
		h2->GetYaxis()->SetLabelOffset(0.005);
		h2->GetYaxis()->SetLabelSize(0.03);
		h2->GetZaxis()->SetTitle(" y [mm]");
		h2->GetZaxis()->CenterTitle();
		h2->GetZaxis()->SetTitleOffset(1.6);
		h2->GetZaxis()->SetLabelOffset(0.015);
		h2->GetZaxis()->SetLabelSize(0.03);
	
	// TH3F *h3 = new TH3F("source position", "source position", 50, -binsize, binsize, 50, -binsize, binsize, 50, -binsize, binsize);
	// h3->GetXaxis()->SetTitle(" z [mm]");
	// h3->GetXaxis()->CenterTitle();
	// h3->GetXaxis()->SetTitleOffset(2);
	// h3->GetYaxis()->SetTitle(" x [mm]");
	// h3->GetYaxis()->CenterTitle();
	// h3->GetYaxis()->SetTitleOffset(2);
	// h3->GetZaxis()->SetTitle(" y [mm]");
	// h3->GetZaxis()->CenterTitle();
	// h3->GetZaxis()->SetTitleOffset(2);

	TPolyMarker3D *pm = new TPolyMarker3D(1,8);
    pm->SetPoint(0,0,0,0);
    pm->SetMarkerColor(kRed+2);

	TCanvas * canvas = new TCanvas("source",RootFileName,600,600);
	canvas->SetFrameFillColor(17);
	
	for (int i=0;i<n;i++)
	{
		T->GetEntry(i);
		// h1->Fill(sourcePosZ1,sourcePosX1,sourcePosY1);
		h2->Fill(globalPosZ,globalPosX,globalPosY);
		// h3->Fill(sourcePosZ1,sourcePosX1,sourcePosY1);
	}
	
    TColor *col46 = gROOT->GetColor(46);
    col46->SetAlpha(0.25);


    // lines: origin
		// create line for x
		    TPolyLine3D *pl3dx = new TPolyLine3D(2);
			pl3dx->SetPoint(0, -100, 0, 0);
		    pl3dx->SetPoint(1, 100, 0, 0);
		    pl3dx->SetLineWidth(1);
		    pl3dx->SetLineColor(46);

	    // create line for y
		    TPolyLine3D *pl3dy = new TPolyLine3D(2);
			pl3dy->SetPoint(0, 0, -100, 0);
		    pl3dy->SetPoint(1, 0, 100, 0);
		    pl3dy->SetLineWidth(1);
		    pl3dy->SetLineColor(46);

		// create line for z
		    TPolyLine3D *pl3dz = new TPolyLine3D(2);
			pl3dz->SetPoint(0, 0, 0, -100);
		    pl3dz->SetPoint(1, 0, 0, 100);
		    pl3dz->SetLineWidth(1);
		    pl3dz->SetLineColor(46);

	// canvas->cd(1);
    gPad->SetTheta( 30.) ; // use these settings to have the same perspective as in GATE. 
    gPad->SetPhi( -120.) ;
    gPad->Update();
	h2->SetFillColor(kRed);
	// h1->Draw("glbox");
	// TGLTH3Composition * comp = new TGLTH3Composition;
	// comp->AddTH3(h2); //, TGLTH3Composition::kSphere);
	// comp->Add(pl3dz);
	// comp->Draw("lego2");
	h2->Draw("");
	pl3dx->Draw("SAME");
	pl3dy->Draw("SAME");
	pl3dz->Draw("SAME");
	pm->Draw("SAME");
	//h3->Draw("source position");
	
	// canvas->cd(2);
	gPad->Modified();
	gPad->Update();
}
