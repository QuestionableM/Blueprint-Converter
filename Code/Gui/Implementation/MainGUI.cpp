#include <msclr/marshal_cppstd.h>
#include <filesystem>
#include <cwctype>

#include "BlueprintConverter\BlueprintConverter.h"
#include "BlueprintConverter\ConvertSettings.h"

#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\MaterialManager.h"
#include "ObjectDatabase\KeywordReplacer.h"
#include "ObjectDatabase\DatabaseLoader.h"

#include "Lib\ProgramSettings.h"
#include "Lib\ConvData.h"
#include "Lib\GuiLib.h"
#include "Lib\String.h"
#include "Lib\File.h"
#include "Lib\Json.h"

#include "Gui/GeneratorSettings.h"
#include "Gui/SettingsGUI.h"
#include "Gui/ModList.h"
#include "Gui/MainGUI.h"
#include "Gui/About.h"

#include "DebugCon.h"

namespace fs = std::filesystem;

namespace BlueprintConverter
{
	MainGUI::MainGUI()
	{
		{
			//Configure the Gui settings

			this->InitializeComponent();
			this->Blueprints = new std::vector<SMBC::Blueprint*>();
			this->TempBPTable = new std::vector<SMBC::Blueprint*>();
			this->BlueprintImage->WaitOnLoad = true;
			this->ActionProgress->Minimum = 0;
			this->GuiUpdater->Start();
		}

	#if _DEBUG
		this->Text += " DEBUG";
	#endif

		{
			//Send messages to text fields

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
		}

		SMBC::MaterialManager::Initialize();
		SMBC::Rotations::InitializeRotations();
	}

	MainGUI::~MainGUI()
	{
		this->Blueprints->clear();
		this->TempBPTable->clear();

		delete this->Blueprints;
		delete this->TempBPTable;

		if (this->components) delete this->components;
	}

