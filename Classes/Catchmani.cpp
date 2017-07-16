#include "Catchmani.h"
#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "datas.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;
using namespace CocosDenshion;
using namespace std;

///<summary>
/// <Declare Global variables >
///</summary>
int static TouchEnable = 0;
int static TutorialState = 0;
float static HP, MP, Max_HP, Max_MP, HP_UP, MP_UP, Money_Get = 1, GameTimer = 0, MobTime[6] = { 0 }; //체/기력 , 회복계수, 얻는돈, 획득돈 계수
int static Power, SkillMP, Money, Defense, Coin = 0, Combo = 0, ComboTime, MaxCombo = 0, Boss_Hit=0, Boss_HitTime, Boss_Angry=0;
int static CrushTime, Special_Skill_Count=0, Skill_BoundTime=0;

Scene* Catchmani::createScene()
{
	auto scene = Scene::create();
	auto layer = Catchmani::create();

	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool Catchmani::init()
{
	if (!Layer::init())
	{
		return false;
	}

	// Play backbround Music
	if (getintData("sound", 0) == 0)
	{
		SimpleAudioEngine::getInstance()->playBackgroundMusic("sounds/Sunny Day.mp3");
		this->schedule(schedule_selector(Catchmani::BackgroundMusic), 92.0);
	}
	srand(time(NULL));

	// Graphics Initilaize
	Init_Graphics();

	//게임을 처음 시작 할 경우 튜토리얼 진행
	if (getintData("Game2_Tutorial", 0) == 0)
		Tutorial();
	else
		Init_Systems();

	return true;
}


///<summary>
///  Game Initialize
///</summary>
// Initialize First UI Graphics
void Catchmani::Init_Graphics()
{
	//Background image
	auto spr_bg = Sprite::create("img/Game2_bg1.png");
	spr_bg->setAnchorPoint(Point(0, 1));
	spr_bg->setPosition(Point(0, 320));
	this->addChild(spr_bg, 0, "background");

	//Hp Tool
	auto spr_Tool_Hp = Sprite::create("img/Tool_Hp.png");
	spr_Tool_Hp->setAnchorPoint(Point(0, 1));
	spr_Tool_Hp->setPosition(Point(5, 317));
	this->addChild(spr_Tool_Hp, 1);

	//Skill Tool
	auto spr_Tool_Skill = Sprite::create("img/Tool_Skill.png");
	spr_Tool_Skill->setAnchorPoint(Point(0, 1));
	spr_Tool_Skill->setPosition(Point(153, 317));
	this->addChild(spr_Tool_Skill, 1);

	// Timer Tool
	auto spr_Tool_Time = Sprite::create("img/Tool_Time.png");
	spr_Tool_Time->setAnchorPoint(Point(0, 1));
	spr_Tool_Time->setPosition(Point(303, 317));
	this->addChild(spr_Tool_Time, 1);

	auto Label_Timer = Label::createWithTTF(StringUtils::format("%dm %ds", 0, 0), "fonts/Navi.ttf", 17);
	Label_Timer->setAnchorPoint(Point(0, 1));
	Label_Timer->setPosition(Point(335, 310)); Label_Timer->setColor(Color3B(-255, -255, -255));
	this->addChild(Label_Timer, 1, "Label_Timer");
}

// Initialize Systems 
void Catchmani::Init_Systems()
{
	//이벤트 리스너에 터치기능 등록
	listener2 = EventListenerTouchOneByOne::create();
	listener2->onTouchBegan = CC_CALLBACK_2(Catchmani::onTouchBegan, this);
	listener2->onTouchEnded = CC_CALLBACK_2(Catchmani::onTouchEnded, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener2, 1);

	// Set Char Sprite loaction
	auto char1 = Sprite::create("img/char1_Normal.png");
	char1->setAnchorPoint(Point(0, 1));
	char1->setPosition(Point(60, 128));
	char1->setScale(0.65);
	this->addChild(char1, 0, "Main_Char");

	// Set Pet Sprite loaction
	if (getintData("MainPet", 0) != 0)
	{
		auto Pet = Sprite::create("img/Pet_dog1.png");
		Pet->setAnchorPoint(Point(0, 1));
		Pet->setPosition(Point(25, 103));
		Pet->setScale(0.8);
		this->addChild(Pet, 0, "Main_Pet");
		Init_Pet(); // 펫의 능력 초기화임
	}

	// Initialize Char's Image & Status & Main Pet
	Set_CharAbility();

	// Make Bounding Box
	auto char_BoundingBox = Sprite::create("img/char_BoundingBox.png");
	char_BoundingBox->setAnchorPoint(Point(0, 1));
	char_BoundingBox->setPosition(Point(84, 124));
    char_BoundingBox->setOpacity(0);
	this->addChild(char_BoundingBox, 0, "char_BoundingBox");

	// Make Skill Menus
	auto item_Heal = MenuItemImage::create("img/Box_Skill_Special.png", "img/Box_Skill_Special.png", CC_CALLBACK_1(Catchmani::Skill_Special, this));
	auto item_Skill = MenuItemImage::create("img/Box_Skill.png", "img/Box_Skill.png", CC_CALLBACK_1(Catchmani::Skill, this));
	auto menu_Buttons = Menu::create(item_Heal, item_Skill, NULL);
	item_Heal->setPosition(Point(-23, -127));
	item_Skill->setPosition(Point(31, -127));
	item_Skill->setName("SkillMenu");
	item_Heal->setName("S_SkillMenu");
	item_Heal->setScale(1.1); item_Skill->setScale(1.1);
	this->addChild(menu_Buttons, 10, "Menu_Skills");

	// Special Skill Item Sprites
	for (int i = 0; i < 3; i++)
	{
		auto Spr_S_Skill = Sprite::create("img/Icon_skill_special.png");
		Spr_S_Skill->setAnchorPoint(Point(0, 1));
		Spr_S_Skill->setPosition(Point(10 + i * 23, 281));
		this->addChild(Spr_S_Skill, 10, StringUtils::format("Spr_S_Skill%d", i + 1));
	}

	//Make Close Menu
	auto item1 = MenuItemImage::create("img/icon_stop.png", "img/icon_stop.png", CC_CALLBACK_1(Catchmani::CloseButton, this));
	auto menu1 = Menu::create(item1, NULL);
	item1->setPosition(Point(216, 136));
	item1->setScale(0.9);
	this->addChild(menu1);

	// Set Char Sprite loaction
	auto Arrow_R = Sprite::create("img/Icon_arrow.png");
	Arrow_R->setAnchorPoint(Point(0, 1));
	Arrow_R->setPosition(Point(60, 54));
	Arrow_R->setScale(1.7); Arrow_R->setOpacity(190);
	this->addChild(Arrow_R, 20, "Arrow_R");

	auto Arrow_L = Sprite::create("img/Icon_arrow.png");
	Arrow_L->setAnchorPoint(Point(0, 1));
	Arrow_L->setPosition(Point(10, 54));
	Arrow_L->setScale(1.7); Arrow_L->setOpacity(190);
	Arrow_L->setFlipX(true);
	this->addChild(Arrow_L, 20, "Arrow_L");

	// Make a Boss
	Make_Boss();

	//Set Schedules
	this->schedule(schedule_selector(Catchmani::Update_UI_Status), 0.01); // 체력, 기력의 지속적 변동 UI 설정
	this->schedule(schedule_selector(Catchmani::Update_Moving), 0.01); // Char Max Move location & Background Moving
	this->schedule(schedule_selector(Catchmani::Mob_Gen), 0.01); // Make New Monsters
	this->schedule(schedule_selector(Catchmani::Update_Mobs), 0.01); // Mobs Status Schedule
	this->schedule(schedule_selector(Catchmani::Crash_Mob), 0.01); // Crash Mobs
	this->schedule(schedule_selector(Catchmani::Update_Boss), 0.01); // Boss Status Schedule
	this->schedule(schedule_selector(Catchmani::Update_S_Skill), 0.01); // Update Special Skill
}

// Initialize Char's Image & Status & Main Pet
void Catchmani::Set_CharAbility()
{
	// 승마니
	if (getintData("MainChar", 0) == 0)
	{
		Power = char1_Normal_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char1_Normal_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Normal_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Normal_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char1_Normal_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char1_Normal_SkillMP;
	}

	//승고딩
	else if (getintData("MainChar", 0) == 1)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(1));

		Power = char1_Format2_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char1_Format2_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Format2_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Format2_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char1_Format2_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char1_Format2_SkillMP;
	}

	//승허렁
	else if (getintData("MainChar", 0) == 2)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(2));

		Power = char1_Format1_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char1_Format1_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Format1_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Format1_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char1_Format1_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char1_Format1_SkillMP;
	}

	//승주영
	else if (getintData("MainChar", 0) == 3)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(3));

		Power = char1_Format3_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char1_Format3_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Format3_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Format3_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char1_Format3_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char1_Format3_SkillMP;
	}

	//승잔나
	else if (getintData("MainChar", 0) == 4)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(4));

		Power = char2_Format1_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char2_Format1_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char2_Format1_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char2_Format1_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char2_Format1_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char2_Format1_SkillMP;
	}

	//승도남
	else if (getintData("MainChar", 0) == 5)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(5));

		Power = char1_Format4_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char1_Format4_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Format4_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Format4_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char1_Format4_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char1_Format4_SkillMP;
	}

	//승소녀
	else if (getintData("MainChar", 0) == 6)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(6));

		Power = char2_Format2_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char2_Format2_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char2_Format2_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char2_Format2_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char2_Format2_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char2_Format2_SkillMP;
	}

	//승법사
	else if (getintData("MainChar", 0) == 7)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(7));

		Power = char1_Format9_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char1_Format9_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Format9_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Format9_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char1_Format9_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char1_Format9_SkillMP;
	}

	//승혈귀
	else if (getintData("MainChar", 0) == 8)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(8));

		Power = char1_Format6_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char1_Format6_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Format6_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Format6_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char1_Format6_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char1_Format6_SkillMP;
	}

	//승수르
	else if (getintData("MainChar", 0) == 9)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(9));

		Power = char1_Format5_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char1_Format5_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Format5_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Format5_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char1_Format5_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char1_Format5_SkillMP;

		Money_Get = 1.5;
	}

	//승아리
	else if (getintData("MainChar", 0) == 10)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(10));

		Power = char2_Format3_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char2_Format3_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char2_Format3_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char2_Format3_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char2_Format3_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char2_Format3_SkillMP;
	}

	//승의사
	else if (getintData("MainChar", 0) == 11)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(11));

		Power = char1_Format8_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char1_Format8_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Format8_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Format8_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char1_Format8_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char1_Format8_SkillMP;
	}

	//승의탑
	else if (getintData("MainChar", 0) == 12)
	{
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		MainChar->setTexture(Get_CharImage(12));

		Power = char1_Format7_Power + getintData("Stat_Char_Poewr", 0) * 5, Defense = getintData("Stat_Char_Defense", 0) * 2;
		Max_HP = char1_Format7_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Format7_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Format7_MP + getintData("Stat_Char_MP", 0) * 10; MP = 0; MP_UP = char1_Format7_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
		SkillMP = char1_Format7_SkillMP;
	}


	/*
	펫의 설정
	*/
	if (getintData("MainPet", 0) == 100)
	{
		auto MainPet = (Sprite*)this->getChildByName("Main_Pet");
		MainPet->setTexture(Get_CharImage(100));

		Power += 5;
		Max_HP += 5; HP = Max_HP; HP_UP += 0.1;
		Max_MP += 10; MP_UP += 0.1;
	}
	else if (getintData("MainPet", 0) == 101)
	{
		auto MainPet = (Sprite*)this->getChildByName("Main_Pet");
		MainPet->setTexture(Get_CharImage(101));

		Power += 5;
		Max_HP += 5; HP = Max_HP; HP_UP += 0.1;
		Max_MP += 10; MP_UP += 0.1;
	}
	else if (getintData("MainPet", 0) == 102)
	{
		auto MainPet = (Sprite*)this->getChildByName("Main_Pet");
		MainPet->setTexture(Get_CharImage(102));

		Power += 5;
		Max_HP += 5; HP = Max_HP; HP_UP += 0.1;
		Max_MP += 10; MP_UP += 0.1;
	}
	else if (getintData("MainPet", 0) == 103)
	{
		auto MainPet = (Sprite*)this->getChildByName("Main_Pet");
		MainPet->setTexture(Get_CharImage(103));

		Power += 10;
		Max_HP += 5; HP = Max_HP; HP_UP += 0.1;
		Max_MP += 10; MP_UP += 0.1;
	}
	else if (getintData("MainPet", 0) == 104)
	{
		auto MainPet = (Sprite*)this->getChildByName("Main_Pet");
		MainPet->setTexture(Get_CharImage(104));

		Power += 10;
		Max_HP += 5; HP = Max_HP; HP_UP += 0.1;
		Max_MP += 10; MP_UP += 0.1;
	}
}

