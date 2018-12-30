const byte clkPin = 5;
const byte dataPin = 2;
const int shift = 1;
boolean adcStarted;
volatile int adcReading;
volatile boolean adcDone;
volatile boolean clk;
volatile int wordcount = 0;
volatile byte worddata = B00000001;
char letter;




void ADC_Init() {
  ADCSRA = bit(ADEN);     //turn ADC on
  ADCSRA |= bit (ADPS0) |  bit (ADPS1) | bit (ADPS2);  // Prescaler of 128
  ADMUX  =  bit (REFS0) | bit (REFS1);    // internal 1.1v reference
}

void ADC_start_conversion(int adc_pin) {
  ADMUX &= ~(0x07) ; //clearing enabled channels
  ADMUX  |= (adc_pin & 0x07) ;    // select input port
  bitSet (ADCSRA, ADSC) ;  //start ADC read
}

int ADC_read_conversion() {
  while (bit_is_set(ADCSRA, ADSC)); //checking if pin is reading mode
  return ADC ;
}

void getData() {
  ADC_start_conversion(dataPin);
  int dataValue = ADC_read_conversion();
  //Serial.println(dataValue);
  if (dataValue > 500) {
    worddata <<= shift; //left shift once
    worddata |= B00000001; //put 1 at LSB

    ADC_start_conversion(clkPin); //start back the clock reading
  }
  else {
    worddata <<= shift; //left shift once
    ADC_start_conversion(clkPin); //start back the clock reading
  }
}

void setup ()
{
  Serial.begin (115200);
  ADC_Init();
  ADC_start_conversion(clkPin);
  Serial.write("Ready To Recieve...\n");

}  // end of setup


// ADC complete ISR
ISR (ADC_vect) //dont think we need this can replace the interupt with readconversion method
{
  byte low, high;

  // we have to read ADCL first; doing so locks both ADCL
  // and ADCH until ADCH is read.  reading ADCL second would
  // cause the results of each conversion to be discarded,
  // as ADCL and ADCH would be locked when it completed.
  low = ADCL;
  high = ADCH;

  adcReading = (high << 8) | low;
  adcDone = true;
}  // end of ADC_vect

void loop () {
  if (adcDone) //ADC complete interrupt keeps reading the value if its done then we check if the reading is high or low
  {
    //Serial.println(adcReading);
    adcStarted = false; //stop reading
    if (adcReading > 300 && clk == false) { //clock HIGH reading is done here
      //Serial.println("clk 1");
      getData();
      clk = true;
      wordcount++;
      //Serial.println(worddata, BIN);
    }
    if (adcReading < 300 && clk == true) { //clock LOW reading is done here
      //Serial.println("clk 0");
      getData();
      clk = false;
      wordcount++;
      //Serial.println(worddata, BIN);
    }
    // print the reading after 8 iterations (1 byte at a time)
    if (wordcount == 8) {
      letter = worddata;
      Serial.write(letter);
      wordcount = 0;
    }

    adcDone = false;
  }

  // if we aren't taking a reading, start a new one
  if (!adcStarted)
  {
    adcStarted = true;
    // start the conversion
    ADCSRA |= bit (ADSC) | bit (ADIE);
  }
}  // end of loop
