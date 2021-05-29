#pragma once

#include <vector>
#include <string>

namespace BlueprintConverter {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class SettingsGUI : public System::Windows::Forms::Form
	{
	public:
		int BinChanges = 0x0000;
		bool scrap_path_changed = false;

		bool mod_paths_changed = false;
		bool blueprint_paths_changed = false;

		SettingsGUI(void);
	protected:
		~SettingsGUI();
	private:
		System::Windows::Forms::Button^ Save_BTN;
		System::Windows::Forms::TabPage^ SMTab;
		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::TextBox^ SMPath;
		System::Windows::Forms::TabPage^ ModTab;
		System::Windows::Forms::TabControl^ SettingTabs;
		System::Windows::Forms::TabPage^ BlueprintTab;
		System::Windows::Forms::TextBox^ ModText_TB;
		System::Windows::Forms::Label^ label3;
		System::Windows::Forms::Button^ ModAdd_BTN;
		System::Windows::Forms::ListBox^ ModList;
		System::Windows::Forms::Button^ ModRemSelected;
		System::Windows::Forms::TextBox^ BlueprintText_TB;
		System::Windows::Forms::ListBox^ BlueprintList;
		System::Windows::Forms::Label^ label4;
		System::Windows::Forms::Button^ BlueprintRemSelected;
		System::Windows::Forms::Button^ BlueprintAdd_BTN;
		System::Windows::Forms::CheckBox^ OpenInWorkshop_CB;
		System::Windows::Forms::ToolTip^ ToolTip_TT;
		System::Windows::Forms::Button^ BrowseSMFolder_BTN;
		System::Windows::Forms::Button^ ModListDirSearch_BTN;
		System::Windows::Forms::Button^ BPDirSearch_BTN;
		System::ComponentModel::IContainer^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->Save_BTN = (gcnew System::Windows::Forms::Button());
			this->SMTab = (gcnew System::Windows::Forms::TabPage());
			this->BrowseSMFolder_BTN = (gcnew System::Windows::Forms::Button());
			this->OpenInWorkshop_CB = (gcnew System::Windows::Forms::CheckBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SMPath = (gcnew System::Windows::Forms::TextBox());
			this->ModTab = (gcnew System::Windows::Forms::TabPage());
			this->ModListDirSearch_BTN = (gcnew System::Windows::Forms::Button());
			this->ModText_TB = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->ModAdd_BTN = (gcnew System::Windows::Forms::Button());
			this->ModList = (gcnew System::Windows::Forms::ListBox());
			this->ModRemSelected = (gcnew System::Windows::Forms::Button());
			this->SettingTabs = (gcnew System::Windows::Forms::TabControl());
			this->BlueprintTab = (gcnew System::Windows::Forms::TabPage());
			this->BPDirSearch_BTN = (gcnew System::Windows::Forms::Button());
			this->BlueprintText_TB = (gcnew System::Windows::Forms::TextBox());
			this->BlueprintList = (gcnew System::Windows::Forms::ListBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->BlueprintRemSelected = (gcnew System::Windows::Forms::Button());
			this->BlueprintAdd_BTN = (gcnew System::Windows::Forms::Button());
			this->ToolTip_TT = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->SMTab->SuspendLayout();
			this->ModTab->SuspendLayout();
			this->SettingTabs->SuspendLayout();
			this->BlueprintTab->SuspendLayout();
			this->SuspendLayout();
			// 
			// Save_BTN
			// 
			this->Save_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->Save_BTN->Cursor = System::Windows::Forms::Cursors::Hand;
			this->Save_BTN->Enabled = false;
			this->Save_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Save_BTN->Location = System::Drawing::Point(359, 290);
			this->Save_BTN->Name = L"Save_BTN";
			this->Save_BTN->Size = System::Drawing::Size(108, 34);
			this->Save_BTN->TabIndex = 1;
			this->Save_BTN->Text = L"Save Changes";
			this->Save_BTN->UseVisualStyleBackColor = true;
			this->Save_BTN->Click += gcnew System::EventHandler(this, &SettingsGUI::Save_BTN_Click);
			// 
			// SMTab
			// 
			this->SMTab->Controls->Add(this->BrowseSMFolder_BTN);
			this->SMTab->Controls->Add(this->OpenInWorkshop_CB);
			this->SMTab->Controls->Add(this->label1);
			this->SMTab->Controls->Add(this->SMPath);
			this->SMTab->Location = System::Drawing::Point(4, 22);
			this->SMTab->Name = L"SMTab";
			this->SMTab->Size = System::Drawing::Size(447, 246);
			this->SMTab->TabIndex = 2;
			this->SMTab->Text = L"General";
			this->SMTab->UseVisualStyleBackColor = true;
			// 
			// BrowseSMFolder_BTN
			// 
			this->BrowseSMFolder_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->BrowseSMFolder_BTN->Location = System::Drawing::Point(376, 29);
			this->BrowseSMFolder_BTN->Margin = System::Windows::Forms::Padding(3, 3, 7, 3);
			this->BrowseSMFolder_BTN->Name = L"BrowseSMFolder_BTN";
			this->BrowseSMFolder_BTN->Size = System::Drawing::Size(64, 22);
			this->BrowseSMFolder_BTN->TabIndex = 4;
			this->BrowseSMFolder_BTN->Text = L"Browse";
			this->BrowseSMFolder_BTN->UseVisualStyleBackColor = true;
			this->BrowseSMFolder_BTN->Click += gcnew System::EventHandler(this, &SettingsGUI::BrowseSMFolder_BTN_Click);
			// 
			// OpenInWorkshop_CB
			// 
			this->OpenInWorkshop_CB->AutoSize = true;
			this->OpenInWorkshop_CB->Cursor = System::Windows::Forms::Cursors::Hand;
			this->OpenInWorkshop_CB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->OpenInWorkshop_CB->Location = System::Drawing::Point(7, 57);
			this->OpenInWorkshop_CB->Name = L"OpenInWorkshop_CB";
			this->OpenInWorkshop_CB->Size = System::Drawing::Size(334, 20);
			this->OpenInWorkshop_CB->TabIndex = 3;
			this->OpenInWorkshop_CB->Text = L"Open workshop items in Steam instead of a browser";
			this->ToolTip_TT->SetToolTip(this->OpenInWorkshop_CB, L"When \"Open in Steam Workshop\" button is pressed with this\r\noption enabled, the to"
				L"ol will show the specified blueprint in \r\nSteam instead of a browser");
			this->OpenInWorkshop_CB->UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(4, 10);
			this->label1->Margin = System::Windows::Forms::Padding(7, 0, 7, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(152, 16);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Path to Scrap Mechanic:";
			// 
			// SMPath
			// 
			this->SMPath->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->SMPath->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SMPath->Location = System::Drawing::Point(7, 29);
			this->SMPath->Margin = System::Windows::Forms::Padding(7, 3, 7, 3);
			this->SMPath->Name = L"SMPath";
			this->SMPath->Size = System::Drawing::Size(359, 22);
			this->SMPath->TabIndex = 0;
			// 
			// ModTab
			// 
			this->ModTab->Controls->Add(this->ModListDirSearch_BTN);
			this->ModTab->Controls->Add(this->ModText_TB);
			this->ModTab->Controls->Add(this->label3);
			this->ModTab->Controls->Add(this->ModAdd_BTN);
			this->ModTab->Controls->Add(this->ModList);
			this->ModTab->Controls->Add(this->ModRemSelected);
			this->ModTab->Location = System::Drawing::Point(4, 22);
			this->ModTab->Name = L"ModTab";
			this->ModTab->Padding = System::Windows::Forms::Padding(3);
			this->ModTab->Size = System::Drawing::Size(447, 246);
			this->ModTab->TabIndex = 1;
			this->ModTab->Text = L"Mod Directories";
			this->ModTab->UseVisualStyleBackColor = true;
			// 
			// ModListDirSearch_BTN
			// 
			this->ModListDirSearch_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ModListDirSearch_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->ModListDirSearch_BTN->Location = System::Drawing::Point(328, 217);
			this->ModListDirSearch_BTN->Name = L"ModListDirSearch_BTN";
			this->ModListDirSearch_BTN->Size = System::Drawing::Size(35, 22);
			this->ModListDirSearch_BTN->TabIndex = 12;
			this->ModListDirSearch_BTN->Text = L"...";
			this->ModListDirSearch_BTN->UseVisualStyleBackColor = true;
			this->ModListDirSearch_BTN->Click += gcnew System::EventHandler(this, &SettingsGUI::ModListDirSearch_BTN_Click);
			// 
			// ModText_TB
			// 
			this->ModText_TB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ModText_TB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ModText_TB->Location = System::Drawing::Point(7, 217);
			this->ModText_TB->Margin = System::Windows::Forms::Padding(3, 3, 3, 4);
			this->ModText_TB->Name = L"ModText_TB";
			this->ModText_TB->Size = System::Drawing::Size(315, 22);
			this->ModText_TB->TabIndex = 11;
			this->ModText_TB->TextChanged += gcnew System::EventHandler(this, &SettingsGUI::ModText_TB_TextChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(4, 10);
			this->label3->Margin = System::Windows::Forms::Padding(3, 7, 3, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(230, 16);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Paths to Directories Containing Mods:";
			// 
			// ModAdd_BTN
			// 
			this->ModAdd_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ModAdd_BTN->Cursor = System::Windows::Forms::Cursors::Hand;
			this->ModAdd_BTN->Enabled = false;
			this->ModAdd_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ModAdd_BTN->Location = System::Drawing::Point(369, 217);
			this->ModAdd_BTN->Margin = System::Windows::Forms::Padding(3, 3, 3, 7);
			this->ModAdd_BTN->Name = L"ModAdd_BTN";
			this->ModAdd_BTN->Size = System::Drawing::Size(71, 22);
			this->ModAdd_BTN->TabIndex = 10;
			this->ModAdd_BTN->Text = L"Add";
			this->ModAdd_BTN->UseVisualStyleBackColor = true;
			this->ModAdd_BTN->Click += gcnew System::EventHandler(this, &SettingsGUI::ModAdd_BTN_Click);
			// 
			// ModList
			// 
			this->ModList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ModList->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ModList->FormattingEnabled = true;
			this->ModList->HorizontalScrollbar = true;
			this->ModList->IntegralHeight = false;
			this->ModList->ItemHeight = 16;
			this->ModList->Location = System::Drawing::Point(7, 29);
			this->ModList->Margin = System::Windows::Forms::Padding(7, 3, 3, 10);
			this->ModList->Name = L"ModList";
			this->ModList->Size = System::Drawing::Size(356, 175);
			this->ModList->TabIndex = 7;
			this->ModList->SelectedIndexChanged += gcnew System::EventHandler(this, &SettingsGUI::ModList_SelectedIndexChanged);
			// 
			// ModRemSelected
			// 
			this->ModRemSelected->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ModRemSelected->Cursor = System::Windows::Forms::Cursors::Hand;
			this->ModRemSelected->Enabled = false;
			this->ModRemSelected->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ModRemSelected->Location = System::Drawing::Point(369, 164);
			this->ModRemSelected->Name = L"ModRemSelected";
			this->ModRemSelected->Size = System::Drawing::Size(71, 40);
			this->ModRemSelected->TabIndex = 9;
			this->ModRemSelected->Text = L"Remove Selected";
			this->ModRemSelected->UseVisualStyleBackColor = true;
			this->ModRemSelected->Click += gcnew System::EventHandler(this, &SettingsGUI::ModRemSelected_Click);
			// 
			// SettingTabs
			// 
			this->SettingTabs->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->SettingTabs->Controls->Add(this->SMTab);
			this->SettingTabs->Controls->Add(this->ModTab);
			this->SettingTabs->Controls->Add(this->BlueprintTab);
			this->SettingTabs->HotTrack = true;
			this->SettingTabs->Location = System::Drawing::Point(12, 12);
			this->SettingTabs->Multiline = true;
			this->SettingTabs->Name = L"SettingTabs";
			this->SettingTabs->SelectedIndex = 0;
			this->SettingTabs->Size = System::Drawing::Size(455, 272);
			this->SettingTabs->TabIndex = 2;
			// 
			// BlueprintTab
			// 
			this->BlueprintTab->Controls->Add(this->BPDirSearch_BTN);
			this->BlueprintTab->Controls->Add(this->BlueprintText_TB);
			this->BlueprintTab->Controls->Add(this->BlueprintList);
			this->BlueprintTab->Controls->Add(this->label4);
			this->BlueprintTab->Controls->Add(this->BlueprintRemSelected);
			this->BlueprintTab->Controls->Add(this->BlueprintAdd_BTN);
			this->BlueprintTab->Location = System::Drawing::Point(4, 22);
			this->BlueprintTab->Name = L"BlueprintTab";
			this->BlueprintTab->Size = System::Drawing::Size(447, 246);
			this->BlueprintTab->TabIndex = 3;
			this->BlueprintTab->Text = L"Blueprint Directories";
			this->BlueprintTab->UseVisualStyleBackColor = true;
			// 
			// BPDirSearch_BTN
			// 
			this->BPDirSearch_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->BPDirSearch_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->BPDirSearch_BTN->Location = System::Drawing::Point(328, 217);
			this->BPDirSearch_BTN->Name = L"BPDirSearch_BTN";
			this->BPDirSearch_BTN->Size = System::Drawing::Size(35, 22);
			this->BPDirSearch_BTN->TabIndex = 17;
			this->BPDirSearch_BTN->Text = L"...";
			this->BPDirSearch_BTN->UseVisualStyleBackColor = true;
			this->BPDirSearch_BTN->Click += gcnew System::EventHandler(this, &SettingsGUI::BPDirSearch_BTN_Click);
			// 
			// BlueprintText_TB
			// 
			this->BlueprintText_TB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->BlueprintText_TB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->BlueprintText_TB->Location = System::Drawing::Point(7, 217);
			this->BlueprintText_TB->Margin = System::Windows::Forms::Padding(3, 3, 3, 4);
			this->BlueprintText_TB->Name = L"BlueprintText_TB";
			this->BlueprintText_TB->Size = System::Drawing::Size(315, 22);
			this->BlueprintText_TB->TabIndex = 16;
			this->BlueprintText_TB->TextChanged += gcnew System::EventHandler(this, &SettingsGUI::BlueprintText_TB_TextChanged);
			// 
			// BlueprintList
			// 
			this->BlueprintList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->BlueprintList->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->BlueprintList->FormattingEnabled = true;
			this->BlueprintList->HorizontalScrollbar = true;
			this->BlueprintList->IntegralHeight = false;
			this->BlueprintList->ItemHeight = 16;
			this->BlueprintList->Location = System::Drawing::Point(7, 29);
			this->BlueprintList->Margin = System::Windows::Forms::Padding(7, 3, 3, 10);
			this->BlueprintList->Name = L"BlueprintList";
			this->BlueprintList->Size = System::Drawing::Size(356, 175);
			this->BlueprintList->TabIndex = 12;
			this->BlueprintList->SelectedIndexChanged += gcnew System::EventHandler(this, &SettingsGUI::BlueprintList_SelectedIndexChanged);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(4, 10);
			this->label4->Margin = System::Windows::Forms::Padding(3, 10, 3, 0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(255, 16);
			this->label4->TabIndex = 13;
			this->label4->Text = L"Paths to Directories Containing Blueprints:";
			// 
			// BlueprintRemSelected
			// 
			this->BlueprintRemSelected->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->BlueprintRemSelected->Cursor = System::Windows::Forms::Cursors::Hand;
			this->BlueprintRemSelected->Enabled = false;
			this->BlueprintRemSelected->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->BlueprintRemSelected->Location = System::Drawing::Point(369, 164);
			this->BlueprintRemSelected->Name = L"BlueprintRemSelected";
			this->BlueprintRemSelected->Size = System::Drawing::Size(71, 40);
			this->BlueprintRemSelected->TabIndex = 14;
			this->BlueprintRemSelected->Text = L"Remove Selected";
			this->BlueprintRemSelected->UseVisualStyleBackColor = true;
			this->BlueprintRemSelected->Click += gcnew System::EventHandler(this, &SettingsGUI::BlueprintRemSelected_Click);
			// 
			// BlueprintAdd_BTN
			// 
			this->BlueprintAdd_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->BlueprintAdd_BTN->Cursor = System::Windows::Forms::Cursors::Hand;
			this->BlueprintAdd_BTN->Enabled = false;
			this->BlueprintAdd_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->BlueprintAdd_BTN->Location = System::Drawing::Point(369, 217);
			this->BlueprintAdd_BTN->Margin = System::Windows::Forms::Padding(3, 3, 3, 7);
			this->BlueprintAdd_BTN->Name = L"BlueprintAdd_BTN";
			this->BlueprintAdd_BTN->Size = System::Drawing::Size(71, 22);
			this->BlueprintAdd_BTN->TabIndex = 15;
			this->BlueprintAdd_BTN->Text = L"Add";
			this->BlueprintAdd_BTN->UseVisualStyleBackColor = true;
			this->BlueprintAdd_BTN->Click += gcnew System::EventHandler(this, &SettingsGUI::BlueprintAdd_BTN_Click);
			// 
			// SettingsGUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(479, 336);
			this->Controls->Add(this->SettingTabs);
			this->Controls->Add(this->Save_BTN);
			this->MinimumSize = System::Drawing::Size(495, 375);
			this->Name = L"SettingsGUI";
			this->ShowIcon = false;
			this->Text = L"Settings";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &SettingsGUI::SettingsGUI_FormClosing);
			this->SMTab->ResumeLayout(false);
			this->SMTab->PerformLayout();
			this->ModTab->ResumeLayout(false);
			this->ModTab->PerformLayout();
			this->SettingTabs->ResumeLayout(false);
			this->BlueprintTab->ResumeLayout(false);
			this->BlueprintTab->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Void SettingsGUI_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e);
		System::Void Save_BTN_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ModText_TB_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void ModAdd_BTN_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ModList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void ModRemSelected_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void BlueprintText_TB_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void BlueprintAdd_BTN_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void BlueprintList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void BlueprintRemSelected_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void SMPath_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void AddItemsToListBox(System::Windows::Forms::ListBox^ list, std::vector<std::wstring>& vec);
		System::Void OpenInWorkshop_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		bool AreTablesEqual(std::vector<std::wstring>& vec, System::Windows::Forms::ListBox^ lb);
		bool AddStringToListBox(
			System::Windows::Forms::TextBox^ tb,
			System::Windows::Forms::ListBox^ lb
		);

		void AddPathsToWstrArray(std::vector<std::wstring>& vec, System::Windows::Forms::ListBox^ lb);
		void ChangeSetting(int mask, bool value);
		System::Void BrowseSMFolder_BTN_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ModListDirSearch_BTN_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void BPDirSearch_BTN_Click(System::Object^ sender, System::EventArgs^ e);
	};
}