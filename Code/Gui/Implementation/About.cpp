#include "Gui/About.h"

namespace SMBC {
	using namespace BlueprintConverter;
};

SMBC::About::About() {
	this->InitializeComponent();

#if _WIN64
	this->Version_LBL->Text += " (x64)";
#else
	this->Version_LBL->Text += " (x86)";
#endif

#ifdef DEBUG
	this->Version_LBL->Text += " DEBUG";
#endif
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

System::Void SMBC::About::GitHubRepo_LL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
	System::Diagnostics::Process::Start("https://github.com/QuestionableM/Blueprint-Converter");
}

System::Void SMBC::About::Creator_LL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
	System::Diagnostics::Process::Start("https://github.com/QuestionableM");
}