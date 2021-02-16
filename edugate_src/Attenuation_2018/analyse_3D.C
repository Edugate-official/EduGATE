{
TChain * T = new TChain("Coincidences");

	TString RootFileName;
	cout << " please enter RootFileName: " << endl;	
	cin >> RootFileName;
	cout << "##### Reading from file: " << RootFileName << endl << endl;
	cout << endl << endl;
	
	T->Add(RootFileName);

	TString GraphicFileName = RootFileName.Remove(RootFileName.Length()-5) + ".gif";

	Float_t sourcePosX1;
	Float_t sourcePosY1;
	Float_t sourcePosZ1;
	T->SetBranchAddress("sourcePosX1",&sourcePosX1);
	T->SetBranchAddress("sourcePosY1",&sourcePosY1);
	T->SetBranchAddress("sourcePosZ1",&sourcePosZ1);

	int n = T->GetEntries();
	
	TH3F *h3 = new TH3F("source position", "source position", 50, -70, 70, 50, -70, 70, 50, -70, 70);
	h3->GetXaxis()->SetTitle(" x [mm]");
	h3->GetXaxis()->CenterTitle();
	h3->GetXaxis()->SetTitleOffset(2);
	h3->GetYaxis()->SetTitle(" y [mm]");
	h3->GetYaxis()->CenterTitle();
	h3->GetYaxis()->SetTitleOffset(2);
	h3->GetZaxis()->SetTitle(" z [mm]");
	h3->GetZaxis()->CenterTitle();
	h3->GetZaxis()->SetTitleOffset(1.2);
	
	for (int i=0;i<n;i++)
	{
		T->GetEntry(i);
		h3->Fill(sourcePosX1,sourcePosY1,sourcePosZ1);
	}
	
	TCanvas * source = new TCanvas("source","source",1);
	
	source->cd(1);
	
	h3->Draw("source position");
}
