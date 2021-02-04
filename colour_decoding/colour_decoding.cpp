#include <Arduino.h>

#define R_LED   (3)
#define G_LED   (5)
#define B_LED   (6)

#define SERIAL_BAUDRATE			(115200)
#define SERIAL_TIMEOUT			(1)
#define COMMAND_BUFFER_SIZE		(7)

struct {
	char buffer[COMMAND_BUFFER_SIZE];
	uint8_t buffer_index = 0;
	uint8_t prefix = 0;
} command;

uint8_t r = 0;
uint8_t g = 0;
uint8_t b = 0;

int hex2dec(uint8_t c)
{
	if(c >= '0' && c <= '9') return c - '0';
	else if(c >= 'A' && c <= 'F') return c - 'A' + 10;
}

void setup()
{
	Serial.begin(SERIAL_BAUDRATE);
	Serial.setTimeout(SERIAL_TIMEOUT);
}

void loop()
{
	if (Serial.available() > 0) {
		command.prefix = Serial.read();
		if (command.prefix == '#') {
			while (command.buffer_index < COMMAND_BUFFER_SIZE - 1) {
				command.buffer[command.buffer_index] = Serial.read();
				command.buffer_index++;
			}
			
			// decoding hex numbers into 3 bytes r, g, and b
			r = hex2dec(command.buffer[1]) + hex2dec(command.buffer[0]) * 16;
			g = hex2dec(command.buffer[3]) + hex2dec(command.buffer[2]) * 16;
			b = hex2dec(command.buffer[5]) + hex2dec(command.buffer[4]) * 16;

			analogWrite(R_LED, r);
			analogWrite(G_LED, g);
			analogWrite(B_LED, b);

			command.buffer_index = 0;	// reset the pointer so we can reuse the buffer
		}
	}
	
	delay(100);
}
