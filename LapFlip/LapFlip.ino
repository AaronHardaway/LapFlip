//This sketch is designed to take the inputs from a four button rf remote and indicate increments and decremnts on two 7-segment flip signs
//to save power the arduino will sleep between button presses.

#include <avr/sleep.h>

//Specifiy starting pins for segment A, the next six are assumed to follow the following order:
//A top, B upper right, C lower right, D bottom, E lower left, F upper left, G middle
//Ones column first
const int onesPin = 31;
//Tens column
const int tensPin = 41;

//specify a variable to hold the condition after wake up:
//0 = nothing to do
//1 = button 1 pressed: increment the display
//2 = button 2 pressed: decrement the display
//3 = button 3 pressed: change the display to reset number
//4 = button 4 pressed: set current lap to new reset number
volatile int state = 0;

//specify variables to hold the current lap number
byte lap = 0;
int lapTens = 0;
int lapOnes = 0;
boolean isReset = false;

//attempts to debounce button presses..
volatile long lastDebounceTime = 0;   // the last time the interrupt was triggered
long debounceDelay = 80;    // the debounce time; decrease if quick button presses are ignored

//variable to hold reset lap
int lapReset = 0;

void setup() {
	Serial.begin(115200);
	Serial.println("Starting..");

	//initialize Ones digit outputs..
	for (int x = onesPin; x < onesPin + 8; x++) {
		pinMode(x, OUTPUT);
	}
	//initialize Tens digit outputs..
	for (int x = tensPin; x < tensPin + 8; x++) {
		pinMode(x, OUTPUT);
	}
	//initialize interrupt inputs..
	for (int x = 18; x < 22; x++){
		pinMode(x, INPUT_PULLUP);
	}
}

void loop() {
	Serial.println("");
	long currentTime = millis();
	if ((currentTime - lastDebounceTime) > debounceDelay) {
		lastDebounceTime = currentTime;
		switch (state) {
		case 0:
			lap = 0;
			break;
		case 1:
			if (isReset) {
				lap++;
				if (lap > 99) {
					lap = 0;
				}
			}
			isReset = true;
			break;
		case 2:
			if (lap > 0) lap--;
			break;
		case 3:
			lap = lapReset;
			break;
		case 4:
			lapReset = lap;
			break;
		}

		Serial.print("Button Lap: ");
		Serial.println(lap);

		if (lap > 9) {
			displayDigit(tensPin, fiGetDigit(lap, 1), onesPin, fiGetDigit(lap, 0));
			Serial.print("Display Lap: ");
			Serial.print(fiGetDigit(lap, 1));
			Serial.println(fiGetDigit(lap, 0));
		}

		if (lap < 10 && lap > 0) {
			displayDigit(tensPin, -1, onesPin, fiGetDigit(0, lap));
			Serial.print("Display Lap: ");
			Serial.println(fiGetDigit(lap, 0));
		}

		if (lap < 1) {
			displayDigit(tensPin, -1, onesPin, -1);
			Serial.print("Display Lap: ");
			Serial.println("");
		}
	}
	sleepNow();
}

//send digits to display, dpin is starting pin for 7 segment ie "A", n1 is the tens digit to display, n2 is the ones digit to display
void displayDigit(int dpin1, int dpin2, int n1, int n2)
{
	switch (n1) {
	case 0:
		displayBlank(dpin1);
		break;
	case 1:
		displayOne(dpin1);
		break;
	case 2:
		displayTwo(dpin1);
		break;
	case 3:
		displayThree(dpin1);
		break;
	case 4:
		displayFour(dpin1);
		break;
	case 5:
		displayFive(dpin1);
		break;
	case 6:
		displaySix(dpin1);
		break;
	case 7:
		displaySeven(dpin1);
		break;
	case 8:
		displayEight(dpin1);
		break;
	case 9:
		displayNine(dpin1);
		break;
	default:
		displayBlank(dpin1);
		break;
	}

	switch (n2) {
	case 0:
		displayZero(dpin2);
		break;
	case 1:
		displayOne(dpin2);
		break;
	case 2:
		displayTwo(dpin2);
		break;
	case 3:
		displayThree(dpin2);
		break;
	case 4:
		displayFour(dpin2);
		break;
	case 5:
		displayFive(dpin2);
		break;
	case 6:
		displaySix(dpin2);
		break;
	case 7:
		displaySeven(dpin2);
		break;
	case 8:
		displayEight(dpin2);
		break;
	case 9:
		displayNine(dpin2);
		break;
	default:
		displayBlank(dpin2);
		break;
	}
	delay(20);
	digitalWrite(dpin1 + 7, HIGH);
	digitalWrite(dpin2 + 7, HIGH);
	delay(200);
	digitalWrite(dpin1 + 7, LOW);
	digitalWrite(dpin2 + 7, LOW);
	delay(20);
	displayBlank(dpin1);
	displayBlank(dpin2);
	delay(20);
}

void sleepNow()
{
	Serial.println("Sleep Start");
	Serial.println("");
	delay(100);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	noInterrupts ();
	sleep_enable();
	attachInterrupt(digitalPinToInterrupt(18), button1, RISING);
	attachInterrupt(digitalPinToInterrupt(19), button2, RISING);
	attachInterrupt(digitalPinToInterrupt(20), button3, RISING);
	attachInterrupt(digitalPinToInterrupt(21), button4, RISING);
	interrupts();
	sleep_cpu();
}

void button1()
{
	sleep_disable();
	noInterrupts();
	detachInterrupt(digitalPinToInterrupt(18));
	state = 1;
}

