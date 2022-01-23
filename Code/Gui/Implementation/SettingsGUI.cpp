#include <fstream>
#include <msclr/marshal_cppstd.h>
#include <filesystem>

#include "Gui/SettingsGUI.h"
#include "Lib/ProgramSettings.h"
#include "Lib/GuiLib/GuiLib.h"
#include "Lib/Bit.h"
#include "Lib/File/FileFunc.h"
#include "Lib/File/FileFunc.h"

namespace SMBC {
	using namespace BlueprintConverter;
};

#define SETTING_BLUEPRINT	0x1000
#define SETTING_MOD_LIST	0x0100
#define SETTING_SM_PATH		0x0010
#define SETTING_STEAM_OPEN	0x0001

typedef SMBC::SettingsGUI SetGui;

SetGui::SettingsGUI()
{
	this->InitializeComponent();

	this->OpenInWorkshop_CB->Checked = SMBC::Settings::OpenLinksInSteam;
	this->SMPath->Text = gcnew System::String(SMBC::Settings::PathToSM.c_str());

	this->AddItemsToListBox(this->ModList, SMBC::Settings::ModFolders);
	this->AddItemsToListBox(this->BlueprintList, SMBC::Settings::BlueprintFolders);

	this->SMPath->TextChanged += gcnew System::EventHandler(this, &SettingsGUI::SMPath_TextChanged);
	this->OpenInWorkshop_CB->CheckedChanged += gcnew System::EventHandler(this, &SettingsGUI::OpenInWorkshop_CB_CheckedChanged);
}

SetGui::~SettingsGUI()
{
	if (components) delete components;
}

void SetGui::SettingsGUI_FormClosing(
	System::Object^ sender,
	System::Windows::Forms::FormClosingEventArgs^ e
) {
	if (this->Save_BTN->Enabled)
	{
		WForms::DialogResult dr = SMBC::Gui::Question(
			"Unsaved Changes",
			"Are you sure that you want to close the settings window without saving any changes?\n\nAll unsaved changes will be lost!"
		);

		if (dr == WForms::DialogResult::No)
			e->Cancel = true;
	}
}

void SetGui::Save_BTN_Click(System::Object^ sender, System::EventArgs^ e)
{
	std::wstring _NewPathWstr = msclr::interop::marshal_as<std::wstring>(this->SMPath->Text);

	if (!SMBC::File::Exists(_NewPathWstr))
	{
		SMBC::Gui::Warning("Save Error", "The specified path to Scrap Mechanic is invalid!");
		return;
	}

	if (!SMBC::File::IsDirectory(_NewPathWstr))
	{
		SMBC::Gui::Warning("Save Error", "The specified path to Scrap Mechanic does not lead to a directory!");
		return;
	}

	this->Save_BTN->Enabled = false;

	bool _SSMP = SMBC::Bit::GetBit<int>(this->BinChanges, SETTING_SM_PATH);
	bool _SSteam = SMBC::Bit::GetBit<int>(this->BinChanges, SETTING_STEAM_OPEN);
	bool _SBlueprint = SMBC::Bit::GetBit<int>(this->BinChanges, SETTING_BLUEPRINT);
	bool _SMods = SMBC::Bit::GetBit<int>(this->BinChanges, SETTING_MOD_LIST);

	if (_SSMP)
	{
		SMBC::Settings::PathToSM = _NewPathWstr;
		this->scrap_path_changed = true;
	}
	if (_SSteam) SMBC::Settings::OpenLinksInSteam = this->OpenInWorkshop_CB->Checked;
	if (_SBlueprint)
	{
		this->AddPathsToWstrArray(SMBC::Settings::BlueprintFolders, this->BlueprintList);
		this->blueprint_paths_changed = true;
	}
	if (_SMods)
	{
		this->AddPathsToWstrArray(SMBC::Settings::ModFolders, this->ModList);
		this->mod_paths_changed = true;
	}

	SMBC::Settings::SaveSettingsFile(_SSMP, _SBlueprint, _SMods, _SSteam);
	this->BinChanges = 0x0000;
}

