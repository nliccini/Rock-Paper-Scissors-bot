// constants and vars for choices, leds, switches, and results (result)
const int ledPin = 13;
const int piezoPin = 8;
const int rockPin = 2;
const int paperPin = 3;
const int scissorsPin = 4;
const int rockChoicePin = 5;
const int paperChoicePin = 6;
const int scissorsChoicePin = 7;
const int winPin = 8;
const int losePin = 9;
const int tiePin = 10;
int playerScore = 0;
int arduScore = 0;
int randomPin;
int choice;  // 0 means rock, 1 means paper, 2 means scissors
int result;  // 0 means arduino wins, 1 means you win, 2 means tie
// array vars inputs (your choice), choices (Arduino's choice), and result (did Arduino win, lose, or tie?)
const int inputs = 3;  // Your rock, paper, scissors
const int choices = 3; // Arduino's rock, paper, scissors
const int results = 3; // win, loss, tie for arduino
// Epsilon vars are the probability that the Arduino will make a random move or look up values from the arrays
float epsilonMax = 0.99; // if the random number is greater than this var, then Arduino will look up a move
const float epsilonMin = 0.02; // this is the min value for epsilon to decrease to
const float epsilonDecay = 0.07; // this is how much epsilon decreases each round so as to increase chances of looking up a move
// Other vars used to increase efficiency and learning
const float alpha = 0.20;
const float gamma = 0.80;
const float kappa = 10;
// arrays
float playresults[inputs][choices][results];
float qScores[inputs][choices];



void setup() {
  Serial.begin(9600);         // You need the Serial port open to play the game and see info
  pinMode(rockPin, OUTPUT);
  pinMode(paperPin, OUTPUT);
  pinMode(scissorsPin, OUTPUT);
  pinMode(rockChoicePin, INPUT);
  pinMode(paperChoicePin, INPUT);
  pinMode(scissorsChoicePin, INPUT);
  randomSeed(analogRead(0));
  arrayInitialization();
  } // end setup() method


void loop() {
  Serial.println("New Round!");

  delay(100);
  
  Serial.print("Value left for guessing: ");
  Serial.println(epsilonMax);

  makeMove();

  delay(500);

// code to make Arduino pick a random choice every time
  /* randomPin = random(2,5);  
  
  digitalWrite(randomPin, HIGH);
  Serial.print("Arduino chose: ");
  if (randomPin == 2) {
    Serial.println("Rock");
  } else if (randomPin == 3) {
    Serial.println("Paper");
  } else if (randomPin == 4) {
    Serial.println("Scissors");
  }
  delay(100); 

  Serial.println("What's the result?"); */
  
// code to make Arduino always win not matter what
  /* if (choice == 0) {           
    digitalWrite(rockPin, LOW);
    digitalWrite(paperPin, HIGH);
    digitalWrite(scissorsPin, LOW);
  } else if (choice == 1) {
    digitalWrite(rockPin, LOW);
    digitalWrite(paperPin, LOW);
    digitalWrite(scissorsPin, HIGH);
  } else if (choice == 2) {
    digitalWrite(rockPin, HIGH);
    digitalWrite(paperPin, LOW);
    digitalWrite(scissorsPin, LOW);
  } */

// code to make Arduino auto-calculate results and scores
  /* if (choice == 0 && digitalRead(paperPin) == HIGH) {     
    arduScore += 1;
    Serial.println("Arduino won.");
  } else if (choice == 1 && digitalRead(scissorsPin) == HIGH) {
    arduScore += 1;
    Serial.println("Arduino won.");
  } else if (choice == 2 && digitalRead(rockPin) == HIGH) {
    arduScore += 1;
    Serial.println("Arduino won.");
  } else if (choice == 0 && digitalRead(scissorsPin) == HIGH) {
    playerScore += 1;
    Serial.println("You won.");
  } else if (choice == 1 && digitalRead(rockPin) == HIGH) {
    playerScore += 1;
    Serial.println("You won.");
  } else if (choice == 2 && digitalRead(paperPin) == HIGH) {
    playerScore += 1;
    Serial.println("You won.");
  } else if (choice == 0 && digitalRead(rockPin) == HIGH) {
    Serial.println("Tie.");
  } else if (choice == 1 && digitalRead(paperPin) == HIGH) {
    Serial.println("Tie.");
  } else if (choice == 2 && digitalRead(scissorsPin) == HIGH) {
    Serial.println("Tie.");
  } */

  if (result == 1) {
    playerScore += 1;
  } else if (result == 0) {
    arduScore += 1;
  } else if (result == 2) {
    //
    } 

  Serial.print("Score: ");
  Serial.print("Ardunio: ");
  Serial.print(arduScore);
  Serial.print(" to ");
  Serial.print("Player: ");
  Serial.println(playerScore);

  delay(750);

// code to make each game end if someone wins 3 rounds (can change amount)
  /* if (arduScore == 3) {
    Serial.println("Arudino wins this game!");
    arduScore = 0;
    playerScore = 0;
  } else if (playerScore == 3) {
    Serial.println("You win this game!");
    arduScore = 0;
    playerScore = 0;
  } */

  Serial.println(" ");
  
  //randomPin = 0;  // uncomment this when you want to make arduino pick random choices every round
// turns off all LEDs to indicate new round
  digitalWrite(rockPin, LOW);
  digitalWrite(paperPin, LOW);
  digitalWrite(scissorsPin, LOW); 
  
  for (int k=0; k<kappa; k++) {
    qScoreUpdate();
  }

} // end loop() method

