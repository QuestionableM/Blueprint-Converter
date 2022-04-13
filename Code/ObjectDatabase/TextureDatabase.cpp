#include "TextureDatabase.h"

#include "ObjectDatabase\KeywordReplacer.h"

#include "Lib\String.h"

namespace SMBC
{
	namespace Texture
	{
		TextureList::TextureList(const std::wstring& dif, const std::wstring& asg, const std::wstring& nor)
		{
			this->dif = dif;
			this->asg = asg;
			this->nor = nor;
		}

		bool TextureList::HasTextures() const
		{
			return (!this->dif.empty() || !this->asg.empty() || !this->nor.empty());
		}

		std::wstring& TextureList::GetStringRef(const int& idx)
		{
			return ((std::wstring*)&this->dif)[idx];
		}

		nlohmann::json TextureList::ToJson() const
		{
			nlohmann::json json_out = nlohmann::json::object();

			if (!dif.empty()) json_out["Dif"] = String::ToUtf8(dif);
			if (!asg.empty()) json_out["Asg"] = String::ToUtf8(asg);
			if (!nor.empty()) json_out["Nor"] = String::ToUtf8(nor);

			return json_out;
		}



		Texture::Texture(const SMBC::Texture::TextureType& tex_type)
		{
			this->TexType = tex_type;
		}

		std::wstring Texture::PickString(const std::size_t& mIdx, const std::wstring& mMatName) const
		{
			return (this->TexType == TextureType::SubMeshList ? std::to_wstring(mIdx) : mMatName);
		}

		nlohmann::json Texture::ToJson() const
		{
			nlohmann::json output_json;

			switch (this->TexType)
			{
			case TextureType::SubMeshList:
				{
					for (const auto& tListData : this->TexList)
						output_json.push_back(tListData.second.ToJson());

					break;
				}
			case TextureType::SubMeshMap:
				{
					for (const auto& tListData : this->TexList)
					{
						const TextureList& tList = tListData.second;
					
						std::string tListStr = String::ToUtf8(tListData.first);
						output_json[tListStr] = tList.ToJson();
					}

					break;
				}
			}

			return output_json;
		}

		void Texture::AddEntry(const std::wstring& name, const SMBC::Texture::TextureList& tex_list)
		{
			this->TexList.insert(std::make_pair(name, tex_list));
		}

		bool Texture::GetEntry(const std::wstring& name, SMBC::Texture::TextureList& tex_list) const
		{
			if (this->TexList.find(name) != this->TexList.end())
			{
				tex_list = this->TexList.at(name);
				return true;
			}

			return false;
		}
	}
}