//
//  GTGroupCache.h
//  SourceLibrary
//
//  Created by 李 侠懿 on 5/6/14.
//
//

#ifndef SourceLibrary_GTGroupCache_h
#define SourceLibrary_GTGroupCache_h

#include "GTMacros.h"
#include "cocos2d.h"
#include <string>
#include <list>
#include <memory>
#include <map>
#include <array>
#include <cassert>

GHOST_NAMESPACE_BEGIN

template <typename _GroupID, typename _TypeID, typename _Object>
struct GroupCacheTraitsUseMap{
    typedef _GroupID GroupIDType;
    typedef _TypeID TypeIDType;
    typedef _Object ObjectType;
    
    typedef std::list<ObjectType*> ObjectContainer;
    typedef std::shared_ptr<ObjectContainer> ObjectContainerPtr;
    
    typedef std::map<TypeIDType, ObjectContainerPtr> TypeID_Objects_Map;
    typedef std::shared_ptr<TypeID_Objects_Map> TypeID_Objects_MapPtr;
    typedef std::map<GroupIDType, TypeID_Objects_MapPtr> GroupID_ObjectsMap_Map;
    
    static void retainObject(ObjectType* pObject)
    {
        pObject->retain();
    }
    
    static void releaseObject(ObjectType* pObject)
    {
        pObject->release();
    }
    
    static void releaseGroup(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid);
    static void releaseGroup(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid);
    static void releaseAllObjects(GroupID_ObjectsMap_Map& objects);
    static bool insertObject(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid, ObjectType* pObject);
    static bool groupRetained(const GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid);
    static bool groupRetained(const GroupID_ObjectsMap_Map& objects, const GroupIDType& gid);

};

template <typename _Object, size_t GroupCount, size_t TypeCount>
struct GroupCacheTraitsUseArray{
    typedef size_t GroupIDType;
    typedef size_t TypeIDType;
    typedef _Object ObjectType;
    
    typedef std::list<ObjectType*> ObjectContainer;
    typedef std::shared_ptr<ObjectContainer> ObjectContainerPtr;
    
    typedef std::array<ObjectContainerPtr, TypeCount> TypeID_Objects_Map;
    typedef std::shared_ptr<TypeID_Objects_Map> TypeID_Objects_MapPtr;
    typedef std::array<TypeID_Objects_MapPtr, GroupCount> GroupID_ObjectsMap_Map;
    
    static void retainObject(ObjectType* pObject)
    {
        pObject->retain();
    }
    
    static void releaseObject(ObjectType* pObject)
    {
        pObject->release();
    }
    
    static void releaseGroup(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid);
    static void releaseGroup(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid);
    static void releaseAllObjects(GroupID_ObjectsMap_Map& objects);
    static bool insertObject(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid, ObjectType* pObject);
    static bool groupRetained(const GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid);
    static bool groupRetained(const GroupID_ObjectsMap_Map& objects, const GroupIDType& gid);
    
};

template <typename _GroupID = std::string, typename _TypeID = std::string, typename _Object = cocos2d::Ref, typename _Traits = GroupCacheTraitsUseMap<_GroupID, _TypeID, _Object> >
class GroupCache{
public:
    typedef _Traits TraitsType;
    typedef typename TraitsType::GroupIDType GroupIDType;
    typedef typename TraitsType::TypeIDType TypeIDType;
    typedef typename TraitsType::ObjectType ObjectType;
    
    typedef typename TraitsType::ObjectContainer ObjectContainer;
    typedef typename TraitsType::ObjectContainerPtr ObjectContainerPtr;
    
private:
    typedef typename TraitsType::TypeID_Objects_Map TypeID_Objects_Map;
    typedef typename TraitsType::TypeID_Objects_MapPtr TypeID_Objects_MapPtr;
    typedef typename TraitsType::GroupID_ObjectsMap_Map GroupID_ObjectsMap_Map;
    
    GroupID_ObjectsMap_Map objects_;
    
public:
    GroupCache() = default;
    ~GroupCache()
    {
        TraitsType::releaseAllObjects(objects_);
    }
    
