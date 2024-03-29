#include "ObjectRotations.hpp"

#include "BlueprintConverter\ObjectDefinitions\Entity.hpp"

#include "Lib\ProgramSettings.hpp"
#include "Lib\Json.hpp"

#include "DebugCon.hpp"

#include <gtc\matrix_transform.hpp>
#include <glm.hpp>

#include <unordered_map>
#include <string>

namespace SMBC
{
	namespace Rotations
	{
		struct RotationEntry
		{
			glm::mat4 rot_matrix;
			glm::vec3 pos_vector;
		};

		struct OffsetVector3
		{
			float x, y, z;
		};

		struct RotationSettings
		{
			OffsetVector3 oRotation;
			OffsetVector3 oPosition;
		};

		using RotationEntryMap = std::unordered_map<int, std::unordered_map<int, RotationEntry>>;
		using RotationSettingsMap = std::unordered_map<int, std::unordered_map<int, RotationSettings>>;

		static RotationEntryMap RotationTable = {};
		static RotationSettingsMap RotationsForEditor = {};

		void LoadRotationsFromFile()
		{
			nlohmann::json rSettings = Json::LoadParseJson(SMBC::Settings::RotSetPath.data());
			if (!rSettings.is_object())
			{
				DebugErrorL("Couldn't read rotation configuration file!");
				return;
			}

			DebugOutL("Loading rotations from file...");

			const auto& rRotations = Json::Get(rSettings, "Rotations");
			if (!rRotations.is_object()) return;

			for (auto& xRotationGroup : RotationsForEditor)
			{
				const auto& xRotationSet = Json::Get(rRotations, std::to_string(xRotationGroup.first));
				if (!xRotationSet.is_object()) continue;

				for (auto& zRotationGroup : xRotationGroup.second)
				{
					const auto& zRotationSet = Json::Get(xRotationSet, std::to_string(zRotationGroup.first));
					if (!zRotationSet.is_object()) continue;

					const auto& zRotRotations = Json::Get(zRotationSet, "Rotation");
					if (zRotRotations.is_object())
					{
						zRotationGroup.second.oRotation.x = Json::Get(zRotRotations, "x").get<float>();
						zRotationGroup.second.oRotation.y = Json::Get(zRotRotations, "y").get<float>();
						zRotationGroup.second.oRotation.z = Json::Get(zRotRotations, "z").get<float>();
					}

					const auto& zRotOffsets = Json::Get(zRotationSet, "Offset");
					if (zRotOffsets.is_object())
					{
						zRotationGroup.second.oPosition.x = Json::Get(zRotOffsets, "x").get<float>();
						zRotationGroup.second.oPosition.y = Json::Get(zRotOffsets, "y").get<float>();
						zRotationGroup.second.oPosition.z = Json::Get(zRotOffsets, "z").get<float>();
					}
				}
			}
		}

		void InitializeRotationEditor()
		{
			std::unordered_map<int, RotationSettings> xAxis1Settings = {
				{ 2,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ 3,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -2, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -3, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } }
			};

			std::unordered_map<int, RotationSettings> xAxis2Settings = {
				{ 1,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ 3,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -1, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -3, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } }
			};

			std::unordered_map<int, RotationSettings> xAxis3Settings = {
				{ 1,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ 2,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -1, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -2, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } }
			};

			std::unordered_map<int, RotationSettings> xAxisNeg1Settings = {
				{ 2,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ 3,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -2, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -3, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } }
			};

			std::unordered_map<int, RotationSettings> xAxisNeg2Settings = {
				{ 1,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ 3,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -1, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -3, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } }
			};

			std::unordered_map<int, RotationSettings> xAxisNeg3Settings = {
				{ 1,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ 2,  { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -1, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } },
				{ -2, { 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f} } }
			};

			RotationsForEditor.insert(std::make_pair(1, xAxis1Settings));
			RotationsForEditor.insert(std::make_pair(2, xAxis2Settings));
			RotationsForEditor.insert(std::make_pair(3, xAxis3Settings));
			RotationsForEditor.insert(std::make_pair(-1, xAxisNeg1Settings));
			RotationsForEditor.insert(std::make_pair(-2, xAxisNeg2Settings));
			RotationsForEditor.insert(std::make_pair(-3, xAxisNeg3Settings));
		}

		void InitializeRotations()
		{
			InitializeRotationEditor();
			LoadRotationsFromFile();

			RotationTable.reserve(RotationsForEditor.size());
			for (const auto& xAxisRotGroup : RotationsForEditor)
			{
				std::unordered_map<int, RotationEntry> rotation_group = {};

				rotation_group.reserve(xAxisRotGroup.second.size());
				for (const auto& zAxisRotGroup : xAxisRotGroup.second)
				{
					const RotationSettings& rSetRef = zAxisRotGroup.second;

					glm::mat4 rotation_matrix(1.0f);
					rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rSetRef.oRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
					rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rSetRef.oRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
					rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rSetRef.oRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

					const RotationEntry new_entry = { rotation_matrix, { rSetRef.oPosition.x, rSetRef.oPosition.y, rSetRef.oPosition.z } };

					rotation_group.insert(std::make_pair(zAxisRotGroup.first, new_entry));
				}

				RotationTable.insert(std::make_pair(xAxisRotGroup.first, rotation_group));
			}

			DebugOutL(0b0101_fg, "Rotations have been initialized!");
		}

		bool RotationExists(const int& xAxis, const int& zAxis)
		{
			const RotationEntryMap::const_iterator x_it = RotationTable.find(xAxis);
			if (x_it != RotationTable.end())
			{
				const std::unordered_map<int, RotationEntry>& zAxisRot = x_it->second;
				if (zAxisRot.find(zAxis) != zAxisRot.end())
					return true;
			}

			DebugErrorL("Couldn't find the specified rotation: (", xAxis, ", ", zAxis, ")");
			return false;
		}

		glm::mat4 GetRotationMatrix(const AxisData& mAxis)
		{
			return RotationTable[mAxis.x][mAxis.z].rot_matrix;
		}

		glm::vec3 GetOffsetPosition(const AxisData& mAxis)
		{
			return RotationTable[mAxis.x][mAxis.z].pos_vector;
		}
	}
}