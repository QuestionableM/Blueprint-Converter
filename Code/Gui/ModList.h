#pragma once

#include "Object Database/ObjectDatabase.h"
#include "Lib/Functions/Functions.h"

namespace BlueprintConverter {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	struct ModListData {
		int mod_index = -1;
		long long used_parts = 0;
	};

	public ref class ModList : public System::Windows::Forms::Form
	{
	public:
		int selected_mod = -1;
		ModList(const SMBC::Blueprint& blueprint);
		std::vector<ModListData>* usedModList = nullptr;

	protected:
		~ModList();

		System::Windows::Forms::Label^ ModCount_LBL;
		System::Windows::Forms::Label^ BPName_LBL;
		System::Windows::Forms::ProgressBar^ ModSearchProgress;
		System::Windows::Forms::ListBox^ ModList_LB;
		System::Windows::Forms::Label^ label3;
		System::Windows::Forms::Button^ OpenInWorkshop_BTN;
		System::Windows::Forms::Button^ OpenInFileExplorer_BTN;
		System::Windows::Forms::Label^ ObjectCount_LBL;
		System::ComponentModel::BackgroundWorker^ ModSearcher_BW;
		System::Windows::Forms::Timer^ GuiUpdater;
	private: System::ComponentModel::IContainer^ components;
	protected:



#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->ModCount_LBL = (gcnew System::Windows::Forms::Label());
			this->BPName_LBL = (gcnew System::Windows::Forms::Label());
			this->ModSearchProgress = (gcnew System::Windows::Forms::ProgressBar());
			this->ModList_LB = (gcnew System::Windows::Forms::ListBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->OpenInWorkshop_BTN = (gcnew System::Windows::Forms::Button());
			this->OpenInFileExplorer_BTN = (gcnew System::Windows::Forms::Button());
			this->ModSearcher_BW = (gcnew System::ComponentModel::BackgroundWorker());
			this->GuiUpdater = (gcnew System::Windows::Forms::Timer(this->components));
			this->ObjectCount_LBL = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// ModCount_LBL
			// 
			this->ModCount_LBL->AutoSize = true;
			this->ModCount_LBL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ModCount_LBL->Location = System::Drawing::Point(9, 9);
			this->ModCount_LBL->Name = L"ModCount_LBL";
			this->ModCount_LBL->Size = System::Drawing::Size(117, 16);
			this->ModCount_LBL->TabIndex = 0;
			this->ModCount_LBL->Text = L"Amount of Mods: 0";
			// 
			// BPName_LBL
			// 
			this->BPName_LBL->AutoSize = true;
			this->BPName_LBL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->BPName_LBL->Location = System::Drawing::Point(9, 25);
			this->BPName_LBL->Name = L"BPName_LBL";
			this->BPName_LBL->Size = System::Drawing::Size(172, 16);
			this->BPName_LBL->TabIndex = 1;
			this->BPName_LBL->Text = L"Blueprint Name: NO_NAME";
			// 
			// ModSearchProgress
			// 
			this->ModSearchProgress->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ModSearchProgress->Location = System::Drawing::Point(12, 296);
			this->ModSearchProgress->Name = L"ModSearchProgress";
			this->ModSearchProgress->Size = System::Drawing::Size(365, 23);
			this->ModSearchProgress->TabIndex = 2;
			// 
			// ModList_LB
			// 
			this->ModList_LB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ModList_LB->Enabled = false;
			this->ModList_LB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ModList_LB->FormattingEnabled = true;
			this->ModList_LB->HorizontalScrollbar = true;
			this->ModList_LB->IntegralHeight = false;
			this->ModList_LB->ItemHeight = 16;
			this->ModList_LB->Location = System::Drawing::Point(12, 87);
			this->ModList_LB->Name = L"ModList_LB";
			this->ModList_LB->Size = System::Drawing::Size(365, 173);
			this->ModList_LB->TabIndex = 3;
			this->ModList_LB->SelectedIndexChanged += gcnew System::EventHandler(this, &ModList::ModList_LB_SelectedIndexChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(9, 68);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(81, 16);
			this->label3->TabIndex = 4;
			this->label3->Text = L"Used Mods:";
			// 
			// OpenInWorkshop_BTN
			// 
			this->OpenInWorkshop_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->OpenInWorkshop_BTN->Enabled = false;
			this->OpenInWorkshop_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->OpenInWorkshop_BTN->Location = System::Drawing::Point(12, 266);
			this->OpenInWorkshop_BTN->Name = L"OpenInWorkshop_BTN";
			this->OpenInWorkshop_BTN->Size = System::Drawing::Size(150, 24);
			this->OpenInWorkshop_BTN->TabIndex = 5;
			this->OpenInWorkshop_BTN->Text = L"Open In Workshop";
			this->OpenInWorkshop_BTN->UseVisualStyleBackColor = true;
			this->OpenInWorkshop_BTN->Click += gcnew System::EventHandler(this, &ModList::OpenInWorkshop_BTN_Click);
			// 
			// OpenInFileExplorer_BTN
			// 
			this->OpenInFileExplorer_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->OpenInFileExplorer_BTN->Enabled = false;
			this->OpenInFileExplorer_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->OpenInFileExplorer_BTN->Location = System::Drawing::Point(227, 266);
			this->OpenInFileExplorer_BTN->Name = L"OpenInFileExplorer_BTN";
			this->OpenInFileExplorer_BTN->Size = System::Drawing::Size(150, 24);
			this->OpenInFileExplorer_BTN->TabIndex = 6;
			this->OpenInFileExplorer_BTN->Text = L"Open In File Explorer";
			this->OpenInFileExplorer_BTN->UseVisualStyleBackColor = true;
			this->OpenInFileExplorer_BTN->Click += gcnew System::EventHandler(this, &ModList::OpenInFileExplorer_BTN_Click);
			// 
			// ModSearcher_BW
			// 
			this->ModSearcher_BW->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &ModList::ModSearcher_BW_DoWork);
			this->ModSearcher_BW->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &ModList::ModSearcher_BW_RunWorkerCompleted);
			// 
			// GuiUpdater
			// 
			this->GuiUpdater->Tick += gcnew System::EventHandler(this, &ModList::GuiUpdater_Tick);
			// 
			// ObjectCount_LBL
			// 
			this->ObjectCount_LBL->AutoSize = true;
			this->ObjectCount_LBL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ObjectCount_LBL->Location = System::Drawing::Point(9, 41);
			this->ObjectCount_LBL->Name = L"ObjectCount_LBL";
			this->ObjectCount_LBL->Size = System::Drawing::Size(129, 16);
			this->ObjectCount_LBL->TabIndex = 7;
			this->ObjectCount_LBL->Text = L"Amount of Objects: 0";
			// 
			// ModList
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(389, 331);
			this->Controls->Add(this->ObjectCount_LBL);
			this->Controls->Add(this->OpenInFileExplorer_BTN);
			this->Controls->Add(this->OpenInWorkshop_BTN);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->ModList_LB);
			this->Controls->Add(this->ModSearchProgress);
			this->Controls->Add(this->BPName_LBL);
			this->Controls->Add(this->ModCount_LBL);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MaximumSize = System::Drawing::Size(405, 370);
			this->MinimumSize = System::Drawing::Size(405, 370);
			this->Name = L"ModList";
			this->ShowIcon = false;
			this->Text = L"Mod List";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &ModList::ModList_FormClosing);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		System::Void ModSearcher_BW_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);
		System::Void ModSearcher_BW_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);
		System::Void GuiUpdater_Tick(System::Object^ sender, System::EventArgs^ e);

		int FindModByObjUuid(const std::wstring& uuid);

		void AddModToList(const int& idx);
		System::Void ModList_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e);
		System::Void ModList_LB_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void OpenInWorkshop_BTN_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void OpenInFileExplorer_BTN_Click(System::Object^ sender, System::EventArgs^ e);
		bool GetCurrentMod(SMBC::ModData& mod);
	};
}