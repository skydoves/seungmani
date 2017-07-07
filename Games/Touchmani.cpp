#include "Touchmani.h"
#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "datas.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;
using namespace CocosDenshion; // Sound library namespace
using namespace std;

///<summary>
/// <Java Native InterFace>
///  Java  ==>  c++
///</summary>

///<summary>
/// <Declare Global variables >
///</summary>
int static TouchEnable = 0; // 0 : Touch Enable,  1 : Touch Unable
int static TutorialState = 0;
float static HP, MP, Max_HP, Max_MP, HP_UP, MP_UP, Money_Get=1; //체/기력 , 회복계수, 얻는돈, 획득돈 계수
int static Power, Score = 0, SkillMP, Money, Defense, Coin = 0, Combo = 0, ComboTime, MaxCombo = 0;
int static MobState[200] = { 0 }, JVTI = (rand() % 1234567) + 12345, JkTI = 0; // 이 변수명은 가짜 변수명. Score 외부 변경을 막기 위한 변수


Scene* Touchmani::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Touchmani::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool Touchmani::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	// play background
	if (getintData("sound", 0) == 0)
	{
		SimpleAudioEngine::getInstance()->playBackgroundMusic("sounds/Sunny Day.mp3");
		this->schedule(schedule_selector(Touchmani::BackgroundMusic), 92.0);
	}
	srand(time(NULL));

	// add touch event listener at dispatcher
	listener1 = EventListenerTouchOneByOne::create();
	listener1->onTouchBegan = CC_CALLBACK_2(Touchmani::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener1, 1);

	// initialize graphics
	Init_Graphics();

	// show toutorial
	if (getintData("Game1_Tutorial", 0) == 0)
		Tutorial();
	else
	Init_Start();

	return true;
}

/*
 * @initialize
 */
void Touchmani::Init_Graphics()
{
	//Background image
	auto spr_bg = Sprite::create("img/background2.png");
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

	//Score Tool & Label
	auto spr_Tool_Time = Sprite::create("img/Tool_Score.png");
	spr_Tool_Time->setAnchorPoint(Point(0, 1));
	spr_Tool_Time->setPosition(Point(303, 315));
	this->addChild(spr_Tool_Time, 1);

	auto Label_Score = Label::createWithTTF(StringUtils::format("%d", Score), "fonts/Navi.ttf", 17);
	Label_Score->setAnchorPoint(Point(0, 1));
	Label_Score->setPosition(Point(335, 310)); Label_Score->setColor(Color3B(-255, -255, -255));
	this->addChild(Label_Score, 1, "Label_Score");
}


/*
* @initialize
*/
void Touchmani::Init_Start()
{
	//캐릭터1의 위치 설정
	auto char1 = Sprite::create("img/char1_Normal.png");
	char1->setAnchorPoint(Point(0, 1));
	char1->setPosition(Point(210, 100));
	char1->setScale(0.65);
	this->addChild(char1, 0, "Main_Char");

	//펫의 위치 설정
	if (getintData("MainPet", 0) != 0)
	{
		auto Pet = Sprite::create("img/Pet_dog1.png");
		Pet->setAnchorPoint(Point(0, 1));
		Pet->setPosition(Point(276, 75));
		Pet->setScale(0.8);
		this->addChild(Pet, 0, "Main_Pet");
		PetInit(); // 펫의 능력 초기화임
	}

	//캐릭터의 능력치를 설정한다 + 펫
	Set_CharAbility();

	//캐릭터1의 바운딩 박스 설정
	auto char_BoundingBox = Sprite::create("img/char_BoundingBox.png");
	char_BoundingBox->setAnchorPoint(Point(0, 1));
	char_BoundingBox->setPosition(Point(234, 100));
	char_BoundingBox->setScale(0.65); char_BoundingBox->setOpacity(0);
	this->addChild(char_BoundingBox, 0, "char_BoundingBox");

	//Make Close Menu
	auto item1 = MenuItemImage::create("img/icon_stop.png", "img/icon_stop.png", CC_CALLBACK_1(Touchmani::CloseButton, this));
	auto menu1 = Menu::create(item1, NULL);
	item1->setPosition(Point(216, 136));
	item1->setScale(0.9);
	this->addChild(menu1);

	//Make Skill & Item Icon
	auto item_Heal = MenuItemImage::create("img/Box_Heal.png", "img/Box_Heal.png", CC_CALLBACK_1(Touchmani::Heal, this));
	auto item_Skill = MenuItemImage::create("img/Box_Skill.png", "img/Box_Skill.png", CC_CALLBACK_1(Touchmani::Skill, this));
	auto menu_Buttons = Menu::create(item_Heal, item_Skill, NULL);
	item_Heal->setPosition(Point(-23, -144));
	item_Skill->setPosition(Point(31, -144));
	item_Heal->setScale(1.1); item_Skill->setScale(1.1);
	this->addChild(menu_Buttons);

	//Set Schedules
	this->schedule(schedule_selector(Touchmani::Update_UI_Status), 0.01); // 체력, 기력의 지속적 변동 UI 설정
	this->schedule(schedule_selector(Touchmani::Update_Char), 0.01);
	this->schedule(schedule_selector(Touchmani::Update_Mobs), 0.01);
	this->schedule(schedule_selector(Touchmani::Update_Mob1), 1.5);
	this->schedule(schedule_selector(Touchmani::Update_Mob2), 3.0);
	this->schedule(schedule_selector(Touchmani::Update_Mob3), 4.5);
	this->schedule(schedule_selector(Touchmani::Update_Mob4), 4.5);
	this->schedule(schedule_selector(Touchmani::Update_Mob5), 9.0);
	this->schedule(schedule_selector(Touchmani::Update_Mob6), 10.0);
	this->schedule(schedule_selector(Touchmani::update), 0.01); // 콤보 등 기타 시스템 업데이트
}


///<summary>
/// <몬스터들의 생성 업데이트>
///</summary>