void SetGui::ModText_TB_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
	this->ModAdd_BTN->Enabled = (this->ModText_TB->TextLength > 0);
}

void SetGui::ModAdd_BTN_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (!this->AddStringToListBox(this->ModText_TB, this->ModList)) return;

	bool _TablesEqual = !this->AreTablesEqual(SMBC::Settings::ModFolders, this->ModList);
	this->ChangeSetting(SETTING_MOD_LIST, _TablesEqual);
}

void SetGui::ModList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
	this->ModRemSelected->Enabled = (this->ModList->SelectedIndex > -1);
}

void SetGui::ModRemSelected_Click(System::Object^ sender, System::EventArgs^ e)
{
	int _CurIdx = this->ModList->SelectedIndex;
	if (_CurIdx <= -1) return;

	this->ModList->Items->RemoveAt(_CurIdx);

	if (this->ModList->Items->Count > 0)
	{
		const bool is_last = (this->ModList->Items->Count == _CurIdx);

		this->ModList->SelectedIndex = (is_last ? _CurIdx - 1 : _CurIdx);
	}

	bool _TablesEqual = !this->AreTablesEqual(SMBC::Settings::ModFolders, this->ModList);
	this->ChangeSetting(SETTING_MOD_LIST, _TablesEqual);
}

void SetGui::BlueprintText_TB_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
	this->BlueprintAdd_BTN->Enabled = (this->BlueprintText_TB->TextLength > 0);
}

void SetGui::BlueprintAdd_BTN_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (!this->AddStringToListBox(this->BlueprintText_TB, this->BlueprintList)) return;

	bool _TablesEqual = !this->AreTablesEqual(SMBC::Settings::BlueprintFolders, this->BlueprintList);
	this->ChangeSetting(SETTING_BLUEPRINT, _TablesEqual);
}

void SetGui::BlueprintList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
	this->BlueprintRemSelected->Enabled = (this->BlueprintList->SelectedIndex > -1);
}

void SetGui::BlueprintRemSelected_Click(System::Object^ sender, System::EventArgs^ e)
{
	int _CurIdx = this->BlueprintList->SelectedIndex;
	if (_CurIdx <= -1) return;

	this->BlueprintList->Items->RemoveAt(_CurIdx);

	if (this->BlueprintList->Items->Count > 0)
	{
		const bool is_last = (this->BlueprintList->Items->Count == _CurIdx);

		this->BlueprintList->SelectedIndex = (is_last ? _CurIdx - 1 : _CurIdx);
	}

	bool _TablesEqual = !this->AreTablesEqual(SMBC::Settings::BlueprintFolders, this->BlueprintList);
	this->ChangeSetting(SETTING_BLUEPRINT, _TablesEqual);
}

void SetGui::SMPath_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
	std::wstring& _WstrPath = msclr::interop::marshal_as<std::wstring>(this->SMPath->Text);
	bool IsChanged = (_WstrPath != SMBC::Settings::PathToSM);
	this->ChangeSetting(SETTING_SM_PATH, IsChanged);
}

void SetGui::AddItemsToListBox(
	System::Windows::Forms::ListBox^ list,
	std::vector<std::wstring>& vec
) {
	list->BeginUpdate();

	for (std::wstring& wstr : vec)
		list->Items->Add(gcnew System::String(wstr.c_str()));

	list->EndUpdate();
}

void SetGui::OpenInWorkshop_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
	bool _IsChanged = (this->OpenInWorkshop_CB->Checked != SMBC::Settings::OpenLinksInSteam);
	this->ChangeSetting(SETTING_STEAM_OPEN, _IsChanged);
}

