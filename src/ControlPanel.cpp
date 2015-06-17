#include "ControlPanel.hpp"
#include <string>

class interface;
uint8_t                 ControlPanel::_pirStates = 0;
Settings                ControlPanel::settings;

PIR::PIRManager*        ControlPanel::pirManager = 0;
Peripheral::Keyboard*   ControlPanel::keyboard = 0;
Screen::LCD*            ControlPanel::lcd = 0;
Screen::Interface*      ControlPanel::interface = 0;

bool                    ControlPanel::_isInMenu = false;
Leds                    ControlPanel::_greenLed({GPIO::Pin::P12});
Leds                    ControlPanel::_orangeLed({GPIO::Pin::P13});
Leds                    ControlPanel::_redLed({GPIO::Pin::P14});
Leds                    ControlPanel::_blueLed({GPIO::Pin::P15});

volatile bool                           ControlPanel::_keyboardInterrupt = false;
volatile ControlPanel::State            ControlPanel::_state = ControlPanel::State::Disarmed;
volatile Peripheral::Keyboard::Button   ControlPanel::_keyboardInterruptButton = Peripheral::Keyboard::Button::None;

Settings::Settings()
{
    P1Settings.id = 1;
    P2Settings.id = 2;
    P3Settings.id = 3;
}

void Settings::SetSetting(std::string settingName, std::string settingValue)
{
    std::stringstream ss;
    ss.str(settingValue);

    if(settingName == "armTime")
        ss >> ArmDelay;
    else if(settingName == "disarmTime")
        ss >> DisarmTime;
    else if(settingName == "pin")
        ss >> Pin;
}

std::string Settings::GetSetting(std::string settingName)
{
    char buffer[4];
    if(settingName == "armTime")
        ext::itoa(ArmDelay, buffer, 10);
    else if(settingName == "disarmTime")
        ext::itoa(DisarmTime, buffer, 10);
    else if(settingName == "pin")
        return Pin;
        
    return std::string(buffer);
}

/* Initializations */

void ControlPanel::InitializeLCD()
{
    debug("Initializing LCD...\n");

    lcd = new Screen::LCD
        (GPIOE,             // GPIOE Peripheral
        GPIO::Pin::P1,      // Registry Set (RS)
        GPIO::Pin::P3,      // Read/Write (RW)
        GPIO::Pin::P5,      // Clock (E)
        GPIO::Pin::P0,      // Data 4
        GPIO::Pin::P2,      // Data 5
        GPIO::Pin::P4,      // Data 6
        GPIO::Pin::P6);     // Data 7

    debug("LCD initialization successful.\n");
}

void ControlPanel::InitializeInterface()
{
    debug("Initializing interface...\n");

    interface = new Screen::Interface(lcd);
    
    debug("Interface initialization successful.\n");
}

void ControlPanel::InitializePirManager()
{
    debug("Initializing PIR manager...\n");

    pirManager = new PIR::PIRManager;

    debug("PIR manager initialization successful.\n");
}

void ControlPanel::InitializeKeyboard()
{
    debug("Initializing keyboard...\n");

    keyboard = new Peripheral::Keyboard
        (GPIOE,             // GPIOE Peripheral
        GPIO::Pin::P11,     // Row 0
        GPIO::Pin::P12,     // Row 1
        GPIO::Pin::P13,     // Row 2
        GPIO::Pin::P14,     // Row 3
        GPIO::Pin::P7,      // Column 0
        GPIO::Pin::P8,      // Column 1
        GPIO::Pin::P9,      // Column 2
        GPIO::Pin::P10);    // Column 3

    debug("Interface: Creating task for keyboard interrupt checking...\n");
    // Task needed for checking if a button has been pressed on the keyboard.
    xTaskCreate(CheckKeyboardEntry, "KeyboardInterruptCheck", 1000, NULL, 3, NULL);

    debug("Keyboard initialization successful.\n");
}

void ControlPanel::InitializeTask(void *args)
{
    debug("Initializing control panel...\n");

    InitializeLCD();
    InitializeInterface();
    InitializePirManager();
    InitializeKeyboard();

    debug("Starting idle updater task...\n");
    xTaskCreate(UpdateIdleInformation, "IdleUpdater", 1000, NULL, 4, NULL);

    debug("Control panel initialization successful.\n");

    vTaskDelete(NULL);
}

/* End initializations */

