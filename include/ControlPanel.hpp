#ifndef CONTROLPANEL_HPP
#define CONTROLPANEL_HPP

#include "main.hpp"
#include "helpers.hpp"

class PirSettings
{
public:
    int id;
    bool Enabled = true;
    bool Delayed = false;
};

class Settings
{
public:
    std::string Pin = "5";

    int ArmDelay = 10;
    int DisarmTime = 10;

    PirSettings P1Settings;
    PirSettings P2Settings;
    PirSettings P3Settings;

    Settings();

    void SetSetting(std::string settingName, std::string settingValue);
    std::string GetSetting(std::string settingName);
};

class ControlPanel
{
public:
    enum class State
    {
        Disarmed,
        Arming,
        Armed,
        EntryArmed,
        Alarm
    };
    
private:
    static std::string _temporaryPin;
    static uint8_t _pirStates;

    static bool _isInMenu;
    volatile static State _state;

    static Leds _greenLed;
    static Leds _redLed;
    static Leds _orangeLed;
    static Leds _blueLed;

    static void InitializePirManager();
    static void InitializeKeyboard();
    static void InitializeLCD();
    static void InitializeInterface();

    static void UpdateIdleInformation(void *args);  // MUST BE FreeRTOS TASK

    static void CheckKeyboardEntry(void *args);     // MUST BE FreeRTOS TASK
    static void KeyboardBehavior();

    static volatile bool _keyboardInterrupt;
    static volatile Peripheral::Keyboard::Button _keyboardInterruptButton;

    static void OpenMainMenu();
    static void ArmAlarm();
    static void DisarmAlarm();

    static void Alarm();

    static void ArmTask(void *args);                // MUST BE FreeRTOS TASK
    static void EnterTimeTask(void *args);             // MUST BE FreeRTOS TASK

public:
    static Settings settings;
    static PIR::PIRManager *pirManager;
    static Peripheral::Keyboard *keyboard;
    static Screen::LCD *lcd;
    static Screen::Interface *interface;

    static void InitializeTask(void *args);         // MUST BE FreeRTOS TASK
    static void KeyboardInterrupt(Peripheral::Keyboard::Button button);

    static void PirMovement(int pirId);
};

#endif //ControlPanel_hpp
