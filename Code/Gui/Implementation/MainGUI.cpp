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
#include "Lib/ProgramSettings.h"

namespace SMBC {
	using namespace BlueprintConverter;
};
namespace fs = std::filesystem;
typedef SMBC::MainGUI _MainGUI;

_MainGUI::MainGUI() {
	this->InitializeComponent();
	this->Blueprints = new std::vector<SMBC::Blueprint>();
	this->TempBPTable = new std::vector<SMBC::Blueprint>();
	this->BlueprintImage->WaitOnLoad = true;
	this->ActionProgress->Minimum = 0;
	this->GuiUpdater->Start();
	this->LoadDatabase();
	this->LoadBlueprints();

#if _DEBUG
	this->Text += " DEBUG";
#endif
}

_MainGUI::~MainGUI() {
	this->Blueprints->clear();
	this->TempBPTable->clear();
	delete this->Blueprints, this->TempBPTable;
	if (this->components) delete this->components;
}

System::Void _MainGUI::BPPath_TB_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	this->Start_BTN->Enabled = (BPPath_TB->TextLength > 0);
}

System::Void _MainGUI::Start_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	std::wstring _BlueprintPath = msclr::interop::marshal_as<std::wstring>(this->BPPath_TB->Text);
	if (!SMBC::FILE::FileExists(_BlueprintPath)) {
		SMBC::GUI::Warning("Invalid path", "The specified path doesn't exist!");
		return;
	}

	std::wstring _BlueprintFile = L"";
	std::wstring _BlueprintName = L"";
	fs::directory_entry _DirEntr(_BlueprintPath);

	if (_DirEntr.is_directory()) {
		std::wstring _BPFileDesc = (_BlueprintPath + L"/description.json");

		nlohmann::json _DescrJson;

		if (!SMBC::JSON::OpenParseJson(_BPFileDesc, _DescrJson, true)) {
			SMBC::GUI::Warning("Parse Error", "Couldn't parse \"description.json\"");
			return;
		}

		std::wstring _BPType = SMBC::JSON::GetJsonWstr(_DescrJson, "type");
		std::wstring _BPName = SMBC::JSON::GetJsonWstr(_DescrJson, "name");

		if (_BPType != L"Blueprint" || _BPName.empty()) {
			SMBC::GUI::Warning("No Data", "The specified folder does not contain any information about the blueprint!");
			return;
		}

		_BlueprintFile = (_BlueprintPath + L"/blueprint.json");
		_BlueprintName = SMBC::Blueprint::FixBlueprintName(_BPName);

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
		SMBC::GUI::Warning("Invalid path", "Unknown file type");
		return;
	}

	if (!_BlueprintName.empty() && !_BlueprintFile.empty()) {
		SMBC::GeneratorSettings^ _GenSettings = gcnew SMBC::GeneratorSettings(_BlueprintName);
		_GenSettings->ShowDialog();

		if (_GenSettings->BlueprintName_TB->TextLength > 0 && _GenSettings->Success) {
			this->ChangeGUIState(true, true, false);

			System::Array^ _ThreadData = gcnew cli::array<System::Object^>(8);

			_ThreadData->SetValue(gcnew System::String(_BlueprintFile.c_str()), (int)0);
			_ThreadData->SetValue(_GenSettings->BlueprintName_TB->Text, (int)1);
			_ThreadData->SetValue(_GenSettings->SeparationType_CB->SelectedIndex, (int)2);
			_ThreadData->SetValue(_GenSettings->ApplyTextures_CB->Checked, (int)3);
			_ThreadData->SetValue(_GenSettings->ExportTexPaths_CB->Checked, (int)4);
			_ThreadData->SetValue(_GenSettings->MaterialsByColor_CB->Checked, (int)5);
			_ThreadData->SetValue(_GenSettings->ExportNormals_CB->Checked, (int)6);
			_ThreadData->SetValue(_GenSettings->ExportUVs_CB->Checked, (int)7);

			this->ObjectGenerator->RunWorkerAsync(_ThreadData);
			this->GuiUpdater->Start();
		}

		return;
	}

	SMBC::GUI::Warning(
		"Something went wrong",
		"This message shouldn't be seen at all, so contact the application developer and provide all the useful info"
	);
}

delegate void ErrorCodeDelegate(int code);

