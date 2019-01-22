
/* ----------------------------------------------
  DigiLx - Linux-style DigiOS version 1.3
  Digispark mini-OS
  Copyright (c) 2019 Jaromaz https://jm.iq.pl
  
  Available commands:
  login, p[0-2] [on|off], temp, help, vcc, clear,
  uptime, clock [1-7], reboot, logout, exit
  ---------------------------------------------- */

// root password of up to seven characters
const char rootPassword[] = "admin12";

//-----------------------------------------------
#include <DigiCDC.h>
char serialChar[1], stringInput[8];
boolean stringComplete = false;
byte state = 1;
byte clocks[] = { 16, 8, 4, 2, 1, 500, 250, 125 };
char login[8];

static void reboot()
//-----------------------------------------------
{
  SerialUSB.print(F("\r\nRebooting ... "));
  noInterrupts();
  CLKPR = 0b10000000;
  CLKPR = 0;
  void (*ptrToFunction)();
  ptrToFunction = 0x0000;
  (*ptrToFunction)();
}

void clockMessageFormat (byte speed)
//-----------------------------------------------
{
  SerialUSB.print(F("\r\nset to "));
  SerialUSB.print(clocks[speed], DEC);
  if (clocks[speed] > 16) {
    SerialUSB.print(F("k"));
  } else {
    SerialUSB.print(F("m"));
  }
  SerialUSB.println(F("Hz\r\n\r\nbye ..."));
}

void clockSpeed(byte speed)
//-----------------------------------------------
// edit the code of this procedure to get the right result
{
  clockMessageFormat(speed);
  for (byte i = 0; i < 12; i++) {
    PORTB |= (1 << 1);
    SerialUSB.delay(200);
    PORTB &= ~(1 << 1);
    SerialUSB.delay(200);
    if (i == 5) {
      CLKPR = 0b10000000;
      CLKPR = speed;
    }
  }
  reboot();
}

//-----------------------------------------------
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)

void uptimeFormat(byte digits, char* form)
//-----------------------------------------------
{
  if (digits > 0)
  {
    SerialUSB.print(digits, DEC);
    SerialUSB.print(F(" "));
    SerialUSB.print(form);
    if (digits > 1) SerialUSB.print(F("s"));
    if (strcmp(form, "second")) {
      SerialUSB.print(F(", "));
    }
  }
}

static void uptime()
//-----------------------------------------------
{
  long seconds = millis() / 1000;
  SerialUSB.print(F("\r\nup "));
  uptimeFormat(elapsedDays(seconds), "day");
  uptimeFormat(numberOfHours(seconds), "hour");
  uptimeFormat(numberOfMinutes(seconds), "minute");
  uptimeFormat(numberOfSeconds(seconds), "second");
  SerialUSB.println();
}

static void getVcc()
//-----------------------------------------------
{
  ADMUX = _BV(MUX3) | _BV(MUX2);
  SerialUSB.delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC));
  uint8_t low  = ADCL;
  uint8_t high = ADCH;
  long result = (high << 8) | low;
  result = 1125300L / result;
  SerialUSB.print(F("\r\nVoltage: "));
  SerialUSB.print(result);
  SerialUSB.println(F(" mV"));
}

static void getTemp()
//-----------------------------------------------
{
  analogReference(INTERNAL1V1);
  analogRead(A0);
  SerialUSB.delay(200);
  int temp = analogRead(A0 + 15) - 273;
  analogReference(DEFAULT);
  SerialUSB.print(F("\r\nDigispark temperature: "));
  SerialUSB.print(temp);
  SerialUSB.println(F("°C"));
}

void clearScreen()
//-----------------------------------------------
{
  for (byte i = 0; i < 35; i++) {
    SerialUSB.println();
    SerialUSB.delay(5);
  }
}

static void help()
//-----------------------------------------------
{
 SerialUSB.println(F("\r\nLinux-style DigiOS version 1.3\r\n\r\nUser\
 Commands:\r\n\r\nlogin,p[0-2] [on|off], temp, help, vcc, clear,\
 \r\nuptime, clock [1-7], reboot, logout, exit\r\n\r\nclock 1 - 8mHz,\
 2 - 4mHz, 3 - 2mHz, 4 - 1mHz,\r\n5 - 500kHz, 6 - 250kHz, 7 - 125kHz"));
}

static void stateChg() {
  state = 2;
}

static void serialReader()
//-----------------------------------------------
{
  while (SerialUSB.available())
  {
    serialChar[0] = (char)SerialUSB.read();
    if ((' ' <= serialChar[0]) && (serialChar[0] <= '~')) {
      strcat(stringInput, serialChar);
    } else {
      if (stringInput[0] != 0) {
        stringComplete = true;
        return;
      }
    }
  }
}

// list of keywords and procedures assigned to them
static const struct { const char phrase[8]; void (*handler)(void); } keys[] =
{
  // ---- comment on this block to get more memory for your own code ---
  { "vcc", getVcc }, { "help", help }, { "temp", getTemp },
  { "reboot", reboot },  { "exit", stateChg }, { "uptime", uptime },
  { "clear", clearScreen },
  // -------------------------------------------------------------------
  { "logout", stateChg }
};


void setup()
//-----------------------------------------------
{
  // Set pins 0-2 as OUTPUT:
  DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
  SerialUSB.begin();
}


void loop()
//-----------------------------------------------
{
  serialReader();
  if (stringComplete)
  {
    // the Android Serial USB Terminal application requires the following 200 ms delays
    // if you are using another system, you can remove all these 200 ms delays.
    SerialUSB.delay(200);

    if (!strcmp(stringInput, "login")) state = 2;

    // password validation
    if (state == 4)
    {
      if (!strcmp(stringInput, rootPassword) and !strcmp(login, "root"))
      {
        state = 3;
      } else {
        SerialUSB.delay(1500);
        SerialUSB.println(F("\r\nLogin incorrect"));
        state = 1;
      }
    }

    // status after logging in
    if (state == 3)
    {
      // ---- comment on this block to get more memory for your own code ---
      if (stringInput[0] == 'p') {
        if ((stringInput[1] - 48) < 3 and stringInput[4] == 'n') {
          PORTB |= (1 << stringInput[1] - 48);
        } else if ((stringInput[1] - 48) < 3 and stringInput[4] == 'f') {
          PORTB &= ~(1 << stringInput[1] - 48);
        }
      }
      if (strstr(stringInput, "clock ")) {
        clockSpeed(stringInput[6] - 48);
      }
      // ---------------------------------------------------------------------

      // keyword procedures
      for (byte i = 0; i < sizeof keys / sizeof * keys; i++) {
        if (!strcmp(stringInput, keys[i].phrase)) keys[i].handler();
      }

      SerialUSB.delay(200);

      if (state == 3) SerialUSB.print(F("\r\nroot@digilx:~# "));
    }

    // password input window
    if (state == 5)
    {
      strcpy(login, stringInput);
      SerialUSB.print(F("\r\npassword: "));
      state = 4;
    }

    // login prompt
    if (state < 3)
    {
      if (state > 1) clearScreen();
      SerialUSB.print(F("\r\nDigiLx 1.3 - Digispark mini-OS\r\n\r\ndigilx login: "));
      state = 5;
    }

    SerialUSB.delay(200);
    stringInput[0] = 0;
    stringComplete = false;
  }
}

