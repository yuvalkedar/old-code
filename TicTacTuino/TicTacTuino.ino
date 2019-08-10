/*
	year: 2016
	name: TicTacTuino04.
	author: arad_eizen and yuval_Kedar.
	platform: arduino pro mini.
	3x3 RGB LEDs joystick for TicTacToe game.
*/

#include <Buttons.h>


//{ defines:
#define LEDS_IN_SIDE				(3)
#define LEDS_COUNT					(LEDS_IN_SIDE * LEDS_IN_SIDE)

#define RED_LED_PIN					(3)
#define GREEN_LED_PIN				(5)
#define BLUE_LED_PIN				(6)
#define COUNTER_RESET_PIN			(7)
#define COUNTER_CLOCK_PIN			(8)
#define JOY_BUTTON_PIN				(9)
#define MOTOR_PIN					(10)
#define JOY_X_PIN					(A0)
#define JOY_Y_PIN					(A1)

#define JOY_LIMIT					(220)
#define JOY_LEFT_LIMIT				(JOY_LIMIT)
#define JOY_RIGHT_LIMIT				(1023 - JOY_LIMIT)
#define JOY_UP_LIMIT				(JOY_LIMIT)
#define JOY_DOWN_LIMIT				(1023 - JOY_LIMIT)

#define LED_MASK_X					(0b101010101)
#define LED_MASK_O					(0b010101010)
#define LED_MASK_SQUARE				(0b111101111)

#define RGB_COLOR_BLACK				(0b000)
#define RGB_COLOR_RED				(0b001)
#define RGB_COLOR_GREEN				(0b010)
#define RGB_COLOR_BLUE				(0b100)
#define RGB_COLOR_YELLOW			(RGB_COLOR_RED 	 | RGB_COLOR_GREEN)						// (0b011)
#define RGB_COLOR_CYAN				(RGB_COLOR_GREEN | RGB_COLOR_BLUE)						// (0b110)
#define RGB_COLOR_MAGENTA			(RGB_COLOR_BLUE	 | RGB_COLOR_RED)						// (0b101)
#define RGB_COLOR_WHITE				(RGB_COLOR_RED 	 | RGB_COLOR_GREEN | RGB_COLOR_BLUE)	// (0b111)
//}

//{ globals
uint16_t red_leds_mask		= 0b000000000;
uint16_t green_leds_mask	= 0b000000000;
uint16_t blue_leds_mask		= 0b000000000;
Buttons joystick;
Buttons button;
uint8_t curser_x;
uint8_t curser_y;
bool player;
//}

//{ helpers
void togglePin(uint8_t pin) {
	digitalWrite(pin, HIGH);
	digitalWrite(pin, LOW);
}

void refresh() {
	togglePin(COUNTER_RESET_PIN);
	for(uint16_t i = 0b000000001; i <= 0b100000000; i *= 2) {
		digitalWrite(RED_LED_PIN,	(i & red_leds_mask)		!= 0);	//
		digitalWrite(GREEN_LED_PIN,	(i & green_leds_mask)	!= 0);	//CR: What is this for? declaring their pins LOW isn't enough?
		digitalWrite(BLUE_LED_PIN,	(i & blue_leds_mask)	!= 0);	//
		delayMicroseconds(10);
		digitalWrite(RED_LED_PIN,	LOW);
		digitalWrite(GREEN_LED_PIN,	LOW);
		digitalWrite(BLUE_LED_PIN,	LOW);
		togglePin(COUNTER_CLOCK_PIN); // shift to the next led
	}
}

void delay_with_refresh(uint32_t ms) {
	uint32_t start_ms = millis();
	while ((start_ms + ms) > millis()) refresh();
}

uint16_t get_leds_mask(uint8_t led_index) {
	return (uint16_t)1 << led_index;	//CR: shouldn't it be led_index-1?
}

uint16_t get_leds_mask(uint8_t x, uint8_t y) {
	return get_leds_mask(x + y * 3);	//CR: affected by the function above.
}

