#pragma once

namespace BlueprintConverter {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class About : public System::Windows::Forms::Form
	{
	public:
		About();
	protected:
		~About();

	private:
		System::Windows::Forms::TextBox^ Description_LBL;
		System::Windows::Forms::LinkLabel^ BBLink_LBL;
		System::Windows::Forms::LinkLabel^ JSONLink_LBL;
		System::Windows::Forms::Label^ Version_LBL;
		System::Windows::Forms::LinkLabel^ GitHubRepo_LL;
		System::Windows::Forms::LinkLabel^ Creator_LL;
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->Description_LBL = (gcnew System::Windows::Forms::TextBox());
			this->BBLink_LBL = (gcnew System::Windows::Forms::LinkLabel());
			this->JSONLink_LBL = (gcnew System::Windows::Forms::LinkLabel());
			this->Version_LBL = (gcnew System::Windows::Forms::Label());
			this->GitHubRepo_LL = (gcnew System::Windows::Forms::LinkLabel());
			this->Creator_LL = (gcnew System::Windows::Forms::LinkLabel());
			this->SuspendLayout();
			// 
			// Description_LBL
			// 
			this->Description_LBL->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->Description_LBL->BackColor = System::Drawing::SystemColors::Menu;
			this->Description_LBL->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->Description_LBL->Cursor = System::Windows::Forms::Cursors::Default;
			this->Description_LBL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Description_LBL->Location = System::Drawing::Point(12, 12);
			this->Description_LBL->Multiline = true;
			this->Description_LBL->Name = L"Description_LBL";
			this->Description_LBL->ReadOnly = true;
			this->Description_LBL->ShortcutsEnabled = false;
			this->Description_LBL->Size = System::Drawing::Size(416, 89);
			this->Description_LBL->TabIndex = 0;
			this->Description_LBL->TabStop = false;
			this->Description_LBL->Text = L"This application allows you to convert any blueprint from Scrap Mechanic into a 3"
				L"D model!";
			this->Description_LBL->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &About::MouseKillFocus);
			this->Description_LBL->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &About::MouseKillFocus);
			// 
			// BBLink_LBL
			// 
			this->BBLink_LBL->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->BBLink_LBL->AutoSize = true;
			this->BBLink_LBL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->BBLink_LBL->LinkArea = System::Windows::Forms::LinkArea(29, 40);
			this->BBLink_LBL->Location = System::Drawing::Point(12, 104);
			this->BBLink_LBL->Name = L"BBLink_LBL";
			this->BBLink_LBL->Size = System::Drawing::Size(240, 20);
			this->BBLink_LBL->TabIndex = 3;
			this->BBLink_LBL->TabStop = true;
			this->BBLink_LBL->Text = L"Object rotation functions by Brent Batch";
			this->BBLink_LBL->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->BBLink_LBL->UseCompatibleTextRendering = true;
			this->BBLink_LBL->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &About::BBLink_LBL_LinkClicked);
			// 
			// JSONLink_LBL
			// 
			this->JSONLink_LBL->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->JSONLink_LBL->AutoSize = true;
			this->JSONLink_LBL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->JSONLink_LBL->LinkArea = System::Windows::Forms::LinkArea(15, 13);
			this->JSONLink_LBL->Location = System::Drawing::Point(12, 124);
			this->JSONLink_LBL->Name = L"JSONLink_LBL";
			this->JSONLink_LBL->Size = System::Drawing::Size(197, 20);
			this->JSONLink_LBL->TabIndex = 4;
			this->JSONLink_LBL->TabStop = true;
			this->JSONLink_LBL->Text = L"JSON Library - Nlohmann JSON";
			this->JSONLink_LBL->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->JSONLink_LBL->UseCompatibleTextRendering = true;
			this->JSONLink_LBL->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &About::JSONLink_LBL_LinkClicked);
			// 
			// Version_LBL
			// 
			this->Version_LBL->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Version_LBL->AutoSize = true;
			this->Version_LBL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Version_LBL->Location = System::Drawing::Point(12, 164);
			this->Version_LBL->Name = L"Version_LBL";
			this->Version_LBL->Size = System::Drawing::Size(86, 16);
			this->Version_LBL->TabIndex = 6;
			this->Version_LBL->Text = L"Version: 1.1.4";
			this->Version_LBL->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// GitHubRepo_LL
			// 
			this->GitHubRepo_LL->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->GitHubRepo_LL->AutoSize = true;
			this->GitHubRepo_LL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->GitHubRepo_LL->Location = System::Drawing::Point(316, 159);
			this->GitHubRepo_LL->Margin = System::Windows::Forms::Padding(3, 0, 3, 5);
			this->GitHubRepo_LL->Name = L"GitHubRepo_LL";
			this->GitHubRepo_LL->Size = System::Drawing::Size(111, 16);
			this->GitHubRepo_LL->TabIndex = 7;
			this->GitHubRepo_LL->TabStop = true;
			this->GitHubRepo_LL->Text = L"GitHub repository";
			this->GitHubRepo_LL->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->GitHubRepo_LL->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &About::GitHubRepo_LL_LinkClicked);
			// 
			// Creator_LL
			// 
			this->Creator_LL->AutoSize = true;
			this->Creator_LL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Creator_LL->LinkArea = System::Windows::Forms::LinkArea(11, 17);
			this->Creator_LL->Location = System::Drawing::Point(12, 144);
			this->Creator_LL->Name = L"Creator_LL";
			this->Creator_LL->Size = System::Drawing::Size(186, 20);
			this->Creator_LL->TabIndex = 8;
			this->Creator_LL->TabStop = true;
			this->Creator_LL->Text = L"Created by Questionable Mark";
			this->Creator_LL->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->Creator_LL->UseCompatibleTextRendering = true;
			this->Creator_LL->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &About::Creator_LL_LinkClicked);
			// 
			// About
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(440, 189);
			this->Controls->Add(this->Creator_LL);
			this->Controls->Add(this->GitHubRepo_LL);
			this->Controls->Add(this->Version_LBL);
			this->Controls->Add(this->JSONLink_LBL);
			this->Controls->Add(this->BBLink_LBL);
			this->Controls->Add(this->Description_LBL);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"About";
			this->ShowIcon = false;
			this->Text = L"About";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		System::Void BBLink_LBL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void JSONLink_LBL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void GitHubRepo_LL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void Creator_LL_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void MouseKillFocus(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
	};
}