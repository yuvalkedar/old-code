#include "pitches.h"
#include "songs.h"
#include <MyRemote.h>

#define SPEAKER		(8)
//#define IR_PIN		(A0)
#define SHAMASH		(2)
#define CANDEL1		(3)
#define CANDEL2		(4)
#define CANDEL3		(5)
#define CANDLE4		(6)
#define CANDEL5		(7)
#define CANDEL6		(8)
#define CANDEL7		(9)
#define CANDEL8		(10)

//keys indexes
#define KEY_ONOFF	0xFFA25D		// power key
#define KEY_NEXT	0xFFC23D		// next song key
#define KEY_PREV	0xFF02FD		// previous song key
#define KEY_PLAY	0xFF22DD		// play/pause song key
#define KEY_0		0xFF6897		// shamash
#define KEY_1		0xFF30CF		// candel1
#define KEY_2		0xFF18E7		// candle2
#define KEY_3		0xFF7A85		// candle3
#define KEY_4		0xFF10EF		// candle4
#define KEY_5		0xFF38C7		// candle5
#define KEY_6		0xFF5AA5		// candle6
#define KEY_7		0xFF42BD		// candle7
#define KEY_8		0xFF4AB5		// candle8


uint8_t song_num = 1;
uint16_t song_notes[]	= {SONG1_NOTES, SONG2_NOTES, SONG3_NOTES, SONG4_NOTES};
uint16_t song_dur[]		= {SONG1_DUR, SONG2_DUR, SONG3_DUR, SONG4_DUR};
uint16_t song_size[]	= {SONG1_SIZE, SONG2_SIZE, SONG3_SIZE, SONG4_SIZE};

bool candles_state[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup(){
//	irBegin(IR_PIN);
	for(uint8_t i = 2; i < 11; i++){
		pinMode(i, OUTPUT);
	}
}

void loop(){
	if(irAvailable()){
		uint32_t key_index = irRead();
		switch(key_index){
			case KEY_ONOFF:
				clear_all();
			break;
			case KEY_NEXT:
				song_num++;
				if(song_num > 4) song_num = 1;
				play_song(song_num);
			break;
			case KEY_PREV:
				song_num--;
				if(song_num < 1) song_num = 3;
				play_song(song_num);
			break;
			case KEY_PLAY:
				clear_all();
				play_song(3);
			break;
			case KEY_0:
				clear_all();
				digitalWrite(2, !candles_state[8]);
			break;
			case KEY_1:
				clear_all();
				digitalWrite(2, !candles_state[8]);
				digitalWrite(3, !candles_state[7]);
			break;
			case KEY_2:
				clear_all();
				digitalWrite(2, !candles_state[8]);
				digitalWrite(3, !candles_state[7]);
				digitalWrite(4, !candles_state[6]);
			break;
			case KEY_3:
				clear_all();
				digitalWrite(2, !candles_state[8]);
				digitalWrite(3, !candles_state[7]);
				digitalWrite(4, !candles_state[6]);
				digitalWrite(5, !candles_state[5]);
			break;
			case KEY_4:
				clear_all();
				digitalWrite(2, !candles_state[8]);
				digitalWrite(3, !candles_state[7]);
				digitalWrite(4, !candles_state[6]);
				digitalWrite(5, !candles_state[5]);
				digitalWrite(6, !candles_state[4]);
			break;
			case KEY_5:
				clear_all();
				digitalWrite(2, !candles_state[8]);
				digitalWrite(3, !candles_state[7]);
				digitalWrite(4, !candles_state[6]);
				digitalWrite(5, !candles_state[5]);
				digitalWrite(6, !candles_state[4]);
				digitalWrite(7, !candles_state[3]);
			break;
			case KEY_6:
				clear_all();
				digitalWrite(2, !candles_state[8]);
				digitalWrite(3, !candles_state[7]);
				digitalWrite(4, !candles_state[6]);
				digitalWrite(5, !candles_state[5]);
				digitalWrite(6, !candles_state[4]);
				digitalWrite(7, !candles_state[3]);
				digitalWrite(8, !candles_state[2]);
			break;
			case KEY_7:
				clear_all();
				digitalWrite(2, !candles_state[8]);
				digitalWrite(3, !candles_state[7]);
				digitalWrite(4, !candles_state[6]);
				digitalWrite(5, !candles_state[5]);
				digitalWrite(6, !candles_state[4]);
				digitalWrite(7, !candles_state[3]);
				digitalWrite(8, !candles_state[2]);
				digitalWrite(9, !candles_state[1]);
			break;
			case KEY_8:
				clear_all();
				uint8_t i = 8;
				for(uint8_t n = 2; n < 11; n++){
					digitalWrite(n, !candles_state[i]);
					i--;
				}
			break;
		}
	}
	delay(10);
}

void play_song(uint8_t num){
	int runIdx = 1;
	int startI = 0;
	int endI = startI + song_size[0];
	while(runIdx < num && runIdx <= song_num + 1){
		runIdx++;
		startI = endI;
		endI = startI + song_size[runIdx-1];
	}
	for(uint8_t note = 0; note < endI+1; note++){
		switch(song_notes[note]){
			case NOTE_C4:
				candles_state[8] = !candles_state[8];
				digitalWrite(3, candles_state[8]);
				break;
			case NOTE_D4:
			case NOTE_D5:
				candles_state[7] = !candles_state[7];
				digitalWrite(4, candles_state[7]);
				break;     
			case NOTE_E4:
				candles_state[6] = !candles_state[6];
				digitalWrite(5, candles_state[6]);
				break;       
			case NOTE_F4:
				candles_state[5] = !candles_state[5];
				digitalWrite(6, candles_state[5]);
				break;   
			case NOTE_FS4:
				candles_state[4] = !candles_state[4];
				digitalWrite(2, candles_state[4]);
				break;   
			case NOTE_G4:
				candles_state[3] = !candles_state[3];
				digitalWrite(7, candles_state[3]);
				break;   
			case NOTE_A3:      
			case NOTE_A4:
				candles_state[2] = !candles_state[2];
				digitalWrite(8, candles_state[2]);
				break;   
			case NOTE_B3:      
			case NOTE_B4:
				candles_state[1] = !candles_state[1];
				digitalWrite(9, candles_state[1]);
				break;  
			case NOTE_C5:
				candles_state[0] = !candles_state[0];
				digitalWrite(10, candles_state[0]);
				break;           
		}
		// to calculate the note duration, take one second divided by the note type.
		int noteDuration = 1000 / song_dur[note];
		tone(SPEAKER, song_notes[note], noteDuration);
		
		// to distinguish the notes, set a minimum time between them.
		int pauseBetweenNotes = noteDuration * 2;
		delay(pauseBetweenNotes);
		// stop the tone playing:
		noTone(SPEAKER);
	}
}

void clear_all(){
	uint8_t i = 8;
	for(uint8_t n = 2; n < 11; n++){
		candles_state[i] = 0;
		digitalWrite(n, candles_state[i]);
		i--;
	}
}
