##Project 2: Building a Device Driver
Build a memory device driver where the device is a stack of a maximum of 10 bytes.  Initially, the stack is empty.  When the stack is written to, the device will push ASCII lower-case alphabets from the user buffer into the stack.  For example, it will push bytes 'a' and 'k' but will not push bytes 'A', 'K', '*', or '2'.  If the buffer becomes full then the driver will stop and return.

When reading the device driver, the driver will pull the bytes out of the stack and put it into the user buffer.  If the stack becomes empty then the driver will stop and return.