bool SetGui::AddStringToListBox(
	System::Windows::Forms::TextBox^ tb,
	System::Windows::Forms::ListBox^ lb
) {
	if (tb->TextLength <= -1) return false;

	std::wstring _Path = msclr::interop::marshal_as<std::wstring>(tb->Text);
	if (!SMBC::File::Exists(_Path))
	{
		SMBC::Gui::Warning("Invalid Path", "The specified path is invalid!");
		return false;
	}

	if (!SMBC::File::IsDirectory(_Path))
	{
		SMBC::Gui::Warning("Invalid Path", "The specified path does not lead to a directory!");
		return false;
	}

	for (int a = 0; a < lb->Items->Count; a++)
	{
		std::wstring _Ws = msclr::interop::marshal_as<std::wstring>(lb->Items[a]->ToString());

		if (SMBC::File::IsEquivalent(_Path, _Ws))
		{
			SMBC::Gui::Warning(
				"Equivalent Path",
				"The specified path is equivalent to one of the items in the list!"
			);
			return false;
		}

		if (_Path == _Ws)
		{
			SMBC::Gui::Warning(
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

bool SetGui::AreTablesEqual(
	std::vector<std::wstring>& vec,
	System::Windows::Forms::ListBox^ lb
) {
	if (vec.size() != lb->Items->Count) return false;

	for (std::size_t a = 0; a < vec.size(); a++)
	{
		std::wstring _WstrLb = msclr::interop::marshal_as<std::wstring>(lb->Items[a]->ToString());
		if (vec[a] != _WstrLb)
			return false;
	}

	return true;
}

void SetGui::AddPathsToWstrArray(std::vector<std::wstring>& vec, System::Windows::Forms::ListBox^ lb)
{
	vec.clear();

	vec.reserve(lb->Items->Count);
	for (int a = 0u; a < lb->Items->Count; a++)
	{
		std::wstring _Wstr = msclr::interop::marshal_as<std::wstring>(lb->Items[a]->ToString());

		vec.push_back(_Wstr);
	}
}

void SetGui::ChangeSetting(int mask, bool value)
{
	int _NewSetting = this->BinChanges;
	SMBC::Bit::SetBit<int>(_NewSetting, mask, value);

	this->BinChanges = _NewSetting;

	this->Save_BTN->Enabled = (this->BinChanges > 0);
}

void SetGui::BrowseSMFolder_BTN_Click(System::Object^ sender, System::EventArgs^ e)
{
	std::wstring _SM_Path = SMBC::Gui::OpenFileName(
		L"Select a Scrap Mechanic Folder",
		FOS_PICKFOLDERS | FOS_DONTADDTORECENT,
		L"All Files (*.*)\0*.*\0",
		static_cast<HWND>(this->Handle.ToPointer())
	);

	if (_SM_Path.empty()) return;

	this->SMPath->Text = gcnew System::String(_SM_Path.c_str());
}

void SetGui::ModListDirSearch_BTN_Click(System::Object^ sender, System::EventArgs^ e)
{
	std::wstring _ModDirPath = SMBC::Gui::OpenFileName(
		L"Select a Directory Containing Mods",
		FOS_PICKFOLDERS | FOS_DONTADDTORECENT,
		L"All Files (*.*)\0*.*\0",
		static_cast<HWND>(this->Handle.ToPointer())
	);

	if (_ModDirPath.empty()) return;

	this->ModText_TB->Text = gcnew System::String(_ModDirPath.c_str());
}

void SetGui::BPDirSearch_BTN_Click(System::Object^ sender, System::EventArgs^ e)
{
	std::wstring _BPDirPath = SMBC::Gui::OpenFileName(
		L"Select a Directory Containing Blueprints",
		FOS_PICKFOLDERS | FOS_DONTADDTORECENT,
		L"All Files (*.*)\0*.*\0",
		static_cast<HWND>(this->Handle.ToPointer())
	);

	if (_BPDirPath.empty()) return;

	this->BlueprintText_TB->Text = gcnew System::String(_BPDirPath.c_str());
}