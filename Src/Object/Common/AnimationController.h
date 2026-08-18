#pragma once
#include <string>
#include <map>
class SceneManager;

class AnimationController
{
	
public :
	// 無効なアニメーションID
	static constexpr int INVALID_ANIMATION_ID = -1;

	// 初期値
	static constexpr float INITIAL_VALUE = 0.0f;

	// アニメーション番号の初期値
	static constexpr int DEFAULT_ANIMATION_INDEX = 0;

	// 複数アニメーション判定に使用する最小アニメーション数
	static constexpr int MULTI_ANIMATION_COUNT = 1;

	// アニメーション番号
	static constexpr int DEFAULT_ANIMATION_NUMBER = 1;

	// 逆再生を表す値
	static constexpr float REVERSE_VALUE = -1.0f;

	// 正方向再生を表す値
	static constexpr float FORWARD_VALUE = 1.0f;

	// アニメーションデータ
	struct Animation
	{
		int model = -1;
		int attachNo = -1;
		int animIndex = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
	};

	// コンストラクタ
	AnimationController(int modelId);
	// デストラクタ
	~AnimationController(void);

	// アニメーション追加
	void Add(int type, const std::string& path, float speed);

	// アニメーション再生
	void Play(int type, bool isLoop = true, 
		float startStep = 0.0f, float endStep = -1.0f, bool isStop = false, bool isForce = false);

	void Update(void);

	// アニメーション終了後に繰り返すループステップ
	void SetEndLoop(float startStep, float endStep, float speed);

	// 再生中のアニメーション
	int GetPlayType(void) const;

	// 再生終了
	bool IsEnd(void) const;

private :

	// モデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	int playType_;
	Animation playAnim_;

	// アニメーションをループするかしないか
	bool isLoop_;

	// アニメーションを止めたままにする
	bool isStop_;

	// アニメーション終了後に繰り返すループステップ
	float stepEndLoopStart_;
	float stepEndLoopEnd_;
	float endLoopSpeed_;

	// 逆再生
	float switchLoopReverse_;

};

