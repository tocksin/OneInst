#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "memorymap.h"

uint16_t codeStart;
uint16_t codeIndex;

struct CodeLine {
    char label[20];
    uint16_t byte;
    char missingUpper[20];
    char missingLower[20];
} code[65536];

void move(uint16_t src, uint16_t dst)
{
    code[codeIndex].byte = src;
    codeIndex++;
    code[codeIndex].byte = dst;
    codeIndex++;
}

/* load: moves a constant into a destination */
void load(uint16_t constant, uint16_t dst)
{
    move(constant,LOAD);
    move(LOAD,dst);
}

void label(char labelstring[20])
{
    if (0==strcmp(code[codeIndex].label,""))
    {
        strcpy(code[codeIndex].label,labelstring);
    }
    else 
    {
        printf("ERROR: overwriting label.  Current:%s, Overwriting:%s\n",code[codeIndex].label,labelstring);
        exit(0);
    }
}

void labelc(char labelstring[20],uint16_t count)
{
    char combined[20];
    sprintf(combined, "%s%u", labelstring, count);
    label(combined);
}

void jump(char labelstring[20])
{
    uint16_t i;
    for (i=codeStart;i<codeIndex;i+=2)
    {
        if (0==strcmp(labelstring,code[i].label))
        {
            if ((i/256)!=(codeIndex/256))
            {
                load((i/256),PCTEMP);
            }
            load((i%256),PCLO);
            return;
        }
    }

    strcpy(code[codeIndex].missingUpper,labelstring);
    load(0x00,PCTEMP);
    strcpy(code[codeIndex].missingLower,labelstring);
    load(0x00,PCLO);
}

void jumpc(char labelstring[20],uint16_t count)
{
    char combined[20];
    sprintf(combined, "%s%u", labelstring, count);
    jump(combined);
}

void branchif1(uint16_t dst, char labelstring[20])
{
    uint16_t i;

    move(dst, PTRADR);
    for (i=codeStart;i<codeIndex;i+=2)
    {
        if (0==strcmp(labelstring,code[i].label))
        {
            if ((i/256)!=(codeIndex/256))
            {
                load((i/256),PCTEMP);
            }
            load((i%256),PTRDAT);
            return;
        }
    }

    strcpy(code[codeIndex].missingUpper,labelstring);
    load(0x00,PCTEMP);
    strcpy(code[codeIndex].missingLower,labelstring);
    load(0x00,PTRDAT);
}

void branchif1c(uint16_t dst, char labelstring[20], uint16_t count)
{
    char combined[20];
    sprintf(combined, "%s%u", labelstring, count);
    branchif1(dst, combined);
}

void memload(uint16_t address, uint16_t dataword)
{
    uint8_t lower_byte;
    
    load((address/256), DRAMHI);
    
    lower_byte = address%256;
    load(lower_byte, DRAMLO);

    if (0x80>lower_byte)
    {
        lower_byte = lower_byte+0x80;
    }
    load(dataword/256,lower_byte);
    load(dataword%256,lower_byte+1);
    
}

/* memread: moves the value at address in memory to dest
 */
void memread(uint16_t address, uint16_t dest)
{
    uint8_t lower_byte;
    
    load((address/256), DRAMHI);
    
    lower_byte = address%256;
    load(lower_byte, DRAMLO);

    if (0x80>lower_byte)
    {
        lower_byte = lower_byte+0x80;
    }
    move (lower_byte,dest);
}

/* memwrite: moves the value in src to the address in memory
 */
void memwrite(uint16_t address, uint16_t src)
{
    uint8_t lower_byte;
    
    load((address/256), DRAMHI);
    
    lower_byte = address%256;
    load(lower_byte, DRAMLO);

    if (0x80>lower_byte)
    {
        lower_byte = lower_byte+0x80;
    }
    move (src,lower_byte);
}


