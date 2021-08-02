/*
  Copyright (c) 2021, Amplified IT
  See the full description at http://labs.amplifiedit.com/centipede

  Support forums are available at https://currents.google.com/communities/100599537603662785064

  Published under an MIT License https://opensource.org/licenses/MIT
  
*/

#include <Keyboard.h>

/* Modify the following definations to fit your device version and whether you want Centipede to Powerwash the device on completion*/

#define device_version 91 // Change to the ChromeOS version you expect to use with Centipede; Changes have been reported in the following ranges 58-68, 69, 70
#define powerwash 1 //[0,1] Set to 0 after confirmation that script works through the enrollment process (Default 1).  
//Powerwash the device BEFORE enrollment is completed - NOTE: Will not work on ENROLLED devices. Used for Setting up Centipede.

/* Modify the following definitions to fit your wireless and enrollment credentials. */

#define wifi_name "" // Define SSID for your wireless connection.
#define wifi_pass "" // Define the password for your wireless connection.
#define wifi_security 2 //[0,1,2] Set to 0 for open, 1 for WEP, 2 for WPA
#define username "" // Define the user name for enrolling the device.
#define password "" // The password for the enrollment GAFE account.

/* Use these options to determine if you want to disable analytics, skip asset ID, or if you need to slow down the Centipede*/

#define sendUsageToGoogle 1 //[0,1] Set to 0 if you want to un-check the box to send usage analytics to Google
#define shutdown_when_finished 0 //[0,1] Set to 0 if you want Centipede to stop at the asset ID and location screen
#define selected_on_boot 3 //[0,1,2,3]  Active location when device poweron : 3 - "Let's go"(Default), 2 - accessibility, 1 - language, 0 - menu 

/* These are advanced options. The defaults should be fine, but feel free to tweak values below. */

#define setWiFi true //[true,false] Set to false for devices that already have WiFi setup and have accepted Terms of Service (ToS) 

/* Use this area for advanced network setup options*/
#define advancedNetworkSetup false //[true,false] Set to true for EAP configuration, and fill out the definitions below
#define eapMethod "LEAP" // Valid options are "LEAP" "PEAP" "EAP-TLS" or "EAP-TTLS" - Note that they require the quotes to work properly
#define phaseTwoAuthentication 2 //[0,1,2,3,4,5,6] Set to 0 for automatic, 1 for EAP-MD5, 2 for MSCHAP(v2 pre v69; v1 V69+, 3 for MSCHAPv2, 4 for PAP, 5 for CHAP, 6 for GTC; v69+)
#define serverCaCertificateCheck 0 //[0,1] 0 is default, 1 is "Do not check"
#define subjectMatch "" // Fill in subject match here if needed for advanced wireless
#define identity "identity" // Fill in identity here if needed for advanced wireless
#define anonymousIdentity "" // Fill in anonymous identity here for advanced wireless
#define saveIdentityAndPassword 0 //[0,1] Set to 1 to save identity and password. NOT RECOMMENDED
#define sso 0 //[0,1] Set to 1 if using Single Sign On - NOTE: May need additional configuration in Advanced Network Setup around line 182.
#define retry 0 //attempt to retry enrollment in the event of an enrollment error;

/* Use this section for additional non-traditional methods*/
#define longer_enrollment_time 10 // Set to additional seconds to wait for Device Configuration and Enrollment
#define sign_in 0 //[0,1] Set to 1 to sign-in to the device after enrollment
#define remove_enrollment_wifi 0 //[0,1] Set to 1 to remove the enrollment wifi network. *sign_in also must be true* - NOTE: Only set to true when Chrome Device Network has been pulled down
#define enroll_device_cert 0 //[0,1] Set to 1 if enrolling device wide certificate *sign_in also must be true* - NOTE: Works best if user _*only*_ has Certificate Enrollment extension force installed

#define slowMode 1 // [0,1] Set to 1 if Centipede appears to be moving too quickly at any screen. This will slow down the entire process
#define update_wait_time 10 // Set to seconds to wait for mandatory updates to be downloaded

