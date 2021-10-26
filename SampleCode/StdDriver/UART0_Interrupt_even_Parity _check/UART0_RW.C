/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2020 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/***********************************************************************************************************/
/* Website: http://www.nuvoton.com                                                                         */
/*  E-Mail : MicroC-8bit@nuvoton.com                                                                       */
/*  Date   : Apr/29/2020                                                                                   */
/***********************************************************************************************************/

/***********************************************************************************************************
//  File Function: ML51 UART0 receive and transmit loop test
/***********************************************************************************************************/
#include "ML51.h"
bit Even_parity_check=0;
bit uart0_receive_flag=0;
unsigned char uart0_receive_data=0;
void UART0_ISR(void) interrupt 4
{
    _push_(SFRS);
    if (RI)
    {
        uart0_receive_flag = 1;
        ACC = SBUF;
				uart0_receive_data = ACC; // read 8-bits of data

	if (RB8 == P)
	{
			Even_parity_check=1;
	// parity is correct for 'Even' parity systems;
	// incorrect for 'Odd' parity systems
	}
	else
	{
		Even_parity_check=0;
	// parity is correct for 'Odd' parity systems;
	// incorrect for 'Even' parity systems
	}

        clr_SCON_RI;                                         // Clear RI (Receive Interrupt).
    }
    _pop_(SFRS);
}

void UART_Send_Data_Even_Parity(unsigned char c)
{
    SFRS = 0x00;	
        TI = 0;
	      ACC = c;
	      TB8 = P;
        SBUF = c;
        while(TI==0);
        TI = 0;
}

/************************************************************************************************************/
/*  Main function                                                                                           */
/************************************************************************************************************/
 void main(void)
{

    MFP_P31_UART0_TXD;
    P31_QUASI_MODE;
    MFP_P30_UART0_RXD;
    P30_INPUT_MODE;

	        SFRS = 0x00;
          SCON = 0x50;             /*UART0 Mode1,REN=1,TI=1 */
          TMOD |= 0x20;            /*Timer1 Mode1*/
          set_PCON_SMOD;          /*UART0 Double Rate Enable*/
	        set_PCON_SMOD0;         //9 bit mode
          set_CKCON_T1M;
          clr_T3CON_BRCK;          /*Serial port 0 baud rate clock source = Timer3*/
          TH1 = 256 - (24000000/16/115200);
          set_TCON_TR1;

   ENABLE_UART0_INTERRUPT;                                   /* Enable UART0 interrupt */
   ENABLE_GLOBAL_INTERRUPT;                                  /* Global interrupt enable */

	
	UART_Send_Data_Even_Parity(0x03);

  while(1)
  {
    if (uart0_receive_flag&Even_parity_check)//check the Even_parity and data in
    {
      uart0_receive_flag = 0;   
      Even_parity_check=0;			
    }
  }
}