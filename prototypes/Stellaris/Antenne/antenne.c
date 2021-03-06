//***********************************************************
//
//       antenne.c
//       Fonctions de d�codage d'antenne
//       2013-03-10
//        
//
//***********************************************************
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/udma.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include <string.h>

#include "antenne.h"

//***********************************************************
//
// Table de contr�le du contr�leur DMA
//
//***********************************************************
#if defined(ewarm)
#pragma data_alignment=1024
unsigned char ucControlTable[1024];
#elif defined(ccs)
#pragma DATA_ALIGN(ucControlTable, 1024)
unsigned char ucControlTable[1024];
#else
unsigned char ucControlTable[1024] __attribute__ ((aligned(1024)));
#endif


//*****************************************************************************
//
// Initialisation du TIMER1B 
//
//*****************************************************************************
void initTimer1B()
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	//
    // Configure the Timer1 CCP3 function to use PB2
    //
    GPIOPinConfigure(GPIO_PB2_CCP3);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_2);
    //
    // Set up Timer1B for edge-timer mode, both edges
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_B_CAP_TIME);
    TimerControlEvent(TIMER1_BASE, TIMER_B, TIMER_EVENT_BOTH_EDGES);
    TimerLoadSet(TIMER1_BASE, TIMER_B, 0xffff);
}

//*****************************************************************************
//
// Initialisation du uDMA
//
//*****************************************************************************
void initDMA(unsigned short *ptrBuffer)
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
	//
    // Enable the uDMA controller error interrupt.  This interrupt will occur
    // if there is a bus error during a transfer.
    //
    ROM_IntEnable(INT_UDMAERR);
    //
    // Enable the uDMA controller.
    //
    ROM_uDMAEnable();
    //
    // Point at the control table to use for channel control structures.
    //
    ROM_uDMAControlBaseSet(ucControlTable);
    //
    // Put the attributes in a known state for the uDMA Timer1B channel.  These
    // should already be disabled by default.
    //
    ROM_uDMAChannelAttributeDisable(UDMA_CHANNEL_TMR1B,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY |
                                    UDMA_ATTR_REQMASK);
    //
    // Configure DMA channel for Timer1B to transfer 16-bit values, 1 at a
    // time.  The source is fixed and the destination increments by 16-bits
    // (2 bytes) at a time.
    //
    ROM_uDMAChannelControlSet(UDMA_CHANNEL_TMR1B | UDMA_PRI_SELECT,
                              UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
                              UDMA_DST_INC_16 | UDMA_ARB_1);

    //
    // Set up the transfer parameters for the Timer1B primary control
    // structure.  The mode is set to basic, the transfer source is the
    // Timer1B register, and the destination is a memory buffer.  The
    // transfer size is set to a fixed number of capture events.
    //
    ROM_uDMAChannelTransferSet(UDMA_CHANNEL_TMR1B | UDMA_PRI_SELECT,
                               UDMA_MODE_BASIC,
                               (void *)(TIMER1_BASE + TIMER_O_TBR),
                               ptrBuffer, MAX_TIMER_EVENTS);
}

//*****************************************************************************
//
// D�marrage du timer et du DMA
//
//*****************************************************************************
void startTimer1B()
{
    //TimerIntEnable(TIMER1_BASE, TIMER_CAPB_EVENT);
    TimerEnable(TIMER1_BASE, TIMER_B);
    //IntEnable(INT_TIMER1B);

    //
    // Now enable the DMA channel for Timer1B.  It should now start performing
    // transfers whenever there is a rising edge detected on the CCP3 pin.
    //
    ROM_uDMAChannelEnable(UDMA_CHANNEL_TMR1B);
}

