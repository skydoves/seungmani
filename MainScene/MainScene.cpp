#include "MainScene.h"
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Touchmani.h"
#include "Catchmani.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;
using namespace CocosDenshion;
using namespace std;

int RankData1[10][2], RankCount=0, RankState=0;
string RankData2[10];

///<summary>
/// <Java Native InterFace>
///  Java  ==>  c++
///</summary>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"

#ifdef __cplusplus
extern "C" {
#endif

	// get ranks from server
	jint Java_org_cocos2dx_cpp_AppActivity_GetRank(JNIEnv *env, jobject obj, jint mainchar, jint score, jstring Name)
	{
		// jstring to string
		const char *nativeString = env->GetStringUTFChars(Name, JNI_FALSE);

		RankData1[RankCount][0] = mainchar;
		RankData1[RankCount][1] = score;
		RankData2[RankCount] = nativeString;

		RankCount++;

		if(mainchar == 100 && score == 123 || RankCount == 10)
			RankState=1;
	}

#ifdef __cplusplus
}
#endif

#endif


///<summary>
/// <Declare Global variables >
///</summary>
int static GameState = 0; // now game state
int static TutorialState = 0;
int static TouchEnable = 0; // 0 : touch enable,  1 : touch unable
int static SubMenuState = 0; // 0 : not active,  1 : subMenuState on active


Scene* MainScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MainScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	// add touch event listenter at eventDIspatcher
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = CC_CALLBACK_2(MainScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

	// asdasdasdasdasdasdasd... - hackers yayyy!!! >< for what?
	if (getintData("asdfasd", 0) == 0)
	{
		if (getintData("money", 0) == 0)
		setintData("money", 2000);

		setintData("asdfasd", 14215241); // __
		setintData("asdfasda", 115124124); // __
		setintData("asdfasdas", 15125121); // __
		setintData("asdfasdasd", 46343432); // __
		setintData("asdfasdfasdf", 92858281); //
		setintData("asdf4sdf2sdf", 51251251); //__
		setintData("as5f42df2sdf", 50301030); // __
		setintData("as5f425f2sdf", 0);  // _ _
	}

	// default skills
	if (getintData("char1_Normal", 0) == 0)
		setintData("char1_Normal", 1);
	setintData("Special_Skill1", 1);
	setintData("Special_Skill2", 1);

	if (getintData("onState", 0) == 0)
		Tutorial();
	else
	    Init_Start();

	return true;
}

void MainScene::Init_Start()
{
	if (GameState == 0)
	{
		string Name_value = "    ";
		Name_value.append(UserDefault::getInstance()->getStringForKey("Name", "부엉이")); // why am I an owl for 9 years?
		Name_value.append("님 반가워요^^");
		const char * Name = Name_value.c_str();
		Info_Mess(1, (char*)Name);
		GameState = 1;
	}

	// get a new skill message
	if (getintData("Game2_Stage5", -1) != -1 && getintData("Special_Skill3", 0) == 0)
	{
		Info_Mess(1, "    특수스킬 '스틸' 획득!");
		setintData("Special_Skill3", 1);
	}
	if (getintData("Game1_HighScore", 0) >= 1500000 && getintData("Special_Skill4", 0) == 0)
	{
		Info_Mess(1, "    특수스킬 '차원의 벽' 획득!");
		setintData("Special_Skill4", 1);
	}


	// play background musics
	if (getintData("sound", 0) == 0)
	{
		SimpleAudioEngine::getInstance()->playBackgroundMusic("sounds/Lovely Summer.mp3");
		this->schedule(schedule_selector(MainScene::BackgroundMusic), 34.0);
	}

	// background image
	auto spr_bg = Sprite::create("img/background2.png");
	spr_bg->setAnchorPoint(Point(0, 1));
	spr_bg->setPosition(Point(0, 320));
	this->addChild(spr_bg, 0);

	// objects image
	auto spr_object1 = Sprite::create("img/char_cat.png");
	spr_object1->setAnchorPoint(Point(0, 1)); spr_object1->setFlippedY(true);
	spr_object1->setPosition(Point(390, 320));
	this->addChild(spr_object1, 0);

	// main menu board image
	auto spr_menu = Sprite::create("img/MainMenu.png");
	spr_menu->setAnchorPoint(Point(0, 1));
	spr_menu->setOpacity(200); spr_menu->setScale(0.9);
	spr_menu->setPosition(Point(47, 290));
	this->addChild(spr_menu, 1, "MainBoard");

	// menu buttons
	auto item1 = MenuItemImage::create("img/icon_status.png", "img/icon_status.png", CC_CALLBACK_1(MainScene::Button_Status, this));
	auto item2 = MenuItemImage::create("img/icon_game.png", "img/icon_game.png", CC_CALLBACK_1(MainScene::Button_Game, this));
	auto item3 = MenuItemImage::create("img/icon_shop.png", "img/icon_shop.png", CC_CALLBACK_1(MainScene::Button_Shop, this));
	auto item4 = MenuItemImage::create("img/icon_rank.png", "img/icon_rank.png", CC_CALLBACK_1(MainScene::Button_Rank, this));
	auto item5 = MenuItemImage::create("img/icon_setting.png", "img/icon_setting.png", CC_CALLBACK_1(MainScene::Button_Setting, this));
	auto item6 = MenuItemImage::create("img/icon_info.png", "img/icon_info.png", CC_CALLBACK_1(MainScene::Button_Info, this));
	auto menu = Menu::create(item1, item2, item3, item4, item5, item6, NULL);
	item1->setPosition(Point(-195, 78));
	item2->setPosition(Point(-195, 41));
	item3->setPosition(Point(-195, 4));
	item4->setPosition(Point(-195, -33));
	item5->setPosition(Point(-195, -69));
	item6->setPosition(Point(-194, -106));
	menu->setOpacity(200);
	this->addChild(menu, 2);

	// money image
	auto spr_money = Sprite::create("img/UI_Money.png");
	spr_money->setAnchorPoint(Point(0, 1));
	spr_money->setPosition(Point(3, 317)); spr_money->setScale(1.2);
	this->addChild(spr_money, 1);

	auto label_money = Label::createWithTTF(StringUtils::format("%d", getintData("money", 0)), "fonts/Navi.ttf", 15);
	label_money->setAnchorPoint(Point(0, 1));
	label_money->setPosition(Point(29, 312)); label_money->setColor(Color3B(-255, -255, -255));
	this->addChild(label_money, 1, "money");

	// ranking systems
	auto this_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::Button_Rank, this, this));
	this->runAction(this_Func1);

	// set main update scheduler
	this->schedule(schedule_selector(MainScene::Updates), 0.01);
}


///<summary>
/// <Main Buttons UI>
///  Main Menus
///  GameState : 1 = Noraml, 2 = Status, 3 = Game(31 = Game2), 4 = Shop(41 = PetShop), 5 = Rank, 6 = Info, 7 = Setting
///</summary>


///<summary>
///  Main Menu System
///</summary>
void MainScene::Button_Status(Ref* pSender)
{
	if (TouchEnable == 0 && GameState != 2 && SubMenuState == 0)
	{
		GameState = 2;
		sound_click();
		Menu_Change("img/MainMenu_status.png");
		auto MainBoard = (Sprite*)this->getChildByName("MainBoard");

		//Label
		std::string Name_value = UserDefault::getInstance()->getStringForKey("Name", " ");
		const char * Name = Name_value.c_str();

		auto Label_Name = Label::createWithTTF(StringUtils::format("%s", Name), "fonts/Navi.ttf", 15);
		Label_Name->setAnchorPoint(Point(0, 1));
		Label_Name->setPosition(Point(215 - (Name_value.size()-2) * 1.5, 208)); Label_Name->setColor(Color3B(-255, -255, -255));
		MainBoard->addChild(Label_Name, 5);

		//Main Char image
		auto Char_Maker = Sprite::create(Get_CharImage(getintData("MainChar", 0)));
		Char_Maker->setAnchorPoint(Point(0, 1));
		Char_Maker->setPosition(Point(185, 180));
		Char_Maker->setOpacity(220); Char_Maker->setScale(0.8);
		MainBoard->addChild(Char_Maker, 5);

		//Main Pet image
		if (getintData("MainPet", 0) != 0)
		{
			auto Char_Pet = Sprite::create(Get_CharImage(getintData("MainPet", 0)));
			Char_Pet->setAnchorPoint(Point(0, 1));
			Char_Pet->setPosition(Point(320, 70));
			Char_Pet->setOpacity(220);
			MainBoard->addChild(Char_Pet, 5);
		}

		//Icons
		auto item1 = MenuItemImage::create("img/Icon_ItemBox_Shield.png", "img/Icon_ItemBox_Shield.png", CC_CALLBACK_1(MainScene::StatusUp, this, 0));
		auto item2 = MenuItemImage::create("img/Icon_ItemBox_HP.png", "img/Icon_ItemBox_HP.png", CC_CALLBACK_1(MainScene::StatusUp, this, 1));
		auto item3 = MenuItemImage::create("img/Icon_ItemBox_MP.png", "img/Icon_ItemBox_MP.png", CC_CALLBACK_1(MainScene::StatusUp, this, 2));
		auto menu = Menu::create(item1, item2, item3, NULL);
		item1->setPosition(Point(50, -25)); item1->setScale(1.2);
		item2->setPosition(Point(-85, -25)); item2->setScale(1.2);
		item3->setPosition(Point(-17, -95)); item3->setScale(1.2);
		menu->setOpacity(220);
		MainBoard->addChild(menu, 5);
	}
}