/*
void set_leds_masks(uint16_t r, uint16_t g, uint16_t b, uint32_t ms) {
	red_leds_mask	= r;
	green_leds_mask	= g;
	blue_leds_mask	= b;
	delay_with_refresh(ms);
}

void set_led_color(uint8_t led_index, uint8_t rgb_color) {
	uint16_t mask = get_leds_mask(led_index);
	
	if (rgb_color & RGB_COLOR_RED)		red_leds_mask |= mask;
	else								red_leds_mask &= ~mask;
	if (rgb_color & RGB_COLOR_GREEN)	green_leds_mask |= mask;
	else								green_leds_mask &= ~mask;
	if (rgb_color & RGB_COLOR_BLUE)		blue_leds_mask |= mask;
	else								blue_leds_mask &= ~mask;	//CR: How does that change the color?
}

void set_led_color(uint8_t x, uint8_t y, uint8_t rgb_color) {
	set_led_color(x + y * 3, rgb_color);
}*/

//}

bitmask_t getJoystickBitmask() {
	/* return the joystick diraction as bitmask */
	bitmask_t joystick_bitmask = 0;
	uint16_t axis_value = analogRead(JOY_X_PIN);

	if		(axis_value < JOY_LEFT_LIMIT)	joystick_bitmask = JOYSTICK_MASK_LEFT;
	else if	(axis_value > JOY_RIGHT_LIMIT)	joystick_bitmask = JOYSTICK_MASK_RIGHT;
	axis_value = analogRead(JOY_Y_PIN);
	if		(axis_value < JOY_UP_LIMIT)		joystick_bitmask |= JOYSTICK_MASK_UP;
	else if	(axis_value > JOY_DOWN_LIMIT)	joystick_bitmask |= JOYSTICK_MASK_DOWN;
	return joystick_bitmask;
}

bitmask_t getButtonBitmaskFunc() {
	/* did the joystick button is down? */
	return (digitalRead(JOY_BUTTON_PIN) ? 0 : 1);			// returns the scanned buttons(s) state
}

void onShortPressJoystick() {
	/* get joystick diraction */
	bitmask_t bitmask = joystick.getButtonsBitmask();
	
	/* turn off the old cursser led */
	blue_leds_mask &= ~get_leds_mask(curser_x, curser_y);
	
	if		(bitmask & JOYSTICK_MASK_LEFT) {
		if (curser_x > 0)	curser_x--;
		else				curser_x = LEDS_IN_SIDE - 1;
	}
	else if	(bitmask & JOYSTICK_MASK_RIGHT) {
		if (curser_x < (LEDS_IN_SIDE - 1))	curser_x++;
		else				curser_x = 0;
	}
	if		(bitmask & JOYSTICK_MASK_UP) {
		if (curser_y > 0)	curser_y--;
		else				curser_y = LEDS_IN_SIDE - 1;
	}
	else if	(bitmask & JOYSTICK_MASK_DOWN) {
		if (curser_y < (LEDS_IN_SIDE - 1))	curser_y++;
		else				curser_y = 0;
	}

	/* turn on the current cursser led */
	blue_leds_mask |= get_leds_mask(curser_x, curser_y);
}

void joy_vibration() {
	for(uint8_t i = 0; i < 5; i++) {
		digitalWrite(10, HIGH);
		delay_with_refresh(250);
		digitalWrite(10, LOW);
		delay_with_refresh(250);
	}
}

bool is_three_sequence(uint16_t mask) {
	/* did the given mask won the game? */
	if ((mask & 0b000000111) == 0b000000111) return true;
	if ((mask & 0b000111000) == 0b000111000) return true;
	if ((mask & 0b111000000) == 0b111000000) return true;
	if ((mask & 0b100100100) == 0b100100100) return true;
	if ((mask & 0b010010010) == 0b010010010) return true;
	if ((mask & 0b001001001) == 0b001001001) return true;
	if ((mask & 0b001010100) == 0b001010100) return true;
	if ((mask & 0b100010001) == 0b100010001) return true;
	return false;
}

