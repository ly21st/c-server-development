




#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include <linux/inetdevice.h>

MODULE_AUTHOR("wangbojing");
MODULE_DESCRIPTION("Kernel module for nic");
MODULE_LICENSE("GPL");


#define MAX_ETH_FRAME_SIZE		1792
#define DEF_MSG_ENABLE			0xffff


struct nic_priv {
	unsigned char *tx_buf;
	unsigned int tx_len;
	unsigned char *rx_buf;
	unsigned int rx_len;
	u32 msg_enable;
};

static struct net_device *nic_dev[2];


static void dump(unsigned char *buffer) {

	unsigned char *p, sbuf[2*(sizeof(struct ethhdr) + sizeof(struct iphdr))];
	int i;
	p = sbuf;

	for (i = 0;i < sizeof(struct ethhdr);i ++) {
		p += sprintf(p, "%02X", buffer[i]);
	}
	printk("eth %s\n", sbuf);

	p = sbuf;
	for (i = 0;i < sizeof(struct iphdr);i ++) {
		p += sprintf(p, "%02X", buffer[sizeof(struct ethhdr) + i]);
	}
	printk("iph %s\n", sbuf);
	
	p = sbuf;
	for (i = 0;i < 4;i ++) {
		p += sprintf(p, "%02X", buffer[sizeof(struct ethhdr) + sizeof(struct iphdr) + i]);
	}
	printk("payload %s\n", sbuf);
	
}

static void nic_rx(struct net_device *dev, int len, unsigned char *buf) {
	struct sk_buff *skb;
	struct nic_priv *priv = netdev_priv(dev);

	netif_info(priv, hw, dev, "%s(#%d), rx:%d\n",
                __func__, __LINE__, len);
	
	skb = dev_alloc_skb(len+2);
	if (!skb) {
		netif_err(priv, rx_err, dev,
                  "%s(#%d), rx: low on mem - packet dropped\n",
                  __func__, __LINE__);
		dev->stats.rx_dropped++;
		return ;
	}

	skb_reserve(skb, 2);
	memcpy(skb_put(skb, len), buf, len);

	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);
	skb->ip_summed = CHECKSUM_UNNECESSARY;
	
	dev->stats.rx_packets ++;
	dev->stats.rx_bytes += len;

	netif_rx(skb);
	
}


static int	nic_open(struct net_device *dev) {

	struct nic_priv *priv = netdev_priv(dev);
	netif_info(priv, ifup, dev, "%s(#%d), priv:%p\n",
                __func__, __LINE__, priv);

	priv->tx_buf = kmalloc(MAX_ETH_FRAME_SIZE, GFP_KERNEL);
	if (priv->tx_buf == NULL) {
		netif_info(priv, ifup, dev, "%s(#%d), cannot alloc tx buf\n",
                    __func__, __LINE__);
		return -ENOMEM;
	}

	netif_start_queue(dev);
	return 0;
}

static int nic_stop(struct net_device *dev) {

	struct nic_priv *priv = netdev_priv(dev);
	netif_info(priv, ifdown, dev, "%s(#%d), priv:%p\n",
                __func__, __LINE__, priv);
	
	if (priv->tx_buf) {
		kfree(priv->tx_buf);
	}

	netif_stop_queue(dev);

	return 0;
}

static void nic_hw_xmit(struct net_device *dev) {

	struct nic_priv *priv = netdev_priv(dev);
	struct iphdr *iph;
	u32 *saddr, *daddr;
	struct in_device *in_dev;
	struct in_ifaddr *if_info;
	
	if (priv->tx_len < sizeof(struct ethhdr) + sizeof(struct iphdr)) {
		netif_info(priv, hw, dev, "%s(#%d), too short\n",
                   __func__, __LINE__);
		return ;
	}

	dump(priv->tx_buf);
	iph = (struct iphdr*)(priv->tx_buf + sizeof(struct ethhdr));
	saddr = &iph->saddr;
	daddr = &iph->daddr;

	netif_info(priv, hw, dev, "%s(#%d), orig, src:%pI4, dst:%pI4, len:%d\n",
                __func__, __LINE__, saddr, daddr, priv->tx_len);

	in_dev = nic_dev[(dev == nic_dev[0] ? 1 : 0)]->ip_ptr;
	if (in_dev) {
		//if_info = in_dev->ifa_list;
		for (if_info = in_dev->ifa_list; if_info; if_info = if_info->ifa_next) {
			*saddr = *daddr = if_info->ifa_address;
			((u8*)saddr)[3]++;

			netif_info(priv, hw, dev, "%s(#%d), new, src:%pI4, dst:%pI4\n",
                        __func__, __LINE__, saddr, daddr);
			break;
		}
		if (!if_info) {
			dev->stats.tx_dropped ++;
			netif_info(priv, hw, dev, "%s(#%d), drop packet\n",
                        __func__, __LINE__);
			return ;
		}
	}

	iph->check = 0;
	iph->check = ip_fast_csum((unsigned char*)iph, iph->ihl);

	dev->stats.tx_packets ++;
	dev->stats.tx_bytes += priv->tx_len;

	nic_rx(nic_dev[(dev == nic_dev[0] ? 1 : 0)], priv->tx_len, priv->tx_buf);
	
}

