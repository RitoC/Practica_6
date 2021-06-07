/*
 * File:   main.c
 * Author: Labmems
 *
 * Created on 27 de abril de 2021, 12:16 AM
 */

#include "device_config.h"

#define _XTAL_FREQ 8000000
#define SWEEP_FREQ 50

#define LCD_DATA_R          PORTD
#define LCD_DATA_W          LATD
#define LCD_DATA_DIR        TRISD
#define LCD_RS              LATCbits.LATC2
#define LCD_RS_DIR          TRISCbits.TRISC2
#define LCD_RW              LATCbits.LATC1
#define LCD_RW_DIR          TRISCbits.TRISC1
#define LCD_E               LATCbits.LATC0
#define LCD_E_DIR           TRISCbits.TRISC0

enum por_ACDC {digital, analog};
void portsInit(void);
char key_scanner(void);
void send2LCD(char);
void LCD_rdy(void);
void LCD_init(void);
void LCD_cmd(char);
char is_number(char number);
char is_sign(char sign);
char is_enter(char enter);
char is_ac(char ac);

//+++++++++++++++++++++++++++++++++++++| MAIN |+++++++++++++++++++++++++++++++++++++
void main(void){
    OSCCON = 0x64;// Set the internal oscillator to 8MHz and stable
    
    //while(1){
    LCD_DATA_DIR = 0x00;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E  = 0;
    LCD_init();
    
//    send2LCD('P');
//    send2LCD('E');
//    send2LCD('I');
//    send2LCD('P');
//    send2LCD('I');
//    }
    
    portsInit();
    char entry;
    char op1;
    char op2;
    char sign;
    char result;
    while(1){
        while(1){
            entry = key_scanner();
            if(is_number(entry) == 1){
                LCD_cmd(0x07);      //Entry mode set
                __delay_ms(100);
                op1 = entry; 
                LCD_cmd(0x8F);     //posicion del primer operando 0F
                __delay_ms(100);
                send2LCD(op1 + '0');    //poner el primer operando en la LCD
                break;
            }
        }
        while(1){
            entry = key_scanner();
            if(is_sign(entry) == 1){
                sign = entry;
                switch (sign) {     //poner signo en la LCD
                    case 14: send2LCD(43) ;
                    break;
                    case 11: send2LCD(45) ;
                    break;
                    case 12: send2LCD(42) ;
                    break;
                    case 13: send2LCD(47) ;
                    break;
                }
                break;
            }
            if(is_ac(entry) == 1){
                LCD_cmd(0x01);//borrar todo en pantalla
                LCD_cmd(0x02);//resetear posiciones
                break;
            }
        }
        while(1){
            entry = key_scanner();
            if(is_number(entry) == 1){
                op2 = entry;
                send2LCD(op2 + '0');    //poner segundo operando en la LCD
                break;
            }
            if(is_ac(entry) == 1){
                LCD_cmd(0x01);//borrar todo en pantalla
                LCD_cmd(0x02);//resetear posiciones
                break;
            }
        }
        while(1){
            entry = key_scanner();
            if(is_enter(entry) == 1){
                LCD_cmd(0xD1);
                LCD_cmd(0x06);      //Entry mode set DESHABILITAR EL SHIFTING
                switch(sign){
                    case 14: result = op1 + op2 ;
                    break;
                    case 11: result = op1 - op2 ;
                    break;
                    case 12: result = op1 * op2 ;
                    break;
                    case 13: result = op1 / op2 ;
                    break;
                }
                if(result > 9 ){
                    send2LCD(result/10 + '0');    //realizar el calculo y mostrar en LCD   
                    send2LCD(result%10 + '0');    //realizar el calculo y mostrar en LCD
                }
                else{
                    send2LCD('0');    //realizar el calculo y mostrar en LCD
                    send2LCD(result + '0');    //realizar el calculo y mostrar en LCD
                }
                
            }
            if(is_ac(entry) == 1){
                LCD_cmd(0x01);//borrar todo en pantalla
                LCD_cmd(0x02);//resetear posiciones
                break;
            }
        }
        
    }
}

//+++++++++++++++++++++++++++++++++++++| FUNCTIONS |+++++++++++++++++++++++++++++++++++++
char is_number(char number){
    if(number < 10 && number > 0){
        return 1;           
    }
    else{
        return 0;
    }
}

char is_sign(char sign){
    if(sign >=11 && sign <= 14 ){
        return 1;           
    }
    else{
        return 0;
    }
}

char is_enter(char enter){
    if(enter == 15 ){
        return 1;           
    }
    else{
        return 0;
    }
}

char is_ac(char ac){
    if(ac == 10 ){
        return 1;           
    }
    else{
        return 0;
    }
}

