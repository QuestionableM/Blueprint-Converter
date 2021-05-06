#include "JsonFunc.h"

#include <sstream>
#include "Lib/OtherFunc/OtherFunc.h"

namespace _JSON = SMBC::JSON;

std::string _JSON::ReadWholeFile(std::ifstream& input) {
	std::string _Output = "";

	if (input.is_open()) {
		bool is_inString = false;
		for (std::string str; std::getline(input, str);) {
			for (int a = 0; a < (int)str.size(); a++) {
				char& _cChar = str[a];
				bool _Far = (a <= (int)str.size() - 1);

				if (_Far) {
					char& _nChar = str[a + 1];

					if (_cChar == '\"' && str[a - 1] != '\\')
						is_inString = !is_inString;

					if (!is_inString && _cChar == '/' && _nChar == '/')
						break;
				}

				_Output += _cChar;
			}
		}
	}

	return _Output;
}

bool _JSON::OpenParseJson(const std::wstring& path, nlohmann::json& obj, const bool _stringify) {
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

std::wstring _JSON::GetJsonWstr(nlohmann::json& json, const std::string& key) {
	std::wstring _Output = L"";
	
	if (json.contains(key)) {
		auto& _value = json.at(key);

		if (_value.is_string())
			_Output.append(SMBC::Other::Utf8ToWide(_value.get<std::string>()));
	}

	return _Output;
}