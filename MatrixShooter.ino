#include <LedControl.h>
#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const byte conPin = 3;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte dataPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;

LedControl lc = LedControl(dataPin, clockPin, loadPin, 1);

const byte downArrowChar[8] = { // 0
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b10101,
	0b01110,
	0b00100
};

const byte downAndUpArrowChar[8] = { // 2
	0b00100,
	0b01110,
	0b10101,
	0b00100,
	0b00100,
	0b10101,
	0b01110,
	0b00100
};

const byte upArrowChar[8] = { // 1
	0b00100,
	0b01110,
	0b10101,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100
};

byte optionMenu = 0;
byte mainMenuLine = 1;
byte settingLine = 1;
byte aboutLine = 1;
const String menus[][6] = {
  {
  "Matrix Shooter",
  "Start",
  "Settings",
  //"Hi-Scores", - de implementat cu eeprom
  "About"
  },

  {
  "Settings", // de implementat cu eeprom
  "Brightness",
  "Contrast",
  "Name",
  "Difficulty",
  "Back"
  },

  {
  "Adrian V.",
  "t.ly/SGC_"
  }
};

const byte mainMenuIndex = 0;
const byte settingsIndex = 1;
const byte aboutIndex = 2;
const byte inSettingIndex = 3;
byte maxIndex = 3;

const String settings[] = {
  " > BRIGHTNESS < ",
  "  > CONTRAST <  ",
  "    > NAME <    ",
  " > DIFFICULTY < ",
  "[EZ]MID HAR INS ",
  " EZ[MID]HAR INS ",
  " EZ MID[HAR]INS ",
  " EZ MID HAR[INS]"
};
byte selectedSetting = 0;

const byte maxBriCon = 16;
byte brightness = 7;
byte contrast = 7;

byte nameIndex = 0;

const String alphanum = "ABCDEFGHIJKLMNOPQRSTUVXYZ1234567890";
byte name[5] = {0, 0, 0, 0, 0};
const byte maxNameIndex = 35;
const byte nameBlink = 125;

byte difficulty = 1; 
// 0 - easy 
// 1 - medium
// 2 - hard
// 3 - insane 
// influences some factors besides the level differences, also a score multiplier
// insane - projectiles will destroy powerups

// const String hiScores[] = {
//   "AAAAA 1234567890",
//   "BBBBB 1134567890",
//   "CCCCC 1114567890",
//   "DDDDD 1111567890",
//   "EEEEE 1111167890"
// };


const String gameInfo[] = {
  "HP:",
  "Lvl:",
  "Pts:",
};

byte maxLives = ceil(5 - difficulty - 0.25 * difficulty);
byte currentLives = maxLives; 
byte level = 1; //  will be kind of impossible from 10 unless you're a pro gamer
unsigned int nextLevelE = 30 - 5 * difficulty + 2 * level; // next level is reached when you kill this many enemies
unsigned int nextLevelT = 60 - 7 * difficulty + 3 * level;  // next level is reached when this much time passes
unsigned long points = 0; 

unsigned long ptsEnemy = 100 * level * difficulty; // how many points are received per enemy killed


const byte fullLife[8] = { // 3
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000
};

const byte emptyLife[8] = { // 4
  B00000,
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B00000
};


// joystick pins
const byte joystickAxisX = A0;
const byte joystickAxisY = A1;

// axis threshold values
const int minThreshold = 400;
const int maxThreshold = 600;

// current axis values;
int axisValueX = 0;
int axisValueY = 0;

bool joystickNeutral = true; // a check if the joystick is in neutral position to allow movement

// pin, current state, debounce time for the joystick button
const int joystickButton = 13;
bool joystickButtonState = false;

unsigned long lastDebounceTime = 0;
const int debounceTime = 250;

unsigned long currentTime = 0;

bool displayUpdated = true;


