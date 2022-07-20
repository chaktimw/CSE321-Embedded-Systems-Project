// #include "mbed.h"
// #include <cstdio>
// void isr_369(void);
// void isr_ABC(void);
// // void isr_6(void);
// // void isr_b(void);

// #define WAIT_TIME_US 250000
// int row = 0;

// InterruptIn dog(PB_8, PullDown);
// InterruptIn cat(PB_9, PullDown);

// int main() {
//   RCC->AHB2ENR |= 0x4;
//   // port c pin 8 and 9 output
// //   GPIOC->MODER |= 0x50000;
// //   GPIOC->MODER &= ~(0xA0000);
//   GPIOC->MODER |=   0x550000;
//   GPIOC->MODER &= ~(0xAA0000);

//   dog.rise(&isr_369);
//   cat.rise(&isr_ABC);

// while (1) {
//     row = row + 1;
//     row %= 2;
//     // if (row == 0) {
//     //     GPIOC->ODR&=~(0x200);
//     //     GPIOC->ODR |= 0x100; //pin 8 
//     // } else {
//     //     GPIOC->ODR&=~(0x100);
//     //     GPIOC->ODR|=0x200; //pin 9
//     // }
//     switch (row) {
//         case 0:
//             GPIOC->ODR&=~(0x200);
//             GPIOC->ODR |= 0x100; //pin 8 
//             // printf("found 3\n");
//             break;
//         case 1:
//             GPIOC->ODR&=~(0x100);
//             GPIOC->ODR|=0x200; //pin 9
//             break;
//     }
//     wait_us(WAIT_TIME_US);
//     wait_us(50);
// }
//   return 0; }

// void isr_369(void) {
//     if (row==0){
//         printf("found 3\n");
//     } else {
//         printf("found 6\n");
//     }
//     wait_us(WAIT_TIME_US);
// }

// void isr_ABC(void) { 
//     if (row==0){
//         printf("found A\n");
//     } else {
//         printf("found B\n");
//     }
//     wait_us(WAIT_TIME_US);
// }

// // void isr_6(void) { printf("found 6\n"); }

// // void isr_b(void) { printf("found B\n"); }
