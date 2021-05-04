#include "Gui/GeneratorSettings.h"

namespace SMBC = BlueprintConverter;

SMBC::GeneratorSettings::GeneratorSettings() {
	this->InitializeComponent();
}

SMBC::GeneratorSettings::~GeneratorSettings() {
	if (this->components) delete this->components;
}

System::Void SMBC::GeneratorSettings::Convert_BTN_Click(System::Object^ sender, System::EventArgs^ e) {
	this->Success = true;
	this->Close();
}

System::Void SMBC::GeneratorSettings::ExportUVs_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	bool _Active = this->ExportUVs_CB->Checked;

	this->ApplyTextures_CB->Enabled = _Active;
	if (this->ApplyTextures_CB->Checked && !_Active)
		this->ApplyTextures_CB->Checked = false;
}