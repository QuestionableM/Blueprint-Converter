#include "Gui/GeneratorSettings.h"

#include "Lib/File/FileFunc.h"
#include "Lib/GuiLib/GuiLib.h"
#include "Lib/OtherFunc/OtherFunc.h"

#include <msclr/marshal_cppstd.h>

typedef BlueprintConverter::GeneratorSettings GenSet;

GenSet::GeneratorSettings(const std::wstring& blueprint_name)
{
	this->InitializeComponent();
	this->SeparationType_CB->SelectedIndex = 0;
	this->BlueprintName_TB->Text = gcnew System::String(blueprint_name.c_str());
}

GenSet::~GeneratorSettings()
{
	if (this->components) delete this->components;
}

void GenSet::Convert_BTN_Click(System::Object^ sender, System::EventArgs^ e)
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

void GenSet::ExportUVs_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
	bool _Active = this->ExportUVs_CB->Checked;

	this->ApplyTextures_CB->Enabled = _Active;
	if (!_Active) this->ApplyTextures_CB->Checked = false;
}

void GenSet::BlueprintName_TB_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
	this->Convert_BTN->Enabled = (this->BlueprintName_TB->TextLength > 0);
}

void GenSet::BlueprintName_TB_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e)
{
	if (!SMBC::Other::IsLetterAllowed(e->KeyChar) && e->KeyChar != 0x8 && e->KeyChar != 0x1)
		e->Handled = true;
}

void GenSet::ApplyTextures_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
	bool _Active = this->ApplyTextures_CB->Checked;

	this->MaterialsByColor_CB->Enabled = _Active;
	if (!_Active) this->MaterialsByColor_CB->Checked = false;
}