#pragma once

#include "Lib\Uuid.hpp"

#include <unordered_map>
#include <string>

namespace SMBC
{
	class PathReplacer
	{
		using StringStorageMap = std::unordered_map<std::wstring, std::wstring>;

		inline static StringStorageMap m_KeyReplacements  = {};
		inline static StringStorageMap m_ResourceUpgrades = {};

		static void CreateKey(std::wstring& key, std::wstring& replacement);
	public:
		static void SetReplacement(const std::wstring& key, const std::wstring& replacement);
		static void SetModData(const std::wstring& path, const SMBC::Uuid& uuid);

		static void UpgradeResource(const std::wstring& mPath, std::wstring& mOutput);
		static void LoadResourceUpgrades(const std::wstring& path);

		static std::wstring ReplaceKey(const std::wstring& path);
		static void ReplaceKeyR(std::wstring& path);

		static void Clear();
	};
}