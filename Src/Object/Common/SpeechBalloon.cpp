#include <DxLib.h>
#include "Transform.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Resource.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"
#include "SpeechBalloon.h"

SpeechBalloon::SpeechBalloon(TYPE type, const Transform& parent) : transformParent_(parent)
{

	type_ = type;

	Resource res = ResourceManager::GetInstance().Load(ResourceManager::SRC::SPEECH_BALLOON);
	image_ = res.handleId_;

	isVisiblePermanent_ = true;
	visibleTime_ = INITIAL_VISIBLE_TIME;

	pos_ = AsoUtility::VECTOR_ZERO;
	localPos_ = AsoUtility::VECTOR_ZERO;;

}

SpeechBalloon::~SpeechBalloon(void)
{
}

void SpeechBalloon::Draw(void)
{

	if (!isVisiblePermanent_)
	{
		visibleTime_ -= SceneManager::GetInstance().GetDeltaTime();
		if (visibleTime_ < INITIAL_VISIBLE_TIME)
		{
			return;
		}
	}

	// 対象物の姿勢から見た、上方向に3D座標をずらす
	VECTOR relativePos = transformParent_.quaRot.PosAxis(localPos_);
	VECTOR pos = VAdd(transformParent_.pos, relativePos);
	pos_ = ConvWorldPosToScreenPos(pos);

	switch (type_)
	{
	case SpeechBalloon::TYPE::TEXT:
		DrawTextOrg();
		break;
	case SpeechBalloon::TYPE::SPEECH:
		DrawSpeech();
		break;
	}

}

void SpeechBalloon::DrawTextOrg(void)
{

	// カメラのSetCameraNearFarから外れていた場合、表示しない
	if (pos_.z > SCREEN_DEPTH_MIN && pos_.z < SCREEN_DEPTH_MAX)
	{
		DrawFormatString(
			static_cast<int>(pos_.x), static_cast<int>(pos_.y),
			TEXT_COLOR, "%s", text_.c_str());
	}

}

void SpeechBalloon::DrawSpeech(void)
{

	// カメラのSetCameraNearFarから外れていた場合、表示しない
	if (pos_.z > SCREEN_DEPTH_MIN && pos_.z < SCREEN_DEPTH_MAX)
	{
		int x = AsoUtility::Round(pos_.x);
		int y = AsoUtility::Round(pos_.y);
		DrawRotaGraph(x, y, SPEECH_DRAW_SCALE, DRAW_ANGLE, image_, true);
		int len = (int)strlen(text_.c_str());
		int width = GetDrawStringWidth(text_.c_str(), len);
		DrawFormatString(
			x - (width / TEXT_CENTER_DIVISOR),
			y - SPEECH_TEXT_OFFSET_Y,
			SPEECH_TEXT_COLOR,
			"%s",
			text_.c_str());
	}

}

void SpeechBalloon::SetText(const std::string& text)
{
	text_ = text;
}

void SpeechBalloon::SetTime(double time)
{
	isVisiblePermanent_ = false;
	visibleTime_ = time;
}

void SpeechBalloon::SetType(TYPE type)
{
	type_ = type;
}

void SpeechBalloon::SetVisiblePermanent(void)
{
	isVisiblePermanent_ = true;
}

void SpeechBalloon::SetLocalPos(const VECTOR& localPos)
{
	localPos_ = localPos;
}
