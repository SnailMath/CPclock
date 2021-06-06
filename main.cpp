#include "calc.hpp"
#include "rtc.hpp"

#ifndef PC
	APP_NAME("CPclock")
	APP_DESCRIPTION("A clock for the cp400. Use the arrow keys to change time and use KEYBOARD to save.")
	APP_AUTHOR("SnailMath")
	APP_VERSION("1.0.0")
#endif

void circle(int x, int y, int r, uint16_t c);

int sinelist[16]={0,15,31,46,61,74,88,100,111,121,129,137,142,146,149,149};

int sine(int x) {
	if(x>30)
		return -sine(60-x);
	if(x>15)
		x=30-x;
	if(x>15||x<0)
		return 0;
	return sinelist[x];
}

int cosine(int x){
	x-=15;
	if(x<0)
		x+=60;
	return(sine(x));
}

void hand(int x, int w, uint16_t c){
	triangle(160-w,160-w,160+w,160+w,160+sine(x),160+cosine(x),c,c);
	triangle(160-w,160+w,160+w,160-w,160+sine(x),160+cosine(x),c,c);
}

void square(int x, int y, int w, int h, uint16_t c){
	for(int ix=0; ix<w; ix++){
		for(int iy=0; iy<h; iy++){
			setPixel(x+ix,y+iy,c);
		}
	}
}

void seg(int x, int y, int w, int w2, uint8_t b, uint16_t c){
	if(b&0b10000000) square(x   , y      , w2+w, w   , c);
	if(b&0b01000000) square(x+w2, y      , w   , w2+w, c);
	if(b&0b00100000) square(x+w2, y+w2   , w   , w2+w, c);
	if(b&0b00010000) square(x   , y+w2+w2, w2+w, w   , c);
	if(b&0b00001000) square(x   , y+w2   , w   , w2+w, c);
	if(b&0b00000100) square(x   , y      , w   , w2+w, c);
	if(b&0b00000010) square(x   , y+w2   , w2+w, w   , c);
}
void segdigit(int x, int y, int w1, int w2, uint8_t n, uint16_t c){
	if(n==0) seg(x,y,w1,w2,0b11111100,c);
	if(n==1) seg(x,y,w1,w2,0b01100000,c);
	if(n==2) seg(x,y,w1,w2,0b11011010,c);
	if(n==3) seg(x,y,w1,w2,0b11110010,c);
	if(n==4) seg(x,y,w1,w2,0b01100110,c);
	if(n==5) seg(x,y,w1,w2,0b10110110,c);
	if(n==6) seg(x,y,w1,w2,0b10111110,c);
	if(n==7) seg(x,y,w1,w2,0b11100100,c);
	if(n==8) seg(x,y,w1,w2,0b11111110,c);
	if(n==9) seg(x,y,w1,w2,0b11110110,c);
}
//prints the bcd number as a seven segment digit to the screen
void segbyte(int x, int y, int w1, int w2, uint8_t n, uint16_t c){
	segdigit(x,       y, w1, w2, n>>4,  c);//upper nibble
	segdigit(x+w2+w2, y, w1, w2, n&0xf, c);//lower nibble
}
void segbyte4(int x, int y, int w1, int w2, uint16_t n, uint16_t c){
	segdigit(x,       y, w1, w2, (n>>12)&0xf,  c);
	segdigit(x +2*w2, y, w1, w2, (n>> 8)&0xf,  c);
	segdigit(x +4*w2, y, w1, w2, (n>> 4)&0xf,  c);
	segdigit(x +6*w2, y, w1, w2, (n    )&0xf,  c);
}

//increments a digit from the value x (digit 0 is the 1's place, 1 is the 10's place...)
int bcdinc(int x, int inc, int digit, int max){
	//x is the input number
	//inc is 1 or -1
	//digit is 0 for 1's place, 1 for 10's place, 2 for 100's place and 3 for 1000's
	//max is the maximum digit (9 in normal case, 5 when conting from 00-59)

	//extract the digit
	int number = (x >> (digit*4)) & 0xf ;
	//increment the digit
	number+=inc;
	//keep it between 0 and max
	if (number<0) number = max;
	if (number>max) number = 0;
	//remomve the old digit
	x &= ~(0xf<<(4*digit));
	//put the new digit in
	x |= number << (4*digit);
	return x;
}


