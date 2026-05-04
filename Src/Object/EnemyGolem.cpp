#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include "../Scene/GameScene.h"
#include "../Object/Player.h"
#include "../Manager/ResourceManager.h"
#include "Common/AnimationController.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "../Card/CannonBase.h"
#include "Planet.h"
#include "EnemyGolem.h"
#include <EffekseerForDXLib.h>

EnemyGolem::EnemyGolem(GameScene& scene)
    : scene_(scene)
{
    animationController_ = nullptr;
    // タックル関連
    isTackling_ = false;
    isTackleCharging_ = false;
    isTackleRunning_ = false;
    tackleHitDone_ = false;

    prevTime_ = 0.0f;
    tackleTimer_ = 0.0f;
    chargeTime_ = 0.7f;
    runTime_ = 1.2f;
    tackleSpeed_ = 25.0f;
    tackleHitRadius_ = 120.0f;    // 例: 半径120
    tackleStartPos_ = VGet(0, 0, 0);
    tackleTargetPos_ = VGet(0, 0, 0);

    tackleDir_ = VGet(0, 0, 0);
    tackleGoal_ = VGet(0, 0, 0);

    // エフェクト関連
    chargeEffectHandle_ = -1;
    chargeEffectPlayId_ = -1;
    isChargeEffectPlaying_ = false;

    // 他のメンバも必要に応じて初期化
    moveSpeed_ = 4.5f;
    attackRange_ = 150.0f;
    attackTimer_ = 0.0f;
    alive_ = true;
    Hp_ = MaxHp_ = 100.0f;  // 例として
    state_ = STATE::NONE;

    moveDir_ = VGet(0.0f, 0.0f, 0.0f);
    jumpPow_ = VGet(0, 0, 0);
    gravity_ = Planet::DEFAULT_GRAVITY_POW;
    isGround_ = false;

    LastPos_ = AsoUtility::VECTOR_ZERO;

    capsule_ = nullptr;
    capsuleOffsetY = 0.0f;

    // 状態遷移設定
    stateChanges_.emplace(STATE::NONE, std::bind(&EnemyGolem::ChangeStateNone, this));
    stateChanges_.emplace(STATE::IDLE, std::bind(&EnemyGolem::ChangeStateIdle, this));
    stateChanges_.emplace(STATE::MOVE, std::bind(&EnemyGolem::ChangeStateMove, this));
    stateChanges_.emplace(STATE::ATTACK, std::bind(&EnemyGolem::ChangeStateAttack, this));
    stateChanges_.emplace(STATE::TACKLE, std::bind(&EnemyGolem::ChangeStateTackle, this));
    stateChanges_.emplace(STATE::REST, std::bind(&EnemyGolem::ChangeStateRest, this));
    stateChanges_.emplace(STATE::DEAD, std::bind(&EnemyGolem::ChangeStateDead, this));
}

EnemyGolem::~EnemyGolem()
{
}

void EnemyGolem::Init()
{
    // モデル読み込み
    transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::ENEMY));
    transform_.scl = VGet(1.0f, 1.0f, 1.0f);
    transform_.pos = VGet(300.0f, 0.0f, 400.0f);
    transform_.quaRot = Quaternion();
    transform_.Update();

    chargeEffectHandle_ = ResourceManager::GetInstance().Load(
        ResourceManager::SRC::TACKLE_CHARGE).handleId_;

    //Capsuleコライダ（Playerと同じ仕組み）
    capsule_ = std::make_unique<Capsule>(transform_);
    capsuleOffsetY = 0.0f;

    // ゴーレム用の当たり範囲（地面用）
    capsule_->SetLocalPosTop({ 0.0f, 200.0f, 0.0f });
    capsule_->SetLocalPosDown({ 0.0f, 20.0f, 0.0f });
    capsule_->SetRadius(60.0f);

    //Playerの攻撃による判定用
    capsuleTopLocal_ = { 0.0f, 200.0f, 0.0f };
    capsuleBottomLocal_ = { 0.0f,  20.0f, 0.0f };
    capsuleRadius_ = 60.0f;

    // モデルのオフセット
    transform_.modelOffset = { 0.0f, -40.0f, 0.0f };

    MaxHp_ = MAX_HP;
    Hp_ = MAX_HP;
    EnemyStart_ = false;

    // アニメーション初期化
    InitAnimation();

    //タックルのエフェクト用
    chargeEffectPlayId_ = -1;
    isChargeEffectPlaying_ = false;

    isTackling_ = false;
    isTackleCharging_ = false;
    isTackleRunning_ = false;

    tackleTimer_ = 0.0f;
    chargeTime_ = 0.7f; // ← 重要: 120 にしてたのが原因なので秒値に戻す
    runTime_ = 1.2f;    // 必要なら再設定

    // 初期アニメ
    animationController_->Play((int)ANIM_TYPE::IDLE);

    ChangeState(STATE::IDLE);

}

