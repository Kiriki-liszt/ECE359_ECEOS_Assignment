#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/list.h>
#include <linux/slab.h>

/* performs a depth-first traversal of the list of tasks in the system. */
void traverse(struct task_struct *ptr) {
/* fill in your code */
	struct task_struct *child;
	struct list_head *list;

	printk("name: %20s  ||  pid: %6d  ||  parent pid: %6d  ||  state: %6li\n", ptr->comm, ptr->pid, ptr->parent->pid, ptr->state);
	list_for_each(list, &ptr->children) {
		child = list_entry(list, struct task_struct, sibling);
		traverse(child);
	}
	return;
}

int simple_init(void)
{
	//struct task_struct *task;
	//struct list_head *list;
	
	//struct task_struct *c;

	printk(KERN_INFO "\nLoading Module\n");

	traverse(&init_task);

	return 0;
}



struct BFSQ {    
    struct task_struct *task;
    struct list_head blist;
};

static LIST_HEAD(BFSH);

void add(struct task_struct *a){
	struct BFSQ *two;
	two = kmalloc(sizeof(*two), GFP_KERNEL);
	two->task = a;
	INIT_LIST_HEAD(&two->blist);
	list_add_tail(&two->blist, &BFSH);
	return;
}

void BFtraverse(struct task_struct *bfss) {

	struct BFSQ *child1;
	struct task_struct *child2, *child3;
	struct list_head *kk, *ll, *dum;

	add(bfss);

	while(!list_empty(&BFSH)) {
		list_for_each_safe(kk, dum, &BFSH) {

			child1 = list_entry(kk, struct BFSQ, blist);
			child2 = child1->task;
 			printk("name: %20s  ||  pid: %6d  ||  parent pid: %6d  ||  state: %6li\n", child2->comm, child2->pid, child2->parent->pid, child2->state);

			list_for_each(ll, &child2->children) {
				child3 = list_entry(ll, struct task_struct, sibling);
				add(child3);
			}

			list_del(kk);
		}
	}
	return;
}


void simple_exit(void) {
	printk(KERN_INFO "\nRemoving Module\n");

	BFtraverse(&init_task);

	return;
}

module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Process Module");
MODULE_AUTHOR("SGG");

