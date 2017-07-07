#ifndef __Main_SCENE_H__
#define __Main_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class MainScene : public Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(MainScene);

	///<summary>
	/// <Java Native InterFace>
	///  C++  ==>  Java
	///</summary>
	void JavaFun(int);

	void Init_Start();

	//Schedules
	void Updates(float delta);

	//Menu_Buttons
	void Button_Status(Ref* pSender);
	void Button_Game(Ref* pSender);
	void Button_Game2_List(Ref* pSender);
	void Button_Shop(Ref* pSender);
	void Button_Rank(Ref* pSender);
	void Button_Setting(Ref* pSender);
	void Button_Info(Ref* pSender);
	void Menu_Change(char*);
	void Button_Game1(Ref* pSender, int value);
	void Button_Game2(Ref* pSender, int value);
	void Pet_shop(Ref* pSender);
	void Rank_ScorollDown(Ref* pSender);
	void Sound_OnOff(Ref* pSender);
	void StatusUp(Ref* pSender, int value);
	void UpPowerAndShield(Ref* pSender, int value);
	void UpHPAndHPUP(Ref* pSender, int value);
	void UpMPAndMPUP(Ref* pSender, int value);
	void CharInfo(Ref* pSender,const char*, int,const char*,const char*, int, int, int, int, int, int, const char*, const char*);
	void CharMenus(Ref* pSender, int value, const char* CharData, int MainChar, int cost);
	void Special_Skill(Ref* pSender);
	void Skill_Info(Ref* pSender, int);
	void Select_Skill(Ref* pSender, int);

	//Touch
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event);
	void Ask_Exit(Ref* pSender, int value);

	//Tutorial
	void Tutorial();
	void SpeechBubble(Ref* pSender, int color, double scale, int x, int y, double time, const char* mess, bool flipX, bool flipY);
	void DeleteBubble(Ref* pSender);

	//Data
	int getintData(char*, int);
	void setintData(char*, int);

	//Info Methods
	void Info_Mess(int Color, char* mess);
	char* Get_CharImage(int);

	//Sound Methods
	void sound_click();
	void BackgroundMusic(float delta);

};

#endif
