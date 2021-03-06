#ifndef _binding_h_
#define _binding_h_

#if __cplusplus 
extern "C"{
#endif

#include "commen.h"
#include "sock.h"
#include "worker.h"

#define LUA_MODEL_NAME "uu"

lua_State *open_lua();
void init_lua_state(lua_State *state);

void close_lua(lua_State *state);

bool lua_remove_sock(Worker *worker, Sock *sock);

bool lua_on_connected(Worker *worker, Sock *sock, bool success);

bool lua_tick(Worker *worker);

bool lua_msg_handler(lua_State *state, SockMsg* msg);

#if __cplusplus
}
#endif


#endif // _binding_h_
