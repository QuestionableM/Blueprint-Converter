#pragma once

#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

namespace SMBC {
	namespace Json {
		std::string ReadWholeFile(std::ifstream& input);
		nlohmann::json LoadParseJson(const std::wstring& path, const bool& _stringify = false);

		void WriteToFile(const std::wstring& path, const nlohmann::json& mJson);

		const static nlohmann::json NullObject = nlohmann::json();
		const nlohmann::json& Get(const nlohmann::json& json, const std::string& key);
		const nlohmann::json& Get(const nlohmann::json& json, const std::size_t& key);

		std::wstring GetWstr(nlohmann::json& json, const std::string& key);
	}
}