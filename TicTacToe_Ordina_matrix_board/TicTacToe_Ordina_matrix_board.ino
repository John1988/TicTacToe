/*
  Tic Tac Toe example

 */

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
const int rows = 3;
const int cols = 3;
const int leds = 2;
const int Q1 = 2;    //  transistor Q1
const int Q2 = 3;    //  transistor Q2
const int Q3 = 4;    //  transistor Q3
const int sd_pin = 11;    // SD - Shift data pin (shift register input)
const int rclk_pin = 10;  // rclk - Enable output latch (edge triggered)
const int srclk_pin = 12; // srclk - Shift-clock (edge triggered)
int updatespeed;
int RowsCount;
int ColsCount; 
int winning_rows;
int winning_cols;
int winner;
bool game_over;
bool player; //Starting player: 0 = green, 1 = red
String winning_dir;
bool LedMatrix[rows][cols][leds] = {{{0 , 0},  {0 , 0},  {0 , 0}},
                                   {{0 , 0},  {0 , 0},  {0 , 0}},
                                   {{0 , 0},  {0 , 0},  {0 , 0}}};
bool TempLedMatrix[rows][cols][leds];
bool MaskLedMatrix[rows][cols][leds];
int BtnMatrix[rows][cols] =       {{5,  A3,  A0},
                                   {6,  A4,  A1},
                                   {7,  A5,  A2}};

//static const uint8_t key_pins[] = {A0,A3,5,A1,A4,6,A2,A5,7};
//static const uint16_t key_name[] = {"K1","K2","K3","K4","K5","K6","K7","K8","K9"};

void setup() {
  updatespeed = 5; // works between 1-5 ms without noticable flicker
  RowsCount = 0;
  ColsCount = 0; 
  winner = -1;
  winning_rows = 0;
  winning_cols = 0;
  clearstatus();
  clearmaskstatus();
  game_over = 0;
  Serial.begin(9600);
  Serial.println("Tic tac toe started - Fasten your seatbelts!!");
  Serial.println("");  
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(2, OUTPUT); // tr1
  pinMode(3, OUTPUT); // tr2
  pinMode(4, OUTPUT); // tr3
  
  pinMode(10, OUTPUT);// rclk
  pinMode(11, OUTPUT);// srclk
  pinMode(12, OUTPUT);// sd

  pinMode(A0, INPUT);// K1
  pinMode(A1, INPUT);// K4
  pinMode(A2, INPUT);// K3
  
  pinMode(A3, INPUT);// K4
  pinMode(A4, INPUT);// K5
  pinMode(A5, INPUT);// K6

  pinMode(5, INPUT);// K7
  pinMode(6, INPUT);// K8
  pinMode(7, INPUT);// K9
}

void loop() {
  // print the serial string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  updateUI();
  checkresult();
}

void updateUI(){ //update display and buttons
  for(int i = 0; i < 3; i++){       //Row
    for(int j = 0; j < 3; j++){     //Column
      for (int m = 0; m < 2; m++){  //LED colour
        if (LedMatrix[i][j][m] == 0 and digitalRead(BtnMatrix[i][j]) and player == m and game_over == 0){
          if (player == 0 and LedMatrix[i][j][1] == 0){
            player = 1;
            LedMatrix[i][j][m] = 1;
          }
          else if (player == 1 and LedMatrix[i][j][0] == 0){
            player = 0;
            LedMatrix[i][j][m] = 1;
          }
        }
        digitalWrite(sd_pin, LedMatrix[i][j][m]); // Serial Data (SD)
        srclk();
      }
    }
    rclk(); // Enable output latch 
    tr(i); // Enable transistor column
    delay(updatespeed); // works between 1-5 ms without noticable flicker
    tr("none"); // Disable transistor column while updating
  }
}

void checkresult(){
  //check result
  for (int m = 0; m < 2; m++){  //LED colour
    for(int j = 0; j < 3; j++){     //Column
      ColsCount = 0;
      for(int i = 0; i < 3; i++){       //Row
        if (LedMatrix[i][j][m] == 1){
          ColsCount++;
        }
        if (ColsCount == 3 ){
          winner = m;
          winning_cols = j;
          winning_dir  = "column";
        }    
      }
    }
    for(int i = 0; i < 3; i++){       //Row
      RowsCount = 0;
      for(int j = 0; j < 3; j++){     //Column
        if (LedMatrix[i][j][m] == 1){
          RowsCount++;
        }
        if (RowsCount == 3 ){
          winner = m;
          winning_rows = i;
          winning_dir  = "row";
        } 
      }
    }
    RowsCount = ColsCount = 0;
    for(int d = 0; d < 3; d++){     //Column  
      if (LedMatrix[d][d][m] == 1){
        RowsCount++;
      }
      if (LedMatrix[d][2-d][m] == 1){
        ColsCount++;
      }
      if (RowsCount == 3){
        winner = m;
        winning_rows = d;
        winning_dir  = "diagonal";       
      }
      if (ColsCount == 3){
        winner = m;
        winning_cols = d;
        winning_dir  = "diagonal";
      }
    }
    int LEDcount = 0;
    for(int j = 0; j < 3; j++){     //Column
      for(int i = 0; i < 3; i++){       //Row
        if (LedMatrix[i][j][0] == 1 or LedMatrix[i][j][1] == 1){
          LEDcount++;
        }
        if (LEDcount == 9 ){
          game_over = 1;
        }          
      }
    }
  }

  if (game_over or winner != -1){
    Serial.println("GAME OVER!");
    if (winner != -1){
      Serial.print(" - Player");
      game_over = 1;
      if (winner == 0){
        Serial.print(" green ");
      }
      else if (winner == 1){
        Serial.print(" red ");
      }
      Serial.print("won!");
      Serial.print(" The winning direction was a ");
      Serial.print(winning_dir);
      if (winning_dir == "row"){
        Serial.print(" at line ");   
        Serial.println(winning_rows);
        MaskLedMatrix[winning_rows][0][winner] = MaskLedMatrix[winning_rows][1][winner] = MaskLedMatrix[winning_rows][2][winner] = 1;
      }
      else if (winning_dir == "column"){
        Serial.print(" at line ");
        Serial.print(winning_cols);
        Serial.println(")");
        MaskLedMatrix[0][winning_cols][winner] = MaskLedMatrix[1][winning_cols][winner] = MaskLedMatrix[2][winning_cols][winner] = 1;
      }
      else if (winning_dir == "diagonal"){
        Serial.println(" ");
        if (winning_cols == 2){
          MaskLedMatrix[0][winning_cols][winner] = MaskLedMatrix[1][1][winner] = MaskLedMatrix[2][winning_cols-2][winner] = 1;  
        }
        else if (winning_rows == 2){
          MaskLedMatrix[0][winning_rows-2][winner] = MaskLedMatrix[1][1][winner] = MaskLedMatrix[2][winning_rows][winner] = 1;
        }      
      }
    }
    else{
      Serial.println("- It's a tie!");
    }
    ShowWinner(); // Becomes dimm when tie or highlights winner
    BlinkLed();   // Fade / blinking effect
    Shift();      // Clean screen and shift all colours (LED selftest)
    Serial.print(" - Next turn the opposite player starts!");
    Serial.print(" (Player: ");    
    if (player == 0){
      Serial.println("green)");
    }
    else if (player == 1){
      Serial.println("red)");
    }
    Serial.println("");
    Serial.println("");
    delay(500); // requird for serial data to finish
    setup();
  }
}

