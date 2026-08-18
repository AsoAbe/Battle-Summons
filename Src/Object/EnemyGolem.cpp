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

    attackRange_ = VECTOR_ZERO_VALUE;
    attackPos = AsoUtility::VECTOR_ZERO;

    EndTime_ = VECTOR_ZERO_VALUE;

    footOffsetY = VECTOR_ZERO_VALUE;

    playerRotY_ = Quaternion();
    goalQuaRot_ = Quaternion();
    stepRotTime_ = VECTOR_ZERO_VALUE;

    attackHitStartTime_ = VECTOR_ZERO_VALUE;
    attackHitEndTime_ = VECTOR_ZERO_VALUE;
    chargeStartTime_ = VECTOR_ZERO_VALUE;

    debugAttackPos_ = AsoUtility::VECTOR_ZERO;

    // タックル関連
    isTackling_ = false;
    isTackleCharging_ = false;
    isTackleRunning_ = false;
    tackleHitDone_ = false;

    prevTime_ = VECTOR_ZERO_VALUE;
    tackleTimer_ = VECTOR_ZERO_VALUE;
    chargeTime_ = DEFAULT_CHARGE_TIME;
    runTime_ = DEFAULT_RUN_TIME;
    tackleSpeed_ = DEFAULT_TACKLE_SPEED;
    tackleHitRadius_ = DEFAULT_TACKLE_RADIUS; 
    tackleStartPos_ = AsoUtility::VECTOR_ZERO;
    tackleTargetPos_ = AsoUtility::VECTOR_ZERO;

    tackleDir_ = AsoUtility::VECTOR_ZERO;
    tackleGoal_ = AsoUtility::VECTOR_ZERO;

    // エフェクト関連
    chargeEffectHandle_ = INVALID_HANDLE;
    chargeEffectPlayId_ = INVALID_HANDLE;
    isChargeEffectPlaying_ = false;

    // 他のメンバも必要に応じて初期化
    moveSpeed_ = DEFAULT_MOVE_SPEED;
    attackTimer_ = VECTOR_ZERO_VALUE;
    alive_ = true;
    MaxHp_ = VECTOR_ZERO_VALUE;
    Hp_ = VECTOR_ZERO_VALUE;
    state_ = STATE::NONE;

    //死亡関連
    deadTimer_ = VECTOR_ZERO_VALUE;
    deadDelay_ = VECTOR_ZERO_VALUE;
    gameOverReserved_ = false;

    damaged_ = false;
    preDamaged_ = false;
    invincibleTimer_ = VECTOR_ZERO_VALUE;

    attackHitDone_ = false;

    EnemyStart_ = false;

    prevPos_ = AsoUtility::VECTOR_ZERO;

    capsuleTopLocal_ = AsoUtility::VECTOR_ZERO;
    capsuleBottomLocal_ = AsoUtility::VECTOR_ZERO;
    capsuleRadius_ = VECTOR_ZERO_VALUE;

    moveDir_ = AsoUtility::VECTOR_ZERO;
	movePow_ = AsoUtility::VECTOR_ZERO;
	movedPos_ = AsoUtility::VECTOR_ZERO;
	LastPos_ = AsoUtility::VECTOR_ZERO;
    jumpPow_ = AsoUtility::VECTOR_ZERO;
    isJump_ = false;

    LastPos_ = AsoUtility::VECTOR_ZERO;

    capsule_ = nullptr;
    capsuleOffsetY = VECTOR_ZERO_VALUE;

    imgShadow_ = INVALID_HANDLE;

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
    transform_.scl = VGet(
        INITIAL_SCALE,
        INITIAL_SCALE,
        INITIAL_SCALE
    );
    transform_.pos = VGet(
        INITIAL_POS_X,
        INITIAL_POS_Y,
        INITIAL_POS_Z
    );
    transform_.quaRot = Quaternion();
    transform_.Update();

    chargeEffectHandle_ = ResourceManager::GetInstance().Load(
        ResourceManager::SRC::TACKLE_CHARGE).handleId_;

    // 丸影画像
    imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;

    //Capsuleコライダ（Playerと同じ仕組み）
    capsule_ = std::make_unique<Capsule>(transform_);
    capsuleOffsetY = VECTOR_ZERO_VALUE;
    //capsuleOffsetY = 90.0f;

    // ゴーレム用の当たり範囲（地面用）
    capsule_->SetLocalPosTop({ VECTOR_ZERO_VALUE, LAND_CAPSULE_TOP_Y, VECTOR_ZERO_VALUE });
    capsule_->SetLocalPosDown({ VECTOR_ZERO_VALUE, LAND_CAPSULE_BOTTOM_Y, VECTOR_ZERO_VALUE });
    capsule_->SetRadius(CAPSULE_RADIUS);

    //Playerの攻撃による判定用
    capsuleTopLocal_ = { VECTOR_ZERO_VALUE, CAPSULE_TOP_Y, VECTOR_ZERO_VALUE };
    capsuleBottomLocal_ = { VECTOR_ZERO_VALUE,  CAPSULE_BOTTOM_Y, VECTOR_ZERO_VALUE };
    capsuleRadius_ = CAPSULE_RADIUS;

    // モデルのオフセット
    transform_.modelOffset = AsoUtility::VECTOR_ZERO;

    MaxHp_ = MAX_HP;
    Hp_ = MAX_HP;
    deadDelay_ = DEAD_DELAY_TIME;

    EnemyStart_ = false;

    // アニメーション初期化
    InitAnimation();

    //タックルのエフェクト用
    chargeEffectPlayId_ = INVALID_HANDLE;
    isChargeEffectPlaying_ = false;

    isTackling_ = false;
    isTackleCharging_ = false;
    isTackleRunning_ = false;

    // 初期アニメ
    animationController_->Play((int)ANIM_TYPE::IDLE);

    ChangeState(STATE::IDLE);

}