void EnemyGolem::Update()
{
    prevPos_ = transform_.pos;
    float delta = SceneManager::GetInstance().GetDeltaTime();

    // 重力計算
    CalcGravityPow();

    // 位置更新・当たり判定
    Collision();

    switch (state_)
    {
    case STATE::IDLE:   UpdateIdle(); break;
    case STATE::MOVE:   UpdateMove(); break;
    case STATE::ATTACK: UpdateAttack(); break;
    case STATE::TACKLE: UpdateTackle(); break;
    case STATE::REST:   UpdateRest(); break;
    case STATE::DEAD:   UpdateDead(); break;
    }

    //エフェクト更新
    UpdateEffekseer3D();

    // ★ルートモーションの打ち消し★
    // （モデルの描画座標をtransform_.posで上書きする）
    transform_.pos = movedPos_;
    transform_.Update();

    transform_.modelOffset.y = -20.0f;

    MV1SetPosition(transform_.modelId, transform_.pos);

    if (isTackleCharging_)
    {
        EffectTackle();
    }

    // アニメーション更新
    animationController_->Update();
    /*  auto& ins = InputManager::GetInstance();
      if (ins.IsNew(KEY_INPUT_C))
      {
          Damage(20);
      }*/
      //CheckCannonExplosion();
}

void EnemyGolem::Draw()
{
#ifdef _DEBUG
    //capsule_->Draw();
#endif
    MV1SetPosition(transform_.modelId, transform_.pos);
    MV1DrawModel(transform_.modelId);


#ifdef _DEBUG
    //DrawSphere3D(
    //    attackPos,              // 球の中心位置
    //    100.0f,                 // 半径
    //    16,                     // 分割数
    //    GetColor(255, 0, 0),    // 拡散色
    //    GetColor(255, 0, 0),    // 鏡面色（同じでOK）
    //    false                   // 塗りつぶさない（ワイヤーフレーム）
    //);
    //DrawSphere3D(
    //    transform_.pos,              // 球の中心位置
    //    tackleHitRadius_,                 // 半径
    //    16,                     // 分割数
    //    GetColor(255, 0, 0),    // 拡散色
    //    GetColor(255, 0, 0),    // 鏡面色（同じでOK）
    //    false                   // 塗りつぶさない（ワイヤーフレーム）
    //);
#endif
    //DrawString(0, 110, "Golem Update OK", GetColor(255, 0, 0));
    //DrawString(0, 140, "Golem Draw OK", GetColor(255, 255, 0));
    auto pos = transform_.pos;
   // DrawFormatString(0, 170, GetColor(0, 255, 0), "Golem Pos: %.0f, %.0f, %.0f", GetPos().x, GetPos().y, GetPos().z);
}

bool EnemyGolem::Release()
{
    return true;
}

void EnemyGolem::InitAnimation()
{
    std::string path = Application::PATH_MODEL + "Golem/";
    animationController_ = std::make_unique<AnimationController>(transform_.modelId);
    animationController_->Add((int)ANIM_TYPE::IDLE, path + "ANIM_Neutral.mv1", 60.0f);
    animationController_->Add((int)ANIM_TYPE::RUN, path + "Golem Run.mv1", 40.0f);
    animationController_->Add((int)ANIM_TYPE::ATTACK, path + "Anim_RightAttack.mv1", 60.0f);
    animationController_->Add((int)ANIM_TYPE::TACKLE, path + "Dash.mv1", 60.0f);
    animationController_->Add((int)ANIM_TYPE::REST, path + "rest.mv1", 20.0f);
    animationController_->Add((int)ANIM_TYPE::DEAD, path + "Down.mv1", 60.0f);
}

