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

	CachedObjectType CachedBlock::Type() const
	{
		return CachedObjectType::Block;
	}

	std::wstring CachedBlock::GetName() const
	{
		return blkPtr->Name;
	}

	const static std::string _fp_t = "Ns 324\nKa 1 1 1\nKd ";
	const static std::string _sp_t = "Ks 0.5 0.5 0.5\nKe 0 0 0\nNi 1.45\nd 1\nillum2\n";

	void CachedBlock::WriteMtlData(const std::wstring& name, std::ofstream& out) const
	{
		if (!blkPtr) return;

		std::string _mtl_mat = "newmtl ";
		String::Combine(_mtl_mat, name, "\n", _fp_t);

		if (ConvertSettings::MatByColor)
			String::Combine(_mtl_mat, String::HexToFloatW(this->color), "\n");
		else
			_mtl_mat.append("0.8 0.8 0.8\n");

		_mtl_mat.append(_sp_t);

		{
			Texture::TextureList& texData = blkPtr->TextureList;
			if (!texData.nor.empty()) String::Combine(_mtl_mat, "map_Bump ", texData.nor, "\n");
			if (!texData.dif.empty()) String::Combine(_mtl_mat, "map_Kd ",   texData.dif, "\n");
			if (!texData.asg.empty()) String::Combine(_mtl_mat, "map_Ks ",   texData.asg, "\n");
		}

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
			std::wstring mtlName = name + L' ' + std::to_wstring(smData->SubMeshIndex);

			std::string _mtl_mat = "newmtl ";
			String::Combine(_mtl_mat, mtlName, "\n", _fp_t);

			if (ConvertSettings::MatByColor)
				String::Combine(_mtl_mat, String::HexToFloatW(this->color), "\n");
			else
				_mtl_mat.append("0.8 0.8 0.8\n");

			_mtl_mat.append(_sp_t);

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

			if (_Success)
			{
				if (!_TList.nor.empty()) String::Combine(_mtl_mat, "map_Bump ", _TList.nor, "\n");
				if (!_TList.dif.empty()) String::Combine(_mtl_mat, "map_Kd ",   _TList.dif, "\n");
				if (!_TList.asg.empty()) String::Combine(_mtl_mat, "map_Ks ",   _TList.asg, "\n");
			}

			_mtl_mat.append("\n");

			out.write(_mtl_mat.c_str(), _mtl_mat.length());
		}
	}

	nlohmann::json CachedPart::WriteTexturePaths() const
	{
		if (!objPtr) return nlohmann::json::object();

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
}