void EnemyGolem::Update()
{
    prevPos_ = transform_.pos;
    float delta = SceneManager::GetInstance().GetDeltaTime();

    switch (state_)
    {
    case STATE::IDLE:   UpdateIdle(); break;
    case STATE::MOVE:   UpdateMove(); break;
    case STATE::ATTACK: UpdateAttack(); break;
    case STATE::TACKLE: UpdateTackle(); break;
    case STATE::REST:   UpdateRest(); break;
    case STATE::DEAD:   UpdateDead(); break;
    }

    // 位置更新・当たり判定&重力
    Collision();

    //エフェクト更新
    UpdateEffekseer3D();

    // ルートモーションの打ち消し
    transform_.Update();

    MV1SetPosition(transform_.modelId, transform_.pos);

    if (isTackleCharging_)
    {
        EffectTackle();
    }

    // アニメーション更新
    animationController_->Update();

}

void EnemyGolem::Draw()
{
#ifdef _DEBUG
    //capsule_->Draw();
#endif
    MV1SetPosition(transform_.modelId, transform_.pos);
    MV1DrawModel(transform_.modelId);

    // 丸影描画
    DrawShadow();

    auto pos = transform_.pos;
}

bool EnemyGolem::Release()
{
    return true;
}

void EnemyGolem::InitAnimation()
{
    std::string path = Application::PATH_MODEL + "Golem/";
    animationController_ = std::make_unique<AnimationController>(transform_.modelId);
    
    animationController_->Add(
        (int)ANIM_TYPE::IDLE,
        path + "ANIM_Neutral.mv1",
        ANIM_FRAME_IDLE
    );

    animationController_->Add(
        (int)ANIM_TYPE::RUN,
        path + "Golem Run.mv1",
        ANIM_FRAME_RUN
    );

    animationController_->Add(
        (int)ANIM_TYPE::ATTACK,
        path + "Anim_RightAttack.mv1",
        ANIM_FRAME_ATTACK
    );

    animationController_->Add(
        (int)ANIM_TYPE::TACKLE,
        path + "Dash.mv1",
        ANIM_FRAME_TACKLE
    );

    animationController_->Add(
        (int)ANIM_TYPE::REST,
        path + "rest.mv1",
        ANIM_FRAME_REST
    );

    animationController_->Add(
        (int)ANIM_TYPE::DEAD,
        path + "Down.mv1",
        ANIM_FRAME_DEAD
    );
}

void EnemyGolem::UpdateNone()
{
}

