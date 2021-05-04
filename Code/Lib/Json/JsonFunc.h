#pragma once

#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

namespace SMBC {
	namespace JSON {
		std::string ReadWholeFile(std::ifstream& input);
		nlohmann::json* OpenParseJson(const std::wstring& path, const bool _stringify = false);
		nlohmann::json OpenParseJsonA(const std::wstring& path);

		std::wstring* GetJsonWstr(nlohmann::json*& json, const std::string& key);
		std::wstring GetJsonWstrA(nlohmann::json*& json, const std::string& key, const std::wstring& replacement = L"");
	}
}