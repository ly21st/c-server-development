

#include <stdio.h>
#include <rte_mbuf.h>
#include <rte_ethdev.h>
#include <rte_kni.h>


#define NB_MBUF					8196
#define MEMPOOL_CACHE_SZ		256

#define NB_RXD					1024
#define NB_TXD					1024
#define MAX_PACKET_SZ 			2048

#define KNI_ENET_HEADER_SIZE    14
#define KNI_ENET_FCS_SIZE       4


static struct rte_mempool *pktmbuf_pool = NULL;
static uint32_t ports_mask = 0;

static struct rte_eth_conf port_conf = {
	.txmode = {
		.mq_mode = ETH_MQ_TX_NONE,
	}
};


int parse_args(int argc, char *argv[]) {

	return 0;

}

static void init_port(uint16_t port) { // eth_dev

	struct rte_eth_dev_info devinfo;

	printf("Initialising port %u\n", (unsigned)port);
	//fflush(stdout);
	
	//get dev info
	rte_eth_dev_info_get(port, &devinfo);
	if (devinfo.tx_offload_capa & DEV_TX_OFFLOAD_MBUF_FAST_FREE)
		port_conf.txmode.offloads |= DEV_TX_OFFLOAD_MBUF_FAST_FREE;
	
	int retval = rte_eth_dev_configure(port, 1, 1, &port_conf);
	if (retval < 0) {
		rte_exit(EXIT_FAILURE, "Could not configure port%u (%d)\n", port, retval);
	}

	uint16_t nb_rxd = NB_RXD;
	uint16_t nb_txd = NB_TXD;
	
	rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);

	printf("Configure ethdev %u\n", (unsigned)port);
	// setup rx queue
	struct rte_eth_rxconf rxq_conf;

	memcpy(&rxq_conf, &devinfo.default_rxconf, sizeof(struct rte_eth_rxconf));
	rxq_conf.offloads = port_conf.rxmode.offloads;

	retval = rte_eth_rx_queue_setup(port, 0, NB_RXD, rte_eth_dev_socket_id(port), 
		&rxq_conf, pktmbuf_pool);
	if (retval < 0) {
		rte_exit(EXIT_FAILURE, "Could not setup up RX queue for port(%u) %d\n",
			(unsigned)port, retval);
	}

	printf("Setup tx queue ethdev %u\n", (unsigned)port);
	
	// setup tx queue
	struct rte_eth_txconf txq_conf;

	memcpy(&txq_conf, &devinfo.default_txconf, sizeof(struct rte_eth_txconf));
	txq_conf.offloads = port_conf.txmode.offloads;

	retval = rte_eth_tx_queue_setup(port, 0, NB_TXD, rte_eth_dev_socket_id(port), 
		&txq_conf);
	if (retval < 0) {
		rte_exit(EXIT_FAILURE, "Could not setup up TX queue for port(%u) %d\n",
			(unsigned)port, retval);
	}
	printf("Setup rx queue ethdev %u\n", (unsigned)port);

	retval = rte_eth_dev_start(port);
	if (retval < 0) {
		rte_exit(EXIT_FAILURE, "Could not start port(%u) %d\n",
			(unsigned)port, retval);
	}

	printf("Start ethdev %u\n", (unsigned)port);
	
}

static rte_atomic32_t kni_pause = RTE_ATOMIC32_INIT(0);

static void print_ethaddr(const char *name, uint8_t *addr_bytes)
{
	char buf[RTE_ETHER_ADDR_FMT_SIZE];
	snprintf(buf, RTE_ETHER_ADDR_FMT_SIZE, "%02X:%02X:%02X:%02X:%02X:%02X",
		 addr_bytes[0],
		 addr_bytes[1],
		 addr_bytes[2],
		 addr_bytes[3],
		 addr_bytes[4],
		 addr_bytes[5]);
	
	printf("%s%s", name, buf);
}


static int kni_change_mtu(uint16_t port_id, unsigned int new_mtu)
{
	int ret;
	uint16_t nb_rxd = NB_RXD;
	struct rte_eth_conf conf;
	struct rte_eth_dev_info dev_info;
	struct rte_eth_rxconf rxq_conf;

	if (!rte_eth_dev_is_valid_port(port_id)) {
		printf("Invalid port id %d\n", port_id);
		return -EINVAL;
	}

	printf("Change MTU of port %d to %u\n", port_id, new_mtu);

	/* Stop specific port */
	rte_eth_dev_stop(port_id);

	memcpy(&conf, &port_conf, sizeof(conf));
	/* Set new MTU */
	if (new_mtu > RTE_ETHER_MAX_LEN)
		conf.rxmode.offloads |= DEV_RX_OFFLOAD_JUMBO_FRAME;
	else
		conf.rxmode.offloads &= ~DEV_RX_OFFLOAD_JUMBO_FRAME;

	/* mtu + length of header + length of FCS = max pkt length */
	conf.rxmode.max_rx_pkt_len = new_mtu + KNI_ENET_HEADER_SIZE +
							KNI_ENET_FCS_SIZE;
	ret = rte_eth_dev_configure(port_id, 1, 1, &conf);
	if (ret < 0) {
		printf("Fail to reconfigure port %d\n", port_id);
		return ret;
	}

	ret = rte_eth_dev_adjust_nb_rx_tx_desc(port_id, &nb_rxd, NULL);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Could not adjust number of descriptors "
				"for port%u (%d)\n", (unsigned int)port_id,
				ret);

	rte_eth_dev_info_get(port_id, &dev_info);
	rxq_conf = dev_info.default_rxconf;
	rxq_conf.offloads = conf.rxmode.offloads;
	ret = rte_eth_rx_queue_setup(port_id, 0, nb_rxd,
		rte_eth_dev_socket_id(port_id), &rxq_conf, pktmbuf_pool);
	if (ret < 0) {
		printf("Fail to setup Rx queue of port %d\n",
				port_id);
		return ret;
	}

	/* Restart specific port */
	ret = rte_eth_dev_start(port_id);
	if (ret < 0) {
		printf("Fail to restart port %d\n", port_id);
		return ret;
	}

	return 0;
}