/*
	Status
*/
//value  0: 공격력, 방어력  1 : 체력, 체력 증가 계수  2 : 마력, 마력 증가 계수 4 : 종료 
void MainScene::StatusUp(Ref* pSender, int value)
{
	//===============================
	//Power & Defense Upgrade SubMenu
	//===============================
	if (TouchEnable == 0 && SubMenuState == 0 && value == 0)
	{
		sound_click();
		SubMenuState = 1;

		//Board & Buttons
		auto item1 = MenuItemImage::create("img/Game1_Menu.png", "img/Game1_Menu.png", CC_CALLBACK_1(MainScene::StatusUp, this, 100));
		auto item2 = MenuItemImage::create("img/Button_Upgrade.png", "img/Button_Upgrade.png", CC_CALLBACK_1(MainScene::UpPowerAndShield, this, 0));
		auto item3 = MenuItemImage::create("img/Button_Upgrade.png", "img/Button_Upgrade.png", CC_CALLBACK_1(MainScene::UpPowerAndShield, this, 1));
		auto item4 = MenuItemImage::create("img/Button_close.png", "img/Button_close.png", CC_CALLBACK_1(MainScene::StatusUp, this, 4));
		auto menu = Menu::create(item1, item2, item3, item4, NULL);
		item2->setPosition(Point(10, 15));
		item3->setPosition(Point(10,-92));
		item4->setPosition(Point(135, 105));
		menu->setOpacity(235);
		this->addChild(menu, 6, "SubMenu");

		//Icon image
		auto spr_Icon = Sprite::create("img/Icon_Shield.png");
		spr_Icon->setAnchorPoint(Point(0, 1));
		spr_Icon->setPosition(Point(160, 257));
		spr_Icon->setOpacity(220);
		item1->addChild(spr_Icon, 5);

		//Borad1 image
		auto spr_SubBoard1 = Sprite::create("img/MainSubBoard4.png");
		spr_SubBoard1->setAnchorPoint(Point(0, 1));
		spr_SubBoard1->setPosition(Point(20, 237));
		spr_SubBoard1->setOpacity(220);
		item1->addChild(spr_SubBoard1, 5);

		//Label item1 Name
		auto Label_Power_Name = Label::createWithTTF(StringUtils::format("공격력"), "fonts/Navi.ttf", 18);
		Label_Power_Name->setAnchorPoint(Point(0, 1));
		Label_Power_Name->setPosition(Point(150, 227)); Label_Power_Name->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Power_Name, 5);

		//Star image - inactive
		for (int i = 0; i < 5; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star_inactive.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 202));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Star image
		for (int i = 0; i < getintData("Stat_Char_Poewr", 0); i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25*i, 202));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Label Upgrade Cost
		auto Label_Cost1 = Label::createWithTTF(StringUtils::format("강화 비용\n %d" , (getintData("Stat_Char_Poewr", 0)+1)*5000), "fonts/Navi.ttf", 14);
		Label_Cost1->setAnchorPoint(Point(0, 1));
		Label_Cost1->setPosition(Point(45, 197)); Label_Cost1->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Cost1, 5);

		if (getintData("Stat_Char_Poewr", 0) == 5)
		{
			item1->removeChild(Label_Cost1);
			item2->setColor(Color3B(189, 189, 189));
		}

		//Borad2 image
		auto spr_SubBoard2 = Sprite::create("img/MainSubBoard4.png");
		spr_SubBoard2->setAnchorPoint(Point(0, 1));
		spr_SubBoard2->setPosition(Point(20, 132));
		spr_SubBoard2->setOpacity(220);
		item1->addChild(spr_SubBoard2, 5);

		//Label item2 Name
		auto Label_Defense_Name = Label::createWithTTF(StringUtils::format("방어력"), "fonts/Navi.ttf", 18);
		Label_Defense_Name->setAnchorPoint(Point(0, 1));
		Label_Defense_Name->setPosition(Point(150, 122)); Label_Defense_Name->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Defense_Name, 5);

		//Star image - inactive
		for (int i = 0; i < 5; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star_inactive.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 97));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Star image
		for (int i = 0; i < getintData("Stat_Char_Defense", 0); i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 97));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Label Upgrade Cost
		auto Label_Cost2 = Label::createWithTTF(StringUtils::format("강화 비용\n %d", (getintData("Stat_Char_Defense", 0) + 1) * 5000), "fonts/Navi.ttf", 14);
		Label_Cost2->setAnchorPoint(Point(0, 1));
		Label_Cost2->setPosition(Point(45, 92)); Label_Cost2->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Cost2, 5);

		if (getintData("Stat_Char_Defense", 0) == 5)
		{
			item1->removeChild(Label_Cost2);
			item3->setColor(Color3B(189, 189, 189));
		}
	}


	//===============================
	//HP, HP UP Upgrade SubMenu
	//===============================
	else if (TouchEnable == 0 && SubMenuState == 0 && value == 1)
	{
		sound_click();
		SubMenuState = 1;

		//Board & Buttons
		auto item1 = MenuItemImage::create("img/Game1_Menu.png", "img/Game1_Menu.png", CC_CALLBACK_1(MainScene::StatusUp, this, 100));
		auto item2 = MenuItemImage::create("img/Button_Upgrade.png", "img/Button_Upgrade.png", CC_CALLBACK_1(MainScene::UpHPAndHPUP, this, 0));
		auto item3 = MenuItemImage::create("img/Button_Upgrade.png", "img/Button_Upgrade.png", CC_CALLBACK_1(MainScene::UpHPAndHPUP, this, 1));
		auto item4 = MenuItemImage::create("img/Button_close.png", "img/Button_close.png", CC_CALLBACK_1(MainScene::StatusUp, this, 4));
		auto menu = Menu::create(item1, item2, item3, item4, NULL);
		item2->setPosition(Point(10, 15));
		item3->setPosition(Point(10, -92));
		item4->setPosition(Point(135, 105));
		menu->setOpacity(235);
		this->addChild(menu, 6, "SubMenu");

		//Icon image
		auto spr_Icon = Sprite::create("img/Icon_heart.png");
		spr_Icon->setAnchorPoint(Point(0, 1));
		spr_Icon->setPosition(Point(155, 257));
		spr_Icon->setOpacity(220);
		item1->addChild(spr_Icon, 5);

		//Borad1 image
		auto spr_SubBoard1 = Sprite::create("img/MainSubBoard4.png");
		spr_SubBoard1->setAnchorPoint(Point(0, 1));
		spr_SubBoard1->setPosition(Point(20, 237));
		spr_SubBoard1->setOpacity(220);
		item1->addChild(spr_SubBoard1, 5);

		//Label item1 Name
		auto Label_Power_Name = Label::createWithTTF(StringUtils::format("체 력"), "fonts/Navi.ttf", 18);
		Label_Power_Name->setAnchorPoint(Point(0, 1));
		Label_Power_Name->setPosition(Point(150, 227)); Label_Power_Name->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Power_Name, 5);

		//Star image - inactive
		for (int i = 0; i < 5; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star_inactive.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 202));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Star image
		for (int i = 0; i < getintData("Stat_Char_HP", 0); i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 202));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Label Upgrade Cost
		auto Label_Cost1 = Label::createWithTTF(StringUtils::format("강화 비용\n %d", (getintData("Stat_Char_HP", 0) + 1) * 5000), "fonts/Navi.ttf", 14);
		Label_Cost1->setAnchorPoint(Point(0, 1));
		Label_Cost1->setPosition(Point(45, 197)); Label_Cost1->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Cost1, 5);

		if (getintData("Stat_Char_HP", 0) == 5)
		{
			item1->removeChild(Label_Cost1);
			item2->setColor(Color3B(189, 189, 189));
		}

		//Borad2 image
		auto spr_SubBoard2 = Sprite::create("img/MainSubBoard4.png");
		spr_SubBoard2->setAnchorPoint(Point(0, 1));
		spr_SubBoard2->setPosition(Point(20, 132));
		spr_SubBoard2->setOpacity(220);
		item1->addChild(spr_SubBoard2, 5);

		//Label item2 Name
		auto Label_Defense_Name = Label::createWithTTF(StringUtils::format("체력 재생"), "fonts/Navi.ttf", 18);
		Label_Defense_Name->setAnchorPoint(Point(0, 1));
		Label_Defense_Name->setPosition(Point(150, 122)); Label_Defense_Name->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Defense_Name, 5);

		//Star image - inactive
		for (int i = 0; i < 5; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star_inactive.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 97));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Star image
		for (int i = 0; i < getintData("Stat_Char_HPUP", 0); i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 97));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Label Upgrade Cost
		auto Label_Cost2 = Label::createWithTTF(StringUtils::format("강화 비용\n %d", (getintData("Stat_Char_HPUP", 0) + 1) * 5000), "fonts/Navi.ttf", 14);
		Label_Cost2->setAnchorPoint(Point(0, 1));
		Label_Cost2->setPosition(Point(45, 92)); Label_Cost2->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Cost2, 5);

		if (getintData("Stat_Char_HPUP", 0) == 5)
		{
			item1->removeChild(Label_Cost2);
			item3->setColor(Color3B(189, 189, 189));
		}
	}


	//===============================
	//MP, MP UP Upgrade SubMenu
	//===============================
	else if (TouchEnable == 0 && SubMenuState == 0 && value == 2)
	{
		sound_click();
		SubMenuState = 1;

		//Board & Buttons
		auto item1 = MenuItemImage::create("img/Game1_Menu.png", "img/Game1_Menu.png", CC_CALLBACK_1(MainScene::StatusUp, this, 100));
		auto item2 = MenuItemImage::create("img/Button_Upgrade.png", "img/Button_Upgrade.png", CC_CALLBACK_1(MainScene::UpMPAndMPUP, this, 0));
		auto item3 = MenuItemImage::create("img/Button_Upgrade.png", "img/Button_Upgrade.png", CC_CALLBACK_1(MainScene::UpMPAndMPUP, this, 1));
		auto item4 = MenuItemImage::create("img/Button_close.png", "img/Button_close.png", CC_CALLBACK_1(MainScene::StatusUp, this, 4));
		auto menu = Menu::create(item1, item2, item3, item4, NULL);
		item2->setPosition(Point(10, 15));
		item3->setPosition(Point(10, -92));
		item4->setPosition(Point(135, 105));
		menu->setOpacity(235);
		this->addChild(menu, 6, "SubMenu");

		//Icon image
		auto spr_Icon = Sprite::create("img/Icon_skill.png");
		spr_Icon->setAnchorPoint(Point(0, 1));
		spr_Icon->setPosition(Point(160, 257));
		spr_Icon->setOpacity(220);
		item1->addChild(spr_Icon, 5);

		//Borad1 image
		auto spr_SubBoard1 = Sprite::create("img/MainSubBoard4.png");
		spr_SubBoard1->setAnchorPoint(Point(0, 1));
		spr_SubBoard1->setPosition(Point(20, 237));
		spr_SubBoard1->setOpacity(220);
		item1->addChild(spr_SubBoard1, 5);

		//Label item1 Name
		auto Label_Power_Name = Label::createWithTTF(StringUtils::format("기 력"), "fonts/Navi.ttf", 18);
		Label_Power_Name->setAnchorPoint(Point(0, 1));
		Label_Power_Name->setPosition(Point(150, 227)); Label_Power_Name->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Power_Name, 5);

		//Star image - inactive
		for (int i = 0; i < 5; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star_inactive.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 202));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Star image
		for (int i = 0; i < getintData("Stat_Char_MP", 0); i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 202));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Label Upgrade Cost
		auto Label_Cost1 = Label::createWithTTF(StringUtils::format("강화 비용\n %d", (getintData("Stat_Char_MP", 0) + 1) * 5000), "fonts/Navi.ttf", 14);
		Label_Cost1->setAnchorPoint(Point(0, 1));
		Label_Cost1->setPosition(Point(45, 197)); Label_Cost1->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Cost1, 5);

		if (getintData("Stat_Char_MP", 0) == 5)
		{
			item1->removeChild(Label_Cost1);
			item2->setColor(Color3B(189, 189, 189));
		}

		//Borad2 image
		auto spr_SubBoard2 = Sprite::create("img/MainSubBoard4.png");
		spr_SubBoard2->setAnchorPoint(Point(0, 1));
		spr_SubBoard2->setPosition(Point(20, 132));
		spr_SubBoard2->setOpacity(220);
		item1->addChild(spr_SubBoard2, 5);

		//Label item2 Name
		auto Label_Defense_Name = Label::createWithTTF(StringUtils::format("기력 재생"), "fonts/Navi.ttf", 18);
		Label_Defense_Name->setAnchorPoint(Point(0, 1));
		Label_Defense_Name->setPosition(Point(150, 122)); Label_Defense_Name->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Defense_Name, 5);

		//Star image - inactive
		for (int i = 0; i < 5; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star_inactive.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 97));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Star image
		for (int i = 0; i < getintData("Stat_Char_MPUP", 0); i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(115 + 25 * i, 97));
			Spr_Star->setOpacity(220);
			item1->addChild(Spr_Star, 5);
		}

		//Label Upgrade Cost
		auto Label_Cost2 = Label::createWithTTF(StringUtils::format("강화 비용\n %d", (getintData("Stat_Char_MPUP", 0) + 1) * 5000), "fonts/Navi.ttf", 14);
		Label_Cost2->setAnchorPoint(Point(0, 1));
		Label_Cost2->setPosition(Point(45, 92)); Label_Cost2->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Cost2, 5);

		if (getintData("Stat_Char_MPUP", 0) == 5)
		{
			item1->removeChild(Label_Cost2);
			item3->setColor(Color3B(189, 189, 189));
		}
	}

	//===============================
	//Close SubMenu
	//===============================
	else if (TouchEnable == 0 && SubMenuState == 1 && value == 4)
	{
		sound_click();
		SubMenuState = 0;
		auto SubMenu = (Sprite*)this->getChildByName("SubMenu");
		this->removeChild(SubMenu);
	}
}

