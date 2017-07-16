#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class HelloWorld : public Layer, public EditBoxDelegate
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

	// implement the "static create()" method manually
	CREATE_FUNC(HelloWorld);
    
		///<summary>
	/// <Java Native InterFace>
	///  C++  ==>  Java
	///</summary>
	void JavaFun(int);

	void Init_Start();

	//Schedules
	void Bounce(float delta);

	//Sound Methods
	void sound_click();

	//Login Methods
	void Login_Complete();

protected:
	//EditBox
	virtual void editBoxReturn(EditBox* editbox);

};

#endif // __HELLOWORLD_SCENE_H__