//*****************************************************************************
//
// Lecture du signal d'antenne 
// Retourne 0 si lecture correcte, 1 si erreur
//
//*****************************************************************************
void readSignal(unsigned short *ptrBuffer, unsigned short *signalBuffer, unsigned int *signalBufferSize)
{
	unsigned long ulIdx;
    unsigned short usTimerElapsed;
    *signalBufferSize = 0;
    for(ulIdx = 1; ulIdx < MAX_TIMER_EVENTS; ulIdx++)
    {
        //
        //   Le DMA lance aussi une interruption lorsqu'il atteint 
        //   0 dû à une erreur. On sort ne fait rien lorsque c'est 
        //   le cas.
        //
        if(ptrBuffer[ulIdx] == ptrBuffer[ulIdx - 1])
        {
            continue;
        }
        usTimerElapsed = ptrBuffer[ulIdx - 1] - ptrBuffer[ulIdx];
        *signalBuffer = usTimerElapsed;
        signalBuffer++;
        *signalBufferSize++;
    }
    //g_bDoneFlag = 0;
    //g_ulTimer0BIntCount = 0;
}
//*****************************************************************************
//
// Fonction d'inversion des membres d'un array
//
//*****************************************************************************
void reverse(unsigned short *s[2])
{
    int i ,j;
    unsigned short c, d;
    for(i=0, j=(sizeof(s)/4)-1; i<j; i++,j--)
    {
        c = s[i][0];
        d = s[i][1];
        s[i][0] = s[j][0];
        s[i][1] = s[j][1];
        s[j][0] = c;
        s[j][1] = d;
    }
}

int bitsCompare(unsigned char s1[7], unsigned char s2[7])
{
	int i;
	int cmp = 0;
	for(i = 0; i < 7; i++)
	{
		if(s1[i] != s2[i])
		{
			cmp = 1;
		}	
	}
	return cmp;
}