//The acutal main
void main2(){

	int sd=0x00; //second in bcd //bimary coded decimal
	int md=0x01; //minute in bcd
	int hd=0x00; //hour   in bcd
	int s;       //second //normal number
	int m;       //minute
	int h;       //hour
	int day=0x01;   //day   in bcd
	int month=0x06; //month in bcd
	int year=0x2021;  //year  in bcd
	int edit = -1;
	while(true){

		//read the rtc
		if(edit<0){
#ifndef PC
			sd = regRead(RSECCNT);
			md = regRead(RMINCNT);
			hd = regRead(RHRCNT);
			day	= regRead(RDAYCNT);
			month	= regRead(RMONCNT);
			year	= regRead(RYRCNT);
#endif
		}

		//convert bcd to normal numbers:
		s = (sd&0xf) + (((sd&0xf0)>>4)*10);
		m = (md&0xf) + (((md&0xf0)>>4)*10);
		h = (hd&0xf) + (((hd&0xf0)>>4)*10);
	
		//don't draw the clock while in edit mode
		//if(edit<0){
			fillScreen(color(0,0,0));
		
			//Example for line(x1,y1,x2,y2,color);
			//line(100,10,300,500,color(255,0,0));      //Use RGB color
		
			//draw the clock itself
			circle(160,160,150,color(255,255,255));
			//cheat for the minute things:
			//for(int i=0;i<60;i++)  hand(i, 6, color(0,0,0));
			for(int i=0;i<60;i+=5) hand(i,20, color(0,0,0));
			circle(160,160,140,color(128,128,128));
			line(160,10,160,20,color(0,0,0));
			
			//minutes
			hand(m, 6, color(0,0,0));
			//seconds
			hand(s, 2, color(255,0,0));
			//hours
			int h12 = ((h>=12)?(h-12):h)*5; //convert from 24h to 12h and multiply by 5
			int sx=sine(h12);
			sx = (sx>>2) + (sx>>1);
			int cx=cosine(h12);
			cx = (cx>>2) + (cx>>1);
			triangle(160-7,160-7,160+7,160+7,160+sx,160+cx,0,0);
			triangle(160-7,160+7,160+7,160-7,160+sx,160+cx,0,0);
			//the center of the clock
			circle(160,160, 10,color(  0,  0,  0));
		//}
	
		//draw the 7segment display
		square(8,328,304,30,color(100,100,100));
		uint16_t segcolor = color(255,255,255);
		int underlinex[] = {12,24, 42,54, 72,84, 120,132,144,156, 174,186, 204,216};
		//time
		segbyte(12   , 332, 2, 6, hd, segcolor);
		square(12+24,332+4,2,2,segcolor); square(12+24,332+10,2,2,segcolor);
		segbyte(12+30, 332, 2, 6, md, segcolor);
		square(12+54,332+4,2,2,segcolor); square(12+54,332+10,2,2,segcolor);
		segbyte(12+60, 332, 2, 6, sd, segcolor);
		//date
		segbyte4(120, 332, 2, 6, year, segcolor);
		square(120+48,330,2,20,segcolor);
		segbyte(120+54, 332, 2, 6, month, segcolor);
		square(120+78,330,2,20,segcolor);
		segbyte(120+84, 332, 2, 6, day, segcolor);
		//underline when in edit mode
		if(edit>=0)
			square(underlinex[edit],348,8,2,color(255,0,0));
	
		LCD_Refresh();
	
		uint32_t key1, key2;	//First create variables
		getKey(&key1, &key2);	//then read the keys
		if(testKey(key1, key2, KEY_EXE))
			break;
		if(testKey(key1, key2, KEY_CLEAR))
			break;
		if(testKey(key1, key2, KEY_LEFT))
			if(edit>0) edit--;
		if(testKey(key1, key2, KEY_RIGHT))
			if(edit<13) edit++;
		int inc = 0;
		if(testKey(key1, key2, KEY_UP))
			inc= 1;
		if(testKey(key1, key2, KEY_DOWN))
			inc=-1;
		if(inc){ //if up or down was pressed, increment or decrement the specific value
			if(edit==0)
				hd = bcdinc(hd, inc, 1, 2);
			if(edit==1)
				hd = bcdinc(hd, inc, 0, 9);
			if(edit==2)
				md = bcdinc(md, inc, 1, 5);
			if(edit==3)
				md = bcdinc(md, inc, 0, 9);
			if(edit==4)
				sd = bcdinc(sd, inc, 1, 5);
			if(edit==5)
				sd = bcdinc(sd, inc, 0, 9);
			if(edit==6)
				year = bcdinc(year, inc, 3, 9);
			if(edit==7)
				year = bcdinc(year, inc, 2, 9);
			if(edit==8)
				year = bcdinc(year, inc, 1, 9);
			if(edit==9)
				year = bcdinc(year, inc, 0, 9);
			if(edit==10)
				month = bcdinc(month, inc, 1, 1);
			if(edit==11)
				month = bcdinc(month, inc, 0, 9);
			if(edit==12)
				day = bcdinc(day, inc, 1, 3);
			if(edit==13)
				day = bcdinc(day, inc, 0, 9);
		}
		if(testKey(key1, key2, KEY_KEYBOARD)){
			edit=-1;
#ifndef PC
			regClearBit(RTC_START);
			regWrite(RSECCNT, sd);
			regWrite(RMINCNT, md);
			regWrite(RHRCNT,  hd);
			regWrite(RDAYCNT, day);
			regWrite(RMONCNT, month);
			regWrite(RYRCNT,  year);
			regSetBit(RTC_START);
#endif
		}
			inc=-1;
	}

}



void circle(int x, int y, int r, uint16_t c){
	for(int ix=-r; ix<r; ix++){
		for(int iy=-r; iy<r; iy++){
			if((ix*ix)+(iy*iy)<(r*r))
			setPixel(x+ix,y+iy,c);
		}
	}
}
