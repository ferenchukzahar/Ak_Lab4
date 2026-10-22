
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Zakhar Ferenchuk");
MODULE_DESCRIPTION("Lab 4: Kernel module with parameters, list_head, and ktime");
MODULE_LICENSE("GPL");

static uint count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "Number of times to print Hello, world!");

struct event_list {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(event_list_head);

static int __init hello_init(void)
{
	struct event_list *element;
	int i;

	if (count == 0) {
		pr_warn("Warning: count is 0\n");
	} else if (count >= 5 && count <= 10) {
		pr_warn("Warning: count is between 5 and 10\n");
	} else if (count > 10) {
		pr_err("Error: count is greater than 10\n");
		return -EINVAL;
	}

	for (i = 0; i < count; i++) {
		element = kmalloc(sizeof(*element), GFP_KERNEL);
		if (!element)
			return -ENOMEM;

		element->time = ktime_get();
		list_add_tail(&element->list, &event_list_head);

		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct event_list *element, *tmp;

	list_for_each_entry_safe(element, tmp, &event_list_head, list) {
		pr_info("Event time: %lld ns\n", ktime_to_ns(element->time));
		list_del(&element->list);
		kfree(element);
	}
}

module_init(hello_init);
module_exit(hello_exit);
