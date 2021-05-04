#pragma once

#include "Blueprint Converter/BlueprintConverter.h"
#include "Lib/Functions/Functions.h"

namespace BlueprintConverter {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MainGUI : public System::Windows::Forms::Form
	{
	public:
		bool LoadedBP = false;
		bool LoadedDatabase = false;
		int SelItemIndex = -1;
		std::vector<SMBC::Blueprint>* Blueprints = nullptr;
		std::vector<SMBC::Blueprint>* TempBPTable = nullptr;
		MainGUI(void);
	protected:
		~MainGUI();
	private:
		System::Windows::Forms::Label^ Result_LBL;
		System::Windows::Forms::MenuStrip^ menuStrip1;
		System::Windows::Forms::ToolStripMenuItem^ aboutToolStripMenuItem;
		System::Windows::Forms::ToolStripMenuItem^ settingsToolStripMenuItem;
		System::Windows::Forms::ToolStripMenuItem^ reloadBlueprintListToolStripMenuItem;
		System::Windows::Forms::ToolStripMenuItem^ reloadObjectDatabaseToolStripMenuItem;
		System::Windows::Forms::ToolStripMenuItem^ settingsToolStripMenuItem1;
		System::Windows::Forms::TextBox^ SearchTB;
		System::Windows::Forms::Label^ label2;
		System::Windows::Forms::Label^ ActionLabel;
		System::Windows::Forms::ProgressBar^ ActionProgress;
		System::ComponentModel::BackgroundWorker^ DatabaseLoader;
		System::Windows::Forms::TextBox^ BPPath_TB;
		System::Windows::Forms::Button^ Start_BTN;
		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::Timer^ GuiUpdater;
		System::ComponentModel::BackgroundWorker^ ObjectGenerator;
		System::Windows::Forms::ListBox^ BlueprintList;
		System::Windows::Forms::PictureBox^ BlueprintImage;
		System::ComponentModel::BackgroundWorker^ BlueprintLoader;
		System::ComponentModel::IContainer^ components;
		System::Windows::Forms::Button^ OpenBlueprint;
		System::Windows::Forms::Button^ OpenOutputFolder_BTN;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->BPPath_TB = (gcnew System::Windows::Forms::TextBox());
			this->Start_BTN = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->GuiUpdater = (gcnew System::Windows::Forms::Timer(this->components));
			this->ObjectGenerator = (gcnew System::ComponentModel::BackgroundWorker());
			this->ActionLabel = (gcnew System::Windows::Forms::Label());
			this->ActionProgress = (gcnew System::Windows::Forms::ProgressBar());
			this->DatabaseLoader = (gcnew System::ComponentModel::BackgroundWorker());
			this->BlueprintList = (gcnew System::Windows::Forms::ListBox());
			this->BlueprintImage = (gcnew System::Windows::Forms::PictureBox());
			this->BlueprintLoader = (gcnew System::ComponentModel::BackgroundWorker());
			this->SearchTB = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->settingsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->reloadBlueprintListToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->reloadObjectDatabaseToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->settingsToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Result_LBL = (gcnew System::Windows::Forms::Label());
			this->OpenBlueprint = (gcnew System::Windows::Forms::Button());
			this->OpenOutputFolder_BTN = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->BlueprintImage))->BeginInit();
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// BPPath_TB
			// 
			this->BPPath_TB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->BPPath_TB->Enabled = false;
			this->BPPath_TB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->BPPath_TB->Location = System::Drawing::Point(12, 47);
			this->BPPath_TB->Name = L"BPPath_TB";
			this->BPPath_TB->Size = System::Drawing::Size(259, 22);
			this->BPPath_TB->TabIndex = 0;
			this->BPPath_TB->TextChanged += gcnew System::EventHandler(this, &MainGUI::BPPath_TB_TextChanged);
			// 
			// Start_BTN
			// 
			this->Start_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->Start_BTN->Enabled = false;
			this->Start_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Start_BTN->Location = System::Drawing::Point(347, 305);
			this->Start_BTN->Name = L"Start_BTN";
			this->Start_BTN->Size = System::Drawing::Size(350, 34);
			this->Start_BTN->TabIndex = 1;
			this->Start_BTN->Text = L"Convert";
			this->Start_BTN->UseVisualStyleBackColor = true;
			this->Start_BTN->Click += gcnew System::EventHandler(this, &MainGUI::Start_BTN_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(9, 27);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(107, 16);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Path to Blueprint:";
			// 
			// GuiUpdater
			// 
			this->GuiUpdater->Interval = 50;
			this->GuiUpdater->Tick += gcnew System::EventHandler(this, &MainGUI::GuiUpdater_Tick);
			// 
			// ObjectGenerator
			// 
			this->ObjectGenerator->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainGUI::ObjectGenerator_DoWork);
			this->ObjectGenerator->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MainGUI::ObjectGenerator_RunWorkerCompleted);
			// 
			// ActionLabel
			// 
			this->ActionLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ActionLabel->AutoSize = true;
			this->ActionLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ActionLabel->Location = System::Drawing::Point(344, 217);
			this->ActionLabel->Name = L"ActionLabel";
			this->ActionLabel->Size = System::Drawing::Size(66, 16);
			this->ActionLabel->TabIndex = 4;
			this->ActionLabel->Text = L"No Action";
			// 
			// ActionProgress
			// 
			this->ActionProgress->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ActionProgress->Location = System::Drawing::Point(347, 236);
			this->ActionProgress->Name = L"ActionProgress";
			this->ActionProgress->Size = System::Drawing::Size(350, 23);
			this->ActionProgress->TabIndex = 5;
			// 
			// DatabaseLoader
			// 
			this->DatabaseLoader->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainGUI::DatabaseLoader_DoWork);
			this->DatabaseLoader->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MainGUI::DatabaseLoader_RunWorkerCompleted);
			// 
			// BlueprintList
			// 
			this->BlueprintList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->BlueprintList->Enabled = false;
			this->BlueprintList->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->BlueprintList->FormattingEnabled = true;
			this->BlueprintList->HorizontalScrollbar = true;
			this->BlueprintList->IntegralHeight = false;
			this->BlueprintList->ItemHeight = 16;
			this->BlueprintList->Location = System::Drawing::Point(12, 119);
			this->BlueprintList->Name = L"BlueprintList";
			this->BlueprintList->Size = System::Drawing::Size(329, 220);
			this->BlueprintList->TabIndex = 6;
			this->BlueprintList->SelectedIndexChanged += gcnew System::EventHandler(this, &MainGUI::BlueprintList_SelectedIndexChanged);
			this->BlueprintList->EnabledChanged += gcnew System::EventHandler(this, &MainGUI::BlueprintList_EnabledChanged);
			// 
			// BlueprintImage
			// 
			this->BlueprintImage->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->BlueprintImage->Location = System::Drawing::Point(347, 27);
			this->BlueprintImage->Name = L"BlueprintImage";
			this->BlueprintImage->Size = System::Drawing::Size(350, 175);
			this->BlueprintImage->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->BlueprintImage->TabIndex = 7;
			this->BlueprintImage->TabStop = false;
			// 
			// BlueprintLoader
			// 
			this->BlueprintLoader->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainGUI::BlueprintLoader_DoWork);
			this->BlueprintLoader->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MainGUI::BlueprintLoader_RunWorkerCompleted);
			// 
			// SearchTB
			// 
			this->SearchTB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->SearchTB->Enabled = false;
			this->SearchTB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SearchTB->Location = System::Drawing::Point(12, 91);
			this->SearchTB->MaxLength = 50;
			this->SearchTB->Name = L"SearchTB";
			this->SearchTB->Size = System::Drawing::Size(329, 22);
			this->SearchTB->TabIndex = 8;
			this->SearchTB->TextChanged += gcnew System::EventHandler(this, &MainGUI::SearchTB_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(9, 72);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(54, 16);
			this->label2->TabIndex = 9;
			this->label2->Text = L"Search:";
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->aboutToolStripMenuItem,
					this->settingsToolStripMenuItem
			});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(709, 24);
			this->menuStrip1->TabIndex = 10;
			this->menuStrip1->Text = L"ToolStrip";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(52, 20);
			this->aboutToolStripMenuItem->Text = L"About";
			this->aboutToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainGUI::aboutToolStripMenuItem_Click);
			// 
			// settingsToolStripMenuItem
			// 
			this->settingsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->reloadBlueprintListToolStripMenuItem,
					this->reloadObjectDatabaseToolStripMenuItem, this->settingsToolStripMenuItem1
			});
			this->settingsToolStripMenuItem->Name = L"settingsToolStripMenuItem";
			this->settingsToolStripMenuItem->Size = System::Drawing::Size(61, 20);
			this->settingsToolStripMenuItem->Text = L"Settings";
			// 
			// reloadBlueprintListToolStripMenuItem
			// 
			this->reloadBlueprintListToolStripMenuItem->Name = L"reloadBlueprintListToolStripMenuItem";
			this->reloadBlueprintListToolStripMenuItem->Size = System::Drawing::Size(199, 22);
			this->reloadBlueprintListToolStripMenuItem->Text = L"Reload Blueprint List";
			this->reloadBlueprintListToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainGUI::reloadBlueprintListToolStripMenuItem_Click);
			// 
			// reloadObjectDatabaseToolStripMenuItem
			// 
			this->reloadObjectDatabaseToolStripMenuItem->Name = L"reloadObjectDatabaseToolStripMenuItem";
			this->reloadObjectDatabaseToolStripMenuItem->Size = System::Drawing::Size(199, 22);
			this->reloadObjectDatabaseToolStripMenuItem->Text = L"Reload Object Database";
			this->reloadObjectDatabaseToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainGUI::reloadObjectDatabaseToolStripMenuItem_Click);
			// 
			// settingsToolStripMenuItem1
			// 
			this->settingsToolStripMenuItem1->Name = L"settingsToolStripMenuItem1";
			this->settingsToolStripMenuItem1->Size = System::Drawing::Size(199, 22);
			this->settingsToolStripMenuItem1->Text = L"Settings";
			this->settingsToolStripMenuItem1->Click += gcnew System::EventHandler(this, &MainGUI::settingsToolStripMenuItem1_Click);
			// 
			// Result_LBL
			// 
			this->Result_LBL->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->Result_LBL->AutoSize = true;
			this->Result_LBL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Result_LBL->Location = System::Drawing::Point(229, 72);
			this->Result_LBL->Name = L"Result_LBL";
			this->Result_LBL->Size = System::Drawing::Size(66, 16);
			this->Result_LBL->TabIndex = 11;
			this->Result_LBL->Text = L"Results: 0";
			this->Result_LBL->Visible = false;
			// 
			// OpenBlueprint
			// 
			this->OpenBlueprint->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->OpenBlueprint->Enabled = false;
			this->OpenBlueprint->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->OpenBlueprint->Location = System::Drawing::Point(277, 47);
			this->OpenBlueprint->Name = L"OpenBlueprint";
			this->OpenBlueprint->Size = System::Drawing::Size(64, 22);
			this->OpenBlueprint->TabIndex = 12;
			this->OpenBlueprint->Text = L"Open";
			this->OpenBlueprint->UseVisualStyleBackColor = true;
			this->OpenBlueprint->Click += gcnew System::EventHandler(this, &MainGUI::OpenBlueprint_Click);
			// 
			// OpenOutputFolder_BTN
			// 
			this->OpenOutputFolder_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->OpenOutputFolder_BTN->Enabled = false;
			this->OpenOutputFolder_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->OpenOutputFolder_BTN->Location = System::Drawing::Point(347, 265);
			this->OpenOutputFolder_BTN->Name = L"OpenOutputFolder_BTN";
			this->OpenOutputFolder_BTN->Size = System::Drawing::Size(350, 34);
			this->OpenOutputFolder_BTN->TabIndex = 13;
			this->OpenOutputFolder_BTN->Text = L"Open Model Output Directory";
			this->OpenOutputFolder_BTN->UseVisualStyleBackColor = true;
			this->OpenOutputFolder_BTN->Click += gcnew System::EventHandler(this, &MainGUI::OpenOutputFolder_BTN_Click);
			// 
			// MainGUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(709, 351);
			this->Controls->Add(this->OpenOutputFolder_BTN);
			this->Controls->Add(this->OpenBlueprint);
			this->Controls->Add(this->Result_LBL);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->SearchTB);
			this->Controls->Add(this->BlueprintImage);
			this->Controls->Add(this->BlueprintList);
			this->Controls->Add(this->ActionProgress);
			this->Controls->Add(this->ActionLabel);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->Start_BTN);
			this->Controls->Add(this->BPPath_TB);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->MinimumSize = System::Drawing::Size(725, 390);
			this->Name = L"MainGUI";
			this->ShowIcon = false;
			this->Text = L"SM Blueprint to Model Converter";
			this->Shown += gcnew System::EventHandler(this, &MainGUI::MainGUI_Shown);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->BlueprintImage))->EndInit();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		System::Void BPPath_TB_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void Start_BTN_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ObjectGenerator_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);
		System::Void GuiUpdater_Tick(System::Object^ sender, System::EventArgs^ e);
		System::Void ObjectGenerator_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);
		System::Void DatabaseLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);
		System::Void DatabaseLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);
		System::Void BlueprintLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);
		System::Void BlueprintLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);
		System::Void BlueprintList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void SearchTB_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void BlueprintList_EnabledChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void aboutToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void LoadDatabase();
		System::Void LoadBlueprints();
		System::Void reloadBlueprintListToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void reloadObjectDatabaseToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ChangeGUIState(bool bploaded, bool databaseloaded, bool bpgenerated);
		System::Void settingsToolStripMenuItem1_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void OpenOptionsGUI();
		System::Void MainGUI_Shown(System::Object^ sender, System::EventArgs^ e);
		System::Void OpenBlueprint_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void OpenOutputFolder_BTN_Click(System::Object^ sender, System::EventArgs^ e);
	};
}