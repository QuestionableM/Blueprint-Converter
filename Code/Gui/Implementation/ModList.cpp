#include "Gui/ModList.h"

#include "ObjectDatabase\KeywordReplacer.h"

#include "Lib\ProgramSettings.h"
#include "Lib\GuiLib.h"
#include "Lib\String.h"
#include "Lib\Json.h"
#include "Lib\File.h"

#include <msclr/marshal_cppstd.h>

namespace BlueprintConverter
{
	static int ProgressCounter = 0;
	static int ProgressMaxCounter = 0;

	ModList::ModList(const SMBC::Blueprint* blueprint)
	{
		this->InitializeComponent();

		ProgressCounter = 0;
		ProgressMaxCounter = 0;
		this->UsedModData   = new std::unordered_map<std::string, ModListData*>();
		this->UsedModVector = new std::vector<ModListData*>();
		this->BPName_LBL->Text = gcnew System::String((L"Blueprint Name: " + blueprint->Name).c_str());

		this->GuiUpdater->Start();
		this->ModSearcher_BW->RunWorkerAsync(gcnew System::String(blueprint->Folder.c_str()));
	}

	ModList::~ModList()
	{
		this->UsedModVector->clear();
		this->UsedModData->clear();

		delete this->UsedModData;
		delete this->UsedModVector;

		if (components) delete components;
	}

	void ModList::ModSearcher_BW_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
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

					SMBC::Uuid uuid_obj(uuid.get<std::string>());
					SMBC::Mod* _ModPtr = this->FindModByObjUuid(uuid_obj);

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

				SMBC::Uuid uuid_obj(uuid.get<std::string>());
				SMBC::Mod* _ModPtr = this->FindModByObjUuid(uuid_obj);

				this->AddModToList(_ModPtr);
			}
		}
	}

	void ModList::ModSearcher_BW_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
	{
		this->GuiUpdater->Stop();

		this->ModSearchProgress->Maximum = ProgressMaxCounter;
		this->ModSearchProgress->Value = (this->ModSearchProgress->Maximum < ProgressCounter) ? this->ModSearchProgress->Maximum : ProgressCounter;

		this->ModCount_LBL->Text = gcnew System::String((L"Amount of Mods: " + std::to_wstring(this->UsedModData->size())).c_str());
		this->ObjectCount_LBL->Text = gcnew System::String((L"Amount of Objects: " + std::to_wstring(ProgressCounter)).c_str());

		if (e->Result != nullptr)
		{
			int result_idx = safe_cast<int>(e->Result);

			System::String^ ErrorMsg;

			switch (result_idx)
			{
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

		{
			this->ModList_LB->BeginUpdate();

			for (const ModListData* mod_list : *this->UsedModVector)
			{
				const std::wstring mModName = (mod_list->ptr != nullptr) ? mod_list->ptr->m_Name : L"UNKNOWN_MOD";

				this->ModList_LB->Items->Add(gcnew System::String((mModName + L" (" + std::to_wstring(mod_list->used_parts) + L")").c_str()));
			}

			this->ModList_LB->EndUpdate();
			this->ModList_LB->Enabled = true;
		}
	}

	void ModList::GuiUpdater_Tick(System::Object^ sender, System::EventArgs^ e)
	{
		this->ModCount_LBL->Text = gcnew System::String((L"Amount of Mods: " + std::to_wstring(this->UsedModData->size())).c_str());
		this->ObjectCount_LBL->Text = gcnew System::String((L"Amount of Objects: " + std::to_wstring(ProgressCounter)).c_str());

		int _ProgressCopy = ProgressCounter;
		this->ModSearchProgress->Maximum = ProgressMaxCounter;
		if (_ProgressCopy > this->ModSearchProgress->Maximum)
			_ProgressCopy = this->ModSearchProgress->Maximum;

		this->ModSearchProgress->Value = _ProgressCopy;
	}

	SMBC::Mod* ModList::FindModByObjUuid(const SMBC::Uuid& uuid)
	{
		const SMBC::ObjectData* cur_object = SMBC::Mod::GetObject(uuid);
		if (!cur_object) return nullptr;

		return cur_object->ModPtr;
	}

	void ModList::AddModToList(SMBC::Mod* ModData)
	{
		const std::string mUuidStr = (ModData != nullptr) ? ModData->m_Uuid.ToString() : "UnknownMod";

		if (this->UsedModData->find(mUuidStr) != this->UsedModData->end())
		{
			ModListData*& lData_Ptr = this->UsedModData->at(mUuidStr);
			lData_Ptr->used_parts++;

			return;
		}

		ModListData* new_listData = new ModListData();
		new_listData->ptr = ModData;
		new_listData->used_parts = 1;

		this->UsedModData->insert(std::make_pair(mUuidStr, new_listData));
		this->UsedModVector->push_back(new_listData);
	}

	void ModList::ModList_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e)
	{
		if (this->ModSearcher_BW->IsBusy)
			e->Cancel = true;
	}

	void ModList::ModList_LB_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		int selected_idx = this->ModList_LB->SelectedIndex;
		if (this->selected_mod == selected_idx) return;
		this->selected_mod = selected_idx;

		bool workshop_id_exists = false;
		bool path_exists        = false;

		SMBC::Mod* pCurMod = this->GetCurrentMod();
		if (pCurMod != nullptr)
		{
			workshop_id_exists = (pCurMod->m_WorkshopId != 0);
			path_exists = !pCurMod->m_Directory.empty();
		}

		this->OpenInWorkshop_BTN->Enabled = workshop_id_exists;
		this->OpenInFileExplorer_BTN->Enabled = path_exists;
	}

	void ModList::OpenInWorkshop_BTN_Click(System::Object^ sender, System::EventArgs^ e)
	{
		SMBC::Mod* pCurMod = this->GetCurrentMod();
		if (pCurMod == nullptr) return;

		if (pCurMod->m_WorkshopId == 0ull)
		{
			SMBC::Gui::Error("Error", "Couldn't open the workshop link to the specified blueprint!");
			return;
		}

		std::wstring lWorkshopLink;
		if (SMBC::Settings::OpenLinksInSteam) lWorkshopLink.append(L"steam://openurl/");
		SMBC::String::Combine(lWorkshopLink, L"https://steamcommunity.com/sharedfiles/filedetails/?id=", std::to_string(pCurMod->m_WorkshopId));

		System::Diagnostics::Process::Start(gcnew System::String(lWorkshopLink.c_str()));
	}

	void ModList::OpenInFileExplorer_BTN_Click(System::Object^ sender, System::EventArgs^ e)
	{
		SMBC::Mod* pCurMod = this->GetCurrentMod();
		if (pCurMod == nullptr) return;

		if (!SMBC::File::Exists(pCurMod->m_Directory))
		{
			SMBC::Gui::Error(L"Internal Error", L"The path to specified mod directory doesn't exist!");
			return;
		}

		const std::wstring path_cpy = SMBC::String::Replace(pCurMod->m_Directory, L'/', L'\\');

		SMBC::Gui::OpenFolderInExplorer(path_cpy);
	}

	SMBC::Mod* ModList::GetCurrentMod()
	{
		int _index = this->ModList_LB->SelectedIndex;
		if (_index <= -1) return nullptr;

		ModListData*& _ModList = this->UsedModVector->at(_index);

		return _ModList->ptr;
	}
}