#include "BlueprintConverter.h"

#include "Blueprint Converter/Object Definitions/Entity/BlueprintData/BlueprintData.h"
#include "Blueprint Converter/Convert Settings/ConvertSettings.h"

#include <gtc/matrix_transform.hpp>

namespace SMBC
{
	namespace BPFunction
	{
		//written by Brent Batch in C# and translated by Questionable Mark into C++
		void GetPartPosAndBounds(
			glm::vec3& pos,
			glm::vec3& bounds,
			const int& xAxis,
			const int& zAxis,
			const bool& isJoint
		) {
			int _XAxisAbs = glm::abs(xAxis);
			int _ZAxisAbs = glm::abs(zAxis);

			switch (_XAxisAbs) {
			case 3:
				switch (_ZAxisAbs) {
				case 1:
					bounds = glm::vec3(bounds.z, bounds.y, bounds.x);
					break;
				case 2:
					bounds = glm::vec3(bounds.y, bounds.z, bounds.x);
					break;
				}
				break;
			case 2:
				switch (_ZAxisAbs)
				{
				case 1:
					bounds = glm::vec3(bounds.z, bounds.x, bounds.y);
					break;
				case 3:
					bounds = glm::vec3(bounds.y, bounds.x, bounds.z);
					break;
				}
				break;
			case 1:
				if (_ZAxisAbs == 2)
					bounds = glm::vec3(bounds.x, bounds.z, bounds.y);
				break;
			}

			if (!isJoint) {
				if (xAxis == -1 || zAxis == -1 || (xAxis == 2 && zAxis == 3) || (xAxis == 3 && zAxis == -2) || (xAxis == -2 && zAxis == -3) || (xAxis == -3 && zAxis == 2))
					pos.x -= bounds.x;
				if (xAxis == -2 || zAxis == -2 || (xAxis == -1 && zAxis == 3) || (xAxis == -3 && zAxis == -1) || (xAxis == 1 && zAxis == -3) || (xAxis == 3 && zAxis == 1))
					pos.y -= bounds.y;
				if (xAxis == -3 || zAxis == -3 || (xAxis == -2 && zAxis == 1) || (xAxis == -1 && zAxis == -2) || (xAxis == 1 && zAxis == 2) || (xAxis == 2 && zAxis == -1))
					pos.z -= bounds.z;
			}
			else {
				if (!(zAxis > 0 || !(bounds.x != 1 || bounds.y != 1 || bounds.z != 1))) {
					switch (zAxis) {
					case -1:
						pos.x -= bounds.x - 1;
						break;
					case -2:
						pos.y -= bounds.y - 1;
						break;
					case -3:
						pos.z -= bounds.z - 1;
						break;
					}
				}
			}
		}

		//written by Brent Batch in C# and translated by Questionable Mark into C++
		glm::mat4 GetPartRotationMatrix(const int& xAxis, const int& zAxis)
		{
			bool _XPos = (xAxis > 0);
			bool _ZPos = (zAxis > 0);
			int _AbsX = glm::abs(xAxis);
			int _AbsZ = glm::abs(zAxis);

			glm::mat4 _XRotation(1.0f);
			glm::mat4 _ZRotation(1.0f);

			switch (_AbsX)
			{
			case 1:
				if (!_XPos) _XRotation = glm::rotate(_XRotation, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				switch (_AbsZ)
				{
				case 2:
					_ZRotation = glm::rotate(_ZRotation, glm::radians(-90.0f), glm::vec3(_ZPos ? -1.0f : 1.0f, 0.0f, 0.0f));
					break;
				case 3:
					if (!_ZPos) _ZRotation = glm::rotate(_ZRotation, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					break;
				}
				break;
			case 2:
				_XRotation = glm::rotate(_XRotation, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, _XPos ? 1.0f : -1.0f));
				switch (_AbsZ)
				{
				case 1:
					_ZRotation = glm::rotate(_ZRotation, glm::radians(-90.0f), glm::vec3(0.0f, _ZPos ? 1.0f : -1.0f, 0.0f));
					break;
				case 3:
					if (!_ZPos) _ZRotation = glm::rotate(_ZRotation, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					break;
				}
				break;
			case 3:
				_XRotation = glm::rotate(_XRotation, glm::radians(-90.0f), glm::vec3(0.0f, _XPos ? -1.0f : 1.0f, 0.0f));
				switch (_AbsZ)
				{
				case 1:
					if (_ZPos == _XPos) _ZRotation = glm::rotate(_ZRotation, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					break;
				case 2:
					_ZRotation = glm::rotate(_ZRotation, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, (_ZPos == _XPos) ? -1.0f : 1.0f));
					break;
				}
				break;
			}

			return _XRotation * _ZRotation;
		}

		glm::vec3 GetPartRotationFromMatrix(const glm::vec3& pos, const glm::vec3& bounds, const glm::mat4& matrix)
		{
			glm::vec3 output_pos = glm::vec4(pos, 1.0f) * matrix;
			output_pos += bounds / 2.0f;

			return output_pos;
		}

		void ConvertBlueprintToObj(const std::wstring& bp_path, const std::wstring& bp_name, ConvertError& cError)
		{
			BlueprintData* bpData = BlueprintData::LoadFromFile(bp_path, cError);
			if (cError || !bpData) return;

			bpData->WriteToFile(bp_name, cError);

			delete bpData;
		}
	}
}