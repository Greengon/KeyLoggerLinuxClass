/*
 *
 * keyLogger.c
 *
 * Simple keylogger which will saved any key pressed by user to an hidden file.
 * The keylogger is a module running in linux space, he doesn't care how he
 * got there.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>

static int __init keyLogger_birth(void){
	pr_info("A new keyLogger was born\n");
}

static void __exit keyLogger_death(void){
	pr_info("The keyLogger is dead\n");
}

module_init(keyLogger_birth);
module_exit(keyLogger_death);

