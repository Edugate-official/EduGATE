{
	TChain * T = new TChain("Coincidences");

	TString RootFileName;
	cout << " please enter RootFileName: " << endl;	
	cin >> RootFileName;
	cout << "##### Reading from file: " << RootFileName << endl << endl;
	cout << endl << endl;
	
	T->Add(RootFileName);

	TString GraphicFileName = RootFileName.Remove(RootFileName.Length()-5) + ".gif";

//	T->Add("Rootfile.root");
//	T->Add("Rootfile_70_1sec_DeeX_eres_15.root");
//	T->Add("Rootfile_70_1sec_DeeX_eres_10.root");
//	T->Add("Rootfile_70_1sec_DeeX_eres_5.root");
//	T->Add("Rootfile_70_1sec_DeeX_eres_1.root");	
//	T->Add("Rootfile_70_1sec_no_DeeX_eres_1.root");	
	
	Float_t sourcePosX1;
	Float_t sourcePosY1;
	Float_t sourcePosZ1;
	Float_t sourcePosX2;
	Float_t sourcePosY2;
	Float_t sourcePosZ2;

	Float_t globalPosX1;
	Float_t globalPosY1;
	Float_t globalPosZ1;
	Float_t globalPosX2;
	Float_t globalPosY2;
	Float_t globalPosZ2;
			
	Float_t eventID1;
	Float_t eventID2;
	Float_t energy1;
	Float_t energy2;
	
	T->SetBranchAddress("sourcePosX1",&sourcePosX1);
	T->SetBranchAddress("sourcePosY1",&sourcePosY1);
	T->SetBranchAddress("sourcePosZ1",&sourcePosZ1);

	T->SetBranchAddress("globalPosX1",&globalPosX1);
	T->SetBranchAddress("globalPosY1",&globalPosY1);
	T->SetBranchAddress("globalPosZ1",&globalPosZ1);

	T->SetBranchAddress("sourcePosX2",&sourcePosX2);
	T->SetBranchAddress("sourcePosY2",&sourcePosY2);
	T->SetBranchAddress("sourcePosZ2",&sourcePosZ2);

	T->SetBranchAddress("globalPosX2",&globalPosX2);
	T->SetBranchAddress("globalPosY2",&globalPosY2);
	T->SetBranchAddress("globalPosZ2",&globalPosZ2);

	T->SetBranchAddress("eventID1",&eventID1);
	T->SetBranchAddress("eventID2",&eventID2);

	T->SetBranchAddress("energy1",&energy1);
	T->SetBranchAddress("energy2",&energy2);
	
	
	TH3F *h3_sourcePos1 = new TH3F("Source Pos1", "Source Pos1", 150, -220, 220, 150, -220, 220, 150, -220, 220);
	h3_sourcePos1->GetXaxis()->SetTitle(" x [mm]");
	h3_sourcePos1->GetXaxis()->CenterTitle();
	h3_sourcePos1->GetXaxis()->SetTitleOffset(2);
	h3_sourcePos1->GetYaxis()->SetTitle(" y [mm]");
	h3_sourcePos1->GetYaxis()->CenterTitle();
	h3_sourcePos1->GetYaxis()->SetTitleOffset(2);
	h3_sourcePos1->GetZaxis()->SetTitle(" z [mm]");
	h3_sourcePos1->GetZaxis()->CenterTitle();
	h3_sourcePos1->GetZaxis()->SetTitleOffset(1.2);

	TH3F *h3_sourcePos2 = new TH3F("Source Pos2", "Source Pos2", 150, -220, 220, 150, -220, 220, 150, -220, 220);
	h3_sourcePos2->GetXaxis()->SetTitle(" x [mm]");
	h3_sourcePos2->GetXaxis()->CenterTitle();
	h3_sourcePos2->GetXaxis()->SetTitleOffset(2);
	h3_sourcePos2->GetYaxis()->SetTitle(" y [mm]");
	h3_sourcePos2->GetYaxis()->CenterTitle();
	h3_sourcePos2->GetYaxis()->SetTitleOffset(2);
	h3_sourcePos2->GetZaxis()->SetTitle(" z [mm]");
	h3_sourcePos2->GetZaxis()->CenterTitle();
	h3_sourcePos2->GetZaxis()->SetTitleOffset(1.2);

	TH2F *h2_sourcePosx12 = new TH2F("Source Posx12", "Source Posx12", 150, -220, 220, 150, -220, 220);
	TH2F *h2_globalPosx12 = new TH2F("Global Posx12", "Global Posx12", 150, -220, 220, 150, -220, 220);
	
	TH1F *h1_e1 = new TH1F("Energy 1", "Energy 1", 80, 0, 800);
	TH1F *h1_e2 = new TH1F("Energy 2", "Energy 2", 80, 0, 800);
		
	TH1F *h1_random_e1 = new TH1F("Energy rndm_1", "Energy rndm_1", 80, 0, 800);
	TH1F *h1_random_e2 = new TH1F("Energy rndm_2", "Energy rndm_2", 80, 0, 800);

	int n_entries = T->GetEntries();
	int random = 0;
	
	for (int i=0;i<n_entries;i++)
	{
		T->GetEntry(i);
		h3_sourcePos1->Fill(sourcePosX1,sourcePosY1,sourcePosZ1);
		h3_sourcePos2->Fill(sourcePosX2,sourcePosY2,sourcePosZ2);
		h2_sourcePosx12->Fill(sourcePosX1,sourcePosX2);
		h2_globalPosx12->Fill(globalPosX1,globalPosX2);
		
		if (eventID1 != eventID2)
		{
			random++;
			h1_random_e1->Fill(energy1*1000);
			h1_random_e2->Fill(energy2*1000);
		}
		else
		{
			h1_e1->Fill(energy1*1000);
			h1_e2->Fill(energy2*1000);
		}
	}
	
	TCanvas *source = new TCanvas("gamma","gamma",1200,600);
	source->Divide(4,2);
	
	source->cd(1);
	h3_sourcePos1->Draw();
	
	source->cd(2);
	h3_sourcePos2->Draw();

	source->cd(3);
	h2_sourcePosx12->Draw();
	
	source->cd(4);
	h2_globalPosx12->Draw();
	
	source->cd(5);
	//source->cd(5)->SetLogy();
	h1_e1->Draw("BAR");
	h1_e1->SetFillStyle(0);
	
	source->cd(6);
	//source->cd(6)->SetLogy();		
	h1_random_e1->Draw("BAR");
	h1_random_e1->SetFillStyle(0);
	
	source->cd(7);
	//source->cd(7)->SetLogy();	
	h1_e2->Draw("BAR");
	h1_e2->SetFillStyle(0);

	source->cd(8);
	//source->cd(8)->SetLogy();	
	h1_random_e2->Draw("BAR");
	h1_random_e2->SetFillStyle(0);

	float ratio = float(random)/float(n_entries)*100;
	
	cout << " # entries: "<< n_entries << "\n" 
	<< " # randoms: "<< random << " \n" 
	<< " ratio: " << ratio << "%" << "\n";
	
	source->Update();    
	source->SaveAs(GraphicFileName);	
	
}