void ControlPanel::UpdateIdleInformation(void *args)
{
    while(true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if(_pirStates & 1)
        {
            if(!pirManager->GetPirState(1))
            {
                lcd->WriteCharAt(' ', 1, 2);
                _pirStates ^= 1;
                _greenLed.set_state(false);
            }
        }

        if(_pirStates & 2)
        {
            if(!pirManager->GetPirState(2))
            {
                lcd->WriteCharAt(' ', 1, 6);
                _pirStates ^= 1;
                _greenLed.set_state(false);
            }
        }

        if(_pirStates & 4)
        {
            if(!pirManager->GetPirState(3))
            {
                lcd->WriteCharAt(' ', 1, 10);
                _pirStates ^= 1;
                _greenLed.set_state(false);
            }
        }
    }
}

void ControlPanel::CheckKeyboardEntry(void *args)
{
    while(ControlPanel::interface == 0) {}

    while(true)
    {
        // Wait for interrupt flag
        while(!ControlPanel::_keyboardInterrupt) {}

        debug("Keyboard button pressed and catched.\n");
        //interface->AppendCharToInput(ControlPanel::_keyboardInterruptButton);
        KeyboardBehavior();
        ControlPanel::_keyboardInterrupt = false;
    }
}

void ControlPanel::KeyboardBehavior()
{
    using namespace Screen;
    using namespace Peripheral;

    switch(interface->GetMode())
    {
        case Interface::Mode::Idle:
        {
            switch(_keyboardInterruptButton)
            {
                case Keyboard::Button::BA:
                case Keyboard::Button::BB:
                case Keyboard::Button::BC:
                case Keyboard::Button::BD:
                case Keyboard::Button::BAsterisk:
                case Keyboard::Button::BHash:
                    break;
                default:
                    interface->SetMode(Interface::Mode::Input);
                    KeyboardBehavior();
                    break;
            }
            
            break;
        }

        case Interface::Mode::Input:
        {
            switch(_keyboardInterruptButton)
            {
                case Keyboard::Button::BAsterisk:
                    if(!_isInMenu && _state == State::Disarmed)
                        OpenMainMenu();
                    else if(_isInMenu && _state == State::Disarmed)
                        //Discard input
                        interface->SetMode(Interface::Mode::Menu);
                    break;

                case Keyboard::Button::BHash:
                    if(!_isInMenu && _state == State::Disarmed)
                        ArmAlarm();
                    else if(!_isInMenu)
                        DisarmAlarm();
                    else
                    {
                        interface->SetMode(Interface::Mode::Menu);
                        //Value accepted, assign it or something
                        settings.SetSetting(interface->GetSelectedElement()->name, interface->GetInput());
                    }
                    break;

                case Keyboard::Button::BA:
                case Keyboard::Button::BB:
                case Keyboard::Button::BC:
                    break;

                case Keyboard::Button::BD:
                    interface->InputBackspace();
                    break;
                    
                default:
                    interface->AppendCharToInput(_keyboardInterruptButton);
                    break;
            }
            break;
        }
            
        case Interface::Mode::Menu:
        {
            switch(_keyboardInterruptButton)
            {
                case Keyboard::Button::B2:
                    interface->ScrollUp();
                    break;

                case Keyboard::Button::B8:
                    interface->ScrollDown();
                    break;

                case Keyboard::Button::BAsterisk:
                    if(interface->IsMenuRoot())
                    {
                        interface->SetMode(Interface::Mode::Idle);
                        _isInMenu = false;
                    }
                    else
                        interface->MoveUp();
                    break;

                case Keyboard::Button::BHash:
                {
                    MenuElement * currentElement = interface->GetSelectedElement();
                    switch(currentElement->type)
                    {
                        case MenuElement::Type::Catalog:
                            debug("Moving down...\n");
                            interface->MoveDown();
                            break;
                        case MenuElement::Type::BoolSetting:
                            if(currentElement->boolSetting != 0)
                            {
                                *currentElement->boolSetting = !(*currentElement->boolSetting);
                                interface->DrawBoolValues();
                            }
                            break;
                        case MenuElement::Type::InputSetting:
                        {
                            //Entering input mode, should insert here value of edited setting
                            //interface->SetInput(...):
                            interface->SetInput(settings.GetSetting(currentElement->name));
                            interface->SetMode(Interface::Mode::Input);
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                    
                default:
                    break;
            }
            break;
        }
        
        case Interface::Mode::Message:
        {
            if(_isInMenu)
                interface->DisposeMessage();
            else
                interface->DisposeMessage(Interface::Mode::Idle);
            break;
        }
        default:
            break;
    }
}

void ControlPanel::KeyboardInterrupt(Peripheral::Keyboard::Button button)
{
    _keyboardInterruptButton = button;
    _keyboardInterrupt = true;
}

void ControlPanel::OpenMainMenu()
{
    debug("Trying to open menu...\n");
    if(interface->GetInput() != settings.Pin)
    {
        interface->DisplayMessage("Wrong pin!");
        interface->SetInput("");
        return;
    }
    _isInMenu = true;
    interface->SetMenu(&Menus::mainMenu);
    interface->SetMode(Screen::Interface::Mode::Menu);
    interface->SetInput("");
}

void ControlPanel::ArmAlarm()
{
    if(interface->GetInput() != settings.Pin)
    {
        interface->DisplayMessage("Wrong pin!");
        interface->SetInput("");
        return;
    }

    debug("Arming...\n");
    _state = State::Arming;
    interface->SetMode(Screen::Interface::Mode::Idle);
    xTaskCreate(ArmTask, "ArmTask", 500, NULL, 4, NULL);
}

void ControlPanel::DisarmAlarm()
{
    if(interface->GetInput() != settings.Pin)
    {
        interface->DisplayMessage("Wrong pin!");
        interface->SetInput("");
        return;
    }

    debug("Disarmed.\n");
    _state = State::Disarmed;
    _redLed.set_state(false);
    _orangeLed.turn_off();
    interface->DisplayMessage("Disarmed.");
}

void ControlPanel::ArmTask(void *args)
{
    char iter[3];
    interface->DisplayMessage("");
    for(int i=0; i<settings.ArmDelay; i++)
    {
        if(_state == State::Disarmed)
        {
            vTaskDelete(NULL);
        }
        debug("Arming time: %d\n", i);
        if(interface->GetMode() == Screen::Interface::Mode::Message)
        {
            ext::itoa(settings.ArmDelay-i, iter, 10);
            interface->DisplayMessage(std::string("To exit: ") + std::string(iter) + std::string("s"));
        }
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }

    debug("Armed.\n");
    interface->DisplayMessage("Armed.");
    _orangeLed.turn_on();
    _state = State::Armed;
    vTaskDelete(NULL);
}

void ControlPanel::EnterTimeTask(void *args)
{
    char iter[3];
    interface->DisplayMessage("");
    for(int i=0; i<settings.DisarmTime; i++)
    {
        if(_state == State::Disarmed)
        {
            debug("Disarmed.\n");
            vTaskDelete(NULL);
            return;
        }
        else if(_state == State::Alarm)
        {
            vTaskDelete(NULL);
            return;
        }
        
        debug("Enter time: %d\n", i);
        if(interface->GetMode() == Screen::Interface::Mode::Message)
        {
            ext::itoa(settings.DisarmTime-i, iter, 10);
            interface->DisplayMessage(std::string("Time left: ") + std::string(iter) + std::string("s"));
        }
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }

    Alarm();
    vTaskDelete(NULL);
}

void ControlPanel::PirMovement(int pirId)
{
    _greenLed.set_state(true);
    switch(pirId)
    {
        case 1:
        {
            if(settings.P1Settings.Enabled)
            {
                _pirStates |= 1;
                if(interface->GetMode() == Screen::Interface::Mode::Idle) lcd->WriteCharAt('M', 1, 2);

                if(_state == State::Armed)
                {
                    if(settings.P1Settings.Delayed)
                    {
                        _state = State::EntryArmed;
                        xTaskCreate(EnterTimeTask, "EnterTime", 500, NULL, 4, NULL);
                    }
                    else
                    {
                        Alarm();
                    }
                }
            }

            break;
        }
        case 2:
        {
            if(settings.P2Settings.Enabled)
            {
                _pirStates |= 2;
                if(interface->GetMode() == Screen::Interface::Mode::Idle) lcd->WriteCharAt('M', 1, 6);

                if(_state == State::Armed)
                {
                    if(settings.P2Settings.Delayed)
                    {
                        _state = State::EntryArmed;
                        xTaskCreate(EnterTimeTask, "EnterTime", 500, NULL, 4, NULL);
                    }
                    else
                    {
                        Alarm();
                    }
                }
            }
            break;
            
        }
        case 3:
        {
            if(settings.P3Settings.Enabled)
            {
                _pirStates |= 4;
                if(interface->GetMode() == Screen::Interface::Mode::Idle) lcd->WriteCharAt('M', 1, 10);

                if(_state == State::Armed)
                {
                    if(settings.P2Settings.Delayed)
                    {
                        _state = State::EntryArmed;
                        xTaskCreate(EnterTimeTask, "EnterTime", 500, NULL, 4, NULL);
                    }
                    else
                    {
                        Alarm();
                    }
                }
            }
            break;
        }
    }
}

void ControlPanel::Alarm()
{
    _state = State::Alarm;
    _redLed.turn_on();
    debug("Alarm!\n");
    interface->DisplayMessage("Alarm!");
}
