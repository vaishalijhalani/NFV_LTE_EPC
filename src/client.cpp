#include "extra.h"

int done = 0;
struct epoll_event *events;
int nevents;
int ep;
map<int, class Ran> fdmap;
//map<int, class Packet> fdmap1;
void setsock_nonblock(int fd)
{
    int flags;
   
    flags = fcntl(fd, F_GETFL, NULL);

    if(-1 == flags)
    {
        printf("fcntl F_GETFL failed.%s", strerror(errno));
        exit(1);
    }

    flags |= O_NONBLOCK;

    if(-1 == fcntl(fd, F_SETFL, flags))   
    {
        printf("fcntl F_SETFL failed.%s", strerror(errno));
        exit(1);
    }       
}


void run_wait()
{
      
  int x = no_of_connections;
  while(1)
  {
      nevents = epoll_wait( ep, events, MAX_EVENTS, 100);
      
      //cout << "in while\n";

      for(int i=0;i<nevents;i++) 
      { 

          //cout <<"events " <<  nevents;

        if (nevents < 0) 
        {
          if (errno != EINTR)
            printf("epoll_wait");
          break;
        }

        if( (events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) 
        {
          cout<<"ERROR: epoll monitoring failed, closing fd"<<'\n';
          close(events[i].data.fd);
          continue;
        }

      else if(events[i].events & EPOLLIN)
      {


              char * dataptr;
              int cur_fd;
              Packet packet;
              Ran ran;  
              int pkt_len;
              cur_fd = events[i].data.fd;
              ran = fdmap[cur_fd];
              //cout << "before read\n";
              packet.clear_pkt();
              int retval = read_stream(cur_fd, packet.data, sizeof(int));
              if (retval > 0) 
              {
                //cout << "in read\n";
                memmove(&pkt_len, packet.data, sizeof(int) * sizeof(uint8_t));
                packet.clear_pkt();
                retval = read_stream(cur_fd, packet.data, pkt_len);
                packet.data_ptr = 0;
                packet.len = retval;

              }
              ran.store_packet(packet);
              fdmap.erase(cur_fd);
              fdmap.insert(make_pair(cur_fd,ran));
              x--;

      }

    }

    if(x==0) break;

  }

}

/*----------------------------------------------------------------------------*/

int main(int argc, char **argv){

	struct mdata{
	//int act;
	//int initial_fd;
  uint64_t imsi;
  uint64_t msisdn;
  uint64_t key;
	uint8_t buf[500];
	int len;
	uint32_t enodeb_s1ap_ue_id;
};



float packets=0, sec=0;
clock_t before;
float rate;
int x = no_of_connections;
int core = 0;
int ret = -1;

char* hostname = "169.254.9.8";
int portno = 5000;

char* conf_file = "main.conf";
int sockid;
int sock_store[no_of_connections];
int clientport=6500;
map<int, int> sock_num;
set<int> list1;
struct mdata fddata;

	/* create epoll descriptor */
	ep = epoll_create( MAX_EVENTS);
	if (ep < 0) {
		printf("Failed to create epoll descriptor!\n");
		return NULL;
	}

	
  struct epoll_event ev;
	int err = 0;

	
  for(int k=0;k<no_of_connections;k++)
  {
  
      //step 4. socket, setsock_nonblock,bind
      sockid = socket(AF_INET, SOCK_STREAM, 0);
      sock_store[k] = sockid;
      list1.insert(sockid);
      if (sockid < 0) 
      {
        printf("Failed to create listening socket!\n");
        return -1;
      }

      setsock_nonblock(sockid);
      int optval = 1;
      setsockopt(sockid, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &optval, sizeof(optval));
     
      struct sockaddr_in saddr,daddr;


      daddr.sin_family = AF_INET;
      daddr.sin_addr.s_addr = inet_addr(hostname);
      daddr.sin_port = htons(portno);
      
        saddr.sin_port = htons(clientport+k);
        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = INADDR_ANY;

        ret = bind(sockid,(struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
        if (ret < 0) 
        {
          printf("Failed to bind to the socket!\n");
          return -1;
        } 

       //cout << "Trying to connect." << endl;
      
        ret = connect( sockid, (struct sockaddr *)&daddr, sizeof(struct sockaddr_in));
        if((ret == -1) && (errno == EINPROGRESS))
        {
          //printf("\nEINPROGRESS\n");
          ev.events = EPOLLOUT | EPOLLET;
          ev.data.fd = sockid;
          epoll_ctl(ep, EPOLL_CTL_ADD, sockid, &ev);

        }

        else if (ret < 0) 
        {
          if (errno != EINPROGRESS) 
          {
            perror("connect");
            close( sockid);
            return -1;
          }
        }

        else
        {

          cout << "immediately connected\n";
          ev.events = EPOLLIN| EPOLLET;
          ev.data.fd = sockid;
          epoll_ctl(ep, EPOLL_CTL_ADD, sockid, &ev);
          x--;

        }
  }
	
	//step 6. epoll_wait
  
  events = (struct epoll_event *)calloc(MAX_EVENTS, sizeof(struct epoll_event));
  if (!events) {
    printf("Failed to create event struct!\n");
    exit(-1);
  }
  int newsockfd = -1;
  char data[1448]; //1420
  uint64_t lSize=1448;
  
	//time(&dpdkuse_ins.before);
	//dpdkuse_ins.before = clock();	
	//dpdkuse_ins.count = 128;
	int i;
	x = no_of_connections;
    int retval;
    int status;
    int ran_num;
	int done1=1;
  int p=0;

  while(done1)
  {
    nevents = epoll_wait( ep, events, MAX_EVENTS, -1);
    
    if (nevents < 0) 
    {
      if (errno != EINTR)
        perror("epoll_wait");
      break;
    }

    for(int i=0;i<nevents;i++) 
    {
      if (events[i].events & EPOLLOUT) 
      {
      	
        int err = 0;
        socklen_t len = sizeof (int);
        int cret = getsockopt (sockid, SOL_SOCKET, SO_ERROR, &err, &len);  //Check if connect succedd or failed??
        if( (cret == 0) && (err == 0)) //Conn estd
        {
            //cout <<"connect after some time\n";
            x--;    
        }
      }
    }

    if(x==0) 
      done1=0;

    
  }

	for(int i=0; i<no_of_connections; i++)
  {

      Ran ran;
  		ran.init(i);
    	ran.initial_attach(sock_store[i]);
	   	epoll_ctl( ep, EPOLL_CTL_DEL, sock_store[i], &ev);
		  ev.events = EPOLLIN;
    	ev.data.fd = sock_store[i];
    	epoll_ctl( ep, EPOLL_CTL_ADD, sock_store[i], &ev);	
      fdmap.insert(make_pair(sock_store[i],ran));

	}
        
	run_wait();

	for(int i=0; i<no_of_connections; i++)
  {
    	
    Ran ran;  
		bool ok;
		ran = fdmap[sock_store[i]];
    //ran.showpack();
		ok = ran.authenticate(sock_store[i]);
		if (!ok) 
		{
			TRACE(cout << "ransimulator_simulate:" << " autn failure" << endl;)
		}
	}

  run_wait();


  for(int i=0; i<no_of_connections; i++)
  {
      
    Ran ran;  
    bool ok;
    ran = fdmap[sock_store[i]];
    //ran.showpack();
    ok = ran.set_security(sock_store[i]);
    if (!ok) {
      TRACE(cout << "ransimulator_simulate:" << " security setup failure" << endl;)
    }
  }

  run_wait();

  /*for(int i=0; i<no_of_connections; i++)
  {
      
    Ran ran;  
    bool ok;
    ran = fdmap[sock_store[i]];
    //ran.showpack();
    ok = ran.set_eps_session(g_traf_mon, sock_store[i]);
    if (!ok) {
      TRACE(cout << "ransimulator_simulate:" << " eps session setup failure" << endl;)
    }
  }

run_wait();

  for(int i=0; i<no_of_connections; i++)
  {
      
    Ran ran;  
    bool ok;
    ran = fdmap[sock_store[i]];
    //ran.showpack();
    ok = ran.detach();
    if (!ok) {
      TRACE(cout << "ransimulator_simulate:" << " detach failure" << endl;)
    }
  }
*/

return 0;
}