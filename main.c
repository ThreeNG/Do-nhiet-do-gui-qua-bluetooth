#include <msp430g2553.h>

float temp_C=0.0;
float temp_F=0.0;
int j,i,w =0;
unsigned int nadc=0;
unsigned char b[4], c[5];
unsigned char a=2;

void adcinit()
{
	ADC10CTL0 = SREF_1 + ADC10SHT_3  + ADC10SR ;
	ADC10CTL1 = ADC10SSEL_0 + ADC10DIV_3 + INCH_10 + CONSEQ_0;
	__delay_cycles(50);
	ADC10CTL0 |= REFON;
	__delay_cycles(50);
	ADC10CTL0 |= ADC10ON;
	
}



void serialInit()
{
	P1SEL= BIT1 + BIT2; //P1.1 = RXD P1.2=TXD
	P1SEL2= BIT1 +BIT2; // P1.1=RXD & P1.2=TXD
	UCA0CTL1|= UCSSEL_2; // SMCLK
	UCA0BR0=104; // BAUDRATE AT 1 MHz 9600
	UCA0BR1=0;//1MHz 9600
    UCA0MCTL= UCBRS0; // MODULATION UCBRSx=1
    UCA0CTL1&=~UCSWRST; // ** INITIALIZE USCI STATE MACHINE
    IE2|= UCA0RXIE; // ENABLE VSCI_A0 RX INTERRUPT
}

unsigned char serialRead()
{
	while(!(IFG2&UCA0RXIFG));   //USCI_A0 RX buffer ready ?
	return UCA0RXBUF;

}
void serialWrite(unsigned char c)
{
	while(!(IFG2&UCA0TXIFG));  // USCI_A0 TX buffer ready ?
	UCA0TXBUF=c; // TX
}
void serialwriteString(const char *str)
{
	while(*str)
		serialWrite(*str++);
}

void serial_write_int(unsigned int temp)
{
	for( i=0;i<4;i++)
	{
		b[i]=temp%10;
		//serialWrite(b[i]);
		temp=temp/10;
	}

	for(j=3;j>=0;j--)
	{
		serialWrite(b[j] + 48);
	}

	serialWrite(' ');
	serialWrite('\n');

}


serial_write_float(float v)
{

	w = v*100;

	for(i=0;i<4;i++)
	{
		c[i]=w%10;
		w=w/10;
	}

	serialWrite(c[3]+48);
	serialWrite(c[2] +48);
	serialWrite('.');
	serialWrite(c[1] + 48);
	serialWrite(c[0] + 48);

	serialWrite(' ');
	serialWrite('\n');





}

void main()
{
	WDTCTL = WDTPW + WDTHOLD;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
    P1DIR = 0x41;
    P1OUT=0x00;


	adcinit();
	serialInit();

	while(1)
	{

		ADC10CTL0 |= ENC + ADC10SC;
		while(ADC10CTL1 & ADC10BUSY)
		{}
//serialwriteString("hello !!");
//serialWrite('\n');
		nadc= ADC10MEM;

		temp_C=(((nadc*420.0)+512.0)/1024.0) - 278.0;
		temp_F= ((temp_C)*(9.0/5.0)) + 32.0;

		a=serialRead();

		if(a== '0')

		{

			P1OUT = 0x41; // both leds are ON
					serialwriteString("Digital ADC value :");
			serial_write_int(nadc);
		}

		else if (a== '1')
		{

			P1OUT=0x01; // only red led is blinking
			__delay_cycles(5000);
			P1OUT=0x00;
			serialwriteString("Celsius :");
		serial_write_float(temp_C);

		}

		else if (a =='2')

		{
			P1OUT=0x40; // only green led is blinking
			__delay_cycles(5000);
			P1OUT=0X00;
			serialwriteString("Farenheit :");
			serial_write_float(temp_F);

		}

		}

}
