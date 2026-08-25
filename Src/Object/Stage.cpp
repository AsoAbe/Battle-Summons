#include <vector>
#include <map>
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "WarpStar.h"
#include "Player.h"
#include "EnemyGolem.h"
#include "Planet.h"
#include "Common/Collider.h"
#include "Common/Transform.h"
#include "Stage.h"

Stage::Stage(std::shared_ptr<Player> player,
	std::shared_ptr<EnemyGolem> enemy)
	: resMng_(ResourceManager::GetInstance())
	, player_(player)
	, enemy_(enemy)
	, activeName_(NAME::MAIN_PLANET)
	, activePlanet_()
	, planets_()
	, warpStars_()
	, nullPlanet(nullptr)
	, step_(INITIAL_VALUE)
{
	activeName_ = NAME::MAIN_PLANET;
	step_ = INITIAL_VALUE;
}

Stage::~Stage(void)
{
	
	// ワープスター
	warpStars_.clear();
	
	// 惑星
	planets_.clear();

}

void Stage::Init(NAME type)
{
	activeName_ = type;

	//メインステージ生成
	switch (type)
	{
	case NAME::MAIN_PLANET:
		MakeMainStage();
		//ステージに浮かぶ星を生成
		MakeWarpStar();
		break;

	case NAME::MAIN_PLANET2:
		MakeMainStage2();
		break;
	}

	step_ = -STEP_INACTIVE;
}

void Stage::Update(void)
{

	// ワープスター
	for (const auto& s : warpStars_)
	{
		s->Update();
	}

	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Update();
	}

}

void Stage::Draw(void)
{
	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Draw();
	}

	// 石壁
	MV1DrawModel(walltransform_.modelId);
}

void Stage::DrawTranslucent(void)
{	
	// ワープスター
	for (const auto& s : warpStars_)
	{
		s->Draw();
	}
}

void Stage::ChangeStage(NAME type)
{

	activeName_ = type;

	// 対象のステージを取得する
	activePlanet_ = GetPlanet(activeName_);

	// ステージの当たり判定をプレイヤーに設定
	player_->ClearCollider();
	player_->AddCollider(activePlanet_.lock()->GetTransform().collider);

	enemy_->ClearCollider();
	enemy_->AddCollider(activePlanet_.lock()->GetTransform().collider);

	if (type == NAME::MAIN_PLANET2)
	{
		player_->AddCollider(whitewall_.collider);
		player_->AddCollider(walltransform_.collider);
		
		enemy_->AddCollider(whitewall_.collider);
		enemy_->AddCollider(walltransform_.collider);
	}

	step_ = TIME_STAGE_CHANGE;

}

std::weak_ptr<Planet> Stage::GetPlanet(NAME type)
{
	if (planets_.count(type) == 0)
	{
		return nullPlanet;
	}

	return planets_[type];
}

std::weak_ptr<Planet> Stage::GetActivePlanet() const
{
	return activePlanet_;
}

void Stage::MakeMainStage(void)
{

	// 一つ目のステージ
	//------------------------------------------------------------------------------
	Transform planetTrans;
	planetTrans.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MAIN_PLANET));
	planetTrans.scl = AsoUtility::VECTOR_ONE;
	planetTrans.quaRot = Quaternion();
	planetTrans.pos = { INITIAL_VALUE, -MAIN_PLANET_POS_Y, INITIAL_VALUE };

	// 当たり判定(コライダ)作成
	planetTrans.MakeCollider(Collider::TYPE::STAGE);

	planetTrans.Update();

	NAME name = NAME::MAIN_PLANET;
	std::shared_ptr<Planet> planet =
		std::make_shared<Planet>(
			name, Planet::TYPE::GROUND, planetTrans);
	planet->Init();
	planets_.emplace(name, std::move(planet));

}

void Stage::MakeMainStage2(void)
{

	// 二つ目のステージ
	//------------------------------------------------------------------------------
	Transform planetTrans;
	planetTrans.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MAIN_PLANET2));
	planetTrans.scl = AsoUtility::VECTOR_ONE;
	planetTrans.quaRot = Quaternion();
	planetTrans.pos = { INITIAL_VALUE, -MAIN_PLANET_POS_Y, INITIAL_VALUE };

	// 当たり判定(コライダ)作成
	planetTrans.MakeCollider(Collider::TYPE::STAGE);

	planetTrans.Update();

	NAME name = NAME::MAIN_PLANET2;
	std::shared_ptr<Planet> planet =
		std::make_shared<Planet>(
			name, Planet::TYPE::GROUND, planetTrans);
	planet->Init();
	planets_.emplace(name, std::move(planet));

	//石壁生成
	StoneWall();

	//透明な壁生成
	WhiteWall();
}

void Stage::MakeWarpStar(void)
{

	Transform trans;
	std::unique_ptr<WarpStar> star;

	// 落とし穴惑星へのワープスター
	//------------------------------------------------------------------------------
	trans.pos = { -WARP_STAR_POS_X, WARP_STAR_POS_Y, WARP_STAR_POS_Z };
	trans.scl = { WARP_STAR_SCALE, WARP_STAR_SCALE, WARP_STAR_SCALE };
	trans.quaRot = Quaternion::Euler(
		AsoUtility::Deg2RadF(WARP_STAR_ROT_X),
		AsoUtility::Deg2RadF(WARP_STAR_ROT_Y),
		AsoUtility::Deg2RadF(INITIAL_VALUE)
	);

	star = std::make_unique<WarpStar>(player_, trans);
	star->Init();
	warpStars_.push_back(std::move(star));
	//------------------------------------------------------------------------------

}


//石壁
void Stage::StoneWall(void)
{
	walltransform_.SetModel(
		resMng_.LoadModelDuplicate(
			ResourceManager::SRC::STONE_WALL));

	walltransform_.scl = AsoUtility::VECTOR_ONE;
	walltransform_.quaRot = Quaternion();
	walltransform_.pos = { 0.0f, -100.0f, 0.0f };

	// 当たり判定(コライダ)作成
	walltransform_.Update();

	walltransform_.MakeCollider(Collider::TYPE::WALL);

}

//透明な壁
void Stage::WhiteWall(void)
{

	whitewall_.SetModel(
		resMng_.LoadModelDuplicate(
			ResourceManager::SRC::WHITE_WALL));

	whitewall_.scl = AsoUtility::VECTOR_ONE;
	whitewall_.quaRot = Quaternion();
	whitewall_.pos = { 0.0f, -100.0f, 0.0f };

	// 当たり判定(コライダ)作成
	whitewall_.MakeCollider(Collider::TYPE::WALL);

	whitewall_.Update();

	player_->AddCollider(whitewall_.collider);

}
