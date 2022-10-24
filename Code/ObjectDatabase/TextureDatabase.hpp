#pragma once

#include "Lib\String.hpp"

#include <nlohmann\json.hpp>

#include <unordered_map>
#include <string>

namespace SMBC
{
	namespace Texture
	{
		enum TextureType
		{
			TexType_None,
			TexType_SubMeshList,
			TexType_SubMeshMap
		};

		struct TextureData
		{
			//Array that contains textures in the following order: dif, asg, nor
			std::wstring m_texData[3];
			std::wstring material;

			TextureData() = default;
			TextureData(const TextureData&) = delete;
			TextureData(TextureData&) = delete;

			inline TextureData(const std::wstring& dif, const std::wstring& asg, const std::wstring& nor)
			{
				m_texData[0] = dif;
				m_texData[1] = asg;
				m_texData[2] = nor;
			}

			inline bool HasTextures() const
			{
				return (!m_texData[0].empty() || !m_texData[1].empty() || !m_texData[2].empty());
			}

			inline std::wstring& GetStringRef(const int& idx) { return m_texData[idx]; }

			inline nlohmann::json ToJson() const
			{
				nlohmann::json json_out = nlohmann::json::object();

				if (!m_texData[0].empty()) json_out["Dif"] = String::ToUtf8(m_texData[0]);
				if (!m_texData[1].empty()) json_out["Asg"] = String::ToUtf8(m_texData[1]);
				if (!m_texData[2].empty()) json_out["Nor"] = String::ToUtf8(m_texData[2]);

				return json_out;
			}
		};

		class SubMeshDataBase
		{
		public:
			virtual nlohmann::json ToJson() const = 0;

			virtual void AddEntry(const std::wstring& key, const std::size_t& key_idx, TextureData* tex_data_ptr) = 0;
			virtual bool GetEntry(const std::wstring& key, const std::size_t& key_idx, TextureData** v_tex_data) const = 0;

			SubMeshDataBase() = default;
			virtual ~SubMeshDataBase() = default;
		};

		class SubMeshList : public SubMeshDataBase
		{
		public:
			using StorageVec = std::vector<TextureData*>;

			inline nlohmann::json ToJson() const override
			{
				nlohmann::json v_output;

				for (std::size_t a = 0; a < m_Storage.size(); a++)
					v_output.push_back(m_Storage[a]->ToJson());

				return v_output;
			}

			inline void AddEntry(const std::wstring& key, const std::size_t& key_idx, TextureData* tex_data_ptr) override
			{
				if (m_Storage.size() <= key_idx)
					m_Storage.resize(key_idx + 1, nullptr);

				m_Storage[key_idx] = tex_data_ptr;
			}

			inline bool GetEntry(const std::wstring& key, const std::size_t& key_idx, TextureData** v_tex_data) const override
			{
				if (key_idx >= m_Storage.size())
					return false;

				(*v_tex_data) = m_Storage[key_idx];
				return true;
			}


			SubMeshList() = default;
			SubMeshList(const SubMeshList&) = delete;
			SubMeshList(SubMeshList&) = delete;

			inline ~SubMeshList()
			{
				for (std::size_t a = 0; a < m_Storage.size(); a++)
					delete m_Storage[a];
			}

		private:
			StorageVec m_Storage = {};
		};

		class SubMeshMap : public SubMeshDataBase
		{
		public:
			using StorageMap = std::unordered_map<std::wstring, TextureData*>;

			inline nlohmann::json ToJson() const override
			{
				nlohmann::json v_output;

				for (const auto& v_texData : m_Storage)
					v_output[String::ToUtf8(v_texData.first)] = v_texData.second->ToJson();

				return v_output;
			}

			inline void AddEntry(const std::wstring& key, const std::size_t& key_idx, TextureData* tex_data_ptr) override
			{
				if (m_Storage.find(key) != m_Storage.end())
					return;

				m_Storage.insert(std::make_pair(key, tex_data_ptr));
			}

			inline bool GetEntry(const std::wstring& key, const std::size_t& key_idx, TextureData** v_tex_data) const override
			{
				const StorageMap::const_iterator v_iter = m_Storage.find(key);
				if (v_iter == m_Storage.end())
					return false;

				(*v_tex_data) = v_iter->second;
				return true;
			}


			SubMeshMap() = default;
			SubMeshMap(const SubMeshMap&) = delete;
			SubMeshMap(SubMeshMap&) = delete;

			inline ~SubMeshMap()
			{
				for (StorageMap::const_iterator v_iter = m_Storage.begin(); v_iter != m_Storage.end(); v_iter++)
					delete v_iter->second;
			}

		private:
			StorageMap m_Storage = {};
		};
	}
}