//깡패 승마니
void Touchmani::Update_Mob1(float delta)
{
	float x = rand() % 2;
	float Move_Speed;

	//깡패 승마니1
	auto char1 = Sprite::create("img/char1_bad6.png");
	char1->setAnchorPoint(Point(0, 1));
	char1->setScale(0.65);
	this->addChild(char1, 1, "char1");

	//Set Position Randomly
	if (x == 0)
		char1->setPosition(Point(460, 100));
	else if (x == 1)
		char1->setPosition(Point(-40, 100));

	//Char HP Bar
	auto Mob_HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, 105, 20));
	Mob_HP_Bar->setScale(0.3);
	Mob_HP_Bar->setAnchorPoint(Point(0, 1));
	Mob_HP_Bar->setPosition(32, 105);
	char1->addChild(Mob_HP_Bar, 0, "Mob_HP_Bar");

    //Set Speed As Score
	if (6.0 - (Score / 30000) * 0.1 > 3.0)
		Move_Speed = 6.0 - (Score / 30000) * 0.1;
	else
		Move_Speed = 3.0;

	//Move
	if (x == 0)
	{
		auto Char_Move = MoveBy::create(Move_Speed, Point(-250, 0));
		auto Char_Move_Repeat = RepeatForever::create(Char_Move);
		char1->runAction(Char_Move_Repeat);
	}
	else if (x == 1)
	{
		auto Char_Move = MoveBy::create(Move_Speed, Point(250, 0));
		auto Char_Move_Repeat = RepeatForever::create(Char_Move);
		char1->runAction(Char_Move_Repeat);
	}

	//Set Mob HP
	char1->setUserData((char*)(int)(HP_Char1*((Score/700000+2)*0.5)));

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

//아저씨 승마니
void Touchmani::Update_Mob2(float delta)
{
	if (Score >= 25000)
	{
		float x = rand() % 2;
		float Move_Speed;

		//아저씨 승마니
		auto char1 = Sprite::create("img/char1_bad5.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setScale(0.65);
		this->addChild(char1, 1, "char2");

		//Set Position Randomly
		if (x == 0)
			char1->setPosition(Point(460, 100));
		else if (x == 1)
			char1->setPosition(Point(-40, 100));

		//Char HP Bar
		auto Mob_HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, 105, 20));
		Mob_HP_Bar->setScale(0.3);
		Mob_HP_Bar->setAnchorPoint(Point(0, 1));
		Mob_HP_Bar->setPosition(32, 105);
		char1->addChild(Mob_HP_Bar, 0, "Mob_HP_Bar");

		//Set Speed As Score
		if (5.0 - (Score / 50000) * 0.1 > 3.0)
			Move_Speed = 5.0 - (Score / 50000) * 0.1;
		else
			Move_Speed = 3.0;

		//Move
		if (x == 0)
		{
			auto Char_Move = MoveBy::create(Move_Speed, Point(-250, 0));
			auto Char_Move_Repeat = RepeatForever::create(Char_Move);
			char1->runAction(Char_Move_Repeat);
		}
		else if (x == 1)
		{
			auto Char_Move = MoveBy::create(Move_Speed, Point(250, 0));
			auto Char_Move_Repeat = RepeatForever::create(Char_Move);
			char1->runAction(Char_Move_Repeat);
		}

		//Set Mob HP
		char1->setUserData((char*)(int)(HP_Char2*((Score / 700000 + 2)*0.5)));

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
}

//복면 승마니
void Touchmani::Update_Mob3(float delta)
{
	if (Score >= 90000)
	{
		float x = rand() % 2;
		float Move_Speed;

		//깡패 승마니1
		auto char1 = Sprite::create("img/char1_bad4.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setScale(0.65);
		this->addChild(char1, 1, "char3");

		//Set Position Randomly
		if (x == 0)
			char1->setPosition(Point(460, 100));
		else if (x == 1)
			char1->setPosition(Point(-40, 100));

		//Char HP Bar
		auto Mob_HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, 105, 20));
		Mob_HP_Bar->setScale(0.3);
		Mob_HP_Bar->setAnchorPoint(Point(0, 1));
		Mob_HP_Bar->setPosition(32, 105);
		char1->addChild(Mob_HP_Bar, 0, "Mob_HP_Bar");

		//Set Speed As Score
		if (6.0 - (Score / 80000) * 0.1 > 3.0)
			Move_Speed = 5.0 - (Score / 80000) * 0.1;
		else
			Move_Speed = 3.0;

		//Move
		if (x == 0)
		{
			auto Char_Jump = JumpBy::create(Move_Speed, Point(-250, 0), 90, 8);
			auto Char_Jump_Repeat = RepeatForever::create(Char_Jump);
			char1->runAction(Char_Jump_Repeat);
		}
		else if (x == 1)
		{
			auto Char_Jump = JumpBy::create(Move_Speed, Point(250, 0), 90, 8);
			auto Char_Jump_Repeat = RepeatForever::create(Char_Jump);
			char1->runAction(Char_Jump_Repeat);
		}

		//Set Mob HP
		char1->setUserData((char*)(int)(HP_Char3*((Score / 700000 + 2)*0.5)));

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
}

//벌 승마니
void Touchmani::Update_Mob4(float delta)
{
	if (Score >= 220000)
	{
		float x = rand() % 2;
		float Move_Speed;

		//벌 승마니
		auto char1 = Sprite::create("img/char1_bad7.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setScale(0.65);
		this->addChild(char1, 1, "char4");

		//Set Position Randomly
		if (x == 0)
			char1->setPosition(Point(460, 250));
		else if (x == 1)
			char1->setPosition(Point(-40, 250));

		//Char HP Bar
		auto Mob_HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, 105, 20));
		Mob_HP_Bar->setScale(0.3);
		Mob_HP_Bar->setAnchorPoint(Point(0, 1));
		Mob_HP_Bar->setPosition(32, 105);
		char1->addChild(Mob_HP_Bar, 0, "Mob_HP_Bar");

		//Set Speed As Score
		if (12.0 - (Score / 50000) * 0.1 > 3.0)
			Move_Speed = 12.0 - (Score / 50000) * 0.1;
		else
			Move_Speed = 2.5;

		//Move
		if (x == 0)
		{
			auto Char_Move = MoveBy::create(Move_Speed, Point(-650, -250));
			auto Char_Move_Repeat = RepeatForever::create(Char_Move);
			char1->runAction(Char_Move_Repeat);
		}
		else if (x == 1)
		{
			auto Char_Move = MoveBy::create(Move_Speed, Point(650, -250));
			auto Char_Move_Repeat = RepeatForever::create(Char_Move);
			char1->runAction(Char_Move_Repeat);
		}

		//Set Mob HP
		char1->setUserData((char*)(int)(HP_Char4*((Score / 700000 + 2)*0.5)));

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
}