void EnemyGolem::UpdateIdle()
{
    LookAtPlayer(ROTATE_LERP_SPEED);


    auto player = GetPlayer();
    if (!player) return;

    float now = SceneManager::GetInstance().GetTotalTime();

    //② スタート直後の遷移（1回だけ）
    VECTOR diff = VSub(player->GetPos(), transform_.pos);
    float dist = VSize(diff);

    if (!EnemyStart_ && dist < DISCOVER_DISTANCE)
    {
        ChangeState(STATE::MOVE);
        animationController_->Play((int)ANIM_TYPE::RUN, true);

        EnemyStart_ = true;   // スタート時の移行は1回のみ
        return;
    }

}

void EnemyGolem::UpdateMove()
{
    if (isTackleCharging_) return;

    auto player = GetPlayer();
    if (!player) return;

    //距離が離れたらSTATE::TACKLEに
    VECTOR toPlayer = VSub(player->GetPos(), transform_.pos);
    float dist = VSize(toPlayer);

    if (state_ == STATE::TACKLE) return;

    if (dist > TACKLE_START_DISTANCE)
    {
        ChargeTackle();
        return;
    }

    // プレイヤーへの方向ベクトル
   
    toPlayer.y = VECTOR_ZERO_VALUE;
    if (dist < MIN_MOVE_DISTANCE) return;

    // プレイヤー方向を向く
    float targetYaw = atan2(toPlayer.x, toPlayer.z) + DX_PI_F;
    Quaternion targetRot = Quaternion::Euler(VECTOR_ZERO_VALUE, targetYaw, VECTOR_ZERO_VALUE);
    transform_.quaRot = Quaternion::Slerp(transform_.quaRot, targetRot, ROTATE_LERP_SPEED);

    // 移動処理（プレイヤーに近づく）
    if (dist > MOVE_DISTANCE)
    {
        VECTOR dir = VNorm(toPlayer);

        // ActorBaseの「移動候補」へセット（直接posに加算しない）
        movePow_ = VScale(dir, moveSpeed_);

    }
    else
    {
        movePow_ = AsoUtility::VECTOR_ZERO;
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
        attackHitStartTime_ = now + ATTACK_HIT_START;
        attackHitEndTime_ = now + ATTACK_HIT_END;
        attackHitDone_ = false;
    }

    float now = SceneManager::GetInstance().GetTotalTime();

    // 攻撃判定ONになるタイミング
    if (!attackHitDone_ && now >= attackHitStartTime_ && now <= attackHitEndTime_)
    {
        if (CheckAttackHit(ATTACK_FORWARD_OFFSET  //位置
            , ATTACK_HEIGHT_OFFSET                //高さ
            , ATTACK_RADIUS))                     //範囲
        {
            player->Damage(ATTACK_DAMAGE);
            attackHitDone_ = true;
        }
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

    if (state_ == STATE::TACKLE) return;
    if (dist <= TACKLE_START_DISTANCE) return;

    // プレイヤー方向を見る（XZ平面のみ）
    VECTOR toPlayer = VSub(player->GetPos(), transform_.pos);
    toPlayer.y = VECTOR_ZERO_VALUE;
    if (VSize(toPlayer) > NORMALIZE_MIN_LENGTH)
    {
        float yaw = atan2(toPlayer.x, toPlayer.z) + DX_PI_F;
        transform_.quaRot = Quaternion::Euler(VECTOR_ZERO_VALUE, yaw, VECTOR_ZERO_VALUE);
    }

    // Golem の現在の forward（実は後ろ向き）
    VECTOR forward = transform_.quaRot.GetForward();

    // 前方向は -forward
    VECTOR realForward = VNorm(VScale(forward, MODEL_FORWARD_REVERSE));

    // 進行方向を固定
    tackleDir_ = realForward;

    // チャージ用タイマー初期化（重要）
    tackleTimer_ = VECTOR_ZERO_VALUE;

    tackleSpeed_ = TACKLE_START_SPEED;
    isTackling_ = true;
    isTackleCharging_ = true;      // チャージ開始
    isTackleRunning_ = false;

    movePow_ = AsoUtility::VECTOR_ZERO;

    //タックルモーションでチャージを始める
    animationController_->Play((int)ANIM_TYPE::TACKLE, true);

    EffectTackle();

} 

void EnemyGolem::UpdateTackle()
{
    auto player = GetPlayer();
    if (!player) return;

    // 初回開始処理
    if (!isTackling_)
    {
        isTackling_ = true;

        // forward（前方向）を使うモデルが逆なら -forward
        VECTOR forward = transform_.quaRot.GetForward();
        tackleDir_ = VNorm(VScale(forward, MODEL_FORWARD_REVERSE));

        tackleSpeed_ = TACKLE_START_SPEED;

    }

    // 移動
    movePow_ = VScale(tackleDir_, tackleSpeed_);

    // プレイヤーの方向（XZ のみ）を取得
    VECTOR toPlayer = VSub(player->GetPos(), transform_.pos);
    toPlayer.y = VECTOR_ZERO_VALUE;

    VECTOR dirXZ = tackleDir_;
    dirXZ.y = VECTOR_ZERO_VALUE;
  

    if (isTackleRunning_ && !tackleHitDone_)
    {
        if (CheckAttackHit(
            VECTOR_ZERO_VALUE,
            TACKLE_HEIGHT_OFFSET,
            tackleHitRadius_))
        {
            player->Damage(TACKLE_DAMAGE);
            tackleHitDone_ = true;
        }
    }

    // ★内積で通過チェック
    float dot = VDot(toPlayer, dirXZ);

    if (dot < TACKLE_PASS_DOT) //プレイヤーを通り過ぎたらタックル終了
    {
        // 減速
        tackleSpeed_ -= TACKLE_DECELERATION;

        if (tackleSpeed_ <= VECTOR_ZERO_VALUE)
        {
            tackleSpeed_ = VECTOR_ZERO_VALUE;

            isTackling_ = false;
            isTackleRunning_ = false;
            DoRest(REST_TIME);
            return;
        }
    }
}

void EnemyGolem::EffectTackle()
{ 
    if (!isTackleCharging_) return;   // チャージ中でなければ動かない

    //  チャージ中処理
   // エフェクトがまだなら再生する
    if (!isChargeEffectPlaying_)
    {
        chargeEffectPlayId_ = PlayEffekseer3DEffect(chargeEffectHandle_);

        SetPosPlayingEffekseer3DEffect(
            chargeEffectPlayId_,
            transform_.pos.x,
            transform_.pos.y + CHARGE_EFFECT_HEIGHT,
            transform_.pos.z
        );

        float scale = CHARGE_EFFECT_SCALE;

        SetScalePlayingEffekseer3DEffect(chargeEffectPlayId_, scale, scale, scale);

        isChargeEffectPlaying_ = true;
    }

    // チャージ時間を進める
    tackleTimer_ += SECOND_PER_FRAME;

    // チャージ完了 → タックルへ遷移
    if (tackleTimer_ >= chargeTime_)
    {
        // エフェクト停止
        if (chargeEffectPlayId_ != INVALID_HANDLE)
        {
            StopEffekseer3DEffect(chargeEffectPlayId_);
            chargeEffectPlayId_ = INVALID_HANDLE;
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
    movePow_ = AsoUtility::VECTOR_ZERO;

    // 状態遷移
    ChangeState(STATE::REST);
    animationController_->Play((int)ANIM_TYPE::REST, true);
}

void EnemyGolem::UpdateDead()
{
    float delta = SceneManager::GetInstance().GetDeltaTime();

    deadTimer_ += delta;

    if (gameOverReserved_ && deadTimer_ >= deadDelay_)
    {
        gameOverReserved_ = false;
        scene_.GameOver();
    }
}

bool EnemyGolem::Damage(int value)
{
    if (!alive_) return false;
    Hp_ -= value;

    if (Hp_ <= VECTOR_ZERO_VALUE)
    {
        Hp_ = VECTOR_ZERO_VALUE;
        alive_ = false;
 
        ChangeState(STATE::DEAD);
        animationController_->Play((int)ANIM_TYPE::DEAD, false);

        deadTimer_ = VECTOR_ZERO_VALUE;
        gameOverReserved_ = true;
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
    dir.y = VECTOR_ZERO_VALUE;
    if (VSize(dir) < MIN_MOVE_DISTANCE) return;

    // Golem モデルの前が -Z なので 180度回転を加える
    float rotRad = atan2f(dir.x, dir.z) + DX_PI_F;
    transform_.quaRot = Quaternion::Euler(VECTOR_ZERO_VALUE, rotRad, VECTOR_ZERO_VALUE);
}

void EnemyGolem::MoveTowardPlayer(float deltaTime)
{
    auto player = GetPlayer();
    if (!player) return;

    VECTOR toPlayer = VSub(player->GetPos(), transform_.pos);
    toPlayer.y = VECTOR_ZERO_VALUE;
    if (VSize(toPlayer) < MIN_MOVE_DISTANCE) return;

    // 目標回転設定
    double rotRad = atan2(toPlayer.x, toPlayer.z);
    SetGoalRotate(rotRad);

    // スムーズ回転
    Rotate(deltaTime);

    // 移動
    moveDir_ = VNorm(toPlayer);
    transform_.pos = VAdd(transform_.pos, VScale(moveDir_, moveSpeed_ * deltaTime));
}

bool EnemyGolem::CheckAttackHit(
    float forwardOffset,
    float heightOffset,
    float radius
)
{
    auto player = GetPlayer();
    if (!player) return false;

    // ★毎フレーム計算する
    VECTOR center = transform_.pos;

    // 前方向（モデルが -Z 向きなので反転）
    VECTOR forward = transform_.quaRot.GetForward();
    forward = VScale(forward, MODEL_FORWARD_REVERSE);

    // 攻撃判定の中心位置
    debugAttackPos_ = VAdd(
        VAdd(center, VScale(forward, forwardOffset)),
        VGet(VECTOR_ZERO_VALUE, heightOffset, VECTOR_ZERO_VALUE)
    );

    // 距離判定
    float dist = VSize(VSub(player->GetPos(), debugAttackPos_));

    return dist <= radius;
}

void EnemyGolem::DeactivateAttackHit()
{
    // 今回は何も必要なし（1フレームのみのダメージ処理）
}


void EnemyGolem::OnLanding(const MV1_COLL_RESULT_POLY& hit)
{
    movedPos_.y = hit.HitPosition.y + LAND_OFFSET_Y;

    // ゴーレムは跳ね・滑りを完全に無効化
    jumpPow_ = AsoUtility::VECTOR_ZERO;

    isGround_ = true;
   
}

void EnemyGolem::OnCapsuleHit(const MV1_COLL_RESULT_POLY& hit, const Capsule& cap)
{
    VECTOR n = hit.Normal;

    // ★Y方向の押し出しを禁止
    VECTOR horizontal = VGet(n.x, VECTOR_ZERO_VALUE, n.z);

    if (VSize(horizontal) > NORMALIZE_MIN_LENGTH)
    {
        horizontal = VNorm(horizontal);
        movedPos_ = VAdd(movedPos_, VScale(horizontal, WALL_PUSH_POWER));
    }
}

void EnemyGolem::DrawShadow(void)
{

    float GOLEM_SHADOW_HEIGHT = SHADOW_HEIGHT;
    float GOLEM_SHADOW_SIZE = SHADOW_SIZE;

    int i;
    MV1_COLL_RESULT_POLY_DIM HitResDim;
    MV1_COLL_RESULT_POLY* HitRes;
    VERTEX3D Vertex[3] = { VERTEX3D(), VERTEX3D(), VERTEX3D() };
    VECTOR SlideVec;
    int ModelHandle;

    // ライティングを無効にする
    SetUseLighting(FALSE);

    // Ｚバッファを有効にする
    SetUseZBuffer3D(TRUE);

    // テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
    SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

    // 影を落とすモデルの数だけ繰り返し
    for (const auto& c : colliders_)
    {

        // チェックするモデルは、jが0の時はステージモデル、1以上の場合はコリジョンモデル
        ModelHandle = c.lock()->modelId_;

        // プレイヤーの直下に存在する地面のポリゴンを取得
        HitResDim = MV1CollCheck_Capsule(
            ModelHandle, INVALID_POLYGON_INDEX,
            transform_.pos, VAdd(transform_.pos, { VECTOR_ZERO_VALUE, -GOLEM_SHADOW_HEIGHT, VECTOR_ZERO_VALUE }), GOLEM_SHADOW_SIZE);

        // 頂点データで変化が無い部分をセット
        GetColorU8(
            static_cast<int>(COLOR_MAX),
            static_cast<int>(COLOR_MAX),
            static_cast<int>(COLOR_MAX),
            static_cast<int>(COLOR_MAX)
        );

        Vertex[0].spc = GetColorU8(
            COLOR_INITIAL_VALUE,
            COLOR_INITIAL_VALUE,
            COLOR_INITIAL_VALUE,
            COLOR_INITIAL_VALUE
        );
        Vertex[0].su = VECTOR_ZERO_VALUE;
        Vertex[0].sv = VECTOR_ZERO_VALUE;
        Vertex[1] = Vertex[0];
        Vertex[2] = Vertex[0];

        // 球の直下に存在するポリゴンの数だけ繰り返し
        HitRes = HitResDim.Dim;
        for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
        {
            // ポリゴンの座標は地面ポリゴンの座標
            Vertex[0].pos = HitRes->Position[0];
            Vertex[1].pos = HitRes->Position[1];
            Vertex[2].pos = HitRes->Position[2];

            // ちょっと持ち上げて重ならないようにする
            SlideVec = VScale(HitRes->Normal, SHADOW_SLIDE_SCALE);
            Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
            Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
            Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

            // ポリゴンの不透明度を設定する
            Vertex[0].dif.a = COLOR_INITIAL_VALUE;
            Vertex[1].dif.a = COLOR_INITIAL_VALUE;
            Vertex[2].dif.a = COLOR_INITIAL_VALUE;
            if (HitRes->Position[0].y > transform_.pos.y - GOLEM_SHADOW_HEIGHT)
                Vertex[0].dif.a = static_cast<int>(roundf(SHADOW_ALPHA_MAX * (1.0f - fabs(HitRes->Position[0].y - transform_.pos.y) / GOLEM_SHADOW_HEIGHT)));

            if (HitRes->Position[1].y > transform_.pos.y - GOLEM_SHADOW_HEIGHT)
                Vertex[1].dif.a = static_cast<int>(roundf(SHADOW_ALPHA_MAX * (1.0f - fabs(HitRes->Position[1].y - transform_.pos.y) / GOLEM_SHADOW_HEIGHT)));

            if (HitRes->Position[2].y > transform_.pos.y - GOLEM_SHADOW_HEIGHT)
                Vertex[2].dif.a = static_cast<int>(roundf(SHADOW_ALPHA_MAX * (1.0f - fabs(HitRes->Position[2].y - transform_.pos.y) / GOLEM_SHADOW_HEIGHT)));

			float uvScale = GOLEM_SHADOW_SIZE * SHADOW_UV_SCALE;

            // ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
            Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / uvScale + SHADOW_UV_OFFSET;
            Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / uvScale + SHADOW_UV_OFFSET;
            Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / uvScale + SHADOW_UV_OFFSET;
            Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / uvScale + SHADOW_UV_OFFSET;
            Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / uvScale + SHADOW_UV_OFFSET;
            Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / uvScale + SHADOW_UV_OFFSET;

            // 影ポリゴンを描画
            DrawPolygon3D(Vertex, SHADOW_POLYGON_COUNT, imgShadow_, TRUE);
        }

        // 検出した地面ポリゴン情報の後始末
        MV1CollResultPolyDimTerminate(HitResDim);
    }

    // ライティングを有効にする
    SetUseLighting(TRUE);

    // Ｚバッファを無効にする
    SetUseZBuffer3D(FALSE);

}

void EnemyGolem::SetGoalRotate(double rotRad)
{
    goalQuaRot_ = Quaternion::Euler(VECTOR_ZERO_VALUE, (float)rotRad, VECTOR_ZERO_VALUE);
}

void EnemyGolem::Rotate(float deltaTime)
{
    transform_.quaRot = Quaternion::Slerp(transform_.quaRot
        , goalQuaRot_
        , deltaTime * ROTATE_SPEED);

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
    attackHitDone_ = false;
    stateUpdate_ = std::bind(&EnemyGolem::UpdateAttack, this);
}

void EnemyGolem::ChangeStateTackle()
{
    isTackling_ = true;
    tackleHitDone_ = false;
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