#include <sys/epoll.h>
#include "sock.h"

#if __cplusplus 
extern "C"{
#endif

Sock *g_sock_fd_map[MAX_FD_NUM] = {0};



inline void heart_beat_sock(Sock *sock, time_t curr_time)
{
	sock->last_pack_time = curr_time;
}

int sock_num = 0;
Sock *create_sock(int fd, int epoll_fd, int epoll_idx)
{
	Sock *sock = (Sock*)Malloc(sizeof(Sock));
	memset(sock, 0, sizeof(Sock));
	sock->fd = fd;
	sock->epoll_idx = epoll_idx;
	//if(worker_idx != 0)
	{
		//printf("--------------------- xxxxx %d\n", sock->worker_idx);
	}
	sock->added = false;

	sock->epoll_fd = epoll_fd;

	heart_beat_sock(sock, time(0));

	sock_num++;
	return sock;
}

static void free_sock(void *data)
{
	Sock *sock = (Sock*)data;
	
	Free(sock);
}

void remove_sock(Sock *sock)
{
	g_sock_fd_map[sock->fd] = 0;
	if(sock->luamsg)
	{
		LuaMsg *msg = sock->luamsg;
		while(msg)
		{
			lua_msg_release(msg);
			msg = msg->next;
		}
	}
	epoll_ctl(sock->epoll_fd, EPOLL_CTL_DEL, sock->fd, 0);
	close(sock->fd);
	
	sock_num--;
	Free(sock);
	//printf("[remove_sock] sock_num: %d\n", sock_num);
}

SockMsg *create_recv_msg(Sock *sock)
{
	int len = *(int*)&(sock->head);
	//printf("len: %d / %d\n", len, MAX_MSG_LEN);
	if(len < 0 || len > MAX_MSG_LEN)
	{
		Log();
		return 0;
	}
	int len_total = sizeof(SockMsg) + len + PACK_HEAD_LEN;
	SockMsg *msg = (SockMsg*)Malloc(len_total);
	memset(msg, 0, len_total);
	msg->sock = sock;
	*(int*)msg->data = *(int*)sock->head;
	msg->len = len;
	msg->buf = msg->data + PACK_HEAD_LEN;
	msg->count++;	
	//printf("........ create_recv_msg : %d %s\n", msg->len, msg->buf);
	return msg;
}

/*
inline SockMsg *create_recv_msg(Sock *sock)
{
	int len = *(uint16_t*)&sock->head;
	int len_total = sizeof(SockMsg) + len;
	SockMsg *msg = (SockMsg*)Malloc(len_total);
	memset(msg, 0, len_total);
	msg->sock = sock;
	*(int*)msg->data = *(int*)sock->head;

	msg->len = len;
	msg->buf = msg->data + PACK_HEAD_LEN;
	msg->count++;	

	return msg;
}
*/

LuaMsg *create_lua_msg(char *str, uint16_t len)
{
	//printf("000 str: %p len: %d\n", str, len);
	LuaMsg *luamsg = (LuaMsg*)Malloc(sizeof(LuaMsg) + PACK_HEAD_LEN + len);
	//printf("111 luamsg: %p str: %p len: %d\n", luamsg, str, len);
	luamsg->len = PACK_HEAD_LEN + len;
	luamsg->count = 0;
	//printf("222 luamsg: %p str: %p len: %d\n", luamsg, str, len);
	luamsg->next = 0;
	*(int*)luamsg->buf = len;
	//printf("333 luamsg: %p str: %p len: %d\n", luamsg, str, len);
	memcpy(luamsg->buf + PACK_HEAD_LEN, str, len);
	//printf("444 luamsg: %p str: %p len: %d\n", luamsg, str, len);
	return luamsg;
}

/*
inline LuaMsg *create_lua_msg(int8_t cmd, int8_t action, char *str, uint16_t len)
{
	//printf("[create lua msg 11] %d %d %d\n", cmd, action, len);
	LuaMsg *luamsg = (LuaMsg*)Malloc(sizeof(LuaMsg) + PACK_HEAD_LEN + len);
	luamsg->len = PACK_HEAD_LEN + len;
	luamsg->count = 0;
	luamsg->next = 0;
	*(uint16_t*)luamsg->buf = PACK_HEAD_LEN + len;
	luamsg->buf[2] = cmd;
	luamsg->buf[3] = action;
	memcpy(luamsg->buf + PACK_HEAD_LEN, str, len);
	//printf("[create lua msg 22] %d %d %d\n", cmd, action, len);
	return luamsg;
}
*/

void push_lua_msg(Sock *sock, LuaMsg *msg)
{
	if(!sock->luamsg)
	{
		sock->luamsg = msg;
		//printf("[push lua msg] 111\n");
	}
	else
	{
		LuaMsg *luamsg = sock->luamsg;
		while(luamsg->next)
		{
			luamsg = luamsg->next;
		}
		luamsg->next = msg;
		//printf("[push lua msg] 222\n");
	}
	lua_msg_retain(msg);

	struct epoll_event event;
    event.data.fd = sock->fd;
    event.events = EPOLLOUT | EPOLLONESHOT;
    //event.events = EPOLLOUT | EPOLLONESHOT;
    //epoll_ctl(sock->epoll_fd, EPOLL_CTL_DEL, sock->fd, 0);
    //epoll_ctl(sock->epoll_fd, EPOLL_CTL_ADD, sock->fd, &event);
    epoll_ctl(sock->epoll_fd, EPOLL_CTL_MOD, sock->fd, &event);
}

void msg_retain(SockMsg *msg)
{
	msg->count++;
}

void msg_release(SockMsg *msg)
{
	msg->count--;
	//printf("[msg_release] 111 %d\n", msg->count);
	if(msg->count == 0)
	{
		//msg->sock->msg = 0;
		//printf("[msg_release] 222 %d\n", msg->count);
		Free(msg);
	}
	//printf("[msg_release] 333 %d\n", msg->count);
}

void lua_msg_retain(LuaMsg *msg)
{
	msg->count++;
	//printf("[lua msg retain] %d\n", msg->count);
}

void lua_msg_release(LuaMsg *msg)
{
	msg->count--;
	//printf("[lua_msg_release] 111 %d\n", msg->count);
	if(msg->count == 0)
	{
		//printf("[lua_msg_release] 2222 %d\n", msg->count);
		//msg->sock->msg = 0;
		Free(msg);
	}
	//printf("[lua_msg_release] 333 %d\n", msg->count);
}

#if __cplusplus
}
#endif