void button2()
{
	sleep_disable();
	noInterrupts();
	detachInterrupt(digitalPinToInterrupt(19));
	state = 2;
}

void button3()
{
	sleep_disable();
	noInterrupts();
	detachInterrupt(digitalPinToInterrupt(20));
	state = 3;
}

void button4()
{
	sleep_disable();
	noInterrupts();
	detachInterrupt(digitalPinToInterrupt(21));
	state = 4;
}

int fiGetDigit(int n, int k)
{//Get K-th Digit from a Number (zero-based index)
	switch (k)
	{
	case 0:return n % 10;
	case 1:return n / 10 % 10;
	case 2:return n / 100 % 10;
	case 3:return n / 1000 % 10;
	case 4:return n / 10000 % 10;
	case 5:return n / 100000 % 10;
	case 6:return n / 1000000 % 10;
	case 7:return n / 10000000 % 10;
	case 8:return n / 100000000 % 10;
	case 9:return n / 1000000000 % 10;
	}
	return 0;
}

void displayZero(int dpin)
{
	digitalWrite(dpin + 0, HIGH);
	digitalWrite(dpin + 1, HIGH);
	digitalWrite(dpin + 2, HIGH);
	digitalWrite(dpin + 3, HIGH);
	digitalWrite(dpin + 4, HIGH);
	digitalWrite(dpin + 5, HIGH);
	digitalWrite(dpin + 6, LOW);
}

void displayOne(int dpin)
{
	digitalWrite(dpin + 0, LOW);
	digitalWrite(dpin + 1, HIGH);
	digitalWrite(dpin + 2, HIGH);
	digitalWrite(dpin + 3, LOW);
	digitalWrite(dpin + 4, LOW);
	digitalWrite(dpin + 5, LOW);
	digitalWrite(dpin + 6, LOW);
}

void displayTwo(int dpin)
{
	digitalWrite(dpin + 0, HIGH);
	digitalWrite(dpin + 1, HIGH);
	digitalWrite(dpin + 2, LOW);
	digitalWrite(dpin + 3, HIGH);
	digitalWrite(dpin + 4, HIGH);
	digitalWrite(dpin + 5, LOW);
	digitalWrite(dpin + 6, HIGH);
}

void displayThree(int dpin)
{
	digitalWrite(dpin + 0, HIGH);
	digitalWrite(dpin + 1, HIGH);
	digitalWrite(dpin + 2, HIGH);
	digitalWrite(dpin + 3, HIGH);
	digitalWrite(dpin + 4, LOW);
	digitalWrite(dpin + 5, LOW);
	digitalWrite(dpin + 6, HIGH);
}

void displayFour(int dpin)
{
	digitalWrite(dpin + 0, LOW);
	digitalWrite(dpin + 1, HIGH);
	digitalWrite(dpin + 2, HIGH);
	digitalWrite(dpin + 3, LOW);
	digitalWrite(dpin + 4, LOW);
	digitalWrite(dpin + 5, HIGH);
	digitalWrite(dpin + 6, HIGH);
}

void displayFive(int dpin)
{
	digitalWrite(dpin + 0, HIGH);
	digitalWrite(dpin + 1, LOW);
	digitalWrite(dpin + 2, HIGH);
	digitalWrite(dpin + 3, HIGH);
	digitalWrite(dpin + 4, LOW);
	digitalWrite(dpin + 5, HIGH);
	digitalWrite(dpin + 6, HIGH);
}

void displaySix(int dpin)
{
	digitalWrite(dpin + 0, HIGH);
	digitalWrite(dpin + 1, LOW);
	digitalWrite(dpin + 2, HIGH);
	digitalWrite(dpin + 3, HIGH);
	digitalWrite(dpin + 4, HIGH);
	digitalWrite(dpin + 5, HIGH);
	digitalWrite(dpin + 6, HIGH);
}

void displaySeven(int dpin)
{
	digitalWrite(dpin + 0, HIGH);
	digitalWrite(dpin + 1, HIGH);
	digitalWrite(dpin + 2, HIGH);
	digitalWrite(dpin + 3, LOW);
	digitalWrite(dpin + 4, LOW);
	digitalWrite(dpin + 5, LOW);
	digitalWrite(dpin + 6, LOW);
}

void displayEight(int dpin)
{
	digitalWrite(dpin + 0, HIGH);
	digitalWrite(dpin + 1, HIGH);
	digitalWrite(dpin + 2, HIGH);
	digitalWrite(dpin + 3, HIGH);
	digitalWrite(dpin + 4, HIGH);
	digitalWrite(dpin + 5, HIGH);
	digitalWrite(dpin + 6, HIGH);
}

void displayNine(int dpin)
{
	digitalWrite(dpin + 0, HIGH);
	digitalWrite(dpin + 1, HIGH);
	digitalWrite(dpin + 2, HIGH);
	digitalWrite(dpin + 3, HIGH);
	digitalWrite(dpin + 4, LOW);
	digitalWrite(dpin + 5, HIGH);
	digitalWrite(dpin + 6, HIGH);
}

void displayBlank(int dpin)
{
	digitalWrite(dpin + 0, LOW);
	digitalWrite(dpin + 1, LOW);
	digitalWrite(dpin + 2, LOW);
	digitalWrite(dpin + 3, LOW);
	digitalWrite(dpin + 4, LOW);
	digitalWrite(dpin + 5, LOW);
	digitalWrite(dpin + 6, LOW);
}