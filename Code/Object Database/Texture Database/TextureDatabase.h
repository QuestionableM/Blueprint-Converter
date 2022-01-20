#pragma once
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace SMBC
{
	namespace Texture
	{
		enum TextureType
		{
			None = 0,
			SubMeshList = 1,
			SubMeshMap = 2
		};

		struct TextureList
		{
			std::wstring dif;
			std::wstring asg;
			std::wstring nor;

			TextureList(const std::wstring& dif, const std::wstring& asg, const std::wstring& nor);
			TextureList() = default;

			bool HasTextures() const;

			std::wstring& GetStringRef(const int& idx);
			nlohmann::json ToJson() const;
		};

		class Texture
		{
		public:
			SMBC::Texture::TextureType TexType = SMBC::Texture::TextureType::None;
			std::unordered_map<std::wstring, SMBC::Texture::TextureList> TexList;

			Texture(const SMBC::Texture::TextureType& tex_type);
			Texture() = default;

			std::wstring PickString(const std::size_t& mIdx, const std::wstring& mMatName) const;
			nlohmann::json ToJson() const;

			void AddEntry(const std::wstring& name, const SMBC::Texture::TextureList& tex_list);
			bool GetEntry(const std::wstring& name, SMBC::Texture::TextureList& tex_list) const;
		};
	}
}