System::Void _MainGUI::ObjectGenerator_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
	System::Array^ _Data = safe_cast<System::Array^>(e->Argument);

	System::String^ _BlueprintPathS = safe_cast<System::String^>(_Data->GetValue((int)0));
	System::String^ _BlueprintNameS = safe_cast<System::String^>(_Data->GetValue((int)1));

	int _SeparationMethod = safe_cast<int>(_Data->GetValue((int)2));

	bool _ApplyTextures = safe_cast<bool>(_Data->GetValue((int)3));
	bool _ExportTexPaths = safe_cast<bool>(_Data->GetValue((int)4));
	bool _MaterialsByColor = safe_cast<bool>(_Data->GetValue((int)5));
	bool _ExportNormals = safe_cast<bool>(_Data->GetValue((int)6));
	bool _ExportUvs = safe_cast<bool>(_Data->GetValue((int)7));

	std::wstring _BlueprintPath = msclr::interop::marshal_as<std::wstring>(_BlueprintPathS);
	std::wstring _BlueprintName = msclr::interop::marshal_as<std::wstring>(_BlueprintNameS);

	SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_READING_JSON, 0);
	SMBC::LastGenerationOutput = SMBC::BPFunction::ConvertBlueprintToObj(
		_BlueprintPath,
		_BlueprintName,
		_SeparationMethod,
		_ExportTexPaths,
		_ApplyTextures && _ExportUvs,
		_ExportUvs,
		_ExportNormals,
		_MaterialsByColor && _ApplyTextures && _ExportUvs
	);
}

System::Void _MainGUI::GuiUpdater_Tick(System::Object^ sender, System::EventArgs^ e) {
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

System::Void _MainGUI::ObjectGenerator_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e) {
	this->GuiUpdater->Stop();
	this->GuiUpdater_Tick(nullptr, nullptr);

	if (SMBC::LastGenerationOutput > 0) SMBC::GUI::Error(
		"Conversion Error",
		gcnew System::String(SMBC::ConversionErrorTable[SMBC::LastGenerationOutput].c_str())
	);
	else SMBC::GUI::Message(
		"Success",
		"Successfully finished generating a 3D model!",
		System::Windows::Forms::MessageBoxButtons::OK,
		System::Windows::Forms::MessageBoxIcon::Information
	);

	SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_NONE, 0);
	this->ChangeGUIState(true, true, true);
	this->ActionProgress->Value = 0;
	this->ActionLabel->Text = "No Action";
}

System::Void _MainGUI::DatabaseLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
	SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_READING_DATABASE, 0);
	SMBC::ObjectDatabase::ClearDatabase();
	SMBC::ObjectDatabase::LoadGameDatabase();
	SMBC::ObjectDatabase::LoadModDatabase();
}

System::Void _MainGUI::DatabaseLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e) {
	SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_NONE, 0);
	this->GuiUpdater->Stop();
	this->ActionProgress->Value = 0;
	this->ActionLabel->Text = gcnew System::String((L"Successfully loaded " + std::to_wstring(SMBC::ObjectDatabase::ObjectDB.size()) + L" objects into the database").c_str());
	this->ChangeGUIState(this->LoadedBP, true, true);
}

System::Void _MainGUI::BlueprintLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
	for (std::wstring& BlueprintFolder : SMBC::Settings::BlueprintFolders) {
		if (!SMBC::FILE::FileExists(BlueprintFolder)) continue;

		for (auto& Folder : fs::directory_iterator(BlueprintFolder, fs::directory_options::skip_permission_denied)) {
			if (!Folder.is_directory()) continue;

			std::wstring BlueprintJson = (Folder.path().wstring() + L"/description.json");
			nlohmann::json _BlueprintDesc;

			if (!SMBC::JSON::OpenParseJson(BlueprintJson, _BlueprintDesc, true)) continue;

			std::wstring _BPName = SMBC::JSON::GetJsonWstr(_BlueprintDesc, "name");
			std::wstring _BPType = SMBC::JSON::GetJsonWstr(_BlueprintDesc, "type");

			if (_BPName.empty() || _BPType != L"Blueprint") continue;

			long long _FileId = 0;
			if (_BlueprintDesc.contains("fileId")) {
				auto& _FileIdJson = _BlueprintDesc.at("fileId");

				if (_FileIdJson.is_number())
					_FileId = _FileIdJson.get<long long>();
			}

			SMBC::Blueprint _NewBlueprint(
				_BPName,
				BlueprintJson,
				Folder.path().wstring(),
				(_FileId > 0) ? std::to_wstring(_FileId) : L""
			);

			this->Blueprints->push_back(_NewBlueprint);
		}
	}
}

