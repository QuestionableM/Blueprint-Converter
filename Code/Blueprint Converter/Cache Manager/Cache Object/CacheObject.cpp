#include "CacheObject.h"
#include "Blueprint Converter/Cache Manager/CacheManager.h"
#include "Blueprint Converter/Convert Settings/ConvertSettings.h"
#include "Lib/String/String.h"

namespace SMBC
{
	nlohmann::json CachedObject::ConstructTexObject(const Texture::TextureList& tex)
	{
		nlohmann::json tex_obj = nlohmann::json::object();

		if (!tex.dif.empty()) tex_obj["Dif"] = String::ToUtf8(tex.dif);
		if (!tex.asg.empty()) tex_obj["Asg"] = String::ToUtf8(tex.asg);
		if (!tex.nor.empty()) tex_obj["Nor"] = String::ToUtf8(tex.nor);

		return tex_obj;
	}

	void CachedObject::WriteMtlTextures(std::string& str, const Texture::TextureList& tList)
	{
		if (!tList.nor.empty()) String::Combine(str, "map_Bump ", tList.nor, "\n");
		if (!tList.dif.empty()) String::Combine(str, "map_Kd ",   tList.dif, "\n");
		if (!tList.asg.empty()) String::Combine(str, "map_Ks ",   tList.asg, "\n");
	}

	const static std::string _fp_t = "Ns 324\nKa 1 1 1\nKd ";
	const static std::string _sp_t = "Ks 0.5 0.5 0.5\nKe 0 0 0\nNi 1.45\nd 1\nillum2\n";

	std::string CachedObject::WriteNewMtlHeader(CachedObject* obj, const std::wstring& name)
	{
		std::string _mtl_mat = "netmtl ";
		String::Combine(_mtl_mat, name, "\n", _fp_t);

		if (ConvertSettings::MatByColor)
			String::Combine(_mtl_mat, String::HexToFloatW(obj->color), "\n");
		else
			_mtl_mat.append("0.8 0.8 0.8\n");

		_mtl_mat.append(_sp_t);

		return _mtl_mat;
	}

	CachedObjectType CachedBlock::Type() const
	{
		return CachedObjectType::Block;
	}

	std::wstring CachedBlock::GetName() const
	{
		return blkPtr->Name;
	}

	void CachedBlock::WriteMtlData(const std::wstring& name, std::ofstream& out) const
	{
		std::string _mtl_mat = this->WriteNewMtlHeader((CachedObject*)this, name);

		this->WriteMtlTextures(_mtl_mat, blkPtr->TextureList);

		_mtl_mat.append("\n");

		out.write(_mtl_mat.c_str(), _mtl_mat.length());
	}

	nlohmann::json CachedBlock::WriteTexturePaths() const
	{
		return ConstructTexObject(blkPtr->TextureList);
	}

	bool CachedBlock::IsValid() const
	{
		return blkPtr->TextureList.HasTextures();
	}

	const Model* CachedBlock::GetModelPtr() const
	{
		return nullptr;
	}



	CachedObjectType CachedPart::Type() const
	{
		return CachedObjectType::Part;
	}

	std::wstring CachedPart::GetName() const
	{
		return objPtr->Name;
	}

	void CachedPart::WriteMtlData(const std::wstring& name, std::ofstream& out) const
	{
		if (!modelPtr) return;

		for (std::size_t a = 0; a < modelPtr->subMeshData.size(); a++)
		{
			const SubMeshData* smData = modelPtr->subMeshData[a];
			const std::wstring mtlName = name + L' ' + std::to_wstring(smData->SubMeshIndex);

			std::string _mtl_mat = this->WriteNewMtlHeader((CachedObject*)this, mtlName);

			bool _Success = false;
			Texture::TextureList _TList;

			Texture::Texture& c_Tex = objPtr->TextureList;
			switch (c_Tex.TexType)
			{
			case Texture::TextureType::SubMeshList:
				_Success = c_Tex.GetTextureByName(std::to_wstring(smData->SubMeshIndex), _TList);
				break;
			case Texture::TextureType::SubMeshMap:
				_Success = c_Tex.GetTextureByName(smData->MaterialName, _TList);
				break;
			}

			if (_Success) this->WriteMtlTextures(_mtl_mat, _TList);

			_mtl_mat.append("\n");

			out.write(_mtl_mat.c_str(), _mtl_mat.length());
		}
	}

	nlohmann::json CachedPart::WriteTexturePaths() const
	{
		nlohmann::json output_json;

		const Texture::Texture& cur_tex = objPtr->TextureList;
		switch (cur_tex.TexType)
		{
		case Texture::TextureType::SubMeshList:
			for (const auto& t_ListData : cur_tex.TexList)
			{
				const Texture::TextureList& tex_list = t_ListData.second;
				if (!tex_list.HasTextures()) continue;

				output_json.push_back(ConstructTexObject(tex_list));
			}

			break;
		case Texture::TextureType::SubMeshMap:
			for (const auto& t_ListData : cur_tex.TexList)
			{
				const Texture::TextureList& tex_list = t_ListData.second;
				if (!tex_list.HasTextures()) continue;

				std::string t_ListStr = String::ToUtf8(t_ListData.first);
				output_json[t_ListStr] = ConstructTexObject(tex_list);
			}

			break;
		}

		return output_json;
	}

	bool CachedPart::IsValid() const
	{
		for (const auto& tex_l : objPtr->TextureList.TexList)
		{
			const Texture::TextureList& tex_list = tex_l.second;

			if (tex_list.HasTextures())
				return true;
		}

		return false;
	}

	const Model* CachedPart::GetModelPtr() const
	{
		return this->modelPtr;
	}
}