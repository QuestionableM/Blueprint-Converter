#include "Json.h"

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
		std::ifstream l_input_file(path);
		if (l_input_file.is_open())
		{
			std::string l_raw_json;

			if (_stringify)
			{
				l_raw_json = Json::ReadWholeFile(l_input_file);
			}
			else
			{
				std::stringstream sstream;

				sstream << l_input_file.rdbuf();
				l_raw_json = sstream.str();
			}

			nlohmann::json l_output_json = nlohmann::json::parse(l_raw_json, nullptr, false, true);
			if (l_output_json.is_discarded())
			{
				DebugOutL("Couldn't parse json file: ", path);
				return nlohmann::json();
			}

			return l_output_json;
		}
	#ifdef _DEBUG
		else
		{
			DebugErrorL("Couldn't open the specified file: ", path);
		}
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
		const nlohmann::detail::iter_impl<const nlohmann::json> it = json.find(key);
		if (it != json.end())
			return it.value();

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