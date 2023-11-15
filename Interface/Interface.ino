#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4IMU imu;
Zumo32U4ButtonA buttonA;
Zumo32U4OLED display;
Zumo32U4Motors motors;
Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Encoders encoders;
Zumo32U4ProximitySensors proxSensors;

int ChallengeValues = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  StartChallenge(PickChallenge());
}

int PickChallenge() {
  int challenge;
  int EncoderVal = 0;
  display.clear();
  encoders.getCountsAndResetRight();
  do {
    EncoderVal = encoders.getCountsRight() / 2;
    Serial.print(EncoderVal);
    Serial.print(" + ");
    Serial.println(challenge);
    if (0 <= EncoderVal && EncoderVal <= 100) {
      if (challenge != 0) {
        display.clear();
        display.print("Task: 1");
      }
      challenge = 0;
    } else if (100 <= EncoderVal && EncoderVal <= 200) {
      if (challenge != 1) {
        display.clear();
        display.print("Task: 2");
      }
      challenge = 1;
    } else if (200 <= EncoderVal && EncoderVal <= 300) {
      if (challenge != 2) {
        display.clear();
        display.print("Task: 3");
      }
      challenge = 2;
    } else if (300 <= EncoderVal && EncoderVal <= 400) {
      if (challenge != 3) {
        display.clear();
        display.print("Task: 4");
      }
      challenge = 3;
    } else if (400 <= EncoderVal && EncoderVal <= 500) {
      if (challenge != 4) {
        display.clear();
        display.print("Task: 5");
      }
      challenge = 4;
    } else if (500 <= EncoderVal && EncoderVal <= 600) {
      if (challenge != 5) {
        display.clear();
        display.print("Task: 6");
      }
      challenge = 5;
    } else if (600 <= EncoderVal && EncoderVal <= 700) {
      if (challenge != 6) {
        display.clear();
        display.print("Task: 7");
      }
      challenge = 6;
    } else {
      encoders.getCountsAndResetRight();
    }
  } while (buttonA.isPressed() == false);

  return challenge;
}

void NameAndCountdown(int ChosenChallenge) {
  display.clear();
  display.print((String) "Chosen:" + (ChosenChallenge + 1));
  for (int i = 0; i < 8; i++) {
    display.gotoXY(i, 1);
    display.print("-");
    delay(350);
  }
}

void StartChallenge(int ChosenChallenge) {
  switch (ChosenChallenge) {
    case 0:
      NameAndCountdown(ChosenChallenge);
      ChallengeValues = Choose_Distance_MAXED(15);
      StartLineUp();
      CH1_DriveProximity(ChallengeValues);
      break;
    case 1:
      NameAndCountdown(ChosenChallenge);
      CH2_LineFollower();
      break;
    case 2:
      NameAndCountdown(ChosenChallenge);
      StartLineUp();

      break;
    case 3:
      NameAndCountdown(ChosenChallenge);
      ChallengeValues = Choose_Distance();
      StartLineUp();
      CH4_Forward_Distance(ChallengeValues);
      break;
    case 4:
      NameAndCountdown(ChosenChallenge);
      StartLineUp();

      break;
    case 5:
      NameAndCountdown(ChosenChallenge);
      StartLineUp();

      break;
    case 6:
      NameAndCountdown(ChosenChallenge);
      StartLineUp();
      CH7_ForwardToLine();
      break;
  }
}