//Upgrade Power & Shield
void MainScene::UpPowerAndShield(Ref* pSender, int value)
{
	if (value == 0)
	{
		if (getintData("Stat_Char_Poewr", 0) != 5 && getintData("money", 0) >= ((getintData("Stat_Char_Poewr", 0) + 1) * 5000))
		{
			setintData("money", getintData("money", 0) - ((getintData("Stat_Char_Poewr", 0) + 1) * 5000));
			setintData("Stat_Char_Poewr", (getintData("Stat_Char_Poewr", 0) + 1));

			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/buy.ogg");

			//Refresh Menu
			auto this_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 4));
			this->runAction(this_Func1);
			auto this_Func2 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 0));
			this->runAction(this_Func2);
		}
		else
		{
			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
		}
	}
	else if (value == 1)
	{
		if (getintData("Stat_Char_Defense", 0) != 5 && getintData("money", 0) >= ((getintData("Stat_Char_Defense", 0) + 1) * 5000))
		{
			setintData("money", getintData("money", 0) - ((getintData("Stat_Char_Defense", 0) + 1) * 5000));
			setintData("Stat_Char_Defense", (getintData("Stat_Char_Defense", 0) + 1));

			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/buy.ogg");

			//Refresh Menu
			auto this_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 4));
			this->runAction(this_Func1);
			auto this_Func2 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 0));
			this->runAction(this_Func2);
		}
		else
		{
			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
		}
	}
}

//Upgrade HP & HPUP
void MainScene::UpHPAndHPUP(Ref* pSender, int value)
{
	if (value == 0)
	{
		if (getintData("Stat_Char_HP", 0) != 5 && getintData("money", 0) >= ((getintData("Stat_Char_HP", 0) + 1) * 5000))
		{
			setintData("money", getintData("money", 0) - ((getintData("Stat_Char_HP", 0) + 1) * 5000));
			setintData("Stat_Char_HP", (getintData("Stat_Char_HP", 0) + 1));

			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/buy.ogg");

			//Refresh Menu
			auto this_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 4));
			this->runAction(this_Func1);
			auto this_Func2 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 1));
			this->runAction(this_Func2);
		}
		else
		{
			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
		}
	}
	else if (value == 1)
	{
		if (getintData("Stat_Char_HPUP", 0) != 5 && getintData("money", 0) >= ((getintData("Stat_Char_HPUP", 0) + 1) * 5000))
		{
			setintData("money", getintData("money", 0) - ((getintData("Stat_Char_HPUP", 0) + 1) * 5000));
			setintData("Stat_Char_HPUP", (getintData("Stat_Char_HPUP", 0) + 1));

			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/buy.ogg");

			//Refresh Menu
			auto this_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 4));
			this->runAction(this_Func1);
			auto this_Func2 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 1));
			this->runAction(this_Func2);
		}
		else
		{
			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
		}
	}
}

//Upgrade MP & MPUP
void MainScene::UpMPAndMPUP(Ref* pSender, int value)
{
	if (value == 0)
	{
		if (getintData("Stat_Char_MP", 0) != 5 && getintData("money", 0) >= ((getintData("Stat_Char_MP", 0) + 1) * 5000))
		{
			setintData("money", getintData("money", 0) - ((getintData("Stat_Char_MP", 0) + 1) * 5000));
			setintData("Stat_Char_MP", (getintData("Stat_Char_MP", 0) + 1));

			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/buy.ogg");

			//Refresh Menu
			auto this_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 4));
			this->runAction(this_Func1);
			auto this_Func2 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 2));
			this->runAction(this_Func2);
		}
		else
		{
			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
		}
	}
	else if (value == 1)
	{
		if (getintData("Stat_Char_MPUP", 0) != 5 && getintData("money", 0) >= ((getintData("Stat_Char_MPUP", 0) + 1) * 5000))
		{
			setintData("money", getintData("money", 0) - ((getintData("Stat_Char_MPUP", 0) + 1) * 5000));
			setintData("Stat_Char_MPUP", (getintData("Stat_Char_MPUP", 0) + 1));

			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/buy.ogg");

			//Refresh Menu
			auto this_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 4));
			this->runAction(this_Func1);
			auto this_Func2 = CallFunc::create(CC_CALLBACK_0(MainScene::StatusUp, this, this, 2));
			this->runAction(this_Func2);
		}
		else
		{
			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
		}
	}
}


/*
	Game
*/
// Main Game Seletc Menu
void MainScene::Button_Game(Ref* pSender)
{
	if (TouchEnable == 0 && GameState != 3 && SubMenuState == 0)
	{
		GameState = 3;
		sound_click();
		Menu_Change("img/MainMenu_game.png");
		auto MainBoard = (Sprite*)this->getChildByName("MainBoard");

		/*
		  Touchmani
		*/
		//Borad image
		auto spr_SubBoard = Sprite::create("img/MainSubBoard1.png");
		spr_SubBoard->setAnchorPoint(Point(0, 1));
		spr_SubBoard->setPosition(Point(75, 237));
		spr_SubBoard->setOpacity(220);
		MainBoard->addChild(spr_SubBoard, 5);

		//Buttons
		auto item1 = MenuItemImage::create("img/SubMenu1.png", "img/SubMenu1.png", CC_CALLBACK_1(MainScene::Button_Game1, this, 0));
		auto menu = Menu::create(item1, NULL);
		item1->setPosition(Point(-15, 57));
		menu->setOpacity(220);
		MainBoard->addChild(menu, 5);

		//Game1 Score
		auto label_Game1_Score = Label::createWithTTF(StringUtils::format("%d 점", getintData("Game1_HighScore", 0)), "fonts/Navi.ttf", 16);
		label_Game1_Score->setAnchorPoint(Point(0, 1));
		label_Game1_Score->setPosition(Point(240, 180)); label_Game1_Score->setColor(Color3B(-255, -255, -255));
		MainBoard->addChild(label_Game1_Score, 5);

		/*
		 Catchmani
		*/
		//Borad image
		auto spr_SubBoard2 = Sprite::create("img/MainSubBoard2.png");
		spr_SubBoard2->setAnchorPoint(Point(0, 1));
		spr_SubBoard2->setPosition(Point(75, 137));
		spr_SubBoard2->setOpacity(220);
		MainBoard->addChild(spr_SubBoard2, 5);

		//Buttons
		auto item2 = MenuItemImage::create("img/SubMenu2.png", "img/SubMenu2.png", CC_CALLBACK_1(MainScene::Button_Game2_List, this));
		auto item3 = MenuItemImage::create("img/Box_Skill_Special.png", "img/Box_Skill_Special.png", CC_CALLBACK_1(MainScene::Special_Skill, this));
		auto menu2 = Menu::create(item2, item3, NULL);
		item2->setPosition(Point(-15, -72));
		item3->setPosition(Point(-120, -72)); item3->setScale(1.2);
		menu2->setOpacity(220);
		MainBoard->addChild(menu2, 5);
	}
}

// Game1 Info Menu with Start
void MainScene::Button_Game1(Ref* pSenderm, int value)
{
	if (TouchEnable == 0 && SubMenuState == 0 && value == 0)
	{
		sound_click();
		SubMenuState = 1;

		//Board & Buttons
		auto item1 = MenuItemImage::create("img/Game1_Menu.png", "img/Game1_Menu.png", CC_CALLBACK_1(MainScene::Button_Game1, this, 100));
		auto item2 = MenuItemImage::create("img/Button_GameStart.png", "img/Button_GameStart.png", CC_CALLBACK_1(MainScene::Button_Game1, this, 1));
		auto item3 = MenuItemImage::create("img/Button_close.png", "img/Button_close.png", CC_CALLBACK_1(MainScene::Button_Game1, this, 2));
		auto item4 = MenuItemImage::create("img/Icon_ShopItem_Heal.png", "img/Icon_ShopItem_Heal.png", CC_CALLBACK_1(MainScene::Button_Game1, this, 3));
		auto menu = Menu::create(item1, item2, item3, item4, NULL);
		item2->setPosition(Point(0, -92));
		item3->setPosition(Point(135, 105));
		item4->setPosition(Point(-84, -29));
		item4->setName("Potison");
		menu->setOpacity(235);
		this->addChild(menu, 5, "SubMenu");

		//Game1 image
		auto Game1_Image = Sprite::create("img/img_Game1.jpg");
		Game1_Image->setAnchorPoint(Point(0, 1));
		Game1_Image->setPosition(Point(50, 235));
		Game1_Image->setScale(0.17);
		item1->addChild(Game1_Image, 5);

		//Label1
		auto label_mess = Label::createWithTTF("게임 설명 : 현실에서 \n이탈한 가짜 승마니가 \n승마니에게 달려듭니다! \n터치로 물리쳐 주세요!", "fonts/Navi.ttf", 15);
		label_mess->setAnchorPoint(Point(0, 1));
		label_mess->setPosition(Point(150, 235)); label_mess->setColor(Color3B(165, 102, 255));
		item1->addChild(label_mess, 5);

		//Label2
		auto label_mess2 = Label::createWithTTF("<아이템>", "fonts/Navi.ttf", 15);
		label_mess2->setAnchorPoint(Point(0, 1));
		label_mess2->setPosition(Point(50, 158)); label_mess2->setColor(Color3B(-255, -255, -255));
		item1->addChild(label_mess2, 5);

		//Label3
		auto label_mess3 = Label::createWithTTF("체력과 마력을 모두 회복 \n시켜줍니다. \n(1개만 소지 가능)", "fonts/Navi.ttf", 14);
		label_mess3->setAnchorPoint(Point(0, 1));
		label_mess3->setPosition(Point(130, 134));
		item1->addChild(label_mess3, 5);  label_mess3->setColor(Color3B(243, 97, 166));

		if (getintData("Potison", 0) == 1)
		item4->setColor(Color3B(189, 189, 189));
	}

	//게임의 시작
	else if (TouchEnable == 0 && SubMenuState == 1 && value == 1)
	{
		SubMenuState = 0;

		auto scene = TransitionFade::create(1.5, Touchmani::createScene());
		Director::getInstance()->replaceScene(scene);
	}

	//Close SubMenu
	else if (TouchEnable == 0 && SubMenuState == 1 && value == 2)
	{
		sound_click();
		SubMenuState = 0;
		auto SubMenu = (Sprite*)this->getChildByName("SubMenu");
		this->removeChild(SubMenu);
	}

	//Buy Heal Item
	else if (TouchEnable == 0 && SubMenuState == 1 && value == 3 && getintData("Potison", 0) == 0)
	{
		if (getintData("money", 0) >= 300)
		{
			//Lose Money
			setintData("money", getintData("money", 0) - 300);

			//Get Potison
			setintData("Potison", 1);

			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/buy.ogg");

			//Change Item Image
			auto menu = (Menu*)this->getChildByName("SubMenu");
			auto Item1 = (MenuItemImage*)menu->getChildByName("Potison");
			Item1->setColor(Color3B(189, 189, 189));
		}
		else
		{
			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
		}
	}
}

