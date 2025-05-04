#include <linux/module.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/inet.h>
#include <linux/socket.h>
#include <linux/proc_fs.h>
#include <linux/rbtree.h>
#include <linux/mutex.h>

#define CLEAR_BUFFER(x, len) \
	int i; \
	for(i=0; i<len; i++) { \
		x[i] = '\0'; \
	} \

// Implimentation of rb_tree can be found at /lib/modules/6.1.0-32-amd64/source/include/linux/rbtree.h

struct addr_node{
   	struct rb_node node; // 24
    	uint32_t addr;       // 4
    	int count;           // 4
}; // 32 bytes

// Returns 0 if equal, -1 if a < b, 1 if a > b
int cmp_func(void* a, void* b){
	const struct addr_node *a_addr = a;
	const struct addr_node *b_addr = b;

	if(a_addr->addr == b_addr->addr){
		return 0;
	}
	if(a_addr->addr < b_addr->addr){
		return -1;
	}
	return 1;
}

void* insert_node(struct rb_root *root, void* addr){
	size_t datasize = 32;

	struct rb_node **spot = &(root->rb_node), *parent = NULL;
	struct addr_node* new_node;
	struct rb_node* buffer_node;
	while(*spot) {
		struct addr_node *spot_data = rb_entry(*spot, struct addr_node, node);
		parent = *spot;
		if(cmp_func(spot_data, addr) == 0){
			printk("FOUND NODE\n");
			(spot_data)->count++;
			return 0;
		}else if(cmp_func(spot_data, addr) == -1){ // addr is the new value
			spot = &((*spot)->rb_right);
		}else{
			spot = &((*spot)->rb_left);
       		}
	}

	new_node = kmalloc(datasize, GFP_KERNEL);
	memcpy(new_node, addr, datasize);
	buffer_node = &new_node->node;
	rb_link_node(buffer_node, parent, spot);
	rb_insert_color(buffer_node, root);
	return new_node;
}

void int_to_addr(char* list, uint32_t addr){
	list[0] = (addr & 0x000000FF);
	list[1] = (addr & 0x0000FF00) >> 8;
	list[2] = (addr & 0x00FF0000) >> 16;
	list[3] = (addr & 0xFF000000) >> 24;
}