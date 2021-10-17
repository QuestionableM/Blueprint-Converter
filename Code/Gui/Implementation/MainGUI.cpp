#include <msclr/marshal_cppstd.h>
#include <filesystem>
#include <cwctype>

#include "Gui/MainGUI.h"
#include "Gui/About.h"
#include "Gui/SettingsGUI.h"
#include "Gui/GeneratorSettings.h"
#include "Gui/ModList.h"

#include "Blueprint Converter/BlueprintConverter.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/ConvData/ConvData.h"
#include "Lib/GuiLib/GuiLib.h"
#include "Lib/File/FileFunc.h"
#include "Lib/Json/JsonFunc.h"
#include "Lib/String/String.h"
#include "Lib/ProgramSettings.h"

#include "Object Database/Database Loader/DatabaseLoader.h"

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

	SendMessage(
		static_cast<HWND>(this->SearchTB->Handle.ToPointer()),
		EM_SETCUEBANNER,
		0,
		(LPARAM)L"Search Blueprints"
	);

	SendMessage(
		static_cast<HWND>(this->BPPath_TB->Handle.ToPointer()),
		EM_SETCUEBANNER,
		0,
		(LPARAM)L"Path to Blueprint"
	);

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
	if (!SMBC::File::Exists(_BlueprintPath)) {
		SMBC::Gui::Warning("Invalid path", "The specified path doesn't exist!");
		return;
	}

	std::wstring _BlueprintFile = L"";
	std::wstring _BlueprintName = L"";
	fs::directory_entry _DirEntr(_BlueprintPath);

	if (_DirEntr.is_directory()) {
		std::wstring _BPFileDesc = (_BlueprintPath + L"/description.json");

		nlohmann::json _DescrJson = SMBC::Json::LoadParseJson(_BPFileDesc);
		if (!_DescrJson.is_object())
		{
			SMBC::Gui::Warning("Parse Error", "Couldn't parse \"description.json\"");
			return;
		}

		std::wstring _BPType = SMBC::Json::GetWstr(_DescrJson, "type");
		std::wstring _BPName = SMBC::Json::GetWstr(_DescrJson, "name");

		if (_BPType != L"Blueprint" || _BPName.empty()) {
			SMBC::Gui::Warning("No Data", "The specified folder does not contain any information about the blueprint!");
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
		SMBC::Gui::Warning("Invalid path", "Unknown file type");
		return;
	}

	if (!_BlueprintName.empty() && !_BlueprintFile.empty()) {
		SMBC::GeneratorSettings^ _GenSettings = gcnew SMBC::GeneratorSettings(_BlueprintName);
		this->MakeFormCentered(_GenSettings);
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

	SMBC::Gui::Warning(
		"Something went wrong",
		"This message shouldn't be seen at all, so contact the application developer and provide all the useful info"
	);
}

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

	SMBC::Error error_data = SMBC::BPFunction::ConvertBlueprintToObj(
		_BlueprintPath,
		_BlueprintName,
		_SeparationMethod,
		_ExportTexPaths,
		_ApplyTextures && _ExportUvs,
		_ExportUvs,
		_ExportNormals,
		_MaterialsByColor && _ApplyTextures && _ExportUvs
	);

	e->Result = static_cast<int>(error_data);
}

System::Void _MainGUI::GuiUpdater_Tick(System::Object^ sender, System::EventArgs^ e) {
	long _State = static_cast<long>(SMBC::ConvData::State);
	if (_State < 0l) return;

	unsigned long long _MaxValue = SMBC::ConvData::ProgressMax;
	unsigned long long _Value = SMBC::ConvData::ProgressValue;

	this->ActionProgress->Maximum = (int)_MaxValue;

	unsigned long long _MaxCast = this->ActionProgress->Maximum;
	if (_MaxCast < _Value) _Value = _MaxCast;

	this->ActionProgress->Value = (int)_Value;

	std::wstring _ProgressValue = SMBC::ActionTable[_State];
	if (_State != 0l)
		SMBC::String::Combine(_ProgressValue, L"(", _Value, L" / ", _MaxValue, L")");

	this->ActionLabel->Text = gcnew System::String(_ProgressValue.c_str());
}

System::Void _MainGUI::ObjectGenerator_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e) {
	this->GuiUpdater->Stop();
	this->GuiUpdater_Tick(nullptr, nullptr);

	int result_code = safe_cast<int>(e->Result);
	if (result_code > 0) SMBC::Gui::Error(
		"Conversion Error",
		gcnew System::String(SMBC::ConversionErrorTable[result_code].c_str())
	);
	else SMBC::Gui::Message(
		"Success",
		"Successfully finished generating a 3D model!",
		System::Windows::Forms::MessageBoxButtons::OK,
		System::Windows::Forms::MessageBoxIcon::Information
	);

	SMBC::ConvData::SetState(SMBC::State::None);
	this->ChangeGUIState(true, true, true);
	this->ActionProgress->Value = 0;
	this->ActionLabel->Text = "No Action";
}

