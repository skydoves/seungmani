
#include "HelloWorldScene.h"
#include "MainScene.h"
#include "SimpleAudioEngine.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;
using namespace CocosDenshion;
using namespace std;

int Bounce_Status = 0;
int gamestate = 0;

///<summary>
/// <Java Native InterFace>
///  Java  ==>  c++
///</summary>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"

#ifdef __cplusplus
extern "C" {
#endif

	jint Java_org_cocos2dx_cpp_AppActivity_cppSum(JNIEnv *env, jobject obj, jint a, jint b)
	{
		return a + b*2;
	}

	
	// admit login success
	jint Java_org_cocos2dx_cpp_AppActivity_MakeID(JNIEnv *env, jobject obj, jstring Name)
	{
		// jstring to string
		const char *nativeString = env->GetStringUTFChars(Name, JNI_FALSE);

		// on login success
		gamestate=1;

		// save flush userName
		UserDefault::getInstance()->setStringForKey("Name", nativeString);
		UserDefault::getInstance()->flush();
	}

#ifdef __cplusplus
}
#endif

#endif


Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	Init_Start();

    return true;
}

void HelloWorld::Init_Start()
{
	//Background image
	auto spr_bg = Sprite::create("img/background1.jpg");
	spr_bg->setAnchorPoint(Point(0, 1));
	spr_bg->setPosition(Point(0, 320));
	this->addChild(spr_bg, 0);

	auto spr1 = Sprite::create("img/char1_Normal.png");
	spr1->setAnchorPoint(Point(0, 1));
	spr1->setPosition(Point(197, 550));
	this->addChild(spr1, 0, "spr1");

	auto action_Move1 = MoveTo::create(4.0, Point(197, 150));
	auto action_BounceOut = EaseBounceOut::create(action_Move1);
	spr1->runAction(action_BounceOut);

	// main update scheduler
	this->schedule(schedule_selector(HelloWorld::Bounce), 0.01);
}

void HelloWorld::Bounce(float delta)
{
	auto spr1 = (Sprite*)this->getChildByName("spr1");

	///<summary>
	/// <Title Opening Interface>
	/// 초기에 화면에 스프라이트들이 순차적으로 날아와서 타이틀을 구성한다.
	///</summary>
	if (spr1->getPositionY() == 150 && Bounce_Status == 0)
	{
		Bounce_Status = 1;

		auto spr2 = Sprite::create("img/Title1.png");
		spr2->setAnchorPoint(Point(0, 1));
		spr2->setPosition(Point(15, 320));
		spr2->setOpacity(0);
		this->addChild(spr2, 0, "spr2");

		auto Action_FadeIn = FadeIn::create(1.5);
		spr2->runAction(Action_FadeIn);

		int sound_state = UserDefault::getInstance()->getIntegerForKey("sound", 0);
		if (sound_state == 0)
			float sound = SimpleAudioEngine::getInstance()->playEffect("sounds/Opening.wav");
	}
	else if (Bounce_Status == 1)
	{
		// get title opacity-fade, login text pannel fly out
		auto spr2 = (Sprite*)this->getChildByName("spr2");

		if (spr2->getOpacity() == 255)
		{
			Bounce_Status = 2;

			std::string Name_value = UserDefault::getInstance()->getStringForKey("Name", "*1^&-_$@");
			const char * Name = Name_value.c_str();

			// login - check on logined
			if (Name_value.compare("*1^&-_$@"))
			{
				Bounce_Status = 3;

				// login success completed
				Login_Complete();
			}

			/*
			 *	for new user
			 */
			else
			{
				// create login sprite
				auto spr3 = Sprite::create("img/login_menu.png");
				spr3->setAnchorPoint(Point(0, 1));
				spr3->setPosition(Point(500, 245));
				this->addChild(spr3, 0, "spr3");

				auto action_Move1 = MoveTo::create(1.0, Point(150, 245));
				auto Action_EaseIn = EaseIn::create(action_Move1, 1.5);
				spr3->runAction(Action_EaseIn);
			}
		}
	}
	else if (Bounce_Status == 2)
	{
		// show editBox
		auto spr3 = (Sprite*)this->getChildByName("spr3");
		if (spr3->getPositionX() == 150)
		{
			Bounce_Status = 3;

			// show name label
			auto Label1 = Label::createWithSystemFont("이름 입력:", "arial", 15);
			Label1->setAnchorPoint(Point(0, 1));
			Label1->setPosition(Point(215, 227));
			//Label1->setColor(Color3B(-255, -255, -255));
			this->addChild(Label1, 0);

			// remove editBox at mobile application
			JavaFun(0);
		}
	}


	///<summary>
	/// <Java Native InterFace>
	///  Java -> C++ 명령을 받으면 일을 직접적으로 처리해주는 부분
	/// [gamestate 변수]
	/// 0 : none
	/// 1 : Login_Complete
	///</summary>

	if (gamestate == 1)
	{
		gamestate = 0;
		Login_Complete();
	}
}


// scence change : transitionFade
void HelloWorld::Login_Complete()
{
	auto scene = TransitionFade::create(1.5, MainScene::createScene());
	Director::getInstance()->replaceScene(scene);
}

void HelloWorld::editBoxReturn(EditBox* editbox)
{

}


// click sound
void HelloWorld::sound_click()
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
int	I
long	J
float	F
double	D
void	V
Object	Ljava/lang/String; L / full path segmentation class
Array	[Ljava/lang/String; [signature [I
*/

void HelloWorld::JavaFun(int value)
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
