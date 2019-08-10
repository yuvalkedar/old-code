#define R_LED   (9)
#define G_LED   (10)
#define B_LED   (11)

char buffer[7];
int pointer	= 0;
int inByte	= 0;

uint8_t r = 0;
uint8_t g = 0;
uint8_t b = 0;

void setup() {
	Serial.begin(115200);
}

void loop() {
	if (Serial.available() > 0) {
		inByte = Serial.read();		//read the incoming byte
		if (inByte == '#') {
			while (pointer < 6) {
				buffer[pointer] = Serial.read();	//store in the buffer
				pointer++;	// move the pointer forward by 1
			}
			
			// now we have the 3 numbers stored as hex numbers
			// we need to decode them into 3 bytes r, g and b
			r = hex2dec(buffer[1]) + hex2dec(buffer[0]) * 16;
			g = hex2dec(buffer[3]) + hex2dec(buffer[2]) * 16;
			b = hex2dec(buffer[5]) + hex2dec(buffer[4]) * 16;
			
			pointer = 0;	// reset the pointer so we can reuse the buffer
		}
	}
	
	analogWrite(R_LED, r);
	analogWrite(G_LED, g);
	analogWrite(B_LED, b);
	delay(100);
}

int hex2dec(uint8_t c) {
	if(c >= '0' && c <= '9') return c - '0';
	else if(c >= 'A' && c <= 'F') return c - 'A' + 10;
}
