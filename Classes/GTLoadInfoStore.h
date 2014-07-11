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
#include <array>
#include <utility>
#include <exception>

#ifdef GHOST_DEBUG
#include <iostream>
#include <iomanip>
#endif // GHOST_DEBUG

GHOST_NAMESPACE_BEGIN

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
template<typename _GroupID, typename _TypeID, typename _Info>
struct LoadInfoTraitsUseMap{
    typedef _GroupID GroupIDType;
    typedef _TypeID TypeIDType;
    typedef _Info InfoType;
    
    typedef std::list<InfoType> TypeInfoContainer;
    typedef std::shared_ptr<TypeInfoContainer> TypeInfoContainerPtr;
    typedef std::map<TypeIDType, TypeInfoContainerPtr> TypeID_TypeInfos_Map;
    typedef std::shared_ptr<TypeID_TypeInfos_Map> TypeID_TypeInfos_MapPtr;
    typedef std::map<GroupIDType, TypeID_TypeInfos_MapPtr> GroupID_TypeInfosMap_Map;
    
    static GroupIDType stringToGroupID(const char* str)
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
    
    static TypeID_TypeInfos_MapPtr makeSureFindGroup(GroupID_TypeInfosMap_Map& infos, const GroupIDType& gid);
    static void insertInfo(TypeID_TypeInfos_Map& infos, const TypeIDType& tid, InfoType&& info);
    static TypeInfoContainerPtr getInfos(const GroupID_TypeInfosMap_Map& infos, const GroupIDType& gid, const TypeIDType& tid);
    static InfoType createInfo(const char** attrs);
    
#ifdef GHOST_DEBUG
    static void dump(const GroupID_TypeInfosMap_Map& infos);
#endif // GHOST_DEBUG
};

template<size_t GroupCount, size_t TypeCount, typename _Info>
struct LoadInfoTraitsUseArrayBase{
    typedef size_t GroupIDType;
    typedef size_t TypeIDType;
    typedef _Info InfoType;
    
    typedef std::list<InfoType> TypeInfoContainer;
    typedef std::shared_ptr<TypeInfoContainer> TypeInfoContainerPtr;
    typedef std::array<TypeInfoContainerPtr, TypeCount> TypeID_TypeInfos_Map;
    typedef std::shared_ptr<TypeID_TypeInfos_Map> TypeID_TypeInfos_MapPtr;
    typedef std::array<TypeID_TypeInfos_MapPtr, GroupCount> GroupID_TypeInfosMap_Map;
    
    static const size_t GROUP_COUNT = GroupCount;
    static const size_t TYPE_COUNT = TypeCount;
    
    static TypeID_TypeInfos_MapPtr makeSureFindGroup(GroupID_TypeInfosMap_Map& infos, const GroupIDType& gid);
    static void insertInfo(TypeID_TypeInfos_Map& infos, const TypeIDType& tid, InfoType&& info);
    static TypeInfoContainerPtr getInfos(const GroupID_TypeInfosMap_Map& infos, const GroupIDType& gid, const TypeIDType& tid);
    static InfoType createInfo(const char** attrs);
};

template<size_t GroupCount, size_t TypeCount, typename _Info, size_t UniqueID>
struct LoadInfoTraitsUseArrayImpl : public LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>{
    
    typedef typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::GroupIDType GroupIDType;
    typedef typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::TypeIDType TypeIDType;
    typedef typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::InfoType InfoType;
    
    typedef typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::TypeInfoContainer TypeInfoContainer;
    typedef typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::TypeInfoContainerPtr TypeInfoContainerPtr;
    
    typedef typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::TypeID_TypeInfos_Map TypeID_TypeInfos_Map;
    typedef typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::TypeID_TypeInfos_MapPtr TypeID_TypeInfos_MapPtr;
    typedef typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::GroupID_TypeInfosMap_Map GroupID_TypeInfosMap_Map;
    
    static std::string s_GroupIDStrings[GroupCount];
    static std::string s_TypeIDStrings[TypeCount];
    
    static const GroupIDType INVALID_GROUP_ID = ~0;
    static const TypeIDType INVALID_TYPE_ID = ~0;
    
    static GroupIDType stringToGroupID(const char* str);
    static std::string groupIDToString(GroupIDType gid);
    static TypeIDType stringToTypeID(const char* str);
    static std::string typeIDToString(TypeIDType tid);
    
#ifdef GHOST_DEBUG
    static void dump(const GroupID_TypeInfosMap_Map& infos);
#endif // GHOST_DEBUG
};

