#pragma once
#include "CardBase.h"
class Monster :
    public CardBase
{

	//‰Šú‰»
	bool Init(void) override;
	//XV
	void Update(void) override;
	//•`‰æ
	void Draw(void) override;
	//‰ğ•ú
	bool Release(void) override;

	void SetParam() override;

};

