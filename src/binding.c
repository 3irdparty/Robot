#include "commen.h"
#include "binding.h"
#include "sock.h"
#include "worker.h"
#include <zlib.h> 
#include "xxtea.h"

#if __cplusplus 
extern "C"{
#endif

const char *xxtea_key = "jiangxuejun";

#define MAX_LUA_MSG_LEN (1024*10)
static char msg_buf[MAX_LUA_MSG_LEN] = { 0 };

int conn = 0;
bool lua_on_connected(Worker *worker, Sock *sock, bool success)
{
	int functionIndex = -3;
	//printf("------ lua_on_connected %d %d\n", sock->fd, ++conn);
	lua_getglobal(worker->state, worker->conn_handler);
	lua_pushinteger(worker->state, sock->fd);
	lua_pushboolean(worker->state, success);
	int traceback = 0;
    lua_getglobal(worker->state, worker->trace_handler); 
    if (!lua_isfunction(worker->state, -1))
    {
        lua_pop(worker->state, 1);                                            /* L: ... func arg1 arg2 ... */
    }
    else
    {
        lua_insert(worker->state, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
        traceback = functionIndex - 1;
    }

	bool ret = lua_pcall(worker->state, 2, 0, traceback);
	if(ret != 0)
	{
		Log();
		printf("lua_pcall failed: %s\n", lua_tostring(worker->state, -1));
		lua_pop(worker->state, 2);
		lua_settop(worker->state, 0);
		return false;
	}

	lua_pop(worker->state, 2);
	lua_settop(worker->state, 0);
	return true;
}

bool lua_remove_sock(Worker *worker, Sock *sock)
{
	int functionIndex = -2;
	lua_getglobal(worker->state, worker->rm_handler);
	lua_pushinteger(worker->state, sock->fd);
	int traceback = 0;
    lua_getglobal(worker->state, worker->trace_handler); 
    if (!lua_isfunction(worker->state, -1))
    {
        lua_pop(worker->state, 1);                                            /* L: ... func arg1 arg2 ... */
    }
    else
    {
        lua_insert(worker->state, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
        traceback = functionIndex - 1;
    }

	bool ret = lua_pcall(worker->state, 1, 0, traceback);
	if(ret != 0)
	{
		Log();
		printf("lua_pcall failed: %s\n", lua_tostring(worker->state,-1));
		lua_pop(worker->state, 2);
		lua_settop(worker->state, 0);
		return false;
	}

	lua_pop(worker->state, 2);
	lua_settop(worker->state, 0);
	return true;
}

bool lua_tick(Worker *worker)
{
	int functionIndex = -1;
	lua_getglobal(worker->state, worker->tick_handler);
	int traceback = 0;
    lua_getglobal(worker->state, worker->trace_handler); 
    if (!lua_isfunction(worker->state, -1))
    {
        lua_pop(worker->state, 1);                                            /* L: ... func arg1 arg2 ... */
    }
    else
    {
        lua_insert(worker->state, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
        traceback = functionIndex - 1;
    }

	bool ret = lua_pcall(worker->state, 0, 0, traceback);
	if(ret != 0)
	{
		Log();
		printf("lua_pcall failed: %s\n", lua_tostring(worker->state,-1));
		lua_pop(worker->state, 2);
		lua_settop(worker->state, 0);
		return false;
	}

	lua_pop(worker->state, 2);
	lua_settop(worker->state, 0);
	return true;
}

bool lua_msg_handler(lua_State *state, SockMsg* msg)
{
	int functionIndex = -3;
	Worker *worker = &g_workers[msg->sock->epoll_idx];
	lua_getglobal(worker->state, worker->msg_handler);
	lua_pushinteger(state, (int)msg->sock->fd);

#if ENABLE_XXTEA_ZLIB
	uLong buf_len = MAX_LUA_MSG_LEN;
	uncompress((Bytef*)msg_buf, &buf_len, (Bytef*)MSG_MSG(msg), MSG_LEN(msg));
	xxtea_long xxlen = 0;
	char* xxresult = (char*)xxtea_decrypt((unsigned char*)msg_buf, buf_len, (unsigned char*)xxtea_key, strlen(xxtea_key), &xxlen);
	lua_pushlstring(state, xxresult, xxlen);
	free(xxresult);
#else
	//printf("lua_msg_handler: %d %s\n", MSG_MSG(msg), MSG_LEN(msg));
	lua_pushlstring(state, MSG_MSG(msg), MSG_LEN(msg));
#endif
	int traceback = 0;
    lua_getglobal(worker->state, worker->trace_handler); 
    if (!lua_isfunction(worker->state, -1))
    {
        lua_pop(worker->state, 1);                                            /* L: ... func arg1 arg2 ... */
    }
    else
    {
        lua_insert(worker->state, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
        traceback = functionIndex - 1;
    }

	bool ret = lua_pcall(worker->state, 2, 0, traceback);
	if(ret != 0)
	{
		Log();
		printf("lua_pcall failed: %s\n", lua_tostring(worker->state, -1));
		lua_pop(worker->state, 2);
		lua_settop(worker->state, 0);
		return false;
	}
	ret = lua_toboolean(state, -1);

	lua_pop(worker->state, 3);
	lua_settop(worker->state, 0);

    return ret;
}


//********************************************************************
static int lua_regist_handlers(lua_State* state)
{

	Worker *worker = 0;
	for(int i=0; i<EPOLL_NUM; i++)
	{
		if(state == g_workers[i].state)
		{

			worker = &g_workers[i];
			break;
		}
	}

	strcpy(worker->msg_handler, lua_tostring(state, 1));
	strcpy(worker->tick_handler, lua_tostring(state, 2));
	strcpy(worker->rm_handler, lua_tostring(state, 3));
	strcpy(worker->conn_handler, lua_tostring(state, 4));
	strcpy(worker->trace_handler, lua_tostring(state, 5));

	//printf("%s %s %s %s %s %s\n", __FILE__, worker->msg_handler, worker->tick_handler, worker->rm_handler, worker->conn_handler, worker->trace_handler);
	return 1;
}

#if 0
static int lua_connect(lua_State* state)
{
	const char *ip = lua_tostring(state, 1);
	int port = lua_tointeger(state, 2);
	
	//bool ret = connect_to(ip, port);
	
	//lua_pushboolean(state, ret);

	return 1;
}
#endif

#if 0
static int lua_connect_to(lua_State* state)
{
	int workerIdx = 0;
	while(state != g_workers[workerIdx].state)
	{
		workerIdx++;
	}

	const char *ip = lua_tostring(state, 1);
	unsigned short port = lua_tointeger(state, 2);
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;

    struct hostent *host;
	if(host = gethostbyname(ip))
	{
        memcpy(&addr.sin_addr, host->h_addr_list[0], host->h_length);
	}
    else
	{
		addr.sin_addr.s_addr = inet_addr(ip);
	}

    //addr.sin_addr.s_addr = inet_addr(host);
    addr.sin_port = htons(port);

	
    Sock *sock = create_sock(sockfd, g_epollfds[workerIdx], workerIdx);
    g_sock_fd_map[sockfd] = sock;
    
    
    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
    {
        //printf("connect success %d %d !\n", workerIdx, g_epollfds[workerIdx]);

        int flags = fcntl(sockfd, F_GETFL, 0);
		fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	    struct epoll_event event;
	    event.data.fd = sockfd;
	    //event.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;
	    event.events = EPOLLIN | EPOLLONESHOT;

	    epoll_ctl(g_epollfds[workerIdx], EPOLL_CTL_ADD, sockfd, &event);

	    lua_pushinteger(state, sockfd);
    }
    else
    {
    	//printf("connecting %d %d!\n", workerIdx, g_epollfds[workerIdx]);
    	lua_pushinteger(state, -1);
    }

    
    //lua_pop(state, 1);

    return 1;
}
#endif

static int lua_connect_to(lua_State* state)
{
	int workerIdx = 0;
	while(state != g_workers[workerIdx].state)
	{
		workerIdx++;
	}

	const char *ip = lua_tostring(state, 1);
	unsigned short port = lua_tointeger(state, 2);
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    
    //sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;

    struct hostent *host;
	if(host = gethostbyname(ip))
	{
        memcpy(&addr.sin_addr, host->h_addr_list[0], host->h_length);
	}
    else
	{
		addr.sin_addr.s_addr = inet_addr(ip);
	}

    //addr.sin_addr.s_addr = inet_addr(host);
    addr.sin_port = htons(port);

	
    Sock *sock = create_sock(sockfd, g_epollfds[workerIdx], workerIdx);
    g_sock_fd_map[sockfd] = sock;
    
    int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
    {
    }
    else
    {
    }
    
    heart_beat_sock(sock, time(0));

    lua_pushinteger(state, sockfd);
    //printf("workerIdx: %d \n", workerIdx);
    add_connector_to_worker(&g_workers[workerIdx], sock);
    
    //lua_pop(state, 1);

    return 1;
}

static int lua_create_lua_msg(lua_State* state)
{
	int sockfd = lua_tointeger(state, 1);
	size_t len;
	const char *str = lua_tolstring(state, 2, &len);

#if ENABLE_XXTEA_ZLIB
	
	xxtea_long xxlen = 0;
	unsigned char* xxresult = xxtea_encrypt((unsigned char*)str, len, (unsigned char*)xxtea_key, strlen(xxtea_key), &xxlen);

	uLong buf_len = MAX_LUA_MSG_LEN;
	compress((Bytef*)msg_buf, &buf_len, (Bytef*)xxresult, xxlen);
	
	#if 0
	printf("---- buf_len: %d\n", buf_len);
	printf("---- sizeof(msg_buf): %d\n", sizeof(msg_buf));
	printf("---- sockfd: %p\n", sockfd);
	printf("---- sock: %p\n", g_sock_fd_map[sockfd]);
	printf("---- epoll_idx: %d\n", g_sock_fd_map[sockfd]->epoll_idx);
	printf("---- luamsg_sending: %p\n", g_workers[g_sock_fd_map[sockfd]->epoll_idx].luamsg_sending);
	#endif
	
	g_workers[g_sock_fd_map[sockfd]->epoll_idx].luamsg_sending = create_lua_msg(msg_buf, buf_len);

	free(xxresult);
#else
	g_workers[g_sock_fd_map[sockfd]->epoll_idx].luamsg_sending = create_lua_msg(str, len);
#endif

	return 1;
}

static int lua_send_msg(lua_State* state)
{
	//Server *server = (Server*)lua_tointeger(state, 1);
	int sockfd = lua_tointeger(state, 1);
	Sock *sock = g_sock_fd_map[sockfd];

	//int msgptr = lua_tointeger(state, 2);
	//LuaMsg *msg = (LuaMsg*)msgptr;

	push_lua_msg(sock, g_workers[sock->epoll_idx].luamsg_sending);

	//printf("[lua send msg] %d\n", sockfd);
	return 1;
}

static const struct luaL_reg c_lua_api_lib[] = 
{
	//{ "redisClient", lua_redis_client },
	//{ "redisGet", lua_redis_get },
	//{ "redisSet", lua_redis_set },
	{ "registHandlers", lua_regist_handlers },
	//{ "setSockServer", lua_set_sock_server },
	{ "connectTo", lua_connect_to },
	{ "createMsg", lua_create_lua_msg },
	{ "sendMsg", lua_send_msg },
	{ NULL, NULL },
};

lua_State *open_lua()
{
	return luaL_newstate();
}

void init_lua_state(lua_State *state)
{
	//lua_State *state = luaL_newstate();
	luaL_openlibs(state);

	luaL_register(state, LUA_MODEL_NAME, c_lua_api_lib);
	

	int ret = luaL_loadfile(state, "init.lua");
	if(ret != 0)
	{
		Log();
		printf("lua_pcall failed: %s\n", lua_tostring(state,-1));
		return;
	}

	ret = lua_pcall(state, 0, 0, 0);

	if(ret != 0)
	{
		Log();
		printf("lua_pcall failed: %s\n", lua_tostring(state,-1));
		return;
	}
	lua_pop(state, 1);

	return;
}

void close_lua(lua_State *state)
{
	lua_close(state);
}

#if __cplusplus
}
#endif