void EnemyGolem::UpdateNone()
{
}

void EnemyGolem::UpdateIdle()
{
    LookAtPlayer(0.1f);

    // Runで上げた分を戻す
    //transform_.modelOffset.y = -30.0f;

    //重力はゼロにする
    jumpPow_ = VGet(0, 0, 0);

    auto player = GetPlayer();
    if (!player) return;

    float now = SceneManager::GetInstance().GetTotalTime();

    //② スタート直後の遷移（1回だけ）
    VECTOR diff = VSub(player->GetPos(), transform_.pos);
    float dist = VSize(diff);

    if (!EnemyStart_ && dist < 400.0f)
    {
        ChangeState(STATE::MOVE);
        animationController_->Play((int)ANIM_TYPE::RUN, true);

        EnemyStart_ = true;   // スタート時の移行は1回のみ
        return;
    }

    //③ 通常Idle → 基本はMOVE に戻す
    // ここに来るのは「タックル後の隙が終わった直後」だけ
   /* ChangeState(STATE::MOVE);
    animationController_->Play((int)ANIM_TYPE::RUN, true);*/
}

void EnemyGolem::UpdateMove()
{
    if (isTackleCharging_) return;

    auto player = GetPlayer();
    if (!player) return;

    //距離が離れたらSTATE::TACKLEに
    VECTOR toPlayer = VSub(player->GetPos(), transform_.pos);
    float dist = VSize(toPlayer);
    //float dist = VSize(VSub(player->GetPos(), transform_.pos));

    constexpr float TACKLE_START_DISTANCE = 800.0f;

    if (state_ == STATE::TACKLE) return;

    if (dist > TACKLE_START_DISTANCE)
    {
        ChargeTackle();
        return;
    }

    //transform_.modelOffset.y = 10.0f;

    // プレイヤーへの方向ベクトル
   
    toPlayer.y = 0;
    if (dist < 0.001f) return;

    // プレイヤー方向を向く
    float targetYaw = atan2(toPlayer.x, toPlayer.z) + DX_PI_F;
    Quaternion targetRot = Quaternion::Euler(0.0f, targetYaw, 0.0f);
    transform_.quaRot = Quaternion::Slerp(transform_.quaRot, targetRot, 0.1f);

    // 移動処理（プレイヤーに近づく）
    if (dist > 200.0f)
    {
        VECTOR dir = VNorm(toPlayer);

        // ★ ActorBaseの「移動候補」へセット（直接posに加算しない）
        movePow_ = VScale(dir, moveSpeed_);

    }
    else
    {
        movePow_ = VGet(0, 0, 0);
        ChangeState(STATE::ATTACK);
    }

    transform_.Update();
}

void EnemyGolem::UpdateAttack()
{
    auto player = GetPlayer();
    if (!player) return;

    //Runで上げた分を戻す
    //transform_.modelOffset.y = -10.0f;

    // 初回のみアニメーション再生
    if (animationController_->GetPlayType() != (int)ANIM_TYPE::ATTACK)
    {
        animationController_->Play((int)ANIM_TYPE::ATTACK, false);

        // 攻撃判定の発生タイミングを設定（例：0.4秒後に当たり、0.2秒間有効）
        float now = SceneManager::GetInstance().GetTotalTime();
        attackHitStartTime_ = now + 0.4f;
        attackHitEndTime_ = now + 0.6f;
        isAttackHitActive_ = false;
    }

    float now = SceneManager::GetInstance().GetTotalTime();

    // 攻撃判定ONになるタイミング
    if (!isAttackHitActive_ && now >= attackHitStartTime_ && now <= attackHitEndTime_)
    {
        isAttackHitActive_ = true;
        ActivateAttackHit();  // 判定ON
    }

    // 攻撃判定終了
    if (isAttackHitActive_ && now > attackHitEndTime_)
    {
        isAttackHitActive_ = false;
        DeactivateAttackHit(); // 判定OFF
    }

    // 攻撃アニメーションが終了したら次の行動へ
    if (animationController_->IsEnd())
    {
        ChangeState(STATE::MOVE);
        animationController_->Play((int)ANIM_TYPE::RUN, true);
    }
}

