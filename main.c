////+++++++++++++++++++++++++++++++++++++| LIBRARIES / HEADERS |+++++++++++++++++++++++++++++++++++++
#include "device_config.h"
#include <stdint.h>
#include <math.h>

//+++++++++++++++++++++++++++++++++++++| DIRECTIVES |+++++++++++++++++++++++++++++++++++++
#define _XTAL_FREQ 4000000
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

//+++++++++++++++++++++++++++++++++++++| DATA TYPES |+++++++++++++++++++++++++++++++++++++
enum por_ACDC {digital, analog};          // digital = 0, analog = 1

////+++++++++++++++++++++++++++++++++++++| ISRs |+++++++++++++++++++++++++++++++++++++
//// ISR for high priority
//void __interrupt ( high_priority ) high_isr( void );
//// ISR for low priority
//void __interrupt ( low_priority ) low_isr( void ); 

//+++++++++++++++++++++++++++++++++++++| FUNCTION DECLARATIONS |+++++++++++++++++++++++++++++++++++++
void portsInit(void);
char key_scanner(void);
void LCD_init(void);
void LCD_cmd(char cx);
void send2LCD(char xy);
void LCD_rdy(void);
char is_number(char number);
char is_sign(char sign);
char is_enter(char enter);
char is_ac(char ac);

//+++++++++++++++++++++++++++++++++++++| MAIN |+++++++++++++++++++++++++++++++++++++
void main(void){
    portsInit();
    LCD_init();
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
                op1 = entry; 
                LCD_cmd(0x8F);     //posicion del primer operando 0F
                send2LCD(op1 + '0');    //poner el primer operando en la LCD
                break;
            }
        }
        while(1){
            entry = key_scanner();
            if(is_sign(entry) == 1){
                sign = entry;
                switch (sign) {     //poner signo en la LCD
                    case 10: send2LCD(43) ;
                    break;
                    case 11: send2LCD(45) ;
                    break;
                    case 12: send2LCD(42) ;
                    break;
                    case 13: send2LCD(246) ;
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
                    case 10: result = op1 + op2 ;
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
                
                break;
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
    if(number < 10 ){
        return 1;           
    }
    else{
        return 0;
    }
}

char is_sign(char sign){
    if(sign >=10 && sign <= 13 ){
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
    if(ac == 14 ){
        return 1;           
    }
    else{
        return 0;
    }
}

char key_scanner(void){
    LATAbits.LA0 = 0;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 1;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 2;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 3;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 10;}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 0;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 4;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 5;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 6;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 11;}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 0;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 7;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 8;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 9;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 12;}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 0;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 14;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 0;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 15;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 13;}   
    else return 'x';
}


void portsInit(void){
    ANSELA = digital;   // Set port A as Digital for keypad driving
    TRISA  = 0xF0;      // For Port A, set pins 4 to 7 as inputs (columns), and pins 0 to 3 as outputs (rows)
    ANSELC = digital;   // Set port C as Digital for 7 segment cathode selection (only 4 pins used)  
    TRISC  = 0x00;      // For Port C, set pins as outputs for cathode selection
    ANSELD = digital;   // Set port D as Digital for 7 segment anodes
    TRISD  = 0x00;      // for Port D, set all pins as outputs for 7 segment anodes
    OSCCON = 0x74;      // Set the internal oscillator to 8MHz and stable
}

// LCD initialization function
void LCD_init(void){
    LATC = 0;               // Make sure LCD control port is low
    LCD_E_DIR = 0;          // Set Enable as output
    LCD_RS_DIR = 0;         // Set RS as output 
    LCD_RW_DIR = 0;         // Set R/W as output
    LCD_cmd(0x38);          // Display to 2x40
    LCD_cmd(0x0F);          // Display on, cursor on and blinking
    LCD_cmd(0x01);          // Clear display and move cursor home
}

// Send command to the LCD
void LCD_cmd(char cx) {
    LCD_rdy();              // wait until LCD is ready
    LCD_RS = 0;             // select IR register
    LCD_RW = 0;             // set WRITE mode
    LCD_E  = 1;             // set to clock data
    Nop();
    LCD_DATA_W = cx;        // send out command
    Nop();                  // No operation (small delay to lengthen E pulse)
    LCD_E = 0;              // complete external write cycle
}

// Function to display data on the LCD
void send2LCD(char xy){
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_E  = 1;
    LCD_DATA_W = xy;
    Nop();
    Nop();
    LCD_E  = 0;
}

// Function to wait until the LCD is not busy
void LCD_rdy(void){
    char test;
    // configure LCD data bus for input
    LCD_DATA_DIR = 0b11111111;
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