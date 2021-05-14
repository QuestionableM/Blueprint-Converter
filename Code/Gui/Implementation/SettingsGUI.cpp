#include <fstream>
#include <msclr/marshal_cppstd.h>
#include <filesystem>

#include "Gui/SettingsGUI.h"
#include "Lib/ProgramSettings.h"
#include "Lib/Functions/Functions.h"
#include "Lib/GuiLib/GuiLib.h"
#include "Lib/Bit/Bit.h"
#include "Lib/File/FileFunc.h"

namespace SMBC {
	using namespace BlueprintConverter;
};

#define SETTING_BLUEPRINT	0x1000
#define SETTING_MOD_LIST	0x0100
#define SETTING_SM_PATH		0x0010
#define SETTING_STEAM_OPEN	0x0001

SMBC::SettingsGUI::SettingsGUI() {
	this->InitializeComponent();

	this->OpenInWorkshop_CB->Checked = SMBC::Settings::OpenLinksInSteam;
	this->SMPath->Text = gcnew System::String(SMBC::Settings::PathToSM.c_str());

	this->AddItemsToListBox(this->ModList, SMBC::Settings::ModFolders);
	this->AddItemsToListBox(this->BlueprintList, SMBC::Settings::BlueprintFolders);

	this->SMPath->TextChanged += gcnew System::EventHandler(this, &SettingsGUI::SMPath_TextChanged);
	this->OpenInWorkshop_CB->CheckedChanged += gcnew System::EventHandler(this, &SettingsGUI::OpenInWorkshop_CB_CheckedChanged);
}

SMBC::SettingsGUI::~SettingsGUI() {
	if (components) delete components;
}

System::Void SMBC::SettingsGUI::SettingsGUI_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
	if (this->Save_BTN->Enabled) {
		WForms::DialogResult dr = SMBC::GUI::Question(
			"Unsaved Changes",
			"Are you sure that you want to close the settings window without saving any changes?\n\nAll unsaved changes will be lost!"
		);

		if (dr == WForms::DialogResult::No)
			e->Cancel = true;
	}
}

System::Void SMBC::SettingsGUI::Save_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	std::wstring _NewPathWstr = msclr::interop::marshal_as<std::wstring>(this->SMPath->Text);
	if (!SMBC::FILE::FileExists(_NewPathWstr)) {
		SMBC::GUI::Warning("Save Error", "The specified path to Scrap Mechanic is invalid!");
		return;
	}

	this->Save_BTN->Enabled = false;

	bool _SSMP = SMBC::Bit::GetBit<int>(this->BinChanges, SETTING_SM_PATH);
	bool _SSteam = SMBC::Bit::GetBit<int>(this->BinChanges, SETTING_STEAM_OPEN);
	bool _SBlueprint = SMBC::Bit::GetBit<int>(this->BinChanges, SETTING_BLUEPRINT);
	bool _SMods = SMBC::Bit::GetBit<int>(this->BinChanges, SETTING_MOD_LIST);

	if (_SSMP) SMBC::Settings::PathToSM = _NewPathWstr;
	if (_SSteam) SMBC::Settings::OpenLinksInSteam = this->OpenInWorkshop_CB->Checked;
	if (_SBlueprint) this->AddPathsToWstrArray(SMBC::Settings::BlueprintFolders, this->BlueprintList);
	if (_SMods) this->AddPathsToWstrArray(SMBC::Settings::ModFolders, this->ModList);

	SMBC::Settings::SaveSettingsFile(_SSMP, _SBlueprint, _SMods, _SSteam);
	this->BinChanges = 0x0000;
}

System::Void SMBC::SettingsGUI::ModText_TB_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	this->ModAdd_BTN->Enabled = (this->ModText_TB->TextLength > 0);
}

System::Void SMBC::SettingsGUI::ModAdd_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	if (!this->AddStringToListBox(this->ModText_TB, this->ModList)) return;

	bool _TablesEqual = !this->AreTablesEqual(SMBC::Settings::ModFolders, this->ModList);
	this->ChangeSetting(SETTING_MOD_LIST, _TablesEqual);
}

System::Void SMBC::SettingsGUI::ModList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	this->ModRemSelected->Enabled = (this->ModList->SelectedIndex > -1);
}

System::Void SMBC::SettingsGUI::ModRemSelected_Click(System::Object^ sender, System::EventArgs^ e) {
	int _CurIdx = this->ModList->SelectedIndex;
	if (_CurIdx <= -1) return;

	this->ModList->Items->RemoveAt(_CurIdx);

	bool _TablesEqual = !this->AreTablesEqual(SMBC::Settings::ModFolders, this->ModList);
	this->ChangeSetting(SETTING_MOD_LIST, _TablesEqual);
}

System::Void SMBC::SettingsGUI::BlueprintText_TB_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	this->BlueprintAdd_BTN->Enabled = (this->BlueprintText_TB->TextLength > 0);
}