void EnemyGolem::ChargeTackle()
{
    auto player = GetPlayer();
    if (!player) return;

    SoundManager::GetInstance().PlaySE(
        SoundManager::SOUND_ID::CHARGE,
        true,
        SoundManager::VOLUME_STANDARD
    );

    float dist = VSize(VSub(player->GetPos(), transform_.pos));
    constexpr float TACKLE_START_DISTANCE = 800.0f;

    if (state_ == STATE::TACKLE) return;
    if (dist <= TACKLE_START_DISTANCE) return;

    // プレイヤー方向を見る（XZ平面のみ）
    VECTOR toPlayer = VSub(player->GetPos(), transform_.pos);
    toPlayer.y = 0.0f;
    if (VSize(toPlayer) > 0.0001f)
    {
        float yaw = atan2(toPlayer.x, toPlayer.z) + DX_PI_F;
        transform_.quaRot = Quaternion::Euler(0.0f, yaw, 0.0f);
    }

    // ★ Golem の現在の forward（実は後ろ向き）
    VECTOR forward = transform_.quaRot.GetForward();

    // ★ 前方向は -forward
    VECTOR realForward = VNorm(VScale(forward, -1.0f));

    // ★ 進行方向を固定
    tackleDir_ = realForward;

    // ★ 進行方向の前方500をゴールとする
    //constexpr float GOAL_DISTANCE = 500.0f;
    //tackleGoal_ = VAdd(transform_.pos, VScale(realForward, GOAL_DISTANCE));

    // チャージ用タイマー初期化（重要）
    tackleTimer_ = 0.0f;

    tackleSpeed_ = 25.0f;
    isTackling_ = true;
    isTackleCharging_ = true;      // チャージ開始
    isTackleRunning_ = false;

    movePow_ = VGet(0, 0, 0);

    //タックルモーションでチャージを始める
    animationController_->Play((int)ANIM_TYPE::TACKLE, true);

    EffectTackle();

    //ChangeState(STATE::TACKLE);
    // エフェクト開始
} 

void EnemyGolem::UpdateTackle()
{
    auto player = GetPlayer();
    if (!player) return;

    // ★初回開始処理
    if (!isTackling_)
    {
        isTackling_ = true;

        // forward（前方向）を使う ※モデルが逆なら -forward
        VECTOR forward = transform_.quaRot.GetForward();
        tackleDir_ = VNorm(VScale(forward, -1.0f));

        tackleSpeed_ = 25.0f;

    }

    // ★移動
    movePow_ = VScale(tackleDir_, tackleSpeed_);

    // ★プレイヤーの方向（XZ のみ）を取得
    VECTOR toPlayer = VSub(player->GetPos(), transform_.pos);
    toPlayer.y = 0;

    VECTOR dirXZ = tackleDir_;
    dirXZ.y = 0;
  
    if (!tackleHitDone_)
    {
        auto player = GetPlayer();
        if (player)
        {
            float distToPlayer = VSize(VSub(player->GetPos(), transform_.pos));
            if (distToPlayer < tackleHitRadius_)
            {
                player->Damage(20);
                tackleHitDone_ = true;
            }
        }
    }
    // ★内積で通過チェック
    float dot = VDot(toPlayer, dirXZ);

    if (dot < -200.0f) //プレイヤーを通り過ぎたらタックル終了
    {
        // 減速
        tackleSpeed_ -= 1.0f;

        if (tackleSpeed_ <= 0.0f)
        {
            tackleSpeed_ = 0.0f;

            isTackling_ = false;
            isTackleRunning_ = false;
            /*ChangeState(STATE::MOVE);
            animationController_->Play((int)ANIM_TYPE::RUN, true);*/
            DoRest(2.0f);
            return;
        }
    }
}

