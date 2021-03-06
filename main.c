/********************************************************************************
Magstripe Emulator and Penetration Tester 0.9
by James Camden Martin
********************************************************************************/

/********************************************************************************
Includes
********************************************************************************/
#include "globaldefinitions.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include <stdio.h>
#include <stdlib.h>

#include <util/delay.h>

#include "display.h"
#include "millis.h"
#include "magnet.h"
#include "keypad.h"

//Data entry modules
#include "bruteforce.h"
#include "manualcodeentry.h"
#include "savedcodemenu.h"
/********************************************************************************
Macros and Defines
********************************************************************************/
#define mainMenuMaxPos 4 
#define debugMenuMaxPos 4 

/********************************************************************************
Function Prototypes
********************************************************************************/


/********************************************************************************
Global Constants
********************************************************************************/
enum menuState { MAIN, BRUTEFORCEWILDCARD, SAVEDCODES, SAVEDCODEEDITOR, DEBUGMENU };

#define testcardlength 5
char testcarddata[testcardlength]={';','1','2','3','?'};

/********************************************************************************
Global Variables
********************************************************************************/
enum menuState menustate = MAIN;
char menuPosition=0;
char testkeyinput=0;

/********************************************************************************
Writes to display based on current menu state
********************************************************************************/
void updateMenuDisplay(void){
    display_setCursor(0,0);
    switch(menustate){
    case MAIN:
        switch(menuPosition){
        case 0:
            display_println("#=Manual Entry");
            break;
        case 1:
            display_println("#=EditSavedCodes");
            break;
        case 2:
            display_println("#=BruteForceWild");
            break;
        case 3:
            display_println("#=BruteForceMax");
            break;
        case 4:
            display_println("#=Debug Menu");
            break;
        default:
            display_clear();
            display_println("Eror: StateUnknwn");
            char textout[10];
            display_setCursor(0,1);
            itoa(menuPosition, textout, 10);
            display_println(textout);
            return;
            break;
        }
        display_setCursor(0,1);
        display_prints("1=Up 2=Dwn 3=Sav");
        break;
    case DEBUGMENU:
        switch(menuPosition){
        case 0:
            display_println("#=Output Test");
            display_setCursor(0,1);
            display_prints("1=Up 2=Dn 3=Back");
            break;
        case 1:
            display_println("Blank");
            display_setCursor(0,1);
            display_prints("1=Up 2=Dn 3=Back");
            break;
        case 2:
            display_println("Blank2");
            display_setCursor(0,1);
            display_prints("1=Up 2=Dn 3=Back");
            break;
        case 3:
            display_println("Blank3");
            display_setCursor(0,1);
            display_prints("1=Up 2=Dn 3=Back");
            break;
        case 4:
            display_println("Blank4");
            display_setCursor(0,1);
            display_prints("1=Up 2=Dn 3=Back");
            break;
        default:
            display_clear();
            display_println("Eror: StateUnknwn");
            char textout[10];
            display_setCursor(0,1);
            itoa(menuPosition, textout, 10);
            display_println(textout);
            break;
        }
    default:
        display_println("Eror: UnkwnMenu");
        break;
    }
}

/********************************************************************************
Main
********************************************************************************/

int main(void) {    
    // initialize pins
    OUTPUT(pinEMAGN);
    OUTPUT(pinEMAGS);
    LOW(pinEMAGN);
    LOW(pinEMAGS);
    OUTPUT(testLED);
    
    display_init(1);
    
    display_setCursor(0,0);
    display_prints("#=Manual Entry");
    display_setCursor(0,1);
    display_prints("1=Up 2=Dwn 3=Sav");

    //Debug code for display and user input:
    //display_clear();
    //display_setCursor(0,0);
    /*
    //Turn on ADC and set prescaler to 64. F_CPU/Prescaler needs to be >50khz and <200khz for atmega8
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1);
    //Analog input pin select
    ADMUX=0x05;

    while(1){
        ADCSRA  |= (1<<ADSC); //start analog to digital conversion
        while (ADCSRA &  (1<<ADSC));  // wait until analog to digital conversion completes; ADSC=0 means Complete
        int present_key_input = ADCW;
        
        char textout[10];
        display_clear();
        itoa(present_key_input, textout, 10);
        display_prints(textout);
        _delay_ms(500);
    }*/


    /*while(1){
        display_setCursor(0,0);
        char text[3];
        text[0]=' ';
        text[1]=inputpoll(500);
        text[2]='\0';
        if(text[1]==-1){
            text[0]='-';
            text[1]='1';	
        }
            
        display_print(text);
    }*/

    
    millis_init(); //Initialize millisecond timer
    millis_pause();
    sei(); // Enable interrupts
    
    //main loop, menu navigation handled here
    while (true) {
        char key;
        switch(menustate){
        case MAIN:;
            updateMenuDisplay();
            key=inputpoll(-1);
            switch(key){
            case '1':
                if(menuPosition==0)
                menuPosition=mainMenuMaxPos;
                else
                menuPosition-=1;

                break;
            case '2':
                menuPosition+=1;
                if(menuPosition>mainMenuMaxPos)
                menuPosition=0;
                break;
            case '3': //Go to the saved code shortcut menu
                savedCodeShortcutMenu();
                break;
            case '#':
                switch(menuPosition){
                case 0:
                    manualCodeEntry();
                    break;
                case 1: //Edit saved codes
                    savedCodeEditor();
                    break;
                case 2:
                    wildcardBruteForce();
                    break;
                case 3: //Brute force Max
                    display_clear();
                    display_prints("Not Implemented");
                    _delay_ms(500);
                case 4:
                    menustate=DEBUGMENU;
                    menuPosition=0;
                    break;									
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        case DEBUGMENU:;
            updateMenuDisplay();
            key=inputpoll(-1);
            switch(key){
            case '1':
                if(menuPosition==0)
                menuPosition=debugMenuMaxPos;
                else
                menuPosition-=1;
                break;
            case '2':
                menuPosition+=1;
                if(menuPosition>debugMenuMaxPos)
                menuPosition=0;
                break;
            case '3': //Back to main menu
                menustate=MAIN;
                menuPosition=0;
                break;
            case '#': //#, activate the selected menu item
                switch(menuPosition){
                case 0:
                    while(1){
                        display_clear();
                        display_setCursor(0,0);
                        display_prints("1=Output 2=Back");
                        display_setCursor(0,1);
                        display_prints("Data:");
                        display_setCursor(4,1);
                        display_printl(testcarddata, testcardlength, 0);

                        key=inputpoll(-1);
                        if(key=='1'){
                            display_clear();
                            display_setCursor(0,0);
                            display_prints("Outputting...");
                            resetMagnetBitBuffer();
                            for(uint8_t i=0;i<testcardlength;i++){
                                writeCharToBuffer(testcarddata[i]);
                            }
                            writeOutEMagBitBuffer();
                            _delay_ms(1000); //Delay so the user doesn't instantly trigger it again
                        }else if(key=='2'){
                            updateMenuDisplay();
                            break;
                        }
                    }
                    break;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

    }

    return 1;
}