// Game2 Stage List Menu
void MainScene::Button_Game2_List(Ref* pSender)
{
	if (TouchEnable == 0 && GameState != 31 && SubMenuState == 0)
	{
		GameState = 31;
		sound_click();
		Menu_Change("img/MainMenu_game.png");
		auto MainBoard = (Sprite*)this->getChildByName("MainBoard");

		// 이전 게임메뉴로 돌아가기
		auto NextShop = MenuItemImage::create("img/Icon_left.png", "img/Icon_left.png", CC_CALLBACK_1(MainScene::Button_Game, this));
		auto NextMenu = Menu::create(NextShop, NULL);
		NextShop->setPosition(-201, -23);
		NextShop->setScale(0.9);
		MainBoard->addChild(NextMenu, 6);

		// Make Menu Background Sprites
		auto Spr_sc1 = Sprite::create("img/Spr_sc3.png");
		Spr_sc1->setAnchorPoint(Point(0, 1));
		Spr_sc1->setPosition(Point(270, 161));
		Spr_sc1->setScale(0.4);
		MainBoard->addChild(Spr_sc1, 5);

		auto Spr_sc2 = Sprite::create("img/Spr_sc2.png");
		Spr_sc2->setAnchorPoint(Point(0, 1));
		Spr_sc2->setPosition(Point(220, 105));
		Spr_sc2->setScale(1.2);
		MainBoard->addChild(Spr_sc2, 5);

		auto Spr_sc3 = Sprite::create("img/Spr_sc1.png");
		Spr_sc3->setAnchorPoint(Point(0, 1));
		Spr_sc3->setPosition(Point(57, 76));
		Spr_sc3->setScale(1.2);
		MainBoard->addChild(Spr_sc3, 5);

		/*
		Game Stage List
		*/
		for (int i = 0; i < 15; i++)
		{
			auto item1 = MenuItemImage::create("img/Shop_ItemBox1.png", "img/Shop_ItemBox1.png", CC_CALLBACK_1(MainScene::Button_Game2, this, i + 1));

			// Locked Stage
			if (getintData((char*)StringUtils::format("Game2_Stage%d", i + 1).c_str(), -1) == -1 && i != 0)
			{
				item1 = MenuItemImage::create("img/Shop_ItemBox1_Locked.png", "img/Shop_ItemBox1_Locked.png", CC_CALLBACK_1(MainScene::Button_Game2, this, 100));
				item1->setColor(Color3B(189, 189, 189));
			}

			// Unlocked Stage
			else
			{
				auto Label_StageN = Label::createWithTTF(StringUtils::format("%d", i + 1), "fonts/Navi.ttf", 30);
				Label_StageN->setAnchorPoint(Point(0, 1));
				Label_StageN->setPosition(Point(76 + (i % 5) * 70 - (i / 9) * 5, 227 - 67 * (i / 5))); Label_StageN->setColor(Color3B(-255, -255, -255));
				MainBoard->addChild(Label_StageN, 6);

				for (int k = 0; k < 3; k++)
				{
					auto Star_Image = Sprite::create("img/Icon_star.png");
					Star_Image->setAnchorPoint(Point(0, 1));
					Star_Image->setPosition(Point(62 + (i % 5) * 70 + k * 15, 195 - 67 * (i / 5)));
					Star_Image->setScale(0.55);
					MainBoard->addChild(Star_Image, 6);

					if (getintData((char*)StringUtils::format("Game2_Stage%d", i + 1).c_str(), 0) - 1 < k)
						Star_Image->setTexture("img/Icon_star_inactive.png");
				}
			}

			auto menu = Menu::create(item1, NULL);
			menu->setOpacity(225);
			menu->setPosition(Point(85 + (i % 5) * 70, 200 - 67 * (i / 5)));
			MainBoard->addChild(menu, 5);
		}
	}
}

// Select Game2 Stage - Start Game
void MainScene::Button_Game2(Ref* pSender, int value)
{
	// Touch Locked Stage
	if (value == 100)
	{
		//Sound
		if (getintData("sound", 0) == 0)
			float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
	}

	// Touch Unlocked Stage
	else
	{
		sound_click();
		setintData("Game2NowStage", value);
		auto scene = TransitionFade::create(1.5, Catchmani::createScene());
		Director::getInstance()->replaceScene(scene);
	}
}

// Game2 Special Skill
void MainScene::Special_Skill(Ref* pSender)
{
	if (TouchEnable == 0 && GameState != 39 && SubMenuState == 0)
	{
		GameState = 39;
		Menu_Change("img/MainMenu_game.png");
		auto MainBoard = (Sprite*)this->getChildByName("MainBoard");
		Skill_Info(this, 1); // 초기 Info 메뉴

		// 이전 게임메뉴로 돌아가기
		auto NextShop = MenuItemImage::create("img/Icon_left.png", "img/Icon_left.png", CC_CALLBACK_1(MainScene::Button_Game, this));
		auto NextMenu = Menu::create(NextShop, NULL);
		NextShop->setPosition(-201, -23);
		NextShop->setScale(0.9);
		MainBoard->addChild(NextMenu, 6);

		//ItemBox image
		for (int i = 0; i < 6; i++)
		{
			auto spr_ItemBox = Sprite::create("img/Shop_ItemBox1.png");
			spr_ItemBox->setAnchorPoint(Point(0, 1));
			spr_ItemBox->setPosition(Point(265 + (i % 2) * 70, 235 - 67 * (i / 2)));
			MainBoard->addChild(spr_ItemBox, 5);

			auto Skill_Icon = MenuItemImage::create(StringUtils::format("img/Skill_%d.png", i + 1), StringUtils::format("img/Skill_%d.png", i + 1), CC_CALLBACK_1(MainScene::Skill_Info, this, i + 1));
			auto menu = Menu::create(Skill_Icon, NULL);
			menu->setOpacity(225);
			Skill_Icon->setPosition(52 + (i % 2) * 70, 42 - 67 * (i / 2));
			MainBoard->addChild(menu, 5);

			if (getintData("NowSpecialSkill", 0) == i + 1)
				spr_ItemBox->setColor(Color3B(255, 167, 167));
		}
	}
}

// Make Skill Info
void MainScene::Skill_Info(Ref* pSender, int Value)
{
	sound_click();

	auto MainBoard = (Sprite*)this->getChildByName("MainBoard");
	auto Skill_Info = (Sprite*)MainBoard->getChildByName("Skill_Info");

	if (Skill_Info)
		MainBoard->removeChild(Skill_Info, true);

	//Borad image
	auto spr_SubBoard2 = Sprite::create("img/Skill_InfoMenu.png");
	spr_SubBoard2->setAnchorPoint(Point(0, 1));
	spr_SubBoard2->setPosition(Point(55, 255));
	spr_SubBoard2->setOpacity(220);
	MainBoard->addChild(spr_SubBoard2, 5, "Skill_Info");

	//Char image
	auto spr_Skill = Sprite::create(StringUtils::format("img/Skill_%d.png", Value));
	spr_Skill->setAnchorPoint(Point(0, 1));
	spr_Skill->setPosition(Point(15, 208));
	spr_SubBoard2->addChild(spr_Skill);

    char *Skill_Name, *Skill_Infos, *Skill_GetInfo;
	if (Value == 1)
	{
		Skill_Name = "넉백";
		Skill_Infos = "보스를 포함한 적들을 \n밀어버린다.";
		Skill_GetInfo = "없음";
	}
	else if (Value == 2)
	{
		Skill_Name = "모방";
		Skill_Infos = "캐릭터 고유 스킬을 \n모방하여 사용한다.";
		Skill_GetInfo = "없음";
	}
	else if (Value == 3)
	{
		Skill_Name = "스틸";
		Skill_Infos = "떨어진 아이템들을 모두 \n줍는다.";
		Skill_GetInfo = "스테이지 5 클리어";
	}
	else if (Value == 4)
	{
		Skill_Name = "차원의 벽";
		Skill_Infos = "보스를 포함한 적들이 \n이동하지 못하는 벽을 \n생성한다.";
		Skill_GetInfo = "터치마니 150만점 \n달성";
	}
	else if (Value == 5)
	{
		Skill_Name = "시공간 균열";
		Skill_Infos = "시공간에 균열을 일으켜 \n잠깐동안 시간을 멈춘다.";
		Skill_GetInfo = "???";
	}
	else if (Value == 6)
	{
		spr_Skill->setPosition(Point(20, 208));
		Skill_Name = "해킹";
		Skill_Infos = "보스를 제외한 적들에게 \n에러를 발생시킨다.";
		Skill_GetInfo = "???";
	}

	//Label Skill Name
	auto Label_Skill_Name = Label::createWithTTF(StringUtils::format("%s", Skill_Name), "fonts/Navi.ttf", 14);
	Label_Skill_Name->setAnchorPoint(Point(0, 1));
	Label_Skill_Name->setPosition(Point(90, 207)); Label_Skill_Name->setColor(Color3B(-255, -255, -255));
	spr_SubBoard2->addChild(Label_Skill_Name, 5);

	//Label Skill Info
	auto Label_Skill_Info = Label::createWithTTF(StringUtils::format("%s", Skill_Infos), "fonts/Navi.ttf", 14);
	Label_Skill_Info->setAnchorPoint(Point(0, 1));
	Label_Skill_Info->setPosition(Point(65, 161)); Label_Skill_Info->setColor(Color3B(-255, -255, -255));
	spr_SubBoard2->addChild(Label_Skill_Info, 5);

	//Label Get Info
	auto Label_Get_Info = Label::createWithTTF(StringUtils::format("%s", Skill_GetInfo), "fonts/Navi.ttf", 14);
	Label_Get_Info->setAnchorPoint(Point(0, 1));
	Label_Get_Info->setPosition(Point(90, 98)); Label_Get_Info->setColor(Color3B(-255, -255, -255));
	spr_SubBoard2->addChild(Label_Get_Info, 5);

	auto Select_button = MenuItemImage::create("img/Button_Select.png", "img/Button_Select.png", CC_CALLBACK_1(MainScene::Select_Skill, this, Value));
	if (getintData((char*)StringUtils::format("Special_Skill%d", Value).c_str(), 0) == 0)
	{
	  Select_button = MenuItemImage::create("img/Button_Select.png", "img/Button_Select.png", CC_CALLBACK_1(MainScene::Select_Skill, this, 0));
	  Select_button->setColor(Color3B(189, 189, 189));
	}
	auto Select_menu = Menu::create(Select_button, NULL);
	Select_menu->setOpacity(225);
	Select_button->setPosition(-140, -120);
	spr_SubBoard2->addChild(Select_menu, 5);
}

// Select Skill
void MainScene::Select_Skill(Ref* pSender, int Value)
{
	if (Value != 0)
	{
		setintData("NowSpecialSkill", Value);
		//Sound
		if (getintData("sound", 0) == 0)
			float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/save.ogg");

		//Refresh Skill Menu
		GameState = 0;
		auto this_Func2 = CallFunc::create(CC_CALLBACK_0(MainScene::Special_Skill, this, this));
		this->runAction(this_Func2);
	}
	else
	{
		//Sound
		if (getintData("sound", 0) == 0)
			float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
	}
}



