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
	private: System::Windows::Forms::TextBox^ textBox1;
	private: System::Windows::Forms::LinkLabel^ BBLink_LBL;
	private: System::Windows::Forms::LinkLabel^ JSONLink_LBL;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label2;

	private:
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->BBLink_LBL = (gcnew System::Windows::Forms::LinkLabel());
			this->JSONLink_LBL = (gcnew System::Windows::Forms::LinkLabel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// textBox1
			// 
			this->textBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox1->BackColor = System::Drawing::SystemColors::Menu;
			this->textBox1->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBox1->Location = System::Drawing::Point(12, 12);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->ShortcutsEnabled = false;
			this->textBox1->Size = System::Drawing::Size(416, 64);
			this->textBox1->TabIndex = 0;
			this->textBox1->TabStop = false;
			this->textBox1->Text = L"This application allows you to convert any blueprint from Scrap Mechanic into a 3"
				L"D model!";
			// 
			// BBLink_LBL
			// 
			this->BBLink_LBL->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->BBLink_LBL->AutoSize = true;
			this->BBLink_LBL->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->BBLink_LBL->LinkArea = System::Windows::Forms::LinkArea(29, 40);
			this->BBLink_LBL->Location = System::Drawing::Point(12, 79);
			this->BBLink_LBL->Name = L"BBLink_LBL";
			this->BBLink_LBL->Size = System::Drawing::Size(240, 20);
			this->BBLink_LBL->TabIndex = 3;
			this->BBLink_LBL->TabStop = true;
			this->BBLink_LBL->Text = L"Object rotation functions by Brent Batch";
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
			this->JSONLink_LBL->Location = System::Drawing::Point(12, 99);
			this->JSONLink_LBL->Name = L"JSONLink_LBL";
			this->JSONLink_LBL->Size = System::Drawing::Size(197, 20);
			this->JSONLink_LBL->TabIndex = 4;
			this->JSONLink_LBL->TabStop = true;
			this->JSONLink_LBL->Text = L"JSON Library - Nlohmann JSON";
			this->JSONLink_LBL->UseCompatibleTextRendering = true;
			this->JSONLink_LBL->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &About::JSONLink_LBL_LinkClicked);
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(9, 119);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(190, 16);
			this->label1->TabIndex = 5;
			this->label1->Text = L"Created by Questionable Mark";
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(9, 137);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(77, 16);
			this->label2->TabIndex = 6;
			this->label2->Text = L"Version: 1.1";
			// 
			// About
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(440, 162);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->JSONLink_LBL);
			this->Controls->Add(this->BBLink_LBL);
			this->Controls->Add(this->textBox1);
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
	};
}