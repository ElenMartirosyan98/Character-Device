#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include "chardev.h"

#include <linux/slab.h>
#include <asm/uaccess.h>
#define BUFSIZE  1000
 


static struct proc_dir_entry *ent;

char* devName = "chardev";
const int major = 40;


static int irq=20;
module_param(irq,int,0660);
 
static int mode=1;
module_param(mode,int,0660);
 

int exportedFunc(int param);
struct scull_dev;

static ssize_t chardev_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) 
{
	printk( KERN_DEBUG "1 write handler\n");
	int num, elem,c,i,m;
	char buf[BUFSIZE];
	printk( KERN_DEBUG "2 write succeed\n");
	if(*ppos > 0 || count > BUFSIZE)
		return -EFAULT;
	
	printk( KERN_DEBUG "3 write succeed\n");
	if(raw_copy_from_user(buf,ubuf,count))
		return -EFAULT;

	printk( KERN_DEBUG "4 write succeed\n");
        num = sscanf(buf,"%d", &elem);

	printk( KERN_DEBUG "5 write succeed\n");
	if(num != 1)
		return -EFAULT;
	
	push(stack, elem);
	
	c = strlen(buf);
	*ppos = c;

	return c;
}

static ssize_t chardev_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) 
{
	char buf[BUFSIZE];
	int len=0;
	printk( KERN_DEBUG "1 read handler\n");
	if(*ppos > 0 || count < BUFSIZE)
		return 0;
	printk( KERN_DEBUG "2 read handler\n");
	int* ptrElem = pop(stack);
        if (!ptrElem)
           return -EFAULT;

	printk( KERN_DEBUG "3 read handler  sz%d\n", *ptrElem);
	printk( KERN_DEBUG "3 read handler sz+1%d\n", *ptrElem);
	printk( KERN_DEBUG "3 read handler sz-1%d\n", *ptrElem);
      	len += sprintf(buf,"%d\n", *ptrElem);

	
	printk( KERN_DEBUG "4 read handler\n");

	
	if(raw_copy_to_user(ubuf,buf,len))
		return -EFAULT;
	
	printk( KERN_DEBUG "5 read handler\n");
	*ppos = len;

	return len;
}

static int chardev_open(struct inode *inode, struct file *filp)
{

	struct scull_dev *dev; /* device information */	
	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev;

	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
		scull_trim(dev); /* ignore errors */	
	}
	return 0;
	/* success */
}


static int chardev_release(struct inode *inode, struct file *file)
{
    printk("Device close\n");
    return 0;
}

static struct file_operations ops = 
{
	.owner = THIS_MODULE,
	.read = chardev_read,
	.write = chardev_write,
	.open = chardev_open,
	.release = chardev_release
};
 
static int simple_init(void)
{
        printk(KERN_INFO "Hello, It is procFileMod init!\n");
 
        int retVal = register_chrdev(major, devName, &ops);
 	
	if (retVal)
	{
           printk(KERN_INFO "Something went wrong %d\n", retVal);
	}

	return 0;
}
 
static void simple_cleanup(void)
{
        printk(KERN_INFO "Bye, It was procFileMod exit!\n");
	proc_remove(ent);
}
 
module_init(simple_init);
module_exit(simple_cleanup)