/*
	 Shop
*/
// Char Shop
void MainScene::Button_Shop(Ref* pSender)
{
	if (TouchEnable == 0 && GameState != 4 && SubMenuState == 0)
	{
		GameState = 4;
		sound_click();
		Menu_Change("img/MainMenu_shop.png");
		auto MainBoard = (Sprite*)this->getChildByName("MainBoard");

		//상점 아이템들의 메뉴처리
		auto item1 = MenuItemImage::create(Get_CharImage(0), Get_CharImage(0), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char1_Normal", 0, "승마니", "그냥 승마니", 1, 1, 1, 1, 1, 0, "없음", "많이 맞아"));
		auto item2 = MenuItemImage::create(Get_CharImage(1), Get_CharImage(1), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char1_Foramt2", 1, "승학생", "공부만 하는 전교 1등 승마니", 1, 1, 3, 1, 1, 5000, "없음", "많이 맞아"));
		auto item3 = MenuItemImage::create(Get_CharImage(2), Get_CharImage(2), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char1_Foramt1", 2, "승허렁", "승마니의 친구. 큰 키로 승부한다!", 3, 1, 1, 1, 1, 10000, "없음", "많이 맞아"));
		auto item4 = MenuItemImage::create(Get_CharImage(3), Get_CharImage(3), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char1_Foramt3", 3, "승주영", "승마니의 친구. 축구선수 이름값 \n만큼 몸으로 승부한다!", 1, 3, 2, 1, 1, 10000, "없음", "많이 맞아"));
		auto item5 = MenuItemImage::create(Get_CharImage(4), Get_CharImage(4), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char2_Foramt1", 4, "승잔나", "승마니의 친구. 바람을 다스릴 줄 \n안다는데..!", 1, 1, 1, 2, 2, 20000, "없음", "바람아 불어라"));
		auto item6 = MenuItemImage::create(Get_CharImage(5), Get_CharImage(5), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char1_Foramt4", 5, "승도남", "까칠한 도시의 승마니!", 2, 2, 2, 2, 2, 25000, "공격하면 적을 더 많이 밀어요", "저리 비켜"));
		auto item7 = MenuItemImage::create(Get_CharImage(6), Get_CharImage(6), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char2_Foramt2", 6, "승소녀", "승마니의 친구. \n태권도를 배워서 약하지 않아!", 2, 3, 3, 2, 2, 50000, "공격시 일정 확률로 적을 날려요", "540도 뒤후려차기"));
		auto item8 = MenuItemImage::create(Get_CharImage(7), Get_CharImage(7), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char1_Foramt9", 7, "승법사", "마법사 승마니", 1, 2, 1, 4, 4, 75000, "공격시 일정 확률로 마법 공격을 해요", "매직 빔"));
		auto item9 = MenuItemImage::create(Get_CharImage(8), Get_CharImage(8), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char1_Foramt6", 8, "승혈귀", "흡혈귀인지 사람인지..", 3, 3, 3, 2, 2, 120000, "공격할때 마다 체력을 회복해요", "피좀 주라"));
		auto item10 = MenuItemImage::create(Get_CharImage(9), Get_CharImage(9), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char1_Foramt5", 9, "승수르", "돈 복이 많은 승마니!", 3, 3, 3, 3, 3, 170000, "돈 획득량이 1.5배 증가해요", "돈은 아픈거야"));
		auto item11 = MenuItemImage::create(Get_CharImage(10), Get_CharImage(10), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char2_Foramt3", 10, "승아리", "승마니의 이상형. 구미호? 사람? ", 3, 3, 3, 4, 4, 210000, "공격시 적을 유혹해서 잠깐 느리게 해요", "나의 사랑을 받아랏"));
		auto item12 = MenuItemImage::create(Get_CharImage(11), Get_CharImage(11), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char1_Foramt8", 11, "승의사", "의사가 된 승마니", 3, 3, 3, 4, 4, 240000, "공격 할때마다 기력을 회복해요", "치료"));
		auto item13 = MenuItemImage::create(Get_CharImage(12), Get_CharImage(12), CC_CALLBACK_0(MainScene::CharInfo, this, this,
			"char1_Foramt7", 12, "승의탑", "일명 바다잡이(?) 라고 \n불리는 강력한 승마니", 5, 3, 3, 4, 4, 320000, "공격시 적을 멈추게 해요 (중첩시 시간 감소)", "신수 흐름 제어"));

		//옆 : 70 아래 : 67 차이
		auto menu = Menu::create(item1, item2, item3, item4, item5, item6, item7, item8, item9, item10, item11, item12, item13, NULL);
		item1->setPosition(Point(-158, 42)); item1->setScale(0.45);
		item2->setPosition(Point(-88, 42));  item2->setScale(0.45);
		item3->setPosition(Point(-18, 42));  item3->setScale(0.45);
		item4->setPosition(Point(52, 42));  item4->setScale(0.45);
		item5->setPosition(Point(122, 42));  item5->setScale(0.45);
		item6->setPosition(Point(-158, -25));  item6->setScale(0.45); 
		item7->setPosition(Point(-88, -25));  item7->setScale(0.45);
		item8->setPosition(Point(-18, -25));  item8->setScale(0.45);
		item9->setPosition(Point(52, -25));  item9->setScale(0.45);
		item10->setPosition(Point(122, -25));  item10->setScale(0.45);
		item11->setPosition(Point(-158, -92));  item11->setScale(0.45);
		item12->setPosition(Point(-88, -92));  item12->setScale(0.45);
		item13->setPosition(Point(-18, -92));  item13->setScale(0.45);
		MainBoard->addChild(menu, 6);

		//ItemBox Image
		for (int i = 0; i < 15; i++)
		{
			//ItemBox image
			auto spr_ItemBox = Sprite::create("img/Shop_ItemBox1.png");
			spr_ItemBox->setAnchorPoint(Point(0, 1));
			spr_ItemBox->setPosition(Point(55 + (i % 5) * 70, 235 - 67 * (i / 5)));
			MainBoard->addChild(spr_ItemBox, 5);

			if (i >= 6 && i <= 9)
				spr_ItemBox->setTexture("img/Shop_ItemBox2.png");
			else if (i >= 10 && i<13)
				spr_ItemBox->setTexture("img/Shop_ItemBox3.png");
			else if (i>= 13)
			{
				spr_ItemBox->setTexture("img/Shop_ItemBox3_hidden.png");
				spr_ItemBox->setColor(Color3B(189, 189, 189));
			}

			if (getintData("MainChar", 0) == i)
				spr_ItemBox->setColor(Color3B(255, 167, 167));
		}

		auto NextShop = MenuItemImage::create("img/Icon_right.png", "img/Icon_right.png", CC_CALLBACK_1(MainScene::Pet_shop, this));
		auto NextMenu = Menu::create(NextShop, NULL);
		NextShop->setPosition(170, -23);
		NextShop->setScale(0.9);
		MainBoard->addChild(NextMenu, 6);
	}
}

// Pet Shop
void MainScene::Pet_shop(Ref* pSender)
{
	GameState = 41;
	sound_click();
	Menu_Change("img/MainMenu_shop.png");
	auto MainBoard = (Sprite*)this->getChildByName("MainBoard");

	// 이전 캐릭터 상점으로 돌아가기
	auto NextShop = MenuItemImage::create("img/Icon_left.png", "img/Icon_left.png", CC_CALLBACK_1(MainScene::Button_Shop, this));
	auto NextMenu = Menu::create(NextShop, NULL);
	NextShop->setPosition(-201, -23);
	NextShop->setScale(0.9);
	MainBoard->addChild(NextMenu, 6);

	//상점 아이템들의 메뉴처리
	auto item1 = MenuItemImage::create(Get_CharImage(100), Get_CharImage(100), CC_CALLBACK_0(MainScene::CharInfo, this, this,
		"Pet_dog1", 100, "꽁냥이", "강아지인지 고양이인지..\n(능력치는 주인공에게 추가 적용)", 1, 1, 1, 1, 1, 20000, "없음", "없음"));
	auto item2 = MenuItemImage::create(Get_CharImage(101), Get_CharImage(101), CC_CALLBACK_0(MainScene::CharInfo, this, this,
		"Pet_cat1", 101, "길냥이", "길거리에서 흔히 보이는 냥이\n(능력치는 주인공에게 추가 적용)", 1, 1, 1, 1, 1, 20000, "없음", "없음"));
	auto item3 = MenuItemImage::create(Get_CharImage(102), Get_CharImage(102), CC_CALLBACK_0(MainScene::CharInfo, this, this,
		"Pet_cat2", 102, "도둑냥", "훔쳐먹는것을 좋아하는 냥이\n(능력치는 주인공에게 추가 적용)", 1, 1, 1, 1, 1, 50000, "돈의 일부를 자동으로 모아줘요", "없음"));
	auto item4 = MenuItemImage::create(Get_CharImage(103), Get_CharImage(103), CC_CALLBACK_0(MainScene::CharInfo, this, this,
		"Pet_cat3", 103, "페르냥", "생긴것과는 다르게 예민한 냥이\n(능력치는 주인공에게 추가 적용) ", 2, 1, 1, 1, 1, 60000, "지속적으로 모든 적을 때려요", "없음"));
	auto item5 = MenuItemImage::create(Get_CharImage(104), Get_CharImage(104), CC_CALLBACK_0(MainScene::CharInfo, this, this,
		"Pet_cat4", 104, "백호당", "무시무시한(?) 백호!\n(능력치는 주인공에게 추가 적용)", 2, 1, 1, 1, 1, 80000, "지속적으로 모든 적을 날려요", "없음"));

	//옆 : 70 아래 : 67 차이
	auto menu = Menu::create(item1, item2, item3, item4, item5, NULL);
	item1->setPosition(Point(-158, 42)); item1->setScale(0.75);
	item2->setPosition(Point(-88, 42));  item2->setScale(0.75);
	item3->setPosition(Point(-18, 42));  item3->setScale(0.75);
	item4->setPosition(Point(52, 42));  item4->setScale(0.75);
	item5->setPosition(Point(122, 42));  item5->setScale(0.75);
	MainBoard->addChild(menu, 6);

	//ItemBox Image
	for (int i = 0; i < 10; i++)
	{
		//ItemBox image
		auto spr_ItemBox = Sprite::create("img/Shop_ItemBox1.png");
		spr_ItemBox->setAnchorPoint(Point(0, 1));
		spr_ItemBox->setPosition(Point(55 + (i % 5) * 70, 235 - 67 * (i / 5)));
		MainBoard->addChild(spr_ItemBox, 5);

		if (i >= 2 && i <= 3)
			spr_ItemBox->setTexture("img/Shop_ItemBox2.png");
		else if (i >= 4 && i<5)
			spr_ItemBox->setTexture("img/Shop_ItemBox3.png");
		else if (i >= 5)
		{
			spr_ItemBox->setTexture("img/Shop_ItemBox3_hidden.png");
			spr_ItemBox->setColor(Color3B(189, 189, 189));
		}

		if (getintData("MainPet", 0) == i+100)
			spr_ItemBox->setColor(Color3B(255, 167, 167));
	}
}

//Pop Up Char, Pet Info Menu
void MainScene::CharInfo(Ref* pSender, const char* CharData, int MainChar, const char* CharName, const char* CharInfo, int Power, int HP, int HPUP, int MP, int MPUP, int cost, const char* Specify, const char* Skill)
{
	if (TouchEnable == 0 && SubMenuState == 0)
	{
		sound_click();
		SubMenuState = 1;

		//Already Buy?
		char* Spr_Name;
		if (getintData((char*)CharData, 0) == 0)
			Spr_Name = "img/Button_Buy.png";
		else
			Spr_Name = "img/Button_Select.png";

		//Board & Buttons
		auto item1 = MenuItemImage::create("img/Char_InfoMenu.png", "img/Char_InfoMenu.png", CC_CALLBACK_1(MainScene::CharMenus, this, 100, "", 0, 0));
		auto item2 = MenuItemImage::create(Spr_Name, Spr_Name, CC_CALLBACK_1(MainScene::CharMenus, this, 0, CharData, MainChar, cost));
		auto item3 = MenuItemImage::create("img/Button_close.png", "img/Button_close.png", CC_CALLBACK_1(MainScene::CharMenus, this, 1, "", 0, 0));

		if (GameState == 41)
			item2 = MenuItemImage::create(Spr_Name, Spr_Name, CC_CALLBACK_1(MainScene::CharMenus, this, 3, CharData, MainChar, cost));

		auto menu = Menu::create(item1, item2, item3, NULL);
		item1->setName("BoardBody");
		item2->setPosition(Point(10, -95)); item2->setScale(0.8); item2->setName("BuyChar");
		item3->setPosition(Point(135, 105));
		this->addChild(menu, 6, "SubMenu");

		//Char image
		auto spr_Char = Sprite::create(Get_CharImage(MainChar));
		spr_Char->setAnchorPoint(Point(0, 1));
		spr_Char->setPosition(Point(35, 245));
		spr_Char->setOpacity(220); 
		if (GameState != 41)
		   spr_Char->setScale(0.5);
		else
		   spr_Char->setScale(0.8);
		item1->addChild(spr_Char, 5);

		//Label Char Name
		auto Label_Char_Name = Label::createWithTTF(StringUtils::format("%s", CharName), "fonts/Navi.ttf", 14);
		Label_Char_Name->setAnchorPoint(Point(0, 1));
		Label_Char_Name->setPosition(Point(145, 257)); Label_Char_Name->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Char_Name, 5);

		//Label Char Info
		auto Label_Char_Info = Label::createWithTTF(StringUtils::format("%s", CharInfo), "fonts/Navi.ttf", 14);
		Label_Char_Info->setAnchorPoint(Point(0, 1));
		Label_Char_Info->setPosition(Point(105, 237)); Label_Char_Info->setColor(Color3B(255, 115, 184));
		item1->addChild(Label_Char_Info, 5);

		//Char Power - Star image
		for (int i = 0; i < Power; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(135 + 10 * i, 187));
			Spr_Star->setOpacity(220); Spr_Star->setScale(0.5);
			item1->addChild(Spr_Star, 5);
		}

		//Char HP - Star image
		for (int i = 0; i < HP; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(120 + 10 * i, 161));
			Spr_Star->setOpacity(220); Spr_Star->setScale(0.5);
			item1->addChild(Spr_Star, 5);
		}

		//Char HPUP - Star image
		for (int i = 0; i < HPUP; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(240 + 10 * i, 161));
			Spr_Star->setOpacity(220); Spr_Star->setScale(0.5);
			item1->addChild(Spr_Star, 5);
		}

		//Char MP - Star image
		for (int i = 0; i < MP; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(120 + 10 * i, 134));
			Spr_Star->setOpacity(220); Spr_Star->setScale(0.5);
			item1->addChild(Spr_Star, 5);
		}

		//Char MPUP - Star image
		for (int i = 0; i < MPUP; i++)
		{
			auto Spr_Star = Sprite::create("img/Icon_star.png");
			Spr_Star->setAnchorPoint(Point(0, 1));
			Spr_Star->setPosition(Point(240 + 10 * i, 134));
			Spr_Star->setOpacity(220); Spr_Star->setScale(0.5);
			item1->addChild(Spr_Star, 5);
		}

		//Label Char Specify Info
		auto Label_Char_Specify = Label::createWithTTF(StringUtils::format("%s", Specify), "fonts/Navi.ttf", 12);
		Label_Char_Specify->setAnchorPoint(Point(0, 1));
		Label_Char_Specify->setPosition(Point(80, 103)); Label_Char_Specify->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Char_Specify, 5);

		//Label Char Skill Info
		auto Label_Char_Skill = Label::createWithTTF(StringUtils::format("%s", Skill), "fonts/Navi.ttf", 12);
		Label_Char_Skill->setAnchorPoint(Point(0, 1));
		Label_Char_Skill->setPosition(Point(80, 80)); Label_Char_Skill->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Char_Skill, 5);

		//Label Char Value
		auto Label_Char_cost = Label::createWithTTF(StringUtils::format("%d", cost), "fonts/Navi.ttf", 14);
		Label_Char_cost->setAnchorPoint(Point(0, 1));
		Label_Char_cost->setPosition(Point(70, 54)); Label_Char_cost->setColor(Color3B(-255, -255, -255));
		item1->addChild(Label_Char_cost, 5);
	}
}

//Buy Chars, Pet & Close Char, Pet Info Menu  value == 0 : As Char , value == 1 : Close Menu , value == 3 : As Pet 
void MainScene::CharMenus(Ref* pSender, int value, const char* CharData, int MainChar, int cost)
{
	if (TouchEnable == 0 && SubMenuState == 1 && value == 0 || value == 3)
	{
		if (getintData((char*)CharData, 0) == 0)
		{
			if (getintData("money", 0) >= cost)
			{
				setintData("money", getintData("money", 0) - cost);
				setintData((char*)CharData, 1);

				//Sound
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/buy.ogg");

				//PopUP Image
				auto SubMenu = (Menu*)this->getChildByName("SubMenu");
				auto Item1 = (MenuItemImage*)SubMenu->getChildByName("BoardBody");
				auto Item2 = (MenuItemImage*)SubMenu->getChildByName("BuyChar");
				Item2->setOpacity(0);

				auto Button_Buy = Sprite::create("img/Button_Select.png");
				Button_Buy->setAnchorPoint(Point(0, 1));
				Button_Buy->setPosition(Point(136, 60));
				Button_Buy->setOpacity(220); Button_Buy->setScale(0.8);
				Item1->addChild(Button_Buy, 6);
			}
			else
			{
				//Sound
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
			}
		}
		//Already Buy Char
		else
		{
			if (value == 0)
				setintData("MainChar", MainChar);
			else if (value == 3)
				setintData("MainPet", MainChar);

			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/save.ogg");

			//Close Menu
			auto this_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::CharMenus, this, this, 1, "", 0, 0));
			this->runAction(this_Func1);

			//Refresh Shop Menu
			if (GameState != 41)
			{
				GameState = 0;
				auto this_Func2 = CallFunc::create(CC_CALLBACK_0(MainScene::Button_Shop, this, this));
				this->runAction(this_Func2);
			}
			else
			{
				GameState = 0;
				auto this_Func2 = CallFunc::create(CC_CALLBACK_0(MainScene::Pet_shop, this, this));
				this->runAction(this_Func2);
			}
		}
	}

	else if (TouchEnable == 0 && SubMenuState == 1 && value == 1)
	{
		sound_click();
		SubMenuState = 0;
		auto SubMenu = (Sprite*)this->getChildByName("SubMenu");
		this->removeChild(SubMenu);
	}
}


/*
	Ranking
*/
void MainScene::Button_Rank(Ref* pSender)
{
	if (TouchEnable == 0 && GameState != 5 && SubMenuState == 0)
	{
		GameState = 5;
		sound_click();
		Menu_Change("img/MainMenu_rank.png");
		auto MainBoard = (Sprite*)this->getChildByName("MainBoard");

		//Scroll Down Button
		auto item1 = MenuItemImage::create("img/Icon_down.png", "img/Icon_down.png", CC_CALLBACK_1(MainScene::Rank_ScorollDown, this));
		auto menu = Menu::create(item1, NULL);
		item1->setPosition(Point(-15, -133));
		item1->setScale(0.8);
		menu->setOpacity(235);
		MainBoard->addChild(menu, 6);

		//Jni
		JavaFun(4);
	}
}

//랭킹 메뉴에서 스크롤을 제일 아래로 내려주는 버튼
void MainScene::Rank_ScorollDown(Ref* pSender)
{
	auto MainBoard = (Sprite*)this->getChildByName("MainBoard");
	auto Scroll = (ScrollView*)MainBoard->getChildByName("RankScroll");

	if (Scroll != NULL)
	{
		sound_click();
		Scroll->setContentOffset(Scroll->maxContainerOffset(), true);
	}
}


/*
	 Setting
*/
//Setting Menu
void MainScene::Button_Setting(Ref* pSender)
{
	if (TouchEnable == 0 && GameState != 6 && SubMenuState == 0)
	{
		GameState = 6;
		sound_click();
		Menu_Change("img/MainMenu_setting.png");
		auto MainBoard = (Sprite*)this->getChildByName("MainBoard");

		//Borad image
		auto spr_SubBoard = Sprite::create("img/MainSubBoard3.png");
		spr_SubBoard->setAnchorPoint(Point(0, 1));
		spr_SubBoard->setPosition(Point(63, 237));
		spr_SubBoard->setOpacity(220);
		MainBoard->addChild(spr_SubBoard, 5, "Setting_SubBoard");

		//Sound On/Off Button
		char* SoundIcon;
		if (getintData("sound", 0) == 0)
			SoundIcon = "img/Icon_SoundOn.png";
		else
			SoundIcon = "img/Icon_SoundOff.png";

		auto item1 = MenuItemImage::create(SoundIcon, SoundIcon, CC_CALLBACK_1(MainScene::Sound_OnOff, this));
		auto menu = Menu::create(item1, NULL);
		item1->setPosition(Point(-195, -132));
		spr_SubBoard->addChild(menu, 6, "Icon_Sound");

		//Label
		auto Label_Sound = Label::createWithTTF("사운드를 끄거나 켭니다.", "fonts/Navi.ttf", 15);
		Label_Sound->setAnchorPoint(Point(0, 1));
		Label_Sound->setPosition(Point(105, 38)); Label_Sound->setColor(Color3B(-255, -255, -255));
		spr_SubBoard->addChild(Label_Sound, 5);
	}
}

//사운드의 On / Off
void MainScene::Sound_OnOff(Ref* pSender)
{
	if (getintData("sound", 0) == 0)
	{
		setintData("sound", 1);
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		GameState = 0;
		
		//Setting Menu의 업데이트
		auto this_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::Button_Setting, this, this));
		this->runAction(this_Func1);
	}
	else
	{
		setintData("sound", 0);
		SimpleAudioEngine::getInstance()->playBackgroundMusic("sounds/Lovely Summer.mp3");
		GameState = 0;

		//Setting Menu의 업데이트
		auto this_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::Button_Setting, this, this));
		this->runAction(this_Func1);
	}

}

