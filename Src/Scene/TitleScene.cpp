#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/SkyDome.h"
#include "../UI/BGM.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
{
	imgPush_ = INVALID_HANDLE;
	imgTitle_ = INVALID_HANDLE;
	skyDome_ = nullptr;
	animationController_ = nullptr;

	bgmplay_ = INITIAL_COUNT;
	bgmcount_ = INITIAL_COUNT;
	bgmtamesi_ = INITIAL_COUNT;

	pushAlpha_ = PUSH_ALPHA_INITIAL;
	pushAlphaDir_ = PUSH_ALPHA_INITIAL;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{


	// 画像読み込み
	imgTitle_ = resMng_.Load(ResourceManager::SRC::TITLE).handleId_;
	imgPush_ = resMng_.Load(ResourceManager::SRC::PUSH_SPACE).handleId_;

	// 背景
	spaceDomeTran_.pos = AsoUtility::VECTOR_ZERO;
	skyDome_ = std::make_unique<SkyDome>(spaceDomeTran_);
	skyDome_->Init();

	float size = INITIAL_VALUE;

	// キャラ
	charactor_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::PLAYER));
	charactor_.pos = { CHARACTER_POS_X,CHARACTER_POS_Y,CHARACTER_POS_Z };
	size = CHARACTER_SCALE;
	charactor_.scl = { size, size, size };
	charactor_.quaRot = Quaternion::Euler(
		INITIAL_VALUE, AsoUtility::Deg2RadF(CHARACTER_ROT_Y), INITIAL_VALUE);
	charactor_.Update();

	// アニメーションの設定
	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = std::make_unique<AnimationController>(charactor_.modelId);
	animationController_->Add(RUN_ANIM_ID, path + "Run.mv1", RUN_ANIM_SPEED);
	animationController_->Play(RUN_ANIM_ID);

	// 定点カメラ
	mainCamera.ChangeMode(Camera::MODE::FIXED_POINT);

	bgm_.Set(BGM::BgmType::TITLE);

	SoundManager::GetInstance().ChangeBGM(
		static_cast<SoundManager::SOUND_ID>(BGM::ToSoundID(BGM::BgmType::TITLE)),
		true,
		SoundManager::VOLUME_BGM
	);

	//Startを点滅
	pushAlpha_ = PUSH_ALPHA_MAX;
	pushAlphaDir_ = PUSH_ALPHA_DIR_MINUS;
}

void TitleScene::Update(void)
{

	// シーン遷移
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		//Player生存判定初期化
		SceneManager::GetInstance().SetPlayerAlive(true);
		SoundManager::GetInstance().PlaySE(
			SoundManager::SOUND_ID::SUCCESS,
			true,
			SoundManager::VOLUME_STANDARD
		);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}

	// キャラアニメーション
	animationController_->Update();

	skyDome_->Update();

	// --- PUSH SPACE 点滅 ---
	pushAlpha_ += pushAlphaDir_ * PUSH_ALPHA_SPEED;   // 数値小さいほどゆっくり

	if (pushAlpha_ <= PUSH_ALPHA_MIN)
	{
		pushAlpha_ = PUSH_ALPHA_MIN;
		pushAlphaDir_ = PUSH_ALPHA_DIR_PLUS;    // フェードインへ
	}
	else if (pushAlpha_ >= PUSH_ALPHA_MAX)
	{
		pushAlpha_ = PUSH_ALPHA_MAX;
		pushAlphaDir_ = PUSH_ALPHA_DIR_MINUS;   // フェードアウトへ
	}

}

void TitleScene::Draw(void)
{

	skyDome_->Draw();

	MV1DrawModel(planet_.modelId);
	MV1DrawModel(movePlanet_.modelId);
	MV1DrawModel(charactor_.modelId);

	DrawRotaGraph(Application::SCREEN_SIZE_X / SCREEN_CENTER_DIVISOR,
		TITLE_DRAW_Y,
		TITLE_DRAW_SCALE,
		DRAW_ROTATION_ZERO,
		imgTitle_,
		true
	);
	
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)pushAlpha_);
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / SCREEN_CENTER_DIVISOR,
		PUSH_DRAW_Y,
		PUSH_DRAW_SCALE,
		DRAW_ROTATION_ZERO,
		imgPush_,
		true
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, BLEND_ALPHA_MIN);
}

bool TitleScene::Release(void)
{
	return false;
}