    GroupCache(const GroupCache& rhs) = delete;
    GroupCache& operator =(const GroupCache& rhs) = delete;
    
public:
    void retainObject(const GroupIDType& gid, const TypeIDType& tid, ObjectType* pObject)
    {
        TraitsType::insertObject(objects_, gid, tid, pObject);
    }
    void releaseGroup(const GroupIDType& gid, const TypeIDType& tid)
    {
        TraitsType::releaseGroup(objects_, gid, tid);
    }
    void releaseGroup(const GroupIDType& gid)
    {
        TraitsType::releaseGroup(objects_, gid);
    }
    bool groupRetained(const GroupIDType& gid, const TypeIDType& tid) const
    {
        return TraitsType::groupRetained(objects_, gid, tid);
    }
    bool groupRetained(const GroupIDType& gid) const
    {
        return TraitsType::groupRetained(objects_, gid);
    }
    
};

template <typename _Traits = GroupCacheTraitsUseMap<std::string, std::string, cocos2d::Ref> >
using GroupCacheByTraits = GroupCache<typename _Traits::GroupIDType, typename _Traits::TypeIDType, typename _Traits::ObjectType, _Traits>;


//----------GroupCacheTraitsUseMap implement--------

template <typename _GroupID, typename _TypeID, typename _Object>
void GroupCacheTraitsUseMap<_GroupID, _TypeID, _Object>::releaseGroup(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid)
{
    auto gIter = objects.find(gid);
    if (objects.end() != gIter)
    {
        auto tIter = (*gIter).second->find(tid);
        if ((*gIter).second->end() != tIter)
        {
            for (auto pObject : *(*tIter).second)
            {
                releaseObject(pObject);
            }
            
            // delete type
            (*gIter).second->erase(tIter);
        }
        
        if ((*gIter).second->empty())
        {
            // delete group
            objects.erase(gIter);
        }
    }
}

template <typename _GroupID, typename _TypeID, typename _Object>
void GroupCacheTraitsUseMap<_GroupID, _TypeID, _Object>::releaseGroup(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid)
{
    auto gIter = objects.find(gid);
    if (objects.end() != gIter)
    {
        for (auto tPair : *(*gIter).second)
        {
            for (auto pObject : *tPair.second)
            {
                releaseObject(pObject);
            }
        }
        
        // delete group
        objects.erase(gIter);
    }
}

template <typename _GroupID, typename _TypeID, typename _Object>
void GroupCacheTraitsUseMap<_GroupID, _TypeID, _Object>::releaseAllObjects(GroupID_ObjectsMap_Map& objects)
{
    for (auto gPair : objects)
    {
        for (auto tPair : *gPair.second)
        {
            for (auto pObject : *tPair.second)
            {
                releaseObject(pObject);
            }
        }
    }
    
    objects.clear();
}

template <typename _GroupID, typename _TypeID, typename _Object>
bool GroupCacheTraitsUseMap<_GroupID, _TypeID, _Object>::insertObject(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid, ObjectType* pObject)
{
    assert(pObject);
    ObjectContainerPtr pContainer;
    
    auto gIter = objects.find(gid);
    if (objects.end() == gIter)
    {
        gIter = objects.insert(std::make_pair(gid, TypeID_Objects_MapPtr(new TypeID_Objects_Map()))).first;
        pContainer.reset(new ObjectContainer());
        (*gIter).second->insert(std::make_pair(tid, pContainer));
    }
    else
    {
        auto tIter = (*gIter).second->find(tid);
        if ((*gIter).second->end() == tIter)
        {
            tIter = (*gIter).second->insert(std::make_pair(tid, ObjectContainerPtr(new ObjectContainer()))).first;
        }
        pContainer = (*tIter).second;
    }
    
    pContainer->push_back(pObject);
    
    retainObject(pObject);
    return true;
}

template <typename _GroupID, typename _TypeID, typename _Object>
bool GroupCacheTraitsUseMap<_GroupID, _TypeID, _Object>::groupRetained(const GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid)
{
    auto gIter = objects.find(gid);
    if (objects.end() != gIter)
    {
        auto tIter = (*gIter).second->find(tid);
        return (*gIter).second->end() != tIter && !(*tIter).second->empty();
    }
    return false;
}

