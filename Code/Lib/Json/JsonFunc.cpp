#include "JsonFunc.h"

#include <sstream>
#include "Lib/OtherFunc/OtherFunc.h"

namespace _JSON = SMBC::Json;

std::string _JSON::ReadWholeFile(std::ifstream& input) {
	std::string _Output = "";

	if (input.is_open()) {
		bool is_inString = false;
		for (std::string str; std::getline(input, str);) {
			for (std::size_t a = 0; a < str.size(); a++) {
				char& _cChar = str[a];

				if (_cChar == '\"' && ((a > 0 && str[a - 1] != '\\') || a == 0))
					is_inString = !is_inString;

				if (!is_inString && str.substr(a, 2) == "//")
					break;

				_Output += _cChar;
			}
		}
	}

	return _Output;
}

bool _JSON::ParseJson(const std::wstring& path, nlohmann::json& obj, const bool _stringify) {
	try {
		std::ifstream _InputFile(path);

		if (!_InputFile.is_open()) return false;

		std::string _RawJson;

		if (_stringify)
			_RawJson = _JSON::ReadWholeFile(_InputFile);
		else {
			std::stringstream sstream;

			sstream << _InputFile.rdbuf();

			_RawJson = sstream.str();
		}

		obj = nlohmann::json::parse(_RawJson, nullptr, true, true);
		return true;
	}
	catch (...) {}

	return false;
}

const nlohmann::json& _JSON::Get(const nlohmann::json& json, const std::string& key) {
	if (json.contains(key))
		return json.at(key);

	return _JSON::NullObject;
}

const nlohmann::json& _JSON::Get(const nlohmann::json& json, const std::size_t& key) {
	if (key < json.size())
		return json[key];

	return _JSON::NullObject;
}

std::wstring _JSON::GetWstr(nlohmann::json& json, const std::string& key) {
	std::wstring _Output = L"";
	
	if (json.contains(key)) {
		auto& _value = json.at(key);

		if (_value.is_string())
			_Output.append(SMBC::Other::Utf8ToWide(_value.get<std::string>()));
	}

	return _Output;
}