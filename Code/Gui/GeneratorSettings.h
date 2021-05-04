#pragma once

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
		GeneratorSettings();
		bool Success = false;
		System::Windows::Forms::CheckBox^ ExportTexPaths_CB;
		System::Windows::Forms::CheckBox^ SeparateParts_CB;
		System::Windows::Forms::GroupBox^ ModelSettings_GB;
		System::Windows::Forms::GroupBox^ DataSettings_GB;
		System::Windows::Forms::CheckBox^ ExportNormals_CB;
		System::Windows::Forms::CheckBox^ ExportUVs_CB;
		System::Windows::Forms::CheckBox^ ApplyTextures_CB;

	protected:
		~GeneratorSettings();

	private:
		System::Windows::Forms::Button^ Convert_BTN;
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->Convert_BTN = (gcnew System::Windows::Forms::Button());
			this->ExportTexPaths_CB = (gcnew System::Windows::Forms::CheckBox());
			this->SeparateParts_CB = (gcnew System::Windows::Forms::CheckBox());
			this->ApplyTextures_CB = (gcnew System::Windows::Forms::CheckBox());
			this->ModelSettings_GB = (gcnew System::Windows::Forms::GroupBox());
			this->DataSettings_GB = (gcnew System::Windows::Forms::GroupBox());
			this->ExportNormals_CB = (gcnew System::Windows::Forms::CheckBox());
			this->ExportUVs_CB = (gcnew System::Windows::Forms::CheckBox());
			this->ModelSettings_GB->SuspendLayout();
			this->DataSettings_GB->SuspendLayout();
			this->SuspendLayout();
			// 
			// Convert_BTN
			// 
			this->Convert_BTN->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->Convert_BTN->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Convert_BTN->Location = System::Drawing::Point(12, 197);
			this->Convert_BTN->Name = L"Convert_BTN";
			this->Convert_BTN->Size = System::Drawing::Size(200, 31);
			this->Convert_BTN->TabIndex = 0;
			this->Convert_BTN->Text = L"Convert";
			this->Convert_BTN->UseVisualStyleBackColor = true;
			this->Convert_BTN->Click += gcnew System::EventHandler(this, &GeneratorSettings::Convert_BTN_Click);
			// 
			// ExportTexPaths_CB
			// 
			this->ExportTexPaths_CB->AutoSize = true;
			this->ExportTexPaths_CB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->ExportTexPaths_CB->Location = System::Drawing::Point(6, 19);
			this->ExportTexPaths_CB->Name = L"ExportTexPaths_CB";
			this->ExportTexPaths_CB->Size = System::Drawing::Size(191, 20);
			this->ExportTexPaths_CB->TabIndex = 1;
			this->ExportTexPaths_CB->Text = L"Export Model Texture Paths";
			this->ExportTexPaths_CB->UseVisualStyleBackColor = true;
			// 
			// SeparateParts_CB
			// 
			this->SeparateParts_CB->AutoSize = true;
			this->SeparateParts_CB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->SeparateParts_CB->Location = System::Drawing::Point(6, 45);
			this->SeparateParts_CB->Name = L"SeparateParts_CB";
			this->SeparateParts_CB->Size = System::Drawing::Size(169, 20);
			this->SeparateParts_CB->TabIndex = 2;
			this->SeparateParts_CB->Text = L"Separate Different Parts";
			this->SeparateParts_CB->UseVisualStyleBackColor = true;
			// 
			// ApplyTextures_CB
			// 
			this->ApplyTextures_CB->AutoSize = true;
			this->ApplyTextures_CB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->ApplyTextures_CB->Location = System::Drawing::Point(6, 71);
			this->ApplyTextures_CB->Name = L"ApplyTextures_CB";
			this->ApplyTextures_CB->Size = System::Drawing::Size(117, 20);
			this->ApplyTextures_CB->TabIndex = 3;
			this->ApplyTextures_CB->Text = L"Apply Textures";
			this->ApplyTextures_CB->UseVisualStyleBackColor = true;
			// 
			// ModelSettings_GB
			// 
			this->ModelSettings_GB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ModelSettings_GB->Controls->Add(this->ExportTexPaths_CB);
			this->ModelSettings_GB->Controls->Add(this->ApplyTextures_CB);
			this->ModelSettings_GB->Controls->Add(this->SeparateParts_CB);
			this->ModelSettings_GB->Location = System::Drawing::Point(12, 12);
			this->ModelSettings_GB->Name = L"ModelSettings_GB";
			this->ModelSettings_GB->Size = System::Drawing::Size(200, 100);
			this->ModelSettings_GB->TabIndex = 4;
			this->ModelSettings_GB->TabStop = false;
			this->ModelSettings_GB->Text = L"Model Settings";
			// 
			// DataSettings_GB
			// 
			this->DataSettings_GB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->DataSettings_GB->Controls->Add(this->ExportNormals_CB);
			this->DataSettings_GB->Controls->Add(this->ExportUVs_CB);
			this->DataSettings_GB->Location = System::Drawing::Point(12, 118);
			this->DataSettings_GB->Name = L"DataSettings_GB";
			this->DataSettings_GB->Size = System::Drawing::Size(200, 73);
			this->DataSettings_GB->TabIndex = 5;
			this->DataSettings_GB->TabStop = false;
			this->DataSettings_GB->Text = L"Data Settings";
			// 
			// ExportNormals_CB
			// 
			this->ExportNormals_CB->AutoSize = true;
			this->ExportNormals_CB->Checked = true;
			this->ExportNormals_CB->CheckState = System::Windows::Forms::CheckState::Checked;
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
			// GeneratorSettings
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(224, 240);
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
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Void Convert_BTN_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ExportUVs_CB_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
	};
}