System::Void _MainGUI::DatabaseLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
	SMBC::DatabaseLoader::LoadDatabase();
}

System::Void _MainGUI::DatabaseLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e) {
	SMBC::ConvData::SetState(SMBC::State::None);
	this->GuiUpdater->Stop();
	this->ActionProgress->Value = 0;
	this->ActionLabel->Text = gcnew System::String((L"Successfully loaded " + std::to_wstring(SMBC::Mod::GetObjectCount()) + L" objects from " + std::to_wstring(SMBC::Mod::GetModCount()) + L" mods").c_str());
	this->ChangeGUIState(this->LoadedBP, true, true);
}

System::Void _MainGUI::BlueprintLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
	for (std::wstring& BlueprintFolder : SMBC::Settings::BlueprintFolders) {
		if (!SMBC::File::Exists(BlueprintFolder)) continue;

		fs::directory_iterator BPDirIter(BlueprintFolder, fs::directory_options::skip_permission_denied);
		for (auto& Folder : BPDirIter) {
			if (!Folder.is_directory()) continue;

			std::wstring BlueprintJson = (Folder.path().wstring() + L"/description.json");

			nlohmann::json _BlueprintDesc = SMBC::Json::LoadParseJson(BlueprintJson, true);
			if (!_BlueprintDesc.is_object())
				continue;

			std::wstring _BPName = SMBC::Json::GetWstr(_BlueprintDesc, "name");
			std::wstring _BPType = SMBC::Json::GetWstr(_BlueprintDesc, "type");

			if (_BPName.empty() || _BPType != L"Blueprint") continue;

			const auto& _FileIdJson = SMBC::Json::Get(_BlueprintDesc, "fileId");
			long long _FileId = _FileIdJson.is_number() ? _FileIdJson.get<long long>() : 0ll;

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
		this->BlueprintList->Items->Add(gcnew System::String(_Blueprint.Name.c_str()));
	this->BlueprintList->EndUpdate();

	this->ChangeGUIState(true, this->LoadedDatabase, true);
	this->UpdateBlueprintLabel(false);
}

System::Void _MainGUI::BlueprintList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	int Index = this->BlueprintList->SelectedIndex;
	if (Index <= -1 || Index == this->SelItemIndex) return;
	this->SelItemIndex = Index;

	SMBC::Blueprint _CurBlueprint;
	if (!this->GetCurrentBlueprint(_CurBlueprint)) {
		SMBC::Gui::Error("Error", "Couldn't get the specified blueprint!");
		return;
	}

	if (!_CurBlueprint.BlueprintExists()) {
		SMBC::Gui::Warning(
			"Blueprint Doesn't Exist",
			"The blueprint you have just selected doesn't exist anymore!\n\nThe blueprint list will be automatically reloaded as soon as this message will be closed"
		);

		this->LoadBlueprints();
		return;
	}

	this->BP_OpenOutputDir_BTN->Enabled = !_CurBlueprint.Folder.empty();
	this->BP_ShowModList_BTN->Enabled = !_CurBlueprint.Path.empty();
	this->BlueprintOptions_CMS->Enabled = (!_CurBlueprint.Folder.empty() || !_CurBlueprint.Path.empty());

	std::wstring _BPImage = _CurBlueprint.FindBlueprintImage();
	this->OpenInWorkshop_BTN->Enabled = !_CurBlueprint.WorkshopId.empty();

	if (this->BlueprintImage->Image) delete this->BlueprintImage->Image;
	this->BlueprintImage->Image = nullptr;
	this->BlueprintImage->Invalidate();

	if (!_BPImage.empty())
		this->BlueprintImage->ImageLocation = gcnew System::String(_BPImage.c_str());
	this->BPPath_TB->Text = gcnew System::String(_CurBlueprint.Folder.c_str());
}

System::Void _MainGUI::SearchTB_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	this->BP_OpenOutputDir_BTN->Enabled = false;
	this->BP_ShowModList_BTN->Enabled = false;
	this->BlueprintOptions_CMS->Enabled = false;

	if (this->TempBPTable->size() > 0) this->TempBPTable->clear();

	this->SelItemIndex = -1;
	if (this->SearchTB->TextLength > 0 && !this->Blueprints->empty()) {
		std::wstring l_SearchWstr = msclr::interop::marshal_as<std::wstring>(this->SearchTB->Text);
		SMBC::PathReplacer::ToLowercaseR(l_SearchWstr);

		for (SMBC::Blueprint& _Blueprint : *this->Blueprints) {
			if (_Blueprint.LowerName.find(l_SearchWstr) != std::wstring::npos)
				this->TempBPTable->push_back(_Blueprint);
		}
	}

	std::vector<SMBC::Blueprint>& _CurList = this->GetCurrentBPList();
	if (!_CurList.empty() || (_CurList.empty() && this->BlueprintList->Items->Count > 0)) {
		this->BlueprintList->BeginUpdate();
		this->BlueprintList->Items->Clear();
		for (SMBC::Blueprint& _Blueprint : _CurList)
			this->BlueprintList->Items->Add(gcnew System::String(_Blueprint.Name.c_str()));
		this->BlueprintList->EndUpdate();
	}
	
	this->UpdateBlueprintLabel(false);
}

