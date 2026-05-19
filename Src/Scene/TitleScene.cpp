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
	constexpr int TITLE_MODEL_ROT_Z = 0;

}

TitleScene::TitleScene(void)
{
	imgPush_ = -1;
	imgTitle_ = -1;
	skyDome_ = nullptr;
	animationController_ = nullptr;

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

	float size;

	//// メイン惑星
	//planet_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::FALL_PLANET));
	//planet_.pos = AsoUtility::VECTOR_ZERO;
	//planet_.scl = AsoUtility::VECTOR_ONE;
	//planet_.Update();

	//// 回転する惑星
	//movePlanet_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::LAST_PLANET));
	//movePlanet_.pos = { -250.0f, -100.0f, -100.0f };
	//size = 0.7f;
	//movePlanet_.scl = { size, size, size };
	//movePlanet_.quaRotLocal = Quaternion::Euler(
	//	AsoUtility::Deg2RadF(90.0f), 0.0f, 0.0f);
	//movePlanet_.Update();

	// キャラ
	charactor_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::PLAYER));
	charactor_.pos = { -250.0f, -32.0f, -105.0f };
	size = 0.4f;
	charactor_.scl = { size, size, size };
	charactor_.quaRot = Quaternion::Euler(
		0.0f, AsoUtility::Deg2RadF(90.0f), 0.0f);
	charactor_.Update();

	// アニメーションの設定
	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = std::make_unique<AnimationController>(charactor_.modelId);
	animationController_->Add(0, path + "Run.mv1", 20.0f);
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
	pushAlpha_ = 255.0f;
	pushAlphaDir_ = -1.0f;
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

	// 惑星の回転
	movePlanet_.quaRot = movePlanet_.quaRot.Mult(
		Quaternion::Euler(0.0f, 0.0f, AsoUtility::Deg2RadF(-1.0f)));
	movePlanet_.Update();

	// キャラアニメーション
	animationController_->Update();

	skyDome_->Update();

	// --- PUSH SPACE 点滅 ---
	pushAlpha_ += pushAlphaDir_ * 2.0f;   // 数値小さいほどゆっくり

	if (pushAlpha_ <= 50.0f)
	{
		pushAlpha_ = 50.0f;
		pushAlphaDir_ = 1.0f;    // フェードインへ
	}
	else if (pushAlpha_ >= 255.0f)
	{
		pushAlpha_ = 255.0f;
		pushAlphaDir_ = -1.0f;   // フェードアウトへ
	}

}

void TitleScene::Draw(void)
{

	skyDome_->Draw();

	MV1DrawModel(planet_.modelId);
	MV1DrawModel(movePlanet_.modelId);
	MV1DrawModel(charactor_.modelId);

	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 350, 0.7, 0.0, imgTitle_, true);
	
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)pushAlpha_);
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		500,
		1.0,
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