static netdev_tx_t nic_start_xmit(struct sk_buff *skb, struct net_device *dev) {

	struct nic_priv *priv = netdev_priv(dev);
	netif_info(priv, drv, dev, "%s(#%d), orig, src:%pI4, dst:%pI4\n",
                __func__, __LINE__, &(ip_hdr(skb)->saddr), &(ip_hdr(skb)->daddr));
	priv->tx_len = skb->len;
	if (likely(priv->tx_len < MAX_ETH_FRAME_SIZE)) {
		if (priv->tx_len < ETH_ZLEN) {
			memset(priv->tx_buf, 0, ETH_ZLEN);
			priv->tx_len = ETH_ZLEN;
		}
		skb_copy_and_csum_dev(skb, priv->tx_buf);
		dev_kfree_skb_any(skb);
	} else {
		dev_kfree_skb_any(skb);
		dev->stats.tx_dropped ++;

		return NETDEV_TX_OK;
	}

	nic_hw_xmit(dev);

	return NETDEV_TX_OK;
}

static int	nic_validate_addr(struct net_device *dev) {
	struct nic_priv *priv = netdev_priv(dev);
	netif_info(priv, drv, dev, "%s(#%d), priv:%p\n",
                __func__, __LINE__, priv);

	return eth_validate_addr(dev);
}

static int nic_change_mtu(struct net_device *dev, int new_mtu) {
	struct nic_priv *priv = netdev_priv(dev);
	netif_info(priv, drv, dev, "%s(#%d), priv:%p, mtu%d\n",
                __func__, __LINE__, priv, new_mtu);

	return eth_change_mtu(dev, new_mtu);
}

static int nic_set_mac_addr(struct net_device *dev, void *addr) {

	struct nic_priv *priv = netdev_priv(dev);
	netif_info(priv, drv, dev, "%s(#%d), priv:%p\n",
                __func__, __LINE__, priv);
	
	return eth_mac_addr(dev, addr);	
}

static int nic_header_create (struct sk_buff *skb, struct net_device *dev,
			   unsigned short type, const void *daddr,
			   const void *saddr, unsigned int len) {

	struct nic_priv *priv = netdev_priv(dev);
	struct ethhdr *eth = (struct ethhdr*)skb_push(skb, ETH_HLEN);
	struct net_device *dst_netdev;

	netif_info(priv, drv, dev, "%s(#%d)\n",
                __func__, __LINE__);

	dst_netdev = nic_dev[(dev == nic_dev[0] ? 1 : 0)];
	eth->h_proto = htons(type);

	memcpy(eth->h_source, saddr ? saddr : dev->dev_addr, dev->addr_len);
	memcpy(eth->h_dest, dst_netdev->dev_addr, dst_netdev->addr_len);
	
	return dev->hard_header_len;
}


static const struct header_ops nic_header_ops = {
	.create = nic_header_create,
};

static const struct net_device_ops nic_netdev_ops = {
	//.ndo_init =  insmod sample.ko
	// open =  ifconfig eth2 192.168.3.123 up
	.ndo_open = nic_open,
	.ndo_stop = nic_stop,
	.ndo_validate_addr = nic_validate_addr,

	.ndo_start_xmit = nic_start_xmit,
	.ndo_change_mtu = nic_change_mtu,
	.ndo_set_mac_address = nic_set_mac_addr,
};

static struct net_device *nic_alloc_netdev(void) {

	struct net_device *netdev = alloc_etherdev(sizeof(struct nic_priv));
	if (!netdev) {
		pr_err("%s(#%d): alloc dev failed", __func__, __LINE__);
		return NULL;
	}

	eth_hw_addr_random(netdev);
	netdev->netdev_ops = &nic_netdev_ops;
	netdev->flags |= IFF_NOARP;

	netdev->features |= NETIF_F_HW_CSUM;
	netdev->header_ops = &nic_header_ops;

	return netdev;
	
}

static int __init nic_init(void) {

	int ret = 0;
	struct nic_priv *priv;

	pr_info("%s(#%d): install module\n", __func__, __LINE__);

	nic_dev[0] = nic_alloc_netdev();
	if (!nic_dev[0]) {
		printk("%s(#%d): alloc netdev[0] failed\n", __func__, __LINE__);
		return -ENOMEM;
	}
	
	nic_dev[1] = nic_alloc_netdev();
	if (!nic_dev[1]) {
		printk("%s(#%d): alloc netdev[1] failed\n", __func__, __LINE__);
		goto alloc_2nd_failed;
	}

	ret = register_netdev(nic_dev[0]);
	if (ret) {
		printk("%s(#%d): reg net driver failed. ret: %d\n", __func__, __LINE__, ret);
		goto reg1_failed;
	}

	ret = register_netdev(nic_dev[1]);
    if (ret) {
        printk("%s(#%d): reg net driver failed. ret:%d\n", __func__, __LINE__, ret);
        goto reg2_failed;
    }

	priv = netdev_priv(nic_dev[0]);
	priv->msg_enable = DEF_MSG_ENABLE;
	priv = netdev_priv(nic_dev[1]);
	priv->msg_enable = DEF_MSG_ENABLE;
	
	return 0;

reg2_failed:
	unregister_netdev(nic_dev[0]);
reg1_failed:
	free_netdev(nic_dev[1]);
alloc_2nd_failed:
	free_netdev(nic_dev[0]);
	
	return ret;
}


static void __exit nic_exit(void) {

	int i = 0;
	pr_info("%s(#%d): remove module\n", __func__, __LINE__);
	for (i = 0;i < ARRAY_SIZE(nic_dev);i ++) {
		unregister_netdev(nic_dev[i]);
		free_netdev(nic_dev[i]);
	}
}



module_init(nic_init);
module_exit(nic_exit);




