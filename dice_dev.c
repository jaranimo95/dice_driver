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

#include <asm/uaccess.h>

unsigned char get_random_byte(int max) {
     unsigned char c;
     get_random_bytes(&c, 1);
     return c%max;
}

static int dice_read(FILE *file, char *buffer, int count, loff_t *offset)
{    
  if(count == 0) return 0;

  int i;
  char data[];

  data = kmalloc(count, GFP_KERNEL);

  for(i = 0; i < count; i++)
  {
    data[i] = get_random_byte(6) + 1;
  }

  if(copy_to_user(buffer, data, count)) kfree(data);

  *offset+=count;
  return *offset;
}

static const struct file_operations dice_fops = {
	.owner		= THIS_MODULE,
	.read		= dice_read,
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
	"/dev/dice",
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
		       "Unable to register dice misc device\n");

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
MODULE_AUTHOR("Valerie Henson <val@nmt.edu>");
MODULE_DESCRIPTION("dice minimal module");
MODULE_VERSION("dev");
