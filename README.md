# KeyLoggerLinuxClass

Collaborators: 
	1.Gon Lubel.
	2.Orel Museri.

Class:
	linux kernel class colman collage.

Description:
	Simple implement of keylogger as module inserted to the linux kernel.

The functionality of the Keylogger are:
- The logger would log anu key strokes that were made on an qwerty keyboard with the execption of F1-12 and special keys.
- The moddule would work in the kernel environment but reading from the character device will be done in user space.
- All files that will be created in user mode will be hidden and have arbitrary name.
- Our key logger will survive restart- The logger would log anu key strokes that were made on an qwerty keyboard with the execption of F1-12 and special keys.
- The moddule would work in the kernel environment but reading from the character device will be done in user space.
- All files that will be created in user mode will be hidden and have arbitrary name.
- Our module code will be using minmal obfuscation.
- Our key logger will survive restart.

Documentation:
Our keylogger moudle name is sys1ogs.ko.

The moudle data varaibles are:
- An array of size BUFFER_LEN(1024) of pointers to char * values.
- Two pointers to work with named keys_usr_ptr and keys_buf_ptr both point to the array.
- buf_pos counter.
- map of type Map struct of key value, holding all keysyms notifactions that are send from the keyboard and a string representing that keysyms.

In the code itself a function name keyLogger_birth is been used as module_init when the moudle is been inserted. The function allocate character device named nu11.
first we register  a major number and check thats its not failed,
to the function alloc_chrdev_region we send the addres of are major number and alloc_chrdev_region put the value in that addres.
next we create the class for are keylogger and named him as DEVICE_NAME that we choose in the start of the file and the function return a sturcter of are class and  put it in key_logger_class and we chack thats we dont get null if we do  so we free the major number and return -1.
next we create the device with device_create  function and give him are class that we create befor and the major number we register with and chack that its work if not we free the major number and the class.
and after we make the device we can rigester him to the system of course free all if fail.

Our moudle uses the notification chane of the keyboard to get notified when ever a keystroke as been made. This is been done by creating a new notifier_block named nb. whice as observer of the keybored will start the keys_pressed function on notifer_call.

The keys_pressed function that will be called will take the data send by the keyboard and if the action that been send is keysyms and key pressed the value of keysyms will be pulled to a char variable. Then the char value will be checked with the map we creatd by using the get_value function.
After gettting the compatailbe string , we will write the string to our buffer, waiting to be read by the user space.

When our device character will be read from the user space dev_read function will be activeted reading the bufffer to user by using the copy_to_user function.

When removing the moudle all devices will be deleted and the notifier block will be unregister.