// Initialize game data If Exit game
void Catchmani::Init_GameValues()
{
	//Stop All Actions and Schedules
	this->pauseSchedulerAndActions();

	for (Sprite* Spr_Mob1 : Mob1)
		Spr_Mob1->pauseSchedulerAndActions();

	//Data init
	TouchEnable = 0;
	GameTimer = 0;
	Mob1.clear();
	Mobs_HP_Bar.clear();
	Item.clear();
	Coin = 0;
	Combo = 0;
	MaxCombo = 0;
	Boss_Hit = 0;
	Boss_HitTime = 0;
	Boss_Angry = 0;
	Special_Skill_Count = 0;
	Skill_BoundTime = 0;

	//터치이벤트 리스너 기능을 확실히 제거
	this->setTouchEnabled(false);
	_eventDispatcher->removeEventListenersForType(EventListener::Type::TOUCH_ONE_BY_ONE);
	Director::getInstance()->getEventDispatcher()->removeEventListener(listener2);
}


///<summary>
///  Game Monsters
///</summary>
// Make Monster Tool
void Catchmani::Make_Mob(int MobN)
{
	auto Background = (Sprite*)this->getChildByName("background");
	float Move_Speed;

	// Make Sprite
	auto char1 = Sprite::create(StringUtils::format("img/Monster/Mob%d.png", MobN));
	char1->setAnchorPoint(Point(0, 1));
	char1->setPosition(Point(940, 128));
	char1->setScale(0.65);
	Background->addChild(char1, 1, StringUtils::format("char%d", MobN));

	//Char HP Bar
	auto Mob_HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, 105, 20));
	Mob_HP_Bar->setScale(0.3);
	Mob_HP_Bar->setAnchorPoint(Point(0, 1));
	Mob_HP_Bar->setPosition(32, 105);
	char1->addChild(Mob_HP_Bar, 0, "Mob_HP_Bar");

	// Move
	if (MobN == 1 || MobN == 2)
	{
		if (6.0 - (getintData("Game2NowStage", 1)*0.2) * 0.1 > 3.0)
			Move_Speed = 6.0 - (getintData("Game2NowStage", 1)*0.2) * 0.1;
		else
			Move_Speed = 3.0;

		auto Char_Move = MoveBy::create(Move_Speed, Point(-250, 0));
		auto Char_Move_Repeat = RepeatForever::create(Char_Move);
		char1->runAction(Char_Move_Repeat);
	}
	else if (MobN == 3)
	{
		if (6.0 - (getintData("Game2NowStage", 1)*0.2) * 0.1 > 3.0)
			Move_Speed = 6.0 - (getintData("Game2NowStage", 1)*0.2) * 0.1;
		else
			Move_Speed = 3.0;

		auto Char_Jump = JumpBy::create(Move_Speed, Point(-250, 0), 90, 8);
		auto Char_Jump_Repeat = RepeatForever::create(Char_Jump);
		char1->runAction(Char_Jump_Repeat);
	}
	else if (MobN == 4)
	{
		char1->setPosition(Point(940, 173));
		if (8.0 - (getintData("Game2NowStage", 1)*0.2) * 0.1 > 3.0)
			Move_Speed = 8.0 - (getintData("Game2NowStage", 1)*0.2) * 0.1;
		else
			Move_Speed = 3.0;
		auto Char_Move = MoveBy::create(Move_Speed, Point(-250, 0));
		auto Char_Move_Repeat = RepeatForever::create(Char_Move);
		char1->runAction(Char_Move_Repeat);
	}
	else if (MobN == 5)
	{
		if (8.0 - (getintData("Game2NowStage", 1)*0.2) * 0.1 > 3.0)
			Move_Speed = 8.0 - (getintData("Game2NowStage", 1)*0.2) * 0.1;
		else
			Move_Speed = 3.0;
		auto Char_Move = MoveBy::create(Move_Speed, Point(-250, 0));
		auto Char_Move_Repeat = RepeatForever::create(Char_Move);
		char1->runAction(Char_Move_Repeat);
	}
	else if (MobN == 6)
	{
		auto Char_Jump = JumpBy::create(0.7, Point(0, 70), 80, 1);
		auto Char_Move = MoveBy::create(0.5, Point(-80, -70));
		auto Char_EaseOut = EaseOut::create(Char_Move, 1.0);
		auto Char_Delay = DelayTime::create(3.0);
		auto Char_Sequence = Sequence::create(Char_Jump, Char_EaseOut, Char_Delay, NULL);
		auto Char_Move_Repeat = RepeatForever::create(Char_Sequence);
		char1->runAction(Char_Move_Repeat);
	}

	//Set Mob HP
	if (MobN == 1)
		char1->setUserData((char*)(int)(HP_Char1 + HP_Char1*getintData("Game2NowStage", 1)*0.2));
	else if (MobN == 2)
		char1->setUserData((char*)(int)(HP_Char2 + HP_Char2*getintData("Game2NowStage", 1)*0.2));
	else if (MobN == 3)
		char1->setUserData((char*)(int)(HP_Char3 + HP_Char3*getintData("Game2NowStage", 1)*0.2));
	else if (MobN == 4)
		char1->setUserData((char*)(int)(HP_Char4 + HP_Char4*getintData("Game2NowStage", 1)*0.2));
	else if (MobN == 5)
		char1->setUserData((char*)(int)(HP_Char5 + HP_Char5*getintData("Game2NowStage", 1)*0.2));
	else if (MobN == 6)
		char1->setUserData((char*)(int)(HP_Char6 + HP_Char6*getintData("Game2NowStage", 1)*0.2));

	int count = 0;
	for (Sprite* Spr_Mob1 : Mob1)
	{
		if ((int)Spr_Mob1->getUserData() == 1000000)
		{
			int location = Mob1.getIndex(Spr_Mob1);
			Mob1.insert(location, char1);
			count = 1;
			break;
		}
	}
	if (count == 0)
		Mob1.pushBack(char1);
}

// Monster Gen
void Catchmani::Mob_Gen(float delta)
{
	for (int i = 0; i < 6; i++)
		MobTime[i] += 0.01;

	if (MobTime[0] >= 2.0)
	{
		Make_Mob(1);
		MobTime[0] = 0;
	}

	if (MobTime[1] >= 3.5 && GameTimer >= 10)
	{
		Make_Mob(2);
		MobTime[1] = 0;
	}

	if (MobTime[2] >= 7.0 && GameTimer >= 30)
	{
		Make_Mob(3);
		MobTime[2] = 0;
	}

	if (MobTime[3] >= 7.0 && GameTimer >= 60)
	{
		Make_Mob(4);
		MobTime[3] = 0;
	}

	if (MobTime[4] >= 12.0 && GameTimer >= 90)
	{
		Make_Mob(5);
		MobTime[4] = 0;
	}

	if (MobTime[5] >= 15.0 && GameTimer >= 120)
	{
		Make_Mob(6);
		MobTime[5] = 0;
	}
}

//모든 몬스터의 지속적인 상태를 관리한다. + 아이템의 드랍 & 제거as
void Catchmani::Update_Mobs(float delta)
{
	///<summary>
	/// <몬스터들의 사망처리 업데이트>
	///</summary>

	//몬스터
	for (Sprite* Spr_Mob1 : Mob1)
	{
		auto Background = (Sprite*)this->getChildByName("background");
		//Update HP Bar When Mob Alive
		if ((int)Spr_Mob1->getUserData() != 1000000)
		{
			auto HP_Bar = (Sprite*)Spr_Mob1->getChildByName("Mob_HP_Bar");
			Spr_Mob1->removeChild(HP_Bar, true);

			int Mob_HP;

			//Set HP Bar Length AS Mob HP
			if (Spr_Mob1->getName().compare("char1") == 0)
				Mob_HP = HP_Char1 + HP_Char1*getintData("Game2NowStage", 1)*0.2;
			else if (Spr_Mob1->getName().compare("char2") == 0)
				Mob_HP = HP_Char2 + HP_Char2*getintData("Game2NowStage", 1)*0.2;
			else if (Spr_Mob1->getName().compare("char3") == 0)
				Mob_HP = HP_Char3 + HP_Char3*getintData("Game2NowStage", 1)*0.2;
			else if (Spr_Mob1->getName().compare("char4") == 0)
				Mob_HP = HP_Char4 + HP_Char4*getintData("Game2NowStage", 1)*0.2;
			else if (Spr_Mob1->getName().compare("char5") == 0)
				Mob_HP = HP_Char5 + HP_Char5*getintData("Game2NowStage", 1)*0.2;
			else if (Spr_Mob1->getName().compare("char6") == 0)
				Mob_HP = HP_Char6 + HP_Char6*getintData("Game2NowStage", 1)*0.2;

			HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, (105 * (int)(Spr_Mob1->getUserData())) / Mob_HP, 20));
			HP_Bar->setPosition(32, 105);
			HP_Bar->setAnchorPoint(Point(0, 1));
			HP_Bar->setScale(0.3);
			Spr_Mob1->addChild(HP_Bar, 0, "Mob_HP_Bar");
		}

		//Die Mob
		if ((int)(Spr_Mob1->getUserData()) <= 0 && (int)(Spr_Mob1->getUserData()) != 1000000)
		{
			//사라지는 처리
			Spr_Mob1->setUserData((char*)1000000);
			Spr_Mob1->stopAllActions();
			auto Spr_Mob1_Fadeout = FadeOut::create(1.0);
			auto Spr_Mob1_Delete = MoveTo::create(0.01, Point(-500, -500));
			auto Spr_Mob1_Sequence = Sequence::create(Spr_Mob1_Fadeout, Spr_Mob1_Delete, NULL);
			Spr_Mob1_Sequence->setTag(0);
			Spr_Mob1->runAction(Spr_Mob1_Sequence);

			// 아이템이 나오는경우
			int Rand;
			Rand = rand() % 100 + 1;

			// 코인 아이템 드랍
			if (Rand >= 1 && Rand < 45)
			{
				auto Coin = Sprite::create("img/Icon_coin.png");
				Coin->setAnchorPoint(Point(1, 0));
				Coin->setScale(0.9);
				Coin->setPosition(Point(Spr_Mob1->getPositionX(), Spr_Mob1->getPositionY() - 68));
				Background->addChild(Coin, 1, "Coin");
				Coin->setUserData((char*)"Coin");

				// Drop Action
				auto action_Jump = JumpTo::create(0.6, Point(Spr_Mob1->getPositionX(), 62), 60, 1);
				Coin->runAction(action_Jump);

				Item.pushBack(Coin);
			}

			// 하트 아이템 드랍
			else if (Rand < 55)
			{
				auto Heart = Sprite::create("img/Icon_heart.png");
				Heart->setAnchorPoint(Point(1, 0));
				Heart->setPosition(Point(Spr_Mob1->getPositionX(), Spr_Mob1->getPositionY() - 68));
				Background->addChild(Heart, 1, "Heart");
				Heart->setUserData((char*)"Heart");

				// Drop Action
				auto action_Jump = JumpTo::create(0.6, Point(Spr_Mob1->getPositionX(), 62), 60, 1);
				Heart->runAction(action_Jump);

				Item.pushBack(Heart);
			}

			// 기력 아이템 드랍
			else if (Rand < 65)
			{
				auto Skill_energy = Sprite::create("img/Icon_skill.png");
				Skill_energy->setAnchorPoint(Point(1, 0));
				Skill_energy->setPosition(Point(Spr_Mob1->getPositionX(), Spr_Mob1->getPositionY() - 68));
				Background->addChild(Skill_energy, 1, "Skill_energy");
				Skill_energy->setUserData((char*)"Skill_energy");

				// Drop Action
				auto action_Jump = JumpTo::create(0.6, Point(Spr_Mob1->getPositionX(), 62), 60, 1);
				Skill_energy->runAction(action_Jump);

				Item.pushBack(Skill_energy);
			}

			// 특수 스킬 드랍
			else if (Rand < 80)
			{
				auto S_Skill = Sprite::create("img/Icon_skill_special.png");
				S_Skill->setAnchorPoint(Point(1, 0));
				S_Skill->setPosition(Point(Spr_Mob1->getPositionX(), Spr_Mob1->getPositionY() - 68));
				Background->addChild(S_Skill, 1, "S_Skill");
				S_Skill->setUserData((char*)"S_Skill");

				// Drop Action
				auto action_Jump = JumpTo::create(0.6, Point(Spr_Mob1->getPositionX(), 62), 60, 1);
				S_Skill->runAction(action_Jump);
				Item.pushBack(S_Skill);
			}

			// 콤보 띄우기
			if (Combo != 0) // 이미 콤보 스프라이트가 떠 있는경우 지워준다.
			{
				auto Spr_Combo = (Sprite*)this->getChildByName("Combo");
				this->removeChild(Spr_Combo);
			}

			Combo++;
			if (MaxCombo < Combo) MaxCombo = Combo;
			ComboTime = time(NULL);
			auto Spr_Combo = Sprite::create("img/Img_combo.png");
			Spr_Combo->setAnchorPoint(Point(0, 1));
			Spr_Combo->setScale(0.8);
			Spr_Combo->setPosition(405, 270);
			this->addChild(Spr_Combo, 1, "Combo");

			auto ComboScore = Label::createWithTTF(StringUtils::format("%d ", Combo), "fonts/Navi.ttf", 27);
			ComboScore->setAnchorPoint(Point(1, 0)); ComboScore->setColor(Color3B(-255, -255, -255));
			ComboScore->setPosition(213, 30);
			Spr_Combo->addChild(ComboScore, 1);

			auto action_Move1 = MoveTo::create(0.4, Point(305, 270));
			auto Action_EaseIn = EaseIn::create(action_Move1, 0.4);
			Spr_Combo->runAction(Action_EaseIn);
		}

		//Delete Sprite & Mob At List
		if ((int)Spr_Mob1->getUserData() == 1000000 && Spr_Mob1->getActionByTag(0) == NULL || Spr_Mob1->getPositionX() <= -30)
		{
			auto Background = (Sprite*)this->getChildByName("background");
			Mob1.eraseObject(Spr_Mob1, true);
			Background->removeChild(Spr_Mob1, true);
			break;
		}
	}

	// 아이템 제거
	for (Sprite* Spr_Item : Item)
	{
		if ((char*)(Spr_Item->getUserData()) == "Touched" && Spr_Item->getActionByTag(0) == NULL)
		{
			auto Background = (Sprite*)this->getChildByName("background");
			Item.eraseObject(Spr_Item, true);
			Background->removeChild(Spr_Item, true);
			break;
		}
	}
}

