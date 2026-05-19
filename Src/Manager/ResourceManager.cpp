#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
{

	// 推奨しませんが、どうしても使いたい方は
	using RES = Resource;
	using RES_T = RES::TYPE;
	const std::string PATH_IMG = Application::PATH_IMAGE;
	const std::string PATH_MDL = Application::PATH_MODEL;
	const std::string PATH_EFF = Application::PATH_EFFECT;
	const std::string PATH_STA = Application::PATH_STAGE;

	std::unique_ptr<Resource> res;

	// タイトル画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "AGSTitle.png");
	resourcesMap_.emplace(SRC::TITLE, std::move(res));

	// PushSpace
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "PushSpace.png");
	resourcesMap_.emplace(SRC::PUSH_SPACE, std::move(res));

	// 吹き出し
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "SpeechBalloon.png");
	resourcesMap_.emplace(SRC::SPEECH_BALLOON, std::move(res));
	
	// プレイヤー
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Player/Vampire.mv1");
	resourcesMap_.emplace(SRC::PLAYER, std::move(res));

	//敵
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Golem/Golem.mv1");
	resourcesMap_.emplace(SRC::ENEMY, std::move(res));	

	// プレイヤー影
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Shadow.png");
	resourcesMap_.emplace(SRC::PLAYER_SHADOW, std::move(res));

	// スカイドーム
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "SkyDome/SkyDome.mv1");
	resourcesMap_.emplace(SRC::SKY_DOME, std::move(res));

	// 最初の惑星
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Planet/MainPlanet.mv1");
	resourcesMap_.emplace(SRC::MAIN_PLANET, std::move(res));
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Stage/Stage.mv1");
	resourcesMap_.emplace(SRC::MAIN_PLANET2, std::move(res));


	// ワープスターモデル
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Star/star.mv1");
	resourcesMap_.emplace(SRC::WARP_STAR, std::move(res));

	// ワープスター用回転エフェクト
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "StarDust/StarDust.efkefc");
	resourcesMap_.emplace(SRC::WARP_STAR_ROT_EFF, std::move(res));

	// ワープの軌跡線
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "Warp/WarpOrbit.efkefc");
	resourcesMap_.emplace(SRC::WARP_ORBIT, std::move(res));

	// Clear
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Congratulations.png");
	resourcesMap_.emplace(SRC::CLEAR, std::move(res));

//　SHOT関連
	// 弾
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Shot/Shot.mv1");
	resourcesMap_.emplace(SRC::SHOT_MODEL, std::move(res));

	// 弾の爆発
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "Blast.efkefc");
	resourcesMap_.emplace(SRC::SHOT_EXPLOSION, std::move(res));

//　CANNON関連
	// 大砲の砲台
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Cannon/Stand.mv1");
	resourcesMap_.emplace(SRC::CANNON_STAND, std::move(res));
	
	// 大砲の砲身
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Cannon/Barrel.mv1");
	resourcesMap_.emplace(SRC::CANNON_BARREL, std::move(res));

	// 砲弾
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Cannon/Shot.mv1");
	resourcesMap_.emplace(SRC::CANNON_SHOT, std::move(res));
	// 砲弾の爆発
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "Explosion.efkefc");
	resourcesMap_.emplace(SRC::CANNON_BLAST, std::move(res));

	//Golemのタックルチャージ
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "chargingOrn.efkefc");
	resourcesMap_.emplace(SRC::TACKLE_CHARGE, std::move(res));
	
	//回復
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "Heal.efkefc");
	resourcesMap_.emplace(SRC::HEAL, std::move(res));

}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second->Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	resourcesMap_.clear();
	delete instance_;
	instance_ = nullptr;
}

const Resource& ResourceManager::Load(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(SRC src)
{

	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *lPair->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	// ロード処理
	rPair->second->Load();

	// 念のためコピーコンストラクタ
	loadedMap_.emplace(src, rPair->second.get());

	return *rPair->second;

}