//두목 승마니
void Touchmani::Update_Mob5(float delta)
{
	if (Score >= 320000)
	{
		float x = rand() % 2;
		float Move_Speed;

		//두목 승마니
		auto char1 = Sprite::create("img/char1_bad1.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setScale(0.8);
		this->addChild(char1, 1, "char5");

		//Set Position Randomly
		if (x == 0)
			char1->setPosition(Point(460, 110));
		else if (x == 1)
			char1->setPosition(Point(-40, 110));

		//Char HP Bar
		auto Mob_HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, 105, 20));
		Mob_HP_Bar->setScale(0.3);
		Mob_HP_Bar->setAnchorPoint(Point(0, 1));
		Mob_HP_Bar->setPosition(32, 105);
		char1->addChild(Mob_HP_Bar, 0, "Mob_HP_Bar");

		//Set Speed As Score
		if (10.0 - (Score / 50000) * 0.1 > 3.0)
			Move_Speed = 10.0 - (Score / 50000) * 0.1;
		else
			Move_Speed = 3.0;

		//Move
		if (x == 0)
		{
			auto Char_Move = MoveBy::create(Move_Speed, Point(-250, 0));
			auto Char_Move_Repeat = RepeatForever::create(Char_Move);
			char1->runAction(Char_Move_Repeat);
		}
		else if (x == 1)
		{
			auto Char_Move = MoveBy::create(Move_Speed, Point(250, 0));
			auto Char_Move_Repeat = RepeatForever::create(Char_Move);
			char1->runAction(Char_Move_Repeat);
		}

		//Set Mob HP
		char1->setUserData((char*)(int)(HP_Char5*((Score / 700000 + 2)*0.5)));

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
}

//로빈 승마니
void Touchmani::Update_Mob6(float delta)
{
	if (Score >= 450000)
	{
		float x = rand() % 2;
		float Move_Speed;

		//로빈 승마니
		auto char1 = Sprite::create("img/char1_bad8.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setScale(0.6);
		this->addChild(char1, 1, "char6");

		//Set Position Randomly
		if (x == 0)
			char1->setPosition(Point(460, 95));
		else if (x == 1)
			char1->setPosition(Point(-40, 95));

		//Char HP Bar
		auto Mob_HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, 105, 20));
		Mob_HP_Bar->setScale(0.3);
		Mob_HP_Bar->setAnchorPoint(Point(0, 1));
		Mob_HP_Bar->setPosition(32, 105);
		char1->addChild(Mob_HP_Bar, 0, "Mob_HP_Bar");

		//Set Speed As Score
		if (4.0 - (Score / 50000) * 0.1 > 2.0)
			Move_Speed = 4.0 - (Score / 50000) * 0.1;
		else
			Move_Speed = 1.0;

		//Move
		if (x == 0)
		{
			auto Char_Jump = JumpBy::create(0.7, Point(0, 70), 80, 1);
			auto Char_Move = MoveBy::create(0.5, Point(-80, -70));
			auto Char_EaseOut = EaseOut::create(Char_Move, 1.0);
			auto Char_Delay = DelayTime::create(Move_Speed);
			auto Char_Sequence = Sequence::create(Char_Jump, Char_EaseOut, Char_Delay, NULL);
			auto Char_Move_Repeat = RepeatForever::create(Char_Sequence);
			char1->runAction(Char_Move_Repeat);
		}
		else if (x == 1)
		{
			auto Char_Jump = JumpBy::create(0.7, Point(0, 70), 80, 1);
			auto Char_Move = MoveBy::create(0.5, Point(80, -70));
			auto Char_EaseOut = EaseOut::create(Char_Move, 1.0);
			auto Char_Delay = DelayTime::create(Move_Speed);
			auto Char_Sequence = Sequence::create(Char_Jump, Char_EaseOut, Char_Delay, NULL);
			auto Char_Move_Repeat = RepeatForever::create(Char_Sequence);
			char1->runAction(Char_Move_Repeat);
		}

		//Set Mob HP
		char1->setUserData((char*)(int)(HP_Char6*((Score / 700000 + 2)*0.5)));

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
}

