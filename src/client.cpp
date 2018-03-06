#include "extra.h"
#include "diameter.h"
#include "gtp.h"
#include "network.h"
#include "packet.h"
#include "s1ap.h"
#include "sctp_client.h"
//#include "sctp_server.h"
//#include "security.h"
#include "sync.h"
#include "telecom.h"
#include "udp_client.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <time.h>

int done = 0;
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

map<int, mdata> fdmap;

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
	int ep = epoll_create( MAX_EVENTS);
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
          printf("\nEINPROGRESS\n");
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
  struct epoll_event *events;
  int nevents;
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
    Ran ran;
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
            cout <<"connect after some time\n";
            x--;    
        }
      }
    }

    if(x==0) 
      done1=0;

    
  }

	for(int i=0; i<no_of_connections; i++)
  {


  		ran.init(i);
    	ran.initial_attach(sock_store[i]);
	   	epoll_ctl( ep, EPOLL_CTL_DEL, sock_store[i], &ev);
		  ev.events = EPOLLIN;
    	ev.data.fd = sock_store[i];
    	epoll_ctl( ep, EPOLL_CTL_ADD, sock_store[i], &ev);	
		  fddata.enodeb_s1ap_ue_id = ran.ran_ctx.enodeb_s1ap_ue_id;
      fddata.key = ran.ran_ctx.key;
      fddata.msisdn = ran.ran_ctx.key;
      fddata.imsi = ran.ran_ctx.imsi;
		  fdmap.insert(make_pair(sock_store[i], fddata));

	}
        

    x=no_of_connections;

 	while(1)
 	{
	    nevents = epoll_wait( ep, events, MAX_EVENTS, 100);
	    
	    //cout << "in while\n";

	    for(i=0;i<nevents;i++) 
		{ 

        	cout <<"events " <<  nevents;

		    if (nevents < 0) 
		    {
		      if (errno != EINTR)
		        printf("epoll_wait");
		      break;
		    }

		    if(	(events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) 
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
          int pkt_len;
			    cur_fd = events[i].data.fd;
			    fddata = fdmap[cur_fd];
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

				      if (packet.len <= 0) 
          		{
              		return false;
          		}
  
          		//TRACE(cout << "ran_authenticate: " << " received request for ran: " << ran.ran_ctx.imsi << endl;)
		        packet.extract_s1ap_hdr();
		        //cout << "packetid " <<  packet.s1ap_hdr.enodeb_s1ap_ue_id << endl;
		        //cout << "storedid " << fddata.enodeb_s1ap_ue_id << endl;
		        
	            if(fddata.enodeb_s1ap_ue_id == packet.s1ap_hdr.enodeb_s1ap_ue_id)
	          	{
	            	  cout << "\nData received by Ran " <<  packet.s1ap_hdr.enodeb_s1ap_ue_id << endl;
	              	  x--;
	            }

				      memcpy(fdmap[cur_fd].buf, packet.data, pkt_len);
				      fdmap[cur_fd].len = pkt_len;


          
          

			}

		}

		if(x==0) break;

	}


	x = no_of_connections;

	for(int i=0; i<no_of_connections; i++)
    {
    	
		bool ok;
		fddata = fdmap[sock_store[i]];
		//Packet pkt1;
		//memcpy(pkt1.data, fddata.buf, fddata.len);
		//cout << pkt1.s1ap_hdr.enodeb_s1ap_ue_id << " before extract\n";
		//pkt1.extract_s1ap_hdr();
		//cout << pkt1.s1ap_hdr.enodeb_s1ap_ue_id << " after\n";
		ok = ran[i].authenticate(sock_store[i], fddata);
		if (!ok) 
		{
			TRACE(cout << "ransimulator_simulate:" << " autn failure" << endl;)
		}

		epoll_ctl( ep, EPOLL_CTL_DEL, sock_store[i], &ev);
		ev.events = EPOLLIN;
    ev.data.fd = sock_store[i];
    epoll_ctl( ep, EPOLL_CTL_ADD, sock_store[i], &ev);	
		fddata.enodeb_s1ap_ue_id = ran.ran_ctx.enodeb_s1ap_ue_id;
		fdmap.insert(make_pair(sock_store[i], fddata));
    
		//write( sockid, "done with connection", 50);
	}


return 0;
}