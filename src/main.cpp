#include <Arduino.h>
#include <string.h>

/* Setarea vitezei de transmisie pentru comunicatia seriala */
#define BAUD_RATE 9600

/* Pinii pentru butoane */
#define BUTTON_START 2
#define BUTTON_DIFFICULTY 3

/* Pinii pentru LED-urile RGB*/
#define LED_BLUE 4
#define LED_GREEN 5
#define LED_RED 6

void startCountdown();
void startNewRound();
void setLEDColor(bool red, bool green, bool blue);
int checkWord(const char *typedWord);
void checkGameTime();
void initializeGame();
void StartStopButtonPress();
void DifficultyButtonPress();
void readCharacter();
void selectNewWord(bool wasCorrect);
void endGame();

/* Dictionarul de cuvinte pe care trebuie sa le introduc */
const char dictionary[15][20] = {
    "casa", "mancare", "bucurie", "copil",
    "floare", "soare", "apa", "frate",
    "sora", "carte", "ras", "nor",
    "computer", "prieten", "banca"};

/* Limitele de timp pentru cele 3 dificultati in milisecunde */
const unsigned long difficultyTimeLimits[3] = {5000, 3750, 2500};

/* Numele pentru fiecare dificultate */
const char *difficultyNames[] = {"Easy", "Medium", "Hard"};

/* Nivelul de dificultate curent: 0(usor), 1(mediu) sau 2(greu) */
int difficultyLevel = 0;

/* Durata unui joc */
const unsigned long gameDuration = 30000;

/* Intarzierea pentru debounce al butoanelor */
const unsigned long debounceDelay = 500;

/* Intarzierea inainte de pornirea jocului */
const unsigned long countdownDelay = 3000;

/* Cuvantul introdus la tastatura*/
char currentWord[20];

/* Cuvantul pe care trebuie sa il tastam */
char targetWord[20];

/* Indexul curent al ultimului caracter introdus la tastatura */
int wordIndex = 0;

/* Scorul jucatorului */
int score = 0;

/* Timpurile pentru apasarile butoanelor de start/stop si dificultate */
volatile unsigned long lastDifficultyPressTime = 0, lastStartPressTime = 0;

/* Variabila pentru masurarea timpului total de joc */
unsigned long gameStartTime = 0;

/* Timpul selectarii unui cuvant nou(pentru a verifica daca a fost introdus in intervalul de timp specific dificultatii) */
unsigned long wordSelectionTime = 0;

/* Variabila folosita pentru a verifica cand a trecut o secunda */
static unsigned long lastUpdateTime = 0;

/* Cifrele afisare in countdown */
static int countdown = 3;

/* Arata daca un cuvant a fost ales */
bool isWordSelected = false;

/* Starea de repaus si de rulare a jocului */
bool isIdle = true, isRunning = false;

void setup()
{
  Serial.begin(BAUD_RATE);

  /* Setez LED-urile ca output, si fac RGB-ul alb */
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  setLEDColor(true, true, true);

  /* Setez butoanele de start/stop si dificultate ca input cu pull-up */
  pinMode(BUTTON_START, INPUT_PULLUP);
  pinMode(BUTTON_DIFFICULTY, INPUT_PULLUP);

  /* Intreruperile pentru cele 2 butoane: start/stop si dificultate*/
  attachInterrupt(digitalPinToInterrupt(BUTTON_START), StartStopButtonPress, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_DIFFICULTY), DifficultyButtonPress, FALLING);
}

void loop()
{
  /* Verifica daca jocul s-a terminat */
  checkGameTime();

  /* Daca jocul ruleaza */
  if (isRunning)
  {
    /* Daca jocul abia a inceput, atunci se produce numaratoarea inversa */
    if (millis() - gameStartTime <= countdownDelay)
    {
      startCountdown();
    }
    else
    {
      /* Cat timp are ce sa citeasca de la jucator */
      if (Serial.available())
      {
        /* Citeste caracterul introdus */
        readCharacter();

        /* Verifica daca cuvantul introdus este corect */
        int result = checkWord(currentWord);

        /* Daca este corect alege un nou cuvant */
        if (result == 1)
        {
          selectNewWord(true);
        }
        /* Daca este corect ce scriem, atunci avem culoarea verde*/
        else if (result == 0)
        {
          setLEDColor(false, true, false);
        }

        /* Daca nu este corect ne avertizeaza prin culoarea rosie */
        else if (result == -1)
        {
          setLEDColor(true, false, false);
        }
      }

      /* Verifica daca timpul pentru fiecare cuvant a expirat, iar daca a expirat alege un nou cuvant */
      if (millis() - wordSelectionTime > difficultyTimeLimits[difficultyLevel])
      {
        selectNewWord(false);
      }
    }
  }
}

/* Functie pentru setarea culorii LED-ului RGB */
void setLEDColor(bool red, bool green, bool blue)
{
  digitalWrite(LED_RED, red ? HIGH : LOW);
  digitalWrite(LED_GREEN, green ? HIGH : LOW);
  digitalWrite(LED_BLUE, blue ? HIGH : LOW);
}

/* Functie care verifica daca cuvantul introdus este corect
-1 - daca literele difera
0  - literele coincid dar nu este complet
1  - daca cuvantul este corect
*/
int checkWord(const char *typedWord)
{
  int typedLength = strlen(typedWord);
  int targetLength = strlen(targetWord);

  if (strncmp(typedWord, targetWord, typedLength) != 0)
  {
    return -1;
  }
  return (typedLength == targetLength) ? 1 : 0;
}