//게임 정보 버튼
void MainScene::Button_Info(Ref* pSender)
{
	if (TouchEnable == 0 && GameState != 7 && SubMenuState == 0)
	{
		GameState = 7;
		sound_click();
		Menu_Change("img/MainMenu_info.png");
		auto MainBoard = (Sprite*)this->getChildByName("MainBoard");

		//Label
		auto Label_Info = Label::createWithTTF("CopyRight : skydoves@naver.com  \n                   엄재웅(여림)", "fonts/Navi.ttf", 18);
		Label_Info->setAnchorPoint(Point(0, 1));
		Label_Info->setPosition(Point(100, 155)); Label_Info->setColor(Color3B(255, 255, 255));
		MainBoard->addChild(Label_Info, 5);

		//Maker Char image
		auto Char_Maker = Sprite::create("img/char_Maker.png");
		Char_Maker->setAnchorPoint(Point(0, 1));
		Char_Maker->setPosition(Point(205, 80));
		Char_Maker->setOpacity(220); Char_Maker->setScale(0.6);
		MainBoard->addChild(Char_Maker, 5);
	}
}

//메뉴가 바뀌는것의 처리
void MainScene::Menu_Change(char* filename)
{
	//Remove Before MainBoard
	auto MainBoard = (Sprite*)this->getChildByName("MainBoard");
	this->removeChild(MainBoard);

	//Make Main Menu Board
	auto spr_menu = Sprite::create(filename);
	spr_menu->setAnchorPoint(Point(0, 1));
	spr_menu->setOpacity(200); spr_menu->setScale(0.9);
	spr_menu->setPosition(Point(47, 290));
	this->addChild(spr_menu, 1, "MainBoard");
}



