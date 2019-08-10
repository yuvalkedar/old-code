#include <SoftwareSerial.h>
#include <Max7219RGMatrix.h>
#include "fonts/MyHeb5x7.h"

// #define USE_DOUBLE_BUFFER		// Use if you use a lot of graphics... if comment so comment AUTO_UPDATE_BUFFER too!
#define WIDTH					(16)
#define HEIGHT					(8)
#define DISPLAY_MONOCOLOR
#define COMMAND_END_CHARACTER	('\x0D')
#define TEXT_BUFFER_SIZE		(40)
#define SERIAL_BUFFER_SIZE		(TEXT_BUFFER_SIZE + 5)

SoftwareSerial bluetooth_serial(3, 2); // RX, TX

char serial_buffer[SERIAL_BUFFER_SIZE];
char text_buffer[TEXT_BUFFER_SIZE] = "HELLO LORD";

uint8_t direction = SCROLL_LEFT_IN_OUT;
uint16_t text_steps;

uint32_t text_scroll_ms = 80;
uint32_t old_text_scroll_ms;


void refresh_text(){
	fillDisplay(BLACK_C);
	text_steps = scrollMsg(MSG_A, text_buffer, direction, 0, 0, (uint8_t *)MyHeb5x7, 1, RED_C, BLACK_C);
}

void setup(){
	bluetooth_serial.begin(9600);
	bluetooth_serial.setTimeout(10);
	
	begin(7, 6, 5);					// dataPin, clockPin, loadPin
	fillDisplay(BLACK_C);

	// drawPixel(0, 0, RED_C); delay(1000);
	refresh_text();
}

void loop(){
	// uint8_t read_bytes_count = bluetooth_serial.readBytesUntil(COMMAND_END_CHARACTER, serial_buffer, SERIAL_BUFFER_SIZE);
	uint8_t read_bytes_count = bluetooth_serial.readBytes(serial_buffer, SERIAL_BUFFER_SIZE);
	uint32_t ms = millis();
	if(read_bytes_count){
		// serial_buffer[read_bytes_count - 1] = NULL;
		serial_buffer[read_bytes_count++] = NULL;		//	do for me the COMMAND_END_CHARACTER
		switch(serial_buffer[0]){
			case 't':
			case 'T':
				memcpy(text_buffer, serial_buffer + 2, read_bytes_count - 1);	// copies read_bytes_count-1 number of characters from serial_buffer to text_buffer
				refresh_text();
			break;
			case 's':
			case 'S':
				text_scroll_ms = atoi(serial_buffer + 2);
			break;
			case 'l':
			case 'L':
				direction = SCROLL_LEFT_IN_OUT;
				refresh_text();
			break;
			case 'r':
			case 'R':
				direction = SCROLL_RIGHT_IN_OUT;
				refresh_text();
			break;
			case 'g':
			case 'G':
			
			break;
			case '?':
				bluetooth_serial.print(
					"IDF_MATRIX_V1.0\n"
					"===============\n"
					"each command must end with '\\n'\n"
					"t <text> // the text to scroll\n"
					"s <speed> // text scrolling speed\n"
					"l // left scroll direction\n"
					"r // right scroll direction\n"
					"g // snake game\n"
					"? // display this help message\n");
			break;
		}
	}
	/* scroll the text one pixel at a time */
	if((ms - old_text_scroll_ms) > text_scroll_ms){
		old_text_scroll_ms = ms;
		if(text_steps--)
			moveMsg(MSG_A);
		else
			refresh_text();
	}
	delay(1);
}