// MATRIX VARIABLES
const int numberOfLines = 8;
int matrix[numberOfLines][numberOfLines] = {
// 0  1  2  3  4  5  6  7 
  {0, 0, 0, 0, 0, 0, 0, 0}, // 0
  {0, 0, 0, 0, 0, 0, 0, 0}, // 1
  {0, 0, 0, 0, 0, 0, 0, 0}, // 2
  {0, 0, 0, 0, 0, 0, 0, 0}, // 3
  {0, 0, 0, 0, 0, 0, 0, 0}, // 4
  {0, 0, 0, 0, 0, 0, 0, 0}, // 5
  {0, 0, 0, 0, 0, 0, 0, 0}, // 6
  {0, 0, 0, 0, 0, 0, 0, 0} // 7
};
byte currentPlayerPosition = 4;

byte enemyMoveLines = 1 * (difficulty + 1);
byte playerMoveLine = 8;
byte powerupStorage = 0; 

// entities
byte playerNumber = 1;
byte enemyNumber = 2;
byte enemyProjectile = 8;
byte playerProjectile = 9;

// powerups
byte oneLifeUp = 4; // cannot get higher than maximum, will not show when lives are at maximum (never on insane)
byte bombPowerUp = 5;
byte invinciblePowerUp = 6;
byte autoShootPowerUp = 7;


float maxEnemies = floor((1 / 3 * level) * difficulty + 3); // maximum number of enemies, the absolute will be 9
int projChance = floor((1 * level) * difficulty + 5); // chance / second of an enemy to shoot a projectile, absolute maximum - 50% / sec
int moveChance = 15 * difficulty; // chance of an enemy to move
float projSpeed = (2.5 - 0.5 * difficulty) * 1000; // enemy projectile speed
int powerupRespawn = (10 + 10 * difficulty) * 1000; // seconds 
int enemySpawnRate = (10 - 2 * difficulty) * 1000; // seconds

byte numberOfEnemies = 0;
int powerupPosition = -1;
byte powerupType = 0;

unsigned long lastEnemySpawn = 0;
unsigned long lastPowerupSpawn = 0;
unsigned long lastEProjMoved = 0;
unsigned long lastPProjMoved = 0;
unsigned int timePassed = 0;
unsigned int enemiesKilled = 0;

unsigned long powerupActiveTime = 0;
const int powerupTime = 1000;

unsigned int autoShootTime = 0;
// blinks for powerups and enemies
const int blinkTime = 250;
unsigned long lastBlink = 0;


const int respawnTime = 5000;

int invTime = respawnTime;
unsigned long invStartTime = 0;
const int respInvBlink = 500;
unsigned long lastInvBlink = 0;

unsigned long lastGameSecond = 0;

const byte bombIcon[8] = { // 5
  B01100,
  B00100,
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B01110
};

const byte invIcon[8] = { // 6
  B00000,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100
};

const byte autoIcon[8] = { // 7
  B00100,
  B01010,
  B10001,
  B11111,
  B10001,
  B00000,
  B10101,
  B00000
};


bool inGame = false;
void setup() {
  // matrix settings
  lc.shutdown(0, false);
  lc.setIntensity(0, brightness);
  lc.clearDisplay(0);

  // lcd settings
  pinMode(conPin, OUTPUT);
  analogWrite(conPin, mapContrast(contrast));
  
  lcd.begin(16, 2);

  lcd.createChar(0, downArrowChar);
  lcd.createChar(1, upArrowChar);
  lcd.createChar(2, downAndUpArrowChar);
  lcd.createChar(3, fullLife);
  lcd.createChar(4, emptyLife);
  lcd.createChar(5, bombIcon);
  lcd.createChar(6, invIcon);
  lcd.createChar(7, autoIcon);

  pinMode(joystickButton, INPUT_PULLUP);
  pinMode(joystickAxisX, INPUT);
  pinMode(joystickAxisY, INPUT);
  Serial.begin(9600);
}
void loop() {
  updateMatrix(inGame);
  currentTime = millis();
  if (inGame == false) {
    // menu loop
    checkMove();
  
    if ((currentTime - lastDebounceTime) > debounceTime && !digitalRead(joystickButton)) {
      changePhase();
      lastDebounceTime = currentTime;
    }

    if (displayUpdated == true) {
      printOnLcd();
    }
  }
  
  if (inGame == true) {
    playerMechanics();
    // game logic loop
    if (currentLives == 0) {
      gameOver();
    }

    if (displayUpdated == true) {
      gameMenu();
    }

    if ((currentTime - lastEProjMoved) > projSpeed) {
      moveProjectiles(false);
      lastEProjMoved = currentTime;
    }

    if ((currentTime - lastPProjMoved) > 75) {
      moveProjectiles(true);
      lastPProjMoved = currentTime;
    }

    if ((currentTime - lastGameSecond) > 1000) {
      scanMatrix();
      if (timePassed == nextLevelT || enemiesKilled == nextLevelE) {
        nextLevel();
      }
      lastGameSecond = currentTime;
    }

    if ((currentTime - lastBlink) > blinkTime && powerupPosition != -1) {
      powerupBlink();
      lastBlink = currentTime;
    }

    if (invTime != 0) {
      if ((currentTime - lastInvBlink) > respInvBlink) {
        invTime = invTime - respInvBlink;
        invBlink();
        lastInvBlink = currentTime;
      }
    }
  }
}

