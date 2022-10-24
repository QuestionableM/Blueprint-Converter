#include "Json.h"

#include <sstream>

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

	std::wstring Json::GetWstr(nlohmann::json& json, const std::string& key)
	{
		if (json.contains(key))
		{
			const auto& v_value = json.at(key);

			if (v_value.is_string())
				return SMBC::String::ToWide(v_value.get_ref<const std::string&>());
		}

		return L"";
	}
}