// Touch Mobs
void Catchmani::Touch_Mob(Point location)
{
	Rect Mobs_Rect;
	for (Sprite* Spr_Mob1 : Mob1)
	{
		auto Background = (Sprite*)this->getChildByName("background");
		Mobs_Rect = Spr_Mob1->getBoundingBox();
		if (Mobs_Rect.containsPoint(location) && (int)(Spr_Mob1->getUserData()) != 1000000)
		{
			int Rand;
			Rand = rand() % 100 + 1;

			//몹 체력의 감소
			if (Rand <= 95)
				Spr_Mob1->setUserData((char*)((int)(Spr_Mob1->getUserData()) - Power));

			// 크리티컬
			else
			{
				Spr_Mob1->setUserData((char*)((int)(Spr_Mob1->getUserData()) - Power * 2));

				if ((Sprite*)this->getChildByName("Critical"))
				{
					auto Spr_critical = (Sprite*)this->getChildByName("Critical");
					this->removeChild(Spr_critical);
				}
				auto Critical = Label::createWithTTF(StringUtils::format("크리티컬!"), "fonts/Navi.ttf", 13);
				Critical->setAnchorPoint(Point(0, 1));
				Critical->setPosition(Point(Spr_Mob1->getPositionX() + Background->getPositionX() + 10, Spr_Mob1->getPositionY()));
				Critical->setColor(Color3B(-255, -255, -255));
				this->addChild(Critical, 10, "Critical");

				auto Action_Fadeout = FadeOut::create(0.8);
				Critical->runAction(Action_Fadeout);
				auto Critical_Jump = JumpBy::create(0.8, Point(0, 20), 20, 1);
				Critical->runAction(Critical_Jump);
			}

			//몬스터의 넉백
			if (Spr_Mob1->getName().compare("char6") != 0)
			{
				Spr_Mob1->pauseSchedulerAndActions();

				//캐릭터에 따른 다른 넉백 및 특성  

				//승도남
				if (getintData("MainChar", 0) == 5)
				{
					auto Mob_Jump = JumpBy::create(0.2, Point(20, 0), 30, 1); 
					Spr_Mob1->runAction(Mob_Jump);
					Spr_Mob1->resumeSchedulerAndActions();
				}

				//승소녀
				else if (getintData("MainChar", 0) == 6)
				{
					int Skill_Rand = rand() % 100 + 1;

					if (Skill_Rand <= 10)
					{
						auto Mob_Jump = JumpBy::create(0.2, Point(70, 0), 30, 1); 
						Spr_Mob1->runAction(Mob_Jump);
						Spr_Mob1->resumeSchedulerAndActions();
					}
					else
					{
						auto Mob_Jump = JumpBy::create(0.2, Point(15, 0), 30, 1); 
						Spr_Mob1->runAction(Mob_Jump);
						Spr_Mob1->resumeSchedulerAndActions();
					}
				}

				//승법사
				else if (getintData("MainChar", 0) == 7)
				{
					int Skill_Rand = rand() % 100 + 1;

					//몹 체력의 감소
					if (Skill_Rand <= 10)
					{
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 150));

						//Particle
						location = Point(48, 130);
						auto Particle = ParticleSystemQuad::create("particles/BlueMagic.plist");
						Particle->setScale(0.15);
						Particle->setPosition(location);
						Spr_Mob1->addChild(Particle, 100);
					}

					else
					{
						auto Mob_Jump = JumpBy::create(0.2, Point(15, 0), 30, 1);
						Spr_Mob1->runAction(Mob_Jump);
					}
					Spr_Mob1->resumeSchedulerAndActions();
				}

				//승아리
				else if (getintData("MainChar", 0) == 10)
				{
					//몹이 느려진다
					if (Spr_Mob1->getPositionX() < 210) { auto Mob_Jump = JumpBy::create(1.0, Point(-15, 0), 0, 0); Spr_Mob1->runAction(Mob_Jump); }
					else{ auto Mob_Jump = JumpBy::create(1.0, Point(15, 0), 0, 0); Spr_Mob1->runAction(Mob_Jump); }
					Spr_Mob1->resumeSchedulerAndActions();
				}

				//승의탑
				else if (getintData("MainChar", 0) == 12)
				{
					//Sound
					if (getintData("sound", 0) == 0)
						float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Magic_Skill.wav");

					//Skill Action
					auto Mob_Jump = JumpBy::create(0.2, Point(15, 0), 30, 1); 
					Spr_Mob1->runAction(Mob_Jump);

					auto Delay = DelayTime::create(0.8);
					auto this_Func1 = CallFunc::create(CC_CALLBACK_0(Catchmani::DisposeStrun, this, this));
					auto Sequence1 = Sequence::create(Delay, this_Func1, NULL);
					this->runAction(Sequence1);
				}
				else
				{
					auto Mob_Jump = JumpBy::create(0.2, Point(15, 0), 30, 1); 
					Spr_Mob1->runAction(Mob_Jump);
					Spr_Mob1->resumeSchedulerAndActions();
				}
			}

			//<캐릭터의 특성>
			// 승혈귀
			if (getintData("MainChar", 0) == 8)
			{
				if (HP + 1 <= Max_HP)
					HP += 1;
			}
			//승의사
			else if (getintData("MainChar", 0) == 11)
			{
				if (MP + 2 <= Max_MP)
					MP += 2;
			}

			//Sound & Particle
			Mob_Effect("particles/particle_Hit1.plist", location, 0);
			}
		}
}

// Touch Items
void Catchmani::Touch_Item(Point location)
{
	Rect Mobs_Rect;
	for (Sprite* Spr_Item : Item)
	{
		auto Background = (Sprite*)this->getChildByName("background");
		Mobs_Rect = Spr_Item->getBoundingBox();
		if (Mobs_Rect.containsPoint(location))
		{
			// 돈을 터치한 경우
			if ((char*)(Spr_Item->getUserData()) == "Coin")
			{
				auto MainChar = (Sprite*)this->getChildByName("Main_Char");
				Coin += 5;

				auto Item_Jump = JumpTo::create(0.8, Point(MainChar->getPositionX() - Background->getPositionX() + 40, MainChar->getPositionY() - 20), 35, 1);
				auto Action_Fadeout = FadeOut::create(0.8);
				auto Spr_Item_Delete = MoveTo::create(0.01, Point(-500, -500));
				auto Spr_Item_Sequence = Sequence::create(Action_Fadeout, Spr_Item_Delete, NULL);
				Spr_Item_Sequence->setTag(0);
				Spr_Item->runAction(Item_Jump);
				Spr_Item->runAction(Spr_Item_Sequence);
				Spr_Item->setUserData((char*)"Touched");
			}

			// 하트를 터치한 경우
			if ((char*)(Spr_Item->getUserData()) == "Heart")
			{
				//Sound
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Potison.wav");

				if (HP + 25 <= Max_HP)
					HP += 25;
				else
					HP = Max_HP;
				auto Item_Move = MoveTo::create(1.2, Point(50 - Background->getPositionX(), 300));
				auto Action_Fadeout = FadeOut::create(1.2);
				auto Spr_Item_Delete = MoveTo::create(0.01, Point(-500, -500));
				auto Spr_Item_Sequence = Sequence::create(Action_Fadeout, Spr_Item_Delete, NULL);
				Spr_Item_Sequence->setTag(0);
				Spr_Item->runAction(Item_Move);
				Spr_Item->runAction(Spr_Item_Sequence);
				Spr_Item->setUserData((char*)"Touched");
			}

			// 스킬에너지를 터치한 경우
			if ((char*)(Spr_Item->getUserData()) == "Skill_energy")
			{
				//Sound
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Potison.wav");

				if (MP + 30 <= Max_MP)
					MP += 30;
				else
					MP = Max_MP;
				auto Item_Move = MoveTo::create(1.2, Point(200 - Background->getPositionX(), 300));
				auto Action_Fadeout = FadeOut::create(1.2);
				auto Spr_Item_Delete = MoveTo::create(0.01, Point(-500, -500));
				auto Spr_Item_Sequence = Sequence::create(Action_Fadeout, Spr_Item_Delete, NULL);
				Spr_Item_Sequence->setTag(0);
				Spr_Item->runAction(Item_Move);
				Spr_Item->runAction(Spr_Item_Sequence);
				Spr_Item->setUserData((char*)"Touched");
			}

			// 특수 스킬 아이템을 터치한 경우
			if ((char*)(Spr_Item->getUserData()) == "S_Skill")
			{
				//Sound
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Potison.wav");

				if (Special_Skill_Count != 3)
					Special_Skill_Count++;

				auto Item_Move = MoveTo::create(1.2, Point(15 - Background->getPositionX(), 270));
				auto Action_Fadeout = FadeOut::create(1.2);
				auto Spr_Item_Delete = MoveTo::create(0.01, Point(-500, -500));
				auto Spr_Item_Sequence = Sequence::create(Action_Fadeout, Spr_Item_Delete, NULL);
				Spr_Item_Sequence->setTag(0);
				Spr_Item->runAction(Item_Move);
				Spr_Item->runAction(Spr_Item_Sequence);
				Spr_Item->setUserData((char*)"Touched");
			}
		}
	}
}

