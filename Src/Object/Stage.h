#pragma once
#include <memory>
#include <map>
#include <vector>
#include "Common/Transform.h"
#include "WarpStar.h"
class ResourceManager;
class WarpStar;
class Planet;
class Player;
class EnemyGolem;

class Stage
{

public:

	// ステージの切り替え間隔
	static constexpr float TIME_STAGE_CHANGE = 1.0f;

	// ステージ名
	enum class NAME
	{
		MAIN_PLANET,
		FALL_PLANET,
		FLAT_PLANET_BASE,
		FLAT_PLANET_ROT01,
		FLAT_PLANET_ROT02,
		FLAT_PLANET_ROT03,
		FLAT_PLANET_ROT04,
		FLAT_PLANET_FIXED01,
		FLAT_PLANET_FIXED02,
		PLANET10,
		LAST_STAGE,
		SPECIAL_STAGE,
		STONE_WALL,
		WHITE_WALL,
	};

	// コンストラクタ
	Stage(std::shared_ptr<Player> player,
		std::shared_ptr<EnemyGolem> enemy);

	// デストラクタ
	~Stage(void);

	void Init(void);
	void Update(void);

	//不透明
	void Draw(void);

	//透明
	void DrawTranslucent(void);
	// ステージ変更
	void ChangeStage(NAME type);

	// 対象ステージを取得
	std::weak_ptr<Planet> GetPlanet(NAME type);
	std::weak_ptr<Planet> GetActivePlanet() const;
	void StoneWall(void);
	void WhiteWall(void);
private:

	// シングルトン参照
	ResourceManager& resMng_;

	// プレイヤー
	//Player& player_;
	std::shared_ptr<Player> player_;

	//敵
	//EnemyGolem& enemy_;
	std::shared_ptr<EnemyGolem> enemy_;

	// ステージアクティブになっている惑星の情報
	NAME activeName_;
	std::weak_ptr<Planet> activePlanet_;

	// 惑星
	std::map<NAME, std::shared_ptr<Planet>> planets_;

	// ワープスター
	std::vector<std::unique_ptr<WarpStar>> warpStars_;

	// 空のPlanet
	std::shared_ptr<Planet> nullPlanet;

	float step_;

	// 最初の惑星
	void MakeMainStage(void);

	// ワープスター
	void MakeWarpStar(void);

	Transform walltransform_;
	Transform whitewall_;

};
