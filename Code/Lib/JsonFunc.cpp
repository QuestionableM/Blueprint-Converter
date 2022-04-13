#include "JsonFunc.h"

#include <sstream>
#include <iomanip>

#include "Lib\String.h"

#include "DebugCon.h"

namespace SMBC
{
	std::string Json::ReadWholeFile(std::ifstream& input)
	{
		std::string _Output = "";

		if (input.is_open())
		{
			bool is_inString = false;
			for (std::string str; std::getline(input, str);)
			{
				for (std::size_t a = 0; a < str.size(); a++)
				{
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

	nlohmann::json Json::LoadParseJson(const std::wstring& path, const bool& _stringify)
	{
		try
		{
			std::ifstream _InputFile(path);
			if (_InputFile.is_open())
			{
				std::string _RawJson;

				if (_stringify)
					_RawJson = Json::ReadWholeFile(_InputFile);
				else
				{
					std::stringstream sstream;

					sstream << _InputFile.rdbuf();
					_RawJson = sstream.str();
				}

				return nlohmann::json::parse(_RawJson, nullptr, true, true);
			}
			#ifdef _DEBUG
			else
			{
				DebugErrorL("Couldn't open the specified file: ", path);
			}
			#endif
		}
	#ifdef _DEBUG
		catch (nlohmann::json::parse_error& err)
		{
			DebugErrorL("Couldn't load the specified json file:\nFile: ", path, "\nByte: ", err.byte, "\nId: ", err.id, "\nError Message: ", err.what());
		}
	#else
		catch (...) {}
	#endif

		return nlohmann::json();
	}

	void Json::WriteToFile(const std::wstring& path, const nlohmann::json& mJson)
	{
		std::ofstream mOutput(path);
		if (!mOutput.is_open()) return;

		mOutput << std::setw(1) << std::setfill('\t') << mJson;
	}

	const nlohmann::json& Json::Get(const nlohmann::json& json, const std::string& key)
	{
		if (json.find(key) != json.end())
			return json.at(key);

		return Json::NullObject;
	}

	const nlohmann::json& Json::Get(const nlohmann::json& json, const std::size_t& key)
	{
		if (key < json.size())
			return json[key];

		return Json::NullObject;
	}

	std::wstring Json::GetWstr(nlohmann::json& json, const std::string& key)
	{
		std::wstring _Output = L"";

		if (json.contains(key))
		{
			auto& _value = json.at(key);

			if (_value.is_string())
				_Output.append(SMBC::String::ToWide(_value.get<std::string>()));
		}

		return _Output;
	}
}