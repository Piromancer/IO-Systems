#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/init.h>
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rozhnov Denis P3400 && Tarasenko Daniil P3400");
MODULE_DESCRIPTION("Lab 1");

static dev_t first;
static struct cdev c_dev; 
static struct class *cl;
static struct proc_dir_entry* entry;


struct res_vector
{
	int *data;
	size_t size;
	size_t idx;
};

struct res_vector rv;

struct res_vector rv_init(void)
{
	return (struct res_vector) {
		.data = vmalloc(sizeof(int)),
		.size = 1,
		.idx = 0,
	};
}

void rv_append(size_t value, struct res_vector *r_vector)
{
	if (r_vector->idx == r_vector->size - 1)
	{
		r_vector->size *= 2;
		int* n_data = (int*)vmalloc(r_vector->size * sizeof(int));
		memcpy(n_data, r_vector->data, r_vector->idx * sizeof(int));
		vfree(r_vector->data);
		r_vector->data = n_data;
	}
	r_vector->data[r_vector->idx++] = value;
}

int count_letters(const char* string) {
	char* cur = string;
	int res = 0;
	while (*cur != '\0' ) {
		if ((*cur >= 'A' && *cur <= 'Z') || 
		     (*cur >= 'a' && *cur <= 'z')) {
			res++;
		}
		cur++;
	}
	return res;
}


static ssize_t proc_write(struct file *file, const char __user * ubuf, size_t count, loff_t* ppos) 
{
	printk(KERN_DEBUG "Attempting to write proc file");
  	return -1;
}


static ssize_t proc_read(struct file *file, char __user * ubuf, size_t count, loff_t* ppos) 
{
  	char sarr[512];
  	int written = 0;
  	int i = 0;
  	size_t len;

  	for (i = 0; i < rv.idx; i++)
    		written += snprintf(&sarr[written], 512 - written, "%d word had %d letters\n", i+1, rv.data[i]);
  	
	sarr[written] = 0;
    len = strlen(sarr);
  	if (*ppos > 0 || count < len) return 0;
  	if (copy_to_user(ubuf, sarr, len) != 0) return -EFAULT;
  	*ppos = len;
  	return len;
}

static int custom_open(struct inode *i, struct file *f)
{
  	return 0;
}

static int custom_close(struct inode *i, struct file *f)
{
  	return 0;
}

static ssize_t custom_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	int i = 0;
  	for(i = 0; i < rv.idx; i++)
		printk(KERN_DEBUG "%d word had %d letters\n", i+1, rv.data[i]);
	return 0;
}

static ssize_t custom_write(struct file *f, const char __user *buf,  size_t len, loff_t *off)
{
	char* user_input = (char*)vmalloc(len * sizeof(char));
	memcpy(user_input, buf, len * sizeof(char));
	user_input[len] = 0;
	int res = count_letters(user_input);
	rv_append(res, &rv);
	*off += len;
  	return len;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write,
};

static struct file_operations device_operations =
{
  	.owner = THIS_MODULE,
  	.open = custom_open,
  	.release = custom_close,
  	.read = custom_read,
  	.write = custom_write
};
 
static int __init ch_drv_init(void)
{
    	printk(KERN_INFO "Initialization\n");
	
	rv = rv_init();

    	entry = proc_create("var5", 0444, NULL, &fops);
    	if (alloc_chrdev_region(&first, 0, 1, "var5") < 0) return -1;
    	if ((cl = class_create(THIS_MODULE, "var5")) == NULL)
	{
		unregister_chrdev_region(first, 1);
		return -1;
	}
    	if (device_create(cl, NULL, first, NULL, "var5") == NULL)
    	{
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
    	}
    	cdev_init(&c_dev, &device_operations);
    	if (cdev_add(&c_dev, first, 1) == -1)
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
    	return 0;
}
 
static void __exit ch_drv_exit(void)
{
    	proc_remove(entry);
    	cdev_del(&c_dev);
    	device_destroy(cl, first);
    	class_destroy(cl);
    	unregister_chrdev_region(first, 1);
    	printk(KERN_INFO "Exit\n");
}
 
module_init(ch_drv_init);
module_exit(ch_drv_exit);