char key_scanner(void){
    while(1){
        LATBbits.LB4 = 0;
        LATBbits.LB5 = 1;
        LATBbits.LB6 = 1;
        LATBbits.LB7 = 1;
        __delay_ms(SWEEP_FREQ);
        if      (PORTBbits.RB0 == 0) {__delay_ms(SWEEP_FREQ); return 1;}
        else if (PORTBbits.RB1 == 0) {__delay_ms(SWEEP_FREQ); return 4;}
        else if (PORTBbits.RB2 == 0) {__delay_ms(SWEEP_FREQ); return 7;}
        else if (PORTBbits.RB3 == 0) {__delay_ms(SWEEP_FREQ); return 10;}//*
        LATBbits.LB4 = 1;
        LATBbits.LB5 = 0;
        LATBbits.LB6 = 1;
        LATBbits.LB7 = 1;
        __delay_ms(SWEEP_FREQ);
        if      (PORTBbits.RB0 == 0) {__delay_ms(SWEEP_FREQ); return 2;}
        else if (PORTBbits.RB1 == 0) {__delay_ms(SWEEP_FREQ); return 5;}
        else if (PORTBbits.RB2 == 0) {__delay_ms(SWEEP_FREQ); return 8;}
        else if (PORTBbits.RB3 == 0) {__delay_ms(SWEEP_FREQ); return 0;}
        LATBbits.LB4 = 1;
        LATBbits.LB5 = 1;
        LATBbits.LB6 = 0;
        LATBbits.LB7 = 1;
        __delay_ms(SWEEP_FREQ);
        if      (PORTBbits.RB0 == 0) {__delay_ms(SWEEP_FREQ); return 3;}
        else if (PORTBbits.RB1 == 0) {__delay_ms(SWEEP_FREQ); return 6;}
        else if (PORTBbits.RB2 == 0) {__delay_ms(SWEEP_FREQ); return 9;}
        else if (PORTBbits.RB3 == 0) {__delay_ms(SWEEP_FREQ); return 15;}//#
        LATBbits.LB4 = 1;
        LATBbits.LB5 = 1;
        LATBbits.LB6 = 1;
        LATBbits.LB7 = 0;
        __delay_ms(SWEEP_FREQ);
        if      (PORTBbits.RB0 == 0) {__delay_ms(SWEEP_FREQ); return 14;}//A
        else if (PORTBbits.RB1 == 0) {__delay_ms(SWEEP_FREQ); return 11;}//B
        else if (PORTBbits.RB2 == 0) {__delay_ms(SWEEP_FREQ); return 12;}//C
        else if (PORTBbits.RB3 == 0) {__delay_ms(SWEEP_FREQ); return 13;}//D   
    }
}



void send2LCD(char xy){
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_E  = 1;
    LCD_DATA_W = xy;
    Nop();
    Nop();
    LCD_E  = 0;
    __delay_ms(250);
}

void LCD_init(void){
    LATC = 0;               // Make sure LCD control port is low
    LCD_E_DIR = 0;          // Set Enable as output
    LCD_RS_DIR = 0;         // Set RS as output 
    LCD_RW_DIR = 0;         // Set R/W as output
    LCD_cmd(0x38);          // Display to 2x16
    __delay_ms(250);  
    LCD_cmd(0x0F);          // Display on, cursor on and blinking
    __delay_ms(250);  
    LCD_cmd(0x01);          // Clear display and move cursor home
    __delay_ms(250);  
}

void LCD_rdy(void){
    char test;
    // configure LCD data bus for input
    LCD_DATA_DIR = 0xFF;
    test = 0x80;
    while(test){
        LCD_RS = 0;         // select IR register
        LCD_RW = 1;         // set READ mode
        LCD_E  = 1;         // setup to clock data
        test = LCD_DATA_R;
        Nop();
        LCD_E = 0;          // complete the READ cycle
        test &= 0x80;       // check BUSY FLAG 
    }
}

void LCD_cmd(char cx) {
   // LCD_rdy();              // wait until LCD is ready
    LCD_RS = 0;             // select IR register
    LCD_RW = 0;             // set WRITE mode
    LCD_E  = 1;             // set to clock data
    __delay_ms(25);
    LCD_DATA_W = cx;        // send out command
    __delay_ms(25);                // No operation (small delay to lengthen E pulse)
    LCD_E = 0;              // complete external write cycle
}

void portsInit(void){
    ANSELB = 0;   // Set port A as Digital for keypad driving
    INTCON2 = 0;
    TRISB  = 0x0F;      // For Port A, set pins 4 to 7 as outputs (rows), and pins 0 to 3 as inputs (cols)
}
