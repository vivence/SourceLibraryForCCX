#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "GTLoadInfoStore.h"
#include "GTGroupCache.h"
#include "GTFunctionConsumer.h"
#include "GTAsyncConsumer.h"
#include "GTFunctionProducer.h"
#include "GTAsyncProducer.h"

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
    
    const char* const GROUPS[] = {
        "Test"
    };
    const char* const TYPES[] = {
        "Texture",
        "SpriteFrame",
        "BitmapFont",
        "BackgroundMusic",
        "SoundEffect",
        "FrameAnimations",
        "Particle"
    };
    typedef ghost::LoadInfoTraitsUseArrayImpl<sizeof(GROUPS)/sizeof(GROUPS[0]), sizeof(TYPES)/sizeof(TYPES[0]), std::map<std::string, std::string>, 0> LoadInfoTraits;
    for (size_t i = 0; i < LoadInfoTraits::GROUP_COUNT; ++i)
    {
        LoadInfoTraits::s_GroupIDStrings[i] = GROUPS[i];
    }
    for (size_t i = 0; i < LoadInfoTraits::TYPE_COUNT; ++i)
    {
        LoadInfoTraits::s_TypeIDStrings[i] = TYPES[i];
    }
    
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
    
    auto pProducer = new ghost::AsyncProducer<ghost::FunctionProducer<>>();
    
    auto pConsumer = new ghost::AsyncConsumer<ghost::FunctionConsumer<>>(pProducer->getQueue(), pProducer->getQueueDestroyed(), pProducer->getQueueMutex(), pProducer->getQueueNotEmpty(), std::chrono::seconds(-1));
    director->getScheduler()->schedule([this, pProducer, pConsumer](float delta){
        Director::getInstance()->getScheduler()->unschedule("produce", this);
        
        // 1
        pProducer->destroyAll();
        // 2
        delete pConsumer;
        // 3
        delete pProducer;
    }, this, 0, 0, 3, false, "close_async_consumer");
    
    director->getScheduler()->schedule([this, pProducer](float delta){
        static int i = 0;
        int productNumber = ++i;
        pProducer->produce([productNumber](){
            CCLOG("product[%d] consumed", productNumber);
        });
    }, this, 1, 9, 0, false, "produce");
    
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
