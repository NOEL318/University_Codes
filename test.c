#include <xc.h>
#define _XTAL_FREQ 8000000

// ================= CONFIGURACIÓN =================
#pragma config FOSC = XT
#pragma config WDTEN = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF

// ================= DEFINICIÓN DE PINES =================
#define LED_VERDE LATCbits.LC0
#define LED_TEMP LATCbits.LC1
#define LED_HUM LATCbits.LC2
#define BUZZER LATAbits.LA5
#define BOTON PORTAbits.RA2
#define DECENAS LATCbits.LC6
#define UNIDADES LATCbits.LC7
#define DISPLAY LATD
#define LED1 LATBbits.LATB0
#define LED2 LATBbits.LATB1
#define LED3 LATBbits.LATB2
#define LED4 LATBbits.LATB3
#define LED5 LATBbits.LATB4
#define LED6 LATBbits.LATB5
#define LED7 LATBbits.LATB6
#define LED8 LATBbits.LATB7

// si la temperatura llega de los 36 a 38 grados y si el sensor de humedad está de 40-60% se debe encender el "LED_VERDE"
// si no está ninguna de las condiciones se debe encender el led purpura y se le debe agregar una barra al "LEDB"

// ================= TABLA DISPLAY =================
const unsigned char tabla7seg[] = {
		0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// ================= ARREGLO PARA LEDs =================
const unsigned char leds[] = {
		0b11111110, // 1 LED encendido (bit 0 = 0)
		0b11111100, // 2 LEDs
		0b11111000, // 3
		0b11110000, // 4
		0b11100000, // 5
		0b11000000, // 6
		0b10000000, // 7
		0b00000000	// 8
};
volatile unsigned char mostrarTemp = 1;

int contadorEventos = 0;

// ================= INICIALIZACIÓN ADC =================
void ADC_Init()
{
	ADCON0 = 0x01;
	ADCON1 = 0x0F; // Todos los pines digitales
	ADCON2 = 0xA9;
	CM1CON0 = 0x00; // Desactiva comparador 1
	CM2CON0 = 0x00; // Desactiva comparador 2
}

unsigned int ADC_Read(unsigned char canal)
{
	ADCON0bits.CHS = canal;
	__delay_us(10);
	ADCON0bits.GO = 1;
	while (ADCON0bits.GO)
		;
	return ((unsigned int)(ADRESH << 8) | ADRESL);
}

// ================= CONVERSIÓN =================
float leerTemperatura()
{
	unsigned int adc = ADC_Read(0);
	return ((float)adc * 5.0f / 1023.0f) * 100.0f;
}

float leerHumedad()
{
	unsigned int adc = ADC_Read(1);
	float voltaje = ((float)adc * 5.0f) / 1023.0f;
	float humedad = ((voltaje - 0.8f) / 3.2f) * 100.0f;
	humedad *= 1.047f;
	return humedad;
}

// ================= DISPLAY =================
void mostrarNumero(unsigned char dec, unsigned char uni, unsigned char punto)
{
	DECENAS = 1;
	UNIDADES = 0;
	DISPLAY = tabla7seg[dec];
	__delay_ms(5);
	DECENAS = 0;
	UNIDADES = 0;

	DECENAS = 0;
	UNIDADES = 1;
	DISPLAY = tabla7seg[uni];
	if (punto)
		DISPLAY |= 0x80;
	__delay_ms(5);
	DECENAS = 0;
	UNIDADES = 0;
}

// ================= INTERRUPCIÓN =================
void __interrupt() ISR()
{
	if (INTCONbits.TMR0IF)
	{
		mostrarTemp = !mostrarTemp;
		TMR0 = 3036;
		INTCONbits.TMR0IF = 0;
	}
}

// ================= MAIN =================
void main(void)
{
	TRISA = 0x07;
	TRISD = 0x00;
	TRISB = 0x00; // Puerto B como salida
	LATB = 0xFF;	// Inicialmente apagado (común ánodo → LEDs apagados con 1)
	TRISCbits.TRISC0 = 0;
	TRISCbits.TRISC1 = 0;
	TRISCbits.TRISC2 = 0;
	TRISCbits.TRISC6 = 0;
	TRISCbits.TRISC7 = 0;

	ADC_Init();

	T0CON = 0x87;
	TMR0 = 3036;
	INTCONbits.TMR0IE = 1;
	INTCONbits.GIE = 1;

	float temp = 0, hum = 0;

	while (1)
	{
		temp = leerTemperatura();
		hum = leerHumedad();

		// Si cualquiera está fuera de rango, incrementa LEDs y mantenlos encendidos
		if (temp < 33 || temp > 38)
		{
			BUZZER = 1;
			LED_VERDE = 0;
			LED_TEMP = 1;
		}
		if (hum < 40 || hum > 60)
		{
			BUZZER = 1;
			LED_VERDE = 0;
			LED_HUM = 1;

			if (estadohumedad == 0)
			{
				estadohumedad = 1;
				contadorEventos++;
				if (contadorEventos > 8)
					contadorEventos = 1;
			}

			switch (contadorEventos)
			{
			case 1:
				LED1 = 1;
				break;
			case 2:
				LED2 = 1;
				break;
			case 3:
				LED3 = 1;
				break;
			case 4:
				LED4 = 1;
				break;
			case 5:
				LED5 = 1;
				break;
			case 6:
				LED6 = 1;
				break;
			case 7:
				LED7 = 1;
				break;
			case 8:
				LED8 = 1;
				break;
			default:
				contadorEventos = 0;
				LED1 = LED2 = LED3 = LED4 = 0;
				LED5 = LED6 = LED7 = LED8 = 0;
				break;
			}
		}
		else
		{
			BUZZER = 0;
			LED_VERDE = 1;
			LED_HUM = 0;
			estadohumedad = 0;
		}
		else
		{
			BUZZER = 0;
			LED_VERDE = 1;
			LED_TEMP = 0;
			LED_HUM = 0;
		}

		// Botón para reiniciar LEDs
		if (BOTON == 0)
		{
			contadorEventos = 0;
			LED1 = 0;
			LED2 = 0;
			LED3 = 0;
			LED4 = 0;
			LED5 = 0;
			LED6 = 0;
			LED7 = 0;
			LED8 = 0;
		}

		unsigned char valor = mostrarTemp ? (unsigned char)temp : (unsigned char)hum;
		if (valor > 99)
			valor = 99;

		for (int i = 0; i < 100; i++)
		{
			mostrarNumero(valor / 10, valor % 10, mostrarTemp ? 0 : 1);
		}
	}
}