#ifndef _worker_h_
#define _worker_h_

#include "commen.h"
#include "sock.h"
//#include "timer.h"

#if __cplusplus 
extern "C"{
#endif

typedef struct
{
	int sockfd;
	int conn_num;
	char info[32];
	char info_len;
}King;

#define MAX_HANDLER_NAME_LAN 64
typedef struct Worker
{
	int epfd;
	pthread_t tid;
	
	time_t last_heart_beat;

	int16_t max_client_idx;
	Sock *clients[MAX_CLIENT_NUM];

	int max_connector_idx;
	Sock *connectors[MAX_CONNECOTR_NUM];

	bool running;

	char buf[WORKER_BUF_LEN];

	lua_State *state;

	LuaMsg *luamsg_sending;

	char msg_handler[MAX_HANDLER_NAME_LAN];
	char tick_handler[MAX_HANDLER_NAME_LAN];
	char rm_handler[MAX_HANDLER_NAME_LAN];
	char conn_handler[MAX_HANDLER_NAME_LAN];
	char trace_handler[MAX_HANDLER_NAME_LAN];
}Worker;

void add_client_to_worker(Worker *worker, Sock *sock);

void remove_client_from_worker(Worker *worker, Sock *sock);

void add_connector_to_worker(Worker *worker, Sock *sock);

void remove_connector_from_worker(Worker *worker, Sock *sock);

void start_workers();

void wait_workers();

void stop_workers();

extern int EPOLL_NUM;
extern int ROBOT_NUM;
extern int g_epollfds[MAX_EPOLL_NUM];
extern Worker g_workers[MAX_EPOLL_NUM];

extern const char *g_self_ip;
extern unsigned short g_self_port;

extern const char *g_god_ip;
extern unsigned short g_god_port;

extern const char *g_wormhole_ip;
extern unsigned short g_wormhole_port;

extern const char *g_redis_ip;
extern unsigned short g_redis_port;

#if __cplusplus
}
#endif


#endif // _worker_h_

