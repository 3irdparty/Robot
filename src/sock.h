#ifndef _sock_h_
#define _sock_h_

#include <time.h>
#include "commen.h"

//#include "safelist.h"

#define PACK_HEAD_LEN 4
#define MSG_HEAD_NOT_READED  0 //未读到msg长度
#define MSG_HEAD_READED  1     //已读到msg长度
#define MSG_READED  2         //msg收完 

#define REGIST_TIMEOUT 3      //认证超时时间
#define HEARTBEAT_TIMEOUT 30  //心跳超时时间

typedef struct SockMsg SockMsg;

typedef struct LuaMsg
{
	uint16_t len;   //SockMsg.msg指向的buf长度
	int16_t count;  //引用计数
	struct LuaMsg *next;
	char buf[0];    //消息体：去掉整个Msg的head四个字节
}LuaMsg;

typedef struct Sock
{
	//来源
	int fd;

	int epoll_fd;

	//收到数据，对方此消息的总长度
	uint16_t len_total;

	//已经读到的数据长度
	uint16_t len_readed;

	//最后响应时间
	time_t last_pack_time;

	//当前读到的数据的状态
	int8_t status;
	
	//加timer后要做连接的认证超时，连接默认非法，非法连接来包丢弃；
	//在xx时间内session认证通过转为合法，才能进行正常收发包处理，如果超时未认证通过，踢掉；
	bool added;

	//包序号标识，从15-0，对包长进行按位异或
	int8_t time_count;

	//bool is_server;

	//存储读到的数据，head为数据头，读够数据头后动态分配msg长度的buf，并读取msg保存到msg里
	int8_t head[PACK_HEAD_LEN];

	//char *buf;
	//uint16_t len_buf;
	uint16_t len_wrote;

	uint16_t worker_idx;

	int epoll_idx;

	SockMsg *msg;
	LuaMsg *luamsg;
}Sock;

typedef struct SockMsg
{
	Sock *sock;     //Msg来源：sock->fd
	uint16_t len;   //SockMsg.msg指向的buf长度
	int16_t count;//引用计数
	int16_t len_wrote;
	//int16_t cmd;
	//int16_t action;
	struct SockMsg *next;
	
	char *buf;
	char data[0];    //消息体：去掉整个Msg的head四个字节
}SockMsg;



#define MSG_SOCK1(msg)              ((((unsigned long long)(msg->sock)) >> 32) & 0xFFFFFFFF)
#define MSG_SOCK2(msg)              (((unsigned long long)(msg->sock)) & 0xFFFFFFFF)
#define MSG_SOCK(msg)               (msg->sock)
#define MSG_CMD(msg)                ((msg)->data[2])
#define MSG_ACTION(msg)             ((msg)->data[3])
#define MSG_MSG(msg)                (&(msg->data[4]))
#define MSG_LEN(msg)                (msg->len) // (msg->len-PACK_HEAD_LEN)
#define SET_MSG_CMD(buf, cmd)       ((buf[2]) = cmd)
#define SET_MSG_ACTION(buf, action) ((buf[3]) = action)
#define SET_MSG_MSG(buf, msg, len)  strncpy((buf+PACK_HEAD_LEN), (msg), (len)); *(uint16_t*)&(buf) = (len) + PACK_HEAD_LEN;


extern Sock *create_sock(int fd, int epoll_fd, int epoll_idx);

extern void remove_sock(Sock *sock);

extern void heart_beat_sock(Sock *sock, time_t curr_time);

extern SockMsg *create_recv_msg(Sock *sock);

extern LuaMsg *create_lua_msg(char *msg, uint16_t len);

extern void push_lua_msg(Sock *sock, LuaMsg *msg);

extern void msg_retain(SockMsg *msg);

extern void msg_release(SockMsg *msg);

extern void lua_msg_retain(LuaMsg *msg);

extern void lua_msg_release(LuaMsg *msg);

extern Sock *g_sock_fd_map[MAX_FD_NUM];

#endif // _sock_h_
