#include "network.h"

MODULE_LICENSE("GPL");

struct nf_hook_ops demo_hook;

unsigned int hook_function(void *priv, struct sk_buff *skb, const struct nf_hook_state *state){
	struct iphdr *ip_header = (struct iphdr *)skb_network_header(skb);
	printk("IP address:  %pI4\n", &(ip_header->saddr));

	// TODO Insert into tree.

	return NF_ACCEPT;
}

ssize_t read_addr(struct file *filp, char *buf, size_t count, loff_t *offp) {
	size_t readlen = 0;

	//TODO Pre order traversal and copy_to_user for each node.

	return readlen;
}

struct proc_ops proc_fops = {
	proc_read: read_addr
};

#define PROC_FILE_NAME "network"
int __init netmon_init(void) {
	// The nf_nook_ops structure will store information used for nf_register_net_hook
	demo_hook.hook = hook_function;       			// Function our hook will run
	demo_hook.hooknum = NF_INET_LOCAL_IN; 			// Look at incoming trafic
	demo_hook.pf = AF_INET;               			// AF_INET is IPv4.  AF_INET6 is IPv6
	nf_register_net_hook(&init_net, &demo_hook); 	// init_net is defined in a header file
	proc_create(PROC_FILE_NAME, 0, NULL, &proc_fops);
	return 0;
}

void __exit netmon_cleanup(void)
{
	nf_unregister_net_hook(&init_net, &demo_hook);
	remove_proc_entry(PROC_FILE_NAME,NULL);
}

module_init(netmon_init);
module_exit(netmon_cleanup);
