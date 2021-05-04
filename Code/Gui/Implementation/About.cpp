#include "Gui/About.h"

namespace SMBC {
	using namespace BlueprintConverter;
};

SMBC::About::About() {
	this->InitializeComponent();
}

SMBC::About::~About() {
	if (components) delete components;
}

System::Void SMBC::About::BBLink_LBL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
	System::Diagnostics::Process::Start("https://www.youtube.com/channel/UCN_fSh10QsNNPYZjsAWKtIA");
}

System::Void SMBC::About::JSONLink_LBL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
	System::Diagnostics::Process::Start("https://github.com/nlohmann/json");
}