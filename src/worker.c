//#include "timer.h"
#include "worker.h"
//#include "handler.h"
//#include "listener.h"
#include "binding.h"

int EPOLL_NUM = 1;
int ROBOT_NUM = 1;
int g_epollfds[MAX_EPOLL_NUM] = {0};
Worker g_workers[MAX_EPOLL_NUM] = {0};

void remove_client_from_worker(Worker *worker, Sock *sock);

static inline bool handle_msg(SockMsg *msg)
{
#if 1

    bool ret = lua_msg_handler(g_workers[msg->sock->epoll_idx].state , msg);
    msg_release(msg);
    return ret;

#else

    g_workers[msg->sock->epoll_idx].luamsg_sending = create_lua_msg(MSG_CMD(msg), MSG_ACTION(msg), MSG_MSG(msg), MSG_LEN(msg));
    push_lua_msg(msg->sock, g_workers[msg->sock->epoll_idx].luamsg_sending);
    msg_release(msg);

    return true;

#endif    
}

static bool check_need_heart_beat(Worker *worker, time_t curr_time)
{
    if((curr_time - worker->last_heart_beat) > HEART_BEAT_TIME)
    {
        worker->last_heart_beat = curr_time;
        return true;
    }
    return false;
}

static bool write_data(Sock *sock)
{
    if(!sock->luamsg)
    {
        return true;
    }

    bool ret = false;
    int nwrite = 0;
    //printf("[write_data] sockfd: %d\n", sock->fd);


    LuaMsg *luamsg = sock->luamsg;
    while(luamsg)
    {
        
        //printf("--- write_data msg: %d %s \n", luamsg->len, luamsg->buf+4);
        nwrite = write(sock->fd, luamsg->buf, luamsg->len - sock->len_wrote);
        //printf("-- write_data nwrite: %d len: %d\n", nwrite, luamsg->len);


        if(nwrite > 0)
        {

            //printf("[write_data] 111 nwrite: %d %d %d\n", nwrite, luamsg->len, sock->len_wrote);
            sock->len_wrote += nwrite;
            if(luamsg->len == sock->len_wrote)
            {
                LuaMsg *next = luamsg->next;
                lua_msg_release(luamsg);
                luamsg = next;
                //printf("[write_data] 111 nwrite: %d %d %d\n", nwrite, luamsg->len, sock->len_wrote);
                sock->len_wrote = 0;

                ret = true;
            }
            else
            {

                ret = true;
            
                break;
            }
        }
        else if((nwrite < 0) && (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK))
        {

            //printf("[write_data] 222 nwrite: %d\n", nwrite);
            ret = true;
            
            break;
        }
        else
        {
            //printf("[write_data] 333 nwrite: %d\n", nwrite);
            //goto Error;
        }    
    }

    //printf("[write_data] nwrite: %d\n", nwrite);
    sock->luamsg = 0;
//Error:
    return ret;
}


//读数据
static bool read_data(Worker *worker, Sock *sock)
{
    int nread = 0;
    int nsave = 0;
    int lencpy = 0;
    while(1)
    {
        nread = read(sock->fd, worker->buf, WORKER_BUF_LEN);

        //非阻塞，无数据，说明本次数据已经读完，返回下次再读
        if((nread < 0) && (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK))
        {
            goto Ok;
        }
        //对方关闭或其他意外，关闭清理sock
        else if(nread <= 0)
        {
            goto Error;
        }

        nsave = 0;

        //如果sock->msg为空，则说明包头还没读取到
        while(nsave < nread)
        {
            if(!sock->msg)
            {
                if(nread >= (PACK_HEAD_LEN - sock->len_readed))
                {
                    nsave = PACK_HEAD_LEN - sock->len_readed;
                    memcpy(sock->head+sock->len_readed, worker->buf, nsave);

                    sock->msg = create_recv_msg(sock);
                    if(!sock->msg)
                    {
                        goto Error;
                    }
                    sock->len_total = sock->msg->len;
                    
                    //已读长度置0
                    sock->len_readed = 0;
                }
                else
                {
                    memcpy(sock->head+sock->len_readed, worker->buf, nread);
                    sock->len_readed += nread;
                    //nsave = nread;
                    goto Ok;
                }
            }
            lencpy = ((nread - nsave) >= (sock->len_total - sock->len_readed)) ? (sock->len_total - sock->len_readed) : (nread - nsave);
            memcpy(sock->msg->buf + sock->len_readed, worker->buf + nsave, lencpy);
            sock->len_readed += lencpy;
            nsave += lencpy;
            if(sock->len_readed == sock->len_total)
            {
                bool ret = handle_msg(sock->msg);
                sock->msg = 0;
                sock->len_readed = 0;
                //处理过程中出错，关闭清理sock
                if(!ret)
                {
                    goto Error;
                }
            }
        }

        if(nread < WORKER_BUF_LEN)
        {
            break;
        }
    }

Ok:
    return true;

Error:
    return false;
}

