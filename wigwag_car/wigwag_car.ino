/*
	Wig-Wag car is an electric 3D printed car (real size)
	based on 1203A drivers.
	date: May 2017
	author: Yuval Kedar	
*/

#define X_POT_PIN		(A0)	// left-right
#define Y_POT_PIN		(A1)	// back-forward
#define REVERSE_LIGHT	(A5)
#define FRONT_LIGHT		(A4)
#define X_OUTPUT		(5)		// left-right
#define Y_OUTPUT		(6)		// back-forward
#define RED_PIN			(9)
#define GREEN_PIN		(10)
#define BLUE_PIN		(11)
#define X_DIR_PIN		(12)	// left-right
#define Y_DIR_PIN		(13)	// back-forward
#define R_LIMIT			(7)
#define L_LIMIT			(8)
#define SPEED_LEVELS	(8)
#define MIN_X_POS		(222)
#define MID_X_POS		()
#define MAX_X_POS		(956)
#define MIN_Y_POS		(190)
#define MID_Y_POS		()
#define MAX_Y_POS		(939)


void setup(){
	Serial.begin(115200);
	pinMode(X_POT_PIN,		INPUT);
	pinMode(Y_POT_PIN,		INPUT);
	pinMode(X_OUTPUT,		OUTPUT);
	pinMode(Y_OUTPUT,		OUTPUT);
	pinMode(RED_PIN,		OUTPUT);
	pinMode(GREEN_PIN,		OUTPUT);
	pinMode(BLUE_PIN,		OUTPUT);
	pinMode(X_DIR_PIN,		OUTPUT);
	pinMode(Y_DIR_PIN,		OUTPUT);
	pinMode(FRONT_LIGHT,	OUTPUT);
	pinMode(REVERSE_LIGHT,	OUTPUT);
	pinMode(R_LIMIT,		INPUT);
	pinMode(L_LIMIT,		INPUT);
}

void loop(){
	int32_t x_potValue = 0;
	int32_t y_potValue = 0;
	bool right_ls = 0;
	bool left_ls = 0;

	right_ls = digitalRead(R_LIMIT);
	left_ls = digitalRead(L_LIMIT);
	
	x_potValue = map(analogRead(X_POT_PIN), MIN_X_POS, MAX_X_POS, SPEED_LEVELS, -SPEED_LEVELS) - 1;
	y_potValue = map(analogRead(Y_POT_PIN), MIN_Y_POS, MAX_Y_POS, -SPEED_LEVELS, SPEED_LEVELS) + 1;
	
	if (abs(y_potValue) > 0)
		y_potValue = map(analogRead(X_POT_PIN), MID_X_POS, MAX_X_POS, 0, SPEED_LEVELS);
	map(analogRead(X_POT_PIN), MID_X_POS, MIN_X_POS, 0, -SPEED_LEVELS);
	
	Serial.print	("x: ");
	Serial.print	(x_potValue);
	Serial.print	(" y: ");
	Serial.print	(y_potValue);
	Serial.print	("	R_LS: ");
	Serial.print	(right_ls);
	Serial.print	(" L_LS: ");
	Serial.println	(left_ls);
	
	analogWrite(X_OUTPUT, map(abs(x_potValue), 0, SPEED_LEVELS, 0, 255));
	digitalWrite(X_DIR_PIN, x_potValue > 0);
	
	analogWrite(Y_OUTPUT, map(abs(y_potValue), 0, SPEED_LEVELS, 0, 255));
	digitalWrite(Y_DIR_PIN, y_potValue > 0);
	
	if (abs(y_potValue) >= 2)
		digitalWrite(FRONT_LIGHT, LOW);
	else digitalWrite(FRONT_LIGHT, HIGH);
	if (y_potValue <= -2)
		digitalWrite(REVERSE_LIGHT, LOW);
	else digitalWrite(REVERSE_LIGHT, HIGH);
	
	delay(1);
}