//*****************************************************************************
//
// D�codage du signal d'antenne 
// Retourne 0 si lecture correcte, 1 si erreur
//
//*****************************************************************************
int decodeSignal(unsigned short *signalBuffer, unsigned int signalBufferSize, unsigned char bitsDecode[7])
{
    unsigned short *cleanData;
    unsigned int dataSize = 0;
    unsigned short smallest_0 = 0xFFFF;
    unsigned short smallest_1 = 0xFFFF;
    int idxA;
    int s_indx = 0;

    // 
    //   Trouver l'élément valide le plus petit et retrait des artéfacts
    //
    for(idxA = 0; idxA < signalBufferSize; idxA++)
    {
        if(signalBuffer[idxA] < smallest_0 && signalBuffer[idxA] > 2000)
        {
            dataSize++;
            *cleanData = signalBuffer[idxA];
            cleanData++;
            if(signalBuffer[idxA] < smallest_0)
            {
                smallest_0 = signalBuffer[idxA];
                s_indx = idxA;
            }
        }
    }
    unsigned short *valuedData[2];
    unsigned short toggle = 0;
    idxA = s_indx;
    //
    //  Assignation des 0 et des 1 à partir de smallest_0
    //
    while(idxA >= 0)
    {
        valuedData[idxA][1] = toggle;
        valuedData[idxA][0] = cleanData[idxA];
        idxA--;
        if(toggle == 0)
        {
            toggle = 1;
        }
        else
        {
            toggle = 0;
        }
    }
    reverse(valuedData);

    idxA = s_indx + 1;
    toggle = 1;
    while(idxA < dataSize)
    {
        valuedData[idxA][1] = toggle;
        valuedData[idxA][0] = cleanData[idxA];
        idxA++;
        if(toggle == 0)
        {
            toggle = 1;
        }
        else
        {
            toggle = 0;
        }
    }

    // 
    //   Trouver le plus petit 1
    //
    for(idxA = 0; idxA < dataSize; idxA++)
    {
        if(valuedData[idxA][1] == 1)
        {
            if(valuedData[idxA][0] < smallest_1)
                smallest_1 = valuedData[idxA][0];
        }
    }

    //
    //   S�quen�aage : On attribue une valeur digitale � chaque timing selon des correspondances suivantes:
    //   10 - un seul z�ro ; 11 - un seul '1' ; 20 - deux z�ros cons�cutifs ; 21 - deux '1' cons�cutifs
    //

    unsigned short *seqData;
    for(idxA = 0; idxA < dataSize; idxA++)
    {
        if(valuedData[idxA][1] == 0)
        {
            if(valuedData[idxA][0] > smallest_0*FACTEUR_0)
                seqData[idxA] = 20;
            else
                seqData[idxA] = 10;
        }
        else
        {
            if(valuedData[idxA][0] > smallest_1*FACTEUR_1)
                seqData[idxA] = 21;
            else
                seqData[idxA] = 11;
        }
    }

    //
    //   Algorithme de Knuth-Moris-Pratt (modifi�)
    //   On recherche la s�quence P des bits d'arr�t et de d�part dans l'�chanillon seqDat
    //

    int *T;
    int sizeT = 0;
    unsigned short P[15] = {10,11,10,11,10,11,10,11,10,11,10,11,10,11,20};

    //
    //   1 - Construction du tableau des d�calages
    //

    for(idxA = dataSize-1; idxA >= 0; idxA--)
    {
        if(seqData[idxA] == 20 && idxA < dataSize - 14 && idxA >= 15 + dataSize)
        {
            *T = idxA;
            T++;
            sizeT++;
        }
            

    }

    //
    //   2 - Recherche de la s�quence
    //

    int debut = -1;

    int m = 0;
    int i = 14;

    if(sizeT > 0)
    {
        while((T[m] - 14 + i) >= 0 && i >= 0 && m < 14)
        {
            if(seqData[T[m] - 14 + i] == P[i])
                i--;
            else
            {
                m++;
                i = 14;
                if(m >= sizeT)
                    break;
            }
        }

        if(i == -1)
        {
            debut = T[m];
        }
        else
            debut = -1;
    }
    else
    {
        debut = -1;
    } 

    // 
    //           D�codage
    //

    if(debut != -1)
    {
        int currentBit = 0;
        idxA = debut + 1;
        int previousBit = 0;
        while(currentBit < 7)
        {
            if(previousBit == 0 && seqData[idxA] == 11)
            {
                bitsDecode[currentBit] = '0';
                currentBit++;
                idxA++;
            }
            else if(previousBit == 0 && seqData[idxA] == 21)
            {
                bitsDecode[currentBit] = '1';
                currentBit++;
                idxA = idxA + 2;
            }
            else if(previousBit == 1 && seqData[idxA] == 10)
            {
                bitsDecode[currentBit] = '1';
                currentBit++;
                idxA = idxA + 2;
            }
            else if(previousBit == 1 && seqData[idxA] == 20)
            {
                bitsDecode[currentBit] = '0';
                currentBit++;
                idxA++;
            }
        }

        // V�rification de la s�quence

        currentBit = 6;
        idxA = debut - 15;
        unsigned char backDecode[7];
        previousBit = 1;
        while(currentBit >= 0)
        {
            if(previousBit == 1 && seqData[idxA] == 21)
            {
                backDecode[currentBit] = '0';
                idxA--;
                currentBit--;
            }
            else if(previousBit == 1 && seqData[idxA] == 11)
            {
                backDecode[currentBit] = '1';
                idxA = idxA - 2;
                currentBit--;
            }
            else if(previousBit == 0 && seqData[idxA] == 10)
            {
                backDecode[currentBit] = '0';
                idxA = idxA - 2;
                currentBit--;
            }
            else if(previousBit == 0 && seqData[idxA] == 20)
            {
                backDecode[currentBit] = '1';
                idxA--;
                currentBit--;
            }
        }

        if(bitsCompare(bitsDecode, backDecode) == 0)
        {
            return 0;
        }
        else
            return 1;
    }
    else
        return 1;
}