int mapContrast(byte con) {
  return map(15 - con, 1, 15, 16, 128);
}

void updateMatrix(bool game) {
  for (int i = 0; i < numberOfLines; i++) {
    for (int j = 0; j < numberOfLines; j++) {
      if (game == 0) {
          lc.setLed(0, i, j, 1);
      } else {
        if (matrix[i][j] == 0) {
          lc.setLed(0, i, j, 0);
        } else {
          lc.setLed(0, i, j, 1);
        }
      }         
    }
  }
}

void refreshProperties() {
  lc.setIntensity(0, brightness);
  analogWrite(conPin, mapContrast(contrast));  
}

void changePhase() {
  if (optionMenu == mainMenuIndex) {
    if (mainMenuLine == 2) {
      maxIndex = 5;
    }
    if (mainMenuLine == 1) {
      startGame();
    }
    optionMenu = mainMenuLine - 1;
  } else if (optionMenu == settingsIndex) {
    settingsMenu(settingLine);
  } else if (optionMenu == aboutIndex) {
    optionMenu = mainMenuIndex;
    maxIndex = 3;
  } else if (optionMenu == inSettingIndex) {
    optionMenu = settingsIndex;
    maxIndex = 5;
  }
  displayUpdated = true;
}

const int lowLimit = 1;
void checkForMovement(byte& index, bool isSetting) {
  axisValueX = analogRead(joystickAxisX);
  axisValueY = analogRead(joystickAxisY);

  if (axisValueX > minThreshold && axisValueX < maxThreshold &&
      axisValueY > minThreshold && axisValueY < maxThreshold) {
        joystickNeutral = true;
  }

  if (joystickNeutral == true) {
    if (isSetting == false) {
      if (axisValueX < minThreshold) {
        if (index > lowLimit) {
          index--;
          displayUpdated = true;
        }
        joystickNeutral = false;
      }
      if (axisValueX > maxThreshold) {
        if (index < maxIndex) {
          index++;
          displayUpdated = true;
        }
        joystickNeutral = false;
      }
    } else {
      if (axisValueY > maxThreshold) {
        if (index > 0) {
          index--;
          displayUpdated = true;
          refreshProperties();
        }
        joystickNeutral = false;
      }
      if (axisValueY < minThreshold) {
        if (index < maxIndex) {
          index++;
          displayUpdated = true;
          refreshProperties();
        }
        joystickNeutral = false;
      }
    }
  }
}

void nameChange(byte& index) {
  axisValueX = analogRead(joystickAxisX);
  if (axisValueX > minThreshold && axisValueX < maxThreshold &&
      axisValueY > minThreshold && axisValueY < maxThreshold) {
        joystickNeutral = true;
  }

  if (joystickNeutral == true) {
    if (axisValueX > maxThreshold) {
      if (index < maxNameIndex) {
        index++;
        displayUpdated = true;
      } else if (index == maxNameIndex) {
        index = 0;
        displayUpdated = true;
      }
      joystickNeutral = false;
    }
    if (axisValueX < minThreshold) {
      if (index > 0) {
        index--;
        displayUpdated = true;
      } else if (index == 0) {
        index = maxNameIndex;
        displayUpdated = true;
      }
      joystickNeutral = false;
    }
  }

}

