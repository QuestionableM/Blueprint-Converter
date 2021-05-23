#include "Gui/ModList.h"

#include "Lib/Json/JsonFunc.h"
#include "Lib/File/FileFunc.h"
#include "Lib/GuiLib/GuiLib.h"
#include "Lib/ProgramSettings.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include <msclr/marshal_cppstd.h>

namespace SMBC {
	using namespace BlueprintConverter;
}

typedef SMBC::ModList _ModListGUI;

static int ProgressCounter = 0;
static int ProgressMaxCounter = 0;

_ModListGUI::ModList(const SMBC::Blueprint& blueprint) {
	this->InitializeComponent();

	ProgressCounter = 0;
	ProgressMaxCounter = 0;
	this->usedModList = new std::vector<SMBC::ModListData>();
	this->BPName_LBL->Text = gcnew System::String((L"Blueprint Name: " + blueprint.BlueprintName).c_str());

	this->GuiUpdater->Start();
	this->ModSearcher_BW->RunWorkerAsync(gcnew System::String(blueprint.BlueprintFolder.c_str()));
}

_ModListGUI::~ModList() {
	this->usedModList->clear();
	delete this->usedModList;
	if (components) delete components;
}

System::Void _ModListGUI::ModSearcher_BW_DoWork(
	System::Object^ sender,
	System::ComponentModel::DoWorkEventArgs^ e
) {
	System::String^ BP_PathS = safe_cast<System::String^>(e->Argument);
	std::wstring BP_Path = msclr::interop::marshal_as<std::wstring>(BP_PathS);

	std::wstring BP_Json_path = BP_Path + L"/blueprint.json";

	nlohmann::json bp_json;
	if (!SMBC::JSON::OpenParseJson(BP_Json_path, bp_json)) {
		e->Result = 1;
		return;
	}

	auto& bp_data = bp_json;

	auto& bodies = bp_data["bodies"];
	if (bodies.is_array()) {
		ProgressMaxCounter += (int)bodies.size();

		for (auto& body : bodies) {
			auto& childs = body["childs"];

			if (!childs.is_array()) continue;

			for (auto& child : childs) {
				ProgressCounter++;

				auto& uuid = child["shapeId"];
				if (!uuid.is_string()) continue;

				std::wstring UuidWstr = SMBC::Other::Utf8ToWide(uuid.get<std::string>());
				int _ModIdx = this->FindModByObjUuid(UuidWstr);

				this->AddModToList(_ModIdx);
			}
		}
	}

	auto& joints = bp_data["joints"];

	if (joints.is_array()) {
		ProgressMaxCounter += (int)joints.size();

		for (auto& joint : joints) {
			ProgressCounter++;

			auto& uuid = joint["shapeId"];
			if (!uuid.is_string()) continue;

			std::wstring UuidWstr = SMBC::Other::Utf8ToWide(uuid.get<std::string>());
			int _ModIdx = this->FindModByObjUuid(UuidWstr);

			this->AddModToList(_ModIdx);
		}
	}
}

System::Void _ModListGUI::ModSearcher_BW_RunWorkerCompleted(
	System::Object^ sender,
	System::ComponentModel::RunWorkerCompletedEventArgs^ e
) {
	this->GuiUpdater->Stop();

	this->ModSearchProgress->Maximum = ProgressMaxCounter;
	this->ModSearchProgress->Value = (this->ModSearchProgress->Maximum < ProgressCounter) ? this->ModSearchProgress->Maximum : ProgressCounter;

	this->ModCount_LBL->Text = gcnew System::String((L"Amount of Mods: " + std::to_wstring(this->usedModList->size())).c_str());
	this->ObjectCount_LBL->Text = gcnew System::String((L"Amount of Objects: " + std::to_wstring(ProgressCounter)).c_str());

	this->ModList_LB->BeginUpdate();
	for (SMBC::ModListData& mod_list : *this->usedModList) {
		SMBC::ModData& _Mod = SMBC::ObjectDatabase::ModDB[mod_list.mod_index];

		std::wstring _ModName = (mod_list.mod_index > -1) ? _Mod.name : L"UNKNOWN_MOD";

		this->ModList_LB->Items->Add(gcnew System::String((_ModName + L" (" + std::to_wstring(mod_list.used_parts) + L")").c_str()));
	}
	this->ModList_LB->EndUpdate();

	this->ModList_LB->Enabled = true;
}

