#include "commen.h"
//#include "listener.h"
#include "worker.h"

const char *LOG_TAG = "";

int main(int argc, char **argv)
{
	
	// Wormhole
	/*if(argc == 3)
	{
		EPOLL_NUM = atoi(argv[1]);
		ROBOT_NUM = atoi(argv[2]);
	}
	*/	
	
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	system("clear");
	
	printf("****************** Start Robots ******************\n");
    start_workers();

    wait_workers();
    
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
			"****************** Stop Robots ******************\n");

    return 0;
}