static int kni_config_network_interface(uint16_t port_id, uint8_t if_up)
{
	int ret = 0;

	if (!rte_eth_dev_is_valid_port(port_id)) {
		printf("Invalid port id %d\n", port_id);
		return -EINVAL;
	}

	printf("Configure network interface of %d %s\n",
					port_id, if_up ? "up" : "down");

	rte_atomic32_inc(&kni_pause);

	if (if_up != 0) { /* Configure network interface up */
		rte_eth_dev_stop(port_id);
		ret = rte_eth_dev_start(port_id);
	} else /* Configure network interface down */
		rte_eth_dev_stop(port_id);

	rte_atomic32_dec(&kni_pause);

	if (ret < 0)
		printf("Failed to start port %d\n", port_id);

	return ret;
}

static int kni_config_mac_address(uint16_t port_id, uint8_t mac_addr[])
{
	int ret = 0;

	if (!rte_eth_dev_is_valid_port(port_id)) {
		printf("Invalid port id %d\n", port_id);
		return -EINVAL;
	}

	printf("Configure mac address of %d\n", port_id);
	print_ethaddr("Address:", mac_addr);

	ret = rte_eth_dev_default_mac_addr_set(port_id,
					(struct rte_ether_addr *)mac_addr);
	if (ret < 0)
		printf("Failed to config mac_addr for port %d\n",
			port_id);

	return ret;
}


struct rte_kni *kni = NULL;

static void check_all_ports_link_status(uint32_t port_mask)
{
#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */
	uint16_t portid;
	uint8_t count, all_ports_up, print_flag = 0;
	struct rte_eth_link link;

	printf("\nChecking link status");
	fflush(stdout);
	for (count = 0; count <= MAX_CHECK_TIME; count++) {
		all_ports_up = 1;
		RTE_ETH_FOREACH_DEV(portid) {
			if ((port_mask & (1 << portid)) == 0)
				continue;
			memset(&link, 0, sizeof(link));
			rte_eth_link_get_nowait(portid, &link);
			/* print link status if flag set */
			if (print_flag == 1) {
				if (link.link_status)
					printf(
					"Port%d Link Up .Speed %u Mbps - %s\n",
						portid, link.link_speed,
				(link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
					("full-duplex") : ("half-duplex\n"));
				else
					printf("Port %d Link Down\n", portid);
				continue;
			}
			/* clear all_ports_up flag if any link down */
			if (link.link_status == ETH_LINK_DOWN) {
				all_ports_up = 0;
				break;
			}
		}
		/* after finally printing all link status, get out */
		if (print_flag == 1)
			break;

		if (all_ports_up == 0) {
			printf(".");
			fflush(stdout);
			rte_delay_ms(CHECK_INTERVAL);
		}

		/* set the print_flag if all ports up or timeout */
		if (all_ports_up == 1 || count == (MAX_CHECK_TIME - 1)) {
			print_flag = 1;
			printf("\ndone\n");
		}
	}
}



static int kni_alloc(uint16_t port) {
	
	// set conf
	struct rte_kni_conf conf;
	memset(&conf, 0, sizeof(struct rte_kni_conf));

	snprintf(conf.name, RTE_KNI_NAMESIZE, "vEth%u", port);
	conf.group_id = (uint16_t)port;
	conf.mbuf_size = MAX_PACKET_SZ;

	struct rte_eth_dev_info devinfo;
	rte_eth_dev_info_get(port, &devinfo);
	rte_eth_macaddr_get(port, (struct rte_ether_addr*)&conf.mac_addr);
	rte_eth_dev_get_mtu(port, &conf.mtu);


	// set eth ops
	struct rte_kni_ops ops;
	memset(&ops, 0, sizeof(struct rte_kni_ops));

	ops.port_id = port;
	ops.config_mac_address = kni_config_mac_address;
	ops.change_mtu = kni_change_mtu;
	ops.config_network_if = kni_config_network_interface;
	

	kni = rte_kni_alloc(pktmbuf_pool, &conf, &ops);
	if (!kni) {
		rte_exit(EXIT_FAILURE, "Fail to create kni for port%u", port);
	}

	
	return 0;
}


int main(int argc, char *argv[]) {

	int retval = rte_eal_init(argc, argv);
	if (retval < 0) {
		rte_exit(EXIT_FAILURE, "Could not initialise EAL (%d)", retval);
	}

	argc -= retval;
	argv += retval;

	retval = parse_args(argc, argv);
	if (retval < 0) {
		rte_exit(EXIT_FAILURE, "Could not parse input parameters\n");
	}

	// malloc 
	// 
	pktmbuf_pool = rte_pktmbuf_pool_create("mbuf_pool", NB_MBUF, 
		MEMPOOL_CACHE_SZ, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
	if (pktmbuf_pool == NULL) {
		rte_exit(EXIT_FAILURE, "Could not initalise mbuf pool\n");
	}

	uint8_t nb_eth_dev =  rte_eth_dev_count_avail(); //ports 
	if (nb_eth_dev == 0) {
		rte_exit(EXIT_FAILURE, "No Supported Ethernet device found\n");
	}

	printf("nb_eth_dev : %d\n", nb_eth_dev);

	
	rte_kni_init(0); //

	init_port(0);

	kni_alloc(0);

	check_all_ports_link_status(ports_mask);
	
	getchar();
}