template <typename _GroupID, typename _TypeID, typename _Object>
bool GroupCacheTraitsUseMap<_GroupID, _TypeID, _Object>::groupRetained(const GroupID_ObjectsMap_Map& objects, const GroupIDType& gid)
{
    auto gIter = objects.find(gid);
    return objects.end() != gIter && !(*gIter).second->empty();
}

//----------GroupCacheTraitsUseArray implement--------

template <typename _Object, size_t GroupCount, size_t TypeCount>
void GroupCacheTraitsUseArray<_Object, GroupCount, TypeCount>::releaseGroup(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid)
{
    try
    {
        auto& pTypes = objects.at(gid);
        if (pTypes)
        {
            auto& pObjects = pTypes->at(tid);
            if (pObjects)
            {
                for (auto pObject : *pObjects)
                {
                    releaseObject(pObject);
                }
                
                // delete type
                pObjects.reset();
            }
            
            if (pTypes->empty())
            {
                // delete group
                pTypes.reset();
            }
        }
    }
    catch (std::out_of_range&)
    {
        
    }
}

template <typename _Object, size_t GroupCount, size_t TypeCount>
void GroupCacheTraitsUseArray<_Object, GroupCount, TypeCount>::releaseGroup(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid)
{
    try
    {
        auto& pTypes = objects.at(gid);
        if (pTypes)
        {
            for (auto pObjects : *pTypes)
            {
                if (pObjects)
                {
                    for (auto pObject : *pObjects)
                    {
                        releaseObject(pObject);
                    }
                }
            }
            
            // delete group
            pTypes.reset();
        }
    }
    catch (std::out_of_range&)
    {
        
    }
}

template <typename _Object, size_t GroupCount, size_t TypeCount>
void GroupCacheTraitsUseArray<_Object, GroupCount, TypeCount>::releaseAllObjects(GroupID_ObjectsMap_Map& objects)
{
    if (!objects.empty())
    {
        for (auto pTypes : objects)
        {
            if (pTypes)
            {
                for (auto pObjects : *pTypes)
                {
                    if (pObjects)
                    {
                        for (auto pObject : *pObjects)
                        {
                            releaseObject(pObject);
                        }
                    }
                }
            }
        }
        GroupID_ObjectsMap_Map().swap(objects);
    }
}

template <typename _Object, size_t GroupCount, size_t TypeCount>
bool GroupCacheTraitsUseArray<_Object, GroupCount, TypeCount>::insertObject(GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid, ObjectType* pObject)
{
    assert(pObject);
    
    try
    {
        auto& pTypes = objects.at(gid);
        if (!pTypes)
        {
            pTypes.reset(new TypeID_Objects_Map());
        }
        
        auto& pObjects = pTypes->at(tid);
        if (!pObjects)
        {
            pObjects.reset(new ObjectContainer());
        }
        
        pObjects->push_back(pObject);
        
        retainObject(pObject);
        return true;
    }
    catch (std::out_of_range&)
    {
        
    }
    return false;
}

template <typename _Object, size_t GroupCount, size_t TypeCount>
bool GroupCacheTraitsUseArray<_Object, GroupCount, TypeCount>::groupRetained(const GroupID_ObjectsMap_Map& objects, const GroupIDType& gid, const TypeIDType& tid)
{
    try
    {
        auto pTypes = objects.at(gid);
        if (!pTypes)
        {
            return false;
        }
        
        auto pObjects = pTypes->at(tid);
        if (!pObjects)
        {
            return false;
        }
        
        return !pObjects->empty();
    }
    catch (std::out_of_range&)
    {
        
    }
    return false;
}

template <typename _Object, size_t GroupCount, size_t TypeCount>
bool GroupCacheTraitsUseArray<_Object, GroupCount, TypeCount>::groupRetained(const GroupID_ObjectsMap_Map& objects, const GroupIDType& gid)
{
    try
    {
        auto pTypes = objects.at(gid);
        if (!pTypes)
        {
            return false;
        }
        
        return !pTypes->empty();
    }
    catch (std::out_of_range&)
    {
        
    }
    return false;
}

GHOST_NAMESPACE_END

#endif
