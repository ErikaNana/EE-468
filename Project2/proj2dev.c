#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

// module attributes
MODULE_LICENSE("GPL"); // this avoids kernel taint warning
MODULE_DESCRIPTION("Device Driver Demo");
MODULE_AUTHOR("Erika Nana");

static char msg[100]={0};
static short readPos=0;
static int times = 0;

// protoypes,else the structure initialization that follows fail
static int dev_open(struct inode *, struct file *);
static int dev_rls(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// structure containing callbacks
static struct file_operations fops = 
{
	.read = dev_read, // address of dev_read
	.open = dev_open,  // address of dev_open
	.write = dev_write, // address of dev_write 
	.release = dev_rls, // address of dev_rls
};


// called when module is loaded, similassr to main()
int init_module(void)
{
	int t = register_chrdev(90,"proj2dev",&fops); //register driver with major:89
	
	if (t<0) printk(KERN_ALERT "Device registration failed..\n");
	else printk(KERN_ALERT "Device registered...\n");

	return t;
}


// called when module is unloaded, similar to destructor in OOP
void cleanup_module(void)
{
	unregister_chrdev(90,"proj2dev");
}


// called when 'open' system call is done on the device file
static int dev_open(struct inode *inod,struct file *fil)
{	
	times++;
	printk(KERN_ALERT"Device opened %d times\n",times);
	return 0;
}

// called when 'read' system call is done on the device file
//driver pulls bytes out of the stack and puts it into the user buffer
//if the stack becomes empty then driver will stop and return
static ssize_t dev_read(struct file *filp,char *buff,size_t len,loff_t *off)
{
	short count = 0;
	while (len && (msg[readPos]!=0))
	{
		put_user(msg[readPos],buff++); //copy byte from kernel space to user space
		count++;
		len--;
		readPos++;
	}
	return count;
}

// called when 'write' system call is done on the device file
//device pushes ascii lower-case alphabets from the user buffer into the stack
//if the buffer becomes full the driver will stop and return
static ssize_t dev_write(struct file *filp,const char *buff,size_t len,loff_t *off)
{
	short ind = len-1;
	short count=0;
	memset(msg,0,100);
	readPos=0;
	while(len>0)
	{
		msg[count++] = buff[ind--]; //copy the given string to the driver but in reverse
		len--;
	}
	return count;
}

// called when 'close' system call is done on the device file
static int dev_rls(struct inode *inod,struct file *fil)
{
	printk(KERN_ALERT"Device closed\n");
	return 0;
}
