#include <fstream>
#include <msclr/marshal_cppstd.h>
#include <filesystem>

#include "Gui/SettingsGUI.h"
#include "Object Database/ObjectDatabase.h"
#include "Lib/Functions/Functions.h"

namespace SMBC {
	using namespace BlueprintConverter;
};

SMBC::SettingsGUI::SettingsGUI() {
	this->InitializeComponent();

	this->SMPath->Text = gcnew System::String(SMBC::ObjectDatabase::_sm_path.c_str());

	this->AddItemsToListBox(this->ModList, SMBC::ObjectDatabase::_mods_path);
	this->AddItemsToListBox(this->BlueprintList, SMBC::Blueprint::BlueprintPaths);

	this->SMPath->TextChanged += gcnew System::EventHandler(this, &SettingsGUI::SMPath_TextChanged);
}

SMBC::SettingsGUI::~SettingsGUI() {
	if (components) delete components;
}

System::Void SMBC::SettingsGUI::SettingsGUI_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
	if (this->Save_BTN->Enabled) {
		System::Windows::Forms::DialogResult dr = System::Windows::Forms::MessageBox::Show(
			"Are you sure that you want to close the settings window without saving any changes?\n\nAll unsaved changes will be lost!",
			"Unsaved Changes",
			System::Windows::Forms::MessageBoxButtons::YesNo,
			System::Windows::Forms::MessageBoxIcon::Question
		);
		if (dr == System::Windows::Forms::DialogResult::No)
			e->Cancel = true;
	}
}

System::Void SMBC::SettingsGUI::Save_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	this->Save_BTN->Enabled = false;

	SMBC::ObjectDatabase::_sm_path = msclr::interop::marshal_as<std::wstring>(this->SMPath->Text);

	SMBC::ObjectDatabase::SaveConfigFile(true, true, true);
}

System::Void SMBC::SettingsGUI::ModText_TB_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	this->ModAdd_BTN->Enabled = (this->ModText_TB->TextLength > 0);
}

System::Void SMBC::SettingsGUI::ModAdd_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	if (this->ModText_TB->TextLength <= 0) return;

	if (this->ModList->Items->Contains(this->ModText_TB->Text)) {
		System::Windows::Forms::MessageBox::Show(
			gcnew System::String("The specified string \"" + this->ModText_TB->Text + "\" already exists!"),
			"Value Exists",
			System::Windows::Forms::MessageBoxButtons::OK,
			System::Windows::Forms::MessageBoxIcon::Warning
		);
		return;
	}

	this->ModList->Items->Add(this->ModText_TB->Text);
	SMBC::ObjectDatabase::_mods_path.push_back(msclr::interop::marshal_as<std::wstring>(this->ModText_TB->Text));
	this->ModText_TB->Clear();
	this->ChangeGUIState(this->BlueprintListChanged, true, this->SMPathChanged, this->SMDataListChanged);
}

System::Void SMBC::SettingsGUI::ModList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	this->ModRemSelected->Enabled = (this->ModList->SelectedIndex > -1);
}

System::Void SMBC::SettingsGUI::ModRemSelected_Click(System::Object^ sender, System::EventArgs^ e) {
	if (this->ModList->SelectedIndex <= -1) return;

	SMBC::ObjectDatabase::_mods_path.erase(SMBC::ObjectDatabase::_mods_path.begin() + this->ModList->SelectedIndex);
	this->ModList->Items->RemoveAt(this->ModList->SelectedIndex);
	this->ChangeGUIState(this->BlueprintListChanged, true, this->SMPathChanged, this->SMDataListChanged);
}

System::Void SMBC::SettingsGUI::BlueprintText_TB_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	this->BlueprintAdd_BTN->Enabled = (this->BlueprintText_TB->TextLength > 0);
}

System::Void SMBC::SettingsGUI::BlueprintAdd_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	if (this->BlueprintText_TB->TextLength <= -1) return;

	if (this->BlueprintList->Items->Contains(this->BlueprintText_TB->Text)) {
		System::Windows::Forms::MessageBox::Show(
			gcnew System::String("The specified string \"" + this->BlueprintText_TB->Text + "\" already exists!"),
			"Value Exists",
			System::Windows::Forms::MessageBoxButtons::OK,
			System::Windows::Forms::MessageBoxIcon::Warning
		);
		return;
	}

	this->BlueprintList->Items->Add(this->BlueprintText_TB->Text);
	SMBC::Blueprint::BlueprintPaths.push_back(msclr::interop::marshal_as<std::wstring>(this->BlueprintText_TB->Text));
	this->BlueprintText_TB->Clear();
	this->ChangeGUIState(true, this->ModListChanged, this->SMPathChanged, this->SMDataListChanged);
}

System::Void SMBC::SettingsGUI::BlueprintList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	this->BlueprintRemSelected->Enabled = (this->BlueprintList->SelectedIndex > -1);
}

System::Void SMBC::SettingsGUI::BlueprintRemSelected_Click(System::Object^ sender, System::EventArgs^ e) {
	if (this->BlueprintList->SelectedIndex <= -1) return;

	SMBC::Blueprint::BlueprintPaths.erase(SMBC::Blueprint::BlueprintPaths.begin() + this->BlueprintList->SelectedIndex);
	this->BlueprintList->Items->RemoveAt(this->BlueprintList->SelectedIndex);
	this->ChangeGUIState(true, this->ModListChanged, this->SMPathChanged, this->SMDataListChanged);
}

System::Void SMBC::SettingsGUI::SMPath_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	std::wstring& _WstrPath = msclr::interop::marshal_as<std::wstring>(this->SMPath->Text);
	bool IsChanged = (_WstrPath != SMBC::ObjectDatabase::_sm_path);
	this->ChangeGUIState(this->BlueprintListChanged, this->ModListChanged, IsChanged, this->SMDataListChanged);
}

System::Void SMBC::SettingsGUI::ChangeGUIState(bool bpList, bool modList, bool SMPath, bool SMData) {
	this->BlueprintListChanged = bpList;
	this->ModListChanged = modList;
	this->SMPathChanged = SMPath;
	this->SMDataListChanged = SMData;

	bool AnyTrue = (bpList || modList || SMPath || SMData);

	this->Save_BTN->Enabled = AnyTrue;
}

System::Void SMBC::SettingsGUI::AddItemsToListBox(System::Windows::Forms::ListBox^ list, std::vector<std::wstring>& vec) {
	list->BeginUpdate();

	for (std::wstring& wstr : vec)
		list->Items->Add(gcnew System::String(wstr.c_str()));

	list->EndUpdate();
}