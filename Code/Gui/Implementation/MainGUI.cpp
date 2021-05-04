#include <msclr/marshal_cppstd.h>
#include <filesystem>
#include <cwctype>

#include "Gui/MainGUI.h"
#include "Gui/About.h"
#include "Gui/SettingsGUI.h"
#include "Gui/GeneratorSettings.h"

#include "Blueprint Converter/BlueprintConverter.h"

#include "Lib/Functions/Functions.h"
#include "Lib/GuiLib/GuiLib.h"
#include "Lib/File/FileFunc.h"
#include "Lib/Json/JsonFunc.h"

namespace SMBC {
	using namespace BlueprintConverter;
};
namespace fs = std::filesystem;

SMBC::MainGUI::MainGUI() {
	this->InitializeComponent();
	this->Blueprints = new std::vector<SMBC::Blueprint>();
	this->TempBPTable = new std::vector<SMBC::Blueprint>();
	this->BlueprintImage->WaitOnLoad = true;
	this->ActionProgress->Minimum = 0;
	this->GuiUpdater->Start();
	this->LoadDatabase();
	this->LoadBlueprints();
}

SMBC::MainGUI::~MainGUI() {
	this->Blueprints->clear();
	this->TempBPTable->clear();
	delete this->Blueprints, this->TempBPTable;
	if (this->components) delete this->components;
}

System::Void SMBC::MainGUI::BPPath_TB_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	this->Start_BTN->Enabled = (BPPath_TB->TextLength > 0);
}

System::Void SHOW_WARNING(System::String^ Title, System::String^ Message) {
	System::Windows::Forms::MessageBox::Show(
		Message, Title,
		System::Windows::Forms::MessageBoxButtons::OK,
		System::Windows::Forms::MessageBoxIcon::Warning
	);
}

System::Void SMBC::MainGUI::Start_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	std::wstring _BlueprintPath = msclr::interop::marshal_as<std::wstring>(this->BPPath_TB->Text);
	if (!SMBC::FILE::FileExists(_BlueprintPath)) {
		SHOW_WARNING("Invalid path", "The specified path doesn't exist!");
		return;
	}

	std::wstring _BlueprintFile = L"";
	std::wstring _BlueprintName = L"";
	fs::directory_entry _DirEntr(_BlueprintPath);

	if (_DirEntr.is_directory()) {
		std::wstring _BPFileDesc = (_BlueprintPath + L"/description.json");

		nlohmann::json* _DescrJson = SMBC::JSON::OpenParseJson(_BPFileDesc, true);
		if (!_DescrJson) {
			SHOW_WARNING("Parse Error", "Couldn't parse the \"description.json\" file");
			goto json_file_cleanup;
		}

		std::wstring* _BPType = SMBC::JSON::GetJsonWstr(_DescrJson, "type");
		std::wstring* _BPName = SMBC::JSON::GetJsonWstr(_DescrJson, "name");

		if (!(_BPType && *_BPType == L"Blueprint" && _BPName)) {
			SHOW_WARNING("No Data", "The specified folder does not contain any information about the blueprint!");
			goto json_data_cleanup;
		}

		_BlueprintFile = (_BlueprintPath + L"/blueprint.json");
		_BlueprintName = SMBC::Blueprint::FixBlueprintName(std::wstring(*_BPName));

	json_data_cleanup:
		delete _BPType, _BPName;
	json_file_cleanup:
		delete _DescrJson;

		if (_BlueprintFile.empty() || _BlueprintName.empty()) return;
	}
	else if (_DirEntr.is_regular_file()) {
		_BlueprintFile = _BlueprintPath;
		if (_DirEntr.path().has_stem())
			_BlueprintName = SMBC::Blueprint::FixBlueprintName(_DirEntr.path().stem().wstring());
		else
			_BlueprintName = L"UnknownBlueprint";
	}
	else {
		SHOW_WARNING("Invalid path", "Unknown file type");
		return;
	}

	if (!_BlueprintName.empty() && !_BlueprintFile.empty()) {
		SMBC::GeneratorSettings^ _GenSettings = gcnew SMBC::GeneratorSettings();
		_GenSettings->ShowDialog();

		if (_GenSettings->Success) {
			this->ChangeGUIState(true, true, false);

			System::Array^ _ThreadData = gcnew cli::array<System::Object^>(7);

			_ThreadData->SetValue(gcnew System::String(_BlueprintFile.c_str()), (int)0);
			_ThreadData->SetValue(gcnew System::String(_BlueprintName.c_str()), (int)1);
			_ThreadData->SetValue(_GenSettings->SeparateParts_CB->Checked, (int)2);
			_ThreadData->SetValue(_GenSettings->ExportTexPaths_CB->Checked, (int)3);
			_ThreadData->SetValue(_GenSettings->ApplyTextures_CB->Checked, (int)4);
			_ThreadData->SetValue(_GenSettings->ExportUVs_CB->Checked, (int)5);
			_ThreadData->SetValue(_GenSettings->ExportNormals_CB->Checked, (int)6);

			this->ObjectGenerator->RunWorkerAsync(_ThreadData);
			this->GuiUpdater->Start();
		}

		return;
	}

	SHOW_WARNING(
		"Something went wrong",
		"This message shouldn't be seen at all, so contact the application developer and provide all the useful info"
	);
}