#if 0
static bool read_data(Worker *worker, Sock *sock)
{
    //int pack_count = 0;
    //int curr_normal_handler_idx = 0;
    //int curr_global_handler_idx = 0;

    //设置心跳 -> 最后响应时间
    
    int nread = 0;
    while(1)
    {
        //如果sock->msg为空，则说明包头还没读取到
        if(!sock->msg)
        {
            //读数据头，len_readed在sock创建时、读到包头、读完一个msg后都会置0
            nread = read(sock->fd, sock->head+sock->len_readed, PACK_HEAD_LEN - sock->len_readed);
            //非阻塞，无数据，说明本次数据已经读完，返回下次再读
            if((nread < 0) && (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK))
            {
                goto Ok;
            }
            //对方关闭或其他意外，关闭清理sock
            else if(nread <= 0)
            {
                goto Error;
            }

            //读到的长度累加
            sock->len_readed += nread;
            //读到包头长度根据包头的消息长度信息创建msg（动态分配内存）
            if(sock->len_readed == PACK_HEAD_LEN)
            {
                sock->msg = create_recv_msg(sock);
                if(!sock->msg)
                {
                    return false;
                }
                //记录剩余消息总长度
                sock->len_total = sock->msg->len;//-PACK_HEAD_LEN;
                /*if(sock->len_total > MAX_MSG_LEN)
                {
                    return false;
                }*/
                //已读长度置0
                sock->len_readed = 0;
            }
            //读到的长度不够包头，则返回，下次再读
            else
            {
                goto Ok;
            }
        }
        //读取包内容
        nread = read(sock->fd, sock->msg->buf + sock->len_readed, sock->len_total - sock->len_readed);
        //非阻塞，无数据，说明本次数据已经读完，返回下次再读
        if((nread < 0) && (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK))
        {
            return;
        }
        //对方关闭或其他意外，关闭清理sock
        else if(nread <= 0)
        {
            goto Error;
        }

        //读到的长度累加
        sock->len_readed += nread;
        //读到完整包内容，则处理msg
        if(sock->len_readed == sock->len_total)
        {
            bool ret = handle_msg(sock->msg);
            sock->msg = 0;
            sock->len_readed = 0;
            //处理过程中出错，关闭清理sock
            if(!ret)
            {
                goto Error;
            }
            break;
        }
    }

Ok:
    return true;

Error:
    return false;
}
#endif

static void *worker_thread(void *arg) {
    Worker *worker = (Worker*)arg;

    int epfd = worker->epfd;

    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];

    time_t curr_time;

    while (worker->running)
    {
        int n = epoll_wait(epfd, &events, MAX_EVENTS, TICK_TIME);
        
        for(int i = 0; i < n; i++)
        {
            curr_time = time(0);

            int sockfd = events[i].data.fd;
            Sock *sock = g_sock_fd_map[sockfd];
            
            
            if(!sock->added)
            {
                add_client_to_worker(worker, sock);
                sock->added = true;
                //lua_on_connected(worker, sock);
                //epoll_ctl(sock->epoll_fd, EPOLL_CTL_DEL, sock->fd, 0);
            }
                   
            if (events[i].events & EPOLLOUT)
            {
                if (!write_data(sock))
                {
                    lua_remove_sock(worker, sock);
                    remove_client_from_worker(worker, sock);
                    remove_sock(sock);
                    continue;
                }
            }

            

            if (events[i].events & EPOLLIN)
            {
                //heart_beat_sock(sock, curr_time);
                if (!read_data(worker, sock))
                {
                    lua_remove_sock(worker, sock);
                    remove_client_from_worker(worker, sock);
                    remove_sock(sock);
                    continue;
                }
            }

            if(!worker->running)
            {
                break;
            }

            event.events = EPOLLIN | EPOLLONESHOT;
            if(sock->luamsg)
            {
                
                event.events |= EPOLLOUT;  
            }
            event.data.fd = sockfd;
            epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &event);
        }

        lua_tick(worker);
    }
    
    close(epfd);   

    return 0;
}


static void *create_worker(int epfd, int idx)
{
    Worker *worker = &g_workers[idx];
    worker->epfd = epfd;
    worker->last_heart_beat = time(0);
    worker->running = true;
    worker->max_client_idx = -1;

    worker->state = open_lua();
    init_lua_state(worker->state);


    worker->luamsg_sending = 0;

    int err = pthread_create(&worker->tid, NULL, &worker_thread, (void*)worker); //创建线程  

    //printf("[create_worker] %d %d\n", idx, epfd);
    return worker;
}

void add_client_to_worker(Worker *worker, Sock *sock)
{
    worker->max_client_idx += 1;
    sock->worker_idx = worker->max_client_idx;
    worker->clients[worker->max_client_idx] = sock;
}

void remove_client_from_worker(Worker *worker, Sock *sock)
{
    if(sock->worker_idx < worker->max_client_idx)
    {
        Sock *tail = worker->clients[worker->max_client_idx];
        tail->worker_idx = sock->worker_idx;
        worker->clients[tail->worker_idx] = tail;
        worker->max_client_idx -= 1;
    }
    else if(sock->worker_idx == worker->max_client_idx)
    {
        worker->max_client_idx -= 1;
    }
    
}

void add_connector_to_worker(Worker *worker, Sock *sock)
{
    worker->max_client_idx += 1;
    sock->worker_idx = worker->max_client_idx;
    worker->clients[worker->max_client_idx] = sock;
}

void remove_connector_from_worker(Worker *worker, Sock *sock)
{
    if(sock->worker_idx < worker->max_client_idx)
    {
        Sock *tail = worker->clients[worker->max_client_idx];
        tail->worker_idx = sock->worker_idx;
        worker->clients[tail->worker_idx] = tail;
        worker->max_client_idx -= 1;
    }
    else if(sock->worker_idx == worker->max_client_idx)
    {
        worker->max_client_idx -= 1;
    }
    
}

void start_workers()
{
    for (int i = 0; i < EPOLL_NUM; i++)
    {
        g_epollfds[i] = epoll_create(10);
        create_worker(g_epollfds[i], i);
    }
}

void wait_workers()
{
    for (int i = 0; i < EPOLL_NUM; i++)
    {
        pthread_join(g_workers[i].tid, NULL);
    }
}

void stop_workers()
{
    for (int i = 0; i < EPOLL_NUM; i++)
    {
        g_workers[i].running = false;
    }
}