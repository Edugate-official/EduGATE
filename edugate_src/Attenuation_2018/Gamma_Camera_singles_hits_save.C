{
	gROOT->Reset();
	
	TString BaseFileName = "Gamma_Camera";
	
//	Float_t			Emax = 1.000;
	
	TString ext = ".root";
	TString gr_ext = ".gif";
	TString RootFileName = BaseFileName + ext;
	TString GraphicFileName = BaseFileName + gr_ext;
	
	TFile f(RootFileName);
	
	TH1 *total_nb_primaries = (TH1*)gDirectory->Get("total_nb_primaries");
	TTree *Singles = (TTree*)gDirectory->Get("Singles");
	TTree *Gate = (TTree*)gDirectory->Get("Gate");
	TTree *Hits = (TTree*)gDirectory->Get("Hits");	
	
	Int_t           comptonPhantom;
	Int_t           comptonCrystal;
	Int_t           RayleighCrystal;
	Int_t           RayleighPhantom;
	Float_t         energy;
	Char_t          comptVolName[40];
	Float_t			scatter_phantom, scatter_null, scatter_compartment, scatter_table;
	Float_t			scatter_collim, scatter_shielding, primary_event, scatter_crystal;
	Float_t			order1, order2, order3, order3, order4, ordersup, ordertot;
	
	
	Float_t	        globalPosX, globalPosY, globalPosZ, sourcePosX, sourcePosY, sourcePosZ;
	Double_t        time;	
	
	Int_t			runID, runID_hits;
	Int_t			eventID, eventID_hits;
	Char_t          processName[40];
	Int_t			PDGEncoding;	
			
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
	Singles->SetBranchAddress("time",&time);
	
	Singles->SetBranchAddress("runID", &runID);
	Singles->SetBranchAddress("eventID", &eventID);
	
	Hits->SetBranchAddress("processName",processName);
	Hits->SetBranchAddress("PDGEncoding",&PDGEncoding);
	Hits->SetBranchAddress("runID", &runID_hits);
	Hits->SetBranchAddress("eventID", &eventID_hits);	
	
	//*******************************************************
	Singles->Draw("energy>>hi_energy","","goff");
	TH1F   *hi_energy = (TH1F*)gDirectory->Get("hi_energy");
	Int_t   nbins     = hi_energy->GetNbinsX();
	Float_t binWidth  = hi_energy->GetBinWidth(1);
	Float_t	Emax      = nbins*binWidth;
	//*******************************************************

	TH1F *primary = new TH1F("primary","",100,0,Emax);
	TH1F *sc_ph   = new TH1F("sc_ph","",100,0,Emax);
	TH1F *sc_cry  = new TH1F("sc_mo","",100,0,Emax);
	TH1F *sc_com  = new TH1F("sc_com","",100,0,Emax);
	TH1F *sc_ta   = new TH1F("sc_ta","",100,0,Emax);
	TH1F *sc_col  = new TH1F("sc_col","",100,0,Emax);
	TH1F *ener    = new TH1F("ener","",100,0,Emax);
	
	TH1F *proc_code = new TH1F("Process_Code","",128,0.,128.);	
	
	TH1F *o1   = new TH1F("o1","",100,0,Emax);
	TH1F *o2   = new TH1F("o2","",100,0,Emax);
	TH1F *o3   = new TH1F("o3","",100,0,Emax);
	TH1F *o4   = new TH1F("o4","",100,0,Emax);
	TH1F *osup = new TH1F("osup","",100,0,Emax);
	
	TH2F *globalPosZ_Y = new TH2F("globalPosZ","globalPosY ",600,-150.0,+150.0.,400,-100.0,+100.0);
	TH2F *globalPosZ_Y_Ewin = new TH2F("globalPosZ_Ewin","globalPosY_Ewin",600,-150.0,+150.0.,400,-100.0,+100.0);
	
//	Float_t CE_value = 0.364;
//	Float_t	LETH = CE_value - 0.1*CE_value;
//	Float_t UETH = CE_value + 0.1*CE_value;
	
	Float_t CE_value = Emax;
	Float_t	LETH = 0.0;
	Float_t UETH = CE_value + 0.1*CE_value;

	cout << endl;
	cout << "##### Reading from file: " << RootFileName << endl;
	cout << endl;

	Int_t nentries = Singles->GetEntries();
	Int_t nentries_Hits = Hits->GetEntries();
	Int_t nbytes = 0;
	Int_t nbytes_Hits = 0;
	
	Int_t MultScat = 0;
	Int_t ElecIon = 0;
	Int_t Trans = 0;	
	Int_t Brems = 0;
	Int_t Photo = 0;
	Int_t Compt = 0;
	Int_t RaylScat = 0;
	Int_t SumProc = 0;
	Int_t PDG_11 = 0; // electron/positron
	Int_t PDG_22 = 0; // gamma
	
	Int_t ih_last = 0;
	
	//cout<<"##### nb of entries (singles): " << nentries <<endl;
	//cout<<"##### nb of nentries_Hits (hits): " << nentries_Hits <<endl;
	
	cout << " Emax: " << Emax << endl;
	cout << endl;
		
	for (Int_t i=0; i<nentries;i++) {
	//for (Int_t i=0; i<100;i++) {	
		nbytes += Singles->GetEntry(i);
		
		globalPosZ_Y->Fill(globalPosZ,globalPosY);
		
		if (energy >= LETH && energy <= UETH) {
			globalPosZ_Y_Ewin->Fill(globalPosZ,globalPosY);
		}
				
		ener.Fill(energy);
		
		if (comptonPhantom == 0 && comptonCrystal == 0) { 
			primary.Fill(energy);
			primary_event++;
			//globalPosZ_Y_Ewin->Fill(globalPosZ,globalPosY);
			} // primary
		
		if (comptonCrystal != 0 && comptonPhantom == 0) {
			scatter_crystal++;
			sc_cry.Fill(energy);
			} // scatter in crystal
		
		//cout<<" -+-+-+- "<< runID <<" "<< eventID <<" ih_last "<< ih_last <<endl;	
		
		Int_t ihits=0;
		Int_t ProcFlag = 0;
		
		for (Int_t ih=ih_last; ih<nentries_Hits;ih++) {
			
			nbytes_Hits += Hits->GetEntry(ih);
			
			if (runID == runID_hits ) {
				
			//cout<< runID <<" "<< runID_hits<< " "<< eventID <<" "<< eventID_hits <<" "<< ih_last <<" "<< ih <<" "<< ihits <<endl;
						
			if (eventID < eventID_hits && ihits > 0) break;
			
			if (eventID == eventID_hits) {
				ih_last = ih;
				//cout<< runID_hits <<" "<< eventID_hits <<" "<< ih <<" "<< ihits <<" "<<PDGEncoding <<" "<< energy <<" "<< processName <<endl;
				
				if (strcmp(processName,"MultipleScattering")==  NULL) {
					MultScat++;
					ProcFlag = 1;
				}
				if (strcmp(processName,"ElectronIonisation")==  NULL) {
					ElecIon++;
					ProcFlag = ProcFlag + 2;
				}
				if (strcmp(processName,"Transportation")==  NULL) {
					Trans++;
					ProcFlag = ProcFlag + 4;					
				}
				if (strcmp(processName,"Bremsstrahlung")==  NULL) {
					Brems++;
					ProcFlag = ProcFlag + 8;					
				}
				if (strcmp(processName,"PhotoElectric")==  NULL) {
					Photo++;
					ProcFlag = ProcFlag + 16;					
				}
				if (strcmp(processName,"Compton")==  NULL) {
					Compt++;
					ProcFlag = ProcFlag + 32;
				}
				if (strcmp(processName,"RayleighScattering")==  NULL) {
					RaylScat++;
					ProcFlag = ProcFlag + 64;					
				}				
				SumProc = MultScat + ElecIon + Trans + Brems + Photo + Compt + RaylScat;
				
				if (PDGEncoding == 11) {
					PDG_11++;
				}
				if (PDGEncoding == 22) {
					PDG_22++;
				}
				if (PDGEncoding != 11 && PDGEncoding != 22) {
					cout<<" 2 i: "<< i <<" "<< runID <<" "<< eventID <<" ih: "<< ih <<" "<< ihits <<" "<< PDGEncoding <<endl;
				}
				if (ProcFlag == 0) {
					cout<<" 3 i: "<< i <<" "<< runID <<" "<< eventID <<" ih: "<< ih <<" "<< ihits <<" "<< processName <<endl;
				}
				
				if (PDGEncoding == 11 && ihits ==1) { // require emitted particle to be an electron
				//if (PDGEncoding == 11) { // require emitted particle to be an electron
					//cout<<" 3 i: " << i << " " << runID << " " << eventID << " ih: " << ih << " " << ihits << " " <<PDGEncoding << " " << energy <<endl;
					//ener.Fill(energy);
				}
				proc_code->Fill(ProcFlag);
				
				ihits = ihits+1;
				} //run & event ID
			} //run ID

		} //nentries_hits
		//ih_last = 0;

		if (strcmp(comptVolName,"Phantom_phys")==  NULL||strcmp(comptVolName,"source_vol_phys") == NULL) {
			scatter_phantom++;
			sc_ph->Fill(energy);
		}	// scatter in phantom
		
		
		if (strcmp(comptVolName,"compartment_phys") == NULL) {
			scatter_compartment++;
			sc_com->Fill(energy);
		}
		
		if (strcmp(comptVolName,"table_phys") == NULL) {			
			scatter_table++;
			sc_ta->Fill(energy);
		}
		
		if (strcmp(comptVolName,"collimator_phys") == NULL) {
			scatter_collim++;
			sc_col->Fill(energy);
		}
		
		if(comptonPhantom + comptonCrystal == 1) {
			o1.Fill(energy);
			order1++;
		}
		
		if(comptonPhantom + comptonCrystal == 2) {
			o2.Fill(energy);
			order2++;
		}
		
		if(comptonPhantom + comptonCrystal == 3) {
			o3.Fill(energy);
			order3++;
		}
		
		if(comptonPhantom + comptonCrystal == 4) {
			o4.Fill(energy);
			order4++;
		}
		
		if(comptonPhantom + comptonCrystal > 4) {
			osup.Fill(energy);
			ordersup++;
		}
		
		if(comptonPhantom != 0 || comptonCrystal != 0) {
			ordertot++;
		}
		
	} // nentries
	
	
	// **************************************** Plots **********************************************
	
	gStyle->SetPalette(1);
	gStyle->SetOptStat(0);
	
	// First Canvas
	
	TCanvas C1("Gamma_Camera","Gamma_Camera",100,100,800,600);
	C1.Divide(2,3);
	
	C1.SetFillColor(0);
	C1.cd(1);
	
	ener->SetFillColor(2);
	ener->SetFillStyle(3023);
	ener->GetXaxis()->SetTitle("MeV");
	ener.Draw();
	TLatex *   tex = new TLatex(0.0169022,11741.43,"Total Sspectrum of the detected events");
	tex->SetLineWidth(2);
	tex->Draw();
	
	C1.SetFillColor(0);
	C1.cd(2);
	
	primary.Draw();
	primary->SetLineColor(6);
	primary->GetXaxis()->SetTitle("MeV");
	
	sc_ph.Draw("same");
	sc_ph ->SetLineColor(1);
	
	sc_ta.Draw("same");
	sc_ta ->SetLineColor(2);
	
	sc_col.Draw("same");
	sc_col ->SetLineColor(3);
	
	sc_com.Draw("same");
	sc_com ->SetLineColor(5);
	
	sc_cry.Draw("same");
	sc_cry ->SetLineColor(4);
	
	TLegend *leg1 = new TLegend(0.2,0.6,0.6,0.85);
	leg1->SetFillColor(0);
	leg1->SetTextSize(0.03);
	leg1->AddEntry(primary,"primary spectrum                 ","l");
	leg1->AddEntry(sc_ph,  "scatter in the phantom           ","l");
	leg1->AddEntry(sc_ta,  "scatter in the table             ","l");
	leg1->AddEntry(sc_col, "scatter in the collimator        ","l");
	leg1->AddEntry(sc_com, "scatter in the backcompartment   ","l");
	leg1->AddEntry(sc_cry, "scatter in the crystal           ","l");
	leg1->Draw();
	 
	tex = new TLatex(-0.0040358,10141.43,"Primary and scatter spectra of the detected events");
	tex->SetLineWidth(2);
	tex->Draw();
		
	C1.SetFillColor(0);
	C1.cd(3);
	
	o1.Draw();
	o1->GetXaxis()->SetTitle("MeV");
	o1 ->SetLineColor(1);
	o2.Draw("same");
	o2 ->SetLineColor(2);
	o3.Draw("same");
	o3 ->SetLineColor(3);
	o4.Draw("same");
	o4 ->SetLineColor(4);
	osup.Draw("same");
	osup ->SetLineColor(5);
	
	TLegend *leg2 = new TLegend(0.2,0.6,0.5,0.85);
	leg2->SetFillColor(0);
	leg2->SetTextSize(0.03);
	leg2->AddEntry(o1,"1st order scatter","l");
	leg2->AddEntry(o2,"2st order scatter","l");
	leg2->AddEntry(o3,"3st order scatter","l");
	leg2->AddEntry(o4,"4st order scatter","l");
	leg2->AddEntry(osup,">4st order scatter","l");
	leg2->Draw();
	
	//	tex = new TLatex(0.022791,792.626,"Scatter spectra of the detected  events");
	tex = new TLatex(0.022791,1482.626,"Scatter spectra of the detected  events");
	
	tex->SetLineWidth(2);
	tex->Draw();
	
	C1.SetFillColor(0);
	C1.cd(4);
	
	globalPosZ_Y->GetXaxis()->SetTitle("globalPosZ");
	globalPosZ_Y->GetYaxis()->SetTitle("globalPosY");
	globalPosZ_Y->Draw();
	
	C1.SetFillColor(0);
	C1.cd(5);
	
	globalPosZ_Y_Ewin->GetXaxis()->SetTitle("globalPosZ");
	globalPosZ_Y_Ewin->GetYaxis()->SetTitle("globalPosY");
	globalPosZ_Y_Ewin->Draw();	
	
	C1.SetFillColor(0);
	C1.cd(6);
	proc_code-> Draw();
	//proc_code->GetXaxis()->SetTitle("ProcFlag");

	
	
	// save Plot to disk 
	
	C1->Update();    
//	C1->SaveAs(GraphicFileName);
	
	
	//	Second Canvas
	/*
	TCanvas C2("PlaneView","PlaneView",100,100,800,600);
	C2.Divide(1,1);
	
	C2.SetFillColor(0);
	C2.cd(1);

	globalPosZ_Y.Draw();
	C2->Update();
	*/
	
	// **************************************** Summary Printout **********************************************
	
	cout << " *********************************************************************************** " << endl;
	cout << " *                                                                                 * " << endl;
	cout << " *   E d u G a t e  S i m u l a t i o n   A n a l y s i s                          * " << endl;
	cout << " *   G A M M A  C A M E R A                                                        * " << endl;
	cout << " *                                                                                 * " << endl;
	cout << " *********************************************************************************** " << endl;
	cout << endl;
	cout<<"##### Number of emitted particles    :  "	<<total_nb_primaries->GetMean()<<endl;
	cout<<"##### Number of detected events      :  "	<<nentries<<endl;
	cout<<"##### Primary events                 :  "	<<primary_event/nentries*100<<"  %"<<endl;
	cout<<"##### Scatter in the phantom         :  "	<<scatter_phantom/nentries*100<<" %"<<endl;
	cout<<"##### Scatter in the table           :  "	<<scatter_table/nentries*100<<" %"<<endl;
	cout<<"##### Scatter in the collimator      :  "	<<scatter_collim/nentries*100<<" %"<<endl;
	cout<<"##### Scatter in the crystal         :  "	<<scatter_crystal/nentries*100<<" %"<<endl;
	cout<<"##### Scatter in the backcompartment :  "	<<scatter_compartment/nentries*100<<" %"<<endl;
	cout << endl;
	cout<<"----- Number of Hits                 :  "	<<nentries_Hits <<endl;
	cout<<"----- Multiple Scattering            :  "	<<MultScat <<endl;
	cout<<"----- Electron Ionisation            :  "	<<ElecIon <<endl;
	cout<<"----- Transportation                 :  "	<<Trans <<endl;	
	cout<<"----- Bremstrahlung                  :  "	<<Brems <<endl;	
	cout<<"----- Photoelectric                  :  "	<<Photo <<endl;	
	cout<<"----- Compton                        :  "	<<Compt <<endl;
	cout<<"----- Rayleigh Scattering            :  "	<<RaylScat <<endl;	
	cout<<"----- Sum                            :  "	<<SumProc <<endl;	
	cout<<"----- positrons/electrons (pdg=11)   :  "	<<PDG_11 <<endl;
	cout<<"----- gamma               (pdg=22)   :  "	<<PDG_22 <<endl;		
	cout<<endl;
	
	
	if (ordertot > 0) {
		cout<<"##### Scatter order  1 : "<<order1/ordertot*100<<" %"<<endl;
		cout<<"##### Scatter order  2 : "<<order2/ordertot*100<<" %"<<endl;
		cout<<"##### Scatter order  3 : "<<order3/ordertot*100<<" %"<<endl;
		cout<<"##### Scatter order  4 : "<<order4/ordertot*100<<" %"<<endl;
		cout<<"##### Scatter order >4 : "<<ordersup/ordertot*100<<" %"<<endl;
	}
	cout << endl;
}
