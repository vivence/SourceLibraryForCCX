//
//  GTLoadInfoStore.h
//  SourceLibrary
//
//  Created by 李 侠懿 on 4/28/14.
//
//

#ifndef __SourceLibrary__GTLoadInfoStore__
#define __SourceLibrary__GTLoadInfoStore__

#include "GTMacros.h"
#include "cocos2d.h"
#include <string>
#include <list>
#include <memory>
#include <map>
#include <exception>

#ifdef GHOST_DEBUG
#include <iostream>
#include <iomanip>
#endif // GHOST_DEBUG

/*
 <Group id="">
    <Texture file_name="texture.pvr.ccz" pixel_format="RGBA4444"/>
    <SpriteFrame file_name="texture.plist" />
    <BitmapFont file_name=\"bitmap_font.fnt\"/>
    <BackgroundMusic file_name="background_music.mp3"/>
    <SoundEffect file_name="sound_effect.mp3"/>
    <FrameAnimations file_name="frame_animations.plist" />
    <Particle file_name="particle_.plist"/>
 </Group>
 */

GHOST_NAMESPACE_BEGIN

template<typename GID, typename TID, typename Info>
struct LoadInfoTraits{
    typedef GID GroupIDType;
    typedef TID TypeIDType;
    typedef Info InfoType;
    
    static GroupIDType stringGroupID(const char* str)
    {
        return std::move(GroupIDType(str));
    }
    
    static std::string groupIDToString(GroupIDType gid)
    {
        return std::move(gid);
    }
    
    static TypeIDType stringToTypeID(const char* str)
    {
        return std::move(TypeIDType(str));
    }
    
    static std::string typeIDToString(TypeIDType tid)
    {
        return std::move(tid);
    }
    
    static InfoType createInfo(const char** attrs)
    {
        InfoType info;
        for (int i = 0; attrs[i]; i += 2)
        {
            const char* key = attrs[i];
            const char* value = attrs[i+1];
            info[key] = value;
        }
        return std::move(info);
    }
};

template<typename GID = std::string, typename TID = std::string, typename Traits = LoadInfoTraits<GID, TID, std::map<std::string, std::string> > >
class LoadInfoStore : public cocos2d::SAXDelegator{
public:
    typedef GID GroupIDType;
    typedef TID TypeIDType;
    typedef Traits TraitsType;
    typedef typename TraitsType::InfoType InfoType;
    
    constexpr static const char* const GROUP_ID_TAG_NAME = "Group";
    
    typedef std::list<InfoType> TypeInfoContainer;
    typedef std::shared_ptr<TypeInfoContainer> TypeInfoContainerPtr;
    
private:
    typedef std::map<TypeIDType, TypeInfoContainerPtr> TypeID_TypeInfos_Map;
    typedef std::shared_ptr<TypeID_TypeInfos_Map> TypeID_TypeInfos_MapPtr;
    typedef std::map<GroupIDType, TypeID_TypeInfos_MapPtr> GroupID_TypeInfosMap_Map;
    
    GroupID_TypeInfosMap_Map infos_;
    typename GroupID_TypeInfosMap_Map::iterator infoIter_ = infos_.end();
    
public:
    explicit LoadInfoStore(const char* fileName) throw(std::exception);
    LoadInfoStore(const char* pData, size_t dataLen) throw(std::exception);
    
    LoadInfoStore(const LoadInfoStore& rhs) = delete;
    LoadInfoStore& operator =(const LoadInfoStore& rhs) = delete;
    
private:
    bool preConstruct(cocos2d::SAXParser& parser);
    
public:
    // implement pure virtual methods of CCSAXDelegator
    virtual void startElement(void* context, const char* name, const char** attrs) override;
    virtual void endElement(void* context, const char* name) override;
    virtual void textHandler(void* context, const char* text, int len) override;
    
public:
    const TypeInfoContainerPtr getInfos(const GroupIDType& gid, const TypeIDType& tid) const;
    
#ifdef GHOST_DEBUG
private:
    void dump() const;
#endif // GHOST_DEBUG
    
};


//----------implement--------

template<typename GID, typename TID, typename Traits>
LoadInfoStore<GID, TID, Traits>::LoadInfoStore(const char* fileName) throw(std::exception)
{
    CCAssert(fileName, "construct LoadInfoStore xml file name can not be null");
    cocos2d::SAXParser parser;
    preConstruct(parser);
    if (!parser.parse(fileName))
    {
        throw std::exception();
    }
#ifdef GHOST_DEBUG
    dump();
#endif // GHOST_DEBUG
}

