#include "Gui/ModList.h"

#include "Lib/Json/JsonFunc.h"
#include "Lib/File/FileFunc.h"
#include "Lib/GuiLib/GuiLib.h"
#include "Lib/ProgramSettings.h"
#include "Lib/String/String.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include <msclr/marshal_cppstd.h>

namespace SMBC
{
	using namespace BlueprintConverter;
}

typedef SMBC::ModList _ModListGUI;

static int ProgressCounter = 0;
static int ProgressMaxCounter = 0;

_ModListGUI::ModList(const SMBC::Blueprint& blueprint)
{
	this->InitializeComponent();

	ProgressCounter = 0;
	ProgressMaxCounter = 0;
	this->usedModList = new std::vector<SMBC::ModListData>();
	this->BPName_LBL->Text = gcnew System::String((L"Blueprint Name: " + blueprint.Name).c_str());

	this->GuiUpdater->Start();
	this->ModSearcher_BW->RunWorkerAsync(gcnew System::String(blueprint.Folder.c_str()));
}

_ModListGUI::~ModList()
{
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

	nlohmann::json bp_json = SMBC::Json::LoadParseJson(BP_Json_path);
	if (!bp_json.is_object())
	{
		e->Result = 1;
		return;
	}

	const auto& bodies = SMBC::Json::Get(bp_json, "bodies");
	if (bodies.is_array())
	{
		ProgressMaxCounter += (int)bodies.size();

		for (auto& body : bodies)
		{
			const auto& childs = SMBC::Json::Get(body, "childs");

			if (!childs.is_array()) continue;

			for (auto& child : childs)
			{
				ProgressCounter++;

				const auto& uuid = SMBC::Json::Get(child, "shapeId");
				if (!uuid.is_string()) continue;

				std::wstring UuidWstr = SMBC::String::ToWide(uuid.get<std::string>());
				SMBC::Mod* _ModPtr = this->FindModByObjUuid(UuidWstr);

				this->AddModToList(_ModPtr);
			}
		}
	}

	const auto& joints = SMBC::Json::Get(bp_json, "joints");
	if (joints.is_array())
	{
		ProgressMaxCounter += (int)joints.size();

		for (auto& joint : joints)
		{
			ProgressCounter++;

			const auto& uuid = SMBC::Json::Get(joint, "shapeId");
			if (!uuid.is_string()) continue;

			std::wstring UuidWstr = SMBC::String::ToWide(uuid.get<std::string>());
			SMBC::Mod* _ModPtr = this->FindModByObjUuid(UuidWstr);

			this->AddModToList(_ModPtr);
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

	if (e->Result != nullptr)
	{
		int result_idx = safe_cast<int>(e->Result);

		System::String^ ErrorMsg;

		switch (result_idx) {
		case 1:
			ErrorMsg = L"Couldn't parse the specified blueprint file!";
			break;
		default:
			ErrorMsg = L"Unknown Error";
			break;
		}

		SMBC::Gui::Error(L"Error", ErrorMsg);
		this->Close();
		return;
	}

	this->ModList_LB->BeginUpdate();
	for (SMBC::ModListData& mod_list : *this->usedModList)
	{
		std::wstring _ModName = L"UNKNOWN_MOD";

		if (mod_list.ptr != nullptr)
			_ModName = mod_list.ptr->Name;

		this->ModList_LB->Items->Add(gcnew System::String((_ModName + L" (" + std::to_wstring(mod_list.used_parts) + L")").c_str()));
	}
	this->ModList_LB->EndUpdate();

	this->ModList_LB->Enabled = true;
}

System::Void _ModListGUI::GuiUpdater_Tick(System::Object^ sender, System::EventArgs^ e)
{
	this->ModCount_LBL->Text = gcnew System::String((L"Amount of Mods: " + std::to_wstring(this->usedModList->size())).c_str());
	this->ObjectCount_LBL->Text = gcnew System::String((L"Amount of Objects: " + std::to_wstring(ProgressCounter)).c_str());

	int _ProgressCopy = ProgressCounter;
	this->ModSearchProgress->Maximum = ProgressMaxCounter;
	if (_ProgressCopy > this->ModSearchProgress->Maximum)
		_ProgressCopy = this->ModSearchProgress->Maximum;

	this->ModSearchProgress->Value = _ProgressCopy;
}

SMBC::Mod* _ModListGUI::FindModByObjUuid(const std::wstring& uuid)
{
	const SMBC::ObjectData* cur_object = SMBC::Mod::GetObject(uuid);
	if (!cur_object) return nullptr;
	
	return cur_object->ModPtr;
}

void _ModListGUI::AddModToList(SMBC::Mod* ModData)
{
	for (SMBC::ModListData& list_data : *this->usedModList)
		if (list_data.ptr == ModData)
		{
			list_data.used_parts++;
			return;
		}

	this->usedModList->push_back({ModData, 1});
}

System::Void _ModListGUI::ModList_FormClosing(
	System::Object^ sender,
	System::Windows::Forms::FormClosingEventArgs^ e
) {
	if (this->ModSearcher_BW->IsBusy)
		e->Cancel = true;
}

System::Void _ModListGUI::ModList_LB_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
	int sel_idx = this->ModList_LB->SelectedIndex;
	if (this->selected_mod == sel_idx) return;
	this->selected_mod = sel_idx;
	
	bool _WorkshopIdExists = false;
	bool _PathExists = false;

	SMBC::Mod* CurMod = this->GetCurrentMod();
	if (CurMod != nullptr)
	{
		_WorkshopIdExists = !CurMod->WorkshopId.empty();
		_PathExists = !CurMod->Path.empty();
	}

	this->OpenInWorkshop_BTN->Enabled = _WorkshopIdExists;
	this->OpenInFileExplorer_BTN->Enabled = _PathExists;
}

System::Void _ModListGUI::OpenInWorkshop_BTN_Click(System::Object^ sender, System::EventArgs^ e)
{
	SMBC::Mod* CurMod = this->GetCurrentMod();
	if (CurMod == nullptr) return;

	if (CurMod->WorkshopId.empty())
	{
		SMBC::Gui::Error("Error", "Couldn't open the workshop link to the specified blueprint!");
		return;
	}

	std::wstring _WorkshopLink;
	if (SMBC::Settings::OpenLinksInSteam) _WorkshopLink.append(L"steam://openurl/");
	SMBC::String::Combine(_WorkshopLink, L"https://steamcommunity.com/sharedfiles/filedetails/?id=", CurMod->WorkshopId);

	System::Diagnostics::Process::Start(gcnew System::String(_WorkshopLink.c_str()));
}

System::Void _ModListGUI::OpenInFileExplorer_BTN_Click(System::Object^ sender, System::EventArgs^ e)
{
	SMBC::Mod* CurMod = this->GetCurrentMod();
	if (CurMod == nullptr) return;

	if (!SMBC::File::Exists(CurMod->Path))
	{
		SMBC::Gui::Error(L"Internal Error", L"The path to specified mod directory doesn't exist!");
		return;
	}

	std::wstring path_cpy = CurMod->Path;
	SMBC::PathReplacer::ReplaceAll(path_cpy, L'/', L'\\');

	SMBC::Gui::OpenFolderInExplorer(path_cpy);
}

SMBC::Mod* _ModListGUI::GetCurrentMod()
{
	int _index = this->ModList_LB->SelectedIndex;
	if (_index <= -1) return nullptr;

	SMBC::ModListData& _ModList = this->usedModList->at(_index);

	return _ModList.ptr;
}