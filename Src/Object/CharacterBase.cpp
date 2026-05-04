#include "../Utility/AsoUtility.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "CharacterBase.h"


CharacterBase::CharacterBase()
    : resMng_(ResourceManager::GetInstance())
    , scnMng_(SceneManager::GetInstance())
{
}

CharacterBase::~CharacterBase() 
{
}
