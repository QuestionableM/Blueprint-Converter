#include "ObjectStorage.h"

#include "Blueprint Converter/Cache Manager/Model Storage/ModelStorage.h"
#include "Blueprint Converter/Convert Settings/ConvertSettings.h"
#include "Lib/String/String.h"
#include "DebugCon.h"

#include <nlohmann/json.hpp>
#include <iomanip>

namespace SMBC
{
	/*std::unordered_map<std::wstring, CachedObject*> ObjectStorage::CachedObjects = {};

	CachedObject* ObjectStorage::LoadCachedBlock(SMBC::Block* block)
	{
		CachedBlock* new_block = new CachedBlock();
		new_block->blkPtr = block->blkPtr;
		new_block->color = block->Color;

		return new_block;
	}

	CachedObject* ObjectStorage::LoadCachedPart(SMBC::Part* part)
	{
		CachedPart* new_part = new CachedPart();
		new_part->objPtr = part->objPtr;
		new_part->color = part->Color;
		new_part->modelPtr = ModelStorage::LoadModel(new_part->objPtr->Path, ConvertSettings::ExportUvs, ConvertSettings::ExportNormals);
		part->modelPtr = new_part->modelPtr;

		return new_part;
	}

	void ObjectStorage::LoadObject(Object* object)
	{
		std::wstring cUuidStr = object->Uuid.ToWstring();
		if (ConvertSettings::MatByColor)
			String::Combine(cUuidStr, L" ", object->Color.StringHex());

		if (CachedObjects.find(cUuidStr) != CachedObjects.end())
		{
			CachedObject* cObject = CachedObjects.at(cUuidStr);
			object->SetModelPtr((SMBC::Model*)cObject->GetModelPtr());

			return;
		}
		else
		{
			CachedObject* new_object = nullptr;
			if (object->Type() == ObjectType::Block)
			{
				new_object = ObjectStorage::LoadCachedBlock(static_cast<Block*>(object));
			}
			else
			{
				new_object = ObjectStorage::LoadCachedPart(static_cast<Part*>(object));
			}

			CachedObjects.insert(std::make_pair(cUuidStr, new_object));
		}
	}

	void ObjectStorage::WriteMtlFile(const std::wstring& path)
	{
		std::ofstream _MtlFile(path);
		if (!_MtlFile.is_open()) return;

		for (const auto& object_data : CachedObjects)
		{
			const SMBC::CachedObject* cObject = object_data.second;
			cObject->WriteMtlData(object_data.first, _MtlFile);
		}

		_MtlFile.close();
	}

	std::string ObjectStorage::GetFullObjectName(const CachedObject* cObject)
	{
		const Mod* cModPtr = cObject->GetModPtr();
		const std::wstring cObjName = cObject->GetName();

		const std::wstring output_str = cObjName + L" (" + cModPtr->Name + L": " + cObject->GetUuid().ToWstring() + L")";
		return String::ToUtf8(output_str);
	}

	void ObjectStorage::WriteTexturePaths(const std::wstring& path)
	{
		std::ofstream _TextureOutput(path);
		if (!_TextureOutput.is_open()) return;

		nlohmann::json _TextureList;

		for (const auto& object_data : CachedObjects)
		{
			const CachedObject* cObject = object_data.second;
			const std::string cObjectFullName = ObjectStorage::GetFullObjectName(cObject);

			if (!cObject->IsValid()) continue;

			_TextureList[cObjectFullName] = cObject->WriteTexturePaths();
		}

		_TextureOutput << std::setfill('\t') << std::setw(1) << _TextureList;
		_TextureOutput.close();
	}

	void ObjectStorage::ClearStorage() {
		DebugOutL("Clearing object storage");

		for (const auto& obj_data : CachedObjects)
			delete obj_data.second;

		CachedObjects.clear();
	}*/
}