// Crash Mobs
void Catchmani::Crash_Mob(float delta)
{
	auto Background = (Sprite*)this->getChildByName("background");
	auto MainChar = (Sprite*)this->getChildByName("Main_Char");
	auto Spr_MainChar = (Sprite*)this->getChildByName("char_BoundingBox");
	auto Boss = (Sprite*)Background->getChildByName("Boss");
	Spr_MainChar->setPositionX(MainChar->getPositionX() + 24 - Background->getPositionX());
	auto location = Spr_MainChar->getPosition();
	Rect Spr_MainChar_Rect = Spr_MainChar->getBoundingBox();
	Rect Mobs_Rect;

	//몬스터와 부딪히는 경우
	for (Sprite* Spr_Mob1 : Mob1)
	{
		Mobs_Rect = Spr_Mob1->getBoundingBox();
		if (Mobs_Rect.intersectsRect(Spr_MainChar_Rect) && (int)(Spr_Mob1->getUserData()) != 1000000)
		{
			//Damage Actior
			float Damage = (int)(Spr_Mob1->getUserData())*0.1 - getintData("Stat_Char_Defense", 0) * 2;

			if (Damage < 0)
				HP -= 5;
			else if (Damage * 3 < 25)
				HP -= Damage * 3;
			else
				HP -= 25;

			//Sound & Particle
			Mob_Effect("particles/particle_Hit1.plist", location , 1);

			//사라지는 처리
			Spr_Mob1->setUserData((char*)1000000);
			Spr_Mob1->stopAllActions();
			auto Spr_Mob1_Fadeout = FadeOut::create(1.0);
			auto Spr_Mob1_Delete = MoveTo::create(0.01, Point(-500, -500));
			auto Spr_Mob1_Sequence = Sequence::create(Spr_Mob1_Fadeout, Spr_Mob1_Delete, NULL);
			Spr_Mob1->runAction(Spr_Mob1_Sequence);

			auto HP_Bar = (Sprite*)Spr_Mob1->getChildByName("Mob_HP_Bar");
			Spr_Mob1->removeChild(HP_Bar, true);
		}
	}

	// Crash Boss Monster
	Mobs_Rect = Boss->getBoundingBox();
	if (Mobs_Rect.intersectsRect(Spr_MainChar_Rect) && (time(NULL) - CrushTime) >= 1)
	{
		CrushTime = time(NULL);
		HP -= 40;

		Boss->pauseSchedulerAndActions();
		auto Mob_Jump = MoveBy::create(0.2, Point(70, 0));
		Boss->runAction(Mob_Jump);
		Boss->resumeSchedulerAndActions();
		Boss_Hit = 0;
	}

	//캐릭터의 체력이 0 이하로 떨어졌을때
	if (HP <= 0)
		Gameover();
}

//몹을 때릴경우 사운드, 이펙트를 처리한다
void Catchmani::Mob_Effect(char* particle, Point location, int sound)
{
	auto Background = (Sprite*)this->getChildByName("background");
	float SkillScale = 0.5;

	//<Particle & Sound As Chars>

	//승잔나
	if (getintData("MainChar", 0) == 4 && sound != 1)
	{
		particle = "particles/Wind.plist";
		SkillScale = 0.15;

		sound = 100;
		if (getintData("sound", 0) == 0)
			float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Wind2.wav");
	}

	//승소녀
	if (getintData("MainChar", 0) == 6 && sound != 1)
	{
		particle = "particles/PopStar.plist";
		SkillScale = 0.15;
	}

	//승법사
	if (getintData("MainChar", 0) == 7 && sound != 1)
		particle = "particles/particle_Hit2.plist";

	//승혈귀
	if (getintData("MainChar", 0) == 8 && sound != 1)
		particle = "particles/particle_Hit3.plist";

	//승수르
	if (getintData("MainChar", 0) == 9 && sound != 1)
		particle = "particles/particle_Hit4.plist";

	//승아리
	if (getintData("MainChar", 0) == 10 && sound != 1)
	{
		particle = "particles/Heart.plist";
		SkillScale = 0.15;
	}

	//승의사
	if (getintData("MainChar", 0) == 11 && sound != 1)
		particle = "particles/particle_Hit5.plist";

	//승의탑
	if (getintData("MainChar", 0) == 12)
	{
		particle = "particles/particle_Hit6.plist";
		SkillScale = 0.15;
	}

	if (getintData("sound", 0) == 0)
	{
		if (sound == 0)
			float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Hit1.wav");
		else if (sound == 1)
			float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Hit1.wav");
		else if (getintData("MainChar", 0) != 4)
			float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Hit1.wav");
	}

	//특수 효과
	auto Particle = ParticleSystemQuad::create(particle);
	Particle->setScale(SkillScale);
	Particle->setPosition(Point(location.x + Background->getPositionX(), location.y));
	Particle->setOpacity(225);
	this->addChild(Particle, 100);
}



