#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>

#include <iostream>

using namespace std;


uint32_t getDelta(struct timeval frist, struct timeval second)
{
    if(second.tv_sec >= frist.tv_sec)
    {
        return ((second.tv_sec-frist.tv_sec)*1000000)+second.tv_usec - frist.tv_usec;
    }else{
        return 0;
    }
}

void TestMemcpyChar(int size)
{
    char* srcbuff  =(char*) malloc(size);
    char* destbuff =(char*) malloc(size);

    memset(destbuff, 0, size);
    memset(srcbuff, 100, size);

    struct timeval tv, tv1, tv2, tv3, tv4;
    gettimeofday(&tv, NULL);
    memcpy(srcbuff, destbuff, size);
    gettimeofday(&tv1, NULL);
    memcpy(destbuff, srcbuff, size);
    gettimeofday(&tv2, NULL);
    memcpy(srcbuff, destbuff, size);
    gettimeofday(&tv3, NULL);
    memcpy(destbuff, srcbuff, size);
    gettimeofday(&tv4, NULL);

    free(destbuff);
    free(srcbuff);
    cout<<"Test memcpy , malloc with unsinged char type\n";
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv,  tv1)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv1, tv2)<<endl;
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv2, tv3)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv3, tv4)<<endl;
    cout<<"   copy 4 times, use total time:"<<getDelta(tv,  tv4)<<endl;
}

void TestMemcpyUchar(int size)
{
    unsigned char* srcbuff  =(unsigned char*) malloc(size);
    unsigned char* destbuff =(unsigned char*) malloc(size);

    memset(destbuff, 0, size);
    memset(srcbuff, 100, size);

    struct timeval tv, tv1, tv2, tv3, tv4;
    gettimeofday(&tv, NULL);
    memcpy(srcbuff, destbuff, size);
    gettimeofday(&tv1, NULL);
    memcpy(destbuff, srcbuff, size);
    gettimeofday(&tv2, NULL);
    memcpy(srcbuff, destbuff, size);
    gettimeofday(&tv3, NULL);
    memcpy(destbuff, srcbuff, size);
    gettimeofday(&tv4, NULL);

    free(destbuff);
    free(srcbuff);
    cout<<"Test memcpy , malloc with unsinged char type\n";
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv,  tv1)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv1, tv2)<<endl;
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv2, tv3)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv3, tv4)<<endl;
    cout<<"   copy 4 times, use total time:"<<getDelta(tv,  tv4)<<endl;
}


void TestOperatorChar(int size)
{
    char* srcbuff  =(char*) malloc(size);
    char* destbuff =(char*) malloc(size);

    memset(destbuff, 0, size);
    memset(srcbuff, 100, size);

    int i;
    struct timeval tv, tv1, tv2, tv3, tv4;
    gettimeofday(&tv, NULL);
    for(i=0 ; i<size; i++)
        srcbuff[i] = destbuff[i];
    gettimeofday(&tv1, NULL);
    for(i=0 ; i<size; i++)
        destbuff[i] = srcbuff[i];
    gettimeofday(&tv2, NULL);
    for(i=0 ; i<size; i++)
        srcbuff[i] = destbuff[i];
    gettimeofday(&tv3, NULL);
    for(i=0 ; i<size; i++)
        destbuff[i] = srcbuff[i];
    gettimeofday(&tv4, NULL);

    free(destbuff);
    free(srcbuff);
    cout<<"\n";
    cout<<"Test operator char , malloc with unsinged char type\n";
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv,  tv1)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv1, tv2)<<endl;
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv2, tv3)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv3, tv4)<<endl;
    cout<<"   copy 4 times, use total time:"<<getDelta(tv,  tv4)<<endl;
}

void TestOperatorShort(int size)
{
    short* srcbuff  =(short*) malloc(size);
    short* destbuff =(short*) malloc(size);

    memset(destbuff, 0, size);
    memset(srcbuff, 100, size);

    int len = size/2;
    int i;
    struct timeval tv, tv1, tv2, tv3, tv4;
    gettimeofday(&tv, NULL);
    for(i=0 ; i<len; i++)
        srcbuff[i] = destbuff[i];
    gettimeofday(&tv1, NULL);
    for(i=0 ; i<len; i++)
        destbuff[i] = srcbuff[i];
    gettimeofday(&tv2, NULL);
    for(i=0 ; i<len; i++)
        srcbuff[i] = destbuff[i];
    gettimeofday(&tv3, NULL);
    for(i=0 ; i<len; i++)
        destbuff[i] = srcbuff[i];
    gettimeofday(&tv4, NULL);

    free((char*)destbuff);
    free((char*)srcbuff);
    cout<<"Test operator short , malloc with unsinged char type\n";
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv,  tv1)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv1, tv2)<<endl;
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv2, tv3)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv3, tv4)<<endl;
    cout<<"   copy 4 times, use total time:"<<getDelta(tv,  tv4)<<endl;
}