///<summary>
///  Game Systems
///</summary>
// Update : Rankling, Labels, etc.. 
void MainScene::Updates(float delta)
{
	//랭킹 정보를 받아 왔을 경우
	if (RankState == 1 && GameState == 5)
	{
		const char * Name;
		auto MainBoard = (Sprite*)this->getChildByName("MainBoard");

		//Reset Datas
		RankState = 0;
		RankCount = 0;

		//레이어 생성 , 레이어 크기 설정
		auto layer = LayerColor::create(Color4B(255, 255, 255, 255));
		layer->setContentSize(Size(350, 670));
		layer->setOpacity(0);

		//스크롤에 레이어 등록
		auto scroll = ScrollView::create(Size(350, 198), layer);
		scroll->setDirection(ScrollView::Direction::VERTICAL);
		scroll->setBounceable(true);
		scroll->setPosition(55, 42);
		MainBoard->addChild(scroll, 5, "RankScroll");

		//Set Scroll Offset
		scroll->setContentOffset(scroll->minContainerOffset());

		//Rank Back Image
		for (int i = 0; i < 10; i++)
		{
			auto item1 = Sprite::create("img/Rank.png");
			item1->setAnchorPoint(Point(0, 1));
			item1->setPosition(80, 670 - i*65);
			layer->addChild(item1);
		}

		//Name Label
		for (int i = 0; i < 10; i++)
		{
			if (RankData1[i][0] == 100 && RankData1[i][1] == 123)
				break;

			Name = RankData2[i].c_str();
			auto Label_Name = Label::createWithTTF(StringUtils::format("%s", Name), "fonts/Navi.ttf", 14);
			Label_Name->setAnchorPoint(Point(0, 1));
			Label_Name->setPosition(Point(135, 664 - i * 65)); Label_Name->setColor(Color3B(-255, -255, -255));
			layer->addChild(Label_Name);
		}

		//Score Label
		for (int i = 0; i < 10; i++)
		{
			if (RankData1[i][0] == 100 && RankData1[i][1] == 123)
				break;

			auto Label_Name = Label::createWithTTF(StringUtils::format("%d", RankData1[i][1]), "fonts/Navi.ttf", 21);
			Label_Name->setAnchorPoint(Point(0, 1));
			Label_Name->setPosition(Point(145, 645 - i * 65)); Label_Name->setColor(Color3B(-255, -255, -255));
			layer->addChild(Label_Name);
		}

		//Main Char Image
		for (int i = 0; i < 10; i++)
		{
			if (RankData1[i][0] == 100 && RankData1[i][1] == 123)
				break;

			auto item1 = Sprite::create();
			item1->setAnchorPoint(Point(0, 1));
			item1->setPosition(90, 660 - i * 65);
			item1->setScale(0.4);
			layer->addChild(item1);

			//Set Char Image
			item1->setTexture(Get_CharImage(RankData1[i][0]));
		}

		//Ranking Number Image
		for (int i = 0; i < 10; i++)
		{
			if (RankData1[i][0] == 100 && RankData1[i][1] == 123)
				break;

			auto item1 = Sprite::create(StringUtils::format("img/Rank_%d.png", i+1));
			item1->setAnchorPoint(Point(0, 1));
			item1->setPosition(15, 663 - i * 64 + (i/4));
			item1->setOpacity(210);
			layer->addChild(item1);
		}
	}

	//Update Money Label
	auto Label_Score = (Label*)this->getChildByName("money");
	Label_Score->setString(StringUtils::format("%d", getintData("money", 0)));

	//불법 프로그램 사용시
	if (getintData("Stat_Char_Poewr", 0) >= 6 || getintData("Stat_Char_Defense", 0) >= 6 || getintData("Stat_Char_HP", 0) >= 6 || getintData("Stat_Char_HPUP", 0) >= 6 || getintData("Stat_Char_MP", 0) >= 6 || getintData("Stat_Char_MPUP", 0) >= 6 || getintData("Potison", 0) >= 2 || getintData("asdfasd", 0) != 14215241 && getintData("asdfasd", 0) != 0)
	{
		setintData("asdfasd", 14215251);
		MessageBox("불법 프로그램 사용으로 인해 신고처리 접수되었습니다.", "<위험> 다음 메세지는 핸드폰의 위치와 일부 정보를 수집합니다.");
		Director::getInstance()->end();
	}

	int money = UserDefault::getInstance()->getIntegerForKey("money", 0);
	int money_de = UserDefault::getInstance()->getIntegerForKey("asdf4sdf2sdf", 0) - 51521252;

	//불법 프로그램 사용시
	if (money != money_de && getintData("as5f425f2sdf", 0) > 0)
	{
		setintData("asdfasd", 14215251);
		MessageBox("불법 프로그램 사용으로 인해 신고처리 접수되었습니다.", "<위험> 다음 메세지는 핸드폰의 위치와 일부 정보를 수집합니다.");
		Director::getInstance()->end();
	}
}

//Main Char 번호에 맞는 캐릭터 이미지를 반환한다
char* MainScene::Get_CharImage(int value)
{
	if (value == 0) // 노말 승마니
		return "img/char1_Normal.png";
	else if(value == 1) // 승고딩
		return "img/char1_Foramt2.png";
	else if (value == 2) // 승허렁
		return "img/char1_Foramt1.png";
	else if (value == 3) // 승주영
		return "img/char1_Foramt3.png";
	else if (value == 4) // 승잔나
		return "img/char2_Foramt1.png";
	else if (value == 5) // 승도남
		return "img/char1_Foramt4.png";
	else if (value == 6) // 승소녀
		return "img/char2_Foramt2.png";
	else if (value == 7) // 승법사
		return "img/char1_Foramt9.png";
	else if (value == 8) // 승혈귀
		return "img/char1_Foramt6.png";
	else if (value == 9) // 승수르
		return "img/char1_Foramt5.png";
	else if (value == 10) // 승아리
		return "img/char2_Foramt3.png";
	else if (value == 11) // 승의사
		return "img/char1_Foramt8.png";
	else if (value == 12) // 승의탑
		return "img/char1_Foramt7.png";

	// Pet Image
	else if (value == 100)
		return "img/Pet_dog1.png";
	else if (value == 101)
		return "img/Pet_cat1.png";
	else if (value == 102)
		return "img/Pet_cat2.png";
	else if (value == 103)
		return "img/Pet_cat3.png";
	else if (value == 104)
		return "img/Pet_cat4.png";
}



///<summary>
///  Tutorial
///</summary>
//튜토리얼
void MainScene::Tutorial()
{
	if (TutorialState == 0)
	{
		//배경음악 설정
		if (getintData("sound", 0) == 0)
			SimpleAudioEngine::getInstance()->playBackgroundMusic("sounds/Sunny Day.mp3");

		//Background image
		auto spr_bg = Sprite::create("img/background2.png");
		spr_bg->setAnchorPoint(Point(0, 1));
		spr_bg->setPosition(Point(0, 320));
		this->addChild(spr_bg, 0);

		//캐릭터1 하늘에서 내려온다
		auto char1 = Sprite::create("img/char1_Normal.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setPosition(Point(150, 550));
		this->addChild(char1, 0, "char1");

		//내려 온 후 말풍선과 함께 스토리 진행 시작
		auto char1_Move1 = MoveTo::create(4.0, Point(150, 127));
		auto char1_Delaytime1 = DelayTime::create(1.5);
		auto char1_BounceOut1 = EaseBounceOut::create(char1_Move1);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 0, 1.3, 65, 186, 2.4, "      하아...", true, false));
		auto cahr1_Sequence1 = Sequence::create(char1_Delaytime1, char1_BounceOut1, char1_Delaytime1, char_Ballon, NULL);
		char1->runAction(cahr1_Sequence1);

	}
	else if (TutorialState == 1) // char1
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 0, 1.9, 35, 226, 2.4, "삶이 왜이렇게 재미없을까..T.T", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 2) // char1
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 0, 1.3, 65, 186, 2.4, "     ...", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 3) // char1
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 0, 1.3, 65, 186, 2.4, "곧 본과 들어갈텐데...", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 4) // char1
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 0, 1.3, 65, 186, 2.4, "   ㅠㅠㅠ", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 5)
	{
		//캐릭터2 하늘에서 내려온다
		auto char1 = Sprite::create("img/char_Maker.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setPosition(Point(240, 550));
		this->addChild(char1, 0, "char2");

		//내려 온 후 말풍선과 함께 스토리 진행 시작
		auto char1_Move1 = MoveTo::create(4.0, Point(240, 127));
		auto char1_Delaytime1 = DelayTime::create(1.5);
		auto char1_BounceOut1 = EaseBounceOut::create(char1_Move1);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 1, 1.4, 280, 186, 2.4, "왠 한숨을 그렇게 쉬어?.?", false, false));
		auto cahr1_Sequence1 = Sequence::create(char1_Delaytime1, char1_BounceOut1, char1_Delaytime1, char_Ballon, NULL);
		char1->runAction(cahr1_Sequence1);
	}
	else if (TutorialState == 6) // char1
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 0, 1.3, 65, 186, 2.4, "인생이 노잼이야 ㄱ-", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 7) // char2
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 1, 1.3, 280, 186, 2.4, " 야, 의대생이!?", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 8) // char2
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 1, 1.3, 280, 186, 2.4, " 이제 본과 아니야?", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 9) // char1
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 0, 1.3, 65, 186, 2.4, "ㅇㅇ..T.T", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 10) // char2
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 1, 1.3, 280, 186, 2.4, "    홀ㅋㅋㅋㅋㅋ", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 11) // char2
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 1, 1.3, 280, 186, 2.4, "    야 힘좀 내보삼!!", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 12) // char2
	{
		auto char2 = (Sprite*)this->getChildByName("char2");
		char2->setTexture("img/char_Maker_Rabbit.png");
		auto char1_jump1 = JumpTo::create(1.0, Point(240, 127), 65, 1);
		auto char1_jump2 = JumpTo::create(1.0, Point(240, 127), 65, 1);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 1, 1.3, 280, 220, 2.4, " 토끼~ 토끼~!!", false, false));
		auto cahr2_Sequence1 = Sequence::create(char_Ballon, char1_jump1, char1_jump2, NULL);
		char2->runAction(cahr2_Sequence1);
	}
	else if (TutorialState == 13) // char1
	{
		auto char1 = (Sprite*)this->getChildByName("char1");
		auto char2 = (Sprite*)this->getChildByName("char2");
		char1->setTexture("img/char1_Normal_Angry.png");
		char2->setTexture("img/char_Maker.png");

		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 0, 1.3, 65, 186, 2.4, "아;; ㅡㅡ 욕할뻔ㅡ", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 14) // char2
	{
		auto char1 = (Sprite*)this->getChildByName("char1");
		char1->setTexture("img/char1_Normal.png");
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 1, 1.3, 280, 186, 2.4, "  ㅋㅋㅋㅋㅋㅋㅋ", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 15) // char1
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 0, 1.9, 35, 226, 2.4, "하.. 이 삶에서 일탈하고 싶어!!", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 16) // char1
	{
		auto char1 = (Sprite*)this->getChildByName("char1");
		auto char2 = (Sprite*)this->getChildByName("char2");
		auto char1_FadeOut = FadeOut::create(0.7);
		auto char2_FadeOut = FadeOut::create(0.7);
		char1->runAction(char1_FadeOut);
		char2->runAction(char2_FadeOut);

		auto spr2 = Sprite::create("img/Title1.png");
		spr2->setAnchorPoint(Point(0, 1));
		spr2->setPosition(Point(15, 320));
		spr2->setOpacity(0);
		this->addChild(spr2, 0, "spr2");

		auto Action_FadeIn = FadeIn::create(3.0);
		spr2->runAction(Action_FadeIn);

		//승마니 타이틀을 띄운후 사라지면 시작
		auto this_DelyTime1 = DelayTime::create(3.0);
		auto this_Ballon = CallFunc::create(CC_CALLBACK_0(MainScene::SpeechBubble, this, this, 0, 0, 800, 226, 0, "", true, true));
		auto spr_Sequence = Sequence::create(this_DelyTime1, this_Ballon, NULL);
		this->runAction(spr_Sequence);

		setintData("onState", 1);
	}
}