delegate void ErrorCodeDelegate(int code);

System::Void SMBC::MainGUI::ObjectGenerator_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
	System::Array^ _Data = safe_cast<System::Array^>(e->Argument);

	System::String^ _BlueprintPathS = safe_cast<System::String^>(_Data->GetValue((int)0));
	System::String^ _BlueprintNameS = safe_cast<System::String^>(_Data->GetValue((int)1));
	bool _SeparateParts = safe_cast<bool>(_Data->GetValue((int)2));
	bool _ExportTexPaths = safe_cast<bool>(_Data->GetValue((int)3));
	bool _ApplyTextures = safe_cast<bool>(_Data->GetValue((int)4));
	bool _ExportUvs = safe_cast<bool>(_Data->GetValue((int)5));
	bool _ExportNormals = safe_cast<bool>(_Data->GetValue((int)6));

	std::wstring _BlueprintPath = msclr::interop::marshal_as<std::wstring>(_BlueprintPathS);
	std::wstring _BlueprintName = msclr::interop::marshal_as<std::wstring>(_BlueprintNameS);

	delete _Data, _BlueprintPathS, _BlueprintNameS;

	SMBC::BlueprintConversionData::SetNewStage(0, 0);
	SMBC::LastGenerationOutput = SMBC::BPFunction::ConvertBlueprintToObj(
		_BlueprintPath,
		_BlueprintName,
		_SeparateParts,
		_ExportTexPaths,
		_ApplyTextures && _ExportUvs,
		_ExportUvs,
		_ExportNormals
	);
}

System::Void SMBC::MainGUI::GuiUpdater_Tick(System::Object^ sender, System::EventArgs^ e) {
	long _Stage = SMBC::BlueprintConversionData::Stage;
	if (_Stage < 0) return;

	long _MaxValue = SMBC::BlueprintConversionData::ProgressBarMax;
	long _Value = SMBC::BlueprintConversionData::ProgressBarValue;

	this->ActionProgress->Maximum = _MaxValue;
	if (this->ActionProgress->Maximum < _Value)
		_Value = this->ActionProgress->Maximum;
	this->ActionProgress->Value = _Value;

	std::wstring _ProgressValue;
	if (_Stage != 0)
		_ProgressValue = L"(" + std::to_wstring(SMBC::BlueprintConversionData::ProgressBarValue) + L" / " + std::to_wstring(SMBC::BlueprintConversionData::ProgressBarMax) + L")";

	this->ActionLabel->Text = gcnew System::String((SMBC::ActionTable[_Stage] + _ProgressValue).c_str());
}

System::Void SMBC::MainGUI::ObjectGenerator_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e) {
	this->GuiUpdater->Stop();
	this->GuiUpdater_Tick(nullptr, nullptr);
	if (SMBC::LastGenerationOutput > 0) System::Windows::Forms::MessageBox::Show(
		gcnew System::String(SMBC::ConversionErrorTable[SMBC::LastGenerationOutput].c_str()),
		"Conversion Error",
		System::Windows::Forms::MessageBoxButtons::OK,
		System::Windows::Forms::MessageBoxIcon::Error
	);
	else System::Windows::Forms::MessageBox::Show(
		"Successfully finished the generation of 3D model!",
		"Success",
		System::Windows::Forms::MessageBoxButtons::OK,
		System::Windows::Forms::MessageBoxIcon::Information
	);
	SMBC::BlueprintConversionData::SetNewStage(-1, 0);
	this->ChangeGUIState(true, true, true);
	this->ActionProgress->Value = 0;
	this->ActionLabel->Text = "No Action";
}

