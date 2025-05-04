#include "network.h"

MODULE_LICENSE("GPL");

struct nf_hook_ops hook;
struct rb_root root = RB_ROOT;

unsigned int hook_function(void *priv, struct sk_buff *skb, const struct nf_hook_state *state){
	struct iphdr *ip_header = (struct iphdr *)skb_network_header(skb);
	printk("IP address:  %pI4\n", &(ip_header->saddr));

	struct addr_node new_address = {
		.addr = ip_header->saddr,
		.count = 1,
	};
	
	insert_node(&root, &new_address);

	return NF_ACCEPT;
}

#define READ_LEN 30
ssize_t read_addr(struct file *filp, char *buf, size_t count, loff_t *offp) {
	char byte_list[4];
	size_t readlen = 0;
	struct addr_node *pos, *n;
	if(*offp){
		return 0;
	}

	rbtree_postorder_for_each_entry_safe(pos, n, &root, node){ // I need to think about if it would really add 24 bytes or 32 * 24 bytes.
		char buffer[READ_LEN];

		int_to_addr(byte_list, pos->addr);

		size_t readAmt = sprintf(buffer, "%d.%d.%d.%d : %d\n", byte_list[0], byte_list[1], byte_list[2], byte_list[3], pos->count);	

		copy_to_user(buf+readlen, buffer, readAmt );
		readlen += readAmt;

		CLEAR_BUFFER(buffer, READ_LEN)
	}

	*offp = readlen;
	return readlen;
}

struct proc_ops proc_fops = {
	proc_read: read_addr
};

#define PROC_FILE_NAME "network"
int __init init(void) {
	// The nf_nook_ops structure will store information used for nf_register_net_hook
	hook.hook = hook_function;       			// Function our hook will run
	hook.hooknum = NF_INET_LOCAL_IN; 			// Look at incoming trafic
	hook.pf = AF_INET;               			// AF_INET is IPv4.  AF_INET6 is IPv6
	nf_register_net_hook(&init_net, &hook); 	// init_net is defined in a header file
	proc_create(PROC_FILE_NAME, 0, NULL, &proc_fops);
	return 0;
}

void __exit cleanup(void) {
	char addr_list[4];
	struct addr_node *pos, *n;

	nf_unregister_net_hook(&init_net, &hook);
	remove_proc_entry(PROC_FILE_NAME,NULL);

	rbtree_postorder_for_each_entry_safe(pos, n, &root, node){
		int_to_addr(addr_list, pos->addr);
		printk("Address: %d.%d.%d.%d : %d\n", addr_list[0], addr_list[1], addr_list[2], addr_list[3], pos->count);
		kfree(pos);
	}
}

module_init(init);
module_exit(cleanup);