template<typename _GroupID = std::string, typename _TypeID = std::string, typename _Traits = LoadInfoTraitsUseMap<_GroupID, _TypeID, std::map<std::string, std::string> > >
class LoadInfoStore : public cocos2d::SAXDelegator{
public:
    constexpr static const char* const GROUP_ID_TAG_NAME = "Group";
    
    typedef _Traits TraitsType;
    typedef typename TraitsType::GroupIDType GroupIDType;
    typedef typename TraitsType::TypeIDType TypeIDType;
    typedef typename TraitsType::InfoType InfoType;
    
    typedef typename TraitsType::TypeInfoContainer TypeInfoContainer;
    typedef typename TraitsType::TypeInfoContainerPtr TypeInfoContainerPtr;
    
private:
    typedef typename TraitsType::TypeID_TypeInfos_Map TypeID_TypeInfos_Map;
    typedef typename TraitsType::TypeID_TypeInfos_MapPtr TypeID_TypeInfos_MapPtr;
    typedef typename TraitsType::GroupID_TypeInfosMap_Map GroupID_TypeInfosMap_Map;
    
    GroupID_TypeInfosMap_Map infos_;
    TypeID_TypeInfos_MapPtr pTypes_;
    
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
    const TypeInfoContainerPtr getInfos(const GroupIDType& gid, const TypeIDType& tid) const
    {
        return TraitsType::getInfos(infos_, gid, tid);
    }
    
    const TypeInfoContainerPtr getInfos(const char* gidString, const char* tidString) const
    {
        return TraitsType::getInfos(infos_, TraitsType::stringToGroupID(gidString), TraitsType::stringToTypeID(tidString));
    }
    
#ifdef GHOST_DEBUG
private:
    void dump() const;
#endif // GHOST_DEBUG
    
};

//----------LoadInfoTraitsUseMap implement--------

template<typename _GroupID, typename _TypeID, typename _Info>
typename LoadInfoTraitsUseMap<_GroupID, _TypeID, _Info>::TypeID_TypeInfos_MapPtr LoadInfoTraitsUseMap<_GroupID, _TypeID, _Info>::makeSureFindGroup(GroupID_TypeInfosMap_Map& infos, const GroupIDType& gid)
{
    auto infoIter = infos.find(gid);
    if (infos.end() == infoIter)
    {
        infoIter = infos.insert(std::make_pair(gid, TypeID_TypeInfos_MapPtr(new TypeID_TypeInfos_Map()))).first;
    }
    return (*infoIter).second;
}

template<typename _GroupID, typename _TypeID, typename _Info>
void LoadInfoTraitsUseMap<_GroupID, _TypeID, _Info>::insertInfo(TypeID_TypeInfos_Map& infos, const TypeIDType& tid, InfoType&& info)
{
    auto iter = infos.find(tid);
    if (infos.end() == iter)
    {
        iter = infos.insert(std::make_pair(tid, TypeInfoContainerPtr(new TypeInfoContainer()))).first;
    }
    (*iter).second->push_back(std::move(info));
}

template<typename _GroupID, typename _TypeID, typename _Info>
typename LoadInfoTraitsUseMap<_GroupID, _TypeID, _Info>::TypeInfoContainerPtr LoadInfoTraitsUseMap<_GroupID, _TypeID, _Info>::getInfos(const GroupID_TypeInfosMap_Map& infos, const GroupIDType& gid, const TypeIDType& tid)
{
    auto gIter = infos.find(gid);
    if (infos.end() != gIter)
    {
        auto tIter = (*gIter).second->find(tid);
        if ((*gIter).second->end() != tIter)
        {
            return (*tIter).second;
        }
    }
    return TypeInfoContainerPtr();
}

template<typename _GroupID, typename _TypeID, typename _Info>
typename LoadInfoTraitsUseMap<_GroupID, _TypeID, _Info>::InfoType LoadInfoTraitsUseMap<_GroupID, _TypeID, _Info>::createInfo(const char** attrs)
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

