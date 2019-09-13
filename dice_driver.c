/*
 * dice minimal kernel module - /dev version
 *
 * Valerie Henson <val@nmt.edu>
 *
 */

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/random.h>
#include <asm/uaccess.h>

// Helper function for getting a random byte
unsigned char get_random_byte(int max) {
     unsigned char c;
     get_random_bytes(&c, 1);
     return c%max;
}

static ssize_t dice_roll(struct file * file, char * buf, size_t count, loff_t *ppos)
{    
  	int i;													 	   // Loop iteration variable
  	char *data;													  // Pointer to random byte data array
  	if(count == 0) return 0;									 // Return 0 if no data to read

  	data = kmalloc(count, GFP_KERNEL);					   	   // Allocate space for data

	for(i = 0; i < count; i++)
    {
  		data[i] = get_random_byte(5) + 1;				   // Get random byte between 1 and 6 [ (0 to 5) + 1 ]
  	}

  	if(copy_to_user(buf, data, count)) kfree(data);		// If values successfully read to user, free allocated memory

  	*ppos+=count;									  // Increment position in file by count
  	return *ppos;
}

/*
 * The only file operation we care about is read.
 */

static const struct file_operations dice_fops = {
	.owner		= THIS_MODULE,
	.read		= dice_roll,
};

static struct miscdevice dice_dev = {
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	/*
	 * Name ourselves /dev/dice.
	 */
	"dice",
	/*
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	&dice_fops
};

static int __init
dice_init(void)
{
	int ret;

	/*
	 * Create the "dice" device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/dice device using
	 * the default rules.
	 */
	ret = misc_register(&dice_dev);
	if (ret)
		printk(KERN_ERR
		       "Unable to register \"dice\" misc device\n");

	return ret;
}

module_init(dice_init);

static void __exit
dice_exit(void)
{
	misc_deregister(&dice_dev);
}

module_exit(dice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christian Jarani");
MODULE_DESCRIPTION("Dice Driver");
MODULE_VERSION("dev");
