#ifndef _XILIST_H_
#define _XILIST_H_

#include <list>
#include <iostream>
#include <sys/time.h>
#include <stdint.h>
#include <mutex>
#include <thread>
#include <unistd.h>

namespace Xilinx {

typedef void (FEventType)(void*);
typedef bool (FConditionType)(void*);

typedef struct _ListNode{
    std::string name; //模块名称
    FEventType* funcallback;        //注册回调函数
    void *pdata;                    //数据指针 execute user

    void *pXiImageDevice;           //XiImageDevice 类
    FEventType* pExecute;           //执行函数
    FConditionType* pCondition;     //判断条件函数
    std::string Mode;               //off, once, continue

    uint32_t timeout;               //设置超时时间,单位毫秒

    struct timeval jointime;        //任务加入时间
    struct timeval exectime;        //任务执行时间

}TListNode;

class XiList{
    public:
        XiList();
        ~XiList();
        void addEvent(TListNode& Node);
        void removeEvent(TListNode& Node);
        void TeminntionThread();
        static void Eventloop(XiList *xlist);
    private:
        std::list<TListNode> Nodes;
        std::mutex           NodeMutex;
        std::thread*         NodeThread;
        bool                 workThread;

        friend class XiImageDevice;
};


}

#endif //_XILIST_H_
