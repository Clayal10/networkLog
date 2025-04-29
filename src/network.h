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
    struct rb_node node;
    uint32_t addr;
};