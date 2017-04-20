#include "XiList.h"

#include "XiLog.h"

namespace Xilinx {

XiList::XiList()
{
    LOG(INFO_LEVEL, "start ..构造");
    NodeThread = new std::thread(Eventloop, this);
    LOG(INFO_LEVEL, "end   ..构造");
}


XiList::~XiList()
{
    LOG(INFO_LEVEL, "start ..虚构");
    TeminntionThread();
    NodeThread->join();
    delete NodeThread;

    if(!Nodes.empty())
        Nodes.clear();
    LOG(INFO_LEVEL, "end   ..虚构");
}

void XiList::TeminntionThread()
{
    workThread = false;
}

void XiList::addEvent(TListNode& Node)
{
    NodeMutex.lock();
    Nodes.push_back(Node);
    NodeMutex.unlock();
}

void XiList::removeEvent(TListNode& Node)
{
    NodeMutex.lock();
    for(std::list<TListNode>::iterator it=Nodes.begin(); it != Nodes.end(); ++it)
    {
        if(it->name == Node.name)
        {
            Nodes.erase(it);
            break;
        }
    }
    NodeMutex.unlock();
}

void XiList::Eventloop(XiList *xlist)
{
    xlist->workThread = true;
    while(xlist->workThread)
    {
        usleep(5000);
        struct timeval currnetime;
        gettimeofday(&currnetime, NULL);
        xlist->NodeMutex.lock();
        for(std::list<TListNode>::iterator it=xlist->Nodes.begin(); it != xlist->Nodes.end(); ++it)
        {
            if(it->pCondition(&(*it)))
            {
                if(it->pExecute != NULL)
                {
                    it->pExecute(&(*it));
                }
            }
        }
        xlist->NodeMutex.unlock();
    }
}



}
