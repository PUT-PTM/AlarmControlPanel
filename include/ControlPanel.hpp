#ifndef CONTROLPANEL_HPP
#define CONTROLPANEL_HPP

#include "main.hpp"

class ControlPanel
{
    static std::string _temporaryPin;

    static bool _isInMenu;
    static bool _isArmed;

    static void InitializePirManager();
    static void InitializeKeyboard();
    static void InitializeLCD();
    static void InitializeInterface();

    static void CheckKeyboardEntry(void *args);     // MUST BE FreeRTOS TASK
    static void KeyboardBehavior();

    static volatile bool _keyboardInterrupt;
    static volatile Peripheral::Keyboard::Button _keyboardInterruptButton;

    static void OpenMainMenu();

    static void ArmAlarm();

public:
    static PIR::PIRManager *pirManager;
    static Peripheral::Keyboard *keyboard;
    static Screen::LCD *lcd;
    static Screen::Interface *interface;

    static void InitializeTask(void *args);         // MUST BE FreeRTOS TASK
    static void KeyboardInterrupt(Peripheral::Keyboard::Button button);
};

#endif //ControlPanel_hpp