/* Do not modify anything below this line unless you're confident that you understand how to program Arduino or C */

// Version Definition
#define VERSION_69 (device_version >= 69)
#define VERSION_70 (device_version >= 70)
#define VERSION_78 (device_version >= 78)
#define VERSION_83 (device_version >= 83)
#define VERSION_85 (device_version >= 85)
#define VERSION_86 (device_version >= 86)
#define VERSION_90 (device_version >= 90)
#define VERSION_91 (device_version >= 91)

// Special characters definition
#define KEY_LEFT_CTRL   0x80
#define KEY_LEFT_SHIFT  0x81
#define KEY_LEFT_ALT    0x82
#define KEY_RIGHT_CTRL  0x84
#define KEY_RIGHT_SHIFT 0x85
#define KEY_RIGHT_ALT   0x86
#define KEY_UP_ARROW    0xDA
#define KEY_DOWN_ARROW  0xD9
#define KEY_LEFT_ARROW  0xD8
#define KEY_RIGHT_ARROW 0xD7
#define KEY_BACKSPACE   0xB2
#define KEY_TAB         0xB3
#define KEY_ENTER       0xB0
#define KEY_ESC         0xB1
#define KEY_CAPS_LOCK   0xC1

int buttonPin = 2;  // Set a button to any pin
int RXLED = 17;
static uint8_t __clock_prescaler = (CLKPR & (_BV(CLKPS0) | _BV(CLKPS1) | _BV(CLKPS2) | _BV(CLKPS3)));

void setup()
{
  setPrescaler(); // Set prescaler to highest clock speed
  Keyboard.begin(); // Start they keyboard emulator
  pinMode(buttonPin, INPUT);  // Set up the debugging pin. If you want to debug the code, use a length of wire to connect pins 2 and GND on the board
  digitalWrite(buttonPin, HIGH);

  pinMode(RXLED, OUTPUT); // Configure the on-board LED
  digitalWrite(RXLED, LOW);
  TXLED1;
  if (digitalRead(buttonPin) == 0) {
    showSuccess();
  }
  wait(5); // Wait for all services to finish loading
}

void loop() { // Main Function - workflow is called within loop();
  if (digitalRead(buttonPin) == 1 ) { // Check for debugging. If not debugging, run the program0
    startPowerWash();
    bootLoop();
  }
}

void bootLoop() {
  //      digitalWrite(RXLED, LOW);   // set the LED on
  TXLED0; //TX LED is not tied to a normally controlled pin
  delay(200);              // wait for a second
  TXLED1;
  delay(200);
  TXLED0; //TX LED is not tied to a normally controlled pin
  delay(200);              // wait for a second
  TXLED1;
  delay(800);
}

void showSuccess() {
  digitalWrite(RXLED, HIGH);  // set the LED off
  while (true) {
    bootLoop();
  }
}

void repeatKey(byte key, int num) {
  for (int i = 0; i < num; i++) {
    Keyboard.write(key);
    wait(1);
  }
}

void blink() {
  digitalWrite(RXLED, LOW);
  //  TXLED1;
  delay(250);
  digitalWrite(RXLED, HIGH);
  //  TXLED0;
  delay(250);
}

void wait(int cycles) {
  for (int i = 0; i < cycles; i++) {
    blink();
    if (slowMode) {
      delay(250);
    }
  }
}

