#ifndef __Touchmani_SCENE_H__
#define __Touchmani_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class Touchmani : public Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(Touchmani);

	///<summary>
	/// <Java Native InterFace>
	///  C++  ==>  Java
	///</summary>
	void JavaFun(char*);
	void JavaFun_kakaolink(char*);
	void JavaFun_ad(int);

	//init
	void Init_Start();
	void Init_Graphics();
	void Set_CharAbility();
	void Game_init();
	void PetInit();

	//Menus
	void CloseButton(Ref* pSender);
	void Ask_Exit(Ref* pSender, int value);
	void Skill(Ref* pSender);
	void Heal(Ref* pSender);
	void KakaoLink(Ref* pSender);

	//In Game
	void Update_UI_Status(float delta);
	void Update_Char(float delta);
	void Update_Mobs(float delta);
	void Hit_Mobs(char* particle, Point location, int sound);
	void Update_Mob1(float delta);
	void Update_Mob2(float delta);
	void Update_Mob3(float delta);
	void Update_Mob4(float delta);
	void Update_Mob5(float delta);
	void Update_Mob6(float delta);
	void Game_Over(Ref* pSender);
	void update(float delta);
	void PetSkill1(float delta);
	void PetSkill2(float delta);

	//Vectors
	Vector<Sprite*> Mobs_HP_Bar;
	Vector<Sprite*> Mob1;
	Vector<Sprite*> Item;

	//Data
	int getintData(char*, int);
	void setintData(char*, int);
	char* Get_CharImage(int);
	void Cal_FakeScore(int);

	//Sound Methods
	void sound_click();
	void BackgroundMusic(float delta);

	//Tutorial
	void Tutorial();
	void SpeechBubble(Ref* pSender, int color, double scale, int x, int y, double time, const char* mess, bool flipX, bool flipY);
	void DeleteBubble(Ref* pSender);

	void RandomBubble(const char*, const char*, const char*);
	void DisposeStrun(Ref* pSender);

	//Touch Event
	EventListenerTouchOneByOne *listener1;
	bool onTouchBegan(Touch *touch, Event *unused_event);
};

#endif // __HELLOWORLD_SCENE_H__
