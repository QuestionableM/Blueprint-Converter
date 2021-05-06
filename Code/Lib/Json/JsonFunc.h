#pragma once

#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

namespace SMBC {
	namespace JSON {
		std::string ReadWholeFile(std::ifstream& input);
		bool OpenParseJson(const std::wstring& path, nlohmann::json& obj, const bool _stringify = false);

		std::wstring GetJsonWstr(nlohmann::json& json, const std::string& key);
	}
}