void EnemyGolem::EffectTackle()
{ 
    if (!isTackleCharging_) return;   // ★重要：チャージ中でなければ動かない

    //  チャージ中処理
   // エフェクトがまだなら再生する
    if (!isChargeEffectPlaying_)
    {
        chargeEffectPlayId_ = PlayEffekseer3DEffect(chargeEffectHandle_);

        SetPosPlayingEffekseer3DEffect(
            chargeEffectPlayId_,
            transform_.pos.x,
            transform_.pos.y + 50,
            transform_.pos.z
        );

        float scale = 30.0f;

        SetScalePlayingEffekseer3DEffect(chargeEffectPlayId_, scale, scale, scale);

        isChargeEffectPlaying_ = true;
    }

    // チャージ時間を進める
    tackleTimer_ += 1.0f / DEFAULT_FPS;

    // チャージ完了 → タックルへ遷移
    if (tackleTimer_ >= chargeTime_)
    {
        // エフェクト停止
        if (chargeEffectPlayId_ >= 0)
        {
            StopEffekseer3DEffect(chargeEffectPlayId_);
            chargeEffectPlayId_ = -1;
        }
        isChargeEffectPlaying_ = false;

        // 突進フラグ
        isTackleCharging_ = false;
        isTackleRunning_ = true;

        // 状態遷移
        ChangeState(STATE::TACKLE);
    }
}

void EnemyGolem::UpdateRest()
{
    float now = SceneManager::GetInstance().GetTotalTime();
   
    if (now >= EndTime_) 
    {
        ChangeState(STATE::MOVE);
        animationController_->Play((int)ANIM_TYPE::RUN, true);
    }
}

void EnemyGolem::DoRest(float value)
{
    // 終了時間設定
    EndTime_ = SceneManager::GetInstance().GetTotalTime() + value;

    //動きを止める
    movePow_ = VGet(0, 0, 0);

    // 状態遷移
    ChangeState(STATE::REST);
    animationController_->Play((int)ANIM_TYPE::REST, true);
}

void EnemyGolem::UpdateDead()
{
    animationController_->Play((int)ANIM_TYPE::DEAD, false);
}

bool EnemyGolem::Damage(int value)
{
    if (!alive_) return false;
    Hp_ -= value;

    if (Hp_ <= 0.0f)
    {
        Hp_ = 0.0f;
        alive_ = false;
 
        scene_.GameOver();
    }
    return true;
}

void EnemyGolem::LookAtPlayer(float rotSpeed)
{
    auto player = GetPlayer();
    if (!player) return;

    VECTOR targetPos = player->GetPos();
    VECTOR dir = VSub(targetPos, transform_.pos);

    // 水平面のみ考慮
    dir.y = 0.0f;
    if (VSize(dir) < 0.001f) return;

    // atan2で回転角を求める
    //float rotRad = atan2f(dir.x, dir.z);

    // Golem モデルの前が -Z なので 180度回転を加える
    float rotRad = atan2f(dir.x, dir.z) + DX_PI_F;
    transform_.quaRot = Quaternion::Euler(0.0f, rotRad, 0.0f);
}

void EnemyGolem::MoveTowardPlayer(float deltaTime)
{
    auto player = GetPlayer();
    if (!player) return;

    VECTOR toPlayer = VSub(player->GetPos(), transform_.pos);
    toPlayer.y = 0.0f;
    if (VSize(toPlayer) < 0.001f) return;

    // 目標回転設定
    double rotRad = atan2(toPlayer.x, toPlayer.z);
    SetGoalRotate(rotRad);

    // スムーズ回転
    Rotate(deltaTime);

    // 移動
    moveDir_ = VNorm(toPlayer);
    transform_.pos = VAdd(transform_.pos, VScale(moveDir_, moveSpeed_ * deltaTime));
}

