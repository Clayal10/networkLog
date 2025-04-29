#include <linux/module.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/inet.h>
#include <linux/socket.h>
#include <linux/proc_fs.h>
#include <linux/rbtree.h>

// Implimentation of rb_tree can be found at /lib/modules/6.1.0-32-amd64/source/include/linux/rbtree.h

struct addr_node{
    struct rb_node node; // 24
    uint32_t addr;       // 4
    int count;           // 4
}; // 32 bytes

void* insert_node(struct rb_root *root, uint32_t addr, size_t node_offset){
    size_t datasize = 32;

    struct rb_node **spot = &(root->rb_node), *parent = NULL;
	void *new_node;
	struct rb_node* buffer_node;
	while(*spot) {
		uint32_t spot_data = (char*)(*spot) + 24; // Address of the addr_node addr
		parent = *spot;
		if(spot_data == addr){ // We found it!
            *((char*)(*spot) + 28) += 1; // Incriment count by 1
			return 0;
		} else if(spot_data > addr){
			spot = &((*spot)->rb_right);
        }
		else{
			spot = &((*spot)->rb_left);
        }
	}

	new_node = kmalloc(datasize, 1);
	memcpy(new_node, addr, datasize);
	buffer_node = new_node + node_offset;
    *((char*)buffer_node + 28) = 0;
	rb_link_node(buffer_node, parent, spot);
	rb_insert_color(buffer_node, root);
	return new_node;
}
