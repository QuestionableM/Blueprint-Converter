#pragma once

#include <string>

namespace BlueprintConverter {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class GeneratorSettings : public System::Windows::Forms::Form
	{
	public:
		GeneratorSettings(const std::wstring& blueprint_name);
		bool Success = false;

	protected:
		~GeneratorSettings();

	public:
		System::Windows::Forms::CheckBox^ ExportTexPaths_CB;
		System::Windows::Forms::CheckBox^ ExportNormals_CB;
		System::Windows::Forms::CheckBox^ ExportUVs_CB;
		System::Windows::Forms::TextBox^ BlueprintName_TB;
		System::Windows::Forms::CheckBox^ ApplyTextures_CB;
		System::Windows::Forms::CheckBox^ MaterialsByColor_CB;
		System::Windows::Forms::ComboBox^ SeparationType_CB;

	private:
		System::Windows::Forms::Button^ Convert_BTN;
		System::Windows::Forms::GroupBox^ ModelSettings_GB;
		System::Windows::Forms::GroupBox^ DataSettings_GB;
		System::Windows::Forms::GroupBox^ BlueprintName_GB;
		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::ToolTip^ ToolTip_TT;
		System::ComponentModel::IContainer^ components;


#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->Convert_BTN = (gcnew System::Windows::Forms::Button());
			this->ExportTexPaths_CB = (gcnew System::Windows::Forms::CheckBox());
			this->ApplyTextures_CB = (gcnew System::Windows::Forms::CheckBox());
			this->ModelSettings_GB = (gcnew System::Windows::Forms::GroupBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SeparationType_CB = (gcnew System::Windows::Forms::ComboBox());
			this->MaterialsByColor_CB = (gcnew System::Windows::Forms::CheckBox());
			this->DataSettings_GB = (gcnew System::Windows::Forms::GroupBox());
			this->ExportNormals_CB = (gcnew System::Windows::Forms::CheckBox());
			this->ExportUVs_CB = (gcnew System::Windows::Forms::CheckBox());
			this->BlueprintName_TB = (gcnew System::Windows::Forms::TextBox());
			this->BlueprintName_GB = (gcnew System::Windows::Forms::GroupBox());
			this->ToolTip_TT = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->ModelSettings_GB->SuspendLayout();
			this->DataSettings_GB->SuspendLayout();
			this->BlueprintName_GB->SuspendLayout();
			this->SuspendLayout();
			// 
			// Convert_BTN
			// 
			this->Convert_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->Convert_BTN->Cursor = System::Windows::Forms::Cursors::Hand;
			this->Convert_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Convert_BTN->Location = System::Drawing::Point(12, 300);
			this->Convert_BTN->Name = L"Convert_BTN";
			this->Convert_BTN->Size = System::Drawing::Size(238, 31);
			this->Convert_BTN->TabIndex = 0;
			this->Convert_BTN->Text = L"Convert";
			this->ToolTip_TT->SetToolTip(this->Convert_BTN, L"Converts a blueprint with the specified settings");
			this->Convert_BTN->UseVisualStyleBackColor = true;
			this->Convert_BTN->Click += gcnew System::EventHandler(this, &GeneratorSettings::Convert_BTN_Click);
			// 
			// ExportTexPaths_CB
			// 
			this->ExportTexPaths_CB->AutoSize = true;
			this->ExportTexPaths_CB->Cursor = System::Windows::Forms::Cursors::Hand;
			this->ExportTexPaths_CB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->ExportTexPaths_CB->Location = System::Drawing::Point(6, 65);
			this->ExportTexPaths_CB->Name = L"ExportTexPaths_CB";
			this->ExportTexPaths_CB->Size = System::Drawing::Size(191, 20);
			this->ExportTexPaths_CB->TabIndex = 1;
			this->ExportTexPaths_CB->Text = L"Export Model Texture Paths";
			this->ExportTexPaths_CB->UseVisualStyleBackColor = true;
			// 
			// ApplyTextures_CB
			// 
			this->ApplyTextures_CB->AutoSize = true;
			this->ApplyTextures_CB->Cursor = System::Windows::Forms::Cursors::Hand;
			this->ApplyTextures_CB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->ApplyTextures_CB->Location = System::Drawing::Point(6, 91);
			this->ApplyTextures_CB->Name = L"ApplyTextures_CB";
			this->ApplyTextures_CB->Size = System::Drawing::Size(117, 20);
			this->ApplyTextures_CB->TabIndex = 3;
			this->ApplyTextures_CB->Text = L"Apply Textures";
			this->ApplyTextures_CB->UseVisualStyleBackColor = true;
			this->ApplyTextures_CB->CheckedChanged += gcnew System::EventHandler(this, &GeneratorSettings::ApplyTextures_CB_CheckedChanged);
			// 
			// ModelSettings_GB
			// 
			this->ModelSettings_GB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ModelSettings_GB->Controls->Add(this->label1);
			this->ModelSettings_GB->Controls->Add(this->SeparationType_CB);
			this->ModelSettings_GB->Controls->Add(this->MaterialsByColor_CB);
			this->ModelSettings_GB->Controls->Add(this->ExportTexPaths_CB);
			this->ModelSettings_GB->Controls->Add(this->ApplyTextures_CB);
			this->ModelSettings_GB->Location = System::Drawing::Point(12, 69);
			this->ModelSettings_GB->Name = L"ModelSettings_GB";
			this->ModelSettings_GB->Size = System::Drawing::Size(238, 145);
			this->ModelSettings_GB->TabIndex = 4;
			this->ModelSettings_GB->TabStop = false;
			this->ModelSettings_GB->Text = L"Model Settings";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(3, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(112, 16);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Separation Type:";
			// 
			// SeparationType_CB
			// 
			this->SeparationType_CB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->SeparationType_CB->Cursor = System::Windows::Forms::Cursors::Hand;
			this->SeparationType_CB->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->SeparationType_CB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->SeparationType_CB->FormattingEnabled = true;
			this->SeparationType_CB->Items->AddRange(gcnew cli::array< System::Object^  >(6) {
				L"None", L"Separate All Objects", L"Separate By Joints",
				L"Group Objects by Uuid", L"Group Objects by Color",
				L"Group Objects by Uuid and Color"
			});
			this->SeparationType_CB->Location = System::Drawing::Point(6, 35);
			this->SeparationType_CB->Name = L"SeparationType_CB";
			this->SeparationType_CB->Size = System::Drawing::Size(226, 24);
			this->SeparationType_CB->TabIndex = 5;
			// 
			// MaterialsByColor_CB
			// 
			this->MaterialsByColor_CB->AutoSize = true;
			this->MaterialsByColor_CB->Cursor = System::Windows::Forms::Cursors::Hand;
			this->MaterialsByColor_CB->Enabled = false;
			this->MaterialsByColor_CB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->MaterialsByColor_CB->Location = System::Drawing::Point(6, 117);
			this->MaterialsByColor_CB->Name = L"MaterialsByColor_CB";
			this->MaterialsByColor_CB->Size = System::Drawing::Size(176, 20);
			this->MaterialsByColor_CB->TabIndex = 4;
			this->MaterialsByColor_CB->Text = L"Group Materials By Color";
			this->MaterialsByColor_CB->UseVisualStyleBackColor = true;
			// 
			// DataSettings_GB
			// 
			this->DataSettings_GB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->DataSettings_GB->Controls->Add(this->ExportNormals_CB);
			this->DataSettings_GB->Controls->Add(this->ExportUVs_CB);
			this->DataSettings_GB->Location = System::Drawing::Point(12, 220);
			this->DataSettings_GB->Name = L"DataSettings_GB";
			this->DataSettings_GB->Size = System::Drawing::Size(238, 74);
			this->DataSettings_GB->TabIndex = 5;
			this->DataSettings_GB->TabStop = false;
			this->DataSettings_GB->Text = L"Data Settings";
			// 
			// ExportNormals_CB
			// 
			this->ExportNormals_CB->AutoSize = true;
			this->ExportNormals_CB->Checked = true;
			this->ExportNormals_CB->CheckState = System::Windows::Forms::CheckState::Checked;
			this->ExportNormals_CB->Cursor = System::Windows::Forms::Cursors::Hand;
			this->ExportNormals_CB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->ExportNormals_CB->Location = System::Drawing::Point(6, 45);
			this->ExportNormals_CB->Name = L"ExportNormals_CB";
			this->ExportNormals_CB->Size = System::Drawing::Size(119, 20);
			this->ExportNormals_CB->TabIndex = 1;
			this->ExportNormals_CB->Text = L"Export Normals";
			this->ExportNormals_CB->UseVisualStyleBackColor = true;
			// 
			// ExportUVs_CB
			// 
			this->ExportUVs_CB->AutoSize = true;
			this->ExportUVs_CB->Checked = true;
			this->ExportUVs_CB->CheckState = System::Windows::Forms::CheckState::Checked;
			this->ExportUVs_CB->Cursor = System::Windows::Forms::Cursors::Hand;
			this->ExportUVs_CB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ExportUVs_CB->Location = System::Drawing::Point(6, 19);
			this->ExportUVs_CB->Name = L"ExportUVs_CB";
			this->ExportUVs_CB->Size = System::Drawing::Size(163, 20);
			this->ExportUVs_CB->TabIndex = 0;
			this->ExportUVs_CB->Text = L"Export UV Coordinates";
			this->ExportUVs_CB->UseVisualStyleBackColor = true;
			this->ExportUVs_CB->CheckedChanged += gcnew System::EventHandler(this, &GeneratorSettings::ExportUVs_CB_CheckedChanged);
			// 
			// BlueprintName_TB
			// 
			this->BlueprintName_TB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->BlueprintName_TB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->BlueprintName_TB->Location = System::Drawing::Point(6, 19);
			this->BlueprintName_TB->MaxLength = 30;
			this->BlueprintName_TB->Name = L"BlueprintName_TB";
			this->BlueprintName_TB->Size = System::Drawing::Size(226, 22);
			this->BlueprintName_TB->TabIndex = 6;
			this->BlueprintName_TB->TextChanged += gcnew System::EventHandler(this, &GeneratorSettings::BlueprintName_TB_TextChanged);
			this->BlueprintName_TB->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GeneratorSettings::BlueprintName_TB_KeyPress);
			// 
			// BlueprintName_GB
			// 
			this->BlueprintName_GB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->BlueprintName_GB->Controls->Add(this->BlueprintName_TB);
			this->BlueprintName_GB->Location = System::Drawing::Point(12, 12);
			this->BlueprintName_GB->Name = L"BlueprintName_GB";
			this->BlueprintName_GB->Size = System::Drawing::Size(238, 51);
			this->BlueprintName_GB->TabIndex = 7;
			this->BlueprintName_GB->TabStop = false;
			this->BlueprintName_GB->Text = L"Output File Name";
			// 
			// GeneratorSettings
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(262, 343);
			this->Controls->Add(this->BlueprintName_GB);
			this->Controls->Add(this->DataSettings_GB);
			this->Controls->Add(this->ModelSettings_GB);
			this->Controls->Add(this->Convert_BTN);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"GeneratorSettings";
			this->ShowIcon = false;
			this->Text = L"Generation Settings";
			this->ModelSettings_GB->ResumeLayout(false);
			this->ModelSettings_GB->PerformLayout();
			this->DataSettings_GB->ResumeLayout(false);
			this->DataSettings_GB->PerformLayout();
			this->BlueprintName_GB->ResumeLayout(false);
			this->BlueprintName_GB->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Void Convert_BTN_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ExportUVs_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void BlueprintName_TB_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void BlueprintName_TB_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e);
		System::Void ApplyTextures_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
	};
}