System::Void _ModListGUI::GuiUpdater_Tick(System::Object^ sender, System::EventArgs^ e) {
	this->ModCount_LBL->Text = gcnew System::String((L"Amount of Mods: " + std::to_wstring(this->usedModList->size())).c_str());
	this->ObjectCount_LBL->Text = gcnew System::String((L"Amount of Objects: " + std::to_wstring(ProgressCounter)).c_str());

	int _ProgressCopy = ProgressCounter;
	this->ModSearchProgress->Maximum = ProgressMaxCounter;
	if (_ProgressCopy > this->ModSearchProgress->Maximum)
		_ProgressCopy = this->ModSearchProgress->Maximum;

	this->ModSearchProgress->Value = _ProgressCopy;
}

int _ModListGUI::FindModByObjUuid(const std::wstring& uuid) {
	for (int idx = 0; idx < (int)SMBC::ObjectDatabase::ModDB.size(); idx++) {
		SMBC::ModData& mod = SMBC::ObjectDatabase::ModDB[idx];

		for (SMBC::BlockData& block : mod.BlockDB)
			if (block._obj_uuid == uuid) return idx;

		for (SMBC::ObjectData& obj : mod.ObjectDB)
			if (obj._obj_uuid == uuid) return idx;
	}

	return -1;
}

void _ModListGUI::AddModToList(const int& idx) {
	for (SMBC::ModListData& list_data : *this->usedModList)
		if (list_data.mod_index == idx) {
			list_data.used_parts++;
			return;
		}

	SMBC::ModListData _NewListData;
	_NewListData.mod_index = idx;
	_NewListData.used_parts = 1;

	this->usedModList->push_back(_NewListData);
}

System::Void _ModListGUI::ModList_FormClosing(
	System::Object^ sender,
	System::Windows::Forms::FormClosingEventArgs^ e
) {
	if (this->ModSearcher_BW->IsBusy) e->Cancel = true;
}

System::Void _ModListGUI::ModList_LB_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	bool _WorkshopIdExists = false;
	bool _PathExists = false;

	SMBC::ModData CurMod;
	if (this->GetCurrentMod(CurMod)) {
		_WorkshopIdExists = !CurMod.workshop_id.empty();
		_PathExists = !CurMod.path.empty();
	}

	this->OpenInWorkshop_BTN->Enabled = _WorkshopIdExists;
	this->OpenInFileExplorer_BTN->Enabled = _PathExists;
}

System::Void _ModListGUI::OpenInWorkshop_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	SMBC::ModData CurMod;
	if (!this->GetCurrentMod(CurMod)) return;

	if (CurMod.workshop_id.empty()) {
		SMBC::GUI::Error("Error", "Couldn't open the workshop link to the specified blueprint!");
		return;
	}

	std::wstring _WorkshopLink;
	if (SMBC::Settings::OpenLinksInSteam) _WorkshopLink.append(L"steam://openurl/");
	_WorkshopLink.append(L"https://steamcommunity.com/sharedfiles/filedetails/?id=");
	_WorkshopLink.append(CurMod.workshop_id);

	System::Diagnostics::Process::Start(gcnew System::String(_WorkshopLink.c_str()));
}

System::Void _ModListGUI::OpenInFileExplorer_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	SMBC::ModData CurMod;
	if (!this->GetCurrentMod(CurMod)) return;

	if (!SMBC::FILE::FileExists(CurMod.path)) {
		SMBC::GUI::Error(L"Internal Error", L"The path to specified mod directory doesn't exist!");
		return;
	}

	std::wstring path_cpy = CurMod.path;
	SMBC::PathReplacer::ReplaceAll(path_cpy, L'/', L'\\');

	SMBC::GUI::OpenFolderInExplorer(path_cpy);
}

bool _ModListGUI::GetCurrentMod(SMBC::ModData& mod) {
	int _index = this->ModList_LB->SelectedIndex;
	if (_index <= -1) return false;

	SMBC::ModListData& _ModList = this->usedModList->at(_index);
	if (_ModList.mod_index <= -1) return false;

	mod = SMBC::ObjectDatabase::ModDB[_ModList.mod_index];
	return true;
}