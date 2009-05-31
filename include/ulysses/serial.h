#ifndef _ULYSSES_SERIAL
#define _ULYSSES_SERIAL

#define COM1 0x3f8
#define COM2 0x2f8

/* init_serial()
 *  Set up the hardware serial device given by port, reading for reading
 *  and/or writing.
 */
void init_serial(unsigned int port);

/* write_serial()
 *  Write the given character to the (already initialised) serial port.
 *  Note that this *may* block if the serial port has unread/unwritten data
 *  in buffered in it.
 */
void write_serial(unsigned int port, const char c);

/* read_serial()
 *  Block until there is data to be read on the (already initialised) serial
 *  port and return the first character.
 */
char read_serial(unsigned int port);

#endif
