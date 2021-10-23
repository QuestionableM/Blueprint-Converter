#pragma once

#include "Object Database/Mod Data/ModData.h"
#include "Lib/Color/Color.h"

namespace SMBC
{
	struct OffsetData;
	struct Model;

	struct Object
	{
		SMBC::Uuid Uuid;
		SMBC::Color Color;
		int xAxis;
		int zAxis;
		glm::vec3 Position;
		glm::vec3 Bounds;
		std::size_t _index;

		virtual ObjectType Type() const = 0;
		virtual void CalcCenterPoint(OffsetData& data) const = 0;
		virtual void WriteToFile(std::ofstream& out, OffsetData& data, const std::size_t& idx, const glm::vec3& offsetVec) const = 0;
		virtual void SetModelPtr(Model* model_ptr) = 0;
		virtual ~Object() = default;
	protected:
		static const glm::mat4 RotationMatrix;

		void WriteVertices(const std::vector<glm::vec3>& vertices, std::ofstream& out, const glm::vec3& offsetVec) const;
		static void WriteUvs(const std::vector<glm::vec2>& uvs, std::ofstream& out);
		void WriteNormals(const std::vector<glm::vec3>& normals, std::ofstream& out) const;
		void WriteTextures(std::ofstream& out, const std::size_t& sub_data_idx, const bool& write_sub_idx) const;
		static void WriteIndices(const std::vector<std::vector<std::vector<long long>>>& data_idx, OffsetData& oData, std::ofstream& out);
		static void WriteObjectSeparator(std::ofstream& out, const std::string& name, const std::size_t& idx);
	};

	struct Part : public Object
	{
		SMBC::PartData* objPtr = nullptr;
		SMBC::Model* modelPtr = nullptr;

		ObjectType Type() const override;
		void CalcCenterPoint(OffsetData& data) const override;
		void WriteToFile(std::ofstream& out, OffsetData& data, const std::size_t& idx, const glm::vec3& offsetVec) const override;
		void SetModelPtr(Model* model_ptr) override;
		~Part() = default;
	};

	struct Block : public Object
	{
		SMBC::BlockData* blkPtr = nullptr;

		ObjectType Type() const override;
		void CalcCenterPoint(OffsetData& data) const override;
		void WriteToFile(std::ofstream& out, OffsetData& data, const std::size_t& idx, const glm::vec3& offsetVec) const override;
		void SetModelPtr(Model* model_ptr) override;
		~Block() = default;
	};
}