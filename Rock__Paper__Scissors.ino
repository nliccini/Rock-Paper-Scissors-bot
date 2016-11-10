// Constants, Pins
const int ledPin = 5;
const int rockWinPin = 2; // same pin used to pick rock and to say Arduino wins
const int paperTiePin = 3;
const int scissorsLosePin = 4;

// Vars
int playerScore = 0;
int arduScore = 0;
int randomPin;
int choice;
int result;
int guesses;

// Array Constants and Vars
const int INPUTS = 3;
const int CHOICES = 3;
const int RESULTS = 3;
float EPSILON_MAX = 0.99;
const float EPSILON_MIN = 0.02;
const float EPSILON_DECAY = 0.07;
const float ALPHA = 0.20;
const float GAMMA = 0.80;
const float KAPPA = 10;
float playresults[INPUTS][CHOICES][RESULTS];
float qScores[RESULTS][CHOICES];

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(rockWinPin, INPUT);
  pinMode(paperTiePin, INPUT);
  pinMode(scissorsLosePin, INPUT);
  randomSeed(analogRead(0));
  arrayInitialization();
  Serial.println("Hello, World! Who wants to play Rock Paper Scissors?");
} //end setup() method

void loop() {
  Serial.println("New Round!");
  guesses = floor(EPSILON_MAX / EPSILON_DECAY);
  delay(100);
  Serial.print("Guesses remaining: ");
  Serial.println(guesses);
  
  makeMove();
  delay(500);

  switch(result) {
    case 0:
      arduScore += 1;
      // Arduino wins
      break;
    case 1:
      // Tie
      break;
    case 2:
      playerScore += 1;
      // Player wins
      break;
  }

  Serial.println("Score: ");
  Serial.print("Arduino: ");
  Serial.print(arduScore);
  Serial.print(" to Player: ");
  Serial.println(playerScore);
  delay(500);
  Serial.println(" ");
  digitalWrite(ledPin, LOW);

  for(int k=0; k<KAPPA; k++) {
    qScoreUpdate();
  }
  
} // end loop() method

/*  Create and initialize the arrays for both playresults and qScores
 */
void arrayInitialization() {
  int i, j, k;
  for(i=0; i<INPUTS; i++) {
    for(j=0; j<CHOICES; j++) {
      for(k=0; k<RESULTS; k++) {
        playresults[i][j][k] = (1/INPUTS);
      }
    }
  }
  for(i=0; i<RESULTS; i++) {
    for(j=0; j<CHOICES; j++) {
      qScores[i][j] = 0;
    }
  }
} // end arrayInitialization() method

/* getChoice() waits for you to push any of the buttons and records your
 * response as the var 'choice' as an int of 0, 1, or 2
 */
int getChoice() {
  Serial.println("Waiting for your choice...");
  while(digitalRead(ledPin) == LOW) {
    if(digitalRead(rockWinPin) == HIGH) {
      choice = 0;
      digitalWrite(ledPin, HIGH);
      delay(500);
      break;
    }
    else if(digitalRead(paperTiePin) == HIGH) {
      choice = 1;
      digitalWrite(ledPin, HIGH);
      delay(500);
      break;
    }
    else if(digitalRead(scissorsLosePin) == HIGH) {
      choice = 2;
      digitalWrite(ledPin, HIGH);
      delay(500);
      break;
    }
  }
  Serial.print("Player chose: ");
  switch(choice) {
    case 0:
      Serial.println("Rock");
      break;
    case 1:
      Serial.println("Paper");
      break;
    case 2:
      Serial.println("Scissors");
      break;
  }
  return choice;
} // end getChoice() method

/* getOutcome() gets the result that you tell the Arduino happened. It could 
 * either win, tie with you, or lose to you. This response is recorded in the 
 * var 'result' as an int 0, 1, or 2
 * 
 */