System::Void _MainGUI::BlueprintList_EnabledChanged(System::Object^ sender, System::EventArgs^ e) {
	this->SearchTB->Enabled = this->BlueprintList->Enabled;
}

System::Void _MainGUI::aboutToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
	SMBC::About^ AboutGUI = gcnew SMBC::About();
	this->MakeFormCentered(AboutGUI);
	AboutGUI->ShowDialog();
	delete AboutGUI;
}

System::Void _MainGUI::LoadDatabase() {
	this->ChangeGUIState(this->LoadedBP, false, true);
	this->GuiUpdater->Start();
	this->DatabaseLoader->RunWorkerAsync();
}
System::Void _MainGUI::LoadBlueprints() {
	if (this->BlueprintImage->Image) delete this->BlueprintImage->Image;
	this->BlueprintImage->Image = nullptr;
	this->BlueprintImage->Invalidate();

	this->SelItemIndex = -1;
	this->ChangeGUIState(false, this->LoadedDatabase, true);
	this->SearchTB->Clear();
	this->Blueprints->clear();
	this->TempBPTable->clear();

	this->UpdateBlueprintLabel(true);
	this->BPListStatus_LBL->Text = L"Loading Blueprints...";

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
		bool _HasWorkshopId = false;
		bool _HasBPFolder = false;
		bool _HasBPPath = false;

		SMBC::Blueprint _CurBlueprint;
		if (this->GetCurrentBlueprint(_CurBlueprint)) {
			_HasWorkshopId = !_CurBlueprint.WorkshopId.empty();
			_HasBPFolder = !_CurBlueprint.Folder.empty();
			_HasBPPath = !_CurBlueprint.Path.empty();
		}

		this->BlueprintOptions_CMS->Enabled = (_HasBPFolder || _HasBPPath);
		this->BP_OpenOutputDir_BTN->Enabled = _HasBPFolder;
		this->BP_ShowModList_BTN->Enabled = _HasBPPath;
		this->OpenInWorkshop_BTN->Enabled = _HasWorkshopId;
	}
	else {
		this->OpenInWorkshop_BTN->Enabled = false;
		this->BlueprintOptions_CMS->Enabled = false;
		this->BP_OpenOutputDir_BTN->Enabled = false;
		this->BP_ShowModList_BTN->Enabled = false;
	}

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
	this->MakeFormCentered(_Settings);
	_Settings->ShowDialog();

	if (_Settings->scrap_path_changed) {
		this->BlueprintList->Items->Clear();
		this->TempBPTable->clear();
		this->Blueprints->clear();

		SMBC::Settings::LoadSettingsFile();

		this->GuiUpdater->Start();
		this->LoadDatabase();
		this->LoadBlueprints();
	}
	else {
		if (_Settings->blueprint_paths_changed) this->LoadBlueprints();
		if (_Settings->mod_paths_changed) this->LoadDatabase();
	}

	delete _Settings;
}

System::Void _MainGUI::MainGUI_Shown(System::Object^ sender, System::EventArgs^ e) {
	if (!SMBC::Settings::PathToSM.empty()) return;

	WForms::DialogResult dr = SMBC::Gui::Question(
		"Missing Path",
		"This program requires a path to Scrap Mechanic to work properly\n\nWould you like to set it up right now?"
	);

	if (dr == WForms::DialogResult::Yes)
		this->OpenOptionsGUI();
}

System::Void _MainGUI::OpenBlueprint_Click(System::Object^ sender, System::EventArgs^ e) {
	std::wstring _FileName = SMBC::Gui::OpenFileName(
		L"Select a Blueprint File",
		0,
		L"All Files (*.*)\0*.*\0",
		static_cast<HWND>(this->Handle.ToPointer())
	);

	if (_FileName.empty()) return;

	this->BPPath_TB->Text = gcnew System::String(_FileName.c_str());
}

System::Void _MainGUI::OpenOutputFolder_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	if (SMBC::File::SafeCreateDir(L".\\Converted Models"))
		SMBC::Gui::OpenFolderInExplorer(L".\\Converted Models");
	else
		SMBC::Gui::Error("Error", "Couldn't open a directory with converted models.");
}