void checkMove() {
  if (optionMenu == mainMenuIndex) {
    checkForMovement(mainMenuLine, 0);
  } 
  if (optionMenu == settingsIndex) {
    checkForMovement(settingLine, 0);
  }
  if (optionMenu == inSettingIndex) {
    switch(selectedSetting) {
      case 0: {
        checkForMovement(brightness, 1);
        break;
      }
      case 1: {
        checkForMovement(contrast, 1);
        break;
      }
      case 2: {
        checkForMovement(nameIndex, 1);
        nameChange(name[nameIndex]);
        break;
      }
      case 3: {
        checkForMovement(difficulty, 1);
        break;
      }
      default: {
        break;
      }
    }
  }
}

void printMenu(int index) {
  lcd.clear();

  //String mainText = menus[optionMenu][0];
  for (auto chr : menus[optionMenu][0]) {
    lcd.write((byte) chr);
  }

  lcd.setCursor(0, 1);
  lcd.write((byte) ('>'));
  lcd.setCursor(2, 1);
  
  //String text = menus[optionMenu][index];
  for (auto chr : menus[optionMenu][index]) {
    lcd.write((byte) chr);
  }
  lcd.setCursor(15, 1);

  if (index == lowLimit) {
    lcd.write(byte(0));
  } else if (index == maxIndex) {
    lcd.write(byte(1));
  } else {
    lcd.write(byte(2));
  }
}

void printSettings(int index) {
  lcd.clear();

  //String mainText = settings[index];
  for (auto chr : settings[index]) {
    lcd.write((byte) chr);
  }

  lcd.setCursor(0, 1);
  switch(index) {
    case 0: {
      for (int i = 0; i < maxBriCon; i++) {
        if (i <= brightness) {
          lcd.write(byte(3));
        } else {
          lcd.write(byte(4));
        }
      }
      break;
    }
    case 1: {
      for (int i = 0; i < maxBriCon; i++) {
        if (i <= contrast) {
          lcd.write(byte(3));
        } else {
          lcd.write(byte(4));
        }
      }
      break;
    }
    case 2: {
      for (auto chr : name) {
        lcd.write((byte) alphanum[chr]);
      }
      break;      
    }
    case 3: {
      //String diffText = settings[difficulty + 4];
      for (auto chr : settings[difficulty + 4]) {
        lcd.write((byte) chr);
      }
      break;
    }
  }
}

void settingsMenu(int line) {
  if (line != 5) {
    optionMenu = inSettingIndex;
    selectedSetting = line - 1;
  }
  switch(line) {
    case 1: { // brightness
      maxIndex = 15;
      break;
    }
    case 2: { // contrast
      maxIndex = 15;
      break;
    }
    case 3: { // name
      maxIndex = 4;
      break;
    }
    case 4: { // difficulty
      maxIndex = 4;
      break;
    }
    case 5: { // back
      optionMenu = mainMenuIndex;
      selectedSetting = 0;
      maxIndex = 3;
      break;
    }
    default: {
      optionMenu = mainMenuIndex;
      selectedSetting = 0;
      maxIndex = 3;
    }
  }
}


