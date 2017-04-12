#ifndef __TimerFifo_H__
#define __TimerFifo_H__

/****************************************************/
/*
    author����־��
    ר��Ϊtimer���Ƶ�һ�����ݽṹ����������Ҫ��
    1.�̰߳�ȫ
    2.Timer ����<
    3.֧��������ʣ����Ը���key����ɾ��һ����ʱ��
*/
/****************************************************/

#include<cassert>
#include<map>

#include "threadmutex.h"
#include "threadcondition.h"
#include "threadlock.h"

using namespace std;

template<typename KeyType,typename ValueType>
class SortedFifo
{
private:
    map<KeyType,ValueType> maplist;
    mutable Mutex mMutex;
    Condition mCondition;

    //this two function cannot be used;
    SortedFifo(const SortedFifo &);
    SortedFifo& operator =(const SortedFifo &);
public:
    typedef struct pair<KeyType,ValueType> Pair;
    //constructors;
    SortedFifo(){};

    //deconstructors
    virtual ~SortedFifo(){};

    unsigned int    size() const;
    bool            empty() const;

    //main functions
    bool            add(KeyType &, ValueType &);
    bool            add(KeyType , ValueType );
    bool            add(KeyType key, ValueType * value);
    ValueType *     getFront();
    ValueType *     findByKey(const KeyType&);
    void            deleteByKey(const KeyType&);
    void            clear();
};

template<typename KeyType,typename ValueType>
unsigned int SortedFifo<KeyType,ValueType>::size() const
{
    //Lock lock(mMutex); (void)lock;
    return maplist.size();
}

template<typename KeyType,typename ValueType>
bool SortedFifo<KeyType,ValueType>::empty() const
{
    //Lock lock(mMutex); (void)lock;
    return maplist.empty();
}

template<typename KeyType,typename ValueType>
bool SortedFifo<KeyType,ValueType>::add(KeyType& key, ValueType & value)
{
    Lock lock(mMutex); (void)lock;
    Pair pair1=make_pair(key, value);
    maplist.insert(pair1);
    return true;
}

template<typename KeyType,typename ValueType>
bool SortedFifo<KeyType,ValueType>::add(KeyType key, ValueType value)
{
    Lock lock(mMutex); (void)lock;
    Pair pair1=make_pair(key, value);
    maplist.insert(pair1);
    return true;
}

template<typename KeyType,typename ValueType>
bool SortedFifo<KeyType,ValueType>::add(KeyType key, ValueType * value)
{
    Lock lock(mMutex); (void)lock;
    Pair pair1=make_pair(key, *value);
    maplist.insert(pair1);
    return true;
}

template<typename KeyType,typename ValueType>
ValueType * SortedFifo<KeyType,ValueType>::getFront()
{
     Lock lock(mMutex); (void)lock;
     if(maplist.empty())
         return NULL;
     typename map<KeyType,ValueType>::iterator ret = maplist.begin();
     typename map<KeyType,ValueType>::iterator iter = maplist.begin();
     while(iter!=maplist.end())
     {
         if(iter->second<ret->second)
             ret = iter;
         iter++;
     }
     return &(ret->second);
}
template<typename KeyType,typename ValueType>
ValueType * SortedFifo<KeyType,ValueType>::findByKey(const KeyType& key )
{
    Lock lock(mMutex); (void)lock;
    typename  map<KeyType,ValueType>::iterator iter = maplist.find(key);
    if(iter ==maplist.end())
        return NULL;
    return &(iter->second);
}

template<typename KeyType,typename ValueType>
 void SortedFifo<KeyType,ValueType>::deleteByKey(const KeyType& key)
 {
    Lock lock(mMutex); (void)lock;
    maplist.erase(key);
 }

 template<typename KeyType,typename ValueType>
 void SortedFifo<KeyType,ValueType>::clear()
 {
    Lock lock(mMutex); (void)lock;
    maplist.clear();
 }
#endif
