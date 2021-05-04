#include "TextureDatabase.h"

#include "Object Database/Keyword Replacer/KeywordReplacer.h"

namespace SM_Tex = SMBC::Texture;

SM_Tex::TextureList::TextureList(
	const std::wstring& dif,
	const std::wstring& asg,
	const std::wstring& nor,
	const std::wstring& name
) {
	this->dif = dif;
	this->asg = asg;
	this->nor = nor;
	this->name = name;
}

void SM_Tex::TextureList::ReplaceTextureKeys() {
	if (!this->dif.empty()) this->dif = SMBC::PathReplacer::ReplaceKey(this->dif);
	if (!this->asg.empty()) this->asg = SMBC::PathReplacer::ReplaceKey(this->asg);
	if (!this->nor.empty()) this->nor = SMBC::PathReplacer::ReplaceKey(this->nor);
}

bool SM_Tex::TextureList::HasTextures() {
	return (!this->dif.empty() || !this->asg.empty() || !this->nor.empty());
}

SM_Tex::Texture::Texture(const SMBC::Texture::TextureType& tex_type) {
	this->TexType = tex_type;
}
void SM_Tex::Texture::AddTexture(const SMBC::Texture::TextureList& tex_list) {
	this->TexList.push_back(tex_list);
}
bool SM_Tex::Texture::GetTextureByName(const std::wstring& name, SMBC::Texture::TextureList& tex_list) {
	for (SMBC::Texture::TextureList& tex_l : this->TexList)
		if (tex_l.name == name) {
			tex_list = tex_l;
			return true;
		}

	return false;
}