//모든 몬스터의 지속적인 상태를 관리한다. + 아이템의 드랍 & 제거
void Touchmani::Update_Mobs(float delta)
{
	///<summary>
	/// <몬스터들의 사망처리 업데이트>
	///</summary>

	//몬스터
	for (Sprite* Spr_Mob1 : Mob1)
	{
		//Update HP Bar When Mob Alive
		if ((int)Spr_Mob1->getUserData() != 1000000)
		{
			auto HP_Bar = (Sprite*)Spr_Mob1->getChildByName("Mob_HP_Bar");
			Spr_Mob1->removeChild(HP_Bar, true);

			int Mob_HP;

			//Set HP Bar Length AS Mob HP
			if (Spr_Mob1->getName().compare("char1") == 0)
				Mob_HP = HP_Char1;
			else if (Spr_Mob1->getName().compare("char2") == 0)
				Mob_HP = HP_Char2;
			else if (Spr_Mob1->getName().compare("char3") == 0)
				Mob_HP = HP_Char3;
			else if (Spr_Mob1->getName().compare("char4") == 0)
				Mob_HP = HP_Char4;
			else if (Spr_Mob1->getName().compare("char5") == 0)
				Mob_HP = HP_Char5;
			else if (Spr_Mob1->getName().compare("char6") == 0)
				Mob_HP = HP_Char6;

			HP_Bar = CCSprite::create("img/HP_Bar.png", CCRectMake(0, 0, (105 * (int)(Spr_Mob1->getUserData())) / Mob_HP, 20));
			HP_Bar->setPosition(32, 105);
			HP_Bar->setAnchorPoint(Point(0, 1));
			HP_Bar->setScale(0.3);
			Spr_Mob1->addChild(HP_Bar, 0, "Mob_HP_Bar");
		}

		//Die Mob
		if ((int)(Spr_Mob1->getUserData()) <= 0 && (int)(Spr_Mob1->getUserData()) != 1000000)
		{
			//Get Score
			if (Spr_Mob1->getName().compare("char1") == 0)
			{
				Score += EXP_Char1*(Score / 700000 + 1);
				MobState[32] += EXP_Char1*(Score / 700000 + 1) + JVTI;
				JkTI++;
				Cal_FakeScore(EXP_Char1*(Score / 700000 + 1)); // 외부에서 Score값을 임의로 변경할경우를 대비
			}
			else if (Spr_Mob1->getName().compare("char2") == 0)
			{
				Score += EXP_Char2*(Score / 700000 + 1);
				MobState[32] += EXP_Char2*(Score / 700000 + 1) + JVTI;
				JkTI++;
				Cal_FakeScore(EXP_Char2*(Score / 700000 + 1) + JVTI);
			}
			else if (Spr_Mob1->getName().compare("char3") == 0)
			{
				Score += EXP_Char3*(Score / 700000 + 1);
				MobState[32] += EXP_Char3*(Score / 700000 + 1) + JVTI;
				JkTI++;
				Cal_FakeScore(EXP_Char3*(Score / 700000 + 1) + JVTI);
			}
			else if (Spr_Mob1->getName().compare("char4") == 0)
			{
				Score += EXP_Char4*(Score / 700000 + 1);
				MobState[32] += EXP_Char4*(Score / 700000 + 1) + JVTI;
				JkTI++;
				Cal_FakeScore(EXP_Char4*(Score / 700000 + 1) + JVTI);
			}
			else if (Spr_Mob1->getName().compare("char5") == 0)
			{
				Score += EXP_Char5*(Score / 700000 + 1);
				MobState[32] += EXP_Char5*(Score / 700000 + 1) + JVTI;
				JkTI++;
				Cal_FakeScore(EXP_Char5*(Score / 700000 + 1) + JVTI);
			}
			else if (Spr_Mob1->getName().compare("char6") == 0)
			{
				Score += EXP_Char6*(Score / 700000 + 1);
				MobState[32] += EXP_Char6*(Score / 700000 + 1) + JVTI;
				JkTI++;
				Cal_FakeScore(EXP_Char6*(Score / 700000 + 1) + JVTI);
			}

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
				Coin->setPosition(Point(Spr_Mob1->getPositionX(), Spr_Mob1->getPositionY()-68));
				this->addChild(Coin, 1, "Coin");
				Coin->setUserData((char*)"Coin");

				// Drop Action
				auto action_Jump = JumpTo::create(0.6, Point(Spr_Mob1->getPositionX(), 34), 60, 1);
				Coin->runAction(action_Jump);

				Item.pushBack(Coin);
			}

			// 하트 아이템 드랍
			else if (Rand < 55)
			{
				auto Heart = Sprite::create("img/Icon_heart.png");
				Heart->setAnchorPoint(Point(1, 0));
				Heart->setPosition(Point(Spr_Mob1->getPositionX(), Spr_Mob1->getPositionY() - 68));
				this->addChild(Heart, 1, "Heart");
				Heart->setUserData((char*)"Heart");

				// Drop Action
				auto action_Jump = JumpTo::create(0.6, Point(Spr_Mob1->getPositionX(), 34), 60, 1);
				Heart->runAction(action_Jump);

				Item.pushBack(Heart);
			}

			// 기력 아이템 드랍
			else if (Rand < 65)
			{
				auto Skill_energy = Sprite::create("img/Icon_skill.png");
				Skill_energy->setAnchorPoint(Point(1, 0));
				Skill_energy->setPosition(Point(Spr_Mob1->getPositionX(), Spr_Mob1->getPositionY() - 68));
				this->addChild(Skill_energy, 1, "Skill_energy");
				Skill_energy->setUserData((char*)"Skill_energy");

				// Drop Action
				auto action_Jump = JumpTo::create(0.6, Point(Spr_Mob1->getPositionX(), 34), 60, 1);
				Skill_energy->runAction(action_Jump);

				Item.pushBack(Skill_energy);
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
		if ((int)Spr_Mob1->getUserData() == 1000000 && Spr_Mob1->getActionByTag(0) == NULL)
		{
			Mob1.eraseObject(Spr_Mob1, true);
			this->removeChild(Spr_Mob1);
			break;
		}
	}

	// 아이템 제거
	for (Sprite* Spr_Item : Item)
	{
		if ((char*)(Spr_Item->getUserData()) == "Touched" && Spr_Item->getActionByTag(0) == NULL)
		{
			Item.eraseObject(Spr_Item, true);
			this->removeChild(Spr_Item);
			break;
		}
	}
}

// 콤보의 초기화 등 기타 시스템 업데이트
void Touchmani::update(float delta)
{
	int NowTime = time(NULL);

	if (NowTime - ComboTime >= 3)
	{
		auto Spr_Combo = (Sprite*)this->getChildByName("Combo");
		this->removeChild(Spr_Combo);
		Combo = 0;
	}

	// 도둑냥은 지속적으로 돈을 회수한다.
	if (getintData("MainPet", 0) == 102)
	{
		for (Sprite* Spr_Item : Item)
		{
			if ((char*)(Spr_Item->getUserData()) == "Coin")
			{
				auto Spr_MainChar = (Sprite*)this->getChildByName("Main_Pet");
				Coin += 3;

				auto Item_Jump = JumpTo::create(0.8, Point(Spr_MainChar->getPositionX() + 20, Spr_MainChar->getPositionY() - 20), 35, 1);
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
}

void Touchmani::PetInit()
{
	// 도둑냥은 update schedule에 있다.

	// 페르냥
	if (getintData("MainPet", 0) == 103)
		this->schedule(schedule_selector(Touchmani::PetSkill1), 5.0);

	// 백호당
	else if (getintData("MainPet", 0) == 104)
		this->schedule(schedule_selector(Touchmani::PetSkill2), 5.0);
}

// 페르냥 스킬
void Touchmani::PetSkill1(float delta)
{
	for (Sprite* Spr_Mob1 : Mob1)
	{
		//몹 체력의 감소
		Spr_Mob1->setUserData((char*)((int)(Spr_Mob1->getUserData()) - (int)(Power/2)));
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

// 백호당 스킬
void Touchmani::PetSkill2(float delta)
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


//터치를 하는경우
bool Touchmani::onTouchBegan(Touch *touch, Event *unused_event)
{
	if (TouchEnable == 0)
	{
		Point location = touch->getLocation();
		Rect Mobs_Rect;

		//몬스터들
		for (Sprite* Spr_Mob1 : Mob1)
		{
			Mobs_Rect = Spr_Mob1->getBoundingBox();

			if (Mobs_Rect.containsPoint(location) && (int)(Spr_Mob1->getUserData()) != 1000000)
			{
				int Rand;
				Rand = rand() % 100 + 1;

				//몹 체력의 감소
				if (Rand <=95)
				  Spr_Mob1->setUserData((char*)((int)(Spr_Mob1->getUserData()) - Power));

				// 크리티컬
				else
				{
					Spr_Mob1->setUserData((char*)((int)(Spr_Mob1->getUserData()) - Power*2));

					if ((Sprite*)this->getChildByName("Critical"))
					{
						auto Spr_critical = (Sprite*)this->getChildByName("Critical");
						this->removeChild(Spr_critical);
					}
					auto Critical = Label::createWithTTF(StringUtils::format("크리티컬!"), "fonts/Navi.ttf", 13);
					Critical->setAnchorPoint(Point(0, 1));
					Critical->setPosition(Point(Spr_Mob1->getPositionX() + 10, Spr_Mob1->getPositionY())); 
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
						if (Spr_Mob1->getPositionX() < 210)
						{
							auto Mob_Jump = JumpBy::create(0.2, Point(-20, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
						}
						else
						{
							auto Mob_Jump = JumpBy::create(0.2, Point(20, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
						}
						Spr_Mob1->resumeSchedulerAndActions();
					}

					//승소녀
					else if (getintData("MainChar", 0) == 6)
					{
						int Skill_Rand = rand() % 100 + 1;

						if (Skill_Rand <= 10)
						{
							if (Spr_Mob1->getPositionX() < 210)
							{
								auto Mob_Jump = JumpBy::create(0.2, Point(-70, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
							}
							else
							{
								auto Mob_Jump = JumpBy::create(0.2, Point(70, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
							}
							Spr_Mob1->resumeSchedulerAndActions();
						}
						else
						{
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
							if (Spr_Mob1->getPositionX() < 210)
							{
								auto Mob_Jump = JumpBy::create(0.2, Point(-15, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
							}
							else
							{
								auto Mob_Jump = JumpBy::create(0.2, Point(15, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
							}
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
						if (Spr_Mob1->getPositionX() < 210)
						{
							auto Mob_Jump = JumpBy::create(0.2, Point(-15, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
						}
						else
						{
							auto Mob_Jump = JumpBy::create(0.2, Point(15, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump);
						}

						auto Delay = DelayTime::create(0.8);
						auto this_Func1 = CallFunc::create(CC_CALLBACK_0(Touchmani::DisposeStrun, this, this));
						auto Sequence1 = Sequence::create(Delay, this_Func1, NULL);
						this->runAction(Sequence1);
					}
					else
					{
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
				Hit_Mobs("particles/particle_Hit1.plist", location, 0);
			}
		}

		// 아이템을 터치 할 경우
		for (Sprite* Spr_Item : Item)
		{
			Mobs_Rect = Spr_Item->getBoundingBox();
			if (Mobs_Rect.containsPoint(location))
			{
				// 돈을 터치한 경우
				if ((char*)(Spr_Item->getUserData()) == "Coin")
				{
					auto Spr_MainChar = (Sprite*)this->getChildByName("char_BoundingBox");
					Coin += 5;

					auto Item_Jump = JumpTo::create(0.8, Point(Spr_MainChar->getPositionX() + 20, Spr_MainChar->getPositionY() - 20), 35, 1);
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
					auto Item_Move = MoveTo::create(1.2, Point(50, 300));
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
					auto Item_Move = MoveTo::create(1.2, Point(200, 300));
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

	return true;
}

//몹들과 부딪히는 경우 (캐릭터의 지속적인 상태)
void Touchmani::Update_Char(float delta)
{
	auto Spr_MainChar = (Sprite*)this->getChildByName("char_BoundingBox");
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
			else if (Damage*3 < 25)
				HP -= Damage*3;
			else
				HP -= 25;

			//Sound & Particle
			Hit_Mobs("particles/particle_Hit1.plist", location, 1);

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

	auto BackGround = (Sprite*)this->getChildByName("background");


	if (Score >= 700000 & Score < 1400000)
		BackGround->setTexture("img/background2_C1.png");
	else if (Score >= 1400000 & Score < 2100000)
		BackGround->setTexture("img/background2_C2.png");
	else if (Score >= 2100000 & Score < 2800000)
		BackGround->setTexture("img/background2_C3.png");
	else if (Score >= 2800000 & Score < 3500000)
		BackGround->setTexture("img/background2_C4.png");

	//캐릭터의 체력이 0 이하로 떨어졌을때
	if (HP <= 0)
	{
		//게임 보상
		auto Game_Over = CallFunc::create(CC_CALLBACK_0(Touchmani::Game_Over, this, this));
		this->runAction(Game_Over);
	}
}

//몹을 때릴경우 사운드, 이펙트를 처리한다
void Touchmani::Hit_Mobs(char* particle, Point location, int sound)
{
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
	Particle->setPosition(location);
	Particle->setOpacity(225);
	this->addChild(Particle, 100);
}

// 체력, 기력의 지속적 변동 UI 설정
void Touchmani::Update_UI_Status(float delta)
{
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

	//Update Score Label
	auto Label_Score = (Label*)this->getChildByName("Label_Score");
	Label_Score->setString(StringUtils::format("%d", Score));

	//Update HP & MP
	if (HP + HP_UP * 0.02 <= Max_HP)
	HP += HP_UP * 0.02;

	if (MP + MP_UP * 0.04 <= Max_MP)
	MP += MP_UP * 0.04;
}

//캐릭터의 능력치 설정
void Touchmani::Set_CharAbility()
{
	// 승마니
	if (getintData("MainChar", 0) == 0)
	{
		Power = char1_Normal_Power + getintData("Stat_Char_Poewr", 0)*5, Defense = getintData("Stat_Char_Defense", 0)*2;
		Max_HP = char1_Normal_HP + getintData("Stat_Char_HP", 0) * 10; HP = Max_HP; HP_UP = char1_Normal_HP_UP + (getintData("Stat_Char_HPUP", 0))*0.1;
		Max_MP = char1_Normal_MP + getintData("Stat_Char_MP", 0)*10; MP = 0; MP_UP = char1_Normal_MP_UP + (getintData("Stat_Char_MPUP", 0))*0.1;
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



//Use Skill
void Touchmani::Skill(Ref* pSender)
{
	if (TouchEnable == 0)
	{
		Point location;

		sound_click();

		//몬스터들에 대해서 스킬 사용
		if (MP >= SkillMP)
		{
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
			/// <스킬 사용시 말풍선>
			///</summary>
			//노말 승마니
			if (getintData("MainChar", 0) == 0)
				RandomBubble(" 얍!! 다맞아랏!", "  필살기!!", "아껴둔 궁극 스킬이닷!!");

			//승고딩
			else if (getintData("MainChar", 0) == 1)
				RandomBubble(" 고3의 분노닷!", "  정석으로 맞아랏!!", "니들이 미분을 알아!?");

			//승허렁
			else if (getintData("MainChar", 0) == 2)
				RandomBubble(" 아오~~!!", "  뎀벼!!", "니넨다 주것어!!");

			//승주영
			else if (getintData("MainChar", 0) == 3)
				RandomBubble(" 몸통박치기!", "  필살기!!", "이엽!");

			//승잔나
			else if (getintData("MainChar", 0) == 4)
				RandomBubble(" 바람아 불어랏~!", "  후~~~", "바람바람!@!");

			//승도남
			else if (getintData("MainChar", 0) == 5)
				RandomBubble(" 아오 저리 비켜!", "  내 앞에서 나왓!", "비켜 =_=");

			//승소녀
			else if (getintData("MainChar", 0) == 6)
				RandomBubble(" 얍얍!!", "  540도 발차기!", "간닷~!!");

			//승법사
			else if (getintData("MainChar", 0) == 7)
				RandomBubble(" 썬더 빔!", "  맞아랏!", "번개번개@@!");

			//승혈귀
			else if (getintData("MainChar", 0) == 8)
				RandomBubble(" 곧 정월 대보름이야..", "  피좀 다오..", "흐아압~!");

			//승수르
			else if (getintData("MainChar", 0) == 9)
				RandomBubble(" 돈은 아픈거야수르", "  돈 별거없다수르", "돈빛이 내린다~~샤랄라");

			//승아리
			else if (getintData("MainChar", 0) == 10)
				RandomBubble("내 사랑을 받아랏~!", " >< 나에게로 와요!", " 매혹!!");

			//승의사
			else if (getintData("MainChar", 0) == 11)
				RandomBubble("사람은 건강해야돼..!", " 어이, 매쓰 줘!", " 다 고쳐드립니다!");

			//승의탑
			else if (getintData("MainChar", 0) == 12)
				RandomBubble("신수흐름제어!", " 타임타임~><", " 무궁화 꽃이 피었습니다!");

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

						//몹 체력의 감소
						Spr_Mob1->setUserData((char*)((int)Spr_Mob1->getUserData() - 75));

						Spr_Mob1->pauseSchedulerAndActions();
						if (Spr_Mob1->getPositionX() < 210) { auto Mob_Jump = JumpBy::create(0.2, Point(-40, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump); }
						else{ auto Mob_Jump = JumpBy::create(0.2, Point(40, 0), 30, 1); Spr_Mob1->runAction(Mob_Jump); }
						Spr_Mob1->resumeSchedulerAndActions();

						//Particle
						Hit_Mobs("particles/particle_Hit1.plist", location, 100);
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
						Hit_Mobs("particles/particle_Hit1.plist", location, 10000);
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
						auto this_Func1 = CallFunc::create(CC_CALLBACK_0(Touchmani::DisposeStrun, this, this));
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
			auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 135, 160, 2.4, " 기력이 부족해..", true, false));
			this->runAction(char_Ballon);
		}
	}
}

//몬스터의 스턴을 해제한다
void Touchmani::DisposeStrun(Ref* pSender)
{
	for (Sprite* Spr_Mob1 : Mob1)
	{
		Spr_Mob1->resumeSchedulerAndActions();
	}
}

//3개중 랜덤으로 말이 나오도록 한다
void Touchmani::RandomBubble(const char* say1, const char* say2, const char* say3)
{
	float x = rand() % 3;
	if (x == 0)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 135, 160, 2.4, say1, true, false));
		this->runAction(char_Ballon);
	}
	else if (x == 1)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 135, 160, 2.4, say2, true, false));
		this->runAction(char_Ballon);
	}
	else if (x == 2)
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 135, 160, 2.4, say3, true, false));
		this->runAction(char_Ballon);
	}
}


//Main Char 번호에 맞는 캐릭터 이미지를 반환한다
char* Touchmani::Get_CharImage(int value)
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


//포션 아이템을 사용한 경우
void Touchmani::Heal(Ref* pSender)
{
	if (TouchEnable == 0)
	{
		sound_click();

		if (getintData("Potison", 0) == 0) // 물약이 부족한경우
		{
			auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 135, 160, 2.4, " 물약이 부족하네 ㅠㅠ", true, false));
			this->runAction(char_Ballon);
		}
		else
		{
			//Sound
			if (getintData("sound", 0) == 0)
				float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Potison.wav");

			setintData("Potison", 0);
			HP = Max_HP;
			MP = Max_MP;

			//특수 효과
			auto Spr_MainChar = (Sprite*)this->getChildByName("char_BoundingBox");
			auto location = Spr_MainChar->getPosition();

			auto Particle = ParticleSystemQuad::create("particles/Potison.plist");
			Particle->setScale(0.8);
			Particle->setPosition(location);
			this->addChild(Particle, 2);

			auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 135, 160, 2.4, "        와우~!!", true, false));
			this->runAction(char_Ballon);
		}
	}
}

//게임 데이터를 초기화 한다
void Touchmani::Game_init()
{
	//Stop All Actions and Schedules
	this->pauseSchedulerAndActions();
	for (Sprite* Spr_Mob1 : Mob1)
	{ Spr_Mob1->pauseSchedulerAndActions(); }

	//Data init
	TouchEnable = 0;
	Score = 0;
	Mob1.clear();
	Mobs_HP_Bar.clear();
	Item.clear();
	Coin = 0;
	Combo = 0;
	MaxCombo = 0;
	JVTI = (rand() % 1234567) + 12345;
	JkTI = 0;
	for (int i = 0; i < 200; i++)
		MobState[i] = 0;

	//터치이벤트 리스너 기능을 확실히 제거
	this->setTouchEnabled(false);
	_eventDispatcher->removeEventListenersForType(EventListener::Type::TOUCH_ONE_BY_ONE);
	Director::getInstance()->getEventDispatcher()->removeEventListener(listener1);
}

// 외부 Score 변경에 대한 실시간 처리 
void Touchmani::Cal_FakeScore(int PlusScore)
{
	for (int i = 0; i < 200; i++)
	{
		if (i != 32)
		MobState[i] = PlusScore;
	}
}

//게임이 끝난경우
void Touchmani::Game_Over(Ref* pSender)
{
		//진행중인 게임 멈춤
		TouchEnable = 1;
		//Stop All Actions and Schedules
		this->pauseSchedulerAndActions();
		for (Sprite* Spr_Mob1 : Mob1)
		{
			Spr_Mob1->pauseSchedulerAndActions();
		}

		if (Score == MobState[32] - JVTI*JkTI || Score <= MobState[32] + EXP_Char6*(Score / 700000 + 1)) // 외부의 변수 조작이 없었는가를 판별, 최대 오차 범위
		{
			//Background image
			auto spr_bg = Sprite::create("img/background_black.png");
			spr_bg->setAnchorPoint(Point(0, 1));
			spr_bg->setPosition(Point(0, 320));
			spr_bg->setOpacity(160);
			this->addChild(spr_bg, 10);

			//Make Advertise
			JavaFun_ad(5);

			//Buttons
			auto item1 = MenuItemImage::create("img/GameOver_Menu.png", "img/GameOver_Menu.png", CC_CALLBACK_1(Touchmani::Ask_Exit, this, 0));
			auto item2 = MenuItemImage::create("img/Button_Right.png", "img/Button_Right.png", CC_CALLBACK_1(Touchmani::Ask_Exit, this, 1));
			auto item3 = MenuItemImage::create("img/btn_kakao.png", "img/btn_kakao.png", CC_CALLBACK_1(Touchmani::KakaoLink, this));
			auto menu = Menu::create(item1, item2, item3, NULL);
			item2->setPosition(Point(70, -65));
			item3->setPosition(Point(-40, -65));
			item3->setScale(0.9);
			this->addChild(menu, 10);

			//게임의 보상*
			Money = (int)(Money_Get*((float)Score / 300)) + Coin;
			setintData("money", getintData("money", 0) + Money);
			Score += MaxCombo * 500; // 최대 콤보 점수 

			//Label - High Score
			auto label_HighScore = Label::createWithTTF(StringUtils::format("%d 점", getintData("Game1_HighScore", 0)), "fonts/Navi.ttf", 15);
			label_HighScore->setAnchorPoint(Point(0, 1));
			label_HighScore->setPosition(Point(220, 170)); label_HighScore->setColor(Color3B(-255, -255, -255));
			this->addChild(label_HighScore, 10);

			//Label - Score
			auto label_Score = Label::createWithTTF(StringUtils::format("%d", Score), "fonts/Navi.ttf", 19);
			label_Score->setAnchorPoint(Point(0, 1));
			label_Score->setPosition(Point(220, 205)); label_Score->setColor(Color3B(-255, -255, -255));
			this->addChild(label_Score, 10);

			//Label - Get Money
			auto label_GetMoney = Label::createWithTTF(StringUtils::format("%d 마니(+%d)", Money, Coin), "fonts/Navi.ttf", 15);
			label_GetMoney->setAnchorPoint(Point(0, 1));
			label_GetMoney->setPosition(Point(220, 135)); label_GetMoney->setColor(Color3B(-255, -255, -255));
			this->addChild(label_GetMoney, 10);

			//최고 점수의 설정
			if (Score > getintData("Game1_HighScore", 0))
			{
				//Set Score Data
				setintData("Game1_HighScore", Score);

				//Imgage
				auto spr_bg = Sprite::create("img/Spr_HighScore.png");
				spr_bg->setAnchorPoint(Point(0, 1));
				spr_bg->setPosition(Point(0, 320)); spr_bg->setOpacity(0);
				this->addChild(spr_bg, 10);

				auto spr_FadeIn = FadeIn::create(2.0);
				spr_bg->runAction(spr_FadeIn);

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

				//JnI - Score Regist
				string Datas;
				Datas = to_string(getintData("MainChar", 0));
				Datas += "&";
				Datas += to_string(Score);
				Datas += "&";
				Datas += UserDefault::getInstance()->getStringForKey("Name", "*1^&-_$@");

				const char * Data = Datas.c_str();

				JavaFun((char*)Data);
			}
		}

		// 불법 프로그램 사용으로 인한 Score 조작
		else
		{
			setintData("asdfasd", 14215251);
			MessageBox("불법 프로그램 사용으로 인해 신고처리 접수되었습니다.", "<위험> 다음 메세지는 핸드폰의 위치와 일부 정보를 수집합니다.");
			Director::getInstance()->end();
		}
}

void Touchmani::KakaoLink(Ref* pSender)
{
	sound_click();

	// Jni - kakao link
	string Datas_kakao;
	Datas_kakao = UserDefault::getInstance()->getStringForKey("Name", "*1^&-_$@");
	Datas_kakao += "&";
	Datas_kakao += to_string(Score);

	const char * Data_kakao = Datas_kakao.c_str();
	JavaFun_kakaolink((char*)Data_kakao);
}

//게임 설명 튜토리얼
void Touchmani::Tutorial()
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
		char1->setPosition(Point(210, 105));
		char1->setScale(0.7);
		this->addChild(char1, 0, "char1");

		//가짜 Skill & Item icon 이미지 설정
		auto Fake_Heal = Sprite::create("img/Box_Heal.png");
		Fake_Heal->setAnchorPoint(Point(0, 1));
		Fake_Heal->setPosition(Point(200, 31)); Fake_Heal->setScale(1.1);
		this->addChild(Fake_Heal, 0, "Fake_Heal");

		auto Fake_Skill = Sprite::create("img/Box_Skill.png");
		Fake_Skill->setAnchorPoint(Point(0, 1));
		Fake_Skill->setPosition(Point(254, 31)); Fake_Skill->setScale(1.1);
		this->addChild(Fake_Skill, 0, "Fake_Skill");

		//화면의 페이드 아웃
		auto Action_FadeIn = FadeOut::create(5.0);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 135, 160, 2.4, "         어라..?", true, false));
		auto cahr1_Sequence1 = Sequence::create(Action_FadeIn, char_Ballon, NULL);
		Black_background->runAction(cahr1_Sequence1);
	}
	else if (TutorialState == 1) // char1
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 135, 160, 2.4, "분명 친구랑 있었는데..", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 2) // char1
	{
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 135, 160, 2.4, "여긴 어디지;;?", true, false));
		this->runAction(char_Ballon);
	}
	else if (TutorialState == 3) // char1
	{
		//캐릭터1의 위치 설정
		auto char1 = Sprite::create("img/char1_bad6.png");
		char1->setAnchorPoint(Point(0, 1));
		char1->setPosition(Point(360, 105));
		char1->setScale(0.7);
		this->addChild(char1, 0, "object_01");

		auto char_delay = DelayTime::create(1.5);
		auto char_Ballon = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 135, 160, 2.4, "헉.. 저 사람은 뭐야..?", true, false));
		auto char_Sequence = Sequence::create(char_delay, char_Ballon, NULL);
		this->runAction(char_Sequence);
	}
	else if (TutorialState == 4) // char1
	{
		auto Info_delay = DelayTime::create(4.5);
		auto Info_Ballon1 = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 2, 1.0, 10, 280, 3.0, " 승마니의 체력이에요", true, true));
		auto Info_Ballon2 = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 2, 1.0, 130, 280, 3.0, "승마니의 기력!\n스킬 사용에 필요해요", true, true));
		auto Info_Ballon3 = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 2, 1.0, 270, 280, 3.0, " 게임에서 획득한 점수!", true, true));
		auto Info_Ballon4 = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 2, 1.0, 110, 70, 3.0, " 구매한 회복약. 한번만\n   사용할수 있어요", true, false));
		auto Info_Ballon5 = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 2, 1.0, 260, 70, 3.0, " 스킬이에요.캐릭터\n     마다 달라요", false, false));
		auto Info_Ballon6 = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 300, 160, 3.0, "난 현실을 이탈한 승마니", true, false));
		auto Info_Ballon7 = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 300, 160, 3.0, "날 터치하면 공격할수\n     있지", true, false));
		auto Info_Ballon8 = CallFunc::create(CC_CALLBACK_0(Touchmani::SpeechBubble, this, this, 0, 1.0, 300, 160, 3.0, "덤벼라 승마나!!", true, false));
		auto Info_Sequence = Sequence::create(Info_Ballon1, Info_delay, Info_Ballon2, Info_delay, Info_Ballon3, Info_delay, Info_Ballon4, Info_delay, Info_Ballon5, Info_delay, Info_Ballon6, Info_delay, Info_Ballon7, Info_delay, Info_Ballon8, NULL);
		this->runAction(Info_Sequence);
	}
	else if (TutorialState == 12) // char1
	{
		auto char1 = (Sprite*)this->getChildByName("char1");
		auto char2 = (Sprite*)this->getChildByName("object_01");
		auto spr1 = (Sprite*)this->getChildByName("Fake_Heal");
		auto spr2 = (Sprite*)this->getChildByName("Fake_Skill");
		this->removeChild(char1); this->removeChild(char2); this->removeChild(spr1); this->removeChild(spr2);
		setintData("Game1_Tutorial", 1);
		Init_Start();
	}
}

