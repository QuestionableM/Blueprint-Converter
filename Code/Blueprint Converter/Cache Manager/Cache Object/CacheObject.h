#pragma once

#include "Object Database/Object Data/ObjectData.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace SMBC
{
	struct Model;

	enum class CachedObjectType { Block, Part };
	struct CachedObject
	{
	protected:
		static nlohmann::json ConstructTexObject(const Texture::TextureList& tex);

	public:
		std::wstring color;

		virtual CachedObjectType Type() const = 0;
		virtual std::wstring GetName() const = 0;
		virtual void WriteMtlData(const std::wstring& name, std::ofstream& out) const = 0;
		virtual nlohmann::json WriteTexturePaths() const = 0;
		virtual bool IsValid() const = 0;
		virtual const Model* GetModelPtr() const = 0;
	};

	struct CachedBlock : public CachedObject
	{
		BlockData* blkPtr = nullptr;

		CachedObjectType Type() const override;
		std::wstring GetName() const override;
		void WriteMtlData(const std::wstring& name, std::ofstream& out) const override;
		nlohmann::json WriteTexturePaths() const override;
		bool IsValid() const override;
		const Model* GetModelPtr() const override;
	};

	struct CachedPart : public CachedObject
	{
		PartData* objPtr = nullptr;
		Model* modelPtr = nullptr;

		CachedObjectType Type() const override;
		std::wstring GetName() const override;
		void WriteMtlData(const std::wstring& name, std::ofstream& out) const override;
		nlohmann::json WriteTexturePaths() const override;
		bool IsValid() const override;
		const Model* GetModelPtr() const override;
	};
}