System::Void SMBC::MainGUI::DatabaseLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
	SMBC::BlueprintConversionData::SetNewStage(7, 0);
	SMBC::ObjectDatabase::ClearDatabase();
	SMBC::ObjectDatabase::LoadGameDatabase();
	SMBC::ObjectDatabase::LoadModDatabase();
}

System::Void SMBC::MainGUI::DatabaseLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e) {
	SMBC::BlueprintConversionData::SetNewStage(-1, 0);
	this->GuiUpdater->Stop();
	this->ActionProgress->Value = 0;
	this->ActionLabel->Text = gcnew System::String((L"Successfully loaded " + std::to_wstring(SMBC::ObjectDatabase::ObjectDB.size()) + L" objects into the database").c_str());
	this->ChangeGUIState(this->LoadedBP, true, true);
}

System::Void SMBC::MainGUI::BlueprintLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
	for (std::wstring& BlueprintFolder : SMBC::Blueprint::BlueprintPaths) {
		if (!SMBC::FILE::FileExists(BlueprintFolder)) continue;

		for (auto& Folder : fs::directory_iterator(BlueprintFolder, fs::directory_options::skip_permission_denied)) {
			if (!Folder.is_directory()) continue;

			std::wstring BlueprintJson = (Folder.path().wstring() + L"/description.json");
			nlohmann::json* _BlueprintDesc = SMBC::JSON::OpenParseJson(BlueprintJson, true);

			if (_BlueprintDesc) {
				std::wstring _BPName = SMBC::JSON::GetJsonWstrA(_BlueprintDesc, "name");
				std::wstring _BPType = SMBC::JSON::GetJsonWstrA(_BlueprintDesc, "type");

				if (_BPName.empty() || (_BPType.empty() || _BPType != L"Blueprint")) continue;

				SMBC::Blueprint _NewBlueprint(_BPName, BlueprintJson, Folder.path().wstring());
				this->Blueprints->push_back(_NewBlueprint);
			}

			delete _BlueprintDesc;
		}
	}
}

System::Void SMBC::MainGUI::BlueprintLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e) {
	this->BlueprintList->BeginUpdate();
	this->BlueprintList->Items->Clear();
	for (SMBC::Blueprint& _Blueprint : *this->Blueprints)
		this->BlueprintList->Items->Add(gcnew System::String(_Blueprint.BlueprintName.c_str()));
	this->BlueprintList->EndUpdate();
	this->ChangeGUIState(true, this->LoadedDatabase, true);
}

System::Void SMBC::MainGUI::BlueprintList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	int Index = this->BlueprintList->SelectedIndex;
	if (Index <= -1 || Index == this->SelItemIndex) return;
	this->SelItemIndex = Index;

	std::vector<SMBC::Blueprint>& _CurrentList = (this->TempBPTable->size() > 0) ? *this->TempBPTable : *this->Blueprints;
	SMBC::Blueprint& _CurBlueprint = _CurrentList.at(Index);
	std::wstring _BPImage = _CurBlueprint.FindBlueprintImage();

	if (this->BlueprintImage->Image) delete this->BlueprintImage->Image;
	this->BlueprintImage->Image = nullptr;
	this->BlueprintImage->Invalidate();

	if (!_BPImage.empty())
		this->BlueprintImage->ImageLocation = gcnew System::String(_BPImage.c_str());
	this->BPPath_TB->Text = gcnew System::String(_CurBlueprint.BlueprintFolder.c_str());
}

System::Void SMBC::MainGUI::SearchTB_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	if (this->TempBPTable->size() > 0) this->TempBPTable->clear();
	if (this->Blueprints->size() <= 0) return;
	this->SelItemIndex = -1;

	if (this->SearchTB->TextLength > 0) {
		std::wstring SearchText = msclr::interop::marshal_as<std::wstring>(this->SearchTB->Text->ToLowerInvariant());
		for (SMBC::Blueprint& _Blueprint : *this->Blueprints) {
			System::String^ _StrCopyS = gcnew System::String(_Blueprint.BlueprintName.c_str());
			std::wstring _StrCopy = msclr::interop::marshal_as<std::wstring>(_StrCopyS->ToLowerInvariant());
			delete _StrCopyS;

			if (_StrCopy.find(SearchText) != std::wstring::npos)
				this->TempBPTable->push_back(_Blueprint);
		}
	}

	std::vector<SMBC::Blueprint>& _CurBlueprint = (this->TempBPTable->size() > 0) ? *this->TempBPTable : *this->Blueprints;
	if (_CurBlueprint.size() <= 0) return;

	this->Result_LBL->Text = gcnew System::String((L"Results: " + std::to_wstring(this->TempBPTable->size())).c_str());
	this->Result_LBL->Visible = (this->SearchTB->TextLength > 0);

	this->BlueprintList->BeginUpdate();
	this->BlueprintList->Items->Clear();
	for (SMBC::Blueprint& _Blueprint : _CurBlueprint)
		this->BlueprintList->Items->Add(gcnew System::String(_Blueprint.BlueprintName.c_str()));
	this->BlueprintList->EndUpdate();
}