///<summary>
///  Skill
///</summary>
//Use Skill
void Catchmani::Skill(Ref* pSender)
{
	if (TouchEnable == 0)
	{
		Point location;
		auto Background = (Sprite*)this->getChildByName("background");

		//몬스터들에 대해서 스킬 사용
		if (MP >= SkillMP)
		{
			sound_click();
			//Use MP
			MP -= SkillMP;

			//Sound
			//승소녀
			if (getintData("MainChar", 0) == 6)
			{
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/PowerKick.wav");
			}
			//승법사
			else if (getintData("MainChar", 0) == 7)
			{
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Magic_Skill.wav");
			}
			//승혈귀
			else if (getintData("MainChar", 0) == 8)
			{
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Magic1.wav");

				if (HP + 40 <= Max_HP)
					HP += 40;
				else
					HP = Max_HP;
			}
			//승수르
			else if (getintData("MainChar", 0) == 9)
			{
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Spin.mp3");
			}
			//승아리 , 승의탑
			else if (getintData("MainChar", 0) == 10 || getintData("MainChar", 0) == 12)
			{
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Magic1.wav");
			}
			//승의사
			else if (getintData("MainChar", 0) == 11)
			{
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Potison.wav");

				if (HP + 40 <= Max_HP)
					HP += 40;
				else
					HP = Max_HP;
			}
			//그외
			else
			{
				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Hit1.wav");
			}


			///<summary>
			/// <스킬이 모든 몬스터에게 적용되는 부분>
			///</summary>
			for (Sprite* Spr_Mob1 : Mob1)
			{
				if ((int)(Spr_Mob1->getUserData()) <= 100000 && (int)(Spr_Mob1->getUserData()) > 0)
				{
					//승마니 승고딩 승허렁 승주영
					if (getintData("MainChar", 0) == 0 || getintData("MainChar", 0) == 1 || getintData("MainChar", 0) == 2 || getintData("MainChar", 0) == 3)
					{
						location = Spr_Mob1->getPosition();
						location.x + Background->getPositionX();

						//몹 체력의 감소
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 75));

						Spr_Mob1->pauseSchedulerAndActions();
						if (Spr_Mob1->getPositionX() < 210) { auto Mob_Jump = JumpBy::create(0.2, Point(-40, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump); }
						else{ auto Mob_Jump = JumpBy::create(0.2, Point(40, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump); }
						Spr_Mob1->resumeSchedulerAndActions();

						//Particle
						Mob_Effect("particles/particle_Hit1.plist", location, 100);
					}

					//승잔나 승도남
					else if (getintData("MainChar", 0) == 4 || getintData("MainChar", 0) == 5)
					{
						location = Spr_Mob1->getPosition();

						//몹 체력의 감소
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 80));

						Spr_Mob1->pauseSchedulerAndActions();
						if (Spr_Mob1->getPositionX() < 210) { auto Mob_Jump = JumpBy::create(0.2, Point(-70, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						else{ auto Mob_Jump = JumpBy::create(0.2, Point(70, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						Spr_Mob1->resumeSchedulerAndActions();

						//Particle
						Mob_Effect("particles/particle_Hit1.plist", location, 10000);
					}

					//승소녀
					else if (getintData("MainChar", 0) == 6)
					{
						location = Point(48, 48);

						//몹 체력의 감소
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 500));

						Spr_Mob1->pauseSchedulerAndActions();
						if (Spr_Mob1->getPositionX() < 210) { auto Mob_Jump = JumpBy::create(0.2, Point(-50, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						else{ auto Mob_Jump = JumpBy::create(0.2, Point(50, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						Spr_Mob1->resumeSchedulerAndActions();

						//Particle
						auto Particle = ParticleSystemQuad::create("particles/PopStar.plist");
						Particle->setScale(0.3);
						Particle->setPosition(location);
						Spr_Mob1->addChild(Particle, 100);
					}

					//승법사
					else if (getintData("MainChar", 0) == 7)
					{
						location = Point(48, 130);

						//몹 체력의 감소
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 150));

						Spr_Mob1->pauseSchedulerAndActions();
						if (Spr_Mob1->getPositionX() < 210) { auto Mob_Jump = JumpBy::create(0.2, Point(-40, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						else{ auto Mob_Jump = JumpBy::create(0.2, Point(40, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						Spr_Mob1->resumeSchedulerAndActions();

						//Particle
						auto Particle = ParticleSystemQuad::create("particles/BlueMagic.plist");
						Particle->setScale(0.15);
						Particle->setPosition(location);
						Spr_Mob1->addChild(Particle, 100);
					}

					//승혈귀
					else if (getintData("MainChar", 0) == 8)
					{
						location = Point(48, 48);

						//몹 체력의 감소
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 300));

						Spr_Mob1->pauseSchedulerAndActions();
						if (Spr_Mob1->getPositionX() < 210) { auto Mob_Jump = JumpBy::create(0.2, Point(-40, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						else{ auto Mob_Jump = JumpBy::create(0.2, Point(40, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						Spr_Mob1->resumeSchedulerAndActions();

						//Particle
						auto Particle = ParticleSystemQuad::create("particles/Red_Light.plist");
						Particle->setScale(0.3);
						Particle->setPosition(location);
						Spr_Mob1->addChild(Particle, 100);
					}

					//승수르
					else if (getintData("MainChar", 0) == 9)
					{
						location = Point(48, 98);

						//몹 체력의 감소
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 300));

						Spr_Mob1->pauseSchedulerAndActions();
						if (Spr_Mob1->getPositionX() < 210) { auto Mob_Jump = JumpBy::create(0.2, Point(-40, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						else{ auto Mob_Jump = JumpBy::create(0.2, Point(40, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						Spr_Mob1->resumeSchedulerAndActions();

						//Particle
						auto Particle = ParticleSystemQuad::create("particles/MoneyDrop.plist");
						Particle->setScale(0.12);
						Particle->setPosition(location);
						Spr_Mob1->addChild(Particle, 100);
					}

					//승아리
					else if (getintData("MainChar", 0) == 10)
					{
						location = Point(48, 48);

						//몹 체력의 감소
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 300));

						//몹이 느려진다
						Spr_Mob1->pauseSchedulerAndActions();
						if (Spr_Mob1->getPositionX() < 210) { auto Mob_Jump = JumpBy::create(2.5, Point(-40, 0), 0, 0); Spr_Mob1->runAction(Mob_Jump); }
						else{ auto Mob_Jump = JumpBy::create(2.5, Point(40, 0), 0, 0); Spr_Mob1->runAction(Mob_Jump); }
						Spr_Mob1->resumeSchedulerAndActions();

						//Particle
						auto Particle = ParticleSystemQuad::create("particles/Heart_Skill.plist");
						Particle->setScale(0.2);
						Particle->setPosition(location);
						Spr_Mob1->addChild(Particle, 100);
					}

					//승의사
					else if (getintData("MainChar", 0) == 11)
					{
						location = Point(48, 48);

						//몹 체력의 감소
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 300));

						Spr_Mob1->pauseSchedulerAndActions();
						if (Spr_Mob1->getPositionX() < 210) { auto Mob_Jump = JumpBy::create(0.2, Point(-40, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						else{ auto Mob_Jump = JumpBy::create(0.2, Point(40, 0), 50, 1); Spr_Mob1->runAction(Mob_Jump); }
						Spr_Mob1->resumeSchedulerAndActions();

						//Particle
						auto Particle = ParticleSystemQuad::create("particles/particle_Hit2.plist");
						Particle->setScale(0.3);
						Particle->setPosition(location);
						Spr_Mob1->addChild(Particle, 100);
					}

					//승의탑
					else if (getintData("MainChar", 0) == 12)
					{
						location = Point(48, 48);

						//몹 체력의 감소
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 400));

						//Skill Action
						Spr_Mob1->pauseSchedulerAndActions();
						auto Delay = DelayTime::create(2.0);
						auto this_Func1 = CallFunc::create(CC_CALLBACK_0(Catchmani::DisposeStrun, this, this));
						auto Sequence1 = Sequence::create(Delay, this_Func1, NULL);
						this->runAction(Sequence1);

						//Particle
						auto Particle = ParticleSystemQuad::create("particles/particle_Hit6.plist");
						Particle->setScale(0.2);
						Particle->setPosition(location);
						Spr_Mob1->addChild(Particle, 100);
					}
				}
			}
		}
		else
		{
			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
		}
	}
}

//Use Special Skill
void Catchmani::Skill_Special(Ref* pSender)
{
	if (Special_Skill_Count == 3)
	{
		auto Background = (Sprite*)this->getChildByName("background");
		auto Boss = (Sprite*)Background->getChildByName("Boss");
		auto MainChar = (Sprite*)this->getChildByName("Main_Char");
		Special_Skill_Count = 0;

		// 넉백
		if (getintData("NowSpecialSkill", 0) == 1)
		{
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Magic_Skill.wav");
			// Normal Monster
			for (Sprite* Spr_Mob1 : Mob1)
			{
				Spr_Mob1->pauseSchedulerAndActions();
				auto Mob_Jump = MoveBy::create(0.2, Point(100, 0));
				Spr_Mob1->runAction(Mob_Jump);
				Spr_Mob1->resumeSchedulerAndActions();

				// Particle Animation
				auto Particle = ParticleSystemQuad::create("particles/Purple_Fire.plist");
				Particle->setScale(0.4);
				Particle->setPosition(Point(-50, 0));
				Spr_Mob1->addChild(Particle, 100, "KnuckBack");

				auto Action_Move = MoveBy::create(1.0, Point(300, MainChar->getPositionY()));
				Particle->runAction(Action_Move);
			}

			// Boss
			Boss->pauseSchedulerAndActions();
			auto Mob_Jump = MoveBy::create(0.2, Point(100, 0));
			Boss->runAction(Mob_Jump);
			Boss->resumeSchedulerAndActions();
		}

		// 모방
		else if (getintData("NowSpecialSkill", 0) == 2)
		{
			MP += SkillMP;
			Skill(this);
		}

		// 스틸
		else if (getintData("NowSpecialSkill", 0) == 3)
		{
			for (Sprite* Spr_Item : Item)
			{
				auto Background = (Sprite*)this->getChildByName("background");

				if (getintData("sound", 0) == 0)
					float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Potison.wav");

				if ((char*)(Spr_Item->getUserData()) == "Coin")
					Coin += 5;
				else if ((char*)(Spr_Item->getUserData()) == "Heart")
				{
					if (HP + 25 <= Max_HP)
						HP += 25;
					else
						HP = Max_HP;
				}
				else if ((char*)(Spr_Item->getUserData()) == "Skill_energy")
				{
					if (MP + 30 <= Max_MP)
						MP += 30;
					else
						MP = Max_MP;
				}
				else if ((char*)(Spr_Item->getUserData()) == "S_Skill")
				{
					if (Special_Skill_Count != 3)
						Special_Skill_Count++;
				}

				auto Item_Jump = JumpTo::create(0.8, Point(MainChar->getPositionX() - Background->getPositionX() + 20, MainChar->getPositionY() - 20), 35, 1);
				auto Action_Fadeout = FadeOut::create(0.8);
				auto Spr_Item_Delete = MoveTo::create(0.01, Point(-500, -500));
				auto Spr_Item_Sequence = Sequence::create(Action_Fadeout, Spr_Item_Delete, NULL);
				Spr_Item_Sequence->setTag(0);
				Spr_Item->runAction(Item_Jump);
				Spr_Item->runAction(Spr_Item_Sequence);
				Spr_Item->setUserData((char*)"Touched");
			}
		}

		// 차원의 벽
		else if (getintData("NowSpecialSkill", 0) == 4)
		{
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/item_power.wav");

			//Background image
			auto Sprite_Bound = Sprite::create("img/Skill_Bound.png");
			Sprite_Bound->setAnchorPoint(Point(0, 1));
			Sprite_Bound->setPosition(Point(MainChar->getPositionX() - Background->getPositionX() + 100, 320));
			Sprite_Bound->setOpacity(0);
			Background->addChild(Sprite_Bound, 100, "Skill_Bound");

			// Particle Animation
			auto Particle = ParticleSystemQuad::create("particles/Skill_Bound.plist");
			Particle->setScale(0.4);
			Particle->setPosition(Point(30, 0));
			Sprite_Bound->addChild(Particle, 0);
			Skill_BoundTime = time(NULL);
		}
	}
	else
	{
		//Sound
		if (getintData("sound", 0) == 0)
			float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/not_sel.ogg");
	}
}

// Update Special Skill
void Catchmani::Update_S_Skill(float delta)
{
	auto Background = (Sprite*)this->getChildByName("background");
	auto Spr_Skill_Bound = (Sprite*)Background->getChildByName("Skill_Bound");
	// Skill Bound
	if (time(NULL) - Skill_BoundTime <= 4 && Spr_Skill_Bound)
	{
		auto Boss = (Sprite*)Background->getChildByName("Boss");
		Rect Mobs_Rect, Spr_Skill_Bound_Rect = Spr_Skill_Bound->getBoundingBox();

		// Crash Normal Monster
		for (Sprite* Spr_Mob1 : Mob1)
		{
			Mobs_Rect = Spr_Mob1->getBoundingBox();
			if (Mobs_Rect.intersectsRect(Spr_Skill_Bound_Rect))
			{
				Spr_Mob1->pauseSchedulerAndActions();
				auto Mob_Jump = MoveBy::create(0.2, Point(50, 0));
				Spr_Mob1->runAction(Mob_Jump);
				Spr_Mob1->resumeSchedulerAndActions();
			}
		}

		// Crash Boss Monster
		Mobs_Rect = Boss->getBoundingBox();
		if (Mobs_Rect.intersectsRect(Spr_Skill_Bound_Rect))
		{
			Boss->pauseSchedulerAndActions();
			auto Mob_Jump = MoveBy::create(0.2, Point(50, 0));
			Boss->runAction(Mob_Jump);
			Boss->resumeSchedulerAndActions();
		}
	}
	else
	{
		auto Background = (Sprite*)this->getChildByName("background");
		auto Spr_Skill_Bound = (Sprite*)Background->getChildByName("Skill_Bound");

		if (Spr_Skill_Bound)
			Background->removeChild(Spr_Skill_Bound, true);
	}
}

//몬스터의 스턴을 해제한다
void Catchmani::DisposeStrun(Ref* pSender)
{
	for (Sprite* Spr_Mob1 : Mob1)
	{
		Spr_Mob1->resumeSchedulerAndActions();
	}
}



///<summary>
///  Boss Monster
///</summary>
// Make a Boss
void Catchmani::Make_Boss()
{
	auto Background = (Sprite*)this->getChildByName("background");
	int Boss_Sprite = (getintData("Game2NowStage", 1)-1) / 5;

	// Make Sprite
	auto char1 = Sprite::create(StringUtils::format("img/Monster/Boss%d.png", Boss_Sprite+1));
	char1->setAnchorPoint(Point(0, 1));
	char1->setPosition(Point(820, 192));
	char1->setFlippedX(true);
	char1->setScale(1.2);
	Background->addChild(char1, 0, "Boss");

	// Boss Moving
	auto Char_Move = MoveBy::create(10.0, Point(-250, 0));
	auto Char_Move_Repeat = RepeatForever::create(Char_Move);
	char1->runAction(Char_Move_Repeat);

	// Boss Animation
	auto animation = Animation::create();
	animation->setDelayPerUnit(0.2);
	for (int i = 0; i < 8; i++)
		animation->addSpriteFrameWithFileName(String::create(StringUtils::format("img/Monster/Boss%d_%d.png", Boss_Sprite + 1, i))->getCString());
	auto action_0 = RepeatForever::create(Animate::create(animation));
	char1->runAction(action_0);

	//Hp Tool
	auto spr_Tool_Hp = Sprite::create("img/Tool_Boss_HPTool.png");
	spr_Tool_Hp->setAnchorPoint(Point(0, 1));
	spr_Tool_Hp->setPosition(Point(110, 262));
	spr_Tool_Hp->setScale(1.2); spr_Tool_Hp->setOpacity(200);
	this->addChild(spr_Tool_Hp, 1, "Boss_HPTool");

	// Make boss Icon on Boss HP Tool
	auto Icon = Sprite::create(StringUtils::format("img/Monster/Boss%d.png", Boss_Sprite + 1));
	Icon->setPosition(Point(14, 23));
	Icon->setFlippedX(true);
	Icon->setScale(0.3);
	spr_Tool_Hp->addChild(Icon, 0, "Boss_Icon");

	// Make HP Bar Sprite
	auto Mob_HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, 105, 20));
	Mob_HP_Bar->setPosition(20, 31);
	Mob_HP_Bar->setAnchorPoint(Point(0, 1));
	Mob_HP_Bar->setScaleX(1.7); Mob_HP_Bar->setOpacity(200);
	spr_Tool_Hp->addChild(Mob_HP_Bar, 0, "Boss_HP_Bar");

	// Set Boss HP
	char1->setUserData((char*)(int)(HP_Boss + HP_Boss*getintData("Game2NowStage", 1)*0.15));
}

// Touch Boss
void Catchmani::Touch_Boss(Point location)
{
	auto Background = (Sprite*)this->getChildByName("background");
	auto Boss = (Sprite*)Background->getChildByName("Boss");
	Rect Mobs_Rect = Boss->getBoundingBox();

	if (Mobs_Rect.containsPoint(location))
	{
		int Rand;
		Rand = rand() % 100 + 1;

		// Hit
		if (Rand <= 100 - Char_Critical)
			Boss->setUserData((char*)((int)(Boss->getUserData()) - Power));

		// Occur Critical
		else
		{
			Boss->setUserData((char*)((int)(Boss->getUserData()) - Power * 2));

			if ((Sprite*)this->getChildByName("Critical"))
			{
				auto Spr_critical = (Sprite*)this->getChildByName("Critical");
				this->removeChild(Spr_critical);
			}
			auto Critical = Label::createWithTTF(StringUtils::format("크리티컬!"), "fonts/Navi.ttf", 13);
			Critical->setAnchorPoint(Point(0, 1));
			Critical->setPosition(Point(Boss->getPositionX() + Background->getPositionX() + 45, Boss->getPositionY()-30));
			Critical->setColor(Color3B(-255, -255, -255));
			this->addChild(Critical, 10, "Critical");

			auto Action_Fadeout = FadeOut::create(0.8);
			Critical->runAction(Action_Fadeout);
			auto Critical_Jump = JumpBy::create(0.8, Point(0, 20), 20, 1);
			Critical->runAction(Critical_Jump);
		}

		// Knuck Back Count
		if (Boss_Hit == 5)
		{
			if (Boss->getPositionX() + 50 <= 820)
			{
				Boss->pauseSchedulerAndActions();
				auto Mob_Jump = MoveBy::create(0.2, Point(50, 0));
				Boss->runAction(Mob_Jump);
				Boss->resumeSchedulerAndActions();
				Boss_Hit = 0;
			}
			else
			{
				Boss->pauseSchedulerAndActions();
				auto Mob_Jump = MoveTo::create(0.2, Point(820, Boss->getPositionY()));
				Boss->runAction(Mob_Jump);
				Boss->resumeSchedulerAndActions();
				Boss_Hit = 0;
			}
		}
		// Dash To Char
		else
		{
			Boss_Hit++;
			int Rand2; 
			Rand2 = rand() % 100 + 1;
			if (Rand2 <= 3)
			{
				// Boss Say - Ballon
				const char* Bal;
				for (int i = 0; i < 3; i++)
				{
					Rand2 = rand() % 5 + 1;
					Make_Mob(Rand2);
				}
				Rand2 = rand() % 4;
				if (Rand2 == 0)
					Bal = "   크앙크앙!!";
				else if (Rand2 == 1)
					Bal = "   크아앙~!";
				else if (Rand2 == 2)
					Bal = "   감히 나를 때리다니..!";
				else if (Rand2 == 3)
					Bal = "   가랏~~!!";
				auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 0, 1.0, Boss->getPositionX() + Background->getPositionX() - 20, Boss->getPositionY() + 5, 2.4, Bal, true, false));
				this->runAction(char_Ballon);

				// Dash
				Boss->pauseSchedulerAndActions();
				auto Mob_Jump = MoveBy::create(0.2, Point(-50, 0));
				Boss->runAction(Mob_Jump);
				Boss->resumeSchedulerAndActions();
			}
		}
		//Sound & Particle
		Mob_Effect("particles/particle_Hit1.plist", location, 0);

		// Boss Touch Time
		Boss_HitTime = time(NULL);
	}
}

// Boss Status Update Schedule
void Catchmani::Update_Boss(float delta)
{
	auto Background = (Sprite*)this->getChildByName("background");
	auto Boss = (Sprite*)Background->getChildByName("Boss");
	auto Boss_HP_Tool = (Sprite*)this->getChildByName("Boss_HPTool");
	auto Boss_HP_Bar = (Sprite*)Boss_HP_Tool->getChildByName("Boss_HP_Bar");
	auto Boss_Icon = (Sprite*)Boss_HP_Tool->getChildByName("Boss_Icon");
	auto Ballon = (Sprite*)this->getChildByName("Ballon");
	auto Label_Ballon = (Sprite*)this->getChildByName("Label_Ballon");
	
	// HP Update
	Boss_HP_Tool->removeChild(Boss_HP_Bar, true);
	int Mob_HP = Mob_HP = HP_Boss + HP_Boss*getintData("Game2NowStage", 1)*0.15;
	Boss_HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, (105 * (int)(Boss->getUserData())) / Mob_HP, 20));
	Boss_HP_Bar->setPosition(20, 32);
	Boss_HP_Bar->setAnchorPoint(Point(0, 1));
	Boss_HP_Bar->setScaleX(1.7); Boss_HP_Bar->setOpacity(200);
	Boss_HP_Tool->addChild(Boss_HP_Bar, 0, "Boss_HP_Bar");

	// On battle => Boss HP Tool Visible
	if (time(NULL) - Boss_HitTime >= 4)
	{
		Boss_HP_Tool->setOpacity(0);
		Boss_HP_Bar->setOpacity(0);
		Boss_Icon->setOpacity(0);
	}
	else
	{
		Boss_HP_Tool->setOpacity(255);
		Boss_HP_Bar->setOpacity(255);
		Boss_Icon->setOpacity(255);
	}

	// Boss Angey
	for (int k = 1; k <= 2; k++)
	{
		if ((int)(Boss->getUserData()) <= (int)((HP_Boss + HP_Boss*getintData("Game2NowStage", 1)*0.15) / 3)*k && Boss_Angry == 2-k)
		{
			int Rand;
			const char* Bal;
			for (int i = 0; i < 5; i++)
			{
				Rand = rand() % 5 + 1;
				Make_Mob(Rand);
			}
			Rand = rand() % 4;
			if (Rand == 0)
				Bal = "   가랏!!";
			else if (Rand == 1)
				Bal = "   내가 질것 같으냐!";
			else if (Rand == 2)
				Bal = "   크아앙~!";
			else if (Rand == 3)
				Bal = "   감히 나를 때리다니..!";
			auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 0, 1.0, Boss->getPositionX() + Background->getPositionX() - 20, Boss->getPositionY() + 5, 2.4, Bal, true, false));
			this->runAction(char_Ballon);
			Boss_Angry++;
		}
	}
	if (Ballon && Label_Ballon)
	{
		Ballon->setPosition(Point(Boss->getPositionX() + Background->getPositionX() - 20, Boss->getPositionY() + 5));
		Label_Ballon->setPosition(Point(Boss->getPositionX() + Background->getPositionX() - 10, Boss->getPositionY() - 15));
	}

	// If Out of Background
	if (Boss->getPositionX() <= 0)
		Boss->setPositionX(860);

	// Boss Die?
	if ((int)(Boss->getUserData()) <= 0)
		GameClear();
}



///<summary>
///  End Game
///</summary>
// Gameover
void Catchmani::Gameover()
{
	//Stop All Actions and Schedules
	this->pauseSchedulerAndActions();
	for (Sprite* Spr_Mob1 : Mob1)
	{
		Spr_Mob1->pauseSchedulerAndActions();
	}
	auto background = (Sprite*)this->getChildByName("background");
	auto Boss = (Sprite*)background->getChildByName("Boss");
	Boss->pauseSchedulerAndActions();

	//Background image
	auto spr_bg = Sprite::create("img/background_black.png");
	spr_bg->setAnchorPoint(Point(0, 1));
	spr_bg->setPosition(Point(0, 320));
	spr_bg->setOpacity(160);
	this->addChild(spr_bg, 10);

	//Make Advertise
	JavaFun_ad(5);

	//Buttons
	auto item1 = MenuItemImage::create("img/GameOver_Menu2.png", "img/GameOver_Menu2.png", CC_CALLBACK_1(Catchmani::Ask_Exit, this, 0));
	auto item2 = MenuItemImage::create("img/Button_Right.png", "img/Button_Right.png", CC_CALLBACK_1(Catchmani::Ask_Exit, this, 1));
	auto menu = Menu::create(item1, item2, NULL);
	item2->setPosition(Point(5, -65));
	this->addChild(menu, 10);

	// 게임오버 보상
	Money = (int)(Money_Get*GameTimer*5 + Coin);
	setintData("money", getintData("money", 0) + Money);

	// Make Star
	for (int i = 0; i < 3; i++)
	{
		auto Star_Image = Sprite::create("img/Icon_star_inactive.png");
		Star_Image->setAnchorPoint(Point(0, 1));
		Star_Image->setPosition(Point(188 + 45 * i, 210));
		Star_Image->setScale(1.3);
		this->addChild(Star_Image, 10);
	}

	//Label - Play Time
	auto label_Time = Label::createWithTTF(StringUtils::format("%d 분 %d 초", (int)GameTimer / 60, (int)GameTimer % 60), "fonts/Navi.ttf", 15);
	label_Time->setAnchorPoint(Point(0, 1));
	label_Time->setPosition(Point(220, 172)); label_Time->setColor(Color3B(-255, -255, -255));
	this->addChild(label_Time, 10);

	//Label - Get Money
	auto label_GetMoney = Label::createWithTTF(StringUtils::format("%d 마니(+%d)", Money, Coin), "fonts/Navi.ttf", 15);
	label_GetMoney->setAnchorPoint(Point(0, 1));
	label_GetMoney->setPosition(Point(220, 135)); label_GetMoney->setColor(Color3B(-255, -255, -255));
	this->addChild(label_GetMoney, 10);

	//Imgage
	auto spr_bg2 = Sprite::create("img/Spr_Gameover.png");
	spr_bg2->setAnchorPoint(Point(0, 1));
	spr_bg2->setPosition(Point(0, 320)); spr_bg2->setOpacity(0);
	this->addChild(spr_bg2, 10);

	auto spr_FadeIn = FadeIn::create(2.0);
	spr_bg2->runAction(spr_FadeIn);
}

// Game Clear
void Catchmani::GameClear()
{
	int Star, NowStage = getintData("Game2NowStage", 1);

	//Stop All Actions and Schedules
	this->pauseSchedulerAndActions();
	for (Sprite* Spr_Mob1 : Mob1)
	{
		Spr_Mob1->pauseSchedulerAndActions();
	}
	auto background = (Sprite*)this->getChildByName("background");
	auto Boss = (Sprite*)background->getChildByName("Boss");
	Boss->pauseSchedulerAndActions();

	//Background image
	auto spr_bg = Sprite::create("img/background_black.png");
	spr_bg->setAnchorPoint(Point(0, 1));
	spr_bg->setPosition(Point(0, 320));
	spr_bg->setOpacity(160);
	this->addChild(spr_bg, 10);

	//Make Advertise
	JavaFun_ad(5);

	//Buttons
	auto item1 = MenuItemImage::create("img/GameOver_Menu2.png", "img/GameOver_Menu2.png", CC_CALLBACK_1(Catchmani::Ask_Exit, this, 0));
	auto item2 = MenuItemImage::create("img/Button_Right.png", "img/Button_Right.png", CC_CALLBACK_1(Catchmani::Ask_Exit, this, 1));
	auto item3 = MenuItemImage::create("img/btn_kakao.png", "img/btn_kakao.png", CC_CALLBACK_1(Catchmani::KakaoLink, this));
	auto menu = Menu::create(item1, item2, item3, NULL);
	item2->setPosition(Point(70, -65));
	item3->setPosition(Point(-40, -65));
	item3->setScale(0.9);
	this->addChild(menu, 10);

	// 게임오버 보상
	if (GameTimer <= 120)
		Star = 3;
	else if (GameTimer <= 180)
		Star = 2;
	else if (GameTimer <= 120)
		Star = 1;

	if (getintData((char*)StringUtils::format("Game2_Stage%d", NowStage).c_str(), 0) == 0)
	{
		Money = (int)(Money_Get*GameTimer * 5 + Coin + Star * 1000);
		setintData("money", getintData("money", 0) + Money);
		setintData((char*)StringUtils::format("Game2_Stage%d", NowStage + 1).c_str(), 0);
	}
	else
	{
		Money = (int)(Money_Get*GameTimer * 5 + Coin);
		setintData("money", getintData("money", 0) + Money);
	}
	setintData((char*)StringUtils::format("Game2_Stage%d", NowStage).c_str(), Star);
	
	// Make Star
	for (int i = 0; i < 3; i++)
	{
		auto Star_Image = Sprite::create("img/Icon_star_inactive.png");
		Star_Image->setAnchorPoint(Point(0, 1));
		Star_Image->setPosition(Point(188 + 45 * i, 210));
		Star_Image->setScale(1.3);
		this->addChild(Star_Image, 10);

		if (i + 1 <= Star)
			Star_Image->setTexture("img/Icon_star.png");
	}

	//Label - Play Time
	auto label_Time = Label::createWithTTF(StringUtils::format("%d 분 %d 초", (int)GameTimer / 60, (int)GameTimer % 60), "fonts/Navi.ttf", 15);
	label_Time->setAnchorPoint(Point(0, 1));
	label_Time->setPosition(Point(220, 172)); label_Time->setColor(Color3B(-255, -255, -255));
	this->addChild(label_Time, 10);

	//Label - Get Money
	auto label_GetMoney = Label::createWithTTF(StringUtils::format("%d 마니(+%d)", Money, Coin), "fonts/Navi.ttf", 15);
	label_GetMoney->setAnchorPoint(Point(0, 1));
	label_GetMoney->setPosition(Point(220, 135)); label_GetMoney->setColor(Color3B(-255, -255, -255));
	this->addChild(label_GetMoney, 10);

	//Imgage
	auto spr_bg2 = Sprite::create("img/Spr_Gameclear.png");
	spr_bg2->setAnchorPoint(Point(0, 1));
	spr_bg2->setPosition(Point(0, 320)); spr_bg2->setOpacity(0);
	this->addChild(spr_bg2, 10);

	auto spr_FadeIn = FadeIn::create(2.0);
	spr_bg2->runAction(spr_FadeIn);

	//Particle
	auto Particle1 = ParticleSystemQuad::create("particles/Congratulation.plist");
	Particle1->setScale(0.35);
	Particle1->setPosition(110, 275);
	this->addChild(Particle1, 10);

	auto Particle2 = ParticleSystemQuad::create("particles/Congratulation.plist");
	Particle2->setScale(0.35);
	Particle2->setPosition(370, 275);
	this->addChild(Particle2, 10);

	if (getintData("sound", 0) == 0)
		float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/win.mp3");
}

// Button - KakaoLink
void Catchmani::KakaoLink(Ref* pSender)
{
	sound_click();

	// Jni - kakao link
	string Datas_kakao;
	Datas_kakao = UserDefault::getInstance()->getStringForKey("Name", "*1^&-_$@");
	Datas_kakao += "&";
	Datas_kakao += to_string((int)GameTimer);
	Datas_kakao += "&";
	Datas_kakao += to_string(getintData("Game2NowStage", 1));

	const char * Data_kakao = Datas_kakao.c_str();
	JavaFun_kakaolink((char*)Data_kakao);
}



///<summary>
///  Pet System
///</summary>
// Initialize Pet System
void Catchmani::Init_Pet()
{
	// 도둑냥
	if (getintData("MainPet", 0) == 102)
		this->schedule(schedule_selector(Catchmani::PetSkill1), 0.01);

	// 페르냥
	else if (getintData("MainPet", 0) == 103)
		this->schedule(schedule_selector(Catchmani::PetSkill2), 5.0);

	// 백호당
	else if (getintData("MainPet", 0) == 104)
		this->schedule(schedule_selector(Catchmani::PetSkill3), 5.0);
}

// 도둑냥 Skill
void Catchmani::PetSkill1(float delta)
{
	for (Sprite* Spr_Item : Item)
	{
		auto Background = (Sprite*)this->getChildByName("background");
		if ((char*)(Spr_Item->getUserData()) == "Coin")
		{
			auto Spr_MainChar = (Sprite*)this->getChildByName("Main_Pet");
			Coin += 3;

			auto Item_Jump = JumpTo::create(0.8, Point(Spr_MainChar->getPositionX() - Background->getPositionX() + 20, Spr_MainChar->getPositionY() - 20), 35, 1);
			auto Action_Fadeout = FadeOut::create(0.8);
			auto Spr_Item_Delete = MoveTo::create(0.01, Point(-500, -500));
			auto Spr_Item_Sequence = Sequence::create(Action_Fadeout, Spr_Item_Delete, NULL);
			Spr_Item_Sequence->setTag(0);
			Spr_Item->runAction(Item_Jump);
			Spr_Item->runAction(Spr_Item_Sequence);
			Spr_Item->setUserData((char*)"Touched");
		}
	}
}

// 페르냥 Skill
void Catchmani::PetSkill2(float delta)
{
	for (Sprite* Spr_Mob1 : Mob1)
	{
		//몹 체력의 감소
		Spr_Mob1->setUserData((char*)((int)(Spr_Mob1->getUserData()) - (int)(Power / 2)));
		Spr_Mob1->pauseSchedulerAndActions();

		if (Spr_Mob1->getPositionX() < 210)
		{
			auto Mob_Jump = JumpBy::create(0.2, Point(-15, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
		}
		else
		{
			auto Mob_Jump = JumpBy::create(0.2, Point(15, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
		}
		Spr_Mob1->resumeSchedulerAndActions();
	}
}

// 백호당 Skill
void Catchmani::PetSkill3(float delta)
{
	for (Sprite* Spr_Mob1 : Mob1)
	{
		//몹 체력의 감소
		Spr_Mob1->setUserData((char*)((int)(Spr_Mob1->getUserData()) - (int)(Power / 2)));
		Spr_Mob1->pauseSchedulerAndActions();

		if (Spr_Mob1->getPositionX() < 210)
		{
			auto Mob_Jump = JumpBy::create(0.2, Point(-50, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
		}
		else
		{
			auto Mob_Jump = JumpBy::create(0.2, Point(50, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
		}
		Spr_Mob1->resumeSchedulerAndActions();
	}
}



///<summary>
///  Game Touch Events
///</summary>
bool Catchmani::onTouchBegan(Touch *touch, Event *unused_event)
{
	Point location = touch->getLocation();

	// Get Sprites
	auto MainChar = (Sprite*)this->getChildByName("Main_Char");
	auto MainPet = (Sprite*)this->getChildByName("Main_Pet");
	auto Background = (Sprite*)this->getChildByName("background");
	auto Arrow_R = (Sprite*)this->getChildByName("Arrow_R");
	auto Arrow_L = (Sprite*)this->getChildByName("Arrow_L");

	// Get Sprite Rect
	Rect Rect_Arrow_R = Arrow_R->getBoundingBox();
	Rect Rect_Arrow_L = Arrow_L->getBoundingBox();

	if (TouchEnable == 0)
	{
		///<summary>
		///  Move Arrow Touch
		///</summary>
		// Move Right
		if (Rect_Arrow_R.containsPoint(location))
		{
			// *Could not Share Action
			auto action1 = MoveBy::create(1.0, Point(50, 0));
			auto Move_Char = RepeatForever::create(action1);
			MainChar->runAction(Move_Char);
			if (getintData("MainPet", 0) != 0)
			{
				auto action2 = MoveBy::create(1.0, Point(50, 0));
				auto Move_Pet = RepeatForever::create(action2);
				MainPet->runAction(Move_Pet);
			}
			auto action3 = MoveBy::create(0.5, Point(-50, 0));
			auto Move_BG = RepeatForever::create(action3);
			Background->runAction(Move_BG);
			Arrow_R->setColor(Color3B(189, 189, 189));
		}

		// Move Left
		if (Rect_Arrow_L.containsPoint(location))
		{
			// *Could not Share Action
			auto action1 = MoveBy::create(0.5, Point(-50, 0));
			auto Move_Char = RepeatForever::create(action1);
			MainChar->runAction(Move_Char);
			if (getintData("MainPet", 0) != 0)
			{
				auto action2 = MoveBy::create(0.5, Point(-50, 0));
				auto Move_Pet = RepeatForever::create(action2);
				MainPet->runAction(Move_Pet);
			}
			auto action3 = MoveBy::create(0.5, Point(50, 0));
			auto Move_BG = RepeatForever::create(action3);
			Background->runAction(Move_BG);
			Arrow_L->setColor(Color3B(189, 189, 189));
		}

		///<summary>
		///  Monsters
		///</summary>
		// Mosters Add in Background vs Touch Event Occur in layer
		if (Background->getPositionX() == 0)
		{
			Point location = touch->getLocation();
			Touch_Mob(Point(location.x, location.y));
			Touch_Item(Point(location.x, location.y));
			Touch_Boss(Point(location.x, location.y));
		}
		else
		{
			Point location = touch->getLocation();
			Touch_Mob(Point(location.x - Background->getPositionX(), location.y));
			Touch_Item(Point(location.x - Background->getPositionX(), location.y));
			Touch_Boss(Point(location.x - Background->getPositionX(), location.y));
		}
	}
	return true;
}

void Catchmani::onTouchEnded(Touch *touch, Event *unused_event)
{
	// Get Sprites
	auto MainChar = (Sprite*)this->getChildByName("Main_Char");
	auto MainPet = (Sprite*)this->getChildByName("Main_Pet");
	auto Background = (Sprite*)this->getChildByName("background");
	auto Arrow_R = (Sprite*)this->getChildByName("Arrow_R");
	auto Arrow_L = (Sprite*)this->getChildByName("Arrow_L");

	// Actions & Effects
	MainChar->stopAllActions();
	if (getintData("MainPet", 0) != 0)
		MainPet->stopAllActions();
	Background->stopAllActions();
	Arrow_R->setColor(Color3B(255, 255, 255));
	Arrow_L->setColor(Color3B(255, 255, 255));
}



///<summary>
///  Game Update Schedules
///</summary>
// Update Game UI Status
void Catchmani::Update_UI_Status(float delta)
{
	// Initialize Combo
	int NowTime = time(NULL);
	if (NowTime - ComboTime >= 3)
	{
		auto Spr_Combo = (Sprite*)this->getChildByName("Combo");
		this->removeChild(Spr_Combo);
		Combo = 0;
	}

	//Update HP Bar
	auto Spr_HP_Bar = (Sprite*)this->getChildByName("HP_Bar");
	this->removeChild(Spr_HP_Bar);

	auto HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, (105 * HP) / Max_HP, 20));
	HP_Bar->setPosition(29, 313);
	HP_Bar->setAnchorPoint(Point(0, 1));
	this->addChild(HP_Bar, 2, "HP_Bar");

	//Update MP Bar
	auto Spr_MP_Bar = (Sprite*)this->getChildByName("MP_Bar");
	this->removeChild(Spr_MP_Bar);

	auto MP_Bar = CCSprite::create("img/MP_Bar.png", CCRectMake(0, 0, (105 * MP) / Max_MP, 20));
	MP_Bar->setPosition(178, 313);
	MP_Bar->setAnchorPoint(Point(0, 1));
	this->addChild(MP_Bar, 2, "MP_Bar");

	// Update Timer Label
	GameTimer += 0.01;
	auto Label_Timer = (Label*)this->getChildByName("Label_Timer");
	Label_Timer->setString(StringUtils::format("%dm %ds", (int)GameTimer / 60, (int)GameTimer % 60));

	//Update HP & MP
	if (HP + HP_UP * 0.02 <= Max_HP)
		HP += HP_UP * 0.02;

	if (MP + MP_UP * 0.04 <= Max_MP)
		MP += MP_UP * 0.04;

	// Skill Active vs InActive
	auto Menu_Skills = (Menu*)this->getChildByName("Menu_Skills");
	auto SkillMenu = (MenuItemImage*)Menu_Skills->getChildByName("SkillMenu");
	auto S_SkillMenu = (MenuItemImage*)Menu_Skills->getChildByName("S_SkillMenu");

	if (MP >= SkillMP)
		SkillMenu->setColor(Color3B(255, 255, 255));
	else
		SkillMenu->setColor(Color3B(189, 189, 189));

	if (Special_Skill_Count == 3)
		S_SkillMenu->setColor(Color3B(255, 255, 255));
	else
		S_SkillMenu->setColor(Color3B(189, 189, 189));

	// Special Skill 
	for (int i = 0; i < 3; i++)
	{
		if (i + 1 <= Special_Skill_Count)
		{
			auto Spr_S_Skill = (Sprite*)this->getChildByName(StringUtils::format("Spr_S_Skill%d", i+1));
			Spr_S_Skill->setColor(Color3B(255, 255, 255));
		}
		else
		{
			auto Spr_S_Skill = (Sprite*)this->getChildByName(StringUtils::format("Spr_S_Skill%d", i + 1));
			Spr_S_Skill->setColor(Color3B(189, 189, 189));
		}
	}
}

// Update Game Systems
void Catchmani::Update_Moving(float delta)
{
	auto MainChar = (Sprite*)this->getChildByName("Main_Char");
	auto MainPet = (Sprite*)this->getChildByName("Main_Pet");
	auto Background = (Sprite*)this->getChildByName("background");

	// Left Move Max
	if (MainChar->getPositionX() <= 50)
	{
		MainChar->setPositionX(50);
		if (getintData("MainPet", 0) != 0)
			MainPet->setPositionX(15);
		if (Background->getPositionX() > 0)
		{
			auto Move = MoveTo::create(0.5, Point(0, Background->getPositionY()));
			Background->runAction(Move);
		}
	}

	// Right Move Max
	if (MainChar->getPositionX() >= 240 && Background->getPositionX() <= -240)
	{
		MainChar->setPositionX(240);
		if (getintData("MainPet", 0) != 0)
			MainPet->setPositionX(205);
	}
	else if (MainChar->getPositionX() == 240 && Background->getPositionX() <= -240)
	{
		MainChar->setPositionX(240);
		if (getintData("MainPet", 0) != 0)
			MainPet->setPositionX(205);
	}

	// Max Background Position
	if (Background->getPositionX() <= -480)
		Background->setPositionX(-480);
}



///<summary>
///  Game Tutorial
///</summary>
void Catchmani::Tutorial()
{
	if (TutorialState == 0)
	{
		//초기 검은 스크린
		auto Black_background = Sprite::create("img/background_black.png");
		Black_background->setAnchorPoint(Point(0, 1));
		Black_background->setPosition(Point(0, 320));
		this->addChild(Black_background, 10);

		//캐릭터1의 위치 설정
		auto char1 = Sprite::create("img/char1_Normal.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setPosition(Point(60, 133));
		char1->setScale(0.7);
		this->addChild(char1, 0, "char1");

		//화면의 페이드 아웃
		auto Action_FadeIn = FadeOut::create(5.0);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 0, 1.0, 5, 188, 2.4, "     으... 여긴 어디지..", true, false));
		auto cahr1_Sequence1 = Sequence::create(Action_FadeIn, char_Ballon, NULL);
		Black_background->runAction(cahr1_Sequence1);
	}
	else if (TutorialState == 1)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 0, 1.0, 5, 188, 2.4, "     정말로 현실을 \n   이탈 해버린걸까..?", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 2)
	{
		auto background = (Sprite*)this->getChildByName("background");

		// Appear Monster Image
		auto char1 = Sprite::create("img/Monster/Boss1.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setPosition(Point(360, 133));
		char1->setScale(0.6);
		char1->setFlippedX(true);
		this->addChild(char1, 0, "object_01");

		auto char2 = Sprite::create("img/Monster/Boss1.png");
		char2->setAnchorPoint(Point(0, 1));
		char2->setPosition(Point(830, 190));
		char2->setFlippedX(true);
		char2->setScale(1.2);
		background->addChild(char2, 0, "object_02");

		auto char_delay = DelayTime::create(1.5);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 0, 1.0, 5, 188, 2.4, "     저.. 저건 주.. \n   아니 오렌지버섯!?", true, false));
		auto char_Sequence = Sequence::create(char_delay, char_Ballon, NULL);
		this->runAction(char_Sequence);
	}
	else if (TutorialState == 3)
	{
		auto Object1 = (Sprite*)this->getChildByName("object_01");
		auto Jump1 = JumpTo::create(1.2, Point(Object1->getPositionX() - 100, Object1->getPositionY()), 65, 2);
		auto char_delay = DelayTime::create(1.5);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 0, 1.0, 5, 188, 2.4, "     으.. 으악!! 오지마!!", true, false));
		Object1->runAction(Jump1);
		auto char_Sequence = Sequence::create(char_delay, char_Ballon, NULL);
		this->runAction(char_Sequence);
	}
	else if (TutorialState == 4)
	{
		//캐릭터2 하늘에서 내려온다
		auto char1 = Sprite::create("img/char_Maker.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setPosition(Point(120, 550));
		char1->setScale(0.7);
		this->addChild(char1, 0, "char2");

		//내려 온 후 말풍선과 함께 스토리 진행 시작
		auto char1_Move1 = MoveTo::create(4.0, Point(120, 133));
		auto char1_Delaytime1 = DelayTime::create(1.0);
		auto char1_BounceOut1 = EaseBounceOut::create(char1_Move1);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 1, 1.0, 130, 198, 1.0, " 얍! 얍!", false, false));
		auto cahr1_Sequence1 = Sequence::create(char1_Delaytime1, char1_BounceOut1, char1_Delaytime1, char_Ballon, NULL);
		char1->runAction(cahr1_Sequence1);
	}
	else if (TutorialState == 5)
	{
		auto Object1 = (Sprite*)this->getChildByName("object_01");

		// Sound Effect
		if (getintData("sound", 0) == 0)
			float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Magic1.wav");

		//특수 효과
		auto Particle = ParticleSystemQuad::create("particles/Red_Light.plist");
		Particle->setScale(0.2);
		Particle->setPosition(Object1->getPosition());
		this->addChild(Particle, 100);

		auto Spr_Mob1_Fadeout = FadeOut::create(1.0);
		Object1->runAction(Spr_Mob1_Fadeout);

		auto char1_Delaytime1 = DelayTime::create(1.0);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 0, 1.0, 5, 188, 2.4, "     야! 이거 어떻게\n   된거야?", true, false));
		auto cahr1_Sequence1 = Sequence::create(char1_Delaytime1, char_Ballon, NULL);
		this->runAction(cahr1_Sequence1);
	}
	else if (TutorialState == 6)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 1, 1.0, 130, 198, 1.0, " 휴.. 너 지금 가상에 갇혔어", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 7)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 1, 1.0, 130, 198, 1.0, " 여길 나가려면 저~기", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 8)
	{
		auto background = (Sprite*)this->getChildByName("background");
		auto Move = MoveTo::create(3.0, Point(-480, 320));
		background->runAction(Move);

		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 1, 3.0, 500, 500, 1.0, "dd", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 9)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 1, 1.0, 130, 198, 1.0, " 저 보스를 잡아야", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 10)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 1, 1.0, 130, 198, 1.0, "이곳에서 나갈 수 있어!", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 11)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 1, 1.5, 130, 198, 1.0, "스토리상 내 역할은 여기까지ㅜㅜ", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 12)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 1, 1.0, 130, 198, 1.0, " 그럼.. 빠이!", false, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 13)
	{
		auto char3 = (Sprite*)this->getChildByName("char2");
		this->removeChild(char3);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 0, 1.0, 5, 188, 1.5, "   아 ㅡㅡ", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 14)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Catchmani::SpeechBubble, this, this, 0, 1.0, 5, 188, 1.5,  "내가 다 물리쳐주지!!", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 15)
	{
		auto End = CallFunc::create(CC_CALLBACK_0(Catchmani::Tutorial_End, this, this));
		this->runAction(End);
	}
}

