


#include <stdio.h>
#include <sys/poll.h>


#define NETMAP_WITH_LIBS

#include <net/netmap_user.h>


#pragma pack(1)

#define PROTO_IP 		0x0800
#define PROTO_UDP			17


#define ETH_LEN			6

struct ethhdr {

	unsigned char h_dest[ETH_LEN]; //mac
	unsigned char h_src[ETH_LEN];
	unsigned short h_proto;

};
// sizeof(struct ethhdr) == 14

struct iphdr {

	unsigned char version:4,
				  hdrlen:4;

	unsigned char tos; //
	unsigned short length;

	unsigned short id;

	unsigned short flag:3,
				   offset:13;

	unsigned char ttl;
	unsigned char proto;

	unsigned short check;

	unsigned int sip;
	unsigned int dip;

};

// sizeof(struct ip) == 20

struct udphdr {

	unsigned short sport;
	unsigned short dport;
	unsigned short length;
	unsigned short check;

};

// sizeof(udphdr)  8


struct udppkt {

	struct ethhdr eh; // 14
	struct iphdr ip;  // 20
	struct udphdr udp; // 8

	unsigned char body[0]; // sizeof(body)=0;
	

};

// sizeof(udppkt) = 44
// sizeof()

// f-stack
// dpdk + bsd 

// netmap , dpdk, raw_socket

// netmap:eth0
// eth0
int main() {

	// eth0  --> ens33
	struct nm_desc *nmr = nm_open("netmap:eth0", NULL, 0, NULL);
	if (nmr == NULL) {
		return -1;
	}

	struct pollfd pfd = {0};
	pfd.fd = nmr->fd; //
	pfd.events = POLLIN;
// select/poll  or epoll
// poll --> select
	while (1) {

		int ret = poll(&pfd, 1, -1);
		if (ret < 0) continue;

		if (pfd.revents & POLLIN) {
			struct nm_pkthdr h;
			unsigned char *stream = nm_nextpkt(nmr, &h); // read

			struct ethhdr *eh = (struct ethhdr*)stream;

			// 0x0800
			if (ntohs(eh->h_proto) == PROTO_IP) {

				struct udppkt *udp = (struct udppkt *)stream;

				if (udp->ip.proto == PROTO_UDP) {

					//
					int udp_length = ntohs(udp->udp.length);

					udp->body[udp_length-8] = '\0';

					printf("udp --> %s\n", udp->body);
					
				}

			}

		}

	}

	return 0;
}




