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

nlohmann::json* _JSON::OpenParseJson(const std::wstring& path, const bool _stringify) {
	nlohmann::json* output = nullptr;

	try {
		std::ifstream _InputFile(path);

		std::string _RawJson;

		if (_InputFile.is_open()) {
			if (_stringify)
				_RawJson = _JSON::ReadWholeFile(_InputFile);
			else {
				std::stringstream sstream;

				sstream << _InputFile.rdbuf();

				_RawJson = sstream.str();
			}

			nlohmann::json& _PJson = nlohmann::json::parse(_RawJson, nullptr, true, true);

			output = new nlohmann::json(_PJson);

			_InputFile.close();
		}
	}
	catch (nlohmann::json::parse_error& p_err) {
		nlohmann::json::parse_error& p = p_err;
	}

	return output;
}

nlohmann::json _JSON::OpenParseJsonA(const std::wstring& path) {
	nlohmann::json _Output;

	try {
		std::ifstream _InputFile(path);

		if (_InputFile.is_open()) {
			std::stringstream sstream;

			sstream << _InputFile.rdbuf();

			nlohmann::json& _PJson = nlohmann::json::parse(sstream.str(), nullptr, true, true);

			_Output = _PJson;

			_InputFile.close();
		}
	}
	catch (nlohmann::json::parse_error& p_err) {
		nlohmann::json::parse_error& p = p_err;
	}

	return _Output;
}

std::wstring* _JSON::GetJsonWstr(nlohmann::json*& json, const std::string& key) {
	std::wstring* _Output = nullptr;

	if (json->contains(key) && json->at(key).is_string())
		_Output = new std::wstring(SMBC::Other::Utf8ToWide(json->at(key).get<std::string>()));

	return _Output;
}

std::wstring _JSON::GetJsonWstrA(nlohmann::json*& json, const std::string& key, const std::wstring& replacement) {
	if (!json->contains(key)) goto return_empty;

	auto& _ResStr = json->at(key);
	if (!_ResStr.is_string()) goto return_empty;

	return SMBC::Other::Utf8ToWide(_ResStr.get<std::string>());

return_empty:
	return replacement;
}