/* rotateleft:  rotates left the value in ALUA */
void rotateleft(void)
{
    static uint16_t counter=0;
    counter++;

    move(TRASH,SHL0);
    move(SHL0,ALUA);
    branchif1c(CARRY,"ROTATE_ADD",counter);
    jumpc("ROTATE_END",counter);
    
    labelc("ROTATE_ADD",counter);
    move(TRASH,APLUS1);
    move(APLUS1,ALUA);
    
    labelc("ROTATE_END",counter);
    move(TRASH,TRASH);
}

/* inc16: increment a 16-bit number
    the address is the LSB of the 16-bit number
    the MSD is address+1
    the result is placed back into the address
 */
void inc16(uint16_t address)
{
    static uint16_t counter=0;
    counter++;
    
    memread(address, ALUA);
    move(TRASH, APLUS1);
    memwrite(address, APLUS1);
    branchif1c(CARRY,"INC_UPPER",counter);
    jumpc("INCDONE",counter);
    labelc("INC_UPPER",counter);
    memread(address+1,ALUA);
    move(TRASH, APLUS1);
    memwrite(address+1, APLUS1);
    labelc("INCDONE",counter);
    move(TRASH,TRASH); // holds the label location
}

/* add16: adds two 16-bit numbers
 *  a and b are the two inputs
    all inputs and outputs must point to memory locations on the same page
    the pointers point to the LSB of the number
    the MSB is the pointer+1
 */
void add16(uint16_t a, uint16_t b, uint16_t result)
{
    memread(a, ALUA);
    memread(b, ADD);
    memwrite(ADD, result);
    branchif1(CARRY,"ADD_W_CARRY");
    
    label("ADD_W_CARRY");
}

void secondPass(void)
{
    uint16_t i, j;
    uint8_t found = 0;
    
    for (i=codeStart;i<codeIndex;i+=1)
    {
        if (0!=strcmp(code[i].missingUpper,""))
        {
            printf("Searching for %s\n",code[i].missingUpper);
            for (j=i; j<codeIndex;j+=1)
            {
                if (0==strcmp(code[i].missingUpper,code[j].label))
                {
                    code[i].byte=(j/256);
                    printf("%s at %X\n",code[j].label,code[i].byte);
                    found = 1;
                }
            }
            if (0==found)
            {
                printf("Error: Label not found in second pass : %s\n",code[i].missingUpper);
                exit(0);
            }
            found = 0;
        }
        if (0!=strcmp(code[i].missingLower,""))
        {
            printf("Searching for %s\n",code[i].missingLower);
            for (j=i; j<codeIndex;j+=1)
            {
                if (0==strcmp(code[i].missingLower,code[j].label))
                {
                    code[i].byte=(j%256);
                    printf("%s at %X\n",code[j].label,code[i].byte);
                    found = 1;
                }
            }
            if (0==found)
            {
                printf("Error: Label not found in second pass : %s\n",code[i].missingLower);
                exit(0);
            }
            found = 0;
        }
    }

}

void sendUart(uint16_t src)
{
    static uint16_t counter=0;
    counter++;

    labelc("SEND",counter);
    branchif1c(UTXBSY, "SEND", counter);
    move(src, UDAT);
}

void printCode(void)
{
    uint16_t i;
    for (i=codeStart;i<codeIndex;i+=2)
    {
        if (0!=strcmp(code[i].label,""))
        {
            printf("%s\n",code[i].label);
        }
        printf("%04X : %02X %02X\n",i,code[i].byte,code[i+1].byte);
    }
}

void printBoot(void)
{
    uint16_t i;
    for (i=codeStart;i<codeIndex;i+=1)
    {
        printf("W%04X%02X\n",i,code[i].byte,code[i+1].byte);
    }
    printf("J%04X\n",codeStart);
}

void printVHDL(void)
{
    uint16_t i;
    for (i=codeStart;i<codeIndex;i+=2)
    {
        printf("%u => x\"%02X\",  ",i,code[i].byte);
        printf("%u => x\"%02X\",\n",i+1,code[i+1].byte);
    }
    
}

/* sendByteToAscii: convert a byte to two-digit ASCII
 *  and send it to the UART
 */