// all methods will be defined below so as to simplify the loop() method above
void arrayInitialization() {
  int i,j,k;
  for(i=0; i<inputs; i++) {
    for(j=0; j<choices; j++) {
      for(k=0; k < results; k++) {
        playresults[i][j][k] = (1 / inputs);
      }
    }
  }
  for(i=0; i<results; i++) {
    for(j=0; j<choices; j++) {
      qScores[i][j] = 0;
    }
  }  
} // end arrayInitialization() method
  
int getChoice() {
  while (digitalRead(rockPin) == LOW && digitalRead(paperPin) == LOW && digitalRead(scissorsPin) == LOW) {                        // code to manually decide winner and results
    if (digitalRead(rockChoicePin) == HIGH) {
      choice = 0;
      break; // rock
    } else if (digitalRead(paperChoicePin) == HIGH) {
      choice = 1; // paper
      break; 
    } else if (digitalRead(scissorsChoicePin) == HIGH) {
      choice = 2; // scissors
      break;
    }
  } 
  Serial.print("You chose: ");
  if (choice == 0) {
    Serial.println("Rock");
  } else if (choice == 1) {
    Serial.println("Paper");
  } else if (choice == 2) {
    Serial.println("Scissors");
  } 
  return choice;
} // end getChoice() method

int getResult() {
  while (digitalRead(rockPin) == HIGH || digitalRead(paperPin) == HIGH || digitalRead(scissorsPin) == HIGH) {                        // code to manually decide winner and results
    if (digitalRead(winPin) == HIGH) {
      result = 0;
      Serial.println("Arduino wins, you lose");
      break;
    } else if (digitalRead(losePin) == HIGH) {
      result = 1; // loss
      Serial.println("You win, Arduino loses");
      break;
    } else if (digitalRead(tiePin) == HIGH) {
      result = 2;
      Serial.println("Tie.");
      break;
    }
  } 
  //Serial.println(result);
  return result;
} // end getResult() method

float randomValue() {
  float x = random(1000) / 100.0;
  x /= 10;
  return x;
} // end randomValue() method

void makeMove() {   
  int i;
  int maxQScore = 0;
  int nextMovement = -1;
  int initialInput = getChoice();
  int result;

  Serial.print("Random or learned move: ");
  Serial.println(randomValue());

  if(randomValue() > epsilonMax) {
    Serial.println("I think I know this!");
    for (i=0; i<choices; i++) {
      if (qScores[initialInput][i] > maxQScore) {
        maxQScore = qScores[initialInput][i];
        nextMovement = i;
      }
    }
  } else if(nextMovement == -1) {
    Serial.println("I'm guessing!");
    float temp = randomValue();
    nextMovement = (temp * choices);
  }

  Serial.print("Arduino chose: ");

  switch(nextMovement) {
    case 0:
      digitalWrite(2, HIGH);
      Serial.println("Rock");
      break;
    case 1:
      digitalWrite(3, HIGH);
      Serial.println("Paper");
      break;
    case 2:
      digitalWrite(4, HIGH);
      Serial.println("Scissors");
  }

  Serial.println("Did I win?");

  int playerResult = getResult();

  playresults[initialInput][nextMovement][result] += alpha;

  for (i=0; i<inputs; i++) {
    playresults[initialInput][nextMovement][i] /= (1 + alpha);
  }

  if(epsilonMax > epsilonMin) {
    epsilonMax -= epsilonDecay;
  }

} // end makeMove() method

void qScoreUpdate() {
  int i, j, k, l;

  float reward = 0;

  float qSum;
  float maxQScore;

  for (i=0; i<inputs; i++) {
    for (j =0; j<choices; j++) {
      if((i==0 && j==0) || (i==0 && j==2) || (i==1 && j==0) || (i==1 && j==1) || (i==2 && j==2)) {
        reward = -10000;
      } else {
        reward = 1;
      }

  qSum = 0;

  for(k=0; k<inputs; k++) {
    maxQScore = 0;
    for (l=0; l<choices; l++) {
      if(qScores[k][l] > maxQScore) {
        maxQScore = qScores[k][l];
      }
    }

  qSum += (playresults[i][j][k] * maxQScore);
    
  }

  qScores[i][j] = reward + (gamma * qSum);
      
    }
  }
} // end qScoreUpdate() method

