#pragma once
class ResourceManager;

class SceneBase
{

public:
	// 初期値
	static constexpr float INITIAL_VALUE = 0.0f;

	// 無効なリソースハンドル
	static constexpr int INVALID_HANDLE = -1;
	static constexpr int INITIAL_COUNT = 0;

	// コンストラクタ
	SceneBase(void);

	// デストラクタ
	virtual ~SceneBase(void) = 0;

	// 初期化処理
	virtual void Init(void) = 0;

	// 更新ステップ
	virtual void Update(void) = 0;

	// 描画処理
	virtual void Draw(void) = 0;
	//解放
	virtual bool  Release(void) = 0;

protected:

	// リソース管理
	ResourceManager& resMng_;

};