#pragma once
#include <string>
#include <vector>

namespace SMBC {
	namespace Texture {
		enum TextureType { None = 0, SubMeshList = 1, SubMeshMap = 2 };

		struct TextureList {
			std::wstring dif;
			std::wstring asg;
			std::wstring nor;
			std::wstring name;

			TextureList(
				const std::wstring& dif,
				const std::wstring& asg,
				const std::wstring& nor,
				const std::wstring& name = L""
			);

			TextureList() = default;

			void ReplaceTextureKeys();
			bool HasTextures();
		};

		class Texture {
		public:
			SMBC::Texture::TextureType TexType = SMBC::Texture::TextureType::None;
			std::vector<SMBC::Texture::TextureList> TexList;

			Texture(const SMBC::Texture::TextureType& tex_type);
			Texture() = default;

			void AddTexture(const SMBC::Texture::TextureList& tex_list);
			bool GetTextureByName(const std::wstring& name, SMBC::Texture::TextureList& tex_list);
		};
	}
}