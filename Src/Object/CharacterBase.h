#pragma once
#include "Common/Transform.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "../Manager/SceneManager.h"
#include "../Utility/AsoUtility.h"
#include <memory>
#include <vector>

class ResourceManager;
class SceneManager;

class CharacterBase
{
public:
    CharacterBase();
    virtual ~CharacterBase();

    virtual void Init(void) = 0;
    virtual void Update(void) = 0;
    virtual void Draw(void) = 0;
    virtual bool Release(void) = 0;

    const Transform& GetTransform(void) const { return transform_; }
    const VECTOR& GetPos(void) const { return transform_.pos; }

protected:
    // 各種マネージャー
    ResourceManager& resMng_;
    SceneManager& scnMng_;

    // モデル制御
    Transform transform_;
    std::unique_ptr<Capsule> capsule_;

    VECTOR LastPos_; //最後にいた座標
};
