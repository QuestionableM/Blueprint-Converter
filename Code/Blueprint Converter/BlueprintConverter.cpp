#include "BlueprintConverter.h"

#include "Lib/Functions/Functions.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/Json/JsonFunc.h"

void SMBC::BPFunction::GetPartPosAndBounds(
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

#include <gtc/matrix_transform.hpp>

//written by Brent Batch in C# and translated by Questionable Mark into C++
glm::vec3 SMBC::BPFunction::GetPartRotation(
	const glm::vec3& _Position,
	const glm::vec3& bounds,
	const int& xAxis,
	const int& zAxis
) {
	bool _XPos = (xAxis > 0);
	bool _ZPos = (zAxis > 0);
	int _AbsX = glm::abs(xAxis);
	int _AbsZ = glm::abs(zAxis);

	glm::mat4 _XRotation(1.0f);
	glm::mat4 _ZRotation(1.0f);

	glm::vec3 _TranslatedPos(_Position);

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

	_TranslatedPos = glm::vec3(glm::vec4(_TranslatedPos, 1.0f) * _XRotation);
	_TranslatedPos = glm::vec3(glm::vec4(_TranslatedPos, 1.0f) * _ZRotation);
	_TranslatedPos += bounds / 2.0f;

	return _TranslatedPos;
}

#include "Blueprint Converter/Blueprint Writer/BlueprintWriter.h"

int SMBC::BPFunction::ConvertBlueprintToObj(
	const std::wstring& blueprint_path,
	const std::wstring blueprint_name,
	const bool& separate_blocks,
	const bool& texture_list,
	const bool& apply_textures,
	const bool& export_uvs,
	const bool& export_normals
) {
	int EscapeCode = 0;
	nlohmann::json* _BlueprintJson = SMBC::JSON::OpenParseJson(blueprint_path);

	SMBC::ConvertedModel::ConvertedModelData conv_data;
	conv_data.apply_texture = apply_textures;
	conv_data.export_normals = export_normals;
	conv_data.export_uvs = export_uvs;
	conv_data.separate_parts = separate_blocks;
	conv_data.tex_list = texture_list;

	SMBC::ConvertedModel _ConvModel(conv_data);
	_ConvModel.ModelName = blueprint_name;

	if (!_BlueprintJson) { EscapeCode = 1; goto skip_generation; }

	if (!(_BlueprintJson->contains("bodies") || _BlueprintJson->contains("joints"))) {
		EscapeCode = 4;
		goto skip_generation;
	}

	SMBC::BlueprintConversionData::SetNewStage(1, 0);
	if (_BlueprintJson->contains("bodies") && _BlueprintJson->at("bodies").is_array()) {
		auto& _BodiesArray = _BlueprintJson->at("bodies");

		for (auto& _Body : _BodiesArray) {
			auto& _Childs = _Body["childs"];
			if (!_Childs.is_array()) continue;

			SMBC::BlueprintConversionData::ProgressBarMax += (uint32_t)_Childs.size();
			for (auto& _Child : _Childs) {
				auto& _ShapeId = _Child["shapeId"];
				auto& _Pos = _Child["pos"];
				auto& _XAxis = _Child["xaxis"];
				auto& _ZAxis = _Child["zaxis"];
				auto& _Bounds = _Child["bounds"];

				if (!(_ShapeId.is_string() && _Pos.is_object() && _XAxis.is_number() && _ZAxis.is_number())) continue;
				auto& _PosX = _Pos["x"];
				auto& _PosY = _Pos["y"];
				auto& _PosZ = _Pos["z"];

				if (!(_PosX.is_number() && _PosY.is_number() && _PosZ.is_number())) continue;
				glm::vec3 _PosVec(_PosX.get<float>(), _PosY.get<float>(), _PosZ.get<float>());

				std::wstring _UuidWstr = SMBC::Other::Utf8ToWide(_ShapeId.get<std::string>());

				if (_Bounds.is_object()) {
					auto& _BoundX = _Bounds["x"];
					auto& _BoundY = _Bounds["y"];
					auto& _BoundZ = _Bounds["z"];

					if (!(_BoundX.is_number() && _BoundY.is_number() && _BoundZ.is_number())) continue;
					glm::vec3 _BoundsVec(_BoundX.get<float>(), _BoundY.get<float>(), _BoundZ.get<float>());

					SMBC::BlockData _BlockD;
					if (!SMBC::ObjectDatabase::GetBlockByUuid(_UuidWstr, _BlockD))
						_BlockD._obj_name = (L"Block (" + _UuidWstr + L")");

					SMBC::SM_Block _Block(
						_PosVec,
						_BoundsVec,
						_XAxis.get<int>(),
						_ZAxis.get<int>(),
						_BlockD._obj_name,
						_BlockD._block_tex,
						_UuidWstr,
						_BlockD._tiling
					);
					_ConvModel.BlockList.push_back(_Block);
				}
				else {
					SMBC::ObjectData _ObjData;

					if (!SMBC::ObjectDatabase::GetPartByUuid(_UuidWstr, _ObjData) || _ObjData._obj_path.empty()) continue;

					SMBC::SM_Part _Part(
						_ObjData._obj_path,
						_ObjData._obj_name,
						_ObjData.obj_textures,
						_PosVec,
						_ObjData._obj_bounds,
						_XAxis.get<int>(),
						_ZAxis.get<int>(),
						_ObjData._obj_uuid
					);
					SMBC::BPFunction::GetPartPosAndBounds(_Part.position, _Part.bounds, _Part.xAxis, _Part.zAxis, false);
					_ConvModel.PartList.push_back(_Part);
					SMBC::BlueprintConversionData::ProgressBarValue++;
				}
			}
		}
	}

	SMBC::BlueprintConversionData::SetNewStage(2, 0);
	if (_BlueprintJson->contains("joints") && _BlueprintJson->at("joints").is_array()) {
		auto& _JointsArray = _BlueprintJson->at("joints");

		SMBC::BlueprintConversionData::ProgressBarMax = (uint32_t)_JointsArray.size();
		for (auto& _Joint : _JointsArray) {
			auto& _Position = _Joint["posA"];
			auto& _XAxis = _Joint["xaxisA"];
			auto& _ZAxis = _Joint["zaxisA"];
			auto& _ShapeId = _Joint["shapeId"];

			if (!(_ShapeId.is_string() && _Position.is_object() && _XAxis.is_number() && _ZAxis.is_number())) continue;
			auto& _PosX = _Position["x"];
			auto& _PosY = _Position["y"];
			auto& _PosZ = _Position["z"];

			if (!(_PosX.is_number() && _PosY.is_number() && _PosZ.is_number())) continue;
			glm::vec3 _JointPos(_PosX.get<float>(), _PosY.get<float>(), _PosZ.get<float>());

			std::wstring _UuidWstr = SMBC::Other::Utf8ToWide(_ShapeId.get<std::string>());

			SMBC::ObjectData _jnt_data;
			if (!SMBC::ObjectDatabase::GetPartByUuid(_UuidWstr, _jnt_data) || _jnt_data._obj_path.empty()) continue;

			SMBC::SM_Part _jnt(
				_jnt_data._obj_path,
				_jnt_data._obj_name,
				_jnt_data.obj_textures,
				_JointPos,
				_jnt_data._obj_bounds,
				_XAxis.get<int>(),
				_ZAxis.get<int>(),
				_UuidWstr
			);
			SMBC::BPFunction::GetPartPosAndBounds(_jnt.position, _jnt.bounds, _jnt.xAxis, _jnt.zAxis, true);
			_ConvModel.PartList.push_back(_jnt);
			SMBC::BlueprintConversionData::ProgressBarValue++;
		}
	}

	EscapeCode = _ConvModel.ConvertAndWrite();
	//EscapeCode = _ConvModel.WriteToFileNewVersion(texture_list, apply_textures, export_uvs, export_normals, separate_blocks);

skip_generation:
	delete _BlueprintJson;
	return EscapeCode;
}