//말풍선을 띄운다
void MainScene::SpeechBubble(Ref* pSender, int color, double scale, int x, int y, double time, const char* mess, bool flipX, bool flipY)
{
	//Set SpeechBubble Image
	auto spr_Bubble = Sprite::create("img/Saybubble_Orange.png");

	if (color == 1)
		spr_Bubble = Sprite::create("img/Saybubble_SkyBlue.png");

	spr_Bubble->setAnchorPoint(Point(0, 1));
	spr_Bubble->setOpacity(0);
	spr_Bubble->setPosition(Point(x, y)); spr_Bubble->setScale(scale);
	spr_Bubble->setFlippedX(flipX);       spr_Bubble->setFlippedY(flipY);
	this->addChild(spr_Bubble, 0, "Ballon");

	auto spr_Bubble_FadeIn = FadeIn::create(1.0);
	auto spr_Bullble_FadeOut = FadeOut::create(0.7);
	auto spr_DelyTime1 = DelayTime::create(time);
	auto spr_Sequence = Sequence::create(spr_Bubble_FadeIn, spr_DelyTime1, spr_Bullble_FadeOut, NULL);
	spr_Bubble->runAction(spr_Sequence);

	//Set SpeechBubble Text
	auto label_Bullble = Label::createWithTTF(StringUtils::format("%s", mess), "fonts/Navi.ttf", 15);
	label_Bullble->setAnchorPoint(Point(0, 1));  label_Bullble->setOpacity(0);
	label_Bullble->setPosition(Point(x+10*scale, y-20*scale)); label_Bullble->setColor(Color3B(-255, -255, -255));
	this->addChild(label_Bullble, 0, "Label_Ballon");

	auto label_Bullble_FadeIn = FadeIn::create(1.0);
	auto label_Bullble_FadeOut = FadeOut::create(0.7);
	auto label_DelyTime1 = DelayTime::create(time);
	auto label_Func1 = CallFunc::create(CC_CALLBACK_0(MainScene::DeleteBubble, this, this));
	auto label_Sequence = Sequence::create(label_Bullble_FadeIn, label_DelyTime1, label_Bullble_FadeOut, label_Func1, NULL);
	label_Bullble->runAction(label_Sequence);

	
	//튜토리얼
	if (TutorialState == 16)
	{
		Init_Start();
	}
}

//말풍선을 지운다
void MainScene::DeleteBubble(Ref* pSender)
{
	auto Ballon = (Sprite*)this->getChildByName("Ballon");
	auto Label_Ballon = (Sprite*)this->getChildByName("Label_Ballon");
	this->removeChild(Ballon);
	this->removeChild(Label_Ballon);

	//튜토리얼 관련
	TutorialState++;
	Tutorial();
}


///<summary>
///  Systems
///</summary>
// 알림 메세지를 자연스럽게 출력한다
void MainScene::Info_Mess(int Color, char* mess)
{
	if (GameState == 1)
	sound_click();

	//Info Message image
	auto spr_mess = Sprite::create("img/MessBox_Red.png");
	spr_mess->setAnchorPoint(Point(0, 1));
	spr_mess->setPosition(Point(142, 370));
	spr_mess->setScale(0.8); spr_mess->setOpacity(240);
	this->addChild(spr_mess, 1);

	auto action_Move1 = MoveTo::create(1.5, Point(142, 320));
	auto Action_EaseIn = EaseIn::create(action_Move1, 1.7);
	auto Action_DelyTime1 = DelayTime::create(3.0);
	auto action_Move2 = MoveTo::create(1.2, Point(142, 370));
	auto Action_Sequence = Sequence::create(Action_EaseIn, Action_DelyTime1, action_Move2, NULL);
	spr_mess->runAction(Action_Sequence);


	//Info Message Label
	auto label1 = Label::createWithTTF(StringUtils::format("%s", mess), "fonts/Navi.ttf", 15);
	label1->setAnchorPoint(Point(0, 1));
	label1->setPosition(Point(148, 356)); label1->setColor(Color3B(-255, -255, -255));
	this->addChild(label1, 1);

	auto label1_Move1 = MoveTo::create(1.5, Point(148, 306));
	auto label1_EaseIn = EaseIn::create(label1_Move1, 1.7);
	auto label1_DelyTime1 = DelayTime::create(3.0);
	auto label1_Move2 = MoveTo::create(1.2, Point(148, 356));
	auto label1_Sequence = Sequence::create(label1_EaseIn, label1_DelyTime1, label1_Move2, NULL);
	label1->runAction(label1_Sequence);
}

//int형 데이터를 cocos 저장소에서 가져온다
int MainScene::getintData(char* data, int defaultvalue)
{
	return UserDefault::getInstance()->getIntegerForKey(data, defaultvalue);
}

//int형 데이터를 cocos 저장소에 저장한다
void MainScene::setintData(char* data, int value)
{
	if (strcmp(data, "money") == 0)
	{
		int money_de = value + 51521252;

		UserDefault::getInstance()->setIntegerForKey("money", value);
		UserDefault::getInstance()->setIntegerForKey("asdf4sdf2sdf", money_de);
		UserDefault::getInstance()->flush();

		setintData("as5f425f2sdf", 20341030);
	}
	else
	{
		UserDefault::getInstance()->setIntegerForKey(data, value);
		UserDefault::getInstance()->flush();
	}
}

//터치 패드를 눌럿을 경우
void MainScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event)
{
	auto AskExit_Board = (Sprite*)this->getChildByName("AskExit");
	if (AskExit_Board == NULL)
	{
			sound_click();
			if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)  // KEY_ESCAPE는 윈도우에선 ESC키, 스마트폰의 BACK키
			{
				//Background image
				auto spr_bg = Sprite::create("img/background_black.png");
				spr_bg->setAnchorPoint(Point(0, 1));
				spr_bg->setPosition(Point(0, 320));
				spr_bg->setOpacity(160);
				this->addChild(spr_bg, 10, "sprblock");

				//Buttons
				auto item1 = MenuItemImage::create("img/QuestionBoard1.png", "img/QuestionBoard1.png", CC_CALLBACK_1(MainScene::Ask_Exit, this, 0));
				auto item2 = MenuItemImage::create("img/Button_okay.png", "img/Button_okay.png", CC_CALLBACK_1(MainScene::Ask_Exit, this, 1));
				auto item3 = MenuItemImage::create("img/Button_no.png", "img/Button_no.png", CC_CALLBACK_1(MainScene::Ask_Exit, this, 2));
				auto menu = Menu::create(item1, item2, item3, NULL);
				item2->setPosition(Point(-48, -42));
				item3->setPosition(Point(48, -42));
				this->addChild(menu, 10, "AskExit");

				//Label
				auto label_mess = Label::createWithTTF("정말로 종료 하시겠습니까?", "fonts/Navi.ttf", 15);
				label_mess->setAnchorPoint(Point(0, 1));
				label_mess->setPosition(Point(165, 183)); label_mess->setColor(Color3B(-255, -255, -255));
				this->addChild(label_mess, 10, "Exit_Mess");
			}
	}
	else
	{
		sound_click();
		auto Background_Block = (Sprite*)this->getChildByName("sprblock");
		auto AskExit_Board = (Sprite*)this->getChildByName("AskExit");
		auto AskExit_Mess = (Sprite*)this->getChildByName("Exit_Mess");
		this->removeChild(Background_Block);
		this->removeChild(AskExit_Board); this->removeChild(AskExit_Mess);
	}
}

//종료 할건지에 대한 버튼을 누를경우
void MainScene::Ask_Exit(Ref* pSender, int value)
{
	if (value == 1)
	{
		sound_click();
		Director::getInstance()->end();
	}
	else if (value == 2)
	{
		TouchEnable = 0;

		sound_click();
		auto Background_Block = (Sprite*)this->getChildByName("sprblock");
		auto AskExit_Board = (Sprite*)this->getChildByName("AskExit");
		auto AskExit_Mess = (Sprite*)this->getChildByName("Exit_Mess");
		this->removeChild(Background_Block);
		this->removeChild(AskExit_Board); this->removeChild(AskExit_Mess);
	}
}

// Click sound
void MainScene::sound_click()
{
	int sound_state = UserDefault::getInstance()->getIntegerForKey("sound", 0);
	if (sound_state == 0)
	float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/touch.wav");
}

//배경음악
void MainScene::BackgroundMusic(float delta)
{
	//배경음악 설정
	if (getintData("sound", 0) == 0)
		SimpleAudioEngine::getInstance()->playBackgroundMusic("sounds/Lovely Summer.mp3");
}



///<summary>
/// <Java Native InterFace>
///  C++  ==>  Java
///</summary>

/*JniHelper::getStaticMethodInfo 제일 마지막 항에 (I, I)V  => 2개의 int형 parameter과 반환형이 void인 jniFunc 함수
boolean	Z
byte	B
char	C
short	S
int	I
long	J
float	F
double	D
void	V
Object	Ljava/lang/String; L / full path segmentation class
Array	[Ljava/lang/String; [signature [I
*/

void MainScene::JavaFun(int value)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t
		, "org/cocos2dx/cpp/AppActivity"
		, "jniFunc"
		, "(I)V"))
	{
		///< 함수 호출
		t.env->CallStaticVoidMethod(t.classID, t.methodID, value);
		///< Release
		t.env->DeleteLocalRef(t.classID);
	}
#endif
}