// Tutorial End initialize
void Catchmani::Tutorial_End(Ref* pSender)
{
	auto background = (Sprite*)this->getChildByName("background");
	auto char1 = (Sprite*)this->getChildByName("char1");
	auto char2 = (Sprite*)this->getChildByName("object_01");
	auto char4 = (Sprite*)background->getChildByName("object_02");
	this->removeChild(char1); this->removeChild(char2);
	background->removeChild(char4, true);
	setintData("Game2_Tutorial", 1);

	auto Move = MoveTo::create(0, Point(0, 320));
	background->runAction(Move);

	Init_Systems();
}

//말풍선을 띄운다   => 색상, 크기, x좌표, y좌표, 시간, 내용, x대칭, y대칭
void Catchmani::SpeechBubble(Ref* pSender, int color, double scale, int x, int y, double time, const char* mess, bool flipX, bool flipY)
{
	auto Ballon = (Sprite*)this->getChildByName("Ballon");
	auto Label_Ballon = (Sprite*)this->getChildByName("Label_Ballon");
	if (Ballon && Label_Ballon)
	{
		this->removeChild(Ballon);
		this->removeChild(Label_Ballon);
	}

	//Set SpeechBubble Image
	auto spr_Bubble = Sprite::create("img/Saybubble_Orange.png");

	if (color == 1)
		spr_Bubble = Sprite::create("img/Saybubble_SkyBlue.png");
	else if (color == 2)
		spr_Bubble = Sprite::create("img/Saybubble_Rainbow.png");

	//승잔나 승소녀 승아리
	if (getintData("MainChar", 0) == 4 || getintData("MainChar", 0) == 6 || getintData("MainChar", 0) == 10)
		spr_Bubble = Sprite::create("img/Saybubble_Pink.png");

	//승도남 승법사 승수르
	if (getintData("MainChar", 0) == 5 || getintData("MainChar", 0) == 7 || getintData("MainChar", 0) == 9)
		spr_Bubble = Sprite::create("img/Saybubble_Green.png");

	//승혈귀 승의사 승의탑
	if (getintData("MainChar", 0) == 8 || getintData("MainChar", 0) == 11 || getintData("MainChar", 0) == 12)
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
	auto label_Bullble = Label::createWithTTF(StringUtils::format("%s", mess), "fonts/Navi.ttf", 11);
	label_Bullble->setAnchorPoint(Point(0, 1));  label_Bullble->setOpacity(0);
	label_Bullble->setPosition(Point(x + 10 * scale, y - 20 * scale)); label_Bullble->setColor(Color3B(-255, -255, -255));
	this->addChild(label_Bullble, 1, "Label_Ballon");

	auto label_Bullble_FadeIn = FadeIn::create(1.0);
	auto label_Bullble_FadeOut = FadeOut::create(0.7);
	auto label_DelyTime1 = DelayTime::create(time);
	auto label_Func1 = CallFunc::create(CC_CALLBACK_0(Catchmani::DeleteBubble, this, this));
	auto label_Sequence = Sequence::create(label_Bullble_FadeIn, label_DelyTime1, label_Bullble_FadeOut, label_Func1, NULL);
	label_Bullble->runAction(label_Sequence);
}