#ifdef GHOST_DEBUG
template<typename _GroupID, typename _TypeID, typename _Info>
void LoadInfoTraitsUseMap<_GroupID, _TypeID, _Info>::dump(const GroupID_TypeInfosMap_Map& infos)
{
    for (auto gPair : infos)
    {
        std::clog<<groupIDToString(gPair.first)<<"[Group]"<<std::endl;
        for (auto tPair : *gPair.second)
        {
            for (auto item : *tPair.second)
            {
                std::clog<<"\t"<<typeIDToString(tPair.first)<<"[Type]"<<std::endl;
                for (auto property : item)
                {
                    std::clog<<"\t\t"<<property.first<<" = "<<property.second<<std::endl;
                }
            }
        }
    }
}
#endif // GHOST_DEBUG

//----------LoadInfoTraitsUseArray implement--------

template<size_t GroupCount, size_t TypeCount, typename _Info>
typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::TypeID_TypeInfos_MapPtr LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::makeSureFindGroup(GroupID_TypeInfosMap_Map& infos, const GroupIDType& gid)
{
    try
    {
        auto& pTypes = infos.at(gid);
        if (!pTypes)
        {
            pTypes.reset(new TypeID_TypeInfos_Map());
        }
        return pTypes;
    }
    catch (std::out_of_range&)
    {
        
    }
    return TypeID_TypeInfos_MapPtr();
}

template<size_t GroupCount, size_t TypeCount, typename _Info>
void LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::insertInfo(TypeID_TypeInfos_Map& infos, const TypeIDType& tid, InfoType&& info)
{
    try
    {
        auto& pInfos = infos.at(tid);
        if (!pInfos)
        {
            pInfos.reset(new TypeInfoContainer());
        }
        pInfos->push_back(std::move(info));
    }
    catch (std::out_of_range&)
    {
    }
}

template<size_t GroupCount, size_t TypeCount, typename _Info>
typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::TypeInfoContainerPtr LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::getInfos(const GroupID_TypeInfosMap_Map& infos, const GroupIDType& gid, const TypeIDType& tid)
{
    try
    {
        auto& pTypes = infos.at(gid);
        if (pTypes)
        {
            return pTypes->at(tid);
        }
    }
    catch (std::out_of_range&)
    {
    }
    return TypeInfoContainerPtr();
}

template<size_t GroupCount, size_t TypeCount, typename _Info>
typename LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::InfoType LoadInfoTraitsUseArrayBase<GroupCount, TypeCount, _Info>::createInfo(const char** attrs)
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

#ifdef GHOST_DEBUG
template<size_t GroupCount, size_t TypeCount, typename _Info, size_t UniqueID>
void LoadInfoTraitsUseArrayImpl<GroupCount, TypeCount, _Info, UniqueID>::dump(const GroupID_TypeInfosMap_Map& infos)
{
    try
    {
        for (size_t i = 0; i < GroupCount; ++i)
        {
            std::clog<<groupIDToString(i)<<"[Group]"<<std::endl;
            auto& pTypes = infos.at(i);
            if (!pTypes)
            {
                continue;
            }
            try
            {
                for (size_t j = 0; j < TypeCount; ++j)
                {
                    auto& pInfos = pTypes->at(j);
                    if (!pInfos)
                    {
                        continue;
                    }
                    for (auto item : *pInfos)
                    {
                        std::clog<<"\t"<<typeIDToString(j)<<"[Type]"<<std::endl;
                        for (auto& property : item)
                        {
                            std::clog<<"\t\t"<<property.first<<" = "<<property.second<<std::endl;
                        }
                    }
                }
            }
            catch (std::out_of_range&)
            {
            }
        }
    }
    catch (std::out_of_range&)
    {
    }
}
#endif // GHOST_DEBUG

template<size_t GroupCount, size_t TypeCount, typename _Info, size_t UniqueID>
std::string LoadInfoTraitsUseArrayImpl<GroupCount, TypeCount, _Info, UniqueID>::s_GroupIDStrings[GroupCount];

template<size_t GroupCount, size_t TypeCount, typename _Info, size_t UniqueID>
std::string LoadInfoTraitsUseArrayImpl<GroupCount, TypeCount, _Info, UniqueID>::s_TypeIDStrings[TypeCount];

template<size_t GroupCount, size_t TypeCount, typename _Info, size_t UniqueID>
typename LoadInfoTraitsUseArrayImpl<GroupCount, TypeCount, _Info, UniqueID>::GroupIDType LoadInfoTraitsUseArrayImpl<GroupCount, TypeCount, _Info, UniqueID>::stringToGroupID(const char* str)
{
    for (size_t gid = 0; gid < GroupCount; ++gid)
    {
        if (s_GroupIDStrings[gid] == str)
        {
            return gid;
        }
    }
    return INVALID_GROUP_ID;
}

