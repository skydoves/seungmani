#ifndef __CATCHMANI_SCENE_H__
#define __CATCHMANI_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class Catchmani: public Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(Catchmani);

	///<summary>
	/// <Java Native InterFace>
	///  C++  ==>  Java
	///</summary>
	void JavaFun_kakaolink(char*);
	void JavaFun_ad(int);

	// System Init
	void Init_Graphics();
	void Init_Systems();
	void Init_GameValues();
	void Set_CharAbility();

	// Vectors
	Vector<Sprite*> Mobs_HP_Bar;
	Vector<Sprite*> Mob1;
	Vector<Sprite*> Item;

	// Menus
	void CloseButton(Ref* pSender);
	void Skill(Ref* pSender);
	void Skill_Special(Ref* pSender);
	void Ask_Exit(Ref* pSender, int value);
	void KakaoLink(Ref* pSender);

	// Tutorial
	void Tutorial();
	void SpeechBubble(Ref* pSender, int color, double scale, int x, int y, double time, const char* mess, bool flipX, bool flipY);
	void DeleteBubble(Ref* pSender);
	void Tutorial_End(Ref* pSender);

	// Game System Schedules
	void Update_UI_Status(float delta);
	void Update_Moving(float delta);
	void Update_S_Skill(float delta);

	// Game Monsters
	void Make_Mob(int);
	void Mob_Gen(float delta);
	void Touch_Mob(Point);
	void Crash_Mob(float delta);
	void Update_Mobs(float delta);
	void Mob_Effect(char* particle, Point location, int sound);
	void DisposeStrun(Ref* pSender);

	// Boss Monster
	void Make_Boss();
	void Touch_Boss(Point);
	void Update_Boss(float delta);

	// End Game
	void Gameover();
	void GameClear();

	// Pet
	void Init_Pet();
	void PetSkill1(float delta);
	void PetSkill2(float delta);
	void PetSkill3(float delta);

	// Item
	void Touch_Item(Point);

	// Data
	int getintData(char*, int);
	void setintData(char*, int);
	char* Get_CharImage(int);

	// Sound Methods
	void sound_click();
	void BackgroundMusic(float delta);

	//Touch Event
	EventListenerTouchOneByOne *listener2;
	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
};

#endif // __HELLOWORLD_SCENE_H__
