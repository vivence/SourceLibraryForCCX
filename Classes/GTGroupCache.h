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
#include <cassert>

GHOST_NAMESPACE_BEGIN

template <typename O>
struct ObjectTraits{
    typedef O ObjectType;
    
    static void retainObject(ObjectType* pObject)
    {
        pObject->retain();
    }
    
    static void releaseObject(ObjectType* pObject)
    {
        pObject->release();
    }
};

template <typename GID = std::string, typename TID = std::string, typename O = cocos2d::Ref, typename Traits = ObjectTraits<O> >
class GroupCache{
public:
    typedef GID GroupIDType;
    typedef TID TypeIDType;
    typedef O ObjectType;
    typedef Traits TraitsType;
    
    typedef std::list<ObjectType*> ObjectContainer;
    typedef std::shared_ptr<ObjectContainer> ObjectContainerPtr;
    
private:
    typedef std::map<TypeIDType, ObjectContainerPtr> TypeID_Objects_Map;
    typedef std::shared_ptr<TypeID_Objects_Map> TypeID_Objects_MapPtr;
    typedef std::map<GroupIDType, TypeID_Objects_MapPtr> GroupID_ObjectsMap_Map;
    
    GroupID_ObjectsMap_Map objects_;
    
public:
    GroupCache() = default;
    ~GroupCache();
    
    GroupCache(const GroupCache& rhs) = delete;
    GroupCache& operator =(const GroupCache& rhs) = delete;
    
public:
    void retainObject(const GroupIDType& gid, const TypeIDType& tid, ObjectType* pObject);
    void releaseGroup(const GroupIDType& gid, const TypeIDType& tid);
    void releaseGroup(const GroupIDType& gid);
    bool groupRetained(const GroupIDType& gid, const TypeIDType& tid) const;
    bool groupRetained(const GroupIDType& gid) const;
    
};


//----------implement--------

template <typename GID, typename TID, typename O, typename Traits>
GroupCache<GID, TID, O, Traits>::~GroupCache()
{
    for (auto gPair : objects_)
    {
        for (auto tPair : *gPair.second)
        {
            for (auto pObject : *tPair.second)
            {
                TraitsType::releaseObject(pObject);
            }
        }
    }
}

template <typename GID, typename TID, typename O, typename Traits>
void GroupCache<GID, TID, O, Traits>::retainObject(const GroupIDType& gid, const TypeIDType& tid, ObjectType* pObject)
{
    assert(pObject);
    ObjectContainerPtr pContainer;
    
    auto gIter = objects_.find(gid);
    if (objects_.end() == gIter)
    {
        gIter = objects_.insert(std::make_pair(gid, TypeID_Objects_MapPtr(new TypeID_Objects_Map()))).first;
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
    
    TraitsType::retainObject(pObject);
}

template <typename GID, typename TID, typename O, typename Traits>
void GroupCache<GID, TID, O, Traits>::releaseGroup(const GroupIDType& gid, const TypeIDType& tid)
{
    auto gIter = objects_.find(gid);
    if (objects_.end() != gIter)
    {
        auto tIter = (*gIter).second->find(tid);
        if ((*gIter).second->end() != tIter)
        {
            for (auto pObject : *(*tIter).second)
            {
                TraitsType::releaseObject(pObject);
            }
            
            // delete type
            (*gIter).second->erase(tIter);
        }
        
        if ((*gIter).second->empty())
        {
             // delete group
            objects_.erase(gIter);
        }
    }
}

template <typename GID, typename TID, typename O, typename Traits>
void GroupCache<GID, TID, O, Traits>::releaseGroup(const GroupIDType& gid)
{
    auto gIter = objects_.find(gid);
    if (objects_.end() != gIter)
    {
        for (auto tPair : *(*gIter).second)
        {
            for (auto pObject : *tPair.second)
            {
                TraitsType::releaseObject(pObject);
            }
        }
        
        // delete group
        objects_.erase(gIter);
    }
}

template <typename GID, typename TID, typename O, typename Traits>
bool GroupCache<GID, TID, O, Traits>::groupRetained(const GroupIDType& gid, const TypeIDType& tid) const
{
    auto gIter = objects_.find(gid);
    if (objects_.end() != gIter)
    {
        auto tIter = (*gIter).second->find(tid);
        return (*gIter).second->end() != tIter && !(*tIter).second->empty();
    }
    return false;
}

template <typename GID, typename TID, typename O, typename Traits>
bool GroupCache<GID, TID, O, Traits>::groupRetained(const GroupIDType& gid) const
{
    auto gIter = objects_.find(gid);
    return objects_.end() != gIter && !(*gIter).second->empty();
}

GHOST_NAMESPACE_END

#endif