//말풍선을 띄운다   => 색상, 크기, x좌표, y좌표, 시간, 내용, x대칭, y대칭
void Touchmani::SpeechBubble(Ref* pSender, int color, double scale, int x, int y, double time, const char* mess, bool flipX, bool flipY)
{
	//Set SpeechBubble Image
	auto spr_Bubble = Sprite::create("img/Saybubble_Orange.png");

	if (color == 1)
		spr_Bubble = Sprite::create("img/Saybubble_SkyBlue.png");
	else if(color == 2)
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
	this->addChild(label_Bullble, 0, "Label_Ballon");

	auto label_Bullble_FadeIn = FadeIn::create(1.0);
	auto label_Bullble_FadeOut = FadeOut::create(0.7);
	auto label_DelyTime1 = DelayTime::create(time);
	auto label_Func1 = CallFunc::create(CC_CALLBACK_0(Touchmani::DeleteBubble, this, this));
	auto label_Sequence = Sequence::create(label_Bullble_FadeIn, label_DelyTime1, label_Bullble_FadeOut, label_Func1, NULL);
	label_Bullble->runAction(label_Sequence);
}

//말풍선을 지운다
void Touchmani::DeleteBubble(Ref* pSender)
{
	auto Ballon = (Sprite*)this->getChildByName("Ballon");
	auto Label_Ballon = (Sprite*)this->getChildByName("Label_Ballon");
	this->removeChild(Ballon);
	this->removeChild(Label_Ballon);

	//튜토리얼 관련

	if (getintData("Game1_Tutorial", 0) == 0)
	{
		TutorialState++;
		Tutorial();
	}
}