int getOutcome() {
  while(digitalRead(ledPin) == HIGH) {  
    //Serial.print(".");
    if(digitalRead(rockWinPin) == HIGH) {
      result = 0;
      digitalWrite(ledPin, LOW);
      break;
    }
    else if(digitalRead(paperTiePin) == HIGH) {
      result = 1;
      digitalWrite(ledPin, LOW);
      break;
    }
    else if(digitalRead(scissorsLosePin) == HIGH) {
      result = 2;
      digitalWrite(ledPin, LOW);
      break;
    }
  }
  Serial.print("Outcome: ");
  switch(result) {
    case 0:
      Serial.println("Arduino wins, you lose!");
      break;
    case 1:
      Serial.println("Tie!");
      break;
    case 2:
      Serial.println("You win, Arduino loses!");
      break;
  }
  return result;
} // end getOutcome() method

/*  randomValue() will generate a random value between 0.00 and 1.00 as a 
 *  float. This value will be used to determine whether the Arduino will
 *  guess on its turn or use a solution from its virtual database
 */
float randomValue() {
  float x = random(1000) / 100.0;
  x /= 10;
  return x;
} // end randomValue() method

/* This is the body of the program: guess or not, then choose a move based
 * on whether it guessed or used learned information
 */
void makeMove() {
  int i;
  int maxqScore = 0;
  int nextMove = -1;
  int initialInput = getChoice();
  int result;

  //Serial.print("Random or learned move: ");
  //Serial.println(randomValue());

// Generate random value, then compare to EPSILON_MAX to determine whether it will guess or not
// If using learned info, then check to see which counter move contains the highest value in the 
// virtual database, then use that value and update the database. (CHOICES) represents what the 
// Arduino will choose. If the Arduino guesses, then it will pick a random number between 0 and 2
  if(randomValue() > EPSILON_MAX) {
    Serial.println("I think I know this!");
    for(i=0; i<CHOICES; i++) {
      if(qScores[initialInput][i] > maxqScore) {
        maxqScore = qScores[initialInput][i];
        nextMove = i;
      }
    }
  }
  else if(nextMove == -1) {
    Serial.println("I'm gonna guess...");
    float temp = randomValue();
    nextMove = (temp * CHOICES);
  }

// Output in human-readable form what move the Arduino chose
  Serial.print("Arduino chose: ");
  switch(nextMove) {
    case 0:
      Serial.println("Rock");
      break;
    case 1:
      Serial.println("Paper");
      break;
    case 2:
      Serial.println("Scissors");
      break;
  }

// Get the outcome from the player by reading the input buttons
  Serial.println("Did I win?");
  int playerResult = getOutcome();

// Increase the value for this move-set and result by ALPHA, then divide every input 
// by (1 + ALPHA) 
  playresults[initialInput][nextMove][result] += ALPHA;
  for(i=0; i<INPUTS; i++) {
    playresults[initialInput][nextMove][i] /= (1 + ALPHA);
  }

  if(EPSILON_MAX > EPSILON_MIN) {
    EPSILON_MAX -= EPSILON_DECAY;
  }
} // end makeMove() mehtod

void qScoreUpdate() {
  int i, j, k, l;
  float reward = 0;
  float qSum;
  float maxqScore;

  for(i=0; i<INPUTS; i++) {
    for(j=0; j<CHOICES; j++) {
      if((i==0 && j==0) || (i==0 && j==2) || (i==1 && j==0) || (i==1 && j==1) || (i==2 && j==2)) {
        reward = -10000;
      }
      else {
        reward = 1;
      }
      
      qSum = 0;
      
      for(k=0; k<INPUTS; k++) {
        for(l=0; l<CHOICES; l++) {
          if(qScores[k][l] > maxqScore) {
            maxqScore = qScores[k][l];
          }
        }
      qSum += (playresults[i][j][k] * maxqScore);
      }

      qScores[i][j] = reward + (GAMMA * qSum);
    }  
  }
} // end qScoreUpdate() method
