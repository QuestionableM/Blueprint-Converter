#include "KeywordReplacer.h"

#include "Lib\String.h"
#include "Lib\Json.h"
#include "Lib\File.h"

#include "DebugCon.h"

namespace SMBC
{
	void PathReplacer::CreateKey(std::wstring& key, std::wstring& replacement)
	{
		String::ToLowerR(key);
		String::ToLowerR(replacement);

		String::ReplaceR(key, L'\\', L'/');
		String::ReplaceR(replacement, L'\\', L'/');
	}

	void PathReplacer::SetReplacement(const std::wstring& key, const std::wstring& replacement)
	{
		std::wstring mLowerKey = key;
		std::wstring mLowerVal = replacement;

		PathReplacer::CreateKey(mLowerKey, mLowerVal);

		StringStorageMap::iterator it = m_KeyReplacements.find(mLowerKey);
		if (it != m_KeyReplacements.end())
		{
			it->second = mLowerVal;
			return;
		}

		m_KeyReplacements.insert(std::make_pair(mLowerKey, mLowerVal));
	}
	
	void PathReplacer::SetModData(const std::wstring& path, const SMBC::Uuid& uuid)
	{
		const std::wstring mContentKey = String::ToWide("$content_" + uuid.ToString());

		PathReplacer::SetReplacement(mContentKey     , path);
		PathReplacer::SetReplacement(L"$mod_data"    , path);
		PathReplacer::SetReplacement(L"$content_data", path);
	}
	
	void PathReplacer::UpgradeResource(const std::wstring& mPath, std::wstring& mOutput)
	{
		std::wstring mLowerPath = mPath;

		{
			String::ToLowerR(mLowerPath);
			String::ReplaceR(mLowerPath, L'\\', L'/');
		}

		const StringStorageMap::iterator it = m_ResourceUpgrades.find(mLowerPath);
		if (it != m_ResourceUpgrades.end())
		{
			mOutput = it->second;
		}
		else
		{
			mOutput = mLowerPath;
		}
	}
	
	void PathReplacer::LoadResourceUpgrades(const std::wstring& path)
	{
		const nlohmann::json uJson = Json::LoadParseJson(path);
		if (!uJson.is_object()) return;

		DebugOutL("Loading resource upgrades: ", 0b1101_fg, path);

		const auto& upgrade_array = Json::Get(uJson, "upgrade");
		if (!upgrade_array.is_array()) return;

		for (const auto& upgrade_list : upgrade_array)
		{
			if (!upgrade_list.is_array()) continue;

			for (const auto& upgrade_obj : upgrade_list)
			{
				const auto& upgrade_key = Json::Get(upgrade_obj, 0);
				const auto& upgrade_val = Json::Get(upgrade_obj, 1);

				if (!(upgrade_key.is_string() && upgrade_val.is_string())) continue;

				std::wstring upKeyWstr = String::ToWide(upgrade_key.get_ref<const std::string&>());
				std::wstring upValWstr = String::ToWide(upgrade_val.get_ref<const std::string&>());

				PathReplacer::CreateKey(upKeyWstr, upValWstr);

				if (m_ResourceUpgrades.find(upKeyWstr) == m_ResourceUpgrades.end())
					m_ResourceUpgrades.insert(std::make_pair(upKeyWstr, upValWstr));
			}
		}
	}

	std::wstring PathReplacer::ReplaceKey(const std::wstring& path)
	{
		std::wstring mOutput;
		PathReplacer::UpgradeResource(path, mOutput);

		const std::size_t mKeyIdx = mOutput.find_first_of(L'/');
		if (mKeyIdx != std::wstring::npos)
		{
			const std::wstring mKeyChunk = mOutput.substr(0, mKeyIdx);
			if (mKeyChunk[0] == '$')
			{
				const StringStorageMap::iterator it = m_KeyReplacements.find(mKeyChunk);
				if (it != m_KeyReplacements.end())
				{
					return (it->second + mOutput.substr(mKeyIdx));
				}
				else
				{
					DebugErrorL("Couldn't replace the specified key: ", mKeyChunk);
				}
			}
		}

		return mOutput;
	}

	void PathReplacer::ReplaceKeyR(std::wstring& path)
	{
		PathReplacer::UpgradeResource(path, path);

		const std::size_t mKeyIdx = path.find_first_of(L'/');
		if (mKeyIdx != std::wstring::npos)
		{
			const std::wstring mKeyChunk = path.substr(0, mKeyIdx);
			if (mKeyChunk[0] != '$') return;

			const StringStorageMap::iterator it = m_KeyReplacements.find(mKeyChunk);
			if (it != m_KeyReplacements.end())
			{
				path = (it->second + path.substr(mKeyIdx));
			}
			else
			{
				DebugErrorL("Couldn't replace the specified key: ", mKeyChunk);
			}
		}
	}

	void PathReplacer::Clear()
	{
		DebugOutL(__FUNCTION__);

		m_KeyReplacements.clear();
		m_ResourceUpgrades.clear();
	}
}