//로비로 이동 버튼
void Touchmani::CloseButton(Ref* pSender)
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
			auto item1 = MenuItemImage::create("img/QuestionBoard1.png", "img/QuestionBoard1.png", CC_CALLBACK_1(Touchmani::Ask_Exit, this, 0));
			auto item2 = MenuItemImage::create("img/Button_okay.png", "img/Button_okay.png", CC_CALLBACK_1(Touchmani::Ask_Exit, this, 1));
			auto item3 = MenuItemImage::create("img/Button_no.png", "img/Button_no.png", CC_CALLBACK_1(Touchmani::Ask_Exit, this, 2));
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
			{ Spr_Mob1->pauseSchedulerAndActions(); }
	}
}

//종료 할건지에 대한 버튼을 누를경우
void Touchmani::Ask_Exit(Ref* pSender, int value)
{
	//Exit Game
	if (value == 1)
	{
		Game_init();
		JavaFun_ad(6); // 광고를 띄운다

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
		{ Spr_Mob1->resumeSchedulerAndActions(); }
	}
}

//배경음악
void Touchmani::BackgroundMusic(float delta)
{
	//배경음악 설정
	if (getintData("sound", 0) == 0)
		SimpleAudioEngine::getInstance()->playBackgroundMusic("sounds/Sunny Day.mp3");
}

