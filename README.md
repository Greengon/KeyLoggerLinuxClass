# KeyLoggerLinuxClass

Collaborators: 
	1.Gon Lubel.
	2.Orel Museri.

Class:
	linux kernel class colman collage.

Description:
	Simple implement of keylogger as module inserted to the linux kernel.

The functionality of the Keylogger are:
- We will hold all logged keys in a buffer.
- For start we will just print the key to the kernel log.
- We will register our LKM to the Keyboard Notification Chain using a notifier_block. For that we will have to create the callback function that will handle each keypress.