void startPowerWash() {

  //starts powerwash
  // Keyboard.press(KEY_LEFT_SHIFT);
  // Keyboard.press(KEY_LEFT_ALT);
  // Keyboard.press(KEY_LEFT_CTRL);
  // Keyboard.press('r');
  // wait(1);
  // Keyboard.releaseAll();
  // wait(1);
  // Keyboard.write(KEY_TAB);
  // wait(1);
  // Keyboard.press(KEY_LEFT_SHIFT);
  // Keyboard.write(KEY_TAB);
  // wait(1);
  // Keyboard.releaseAll();
  // Keyboard.write(KEY_ENTER);
  // wait(1);
  // repeatKey(KEY_TAB, 2); //tab tab enter - confirms powerwash
  // Keyboard.write(KEY_ENTER);

  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.write('s');
  Keyboard.release(KEY_LEFT_ALT);
  Keyboard.release(KEY_LEFT_SHIFT);
  wait(2);




  // Keyboard.write(KEY_ENTER); 
  // while (digitalRead(buttonPin) != 1) { // wait for reboot
  //     bootLoop();
  //   }
  // TXLED0;
  // wait(25);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.write('s');
  Keyboard.release(KEY_LEFT_ALT);
  Keyboard.release(KEY_LEFT_SHIFT);
  wait(2);
  //to select the Network
  repeatKey(KEY_TAB, 3);  // 3 for pre v70, 4 for ver 70 (black menu)
  wait(2);
  Keyboard.write(KEY_ENTER);
  wait(1);
  Keyboard.write(KEY_ENTER);
  wait(2);
  //to select the 'add Wifi' icon
  repeatKey(KEY_TAB, 3);
  wait(2);
  Keyboard.write(KEY_ENTER);
  wait(4);
  // SSID
  Keyboard.print(wifi_name);
  wait(2);
  Keyboard.write(KEY_TAB);
  repeatKey(KEY_DOWN_ARROW, 2);
  Keyboard.write(KEY_TAB);
  Keyboard.print(wifi_pass);
  repeatKey(KEY_TAB, 3);
  Keyboard.write(KEY_ENTER);
  wait(2);

  //welcome to chromebook screen
  // Keyboard.write(KEY_TAB);
  // wait(2);
  // Keyboard.write(KEY_TAB);
  // wait(2);
  // Keyboard.write(KEY_TAB);
  // wait(2);
  Keyboard.write(KEY_ENTER);
  wait(2);
  Keyboard.write(KEY_ENTER);
  wait(2);

  //Terms of service
  Keyboard.write(KEY_TAB);
  Keyboard.press(KEY_LEFT_SHIFT);
  repeatKey(KEY_TAB, 6);
  Keyboard.releaseAll();
  Keyboard.write(KEY_ENTER); 
  wait(2);
  repeatKey(KEY_TAB, 2); 
  Keyboard.write(KEY_ENTER); 
  wait(25);
  Keyboard.write(KEY_ENTER);
  wait(8);

  //power down 
  repeatKey(KEY_TAB, 3);
  Keyboard.write(KEY_ENTER);
  wait(25);

  //todo chrome vox reader dialog
  // final sleep hibernate
}

void wifiConfig() {
  // Access the Network option from the system tray (Status Area).
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.write('s');
  Keyboard.release(KEY_LEFT_ALT);
  Keyboard.release(KEY_LEFT_SHIFT);
  wait(2);
  //to select the Network
  repeatKey(KEY_TAB, 3 + VERSION_70);  // 3 for pre v70, 4 for ver 70 (black menu)
  wait(2);
  Keyboard.write(KEY_ENTER);
  wait(2);
  //to select the 'add Wifi' icon
  repeatKey(KEY_TAB, 3);
  Keyboard.write(KEY_ENTER);
  wait(2);
  // SSID
  Keyboard.print(wifi_name);
  wait(2);
  // TAB
  Keyboard.write(KEY_TAB);
  wait(2);
  if (wifi_security == 0) {
    repeatKey(KEY_TAB, 2);
  }
  wait(2);
  // Enter
  Keyboard.write(KEY_ENTER); // Connect
  // Delay 15 seconds to connect
  wait(15);
}

void setPrescaler() {
  // Disable interrupts.
  uint8_t oldSREG = SREG;
  cli();

  // Enable change.
  CLKPR = _BV(CLKPCE); // write the CLKPCE bit to one and all the other to zero

  // Change clock division.
  CLKPR = 0x0; // write the CLKPS0..3 bits while writing the CLKPE bit to zero

  // Copy for fast access.
  __clock_prescaler = 0x0;

  // Recopy interrupt register.
  SREG = oldSREG;
}

//TODO:
#define in_developer_mode 0 // Set to 1 if device is in developer mode 
