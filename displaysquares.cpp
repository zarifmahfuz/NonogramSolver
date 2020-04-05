/*acknowledgement:copied code provided in class to setup tft display*/


// core graphics library (written by Adafruit)
#include <Adafruit_GFX.h>

// Hardware-specific graphics library for MCU Friend 3.5" TFT LCD shield
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
// touch screen pins, obtained from the documentaion
#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000


MCUFRIEND_kbv tft;

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320

//currently hardcoded puzzle size
int puzzle_size = 20;
int square_size;
//create an array to hold fillmode of each square
// X coordinate first, then Y
int8_t squares[20][20];


void setup() {
  init();

  Serial.begin(9600);

  uint16_t ID = tft.readID();    
  Serial.print("ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9481; 
  tft.begin(ID);                 
  tft.setRotation(1);
  tft.fillScreen(TFT_BLUE);
}

//touching outside grid clears the puzzle
void emptyGrid();

void build() {
	square_size = DISPLAY_HEIGHT/puzzle_size; //integer division
	//for centering the puzzle
	int squareY = (320 - puzzle_size*square_size)/2;
	//save start position of X as it is required later on again
	int startX = (480 - puzzle_size*square_size)/2;

	//iterate through each column and row drawing squares
	for(int i = 0; i < puzzle_size; i++){
		int squareX = startX;
		for(int j = 0; j < puzzle_size; j++){
			if(squares[j][i] == 1){
				tft.fillRect(squareX, squareY, square_size, square_size, TFT_BLACK);
			}
			else{
				tft.fillRect(squareX, squareY, square_size, square_size, TFT_WHITE);
			}
			tft.drawRect(squareX, squareY, square_size, square_size, TFT_BLACK);
			squareX += square_size;
			
		}
		squareY += square_size;
	}
}

void processTouch() {
	//get touch coordinates
	TSPoint touch = ts.getPoint();
	pinMode(YP, OUTPUT); 
	pinMode(XM, OUTPUT); 
	if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE) {
		return;
	}
	//mapping touch to screen
	int16_t screen_x = map(touch.y, TS_MINX, TS_MAXX, DISPLAY_WIDTH-1, 0);
	int16_t screen_y = map(touch.x, TS_MINY, TS_MAXY, DISPLAY_HEIGHT-1, 0);

	//calculates the bounds of the grid
	int boundY = (320 - puzzle_size*square_size)/2;
	int boundX = (480 - puzzle_size*square_size)/2;

	//convert the touch into square number in x,y form
	int touch_squareX = (screen_x-boundX)/square_size;
	int touch_squareY = (screen_y-boundY)/square_size;
	
	//if touch is outside grid, clear the puzzle
	if((screen_x < boundX)||(screen_x > 480 - boundX)){
		emptyGrid();
		return;
	}

	if((screen_y < boundY)||(screen_y > 320 - boundY)){
		emptyGrid();
		return;
	}

	//alternate the color of the square
	if(squares[touch_squareX][touch_squareY] == 0){
		squares[touch_squareX][touch_squareY] = 1;
		//proper positioning needs to be calculated before drawing a rectangle
		tft.fillRect(boundX + touch_squareX * square_size,boundY + touch_squareY * square_size, square_size, square_size, TFT_BLACK);
		tft.drawRect(boundX +touch_squareX * square_size,boundY + touch_squareY * square_size, square_size, square_size, TFT_BLACK);
	} 
	else{
		squares[touch_squareX][touch_squareY] = 0;
		//proper positioning needs to be calculated before drawing a rectangle
		tft.fillRect(boundX +touch_squareX * square_size,boundY + touch_squareY * square_size, square_size, square_size, TFT_WHITE);
		tft.drawRect(boundX +touch_squareX * square_size,boundY + touch_squareY * square_size, square_size, square_size, TFT_BLACK);
	}
	delay(250);
}

void emptyGrid(){
	//initialize an empty grid by iterating through the array
	for(int i = 0; i < puzzle_size; i++){
		for(int j = 0; j < puzzle_size; j++){
			squares[j][i] = 0;
		}
	}
	//draw the grid
	build();
}


int main() {
	setup();
	emptyGrid();
	
	while(true){
		processTouch();
	}
	return 0;

}