void tr(int tr){
  digitalWrite(Q1, LOW);
  digitalWrite(Q2, LOW); 
  digitalWrite(Q3, LOW);
  switch (tr) {
      case 0: 
        digitalWrite(Q1, HIGH);
        break;
      case 1: 
        digitalWrite(Q2, HIGH);
        break;
      case 2: 
        digitalWrite(Q3, HIGH);
        break;
      case 3:
        digitalWrite(Q1, HIGH);
        digitalWrite(Q2, HIGH); 
        digitalWrite(Q3, HIGH);
       break;
      default:
        //Type "none"
        break;
  }
}

void srclk(){
  digitalWrite(srclk_pin, HIGH); //= "srclk"
  digitalWrite(srclk_pin, LOW); //= "srclk"
}

void rclk(){ 
  digitalWrite(rclk_pin, HIGH); // = "rclk"
  digitalWrite(rclk_pin, LOW); // = "rclk"     
}

void clearstatus(){ //for clearing the LedMatrix
  for(int i = 0; i < 3; i++){       //Row
    for(int j = 0; j < 3; j++){     //Column
      for (int m = 0; m < 2; m++){  //LED colour
        LedMatrix[i][j][m] = 0;
      }  
    }
  }
}

void clearmaskstatus(){ //for clearing MaskLedMatrix (required for restart purposes)
  for(int i = 0; i < 3; i++){       //Row
    for(int j = 0; j < 3; j++){     //Column
      for (int m = 0; m < 2; m++){  //LED colour
        MaskLedMatrix[i][j][m] = 0;
      }  
    }
  }
}

void ShowWinner(){
  memcpy(TempLedMatrix, LedMatrix, sizeof(LedMatrix)); //backup
  int tempupdatespeed = updatespeed; //backup
  int state = 1;
  for(int o = 1; o < 80; o++){
    if (o % 10 == 0){
      state = !state;
    }  
    clearstatus();  
    for(int i = 0; i < 3; i++){       //Row
      for(int j = 0; j < 3; j++){     //Column
        for (int m = 0; m < 2; m++){  //LED colour
          if (MaskLedMatrix[i][j][m] == 1){
            LedMatrix[i][j][m] = state;   
          }
        }
      }  
    }
    updatespeed = 6;
    updateUI();
    memcpy(LedMatrix, TempLedMatrix, sizeof(LedMatrix)); //restore
    updatespeed = 1;
    updateUI();
  }
  updatespeed = tempupdatespeed; //restore
}

void BlinkLed(){
  for(int i = 1; i < 50; i++){ //fade on
    tr("none");
    delay(50/i);
    updateUI();
  }
  for(int i = 1; i < 100; i++){ //wave
    updatespeed = 100/i;
    updateUI();
    delay(i/100);
  }
  for(int i = 50; i < 0; i--){ // fade off
    updatespeed = 5;
    delay(50/i);
    updateUI();
  }
}

void Shift(){ // Clean screen and shift all colours (LED selftest)
  clearstatus();
  updateUI();
  digitalWrite(sd_pin, HIGH);// Serial Data (SD)
  for (int m = 0; m < 17; m++){
    updatespeed--;
    srclk();
    rclk(); // Enable output latch 
    tr(3); // Enable transistor column
    delay(100);
    if (m == 8){
      digitalWrite(sd_pin, LOW);// Serial Data (SD)
    }
  }
}

void serialEvent() {
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
    //Selftesting feature (using terminal)
    if (inputString == "c") {
      Serial.println("column test");
      LedMatrix[0][1][1] = LedMatrix[1][1][1] = LedMatrix[2][1][1] = 1;
    }
    if (inputString == "r") {
      Serial.println("row test");
      LedMatrix[1][0][1] = LedMatrix[1][1][1] = LedMatrix[1][2][1] = 1;
    }    
  }
}