void EnemyGolem::ActivateAttackHit()
{
    auto player = GetPlayer();
    if (!player) return;

    // 前方方向を取得
    VECTOR forward = transform_.quaRot.GetForward();

    // ★攻撃判定を少し上へ持ち上げる
    VECTOR offset = VGet(0.0f, 70.0f, 0.0f);

    // ゴーレムの手前方に攻撃範囲を作る（例: 半径100）
    attackPos = VAdd(
        VAdd(transform_.pos, VScale(forward, -120.0f)),
        offset
    );

    // プレイヤーとの距離でヒットチェック
    VECTOR diff = VSub(player->GetPos(), attackPos);
    float dist = VSize(diff);

    if (dist < 140.0f)
    {
        player->Damage(20);
    }

}

void EnemyGolem::DeactivateAttackHit()
{
    // 今回は何も必要なし（1フレームのみのダメージ処理）
}

void EnemyGolem::Collision(void)
{
    // 移動後の仮座標を決める
    movedPos_ = VAdd(transform_.pos, movePow_);

    //重力衝突
    CollisionGravity();

    //カプセル衝突
    CollisionCapsule();

    if (VSize(VSub(movedPos_, prevPos_)) > 200.0f)
    {
        // 何かおかしい場合（ワープ・突き抜け）の緊急リセット
        movedPos_ = prevPos_;
    }

    // 移動が確定した座標を保存
    LastPos_ = movedPos_;

    // ---------------------------------------------
    //  落下判定（高さが -200 以下 = ステージ外）
    // ---------------------------------------------
    if (movedPos_.y < -1000.0f)
    {
        // ステージ中心（例として (0,0,0)）方向へ少し寄せる
        VECTOR stageCenter = VGet(0.0f, 0.0f, 0.0f);

        // LastPos_ → 中心への方向
        VECTOR dir = VSub(stageCenter, LastPos_);
        dir = VNorm(dir);

        // ★ 戻す位置＝直前位置 ＋ 中心方向へ 50 だけ寄せる
        movedPos_ = VAdd(LastPos_, VScale(dir, 100.0f));

        // ★ 少しだけ浮かせて衝突が安定するように
        movedPos_.y = 5.0f;

        // 重力リセット
        jumpPow_ = VGet(0, 0, 0);
    }

    //最終座標を反映
    transform_.pos = movedPos_;
}

void EnemyGolem::CollisionGravity(void)
{
    // 重力の追加
    movedPos_ = VAdd(movedPos_, jumpPow_);

    VECTOR dirGravity = AsoUtility::DIR_D;
    VECTOR dirUp = AsoUtility::DIR_U;

    float checkLen = 20.0f;

    // 衝突判定Ray
    VECTOR rayStart = VAdd(movedPos_, VScale(dirUp, checkLen));
    VECTOR rayEnd = VAdd(movedPos_, VScale(dirGravity, checkLen));

    for (auto c : colliders_)
    {
        if (c.expired()) continue;

        auto hit = MV1CollCheck_Line(
            c.lock()->modelId_, -1, rayStart, rayEnd);

        if (hit.HitFlag > 0)
        {
            // 地形の角度は考慮せず、垂直に補正する
            movedPos_.y = hit.HitPosition.y + 2.0f;

            // 重力リセット（坂の下へ滑る力を完全にゼロ
            jumpPow_ = VGet(0, 0, 0);

            isGround_ = true;
            return;
        }
    }

    // 接地していない
    isGround_ = false;
}

void EnemyGolem::CollisionCapsule(void)
{
    Transform trans = transform_;
    trans.pos = movedPos_;
    trans.Update();

    Capsule cap = Capsule(*capsule_, trans);

    for (const auto& c : colliders_)
    {
        if (c.expired()) continue;

        auto hits = MV1CollCheck_Capsule(
            c.lock()->modelId_, -1,
            cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());

        for (int i = 0; i < hits.HitNum; i++)
        {
            auto hit = hits.Dim[i];

            for (int t = 0; t < 10; t++)
            {
                int pHit = HitCheck_Capsule_Triangle(
                    cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
                    hit.Position[0], hit.Position[1], hit.Position[2]);

                if (pHit)
                {
                    VECTOR n = hit.Normal;

                    // 法線の水平成分（XZ）は無効化：垂直方向だけにする
                    VECTOR verticalOnly = VGet(0.0f, n.y, 0.0f);

                    // 垂直分だけで押し出し（XZ方向に影響しない）
                    movedPos_ = VAdd(movedPos_, VScale(verticalOnly, 1.0f));

                    trans.pos = movedPos_;
                    trans.Update();
                    continue;
                }
                break;
            }
        }

        MV1CollResultPolyDimTerminate(hits);
    }
}