//말풍선을 지운다
void Catchmani::DeleteBubble(Ref* pSender)
{
	auto Ballon = (Sprite*)this->getChildByName("Ballon");
	auto Label_Ballon = (Sprite*)this->getChildByName("Label_Ballon");
	this->removeChild(Ballon);
	this->removeChild(Label_Ballon);

	//튜토리얼 관련

	if (getintData("Game2_Tutorial", 0) == 0)
	{
		TutorialState++;
		Tutorial();
	}
}



///<summary>
///  Game System
///</summary>
// Get Main Char Image & Main Pet Image
char* Catchmani::Get_CharImage(int value)
{
	if (value == 0) // 노말 승마니
		return "img/char1_Normal.png";
	else if (value == 1) // 승고딩
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

//로비로 이동 버튼
void Catchmani::CloseButton(Ref* pSender)
{
	if (TouchEnable == 0)
	{
		sound_click();
		TouchEnable = 1;

		//Background image
		auto spr_bg = Sprite::create("img/background_black.png");
		spr_bg->setAnchorPoint(Point(0, 1));
		spr_bg->setPosition(Point(0, 320));
		spr_bg->setOpacity(160);
		this->addChild(spr_bg, 10, "sprblock");

		//Buttons
		auto item1 = MenuItemImage::create("img/QuestionBoard1.png", "img/QuestionBoard1.png", CC_CALLBACK_1(Catchmani::Ask_Exit, this, 0));
		auto item2 = MenuItemImage::create("img/Button_okay.png", "img/Button_okay.png", CC_CALLBACK_1(Catchmani::Ask_Exit, this, 1));
		auto item3 = MenuItemImage::create("img/Button_no.png", "img/Button_no.png", CC_CALLBACK_1(Catchmani::Ask_Exit, this, 2));
		auto menu = Menu::create(item1, item2, item3, NULL);
		item2->setPosition(Point(-48, -42));
		item3->setPosition(Point(48, -42));
		this->addChild(menu, 10, "AskExit");

		//Label
		auto label_mess = Label::createWithTTF("로비로 이동하시겠습니까?", "fonts/Navi.ttf", 15);
		label_mess->setAnchorPoint(Point(0, 1));
		label_mess->setPosition(Point(165, 183)); label_mess->setColor(Color3B(-255, -255, -255));
		this->addChild(label_mess, 10, "Exit_Mess");

		//Stop All Actions and Schedules
		this->pauseSchedulerAndActions();
		for (Sprite* Spr_Mob1 : Mob1)
		{
			Spr_Mob1->pauseSchedulerAndActions();
		}
		auto background = (Sprite*)this->getChildByName("background");
		auto Boss = (Sprite*)background->getChildByName("Boss");
		Boss->pauseSchedulerAndActions();
	}
}

// ask exit
void Catchmani::Ask_Exit(Ref* pSender, int value)
{
	//Exit Game
	if (value == 1)
	{
		Init_GameValues();
		JavaFun_ad(6); // Close Advertise

		auto scene = TransitionFade::create(1.5, MainScene::createScene());
		Director::getInstance()->replaceScene(scene);
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

		//Resume All Actions and Schedules
		this->resumeSchedulerAndActions();
		for (Sprite* Spr_Mob1 : Mob1)
		{
			Spr_Mob1->resumeSchedulerAndActions();
		}
		auto background = (Sprite*)this->getChildByName("background");
		auto Boss = (Sprite*)background->getChildByName("Boss");
		Boss->resumeSchedulerAndActions();
	}
}

// get int-formated data from flush
int Catchmani::getintData(char* data, int defaultvalue)
{
	return UserDefault::getInstance()->getIntegerForKey(data, defaultvalue);
}

// save int-formated data at flush
void Catchmani::setintData(char* data, int value)
{
	if (strcmp(data, "money") == 0)
	{
		int money_de = value + 51521252;

		UserDefault::getInstance()->setIntegerForKey("money", value);
		UserDefault::getInstance()->setIntegerForKey("asdf4sdf2sdf", money_de);
		UserDefault::getInstance()->flush();
	}
	else
	{
		UserDefault::getInstance()->setIntegerForKey(data, value);
		UserDefault::getInstance()->flush();
	}
}

// Play Background Music Forever
void Catchmani::BackgroundMusic(float delta)
{
	// play background
	if (getintData("sound", 0) == 0)
		SimpleAudioEngine::getInstance()->playBackgroundMusic("sounds/Sunny Day.mp3");
}

// click sound
void Catchmani::sound_click()
{
	int sound_state = UserDefault::getInstance()->getIntegerForKey("sound", 0);
	if (sound_state == 0)
		float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/touch.wav");
}


///<summary>
/// <Java Native InterFace>
///  C++  ==>  Java
///</summary>
// Pop Up Advertise
void Catchmani::JavaFun_ad(int value)
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

// Kakaolink
void Catchmani::JavaFun_kakaolink(char* data1)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t
		, "org/cocos2dx/cpp/AppActivity"
		, "jniFunc4"
		, "(Ljava/lang/String;)V"))
	{
		///< 함수 호출
		jstring param1 = t.env->NewStringUTF(data1);
		t.env->CallStaticVoidMethod(t.classID, t.methodID, param1);
		///< Release
		t.env->DeleteLocalRef(t.classID);
		t.env->DeleteLocalRef(param1);
	}
#endif
}