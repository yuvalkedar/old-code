#include <Arduino.h>
#include <FastLED.h>
#include <RTClib.h>

#define LEDS_PIN			(6)
#define LED_COUNT			(58)
#define LED_BRIGHTNESS			(20)


#define DIGITS_COUNT			(4)
#define SEGMENTS_PER_DIGIT		(7)
#define LEDS_PER_SEGMENT		(2)
#define SEGMENT_ON_COLOR		(255)
#define SEGMENT_OFF_COLOR		(0)
#define DOT_ON_COLOR			(255)
#define DOT_OFF_COLOR			(0)
#define DOT_U_LED_INDEX			(56)
#define DOT_D_LED_INDEX			(57)
#define DOT_U_MASK			(0x01)
#define DOT_D_MASK			(0x02)
#define DOT_BOTH_MASK			(DOT_U_MASK | DOT_D_MASK)

#define MIN_PER_HOUR			(60)
#define SECS_PER_MIN			(60)
#define SECS_PER_DAY			(SECS_PER_MIN * MIN_PER_HOUR * 24)
#define MIN_PER_DAY			(SECS_PER_DAY / SECS_PER_MIN)

#define CLOCK_DOT_MS			(650)
#define FIX_COLOR_MS			(10*1000)

CRGB pixels[LED_COUNT];
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

uint8_t fix_clock_color_index;
uint32_t clock_color;

uint8_t bitmask_to_7_seg[SEGMENTS_PER_DIGIT] = {0x10, 0x08, 0x04, 0x02, 0x01, 0x20, 0x40};
	
static const uint8_t FONT[] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,	// 0,1,2,3,4,5,6,7,8,9
	// 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76,	0x30, 0x0E,	// a,b,c,d,e,f,g,h,i,j
	// 0x00, 0x38, 0x00, 0x54, 0x3F, 0x73, 0x67, 0x50, 0x6D, 0x78,	// k,l,m,n,o,p,q,r,s,t
	// 0x3E, 0x1C, 0x00, 0x00, 0x6E, 0x5C, 0x08, 0x40, 0x01, 0x00,	// u,v,w,x,y,z,_,-,^, 
};

static const uint32_t FIX_CLOCK_COLORS[] = {
	0xdddddd,	//dimmer white
	// 0xff008d,	//pink
	// 0xff00ff,	//magenta
	// 0x0000ff,	//blue
	// 0x00ffff,	//cyan
	// 0x00ff00,	//green
	// 0xffff00,	//yellow
	// 0xff7400,	//orange
	// 0xff0000	//red
};

void draw_digit(uint8_t index, uint8_t bitmask)
{
	if (index >= DIGITS_COUNT)
		return;
	uint32_t segment_color;
	index *= SEGMENTS_PER_DIGIT * LEDS_PER_SEGMENT;
	for (uint8_t i = 0; i < SEGMENTS_PER_DIGIT; i++) {
		segment_color = (bitmask & bitmask_to_7_seg[i] ? clock_color : 0);
		pixels[index + i * 2 + 0] = segment_color;
		pixels[index + i * 2 + 1] = segment_color;
	}	
}

uint16_t get_current_time()
{
	// DateTime now = RTC.now();
	DateTime now = rtc.now();
	return now.hour() * MIN_PER_HOUR + now.minute();
	// return 1000;
}

void print_dots(uint8_t dots)
{
	// Blink dots
	// pixels.setPixelColor(DOT_U_LED_INDEX, (dots & DOT_U_MASK ? clock_color : 0));
	// pixels.setPixelColor(DOT_D_LED_INDEX, (dots & DOT_D_MASK ? clock_color : 0));
	
	pixels[DOT_D_LED_INDEX] = FIX_CLOCK_COLORS[0];
	pixels[DOT_U_LED_INDEX] = FIX_CLOCK_COLORS[0];
	FastLED.show();
}

void print_time(uint16_t minutes)
{
	uint8_t hours = minutes / MIN_PER_HOUR;
	minutes %= MIN_PER_HOUR;

	draw_digit(0, (hours > 9 ? FONT[hours / 10 % 10] : 0));
	draw_digit(1, FONT[hours % 10]);
	draw_digit(2, FONT[minutes / 10 % 10]);
	draw_digit(3, FONT[minutes % 10]);
	FastLED.show();
}

void setup()
{
	Serial.begin(115200);

	FastLED.addLeds<WS2812B, LEDS_PIN, GRB>(pixels, LED_COUNT);
	FastLED.setBrightness(LED_BRIGHTNESS);
	FastLED.show();
	
	fix_clock_color_index = 0;
	clock_color = FIX_CLOCK_COLORS[fix_clock_color_index];


	/* test print_time */
	// for (uint8_t i = 0; i < LED_COUNT; i++) {
		// pixels.setPixelColor(i, 0);
		// pixels.show();
	// }
	// for (uint8_t i = 0; i < 10; i++) {
		// print_time(i);
		// delay(1000);
	// }

	if (!rtc.begin()){
		Serial.println("Couldn't find RTC");
		Serial.flush();
		abort();
	}

	// January 21, 2014 at 3am
	rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

void loop()
{
	uint8_t dots = 0;
	uint16_t cur_time, old_time = 0;
	uint32_t cur_ms, old_ms = 0xFFFF, delta_ms, fix_color_ms = 0;
	
	for (;;) {
		cur_ms = millis();
		delta_ms = cur_ms - old_ms;
		
		if (CLOCK_DOT_MS < delta_ms) {
			old_ms = cur_ms;
			cur_time = get_current_time();
			if (old_time != cur_time) {
				print_time(cur_time);
				old_time = cur_time;
			}
			print_dots(dots);
			dots = ~dots;
		}
		if (cur_ms > fix_color_ms) {
			// set next fix color index
			if (++fix_clock_color_index >= (sizeof(FIX_CLOCK_COLORS) / sizeof(*FIX_CLOCK_COLORS)))
				fix_clock_color_index = 0;
			clock_color = FIX_CLOCK_COLORS[fix_clock_color_index];
			fix_color_ms = cur_ms + FIX_COLOR_MS;
			// update display
			print_time(cur_time);
			print_dots(dots);
		}
		delay(1);
	}
}
