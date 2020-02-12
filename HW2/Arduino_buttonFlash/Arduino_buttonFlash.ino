// vars
#define BUTTON 4
#define LED 5
bool buttonclick;
bool clickedflag = false;
bool on = false;


void setup() {
  // put your setup code here, to run once:

  // setup serial connunication speed
  Serial.begin(9600);

  // setup pin as input
  pinMode(BUTTON, INPUT);

  // setup pin as output
  pinMode(LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  // read button state
  buttonclick = digitalRead(BUTTON);

  // if the button is clicked and it was not clicked previously
  if (buttonclick and not clickedflag and not on){

      // turn the pin on 
      digitalWrite(LED, HIGH);
      
      // set the clicked flag
      clickedflag = true;

      // set light on bool
      on = true;

      //sleep to wait for click to be done
      delay(1000);
  }

  else if (buttonclick and not clickedflag and on){
      // turn the pin off 
      digitalWrite(LED, LOW);

      // set light off bool
      on = false;
      
      //sleep to wait for click to be done
      delay(1000);
  }

  // if the button is not clicked now and it was clicked previously (a commplete click has now happened)
  if (not buttonclick and clickedflag){ 

    // set the clicked flag
    clickedflag = false;
  }
  
}