void EnemyGolem::CalcGravityPow(void)
{

    // プレイヤーと同じ重力方向
    VECTOR dirGravity = AsoUtility::DIR_D;

    // 重力ベクトルを計算
    VECTOR gravityVec = VScale(dirGravity, gravity_ * 0.038f); // ※ 60FPS タイマー調整

    // 重力加算
    jumpPow_ = VAdd(jumpPow_, gravityVec);

    // 最初は実装しない。地面と突き抜けることを確認する。
    // 内積
    //float dot = VDot(dirGravity, jumpPow_);
    //if (dot >= 0.0f)
    //{
    //	// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
    //	jumpPow_ = gravity;
    //}

}

void EnemyGolem::SetGoalRotate(double rotRad)
{
    goalQuaRot_ = Quaternion::Euler(0.0f, (float)rotRad, 0.0f);
}

void EnemyGolem::Rotate(float deltaTime)
{
    transform_.quaRot = Quaternion::Slerp(transform_.quaRot
        , goalQuaRot_
        , deltaTime * 5.0f);

    transform_.quaRot.Normalize();
}

void EnemyGolem::ChangeState(STATE state)
{
    state_ = state;
    stateChanges_[state_]();
}

void EnemyGolem::ChangeStateNone()
{
    stateUpdate_ = std::bind(&EnemyGolem::UpdateNone, this);
}

void EnemyGolem::ChangeStateIdle()
{
    stateUpdate_ = std::bind(&EnemyGolem::UpdateIdle, this);
}

void EnemyGolem::ChangeStateMove()
{
    stateUpdate_ = std::bind(&EnemyGolem::UpdateMove, this);
}

void EnemyGolem::ChangeStateAttack()
{
    stateUpdate_ = std::bind(&EnemyGolem::UpdateAttack, this);
}

void EnemyGolem::ChangeStateTackle()
{
    isTackling_ = true;
    stateUpdate_ = std::bind(&EnemyGolem::UpdateTackle, this);
}

void EnemyGolem::ChangeStateRest()
{
    stateUpdate_ = std::bind(&EnemyGolem::UpdateRest, this);
}

void EnemyGolem::ChangeStateDead()
{
    stateUpdate_ = std::bind(&EnemyGolem::UpdateDead, this);
}

void EnemyGolem::AddCollider(std::weak_ptr<Collider> collider)
{
    colliders_.push_back(collider);
}

void EnemyGolem::ClearCollider(void)
{
    colliders_.clear();
}

VECTOR EnemyGolem::GetPos() const
{
    return transform_.pos;
}

VECTOR EnemyGolem::GetCapsuleTop() const
{
    return VAdd(movedPos_, capsuleTopLocal_);
}

VECTOR EnemyGolem::GetCapsuleBottom() const
{
    return VAdd(movedPos_, capsuleBottomLocal_);
}

float EnemyGolem::GetRadius() const
{
    return capsule_ ? capsule_->GetRadius() : 0.0f;
}

void EnemyGolem::SetCannon(std::shared_ptr<CannonBase> cannon)
{
    cannon_ = cannon;
}

void EnemyGolem::SetPlayer(std::shared_ptr<Player> player)
{
    player_ = player;
}

std::weak_ptr<CannonBase> EnemyGolem::GetCannonWeakPtr() const
{
    return cannon_;
}

const Capsule& EnemyGolem::GetCapsule(void) const
{
    return *capsule_;
}

std::shared_ptr<Player> EnemyGolem::GetPlayer() const
{
    return scene_.GetPlayer();
}

float EnemyGolem::GetMaxHp()
{
    return MaxHp_;
}

float EnemyGolem::GetHp()
{
    return Hp_;
}