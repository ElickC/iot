// vars
#define BUTTON 4
#define LED 5
bool buttonstate;
bool clickedflag = false;

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
  buttonstate = digitalRead(BUTTON);

  // if the button is clicked and it was not clicked previously
  if (buttonstate and not clickedflag){

      // turn the pin on 
      digitalWrite(LED, HIGH);
      
      // set the clicked flag
      clickedflag = true;
  }

  // if the button is not clicked now and it was clicked previously (a commplete click has now happened)
  if (not buttonstate and clickedflag){

    // turn the pin off 
      digitalWrite(LED, LOW);

    // set the clicked flag
    clickedflag = false;

    // print
    Serial.println("clicked");
  }
  
}
