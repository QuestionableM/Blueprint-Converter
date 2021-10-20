#include "TextureDatabase.h"

#include "Object Database/Keyword Replacer/KeywordReplacer.h"

namespace SMBC
{
	typedef Texture::TextureList TexList;
	typedef Texture::Texture	 Tex;

	TexList::TextureList(const std::wstring& dif, const std::wstring& asg, const std::wstring& nor)
	{
		this->dif = dif;
		this->asg = asg;
		this->nor = nor;
	}

	void TexList::ReplaceTextureKeys()
	{
		if (!this->dif.empty()) this->dif = PathReplacer::ReplaceKey(this->dif);
		if (!this->asg.empty()) this->asg = PathReplacer::ReplaceKey(this->asg);
		if (!this->nor.empty()) this->nor = PathReplacer::ReplaceKey(this->nor);
	}

	bool TexList::HasTextures() const
	{
		return (!this->dif.empty() || !this->asg.empty() || !this->nor.empty());
	}

	Tex::Texture(const SMBC::Texture::TextureType& tex_type)
	{
		this->TexType = tex_type;
	}

	void Tex::AddTexture(const std::wstring& name, const SMBC::Texture::TextureList& tex_list)
	{
		this->TexList.insert(std::make_pair(name, tex_list));
	}

	bool Tex::GetTextureByName(const std::wstring& name, SMBC::Texture::TextureList& tex_list) const
	{
		if (this->TexList.find(name) != this->TexList.end())
		{
			tex_list = this->TexList.at(name);
			return true;
		}

		return false;
	}
}