template<size_t GroupCount, size_t TypeCount, typename _Info, size_t UniqueID>
std::string LoadInfoTraitsUseArrayImpl<GroupCount, TypeCount, _Info, UniqueID>::groupIDToString(GroupIDType gid)
{
    if (GroupCount > gid)
    {
        return s_GroupIDStrings[gid];
    }
    return std::string();
}

template<size_t GroupCount, size_t TypeCount, typename _Info, size_t UniqueID>
typename LoadInfoTraitsUseArrayImpl<GroupCount, TypeCount, _Info, UniqueID>::TypeIDType LoadInfoTraitsUseArrayImpl<GroupCount, TypeCount, _Info, UniqueID>::stringToTypeID(const char* str)
{
    for (size_t tid = 0; tid < TypeCount; ++tid)
    {
        if (s_TypeIDStrings[tid] == str)
        {
            return tid;
        }
    }
    return INVALID_TYPE_ID;
}

template<size_t GroupCount, size_t TypeCount, typename _Info, size_t UniqueID>
std::string LoadInfoTraitsUseArrayImpl<GroupCount, TypeCount, _Info, UniqueID>::typeIDToString(TypeIDType tid)
{
    if (TypeCount > tid)
    {
        return s_TypeIDStrings[tid];
    }
    return std::string();
}


//----------LoadInfoStore implement--------

template<typename _GroupID, typename _TypeID, typename _Traits>
LoadInfoStore<_GroupID, _TypeID, _Traits>::LoadInfoStore(const char* fileName) throw(std::exception)
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

template<typename _GroupID, typename _TypeID, typename _Traits>
LoadInfoStore<_GroupID, _TypeID, _Traits>::LoadInfoStore(const char* pData, size_t dataLen) throw(std::exception)
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

template<typename _GroupID, typename _TypeID, typename _Traits>
bool LoadInfoStore<_GroupID, _TypeID, _Traits>::preConstruct(cocos2d::SAXParser& parser)
{
    if (false == parser.init("UTF-8") )
    {
        return false;
    }
    parser.setDelegator(this);
    return true;
}

#ifdef GHOST_DEBUG
template<typename _GroupID, typename _TypeID, typename _Traits>
void LoadInfoStore<_GroupID, _TypeID, _Traits>::dump() const
{
    std::clog<<"--------------dump------------>"<<std::endl;
    std::clog<<"class: "<<typeid(*this).name()<<std::endl;
    TraitsType::dump(infos_);
    std::clog<<"--------------dump------------<"<<std::endl;
}
#endif // GHOST_DEBUG

// implement pure virtual methods of CCSAXDelegator
template<typename _GroupID, typename _TypeID, typename _Traits>
void LoadInfoStore<_GroupID, _TypeID, _Traits>::startElement(void* context, const char* name, const char** attrs)
{
    if (pTypes_)
    {
        TypeIDType tid(std::move(TraitsType::stringToTypeID(name)));
        TraitsType::insertInfo(*pTypes_, tid, TraitsType::createInfo(attrs));
    }
    else if (0 == strcmp(GROUP_ID_TAG_NAME, name))
    {
        for (int i = 0; attrs[i]; i += 2)
        {
            const char* key = attrs[i];
            const char* value = attrs[i+1];
            if (0 == strcmp("id", key))
            {
                GroupIDType gid(std::move(TraitsType::stringToGroupID(value)));
                pTypes_ = TraitsType::makeSureFindGroup(infos_, gid);
                break;
            }
        }
    }
}

template<typename _GroupID, typename _TypeID, typename _Traits>
void LoadInfoStore<_GroupID, _TypeID, _Traits>::endElement(void* context, const char* name)
{
    if (0 == strcmp(GROUP_ID_TAG_NAME, name))
    {
        pTypes_.reset();
    }
}

template<typename _GroupID, typename _TypeID, typename _Traits>
void LoadInfoStore<_GroupID, _TypeID, _Traits>::textHandler(void* context, const char* text, int len)
{
    
}

GHOST_NAMESPACE_END

#endif /* defined(__SourceLibrary__GTLoadInfoStore__) */