	void MainGUI::BPPath_TB_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->Start_BTN->Enabled = (BPPath_TB->TextLength > 0);
	}

	void MainGUI::Start_BTN_Click(System::Object^ sender, System::EventArgs^ e)
	{
		std::wstring _BlueprintPath = msclr::interop::marshal_as<std::wstring>(this->BPPath_TB->Text);
		if (!SMBC::File::Exists(_BlueprintPath))
		{
			SMBC::Gui::Warning("Invalid path", "The specified path doesn't exist!");
			return;
		}

		std::wstring _BlueprintFile = L"";
		std::wstring _BlueprintName = L"";
		fs::directory_entry _DirEntr(_BlueprintPath);

		if (_DirEntr.is_directory())
		{
			std::wstring _BPFileDesc = (_BlueprintPath + L"/description.json");

			nlohmann::json _DescrJson = SMBC::Json::LoadParseJson(_BPFileDesc);
			if (!_DescrJson.is_object())
			{
				SMBC::Gui::Warning("Parse Error", "Couldn't parse \"description.json\"");
				return;
			}

			std::wstring _BPType = SMBC::Json::GetWstr(_DescrJson, "type");
			std::wstring _BPName = SMBC::Json::GetWstr(_DescrJson, "name");

			if (_BPType != L"Blueprint" || _BPName.empty())
			{
				SMBC::Gui::Warning("No Data", "The specified folder does not contain any information about the blueprint!");
				return;
			}

			_BlueprintFile = (_BlueprintPath + L"/blueprint.json");
			_BlueprintName = SMBC::Blueprint::FixBlueprintName(_BPName);

			if (_BlueprintFile.empty() || _BlueprintName.empty()) return;
		}
		else if (_DirEntr.is_regular_file())
		{
			_BlueprintFile = _BlueprintPath;
			if (_DirEntr.path().has_stem())
				_BlueprintName = SMBC::Blueprint::FixBlueprintName(_DirEntr.path().stem().wstring());
			else
				_BlueprintName = L"UnknownBlueprint";
		}
		else
		{
			SMBC::Gui::Warning("Invalid path", "Unknown file type");
			return;
		}

		if (!_BlueprintName.empty() && !_BlueprintFile.empty())
		{
			BlueprintConverter::GeneratorSettings^ pGenSettings = gcnew BlueprintConverter::GeneratorSettings(_BlueprintName);
			this->MakeFormCentered(pGenSettings);
			pGenSettings->ShowDialog();

			if (pGenSettings->BlueprintName_TB->TextLength > 0 && pGenSettings->Success)
			{
				this->ChangeGUIState(true, true, false);

				System::Array^ _ThreadData = gcnew cli::array<System::Object^>(8);

				_ThreadData->SetValue(gcnew System::String(_BlueprintFile.c_str()), (int)0);
				_ThreadData->SetValue(pGenSettings->BlueprintName_TB->Text, (int)1);
				_ThreadData->SetValue(pGenSettings->SeparationType_CB->SelectedIndex, (int)2);
				_ThreadData->SetValue(pGenSettings->ApplyTextures_CB->Checked, (int)3);
				_ThreadData->SetValue(pGenSettings->ExportTexPaths_CB->Checked, (int)4);
				_ThreadData->SetValue(pGenSettings->MaterialsByColor_CB->Checked, (int)5);
				_ThreadData->SetValue(pGenSettings->ExportNormals_CB->Checked, (int)6);
				_ThreadData->SetValue(pGenSettings->ExportUVs_CB->Checked, (int)7);

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

	void MainGUI::ObjectGenerator_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		System::Array^ _Data = safe_cast<System::Array^>(e->Argument);

		System::String^ _BlueprintPathS = safe_cast<System::String^>(_Data->GetValue((int)0));
		System::String^ _BlueprintNameS = safe_cast<System::String^>(_Data->GetValue((int)1));

		int sSeparationMethod = safe_cast<int>(_Data->GetValue((int)2));

		bool sApplyTextures = safe_cast<bool>(_Data->GetValue((int)3));
		bool sExportTexPaths = safe_cast<bool>(_Data->GetValue((int)4));
		bool sMaterialsByColor = safe_cast<bool>(_Data->GetValue((int)5));
		bool sExportNormals = safe_cast<bool>(_Data->GetValue((int)6));
		bool sExportUvs = safe_cast<bool>(_Data->GetValue((int)7));

		std::wstring _BlueprintPath = msclr::interop::marshal_as<std::wstring>(_BlueprintPathS);
		std::wstring _BlueprintName = msclr::interop::marshal_as<std::wstring>(_BlueprintNameS);

		SMBC::ConvertSettings::SeparationMethod = sSeparationMethod;
		SMBC::ConvertSettings::ApplyTextures = sApplyTextures && sExportUvs;
		SMBC::ConvertSettings::TextureList = sExportTexPaths;
		SMBC::ConvertSettings::MatByColor = sMaterialsByColor && sApplyTextures && sExportUvs;
		SMBC::ConvertSettings::ExportNormals = sExportNormals;
		SMBC::ConvertSettings::ExportUvs = sExportUvs;

		SMBC::ConvertError cError;
		SMBC::BPFunction::ConvertBlueprintToObj(_BlueprintPath, _BlueprintName, cError);

		System::Array^ rArray = nullptr;
		if (cError)
		{
			rArray = gcnew cli::array<System::Object^>(2);
			rArray->SetValue(true, (int)0);
			rArray->SetValue(gcnew System::String(cError.GetString().c_str()), (int)1);
		}
		else
		{
			rArray = gcnew cli::array<System::Object^>(1);
			rArray->SetValue(false, (int)0);
		}

		e->Result = rArray;
	}

	void MainGUI::GuiUpdater_Tick(System::Object^ sender, System::EventArgs^ e)
	{
		if (SMBC::ConvData::State == SMBC::ConvState::None) return;

		std::wstring state_output = SMBC::ConvData::GetStateString();
		if (SMBC::ConvData::StateHasNumbers())
		{
			std::size_t max_value = SMBC::ConvData::ProgressMax;
			std::size_t cur_value = SMBC::ConvData::ProgressValue;

			this->ActionProgress->Maximum = (int)max_value;

			std::size_t max_cast = this->ActionProgress->Maximum;
			if (max_cast <cur_value)
			{
				cur_value = max_cast;
			}

			this->ActionProgress->Value = (int)cur_value;

			state_output += (L"(" + std::to_wstring(cur_value) + L" / " + std::to_wstring(max_value) + L")");
		}

		this->ActionLabel->Text = gcnew System::String(state_output.c_str());
	}

	void MainGUI::ObjectGenerator_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
	{
		this->GuiUpdater->Stop();
		this->GuiUpdater_Tick(nullptr, nullptr);

		this->ActionProgress->Value = this->ActionProgress->Maximum;

		System::Array^ res_array = safe_cast<System::Array^>(e->Result);
		const bool has_errors = safe_cast<bool>(res_array->GetValue((int)0));

		if (has_errors)
		{
			SMBC::ConvData::SetState(SMBC::ConvState::CV_Failure, 0);

			System::String^ mErrorMsg = safe_cast<System::String^>(res_array->GetValue((int)1));

			SMBC::Gui::Error("Conversion Error", mErrorMsg);
		}
		else
		{
			SMBC::ConvData::SetState(SMBC::ConvState::CV_Success, 0);

			SMBC::Gui::Message(
				"Success",
				"Successfully finished generating a 3D model!",
				System::Windows::Forms::MessageBoxButtons::OK,
				System::Windows::Forms::MessageBoxIcon::Information
			);
		}

		this->ChangeGUIState(true, true, true);
		this->ActionProgress->Value = 0;

		this->GuiUpdater_Tick(nullptr, nullptr);
	}

	void MainGUI::DatabaseLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		SMBC::DatabaseLoader::LoadDatabase();
	}

	void MainGUI::DatabaseLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
	{
		SMBC::ConvData::SetState(SMBC::ConvState::None, 0);

		this->GuiUpdater->Stop();
		this->ActionProgress->Value = 0;

		this->ActionLabel->Text = gcnew System::String((L"Successfully loaded " + std::to_wstring(SMBC::Mod::GetObjectCount()) + L" objects from " + std::to_wstring(SMBC::Mod::GetModCount()) + L" mods").c_str());
		this->ChangeGUIState(this->LoadedBP, true, true);
	}

	void MainGUI::BlueprintLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		std::error_code mError;

		for (const std::wstring& BlueprintFolder : SMBC::Settings::BlueprintFolders)
		{
			if (!SMBC::File::Exists(BlueprintFolder)) continue;

			fs::directory_iterator BPDirIter(BlueprintFolder, fs::directory_options::skip_permission_denied, mError);
			for (const auto& Folder : BPDirIter)
			{
				if (mError)
				{
					DebugErrorL("Couldn't get an item in: ", BlueprintFolder);
					continue;
				}

				if (!Folder.is_directory()) continue;

				SMBC::Blueprint* new_blueprint = SMBC::Blueprint::CreateBlueprintFromDirectory(Folder.path().wstring());
				if (!new_blueprint) continue;

				this->Blueprints->push_back(new_blueprint);
			}
		}
	}

	void MainGUI::BlueprintLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
	{
		this->BlueprintList->BeginUpdate();

		this->BlueprintList->Items->Clear();
		for (SMBC::Blueprint*& bpObject : *this->Blueprints)
			this->BlueprintList->Items->Add(gcnew System::String(bpObject->Name.c_str()));

		this->BlueprintList->EndUpdate();

		this->ChangeGUIState(true, this->LoadedDatabase, true);
		this->UpdateBlueprintLabel(false);
	}

	void MainGUI::BlueprintList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		const int Index = this->BlueprintList->SelectedIndex;
		if (Index <= -1 || Index == this->SelItemIndex) return;
		this->SelItemIndex = Index;

		SMBC::Blueprint* cur_blueprint = this->GetCurrentBlueprint();
		if (!cur_blueprint)
		{
			SMBC::Gui::Error("Error", "Couldn't get the specified blueprint!");
			return;
		}

		if (!cur_blueprint->BlueprintExists())
		{
			SMBC::Gui::Warning(
				"Blueprint Doesn't Exist",
				"The blueprint you have just selected doesn't exist anymore!\n\nThe blueprint list will be automatically reloaded as soon as this message will be closed"
			);

			this->LoadBlueprints();
			return;
		}

		this->BP_OpenOutputDir_BTN->Enabled = !cur_blueprint->Folder.empty();
		this->BP_ShowModList_BTN->Enabled = !cur_blueprint->Path.empty();
		this->BlueprintOptions_CMS->Enabled = (!cur_blueprint->Folder.empty() || !cur_blueprint->Path.empty());
		this->OpenInWorkshop_BTN->Enabled = !cur_blueprint->WorkshopId.empty();

		if (this->BlueprintImage->Image)
		{
			delete this->BlueprintImage->Image;
			this->BlueprintImage->Image = nullptr;
		}
		this->BlueprintImage->Invalidate();

		if (!cur_blueprint->ImagePath.empty())
			this->BlueprintImage->ImageLocation = gcnew System::String(cur_blueprint->ImagePath.c_str());

		this->BPPath_TB->Text = gcnew System::String(cur_blueprint->Folder.c_str());
	}

	void MainGUI::SearchTB_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->BP_OpenOutputDir_BTN->Enabled = false;
		this->BP_ShowModList_BTN->Enabled = false;
		this->BlueprintOptions_CMS->Enabled = false;

		if (this->TempBPTable->size() > 0) this->TempBPTable->clear();

		this->SelItemIndex = -1;
		if (this->SearchTB->TextLength > 0 && !this->Blueprints->empty())
		{
			std::wstring l_SearchWstr = msclr::interop::marshal_as<std::wstring>(this->SearchTB->Text);
			SMBC::String::ToLowerR(l_SearchWstr);

			for (SMBC::Blueprint*& _Blueprint : *this->Blueprints)
			{
				if (_Blueprint->LowerName.find(l_SearchWstr) != std::wstring::npos)
					this->TempBPTable->push_back(_Blueprint);
			}
		}

		std::vector<SMBC::Blueprint*>* cur_list = this->GetCurrentBPList();
		if (!cur_list->empty() || (cur_list->empty() && this->BlueprintList->Items->Count > 0))
		{
			this->BlueprintList->BeginUpdate();
			this->BlueprintList->Items->Clear();

			for (SMBC::Blueprint*& _Blueprint : *cur_list)
				this->BlueprintList->Items->Add(gcnew System::String(_Blueprint->Name.c_str()));

			this->BlueprintList->EndUpdate();
		}

		this->UpdateBlueprintLabel(false);
	}

	void MainGUI::BlueprintList_EnabledChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->SearchTB->Enabled = this->BlueprintList->Enabled;
	}

	void MainGUI::aboutToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
	{
		BlueprintConverter::About^ AboutGUI = gcnew BlueprintConverter::About();
		this->MakeFormCentered(AboutGUI);
		AboutGUI->ShowDialog();
	}

	void MainGUI::LoadDatabase()
	{
		this->ChangeGUIState(this->LoadedBP, false, true);
		this->GuiUpdater->Start();
		this->DatabaseLoader->RunWorkerAsync();
	}

	void MainGUI::LoadBlueprints()
	{
		if (this->BlueprintImage->Image) delete this->BlueprintImage->Image;
		this->BlueprintImage->Image = nullptr;
		this->BlueprintImage->Invalidate();

		this->SelItemIndex = -1;
		this->ChangeGUIState(false, this->LoadedDatabase, true);
		this->SearchTB->Clear();
		this->TempBPTable->clear();

		{
			for (SMBC::Blueprint*& bp : *this->Blueprints)
				delete bp;

			this->Blueprints->clear();
		}

		this->UpdateBlueprintLabel(true);
		this->BPListStatus_LBL->Text = L"Loading Blueprints...";

		this->BlueprintLoader->RunWorkerAsync();
	}

	void MainGUI::reloadBlueprintListToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
	{
		this->LoadBlueprints();
	}

	void MainGUI::reloadObjectDatabaseToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
	{
		this->LoadDatabase();
	}

	void MainGUI::ChangeGUIState(bool bploaded, bool databaseloaded, bool bpgenerated)
	{
		this->LoadedBP = bploaded;
		this->LoadedDatabase = databaseloaded;

		bool dpandbp = (databaseloaded && bpgenerated);
		bool bplandbp = (bploaded && bpgenerated);

		this->BPPath_TB->Enabled = dpandbp;
		this->Start_BTN->Enabled = (dpandbp && this->BPPath_TB->TextLength > 0);

		if (dpandbp)
		{
			bool _HasWorkshopId = false;
			bool _HasBPFolder = false;
			bool _HasBPPath = false;

			SMBC::Blueprint* cur_bp = this->GetCurrentBlueprint();
			if (cur_bp != nullptr)
			{
				_HasWorkshopId = !cur_bp->WorkshopId.empty();
				_HasBPFolder = !cur_bp->Folder.empty();
				_HasBPPath = !cur_bp->Path.empty();
			}

			this->BlueprintOptions_CMS->Enabled = (_HasBPFolder || _HasBPPath);
			this->BP_OpenOutputDir_BTN->Enabled = _HasBPFolder;
			this->BP_ShowModList_BTN->Enabled = _HasBPPath;
			this->OpenInWorkshop_BTN->Enabled = _HasWorkshopId;
		}
		else
		{
			this->OpenInWorkshop_BTN->Enabled = false;
			this->BlueprintOptions_CMS->Enabled = false;
			this->BP_OpenOutputDir_BTN->Enabled = false;
			this->BP_ShowModList_BTN->Enabled = false;
		}

		this->OpenBlueprint->Enabled = dpandbp;
		this->OpenOutputFolder_BTN->Enabled = dpandbp;
		this->reloadBlueprintListToolStripMenuItem->Enabled = bplandbp;
		this->reloadObjectDatabaseToolStripMenuItem->Enabled = dpandbp;
		this->settingsToolStripMenuItem1->Enabled = bploaded && databaseloaded && bpgenerated;
		this->menuStrip1->Enabled = bpgenerated;

		bool _BothLoaded = (dpandbp && bplandbp);

		this->BlueprintList->Enabled = _BothLoaded;
	}

	void MainGUI::settingsToolStripMenuItem1_Click(System::Object^ sender, System::EventArgs^ e)
	{
		this->OpenOptionsGUI();
	}

	void MainGUI::OpenOptionsGUI()
	{
		SettingsGUI^ pSettings = gcnew SettingsGUI();
		this->MakeFormCentered(pSettings);
		pSettings->ShowDialog();

		if (pSettings->scrap_path_changed)
		{
			this->BlueprintList->Items->Clear();
			this->TempBPTable->clear();
			this->Blueprints->clear();

			SMBC::Settings::ReadConfig();

			this->GuiUpdater->Start();
			this->LoadDatabase();
			this->LoadBlueprints();
		}
		else
		{
			if (pSettings->blueprint_paths_changed) this->LoadBlueprints();
			if (pSettings->mod_paths_changed) this->LoadDatabase();
		}
	}

	void MainGUI::MainGUI_Shown(System::Object^ sender, System::EventArgs^ e)
	{
		const bool mShouldSave = SMBC::Settings::ReadConfig();

		if (SMBC::Settings::PathToSM.empty())
		{
			WForms::DialogResult dr = SMBC::Gui::Question(
				"Missing Path",
				"This program requires a path to Scrap Mechanic to work properly\n\nWould you like to set it up right now?"
			);

			if (dr == WForms::DialogResult::Yes)
				this->OpenOptionsGUI();
			else
				this->Close();

			return;
		}

		if (mShouldSave)
		{
			SMBC::Settings::SaveConfig();
		}

		this->LoadDatabase();
		this->LoadBlueprints();
	}

	void MainGUI::OpenBlueprint_Click(System::Object^ sender, System::EventArgs^ e)
	{
		std::wstring _FileName = SMBC::Gui::OpenFileName(
			L"Select a Blueprint File",
			0,
			L"All Files (*.*)\0*.*\0",
			static_cast<HWND>(this->Handle.ToPointer())
		);

		if (_FileName.empty()) return;

		this->BPPath_TB->Text = gcnew System::String(_FileName.c_str());
	}

	void MainGUI::OpenOutputFolder_BTN_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (SMBC::File::SafeCreateDir(L".\\Converted Models"))
			SMBC::Gui::OpenFolderInExplorer(L".\\Converted Models");
		else
			SMBC::Gui::Error("Error", "Couldn't open a directory with converted models.");
	}

	void MainGUI::MainGUI_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e)
	{
		if (this->ObjectGenerator->IsBusy)
		{
			WForms::DialogResult dr = SMBC::Gui::Question(
				"Close",
				"Are you sure that you want to close the program while it's converting a blueprint?\n\nThis might corrupt the file it's currently writing!"
			);

			if (dr != WForms::DialogResult::Yes)
				e->Cancel = true;
		}
	}

	void MainGUI::OpenInWorkshop_BTN_Click(System::Object^ sender, System::EventArgs^ e)
	{
		SMBC::Blueprint* cur_bp = this->GetCurrentBlueprint();

		if (cur_bp->WorkshopId.empty())
		{
			SMBC::Gui::Error("Error", "Couldn't open the workshop link to the specified blueprint!");
			return;
		}

		std::wstring _WorkshopLink;
		if (SMBC::Settings::OpenLinksInSteam)
			_WorkshopLink.append(L"steam://openurl/");

		SMBC::String::Combine(_WorkshopLink, L"https://steamcommunity.com/sharedfiles/filedetails/?id=", cur_bp->WorkshopId);

		System::Diagnostics::Process::Start(gcnew System::String(_WorkshopLink.c_str()));
	}

	void MainGUI::BP_OpenOutputDir_BTN_Click(System::Object^ sender, System::EventArgs^ e)
	{
		SMBC::Blueprint* cur_blueprint = this->GetCurrentBlueprint();
		if (!cur_blueprint) return;

		if (!SMBC::File::Exists(cur_blueprint->Folder))
		{
			SMBC::Gui::Error("Internal Error", "The path to the selected blueprint doesn't exist!");
			return;
		}

		std::wstring path_cpy = cur_blueprint->Folder;
		SMBC::String::ReplaceR(path_cpy, L'/', L'\\');

		SMBC::Gui::OpenFolderInExplorer(path_cpy);
	}

	SMBC::Blueprint* MainGUI::GetCurrentBlueprint()
	{
		const int cur_idx = this->BlueprintList->SelectedIndex;
		if (cur_idx <= -1) return nullptr;

		std::vector<SMBC::Blueprint*>* cur_list = this->GetCurrentBPList();
		if ((int)cur_list->size() < cur_idx) return nullptr;

		return cur_list->at(cur_idx);
	}

	void MainGUI::BP_ShowModList_BTN_Click(System::Object^ sender, System::EventArgs^ e)
	{
		SMBC::Blueprint* sel_blueprint = this->GetCurrentBlueprint();
		if (!sel_blueprint) return;

		if (!SMBC::File::Exists(sel_blueprint->Path))
		{
			SMBC::Gui::Warning(L"Invalid Blueprint", L"The path to specified blueprint doesn't exist anymore!");
			return;
		}

		ModList^ ModListGUI = gcnew ModList(sel_blueprint);
		this->MakeFormCentered(ModListGUI);
		ModListGUI->ShowDialog();

		delete ModListGUI;
	}

	std::vector<SMBC::Blueprint*>* MainGUI::GetCurrentBPList()
	{
		return (this->SearchTB->TextLength > 0) ? this->TempBPTable : this->Blueprints;
	}

	void MainGUI::UpdateBlueprintLabel(bool bp_loading)
	{
		std::vector<SMBC::Blueprint*>* cur_list = this->GetCurrentBPList();
		bool _visible = (cur_list->empty() || bp_loading);

		if (this->BPListStatus_LBL->Visible != _visible)
			this->BPListStatus_LBL->Visible = _visible;

		if (_visible)
		{
			System::String^ old_label = this->BPListStatus_LBL->Text;

			if (this->SearchTB->TextLength > 0)
				this->BPListStatus_LBL->Text = L"No Results";
			else
				this->BPListStatus_LBL->Text = L"No Blueprints";

			if ((this->BPListStatus_LBL->Text != old_label) || _visible)
				this->UpdateLabelPosition();
		}
	}

	void MainGUI::BlueprintList_SizeChanged(System::Object^ sender, System::EventArgs^ e)
	{
		if (!this->BPListStatus_LBL->Visible) return;

		this->UpdateLabelPosition();
	}

	void MainGUI::UpdateLabelPosition()
	{
		int p_x = (this->BlueprintList->Location.X + this->BlueprintList->Size.Width) / 2;
		int p_y = (this->BlueprintList->Location.Y + this->BlueprintList->Size.Height) / 2;

		this->BPListStatus_LBL->Location = System::Drawing::Point(
			p_x - (this->BPListStatus_LBL->Size.Width / 2),
			p_y - (this->BPListStatus_LBL->Size.Height / 2)
		);
	}

	void MainGUI::BlueprintList_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		if (e->Button != System::Windows::Forms::MouseButtons::Right) return;

		int idx = this->BlueprintList->IndexFromPoint(e->X, e->Y);
		if (idx <= -1) return;

		this->BlueprintList->SetSelected(idx, true);
	}

	void MainGUI::MakeFormCentered(System::Windows::Forms::Form^ form)
	{
		form->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
		form->Location = System::Drawing::Point(
			(this->Location.X + this->Size.Width / 2) - (form->Size.Width / 2),
			(this->Location.Y + this->Size.Height / 2) - (form->Size.Height / 2)
		);
	}
}