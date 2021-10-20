#include "Gui/About.h"
#include "Lib/BuildVersion.h"
#include <string>

#include <Windows.h>

namespace BlueprintConverter {
	About::About()
	{
		this->InitializeComponent();

		this->Version_LBL->Text += gcnew System::String((L"." + std::to_wstring(SMBC_BUILD_VERSION)).c_str());

	#if _WIN64
		this->Version_LBL->Text += " (x64)";
	#else
		this->Version_LBL->Text += " (x86)";
	#endif

	#if _DEBUG
		this->Version_LBL->Text += " DEBUG";
	#endif
	}

	About::~About()
	{
		if (components) delete components;
	}

	void About::BBLink_LBL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://www.youtube.com/channel/UCN_fSh10QsNNPYZjsAWKtIA");
	}

	void About::JSONLink_LBL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/nlohmann/json");
	}

	void About::GitHubRepo_LL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/QuestionableM/Blueprint-Converter");
	}

	void About::Creator_LL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/QuestionableM");
	}

	void About::MouseKillFocus(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		this->Description_LBL->SelectionLength = 0;
		SendMessage(
			static_cast<HWND>(this->Description_LBL->Handle.ToPointer()),
			WM_KILLFOCUS,
			0,
			NULL
		);
	}
}