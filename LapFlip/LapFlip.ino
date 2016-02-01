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

volatile long lastDebounceTime = 0;   // the last time the interrupt was triggered
long debounceDelay = 80;    // the debounce time; decrease if quick button presses are ignored

//variable to hold reset lap
int lapReset = 0;


void setup() {
	ADCSRA = 0; //disable ADC (analog to digital converter)

	Serial.begin(115200);
	Serial.println("Starting..");

	//initialize outputs
	for (int x = onesPin; x < onesPin + 7; x++) {
		pinMode(x, OUTPUT);
	}
	for (int x = tensPin; x < tensPin + 7; x++) {
		pinMode(x, OUTPUT);
	}
	pinMode(18, INPUT_PULLUP);
	pinMode(19, INPUT_PULLUP);
	pinMode(20, INPUT_PULLUP);
	pinMode(21, INPUT_PULLUP);

}

void loop() {
	long currentTime = millis();
	if ((currentTime - lastDebounceTime) > debounceDelay) {
		lastDebounceTime = currentTime;
		switch (state) {
		case 0:
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
			displayDigit(tensPin, fiGetDigit(lap, 0));
			displayDigit(onesPin, fiGetDigit(lap, 1));
			Serial.print("Display Lap: ");
			Serial.print(fiGetDigit(lap, 1));
			Serial.println(fiGetDigit(lap, 0));
		}

		if (lap < 10 && lap > 0) {
			displayDigit(tensPin, -1);
			displayDigit(onesPin, fiGetDigit(0, lap));
			Serial.print("Display Lap: ");
			Serial.println(fiGetDigit(lap, 0));
		}

		if (lap < 1) {
			displayDigit(tensPin, -1);
			displayDigit(onesPin, -1);
			Serial.print("Display Lap: ");
			Serial.println("");
		}

	}
	sleepNow();
}

void displayDigit(int dpin, int n)  //dpin is starting pin for 7 segment ie "A", n is the number to display
{
	switch (n) {
	case 0:
		digitalWrite(dpin + 0, HIGH);
		digitalWrite(dpin + 1, HIGH);
		digitalWrite(dpin + 2, HIGH);
		digitalWrite(dpin + 3, HIGH);
		digitalWrite(dpin + 4, HIGH);
		digitalWrite(dpin + 5, HIGH);
		digitalWrite(dpin + 6, LOW);
		break;
	case 1:
		digitalWrite(dpin + 0, LOW);
		digitalWrite(dpin + 1, HIGH);
		digitalWrite(dpin + 2, HIGH);
		digitalWrite(dpin + 3, LOW);
		digitalWrite(dpin + 4, LOW);
		digitalWrite(dpin + 5, LOW);
		digitalWrite(dpin + 6, LOW);
		break;
	case 2:
		digitalWrite(dpin + 0, HIGH);
		digitalWrite(dpin + 1, HIGH);
		digitalWrite(dpin + 2, LOW);
		digitalWrite(dpin + 3, HIGH);
		digitalWrite(dpin + 4, HIGH);
		digitalWrite(dpin + 5, LOW);
		digitalWrite(dpin + 6, HIGH);
		break;
	case 3:
		digitalWrite(dpin + 0, HIGH);
		digitalWrite(dpin + 1, HIGH);
		digitalWrite(dpin + 2, HIGH);
		digitalWrite(dpin + 3, HIGH);
		digitalWrite(dpin + 4, LOW);
		digitalWrite(dpin + 5, LOW);
		digitalWrite(dpin + 6, HIGH);
		break;
	case 4:
		digitalWrite(dpin + 0, LOW);
		digitalWrite(dpin + 1, HIGH);
		digitalWrite(dpin + 2, HIGH);
		digitalWrite(dpin + 3, LOW);
		digitalWrite(dpin + 4, LOW);
		digitalWrite(dpin + 5, HIGH);
		digitalWrite(dpin + 6, HIGH);
		break;
	case 5:
		digitalWrite(dpin + 0, HIGH);
		digitalWrite(dpin + 1, LOW);
		digitalWrite(dpin + 2, HIGH);
		digitalWrite(dpin + 3, HIGH);
		digitalWrite(dpin + 4, LOW);
		digitalWrite(dpin + 5, HIGH);
		digitalWrite(dpin + 6, HIGH);
		break;
	case 6:
		digitalWrite(dpin + 0, HIGH);
		digitalWrite(dpin + 1, LOW);
		digitalWrite(dpin + 2, HIGH);
		digitalWrite(dpin + 3, HIGH);
		digitalWrite(dpin + 4, HIGH);
		digitalWrite(dpin + 5, HIGH);
		digitalWrite(dpin + 6, HIGH);
		break;
	case 7:
		digitalWrite(dpin + 0, HIGH);
		digitalWrite(dpin + 1, HIGH);
		digitalWrite(dpin + 2, HIGH);
		digitalWrite(dpin + 3, LOW);
		digitalWrite(dpin + 4, LOW);
		digitalWrite(dpin + 5, LOW);
		digitalWrite(dpin + 6, LOW);
		break;
	case 8:
		digitalWrite(dpin + 0, HIGH);
		digitalWrite(dpin + 1, HIGH);
		digitalWrite(dpin + 2, HIGH);
		digitalWrite(dpin + 3, HIGH);
		digitalWrite(dpin + 4, HIGH);
		digitalWrite(dpin + 5, HIGH);
		digitalWrite(dpin + 6, HIGH);
		break;
	case 9:
		digitalWrite(dpin + 0, HIGH);
		digitalWrite(dpin + 1, HIGH);
		digitalWrite(dpin + 2, HIGH);
		digitalWrite(dpin + 3, HIGH);
		digitalWrite(dpin + 4, LOW);
		digitalWrite(dpin + 5, HIGH);
		digitalWrite(dpin + 6, HIGH);
		break;
	default:
		digitalWrite(dpin + 0, LOW);
		digitalWrite(dpin + 1, LOW);
		digitalWrite(dpin + 2, LOW);
		digitalWrite(dpin + 3, LOW);
		digitalWrite(dpin + 4, LOW);
		digitalWrite(dpin + 5, LOW);
		digitalWrite(dpin + 6, LOW);
		break;
	}
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
