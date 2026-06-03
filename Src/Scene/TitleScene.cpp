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

namespace {
	constexpr int appear_interval = 20;  // 出現までのフレーム
	constexpr int menu_line_height = 40; // メニューの1つあたりの高さ
	constexpr int margin_size = 20;      // ポーズメニュー枠の余白

	constexpr int CONTROL_TEXT_SIZE = 18;

	//SoundManager側をそのまま使うと計算が合わなくなるため、後で数値を書き換えてこの定数は不要にする
	constexpr int VOLUME_BGM_TITLE = static_cast<int>(SoundManager::VOLUME_BGM * 1.17f);
	constexpr int VOLUME_BGM_MAIN = 255 * 75 / 100;

	constexpr int TITLE_MODEL_ROT_X = -90;

	// キャラクター配置位置
	constexpr float CHARACTER_POS_X = -250.0f;
	constexpr float CHARACTER_POS_Y = -32.0f;
	constexpr float CHARACTER_POS_Z = -105.0f;

	// キャラクター表示倍率
	constexpr float CHARACTER_SCALE = 0.4f;

	// キャラクター初期向き(Y軸回転角度)
	constexpr float CHARACTER_ROT_Y = 90.0f;

	// 走るアニメーション
	constexpr float RUN_ANIM_SPEED = 20.0f;

	// PUSH SPACE 点滅設定
	constexpr float PUSH_ALPHA_MAX = 255.0f;
	constexpr float PUSH_ALPHA_MIN = 50.0f;
	constexpr float PUSH_ALPHA_SPEED = 2.0f;
	constexpr float PUSH_ALPHA_DIR_PLUS = 1.0f;
	constexpr float PUSH_ALPHA_DIR_MINUS = -1.0f;

	// タイトル画像描画位置
	constexpr int TITLE_DRAW_Y = 350;
	constexpr double TITLE_DRAW_SCALE = 0.7;

	// PUSH SPACE画像描画位置
	constexpr int PUSH_DRAW_Y = 500;
	constexpr double PUSH_DRAW_SCALE = 1.0;
}

TitleScene::TitleScene(void)
{
	imgPush_ = -1;
	imgTitle_ = -1;
	skyDome_ = nullptr;
	animationController_ = nullptr;

	bgmplay_ = 0;
	bgmcount_ = 0;
	bgmtamesi_ = 0;

	pushAlpha_ = 0.0f;
	pushAlphaDir_ = 0.0f;
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

	float size = 0.0f;

	// キャラ
	charactor_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::PLAYER));
	charactor_.pos = { CHARACTER_POS_X,CHARACTER_POS_Y,CHARACTER_POS_Z };
	size = CHARACTER_SCALE;
	charactor_.scl = { size, size, size };
	charactor_.quaRot = Quaternion::Euler(
		0.0f, AsoUtility::Deg2RadF(CHARACTER_ROT_Y), 0.0f);
	charactor_.Update();

	// アニメーションの設定
	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = std::make_unique<AnimationController>(charactor_.modelId);
	animationController_->Add(0, path + "Run.mv1", RUN_ANIM_SPEED);
	animationController_->Play(0);

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

	DrawRotaGraph(Application::SCREEN_SIZE_X / 2,
		TITLE_DRAW_Y,
		TITLE_DRAW_SCALE,
		0.0,
		imgTitle_,
		true
	);
	
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)pushAlpha_);
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		PUSH_DRAW_Y,
		PUSH_DRAW_SCALE,
		0.0,
		imgPush_,
		true
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

bool TitleScene::Release(void)
{
	return false;
}