void gameMenu() {
  lcd.clear();

  //String hp = gameInfo[0];
  for (auto chr : gameInfo[0]) {
    lcd.write((byte) chr);
  }

  for (int i = 0; i < maxLives; i++) {
    if (i <= currentLives - 1) {
      lcd.write(byte(3));
    } else {
      lcd.write(byte(4));
    }
  }
  
  lcd.setCursor(9, 0);
  //String lvl = gameInfo[1];
  for(auto chr : gameInfo[1]) {
    lcd.write((byte) chr);
  }
  //String levelStr = String(level);
  for(auto chr : String(level)) {
    lcd.write((byte) chr);
  }

  lcd.setCursor(0, 1);
  //String pts = gameInfo[2];
  for (auto chr : gameInfo[2]) {
    lcd.write((byte) chr);
  }
  //String pointsStr = String(points);
  for(auto chr : String(points)) {
    lcd.write((byte) chr);
  }

  lcd.setCursor(15, 1);
  if (powerupStorage != 0) {
    lcd.write(byte(powerupStorage));
  } else {
    lcd.write((byte) 'X');
  }
}

void printOnLcd() {
  if (optionMenu == mainMenuIndex) {
    printMenu(mainMenuLine);
    displayUpdated = false;
  } 
  if (optionMenu == settingsIndex) {
    printMenu(settingLine);
    displayUpdated = false;
  } 
  if (optionMenu == aboutIndex) {
    printMenu(aboutLine);
    displayUpdated = false;
  }
  if (optionMenu == inSettingIndex) {
    printSettings(selectedSetting);
    displayUpdated = false;
  }
}

// ################################################ //
// #### GAME LOGIC - SEPARATED FROM MENU LOGIC #### //
// ################################################ //
void startGame() {
  inGame = true;
  difficulty = difficulty + 1; // mitigation for easy being 0
  // default game settings + difficulty related settings
  maxLives = ceil(6 - difficulty - 0.25 * difficulty);
  currentLives = maxLives;
  
  level = 0;
  points = 0;
  
  enemyMoveLines = 1 * difficulty;
  moveChance = 20 * difficulty;
  projSpeed = (1 - 0.225 * difficulty) * 1000;
  powerupRespawn = (5 + 5 * difficulty) * 1000;
  enemySpawnRate = (5 - 1 * difficulty) * 1000;

  invTime = 3000;
  powerupStorage = 0;
  powerupPosition = -1;
  powerupType = 0;
  
  spawnPlayer();
  nextLevel();
}

void spawnPlayer() {
  currentPlayerPosition = 4;
  matrix[numberOfLines - 1][currentPlayerPosition] = playerNumber;
}

void nextLevel() { // all level related settings change
  displayUpdated = true;
  level = level + 1;
  nextLevelE = 15 - 3 * difficulty + 2 * level;
  nextLevelT = 60 - 7 * difficulty + 3 * level;

  enemiesKilled = 0;
  timePassed = 0;

  ptsEnemy = 100 * level * difficulty;
  maxEnemies = min(floor((1 / 3 * level) * difficulty + 4), 10);
  projChance = min(floor((1 * level) * difficulty + 5 * difficulty), 75);
}

void scanMatrix() {
  numberOfEnemies = 0;
  timePassed = timePassed + 1;
  for (int i = 0; i < numberOfLines; i++) {
    for (int j = 0; j < numberOfLines; j++) {
      if (matrix[i][j] == 2) {
        numberOfEnemies = numberOfEnemies + 1;
        spawnProjectile(i, j, false);
        moveEnemy(i, j);
      }
    }
  }
  if ((currentTime - lastEnemySpawn) > enemySpawnRate && numberOfEnemies < maxEnemies) {
    spawnEnemy();    
    lastEnemySpawn = currentTime;
  }
  if ((currentTime - lastPowerupSpawn) > powerupRespawn && powerupStorage == 0 && powerupPosition == -1) {
    spawnPowerup();
    lastPowerupSpawn = currentTime;
  }
}

void spawnEnemy() {
  randomSeed(analogRead(2));
  byte row = random(enemyMoveLines);
  byte col = random(numberOfLines);
  if (matrix[row][col] != 0) {
    return;
  }
  matrix[row][col] = enemyNumber; 
}

void spawnPowerup() {
  randomSeed(analogRead(2));
  byte col = random(numberOfLines);
  byte powerup;
  if (currentLives == maxLives) {
    powerup = random(3) + 5;
  } else {
    powerup = random(4) + 4;
  }
  if (matrix[numberOfLines - 1][col] == 1) {
    pickupPowerup();
    return;
  }
  matrix[numberOfLines - 1][col] = powerup;
  powerupPosition = col;
  powerupType = powerup;
}