System::Void _MainGUI::BlueprintLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e) {
	this->BlueprintList->BeginUpdate();
	this->BlueprintList->Items->Clear();
	for (SMBC::Blueprint& _Blueprint : *this->Blueprints)
		this->BlueprintList->Items->Add(gcnew System::String(_Blueprint.BlueprintName.c_str()));
	this->BlueprintList->EndUpdate();
	this->ChangeGUIState(true, this->LoadedDatabase, true);
}

System::Void _MainGUI::BlueprintList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	int Index = this->BlueprintList->SelectedIndex;
	if (Index <= -1 || Index == this->SelItemIndex) return;
	this->SelItemIndex = Index;

	SMBC::Blueprint _CurBlueprint;
	if (!this->GetCurrentBlueprint(_CurBlueprint)) {
		SMBC::GUI::Error("Error", "Couldn't get the specified blueprint!");
		return;
	}

	if (!_CurBlueprint.BlueprintExists()) {
		SMBC::GUI::Warning(
			"Blueprint Doesn't Exist",
			"The blueprint you have just selected doesn't exist anymore!\n\nThe blueprint list will be automatically reloaded as soon as this message will be closed"
		);

		this->LoadBlueprints();

		return;
	}

	std::wstring _BPImage = _CurBlueprint.FindBlueprintImage();
	this->OpenInWorkshop_BTN->Enabled = !_CurBlueprint.WorkshopId.empty();

	if (this->BlueprintImage->Image) delete this->BlueprintImage->Image;
	this->BlueprintImage->Image = nullptr;
	this->BlueprintImage->Invalidate();

	if (!_BPImage.empty())
		this->BlueprintImage->ImageLocation = gcnew System::String(_BPImage.c_str());
	this->BPPath_TB->Text = gcnew System::String(_CurBlueprint.BlueprintFolder.c_str());
}

System::Void _MainGUI::SearchTB_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	if (this->TempBPTable->size() > 0) this->TempBPTable->clear();
	if (this->Blueprints->size() <= 0) return;
	this->SelItemIndex = -1;

	if (this->SearchTB->TextLength > 0) {
		std::wstring SearchText = msclr::interop::marshal_as<std::wstring>(this->SearchTB->Text->ToLowerInvariant());
		for (SMBC::Blueprint& _Blueprint : *this->Blueprints) {
			System::String^ _StrCopyS = gcnew System::String(_Blueprint.BlueprintName.c_str());
			std::wstring _StrCopy = msclr::interop::marshal_as<std::wstring>(_StrCopyS->ToLowerInvariant());

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

System::Void _MainGUI::BlueprintList_EnabledChanged(System::Object^ sender, System::EventArgs^ e) {
	this->SearchTB->Enabled = this->BlueprintList->Enabled;
}

System::Void _MainGUI::aboutToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
	SMBC::About^ AboutGUI = gcnew SMBC::About();
	AboutGUI->ShowDialog();
	delete AboutGUI;
}

System::Void _MainGUI::LoadDatabase() {
	this->ChangeGUIState(this->LoadedBP, false, true);
	this->GuiUpdater->Start();
	this->DatabaseLoader->RunWorkerAsync();
}
System::Void _MainGUI::LoadBlueprints() {
	this->SelItemIndex = -1;
	this->ChangeGUIState(false, this->LoadedDatabase, true);
	this->SearchTB->Clear();
	this->Blueprints->clear();
	this->TempBPTable->clear();
	this->BlueprintLoader->RunWorkerAsync();
}

System::Void _MainGUI::reloadBlueprintListToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
	this->LoadBlueprints();
}

System::Void _MainGUI::reloadObjectDatabaseToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
	this->LoadDatabase();
}

