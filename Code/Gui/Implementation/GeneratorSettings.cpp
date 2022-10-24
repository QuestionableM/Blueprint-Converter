#include "Gui\GeneratorSettings.h"

#include "BlueprintConverter\ConvertSettings.hpp"

#include "Lib\String.hpp"
#include "Lib\GuiLib.hpp"
#include "Lib\File.hpp"

#include <msclr\marshal_cppstd.h>

namespace BlueprintConverter
{
	GeneratorSettings::GeneratorSettings(const std::wstring& blueprint_name)
	{
		this->InitializeComponent();
		this->SeparationType_CB->SelectedIndex = 0;
		this->BlueprintName_TB->Text = gcnew System::String(blueprint_name.c_str());

		this->SeparationType_CB->SelectedIndex = SMBC::ConvertSettings::SeparationMethod;
		this->ExportTexPaths_CB->Checked = SMBC::ConvertSettings::TextureList;
		this->ApplyTextures_CB->Checked = SMBC::ConvertSettings::ApplyTextures && SMBC::ConvertSettings::ExportUvs;
		this->MaterialsByColor_CB->Checked = SMBC::ConvertSettings::MatByColor && SMBC::ConvertSettings::ApplyTextures;

		this->ExportUVs_CB->Checked = SMBC::ConvertSettings::ExportUvs;
		this->ExportNormals_CB->Checked = SMBC::ConvertSettings::ExportNormals;
	}

	GeneratorSettings::~GeneratorSettings()
	{
		if (this->components) delete this->components;
	}

	void GeneratorSettings::Convert_BTN_Click(System::Object^ sender, System::EventArgs^ e)
	{
		std::wstring _bp_name = msclr::interop::marshal_as<std::wstring>(this->BlueprintName_TB->Text);

		if (SMBC::File::IsBadPath(L"./Converted Models/" + _bp_name))
		{
			SMBC::Gui::Warning(
				"Invalid name",
				"The specified blueprint name is invalid.\n\nPlease, pick the other name"
			);
			return;
		}

		this->Success = true;
		this->Close();
	}

	void GeneratorSettings::ExportUVs_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		bool _Active = this->ExportUVs_CB->Checked;

		this->ApplyTextures_CB->Enabled = _Active;
		if (!_Active) this->ApplyTextures_CB->Checked = false;
	}

	void GeneratorSettings::BlueprintName_TB_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->Convert_BTN->Enabled = (this->BlueprintName_TB->TextLength > 0);
	}

	void GeneratorSettings::BlueprintName_TB_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e)
	{
		if (!SMBC::String::IsLetterAllowed(e->KeyChar) && e->KeyChar != 0x8 && e->KeyChar != 0x1)
			e->Handled = true;
	}

	void GeneratorSettings::ApplyTextures_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		bool _Active = this->ApplyTextures_CB->Checked;

		this->MaterialsByColor_CB->Enabled = _Active;
		if (!_Active) this->MaterialsByColor_CB->Checked = false;
	}
}