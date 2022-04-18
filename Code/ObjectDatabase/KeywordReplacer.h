#pragma once

#include <string>
#include <unordered_map>

#include "Lib\Uuid.h"

namespace SMBC
{
	class PathReplacer
	{
		inline static std::unordered_map<std::wstring, std::wstring> m_KeyReplacements  = {};
		inline static std::unordered_map<std::wstring, std::wstring> m_ResourceUpgrades = {};

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