void sendByteToAscii(uint16_t address)
{
    static uint16_t counter=0;
    counter++;

    //upper 4 bits
    memread(address, ALUA);
    //load (0xB7,ALUA);

    rotateleft();   // shift right by 4
    rotateleft();
    rotateleft();
    rotateleft();
    load(0x0F,ANDA);
    move(ANDA,ALUA);
    
    load(0x0A,GTA);             // branch if number is >9
    branchif1c(CARRY,"UPPER_LETTER",counter);
    load(0x30,ADD);             // add ASCII 0 
    jumpc("SEND_UPPER",counter);
    labelc("UPPER_LETTER",counter);
    load(0x37,ADD);             // add ASCII A
    labelc("SEND_UPPER",counter);
    move(TRASH,TRASH);
    sendUart(ADD);              // send number
    
    //check lower 4 bits
    memread(address, ALUA);
    //load (0xB7,ALUA);

    load(0x0F,ANDA);             // remove upper bits
    move(ANDA, ALUA);
    load(0x0A,GTA);             // branch if number is >9
    branchif1c(CARRY,"LOWER_LETTER",counter);
    load(0x30,ADD);             // add ASCII 0 
    jumpc("SEND_LOWER",counter);
    labelc("LOWER_LETTER",counter);
    load(0x37,ADD);             // add ASCII A
    labelc("SEND_LOWER",counter);    
    move(TRASH,TRASH);
    sendUart(ADD);              // send number
}


void aluTester(uint16_t start)
{
    if (0!=start)
    {
        codeStart = start;
        codeIndex = codeStart;
    }

    label("ALU_TEST");
    load(0x35, ALUA);
    load(0x03, SUB);
    move(SUB,   UDAT);
    jump("ALU_TEST");
}

void uartEcho(uint16_t start)
{
    if (0!=start)
    {
        codeStart = start;
        codeIndex = codeStart;
    }
    
    label("UART_ECHO");
    branchif1(URXEMP, "UART_ECHO");
    sendUart(UDAT);
    jump("UART_ECHO");
}

void uartEcho2(uint16_t start)
{
    if (0!=start)
    {
        codeStart = start;
        codeIndex = codeStart;
    }

    label("UART_ECHO");
    branchif1(UTXBSY, "UART_ECHO");
    branchif1(URXRDY, "UART_SEND");
    jump("UART_ECHO");
    label("UART_SEND");
    move(UDAT,      UDAT);
    jump("UART_ECHO");
}
  
#define STACK_POINTER (0xFFFE)
#define STACK_INIT (0xFFFD)

void initStack(uint16_t start)
{
    uint8_t lower_byte;
    
    if (0!=start)
    {
        codeStart = start;
        codeIndex = codeStart;
    }
    /*
    lower_byte = STACK_POINTER%256;
    
    load((STACK_POINTER/256), DRAMHI);
    load(lower_byte, DRAMLO);

    if (0x80>lower_byte)
    {
        lower_byte = lower_byte+0x80;
    }
    load(STACK_INIT/256,lower_byte);
    load(STACK_INIT%256,lower_byte+1);
    */
    memload(STACK_POINTER,STACK_INIT);
}

uint8_t main()
{
    codeStart = 0x9000;
    codeIndex = codeStart;
//    aluTester(0);
//    uartEcho(0x9100);
//    uartEcho2(0x9100);
//    initStack(0x8100);

//    sendByteToAscii(0x8100);

//    load(0x0D,ALUA);
//    sendUart(ALUA);

    sendByteToAscii(0x8011);
    sendByteToAscii(0x8010);
    load(0x0D, ALUA);
    sendUart(ALUA);

    inc16(0x8010);

    sendByteToAscii(0x8011);
    sendByteToAscii(0x8010);
    load(0x0D, ALUA);
    sendUart(ALUA);

    load(0x05,PCTEMP);
    load(0x00,PCLO);
    
    secondPass();

    printCode();
    //printVHDL();
    //printBoot();
    
    return 0;
}