System::Void _MainGUI::MainGUI_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
	if (this->ObjectGenerator->IsBusy) {
		WForms::DialogResult dr = SMBC::Gui::Question(
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
		SMBC::Gui::Error("Error", "Couldn't open the workshop link to the specified blueprint!");
		return;
	}

	std::wstring _WorkshopLink;
	if (SMBC::Settings::OpenLinksInSteam)
		_WorkshopLink.append(L"steam://openurl/");

	SMBC::String::Combine(_WorkshopLink, L"https://steamcommunity.com/sharedfiles/filedetails/?id=", _CurBlueprint.WorkshopId);

	System::Diagnostics::Process::Start(gcnew System::String(_WorkshopLink.c_str()));
}

System::Void _MainGUI::BP_OpenOutputDir_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	SMBC::Blueprint _CurBlueprint;
	if (!this->GetCurrentBlueprint(_CurBlueprint)) return;

	if (!SMBC::File::Exists(_CurBlueprint.Folder)) {
		SMBC::Gui::Error("Internal Error", "The path to the selected blueprint doesn't exist!");
		return;
	}

	std::wstring path_cpy = _CurBlueprint.Folder;
	SMBC::PathReplacer::ReplaceAll(path_cpy, L'/', L'\\');
	
	SMBC::Gui::OpenFolderInExplorer(path_cpy);
}

bool _MainGUI::GetCurrentBlueprint(SMBC::Blueprint& bp) {
	int _CurIdx = this->BlueprintList->SelectedIndex;

	if (_CurIdx <= -1) return false;

	std::vector<SMBC::Blueprint>& _CurList = this->GetCurrentBPList();
	if ((int)_CurList.size() < _CurIdx) return false;
	bp = _CurList.at(_CurIdx);
	
	return true;
}

System::Void _MainGUI::BP_ShowModList_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	SMBC::Blueprint sel_blueprint;

	if (!this->GetCurrentBlueprint(sel_blueprint)) return;

	if (!SMBC::File::Exists(sel_blueprint.Path)) {
		SMBC::Gui::Warning(L"Invalid Blueprint", L"The path to specified blueprint doesn't exist anymore!");
		return;
	}

	SMBC::ModList^ ModListGUI = gcnew SMBC::ModList(sel_blueprint);
	this->MakeFormCentered(ModListGUI);
	ModListGUI->ShowDialog();

	delete ModListGUI;
}

std::vector<SMBC::Blueprint>& _MainGUI::GetCurrentBPList() {
	return (this->SearchTB->TextLength > 0) ? *this->TempBPTable : *this->Blueprints;
}

void _MainGUI::UpdateBlueprintLabel(bool bp_loading) {
	std::vector<SMBC::Blueprint>& cur_list = this->GetCurrentBPList();
	bool _visible = (cur_list.empty() || bp_loading);

	if (this->BPListStatus_LBL->Visible != _visible)
		this->BPListStatus_LBL->Visible = _visible;

	if (!_visible) return;

	System::String^ old_label = this->BPListStatus_LBL->Text;

	if (this->SearchTB->TextLength > 0)
		this->BPListStatus_LBL->Text = L"No Results";
	else
		this->BPListStatus_LBL->Text = L"No Blueprints";

	if ((this->BPListStatus_LBL->Text != old_label) || _visible)
		this->UpdateLabelPosition();
}

System::Void _MainGUI::BlueprintList_SizeChanged(System::Object^ sender, System::EventArgs^ e) {
	if (!this->BPListStatus_LBL->Visible) return;

	this->UpdateLabelPosition();
}

void _MainGUI::UpdateLabelPosition() {
	int p_x = (this->BlueprintList->Location.X + this->BlueprintList->Size.Width) / 2;
	int p_y = (this->BlueprintList->Location.Y + this->BlueprintList->Size.Height) / 2;

	this->BPListStatus_LBL->Location = System::Drawing::Point(
		p_x - (this->BPListStatus_LBL->Size.Width / 2),
		p_y - (this->BPListStatus_LBL->Size.Height / 2)
	);
}

System::Void _MainGUI::BlueprintList_MouseDown(
	System::Object^ sender,
	System::Windows::Forms::MouseEventArgs^ e
) {
	if (e->Button != System::Windows::Forms::MouseButtons::Right) return;

	int idx = this->BlueprintList->IndexFromPoint(e->X, e->Y);
	if (idx <= -1) return;

	this->BlueprintList->SetSelected(idx, true);
}

System::Void _MainGUI::MakeFormCentered(System::Windows::Forms::Form^ form) {
	form->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
	form->Location = System::Drawing::Point(
		(this->Location.X + this->Size.Width / 2) - (form->Size.Width / 2),
		(this->Location.Y + this->Size.Height / 2) - (form->Size.Height / 2)
	);
}