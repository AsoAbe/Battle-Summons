#pragma once
#include <memory>
#include "SceneBase.h"
#include"../UI/BGM.h"
#include "../Object/Common/Transform.h"
class SceneManager;
class SkyDome;
class AnimationController;

class GameOverScene : public SceneBase
{

public:

	// コンストラクタ
	GameOverScene(void);

	// デストラクタ
	~GameOverScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	//解放
	bool  Release(void)override;

private:

	//BGM
	BGM bgm_;

	int Clear;
	int Over;

};
