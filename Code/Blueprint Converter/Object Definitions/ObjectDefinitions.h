#pragma once

#include <glm.hpp>
#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Mod Data/ModData.h"

namespace SMBC {
	struct SubMeshData
	{
		std::wstring MaterialName;
		int SubMeshIndex;

		std::vector<std::vector<std::vector<long long>>> DataIdx;

		bool IsEmpty();
	};

	struct Model
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<SMBC::SubMeshData*> subMeshData;

		std::wstring meshPath;

		bool IsEmpty();

		~Model();
	};

	struct OffsetData;

	struct Object
	{
		SMBC::Uuid Uuid;
		std::wstring Color;
		int xAxis;
		int zAxis;
		glm::vec3 Position;
		glm::vec3 Bounds;
		std::size_t _index;

		virtual ObjectType Type() const = 0;
		virtual void CalcCenterPoint(OffsetData& data) const = 0;
		virtual void WriteToFile(std::ofstream& out, OffsetData& data, const std::size_t& idx, const glm::vec3& offsetVec) const = 0;
		virtual void SetModelPtr(Model* model_ptr) = 0;
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
	};

	struct Block : public Object
	{
		SMBC::BlockData* blkPtr = nullptr;

		ObjectType Type() const override;
		void CalcCenterPoint(OffsetData& data) const override;
		void WriteToFile(std::ofstream& out, OffsetData& data, const std::size_t& idx, const glm::vec3& offsetVec) const override;
		void SetModelPtr(Model* model_ptr) override;
	};

	struct CubeMesh
	{
		std::vector<glm::vec3> Vertices;
		std::vector<glm::vec2> TexturePoints = {
			{0.0f, 1.0f}, {1.0f, 0.0f},
			{1.0f, 1.0f}, {1.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f},
			{1.0f, 0.0f}, {0.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f},
			{1.0f, 0.0f}, {0.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f},
			{0.0f, 1.0f}, {0.0f, 0.0f},
			{0.0f, 0.0f}, {0.0f, 1.0f},
			{1.0f, 1.0f}, {0.0f, 1.0f},
			{1.0f, 1.0f}, {1.0f, 1.0f}
		};

		/*
			reference
			this->TexturePoints = {
				{4.0f, 3.0f}, {5.0f, 2.0f},
				{5.0f, 3.0f}, {3.0f, 3.0f},
				{2.0f, 2.0f}, {3.0f, 2.0f},
				{5.0f, 0.0f}, {4.0f, 1.0f},
				{4.0f, 0.0f}, {1.0f, 1.0f},
				{0.0f, 0.0f}, {1.0f, 0.0f},
				{1.0f, 2.0f}, {0.0f, 3.0f},
				{0.0f, 2.0f}, {3.0f, 0.0f},
				{2.0f, 1.0f}, {2.0f, 0.0f},
				{4.0f, 2.0f}, {2.0f, 3.0f},
				{5.0f, 1.0f}, {0.0f, 1.0f},
				{1.0f, 3.0f}, {3.0f, 1.0f}
			}
		*/

		std::vector<glm::vec3> Normals = {
			{-1.0, 0.0, 0.0}, {0.0, 0.0, -1.0},
			{1.0, 0.0, 0.0}, {0.0, 0.0, 1.0},
			{0.0, -1.0, 0.0}, {0.0, 1.0, 0.0}
		};
		std::vector<std::vector<std::vector<long long>>> DataIndexes = {
			{{0, 0, 0}, {1, 1, 0}, {2, 2, 0}},
			{{3, 3, 1}, {4, 4, 1}, {1, 5, 1}},
			{{5, 6, 2}, {6, 7, 2}, {4, 8, 2}},
			{{7, 9, 3}, {2, 10, 3}, {6, 11, 3}},
			{{4, 12, 4}, {2, 13, 4}, {1, 14, 4}},
			{{3, 15, 5}, {7, 16, 5}, {5, 17, 5}},
			{{0, 0, 0}, {3, 18, 0}, {1, 1, 0}},
			{{3, 3, 1}, {5, 19, 1}, {4, 4, 1}},
			{{5, 6, 2}, {7, 20, 2}, {6, 7, 2}},
			{{7, 9, 3}, {0, 21, 3}, {2, 10, 3}},
			{{4, 12, 4}, {6, 22, 4}, {2, 13, 4}},
			{{3, 15, 5}, {0, 23, 5}, {7, 16, 5}}
		};

	public:
		CubeMesh(const glm::vec3& bounds, const glm::vec3& pos, const int& tiling);
	};
}