template<typename GID, typename TID, typename Traits>
LoadInfoStore<GID, TID, Traits>::LoadInfoStore(const char* pData, size_t dataLen) throw(std::exception)
{
    CCAssert(pData, "construct LoadInfoStore pData can not be null");
    cocos2d::SAXParser parser;
    preConstruct(parser);
    if (!parser.parse(pData, dataLen))
    {
        throw std::exception();
    }
#ifdef GHOST_DEBUG
    dump();
#endif // GHOST_DEBUG
}

template<typename GID, typename TID, typename Traits>
bool LoadInfoStore<GID, TID, Traits>::preConstruct(cocos2d::SAXParser& parser)
{
    if (false == parser.init("UTF-8") )
    {
        return false;
    }
    parser.setDelegator(this);
    return true;
}

// implement pure virtual methods of CCSAXDelegator
template<typename GID, typename TID, typename Traits>
void LoadInfoStore<GID, TID, Traits>::startElement(void* context, const char* name, const char** attrs)
{
    if (infos_.end() != infoIter_)
    {
        TypeIDType tid(std::move(TraitsType::stringToTypeID(name)));
        auto iter = (*infoIter_).second->find(tid);
        if ((*infoIter_).second->end() == iter)
        {
            iter = (*infoIter_).second->insert(std::make_pair(tid, TypeInfoContainerPtr(new TypeInfoContainer()))).first;
        }
        (*iter).second->push_back(std::move(Traits::createInfo(attrs)));
    }
    else if (0 == strcmp(GROUP_ID_TAG_NAME, name))
    {
        for (int i = 0; attrs[i]; i += 2)
        {
            const char* key = attrs[i];
            const char* value = attrs[i+1];
            if (0 == strcmp("id", key))
            {
                GroupIDType gid(std::move(TraitsType::stringGroupID(value)));
                infoIter_ = infos_.find(gid);
                if (infos_.end() == infoIter_)
                {
                    infoIter_ = infos_.insert(std::make_pair(gid, TypeID_TypeInfos_MapPtr(new TypeID_TypeInfos_Map()))).first;
                }
                break;
            }
        }
    }
}

template<typename GID, typename TID, typename Traits>
void LoadInfoStore<GID, TID, Traits>::endElement(void* context, const char* name)
{
    if (0 == strcmp(GROUP_ID_TAG_NAME, name))
    {
        infoIter_ = infos_.end();
    }
}

template<typename GID, typename TID, typename Traits>
void LoadInfoStore<GID, TID, Traits>::textHandler(void* context, const char* text, int len)
{
    
}

template<typename GID, typename TID, typename Traits>
const typename LoadInfoStore<GID, TID, Traits>::TypeInfoContainerPtr LoadInfoStore<GID, TID, Traits>::getInfos(const GroupIDType& gid, const TypeIDType& tid) const
{
    auto gIter = infos_.find(gid);
    if (infos_.end() != gIter)
    {
        auto tIter = (*gIter).second->find(tid);
        if ((*gIter).second->end() != tIter)
        {
            return (*tIter).second;
        }
    }
    return nullptr;
}

#ifdef GHOST_DEBUG
template<typename GID, typename TID, typename Traits>
void LoadInfoStore<GID, TID, Traits>::dump() const
{
    std::clog<<"--------------dump------------>"<<std::endl;
    std::clog<<"class: "<<typeid(*this).name()<<std::endl;
    for (auto gPair : infos_)
    {
        std::clog<<TraitsType::groupIDToString(gPair.first)<<"[Group]"<<std::endl;
        for (auto tPair : *gPair.second)
        {
            for (auto item : *tPair.second)
            {
                std::clog<<"\t"<<TraitsType::typeIDToString(tPair.first)<<"[Type]"<<std::endl;
                for (auto property : item)
                {
                    std::clog<<"\t\t"<<property.first<<" = "<<property.second<<std::endl;
                }
            }
        }
    }
    std::clog<<"--------------dump------------<"<<std::endl;
}
#endif // GHOST_DEBUG

GHOST_NAMESPACE_END

#endif /* defined(__SourceLibrary__GTLoadInfoStore__) */