System::Void SMBC::MainGUI::BlueprintList_EnabledChanged(System::Object^ sender, System::EventArgs^ e) {
	this->SearchTB->Enabled = this->BlueprintList->Enabled;
}

System::Void SMBC::MainGUI::aboutToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
	SMBC::About^ AboutGUI = gcnew SMBC::About();
	AboutGUI->ShowDialog();
	delete AboutGUI;
}

System::Void SMBC::MainGUI::LoadDatabase() {
	this->ChangeGUIState(this->LoadedBP, false, true);
	this->GuiUpdater->Start();
	this->DatabaseLoader->RunWorkerAsync();
}
System::Void SMBC::MainGUI::LoadBlueprints() {
	this->SelItemIndex = -1;
	this->ChangeGUIState(false, this->LoadedDatabase, true);
	this->SearchTB->Clear();
	this->Blueprints->clear();
	this->TempBPTable->clear();
	this->BlueprintLoader->RunWorkerAsync();
}

System::Void SMBC::MainGUI::reloadBlueprintListToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
	this->LoadBlueprints();
}

System::Void SMBC::MainGUI::reloadObjectDatabaseToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
	this->LoadDatabase();
}

System::Void SMBC::MainGUI::ChangeGUIState(bool bploaded, bool databaseloaded, bool bpgenerated) {
	this->LoadedBP = bploaded;
	this->LoadedDatabase = databaseloaded;

	bool dpandbp = (databaseloaded && bpgenerated);
	bool bplandbp = (bploaded && bpgenerated);

	this->BPPath_TB->Enabled = dpandbp;
	this->Start_BTN->Enabled = (dpandbp && this->BPPath_TB->TextLength > 0);

	this->OpenBlueprint->Enabled = dpandbp;
	this->OpenOutputFolder_BTN->Enabled = dpandbp;
	this->reloadBlueprintListToolStripMenuItem->Enabled = bplandbp;
	this->reloadObjectDatabaseToolStripMenuItem->Enabled = dpandbp;
	this->menuStrip1->Enabled = bpgenerated;

	bool _BothLoaded = (dpandbp && bplandbp);

	this->BlueprintList->Enabled = _BothLoaded;
}

System::Void SMBC::MainGUI::settingsToolStripMenuItem1_Click(System::Object^ sender, System::EventArgs^ e) {
	this->OpenOptionsGUI();
}

System::Void SMBC::MainGUI::OpenOptionsGUI() {
	SMBC::SettingsGUI^ _Settings = gcnew SMBC::SettingsGUI();
	_Settings->ShowDialog();
	delete _Settings;
}

System::Void SMBC::MainGUI::MainGUI_Shown(System::Object^ sender, System::EventArgs^ e) {
	if (SMBC::ObjectDatabase::_sm_path.empty()) {
		System::Windows::Forms::DialogResult dr = System::Windows::Forms::MessageBox::Show(
			"This program requires a path to Scrap Mechanic to work properly\n\nWould you like to set it up right now?",
			"Missing Path",
			System::Windows::Forms::MessageBoxButtons::YesNo,
			System::Windows::Forms::MessageBoxIcon::Question
		);

		if (dr == System::Windows::Forms::DialogResult::Yes)
			this->OpenOptionsGUI();
	}
}

System::Void SMBC::MainGUI::OpenBlueprint_Click(System::Object^ sender, System::EventArgs^ e) {
	this->ChangeGUIState(this->LoadedBP, this->LoadedDatabase, false);
	std::wstring _FileName = SMBC::GUI::OpenFileName();

	if (!_FileName.empty())
		this->BPPath_TB->Text = gcnew System::String(_FileName.c_str());

	this->ChangeGUIState(this->LoadedBP, this->LoadedDatabase, true);
}

System::Void SMBC::MainGUI::OpenOutputFolder_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	if (!SMBC::FILE::FileExists(L".\\Converted Models"))
		std::filesystem::create_directory(L".\\Converted Models");

	SMBC::GUI::OpenFolderInExplorer(L".\\Converted Models");
}