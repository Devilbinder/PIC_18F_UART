#include <xc.h>
#include <pic18f4520.h>
#include "conbits.h"
#include "stdint.h"
#include "stdbool.h"

uint8_t data = 'A';
bool got_data_bool = false;

void __interrupt() high_isr(void);
void __interrupt(low_priority) low_isr(void);


void uart_init(uint16_t gen_reg, unsigned sync,unsigned brgh, unsigned brg16){
    
    TRISCbits.RC7=1;
    TRISCbits.RC6=1;
    
    SPBRGH = (gen_reg & 0xFF00) >> 8;
    SPBRG = gen_reg & 0x00FF;
    
    RCSTAbits.CREN = 1;
    RCSTAbits.SPEN = 1;
    BAUDCONbits.BRG16 = brg16;
    
    TXSTAbits.SYNC = sync;
    TXSTAbits.BRGH = brgh;
    TXSTAbits.TXEN = 1;
    
    IPR1bits.RCIP=1;
    PIE1bits.RCIE=1;
    
   // IPR1bits.TXIP=0;
    //PIE1bits.TXIE=1;   
}

void uart_send(uint8_t *c){
    TXREG=*c;
    while(TXSTAbits.TRMT==0){
        Nop();
    }
}

void uart_receiver(uint8_t *c, bool *rx_flag){
    
    if(RCSTAbits.FERR){
        uint8_t er = RCREG;
    }else if(RCSTAbits.OERR){
        RCSTAbits.CREN=0;
        RCSTAbits.CREN=1;
    }else{
        *c = RCREG;
        *rx_flag = true;
    }
    
}

void main(void) {

    OSCCONbits.IRCF = 0x07;
    OSCCONbits.SCS = 0x03;
    while(OSCCONbits.IOFS!=1);
    
    uart_init(51,0,1,0);
    
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1; 
    INTCONbits.GIEL = 1;
    
    while(1){
    // __delay_ms(200);
     
    // uart_send(&data);
        
        if(got_data_bool){
            uart_send(&data);
            got_data_bool = false;
        }
        
    } 
}

void __interrupt() high_isr(void){
    INTCONbits.GIEH = 0;
    if(PIR1bits.RCIF){
        uart_receiver(&data,&got_data_bool);
       PIR1bits.RCIF=0;
    }
    
    INTCONbits.GIEH = 1;
}

void __interrupt(low_priority) low_isr(void){
    INTCONbits.GIEH = 0;
    if(PIR1bits.TXIF){
        PIR1bits.TXIF=0;
    }
    INTCONbits.GIEH = 1;
}



