
// GenerateGateConfiguration_MR_PET.C
// This macro allows the configuration of a GATE simulation.
// The user is allowed to configure various simulation parameters 
// by choosing between predefined options set in MR_PET.txt.
// The result is written into the GATE macro file "configuration.mac",
// which is called by the main GATE macro.



#include <TApplication.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGListBox.h>
#include <TList.h>
#include <TGComboBox.h>
class GateConfiguration : public TGMainFrame {

private:
	TGComboBox          **fCombo;
	TGCheckButton       *fCheckMulti;
	TList               *fSelected;   
	TGHorizontalFrame 	**hframes;
	TGLabel **labels;
	int numCombo;
	const char* config;
	const char* value;

public:
	GateConfiguration(const TGWindow *p, UInt_t w, UInt_t h, const char* conf);
	virtual ~GateConfiguration();
	void DoExit();
	void GenerateConfigFile();
	void Save();


	ClassDef(GateConfiguration, 0)
};

void GateConfiguration::GenerateConfigFile()
{	
	
	ofstream fout("configuration.mac");
	bool gamma=true;
	for (int i=0; i<numCombo; i++) {
		
		if (fCombo[i]->GetSelected()!=-1) {
			TGLBEntry *entry=fCombo[i]->GetSelectedEntry();
			value =((TGTextLBEntry*)entry)->GetText()->GetString();	
			cout<<"/control/alias "<<labels[i]->GetText()->GetString()<<" "<<value<<endl;
			fout<<"/control/alias "<<labels[i]->GetText()->GetString()<<" "<<value<<"\n";

        }
		
    }
	
		// cout<<"/control/alias RootFileName MR_PET_{Camera_Type}_{CrystalMaterial}_{SourceEnergy}_{particle}_{SourceActivity}_{Yes_No_annihil}_{Source_Type}_{B0}"<<endl;
		// fout<<"/control/alias RootFileName MR_PET_{Camera_Type}_{CrystalMaterial}_{SourceEnergy}_{particle}_{SourceActivity}_{Yes_No_annihil}_{Source_Type}_{B0}"<<"\n";
		
		cout<<"/control/alias RootFileName MR_PET_{Camera_Type}_{Medium}_{SourceActivity}_{Source_Type}_{B0_x}_{B0_y}_{B0_z}_{Annihilation}"<<endl;
		fout<<"/control/alias RootFileName MR_PET_{Camera_Type}_{Medium}_{SourceActivity}_{Source_Type}_{B0_x}_{B0_y}_{B0_z}_{Annihilation}"<<"\n";

					
	fout.close();

}


void GateConfiguration::Save()
{	
	ofstream fout(config);
	
	for (int i=0; i<numCombo; i++) {
		
		fout<<labels[i]->GetText()->GetString()<<":";
		
		TGLBEntry *entry=fCombo[i]->GetSelectedEntry();
		value =((TGTextLBEntry*)entry)->GetText()->GetString();
		fout<<value<<";";
		int select=fCombo[i]->GetSelected();
		for( int j=1; j<=fCombo[i]->GetNumberOfEntries();j++){
			
			if(j!=select){
				fCombo[i]->Select(j);
				entry=fCombo[i]->GetSelectedEntry();
				value =((TGTextLBEntry*)entry)->GetText()->GetString();
				fout<<value<<";";
			}
		}
		fout<<endl;
		fCombo[i]->Select(select);
	}
	fout.close();
}

void GateConfiguration::DoExit()
{
   GenerateConfigFile();
   Save();
   Printf("End of configuration. Passing parameters to GATE.");
   gApplication->Terminate(0);
}

GateConfiguration::GateConfiguration(const TGWindow *p, UInt_t w, UInt_t h, const char* conf) :
   TGMainFrame(p, w, h)
{
	
	config=conf;
	numCombo=0;
	
	string temp;
	ifstream fin(config);

    while(getline(fin,temp) )
    {
		if (!temp.empty()) {
			numCombo++;
		}
    }
	if (temp!="") {
		numCombo++;
	}
	
	fin.close();

	hframes= new TGHorizontalFrame*[numCombo];
	fCombo=new TGComboBox*[numCombo];	
	labels=new TGLabel*[numCombo];
	
	fin.open(config);
	
	for (int i=0; i<numCombo; i++) {

		hframes[i] = new TGHorizontalFrame(this, 350, 20, kLHintsExpandX);	
		fCombo[i] = new TGComboBox(hframes[i], 100);


		int numParameters=0;
		int found=0;
		
		getline(fin,temp);
	
		while(found!=-1){
			found=temp.find(";",found+1);
			numParameters++;
		};
		numParameters--;

		found=temp.find(":");
		char al[120];
		temp.copy(al,found,0);
		al[found]='\0';
		labels[i]=new TGLabel(hframes[i],new TGString(al));	

		for (int j=0; j<numParameters; j++) {
			
			int start=found+1;
			found=temp.find(";",start);
			temp.copy(al,found-start,start);
			al[found-start]='\0';
			
			fCombo[i]->AddEntry(al, j+1);
			}
		
		if(strcmp(labels[i]->GetText()->GetString(),"SourceEnergy") &&
		   strcmp(labels[i]->GetText()->GetString(),"Ion_Z_val") &&
		   strcmp(labels[i]->GetText()->GetString(),"Ion_A_val") )
			
		fCombo[i]->Select(1);
//		fCombo[i]->EnableTextInput(true);
		fCombo[i]->EnableTextInput(false);		
		fCombo[i]->Resize(300, 20);	
		hframes[i]->AddFrame(labels[i],new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));
		hframes[i]->AddFrame(fCombo[i], new TGLayoutHints(kLHintsExpandX , 5, 5, 5, 5));
		AddFrame(hframes[i], new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));		
	}
	
// Create a horizontal frame containing button(s)
	TGHorizontalFrame *hframe = new TGHorizontalFrame(this, 350, 20, kFixedWidth);
	TGTextButton *generate = new TGTextButton(hframe, "&Generate configuration.mac and Start");
	generate->Connect("Pressed()", "GateConfiguration", this, "DoExit()");
	hframe->AddFrame(generate, new TGLayoutHints(kFixedWidth | kLHintsCenterX, 5, 5, 3, 4));
	

	AddFrame(hframe, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));

	// Set a name to the main frame   
	SetWindowName("EduGate MR_PET Configuration");
	MapSubwindows();

	// Initialize the layout algorithm via Resize()
	Resize(GetDefaultSize());

	// Map main frame
	MapWindow();
 	//  fListBox->Select(1);
}

GateConfiguration::~GateConfiguration()
{
   // Clean up main frame...
   Cleanup();	
}



void GenerateGateConfiguration_MR_PET(const char* conf)
{
   // Popup the GUI...
   GateConfiguration *Spectrometry=new GateConfiguration(gClient->GetRoot(), 400, 200, conf);

}