void spawnProjectile(byte i, byte j, bool friendly) {
  if (friendly == true) {
    if (matrix[i - 1][j] == 0 || matrix[i - 1][j] == playerProjectile) {
      matrix[i - 1][j] = playerProjectile;
    } else {
      matrix[i - 1][j] = 0;
      points = points + 1;
    }
  } else {
    randomSeed(analogRead(2));
    if (random(100) < projChance) {
      if (friendly == false) {
        if (matrix[i + 1][j] == 0) {
          matrix[i + 1][j] = enemyProjectile;
        }
      }
    }
  }
}

void playerMechanics() {
  axisValueX = analogRead(joystickAxisX);
  axisValueY = analogRead(joystickAxisY);

  if (axisValueX > minThreshold && axisValueX < maxThreshold &&
      axisValueY > minThreshold && axisValueY < maxThreshold) {
        joystickNeutral = true;
  }

  if (joystickNeutral == true) {
    if (axisValueY > maxThreshold) {
      if (currentPlayerPosition > 0) {
        matrix[numberOfLines - 1][currentPlayerPosition] = 0;
        currentPlayerPosition--;
        matrix[numberOfLines - 1][currentPlayerPosition] = playerNumber;
        if (currentPlayerPosition == powerupPosition) {
          pickupPowerup();
        }
      }
      joystickNeutral = false;
    }
    if (axisValueY < minThreshold) {
      if (currentPlayerPosition < numberOfLines - 1) {
        matrix[numberOfLines - 1][currentPlayerPosition] = 0;
        currentPlayerPosition++;
        matrix[numberOfLines - 1][currentPlayerPosition] = playerNumber;
        if (currentPlayerPosition == powerupPosition) {
          pickupPowerup();
        }
      }
      joystickNeutral = false;
    }
  }

  if (digitalRead(joystickButton)) {
    joystickButtonState = false;
  }

  if (autoShootTime <= 0) {
    if ((currentTime - lastDebounceTime) > debounceTime && !digitalRead(joystickButton) && joystickButtonState == false) {
        spawnProjectile(numberOfLines - 1, currentPlayerPosition, true);
        powerupActiveTime = currentTime;
        lastDebounceTime = currentTime;
        joystickButtonState = true;
    }
  } else {
    if ((currentTime - lastDebounceTime) > debounceTime) {
      spawnProjectile(numberOfLines - 1, currentPlayerPosition, true);
      autoShootTime = autoShootTime - debounceTime;
      lastDebounceTime = currentTime;
    }
  }

  if ((currentTime - powerupActiveTime) > powerupTime && !digitalRead(joystickButton)) {
    usePowerup();
  }  
}


void moveEnemy(byte i, byte j) {
  randomSeed(analogRead(2));
  byte direction = random(4);
  if (random(100) <= moveChance) {
    switch(direction) {
      case 0: { // MOVING UP
        if (i > 0 && matrix[i - 1][j] == 0) {
          matrix[i - 1][j] = enemyNumber;
          matrix[i][j] = 0;
        }
        break;
      }
      case 1: { // MOVING DOWN
        if (i > enemyMoveLines - 1 && matrix[i + 1][j] == 0) {
          matrix[i + 1][j] = enemyNumber;
          matrix[i][j] = 0;
        }
        break;
      }
      case 2: { // MOVING LEFT
        if (j > 0 && matrix[i][j - 1] == 0) {
          matrix[i][j - 1] = enemyNumber;
          matrix[i][j] = 0;
        }
        break;
      }
      case 3: { // MOVING RIGHT
        if (j < numberOfLines && matrix[i][j + 1] == 0) {
          matrix[i][j + 1] = enemyNumber;
          matrix[i][j] = 0;          
        }
        break;
      }
    }
  }
}

