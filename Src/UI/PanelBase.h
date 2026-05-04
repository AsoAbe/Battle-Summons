#pragma once

#include "../Application.h"
#include "../Common/DrawUtility.h"


class GameScene;
/// <summary>
/// メインスクリーンの上に描画するPanel。処理を終えたらGameScene::DeletePanelで自身を消去する
/// </summary>
class PanelBase
{
public:
	/// <summary>
	/// 表示後に操作を受け付けない時間(フレーム)
	/// </summary>
	static constexpr int PANEL_MIN_TIME = Application::FPS;

	/// <summary>
	/// シーン遷移テキストのY座標
	/// </summary>
	static constexpr int TITLESCENE_TEXT_Y = 400;

	/// <summary>
	/// ゲームオーバー画像(現在はテキストのみ)のY座標
	/// </summary>
	static constexpr int TEXT_Y = 200;

	/// <summary>
	/// スコアテキストの開始位置Y座標
	/// </summary>
	static constexpr int SCORE_TEXT_Y = 232;

	/// <summary>
	/// スコアテキストのリザルト部分の開始位置Y座標
	/// </summary>
	static constexpr int SCORE_RESULT_TEXT_Y = 332;

	/// <summary>
	/// スコアテキストの開始位置X座標
	/// </summary>
	static constexpr int SCORE_TEXT_X = 246;
	/// <summary>
	/// スコアテキストの数字部分の開始位置
	/// </summary>
	static constexpr int SCORE_NUM_TEXT_X = SCORE_TEXT_X + (DrawUtility::DEFAULT_TEXT_SIZE * 12);

	PanelBase(GameScene& Gamescene);
	virtual ~PanelBase();

	void virtual Update();
	void virtual Draw();
	/// <summary>
	/// アクティブ状態を取得
	/// </summary>
	bool IsActive()const;
	/// <summary>
	/// アクティブ状態を切り替え
	/// </summary>
	void virtual SetActive(bool b);
protected:
	GameScene& scene_;
	bool isActive_;

};
