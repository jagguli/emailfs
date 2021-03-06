/***************************************************************************
 *   Copyright (C) 2005 by Steven Joseph  				   *
 *   stevenjose@gmail.com  						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "kern_sock.h"
#include "pop_ops.h"
#define NETLINK_TEST 17

char* kern_recv(int flag);
char *debg[1024];
int debug(char *str){
	static int cnt;
	char temp[200];
	sprintf(temp,"echo \"DAEMON %d: %s \n\">>daemonlog",cnt,str);
	system (temp);
	cnt++;
}

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;


int kern_msg_handler(){
	int ret,i,inum;
	char cmd[100],*tmp,*tmp2;
	
	sock_fd = socket(PF_NETLINK, SOCK_RAW,NETLINK_TEST);
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();  /* self pid */
	src_addr.nl_groups = 0;  /* not in mcast groups */
	bind(sock_fd, (struct sockaddr*)&src_addr,sizeof(src_addr));
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0;   /* For Linux Kernel */
	dest_addr.nl_groups = 0; /* unicast */
	nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	/* Fill the netlink message header */
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid();  /* self pid */
	nlh->nlmsg_flags = 0;
	/* Fill in the netlink message payload */
	strcpy(NLMSG_DATA(nlh), "Hello you!");
	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	while((ret=sendmsg(sock_fd, &msg,0))==-1);
	//printf("DAEMON:return from sendmsg 1:%d\n", ret);
	/* Read message from kernel */
	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	//printf("DAEMON: Waiting for message 1\n");
	recvmsg(sock_fd, &msg,MSG_DONTWAIT);
	//printf("DAEMON: Received message payload from kern: %s\n",NLMSG_DATA(nlh));
	strcpy(cmd,NLMSG_DATA(nlh));
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0;   /* For Linux Kernel */
	dest_addr.nl_groups = 0; /* unicast */
	nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	/* Fill the netlink message header */
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid();  /* self pid */
	nlh->nlmsg_flags = 0;
	/* Fill in the netlink message payload */
	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	//printf("recieved : %s",cmd);
	if(strcmp(cmd,"LISTNEXT")==0){
		//printf("DAEMON: list command");
		ret=get_next_entry(NLMSG_DATA(nlh));
		if(ret==-1)strcpy(NLMSG_DATA(nlh), "\n");
		sendmsg(sock_fd, &msg, MSG_DONTWAIT);
		//printf("DAEMON: return from sendmsg 2: %d\n",ret);
		close(sock_fd);
		return ret;
	}
	else if(strncmp(cmd,"RDPG:",5)==0){
		tmp=cmd;
		tmp=(char *)(tmp)+(7*sizeof(char));
		printf("DAEMON: Readpage for page:%s \n",tmp);
		get_page(tmp,0,(char *)NLMSG_DATA(nlh),MAX_PAYLOAD);
//		strcpy(NLMSG_DATA(nlh), "PAGEREAD");
		printf("DAEMON : Page Read: %s\n",NLMSG_DATA(nlh));
		sendmsg(sock_fd, &msg, MSG_DONTWAIT);
		close(sock_fd);
		return ret;
	}
	else if(strcmp(cmd,"CHECKDCACHE")==0){
		printf("DAEMON: checking dcache validity\n");
		if(check_mail_changes()==1)strcpy(NLMSG_DATA(nlh), "invalid");
		else strcpy(NLMSG_DATA(nlh), "valid");
		sendmsg(sock_fd, &msg, MSG_DONTWAIT);
		close(sock_fd);
		return ret;
	}
	else if(strncmp(cmd,"LKP",3)==0){
		printf("DAEMON: LOOKUP\n");
		tmp=cmd;
		tmp=(char *)(tmp)+(3*sizeof(char));
		printf("name for dcache lookup %s\n",tmp);
		inum=dcache_ilookup(tmp);
		memcpy(NLMSG_DATA(nlh),&inum,sizeof(int));
		sendmsg(sock_fd, &msg, MSG_DONTWAIT);
		close(sock_fd);
		return ret;
	}
	close(sock_fd);
	return ret;
}