void onShortPressButton() {
	uint16_t mask = get_leds_mask(curser_x, curser_y);
	/* check that the led under the cruser is empty */
	if (((mask & red_leds_mask) != 0) || ((mask & green_leds_mask) != 0))
		return;
	/* get pointer to the current mask (player = true - red, else green) */
	uint16_t * player_mask = (player ? &red_leds_mask : &green_leds_mask);
	/* turn on the led under the cruser */
	*player_mask |= mask;
	
	/* did the player won? */
	if (is_three_sequence(*player_mask)) {
		joy_vibration();
		delay_with_refresh(500);
		red_leds_mask = 0;
		green_leds_mask = 0;
		blue_leds_mask = 0;
		*player_mask |= 0b111111111;
		delay_with_refresh(1500);
		game_restart();
	}
	/* did all leds are on? */
	else if ((red_leds_mask | green_leds_mask) == 0b111111111) {
		joy_vibration();
		red_leds_mask = 0b111111111;
		green_leds_mask = 0b111111111;
		blue_leds_mask = 0;
		delay_with_refresh(1500);
		game_restart();
	}
	/* go on... */
	else {
		player = !player;
	}
}

void game_restart() {
	red_leds_mask = 0;
	green_leds_mask = 0;
	blue_leds_mask = 0b000010000;
	curser_x = 1;
	curser_y = 1;
	player = false;
}

//{ setup and loop
void setup() {
	Serial.begin(115200);
	pinMode(JOY_BUTTON_PIN,		INPUT_PULLUP);
	pinMode(RED_LED_PIN,		OUTPUT);
	pinMode(GREEN_LED_PIN,		OUTPUT);
	pinMode(BLUE_LED_PIN,		OUTPUT);
	pinMode(COUNTER_RESET_PIN,	OUTPUT);
	pinMode(COUNTER_CLOCK_PIN,	OUTPUT);
	pinMode(JOY_X_PIN,			INPUT);
	pinMode(JOY_Y_PIN,			INPUT);
	
	joystick.begin(getJoystickBitmask);	
	joystick.setPressMillis(
		70,							// short_press_millis - the minimal amount of milliseconds for a "short press".
		200,						// long_press_millis - the minimal amount of milliseconds for a "long press".
		250							// pulse_press_millis - the amount of milliseconds between "long pulse".
	);
	joystick.setShortPressFunc(
		onShortPressJoystick		// function that will be called once when button is down for more then short_press_millis.
	);
	joystick.setLongPulseFunc(
		onShortPressJoystick		// function that will be called once when button is down for more then short_press_millis.
	);
	
	button.begin(getButtonBitmaskFunc);
	button.setShortPressFunc(
		onShortPressButton		// function that will be called once when button is down for more then short_press_millis.
	);
	game_restert();
}

void loop() {
	// update the joystick and keypad states and call events if needed, max 50ms between calls.
	joystick.update();
	button.update();
	delay_with_refresh(1);

	//Serial.println(analogRead(A1));
	/*
	if (digitalRead(JOY_BUTTON_PIN))
		set_leds_masks(LED_MASK_X, 0, 0, 1);
	else
		set_leds_masks(0, LED_MASK_O, 0, 1);
	*/
	/*
	set_leds_masks(LED_MASK_X, 0, 0, 1000);
	set_leds_masks(0, LED_MASK_O, 0, 1000);
	set_leds_masks(0, 0, LED_MASK_SQUARE, 1000);
	*/
	/*
	static uint8_t led_index = 0;
	set_led_color(led_index, RGB_COLOR_CYAN);
	if (++led_index >= LEDS_COUNT) led_index = 0;
	delay_with_refresh(500);
	*/
	/*
	red_leds_mask	= 0b111000000;
	green_leds_mask	= 0b000111000;
	blue_leds_mask	= 0b000000111;
	delay_with_refresh(2000);
	
	red_leds_mask	= 0b000111000;
	green_leds_mask	= 0b000000111;
	blue_leds_mask	= 0b111000000;
	delay_with_refresh(2000);

	red_leds_mask	= 0b000000111;
	green_leds_mask	= 0b111000000;
	blue_leds_mask	= 0b000111000;
	delay_with_refresh(2000);
	//red_leds_mask = buttons_leds_mask;
	
	static uint8_t i = 0;
	if (i++ == 0)
	{
		if (red_leds_mask < 0b100000000)
			red_leds_mask *= 2;
		else
			red_leds_mask = 1;
		//red_leds_mask = ~red_leds_mask;
		Serial.println(red_leds_mask, BIN);
	}
	refresh();
*/
}
//}
