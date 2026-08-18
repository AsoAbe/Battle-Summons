#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Resource.h"

Resource::Resource(void)
{
	type_ = TYPE::NONE;
	path_ = "";

	numX_ = INVALID_HANDLE;
	numY_ = INVALID_HANDLE;
	sizeX_ = INVALID_HANDLE;
	sizeY_ = INVALID_HANDLE;

	handleId_ = INVALID_HANDLE;
}

Resource::Resource(TYPE type, const std::string& path)
{
	type_ = type;
	path_ = path;

	numX_ = INVALID_HANDLE;
	numY_ = INVALID_HANDLE;
	sizeX_ = INVALID_HANDLE;
	sizeY_ = INVALID_HANDLE;

	handleId_ = INVALID_HANDLE;
}

Resource::Resource(TYPE type,
	const std::string& path,
	int numX,
	int numY,
	int sizeX,
	int sizeY)
{
	type_ = type;
	path_ = path;

	numX_ = numX;
	numY_ = numY;
	sizeX_ = sizeX;
	sizeY_ = sizeY;

	handleId_ = INVALID_HANDLE;
}

Resource::~Resource(void)
{
}

void Resource::Load(void)
{

	switch (type_)
	{
	case Resource::TYPE::IMG:
		// âÊëú
		handleId_ = LoadGraph(path_.c_str());
		break;

	case Resource::TYPE::IMGS:
		// ï°êîâÊëú
		handleIds_.resize(numX_ * numY_);

		LoadDivGraph(
			path_.c_str(),
			numX_ * numY_,
			numX_, numY_,
			sizeX_, sizeY_,
			handleIds_.data());
		break;

	case Resource::TYPE::MODEL:
		// ÉÇÉfÉã
		handleId_ = MV1LoadModel(path_.c_str());
		break;

	case Resource::TYPE::EFFEKSEER:

		handleId_ = LoadEffekseerEffect(path_.c_str());
		break;

	}

}

void Resource::Release(void)
{

	switch (type_)
	{
	case Resource::TYPE::IMG:
		DeleteGraph(handleId_);
		break;

	case Resource::TYPE::IMGS:
	{
		int num = numX_ * numY_;
		for (int i = 0; i < num; i++)
		{
			DeleteGraph(handleIds_[i]);
		}
		handleIds_.clear();
	}
		break;

	case Resource::TYPE::MODEL:
	{
		MV1DeleteModel(handleId_);
		
		auto ids = duplicateModelIds_;
		
		for (auto id : ids)
		{
			MV1DeleteModel(id);
		}
	}
		break;

	case Resource::TYPE::EFFEKSEER:

		DeleteEffekseerEffect(handleId_);
		break;

	}

}

void Resource::CopyHandle(int* imgs)
{

	if (handleIds_.empty())
	{
		return;
	}

	int num = numX_ * numY_;
	for (int i = 0; i < num; i++)
	{
		imgs[i] = handleIds_[i];
	}

}
