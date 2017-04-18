#include "XiList.h"

namespace Xilinx {

XiList::XiList()
{
    std::cout<<"Create XiList"<<std::endl;
    NodeThread = new std::thread(Eventloop, this);
}


XiList::~XiList()
{
    TeminntionThread();
    NodeThread->join();
    delete NodeThread;

    if(!Nodes.empty())
        Nodes.clear();
    std::cout<<"Destroy XiList"<<std::endl;
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
