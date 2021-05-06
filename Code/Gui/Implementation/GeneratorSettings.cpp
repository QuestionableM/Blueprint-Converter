#include "Gui/GeneratorSettings.h"
#include "Lib/File/FileFunc.h"
#include <msclr/marshal_cppstd.h>

namespace SMBC {
	using namespace BlueprintConverter;
};

SMBC::GeneratorSettings::GeneratorSettings(const std::wstring& blueprint_name) {
	this->InitializeComponent();
	this->BlueprintName_TB->Text = gcnew System::String(blueprint_name.c_str());
}

SMBC::GeneratorSettings::~GeneratorSettings() {
	if (this->components) delete this->components;
}

System::Void SMBC::GeneratorSettings::Convert_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	std::wstring _bp_name = msclr::interop::marshal_as<std::wstring>(this->BlueprintName_TB->Text);

	if (SMBC::FILE::IsBadPath(L"./Converted Models/" + _bp_name)) {
		System::Windows::Forms::MessageBox::Show(
			L"The specified blueprint name is invalid.\n\nPlease, pick the other name",
			L"Invalid name",
			System::Windows::Forms::MessageBoxButtons::OK,
			System::Windows::Forms::MessageBoxIcon::Warning
		);
		return;
	}

	this->Success = true;
	this->Close();
}

System::Void SMBC::GeneratorSettings::ExportUVs_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	bool _Active = this->ExportUVs_CB->Checked;

	this->ApplyTextures_CB->Enabled = _Active;
	if (!_Active) this->ApplyTextures_CB->Checked = false;
}

System::Void SMBC::GeneratorSettings::BlueprintName_TB_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	this->Convert_BTN->Enabled = (this->BlueprintName_TB->TextLength > 0);
}

#include "Lib/OtherFunc/OtherFunc.h"

System::Void SMBC::GeneratorSettings::BlueprintName_TB_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e) {
	if (!SMBC::Other::IsLetterAllowed(e->KeyChar) && e->KeyChar != 0x8 && e->KeyChar != 0x1)
		e->Handled = true;
}

System::Void SMBC::GeneratorSettings::ApplyTextures_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	bool _Active = this->ApplyTextures_CB->Checked;

	this->MaterialsByColor_CB->Enabled = _Active;
	if (!_Active) this->MaterialsByColor_CB->Checked = false;
}