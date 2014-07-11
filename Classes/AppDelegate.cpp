#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "GTLoadInfoStore.h"
#include "GTGroupCache.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);
    
    //-------test---------->
    std::string str = "<Group id=\"Test\">"
    "<Texture file_name=\"texture.pvr.ccz\" pixel_format=\"RGBA4444\"/>"
    "<SpriteFrame file_name=\"texture.plist\" />"
    "<BitmapFont file_name=\"bitmap_font.fnt\"/>"
    "<BackgroundMusic file_name=\"background_music.mp3\"/>"
    "<SoundEffect file_name=\"sound_effect.mp3\"/>"
    "<FrameAnimations file_name=\"frame_animations.plist\" />"
    "<Particle file_name=\"particle_.plist\"/>"
    "</Group>";
    typedef ghost::LoadInfoTraitsUseArrayImpl<1, 7, std::map<std::string, std::string>, 0> LoadInfoTraits;
    LoadInfoTraits::s_String_GroupID[0] = std::make_pair("Test", 0);
    LoadInfoTraits::s_String_TypeID[0] = std::make_pair("Texture", 0);
    LoadInfoTraits::s_String_TypeID[1] = std::make_pair("SpriteFrame", 1);
    LoadInfoTraits::s_String_TypeID[2] = std::make_pair("BitmapFont", 2);
    LoadInfoTraits::s_String_TypeID[3] = std::make_pair("BackgroundMusic", 3);
    LoadInfoTraits::s_String_TypeID[4] = std::make_pair("SoundEffect", 4);
    LoadInfoTraits::s_String_TypeID[5] = std::make_pair("FrameAnimations", 5);
    LoadInfoTraits::s_String_TypeID[6] = std::make_pair("Particle", 6);
    
//    typedef ghost::LoadInfoTraitsUseMap<std::string, std::string, std::map<std::string, std::string>> LoadInfoTraits;
    
    ghost::LoadInfoStore<LoadInfoTraits::GroupIDType, LoadInfoTraits::TypeIDType, LoadInfoTraits> testLoadInfoStore(str.c_str(), str.length());
    auto pInfos = testLoadInfoStore.getInfos("Test", "Texture");
    if (pInfos)
    {
        for (auto& info : *pInfos)
        {
            CCLOG("Test|Texture: ");
            for (auto& key_value : info)
            {
                CCLOG("%s=%s", key_value.first.c_str(), key_value.second.c_str());
            }
        }
    }
//    typedef ghost::GroupCacheTraitsUseArray<cocos2d::Ref, 10, 10> GroupCacheTraits;
//    ghost::GroupCache<GroupCacheTraits::GroupIDType, GroupCacheTraits::TypeIDType, GroupCacheTraits::ObjectType, GroupCacheTraits> testGroupCache;
//    testGroupCache.retainObject(0, 0, nullptr);
//    testGroupCache.releaseGroup(0, 0);
//    testGroupCache.releaseGroup(0);
//    testGroupCache.groupRetained(0, 0);
//    testGroupCache.groupRetained(0);
    //-------test----------<
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