void moveProjectiles(bool isFriendly) {
  if (isFriendly == false) {
    for (int i = numberOfLines - 1; i >= 0; i--) {
      for (int j = numberOfLines - 1; j >= 0; j--) {
        if (matrix[i][j] == enemyProjectile) {
          if (i == numberOfLines - 1) {
            matrix[i][j] = 0;
          } else {
            if (matrix[i + 1][j] == playerNumber) {
              if (invTime == 0) {
                loseLife();
                displayUpdated = true;
              }
            } else if (matrix[i + 1][j] > playerNumber && matrix[i + 1][j] <= playerProjectile) {
              matrix[i + 1][j] = 0;
            } else if (matrix[i + 1][j] == 0) {
              matrix[i + 1][j] = enemyProjectile;
            }
            matrix[i][j] = 0;
          }
        }
      }
    } 
  } else {
    for (int i = 0; i < numberOfLines; i++) {
      for (int j = 0; j < numberOfLines; j++) {
        if (matrix[i][j] == playerProjectile) {
          if (i == 0) {
            matrix[i][j] = 0;
          } else {
            if (matrix[i - 1][j] == enemyProjectile) {
              points = points + ptsEnemy / 10;
              displayUpdated = true;
              matrix[i - 1][j] = 0;
            } else if (matrix[i - 1][j] == enemyNumber) {
              points = points + ptsEnemy;
              enemiesKilled = enemiesKilled + 1;
              displayUpdated = true;
              matrix[i - 1][j] = 0;
            } else if (matrix[i - 1][j] == 0) {
              matrix[i - 1][j] = playerProjectile;
            }
            matrix[i][j] = 0;
          }
        }
      }
    }
  }
}

void powerupBlink() {
  if (matrix[numberOfLines - 1][powerupPosition] == 0) {
    matrix[numberOfLines - 1][powerupPosition] = powerupType;
  } else {
    matrix[numberOfLines - 1][powerupPosition] = 0;
  }
}

void invBlink() {
  if (matrix[numberOfLines - 1][currentPlayerPosition] == 0) {
    matrix[numberOfLines - 1][currentPlayerPosition] = playerNumber;
  } else {
    matrix[numberOfLines - 1][currentPlayerPosition] = 0;
  }
}

void pickupPowerup() {
    displayUpdated = true;
    if (powerupType == oneLifeUp) {
      currentLives = currentLives + 1;
    } else {
      powerupStorage = powerupType;
    }
    powerupPosition = -1;
    powerupType = 0; 
}

void usePowerup() {
  if (powerupStorage == bombPowerUp) {
    points = points + ptsEnemy * numberOfEnemies;
    for (int i = 0; i < numberOfLines * 4; i++) {
      spawnProjectile((numberOfLines * 4 - i) / (numberOfLines / 2), i % 8, true);      
    }
  }
  if (powerupStorage == autoShootPowerUp) {
    autoShootTime = 10000;
  }
  if (powerupStorage == invinciblePowerUp) {
    invStartTime = currentTime;
    invTime = 10000;
  }

  powerupStorage = 0;
}


void loseLife() {
  currentLives = currentLives - 1;
  killAllEnemies();
  spawnPlayer();
  invStartTime = currentTime;
  invTime = 5000;
}

void killAllEnemies() {
  for (int i = 0; i < numberOfLines; i++) {
    for (int j = 0; j < numberOfLines; j++) {
      if (matrix[i][j] < oneLifeUp || matrix[i][j] > invinciblePowerUp) {
        matrix[i][j] = 0;
      }
    }
  }
  numberOfEnemies = 0;
}

void gameOver() {
  resetAll();
  inGame = false;
}

void resetAll() {
  optionMenu = 0;
  mainMenuLine = 1;
  settingLine = 1;
  aboutLine = 1;
  maxIndex = 3;
  selectedSetting = 0;
  brightness = 7;
  contrast = 7;
  nameIndex = 0;
  difficulty = 1;
  numberOfEnemies = 0;
  powerupPosition = -1;
  powerupType = 0;
  timePassed = 0;
  enemiesKilled = 0;
  displayUpdated = true;
}