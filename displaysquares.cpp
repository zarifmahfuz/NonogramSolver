/*acknowledgement:copied code provided in class to setup tft display*/


// core graphics library (written by Adafruit)
#include <Adafruit_GFX.h>

// Hardware-specific graphics library for MCU Friend 3.5" TFT LCD shield
#include <MCUFRIEND_kbv.h>

MCUFRIEND_kbv tft;

#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320

//currently hardcoded puzzle size
int puzzle_size = 20;

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

void build() {
	int square_size = DISPLAY_HEIGHT/puzzle_size; //integer division
	//for centering the puzzle
	int squareY = (320 - puzzle_size*square_size)/2;
	//save start position of X as it is required later on again
	int startX = (480 - puzzle_size*square_size)/2;

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

int main() {
	setup();
	for(int i = 0; i < puzzle_size; i++){
		for(int j = 0; j < puzzle_size; j++){
			squares[j][i] = 0;
		}
	}

	build();

	return 0;

}