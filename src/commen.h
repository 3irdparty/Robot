#ifndef _commen_h_
#define _commen_h_

#include <stdio.h>  
#include <pthread.h>  
#include <string.h>  
#include <sys/types.h>  
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <signal.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>


#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"



//
//static int countxx = 0;
#define Malloc(n) malloc(n); //countxx++; printf("---> Malloc countxx: %d, file: %s line: %d\n", countxx, __FILE__, __LINE__);
#define Free(p)   free(p) //{ free(p); countxx--; printf("---> Free countxx: %d, file: %s line: %d\n", countxx, __FILE__, __LINE__); }
#define Realloc realloc

//define bool
#define bool int8_t
#define true 1
#define false 0

#define MAX_EPOLL_NUM 16
#define WORKER_PER_EPOLL 1
#define WORKER_BUF_LEN (1024*100)

#define MAX_EVENTS 5

//socket配置
#define MAX_FD_NUM        (0xFFFF+100)   //最大fd值
#define MAX_CONNECOTR_NUM        (0x0FFF)   //最大fd值
#define MAX_CLIENT_NUM    0xFFFF    //最大连接数
#define MAX_MSG_LEN       0xFF //最大消息长度

#define MAX_KING_NUM 20

//数据库配置
#define MAX_REDIS_CMD_LEN (1024*10)


//时间参数配置
#define USEC_PER_SEC    (1000*1000)        //每秒毫秒数
#define HEART_BEAT_TIME 5 //心跳间隔
#define SOCK_TIME_OUT   15                 //连接超时时间
#define SEND_SLEEP_US   5                  //重新发包间隔

#define TICK_TIME 100 //定时器间隔，如果为100毫秒，则每秒至少tick10次

extern const char *LOG_TAG;
#define Log(xx)     printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__)

#define ENABLE_XXTEA_ZLIB 1

#endif // _commen_h_