System::Void _MainGUI::ChangeGUIState(bool bploaded, bool databaseloaded, bool bpgenerated) {
	this->LoadedBP = bploaded;
	this->LoadedDatabase = databaseloaded;

	bool dpandbp = (databaseloaded && bpgenerated);
	bool bplandbp = (bploaded && bpgenerated);

	this->BPPath_TB->Enabled = dpandbp;
	this->Start_BTN->Enabled = (dpandbp && this->BPPath_TB->TextLength > 0);

	if (dpandbp) {
		SMBC::Blueprint _CurBlueprint;
		if (this->GetCurrentBlueprint(_CurBlueprint)) {
			if (!_CurBlueprint.WorkshopId.empty())
				this->OpenInWorkshop_BTN->Enabled = true;
		}
	}
	else this->OpenInWorkshop_BTN->Enabled = false;

	this->OpenBlueprint->Enabled = dpandbp;
	this->OpenOutputFolder_BTN->Enabled = dpandbp;
	this->reloadBlueprintListToolStripMenuItem->Enabled = bplandbp;
	this->reloadObjectDatabaseToolStripMenuItem->Enabled = dpandbp;
	this->menuStrip1->Enabled = bpgenerated;

	bool _BothLoaded = (dpandbp && bplandbp);

	this->BlueprintList->Enabled = _BothLoaded;
}

System::Void _MainGUI::settingsToolStripMenuItem1_Click(System::Object^ sender, System::EventArgs^ e) {
	this->OpenOptionsGUI();
}

System::Void _MainGUI::OpenOptionsGUI() {
	SMBC::SettingsGUI^ _Settings = gcnew SMBC::SettingsGUI();
	_Settings->ShowDialog();
	delete _Settings;
}

System::Void _MainGUI::MainGUI_Shown(System::Object^ sender, System::EventArgs^ e) {
	if (!SMBC::Settings::PathToSM.empty()) return;

	WForms::DialogResult dr = SMBC::GUI::Question(
		"Missing Path",
		"This program requires a path to Scrap Mechanic to work properly\n\nWould you like to set it up right now?"
	);

	if (dr == WForms::DialogResult::Yes)
		this->OpenOptionsGUI();
}

System::Void _MainGUI::OpenBlueprint_Click(System::Object^ sender, System::EventArgs^ e) {
	std::wstring _FileName = SMBC::GUI::OpenFileName(
		L"Select a Blueprint File",
		0,
		L"All Files (*.*)\0*.*\0",
		static_cast<HWND>(this->Handle.ToPointer())
	);

	if (_FileName.empty()) return;

	this->BPPath_TB->Text = gcnew System::String(_FileName.c_str());
}

System::Void _MainGUI::OpenOutputFolder_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	if (!SMBC::FILE::FileExists(L".\\Converted Models"))
		std::filesystem::create_directory(L".\\Converted Models");

	SMBC::GUI::OpenFolderInExplorer(L".\\Converted Models");
}

System::Void _MainGUI::MainGUI_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
	if (this->ObjectGenerator->IsBusy) {
		WForms::DialogResult dr = SMBC::GUI::Question(
			"Close",
			"Are you sure that you want to close the program while it's converting a blueprint?\n\nThis might corrupt the file it's currently writing!"
		);

		if (dr != WForms::DialogResult::Yes)
			e->Cancel = true;
	}
}

System::Void _MainGUI::OpenInWorkshop_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	SMBC::Blueprint _CurBlueprint;
	if (!this->GetCurrentBlueprint(_CurBlueprint)) return;

	if (_CurBlueprint.WorkshopId.empty()) {
		SMBC::GUI::Error("Error", "Couldn't open the workshop link to the specified blueprint!");
		return;
	}

	std::wstring _WorkshopLink;
	if (SMBC::Settings::OpenLinksInSteam)
		_WorkshopLink.append(L"steam://openurl/");

	_WorkshopLink.append(L"https://steamcommunity.com/sharedfiles/filedetails/?id=");
	_WorkshopLink.append(_CurBlueprint.WorkshopId);

	System::Diagnostics::Process::Start(gcnew System::String(_WorkshopLink.c_str()));
}

bool _MainGUI::GetCurrentBlueprint(SMBC::Blueprint& bp) {
	int _CurIdx = this->BlueprintList->SelectedIndex;

	if (_CurIdx <= -1) return false;

	std::vector<SMBC::Blueprint>& _CurrentList = (this->TempBPTable->size() > 0) ? *this->TempBPTable : *this->Blueprints;
	bp = _CurrentList.at(_CurIdx);
	
	return true;
}