#pragma once

#include <fstream>
#include <iomanip>
#include <string>

#include <nlohmann/json.hpp>

namespace SMBC
{
	class Json
	{
	public:
		static std::string ReadWholeFile(std::ifstream& input);
		static nlohmann::json LoadParseJson(const std::wstring& path, const bool& _stringify = false);

		inline static void WriteToFile(const std::wstring& path, const nlohmann::json& mJson)
		{
			std::ofstream v_output(path);
			if (!v_output.is_open())
				return;

			v_output << std::setw(1) << std::setfill('\t') << mJson;
			v_output.close();
		}

		inline static const nlohmann::json& Get(const nlohmann::json& json, const std::string& key)
		{
			const auto it = json.find(key);
			if (it != json.end())
				return it.value();

			return Json::m_NullObject;
		}

		inline static const nlohmann::json& Get(const nlohmann::json& json, const std::size_t& key)
		{
			if (key < json.size())
				return json[key];

			return Json::m_NullObject;
		}

		static std::wstring GetWstr(const nlohmann::json& json, const std::string& key);

	private:
		inline const static nlohmann::json m_NullObject = nlohmann::json();
	};
}