void TestOperatorInt(int size)
{
    int* srcbuff  =(int*) malloc(size);
    int* destbuff =(int*) malloc(size);

    memset(destbuff, 0, size);
    memset(srcbuff, 100, size);

    int len = size/4;
    int i;
    struct timeval tv, tv1, tv2, tv3, tv4;
    gettimeofday(&tv, NULL);
    for(i=0 ; i<len; i++)
        srcbuff[i] = destbuff[i];
    gettimeofday(&tv1, NULL);
    for(i=0 ; i<len; i++)
        destbuff[i] = srcbuff[i];
    gettimeofday(&tv2, NULL);
    for(i=0 ; i<len; i++)
        srcbuff[i] = destbuff[i];
    gettimeofday(&tv3, NULL);
    for(i=0 ; i<len; i++)
        destbuff[i] = srcbuff[i];
    gettimeofday(&tv4, NULL);

    free((char*)destbuff);
    free((char*)srcbuff);
    cout<<"Test operator int , malloc with unsinged char type\n";
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv,  tv1)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv1, tv2)<<endl;
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv2, tv3)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv3, tv4)<<endl;
    cout<<"   copy 4 times, use total time:"<<getDelta(tv,  tv4)<<endl;
}

void TestOperatorint64(int size)
{
    unsigned long long* srcbuff  =(unsigned long long*) malloc(size);
    unsigned long long* destbuff =(unsigned long long*) malloc(size);

    memset(destbuff, 0, size);
    memset(srcbuff, 100, size);

    int len = size/8;

    int i;
    struct timeval tv, tv1, tv2, tv3, tv4;
    gettimeofday(&tv, NULL);
    for(i=0 ; i<len; i++)
        srcbuff[i] = destbuff[i];
    gettimeofday(&tv1, NULL);
    for(i=0 ; i<len; i++)
        destbuff[i] = srcbuff[i];
    gettimeofday(&tv2, NULL);
    for(i=0 ; i<len; i++)
        srcbuff[i] = destbuff[i];
    gettimeofday(&tv3, NULL);
    for(i=0 ; i<len; i++)
        destbuff[i] = srcbuff[i];
    gettimeofday(&tv4, NULL);

    free((char*)destbuff);
    free((char*)srcbuff);
    cout<<"Test operator long long , malloc with unsinged char type\n";
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv,  tv1)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv1, tv2)<<endl;
    cout<<"   from buff1->buff2, use time:"<<getDelta(tv2, tv3)<<endl;
    cout<<"   from buff2->buff1, use time:"<<getDelta(tv3, tv4)<<endl;
    cout<<"   copy 4 times, use total time:"<<getDelta(tv,  tv4)<<endl;
}








int main()
{
    TestMemcpyUchar(1024*1280);
    TestMemcpyChar(1024*1280);
    TestOperatorChar(1024*1280);
    TestOperatorShort(1024*1280);
    TestOperatorInt(1024*1280);
    TestOperatorint64(1024*1280);

    return 0;



    char* destbuff =(char*) malloc(1024*1280);
    char* srcbuff  =(char*) malloc(1024*1280);
    memset(destbuff, 0, 1024*1280);
    memset(srcbuff, 100, 1024*1280);

    int i;
    for(i=0 ; i<5; i++)
    {
        struct timeval tv, tv1, tv2, tv3, tv4;
        gettimeofday(&tv, NULL);
        memcpy(srcbuff, destbuff, 1024*1280);
        gettimeofday(&tv1, NULL);
        memcpy(destbuff, srcbuff, 1024*1280);
        gettimeofday(&tv2, NULL);
        memcpy(srcbuff, destbuff, 1024*1280);
        gettimeofday(&tv3, NULL);
        memcpy(destbuff, srcbuff, 1024*1280);
        gettimeofday(&tv4, NULL);
        printf("test..\n");
        printf("time1:%u.%u\n",tv.tv_sec ,tv.tv_usec);
        printf("time2:%u.%u user time:%u\n",tv1.tv_sec,tv1.tv_usec,tv1.tv_usec > tv.tv_usec? tv1.tv_usec - tv.tv_usec:tv.tv_usec+1000000 - tv1.tv_usec);
        printf("time3:%u.%u user time:%u\n",tv2.tv_sec,tv2.tv_usec,tv2.tv_usec > tv1.tv_usec? tv2.tv_usec - tv1.tv_usec:tv1.tv_usec+1000000 - tv2.tv_usec);
        printf("time4:%u.%u user time:%u\n",tv3.tv_sec,tv3.tv_usec,tv3.tv_usec > tv2.tv_usec? tv3.tv_usec - tv2.tv_usec:tv2.tv_usec+1000000 - tv3.tv_usec  );
        printf("time5:%u.%u user time:%u\n",tv4.tv_sec,tv4.tv_usec,tv4.tv_usec > tv3.tv_usec? tv4.tv_usec - tv3.tv_usec:tv3.tv_usec+1000000 - tv4.tv_usec  );
    }
    free(destbuff);
    free(srcbuff);
    return 0;
}
