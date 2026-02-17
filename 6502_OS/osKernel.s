ACR = $600B			; 6522 Auxiliary Control Register
DDRA = $6003			; 6522 Data Direction Register for Port A
DDRB = $6002			; 6522 Data Direction Register for Port B
IER = $600E			; 6522 Interrupt Enable Register
IFR = $600D			; 6522 Interrupt Flag Register
PORTA = $6001			; 6522 Register for Port A
PORTB = $6000			; 6522 Register for Port B
T1CH = $6005			; 6522 Timer 1 High Order Counter
T1CL = $6004			; 6522 Timer 1 Low Order Counter

E = %10000000 			; Enable bit for sending instructions to LCD
RW = %01000000
RS = %00100000



	.org $8010
nmi:
	;; disable interrupts until we are done handling this one
	sei

	;; push a as we are about to overwrite it
	;; pull it back when storing the thread's context
	pha

	;; Load these registers from the 6522 to reset the timer
	lda IFR
	lda IER
	lda T1CL
	
	lda $0002
	cmp #0

	beq switch_to_thread_1

# this is just dummy code for now...