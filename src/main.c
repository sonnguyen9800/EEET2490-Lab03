#include "uart.h"
#include "uart0.h"
void main()
{
 // set up serial console
 uart0_init();

 // say hello
 uart0_puts("Hello World!\n");
 // echo everything back
 while(1) {
 //read each char
 char c = uart_getc();
 uart0_puts("You typed:");
 uart0_sendc(c);
 uart0_puts("\n");

// char c = 'a';
// uart0_puts("You typed:");
// uart0_sendc(c);
// uart0_puts("\n");

 }
}