//int형 데이터를 cocos 저장소에서 가져온다
int Touchmani::getintData(char* data, int defaultvalue)
{
	return UserDefault::getInstance()->getIntegerForKey(data, defaultvalue);
}

//int형 데이터를 cocos 저장소에 저장한다
void Touchmani::setintData(char* data, int value)
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

// Click sound
void Touchmani::sound_click()
{
	int sound_state = UserDefault::getInstance()->getIntegerForKey("sound", 0);
	if (sound_state == 0)
		float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/touch.wav");
}


///<summary>
/// <Java Native InterFace>
///  C++  ==>  Java
///</summary>

/*JniHelper::getStaticMethodInfo
boolean	Z
byte	B
char	C
short	S
int  	I
long	J
float	F
double	D
void	V
Object	Ljava/lang/String; L / full path segmentation class
Array	[Ljava/lang/String; [signature [I
*/

void Touchmani::JavaFun(char* data1)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t
		, "org/cocos2dx/cpp/AppActivity"
		, "jniFunc2"
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

void Touchmani::JavaFun_kakaolink(char* data1)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	/**
	JniHelper를 통해 org/cocos2dx/application/에 있는 ApplicationDemo class의 JavaJniTestFunc함수 정보를 가져온다.
	*/
	if (JniHelper::getStaticMethodInfo(t
		, "org/cocos2dx/cpp/AppActivity"
		, "jniFunc3"
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

void Touchmani::JavaFun_ad(int value)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	/**
	JniHelper를 통해 org/cocos2dx/application/에 있는 ApplicationDemo class의 JavaJniTestFunc함수 정보를 가져온다.
	*/
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
