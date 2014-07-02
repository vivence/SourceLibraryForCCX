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
    ghost::LoadInfoStore<> testLoadInfoStore(str.c_str(), str.length());
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
    //    ghost::GroupCache<> testGroupCache;
    //    testGroupCache.retainObject("groupID", "typeID", nullptr);
    //    testGroupCache.releaseGroup("groupID", "typeID");
    //    testGroupCache.releaseGroup("groupID");
    //    testGroupCache.groupRetained("groupID", "typeID");
    //    testGroupCache.groupRetained("groupID");
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