/* Functia care incheie jocul si reseteaza LED-ul in alb */
void endGame()
{
  Serial.print("\nScor final: ");
  Serial.println(score);
  setLEDColor(true, true, true);
}

/* Verifica daca timpul a expirat, iar daca a expirat seteaza totul la starea de la inceput */
void checkGameTime()
{
  if (isRunning && millis() - gameStartTime > gameDuration + countdownDelay)
  {
    isIdle = true;
    isRunning = false;
    endGame();
  }
}

/* Functia pentru setarea unei noi runde */
void startNewRound()
{
  /* Setez indexul la 0 */
  wordIndex = 0;

  /* Selectez un cuvant random */
  strcpy(targetWord, dictionary[random(15)]);

  /* Afisez culoarea care trebuie introdusa la tastatura */
  Serial.println(targetWord);

  /* Setez culoarea verde */
  setLEDColor(false, true, false);

  /* Goleste buffer-ul */
  while (Serial.available() > 0)
  {
    Serial.read();
  }
}

/* Functia de numaratoare inversa*/
void startCountdown()
{
  if (millis() - gameStartTime <= countdownDelay)
  {
    /* Sting LED-ul atunci cand am la countdown un numar par*/
    if (countdown % 2 == 0)
    {
      setLEDColor(false, false, false);
    }
    /* Aprind LED-ul atunci cand am la countdown un numar impar*/
    else
    {
      setLEDColor(true, true, true);
    }
    /* Verific daca a trecut o secunda */
    if (millis() - lastUpdateTime >= 1000)
    {
      /* Actualizeaza timpul ultimei actualizari */
      lastUpdateTime = millis();
      /* Cat timp countdown este mai mare decat 0 afisez numaratoarea inversa */
      if (countdown > 0)
      {
        Serial.println(countdown);
        countdown--;
      }
    }
  }
  /* Cand countdown s-a terminat, incep runda*/
  else if (countdown <= 0)
  {
    startNewRound();
  }
}

/* Initializeaza variabilele pentru un nou joc */
void initializeGame()
{
  /* Goleste buffer-ul */
  while (Serial.available() > 0)
  {
    Serial.read();
  }

  /* Inregistreaza momentul startului */
  gameStartTime = millis();

  /* Reseteaza scorul */
  score = 0;

  /* Reseteaza contorul de blink */
  countdown = 3;
}

/* Functia de intrerupere a butonului de dificultate */
void DifficultyButtonPress()
{
  /* Daca jocul nu ruleaza, intarzierea de debounce a trecut, iar butonul este apasat, atunci schimba dificultatea */
  if (isIdle && millis() - lastDifficultyPressTime > debounceDelay && digitalRead(BUTTON_DIFFICULTY) == LOW)
  {
    lastDifficultyPressTime = millis();
    difficultyLevel++;
    if (difficultyLevel >= 3)
    {
      difficultyLevel = 0;
    }
    Serial.print(difficultyNames[difficultyLevel]);
    Serial.println(" mode on!");
  }
}

/* Functia de intrerupere a butonului de stop */
void StartStopButtonPress()
{
  /* Verifica daca intarzierea de debounce a trecut si daca butonul de start este apasat */
  if (millis() - lastStartPressTime > debounceDelay && digitalRead(BUTTON_START) == LOW)
  {
    isWordSelected = false;
    lastStartPressTime = millis();
    if (isIdle)
    {
      isIdle = false;
      isRunning = true;
    }
    else
    {
      isIdle = true;
      isRunning = false;
    }
    isRunning ? initializeGame() : endGame();
  }
}

/* Functia de citire a caracterelor */
void readCharacter()
{
  char letter = Serial.read();
  /* Verifica daca tasta apasata este backspace si nu este primul caracter */
  if (int(letter) == '\b' && wordIndex > 0)
  {
    /* Sterge ultima litera */
    currentWord[--wordIndex] = '\0';
  }

  else
  /* Adauga litera */
  {
    currentWord[wordIndex++] = letter;
    currentWord[wordIndex] = '\0';
  }
}

/* Functia de selectare a cuvantului */
void selectNewWord(bool wasCorrect)
{
  /* Daca cuvantul a fost corect incrementeaza scorul */
  if (wasCorrect)
  {
    score++;
    Serial.println("\nCorect!\n");
  }
  /* Daca nu este corect, timpul a trecut si am avut un cuvant inainte, atunci afisez ca timpul a trecut */
  else if (isWordSelected)
  {
    Serial.println("\n Timpul a trecut!\n");
  }

  /* Reseteaza cuvantul curent */
  strcpy(currentWord, "");

  /* Reseteaza indexul */
  wordIndex = 0;

  /* Alege un cuvant random */
  strcpy(targetWord, dictionary[random(15)]);

  /* Dupa ce am ales un cuvant, isWordSelected true pentru ca avem un cuvant */
  isWordSelected = true;

  /* Afiseaza acel cuvant nou */
  Serial.println(targetWord);

  /* Modifica momentul in care a fost ales noul cuvant */
  wordSelectionTime = millis();

  /* Modific LED-ul in verde */
  setLEDColor(false, true, false);

  /* Goleste buffer-ul */
  while (Serial.available() > 0)
  {
    Serial.read();
  }
}