System::Void SMBC::SettingsGUI::BlueprintAdd_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	if (!this->AddStringToListBox(this->BlueprintText_TB, this->BlueprintList)) return;

	bool _TablesEqual = !this->AreTablesEqual(SMBC::Settings::BlueprintFolders, this->BlueprintList);
	this->ChangeSetting(SETTING_BLUEPRINT, _TablesEqual);
}

System::Void SMBC::SettingsGUI::BlueprintList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	this->BlueprintRemSelected->Enabled = (this->BlueprintList->SelectedIndex > -1);
}

System::Void SMBC::SettingsGUI::BlueprintRemSelected_Click(System::Object^ sender, System::EventArgs^ e) {
	int _CurIdx = this->BlueprintList->SelectedIndex;
	if (_CurIdx <= -1) return;

	this->BlueprintList->Items->RemoveAt(_CurIdx);

	bool _TablesEqual = !this->AreTablesEqual(SMBC::Settings::BlueprintFolders, this->BlueprintList);
	this->ChangeSetting(SETTING_BLUEPRINT, _TablesEqual);
}

System::Void SMBC::SettingsGUI::SMPath_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	std::wstring& _WstrPath = msclr::interop::marshal_as<std::wstring>(this->SMPath->Text);
	bool IsChanged = (_WstrPath != SMBC::Settings::PathToSM);
	this->ChangeSetting(SETTING_SM_PATH, IsChanged);
}

System::Void SMBC::SettingsGUI::AddItemsToListBox(
	System::Windows::Forms::ListBox^ list,
	std::vector<std::wstring>& vec
) {
	list->BeginUpdate();

	for (std::wstring& wstr : vec)
		list->Items->Add(gcnew System::String(wstr.c_str()));

	list->EndUpdate();
}

System::Void SMBC::SettingsGUI::OpenInWorkshop_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	bool _IsChanged = (this->OpenInWorkshop_CB->Checked != SMBC::Settings::OpenLinksInSteam);
	this->ChangeSetting(SETTING_STEAM_OPEN, _IsChanged);
}

#include "Lib/File/FileFunc.h"

bool SMBC::SettingsGUI::AddStringToListBox(
	System::Windows::Forms::TextBox^ tb,
	System::Windows::Forms::ListBox^ lb
) {
	if (tb->TextLength <= -1) return false;

	std::wstring _Path = msclr::interop::marshal_as<std::wstring>(tb->Text);
	if (!SMBC::FILE::FileExists(_Path)) {
		SMBC::GUI::Warning("Invalid Path", "The specified path is invalid!");
		return false;
	}

	for (int a = 0; a < lb->Items->Count; a++) {
		std::wstring _Ws = msclr::interop::marshal_as<std::wstring>(lb->Items[a]->ToString());

		if (SMBC::FILE::IsEquivalent(_Path, _Ws)) {
			SMBC::GUI::Warning(
				"Equivalent Path",
				"The specified path is equivalent to one of the items in the list!"
			);
			return false;
		}

		if (_Path == _Ws) {
			SMBC::GUI::Warning(
				"Value Exists",
				"The specified string already exists!"
			);
			return false;
		}
	}

	lb->Items->Add(tb->Text);
	tb->Clear();
	return true;
}

bool SMBC::SettingsGUI::AreTablesEqual(
	std::vector<std::wstring>& vec,
	System::Windows::Forms::ListBox^ lb
) {
	if (vec.size() != lb->Items->Count) return false;

	for (uint32_t a = 0u; a < vec.size(); a++) {
		std::wstring _WstrLb = msclr::interop::marshal_as<std::wstring>(lb->Items[a]->ToString());
		if (vec[a] != _WstrLb)
			return false;
	}

	return true;
}

void SMBC::SettingsGUI::AddPathsToWstrArray(std::vector<std::wstring>& vec, System::Windows::Forms::ListBox^ lb) {
	vec.clear();

	vec.reserve(lb->Items->Count);
	for (int a = 0u; a < lb->Items->Count; a++) {
		std::wstring _Wstr = msclr::interop::marshal_as<std::wstring>(lb->Items[a]->ToString());

		vec.push_back(_Wstr);
	}
}

void SMBC::SettingsGUI::ChangeSetting(int mask, bool value) {
	int _NewSetting = this->BinChanges;
	SMBC::Bit::SetBit<int>(_NewSetting, mask, value);

	this->BinChanges = _NewSetting;

	this->Save_BTN->Enabled = (this->BinChanges > 0);
}

System::Void SMBC::SettingsGUI::BrowseSMFolder_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	std::wstring _SM_Path = SMBC::GUI::OpenFileName(
		L"Select a Scrap Mechanic Folder",
		FOS_PICKFOLDERS,
		L"All Files (*.*)\0*.*\0",
		static_cast<HWND>(this->Handle.ToPointer())
	);

	if (_SM_Path.empty()) return;

	this->SMPath->Text = gcnew System::String(_SM_Path.c_str());
}