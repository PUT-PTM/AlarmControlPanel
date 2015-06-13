#include "ControlPanel.hpp"

std::string             ControlPanel::_temporaryPin = "1";

PIR::PIRManager*        ControlPanel::pirManager = 0;
Peripheral::Keyboard*   ControlPanel::keyboard = 0;
Screen::LCD*            ControlPanel::lcd = 0;
Screen::Interface*      ControlPanel::interface = 0;

bool                    ControlPanel::_isInMenu = false;
bool                    ControlPanel::_isArmed = false;

volatile bool                           ControlPanel::_keyboardInterrupt = false;
volatile Peripheral::Keyboard::Button   ControlPanel::_keyboardInterruptButton = Peripheral::Keyboard::Button::None;

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
    xTaskCreate(CheckKeyboardEntry, "KeyboardInterruptCheck", 1000, NULL, 4, NULL);

    debug("Keyboard initialization successful.\n");
}

void ControlPanel::InitializeTask(void *args)
{
    debug("Initializing control panel...\n");

    InitializeLCD();
    InitializeInterface();
    InitializePirManager();
    InitializeKeyboard();

    debug("Control panel initialization successful.\n");

    vTaskDelete(NULL);
}

/* End initializations */

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
            interface->SetMode(Interface::Mode::Input);
            KeyboardBehavior();
            break;

        case Interface::Mode::Input:
            switch(_keyboardInterruptButton)
            {
                case Keyboard::Button::BAsterisk:
                    if(!_isInMenu)
                    {
                        _isInMenu = true;
                        OpenMainMenu();
                    }
                    else
                        interface->SetMode(Interface::Mode::Menu);
                    break;

                case Keyboard::Button::BHash:
                    if(!_isInMenu)
                        ArmAlarm();
                    else
                    {
                        interface->SetMode(Interface::Mode::Menu);
                        //Value accepted, assign it or something
                    }
                    break;

                case Keyboard::Button::BA:
                case Keyboard::Button::BB:
                case Keyboard::Button::BC:
                case Keyboard::Button::BD:
                    break;

                default:
                    interface->AppendCharToInput(_keyboardInterruptButton);
                    break;
            }
            break;
            
        case Interface::Mode::Menu:
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
                        interface->SetMode(Interface::Mode::Idle);
                    else
                        interface->MoveUp();
                    break;

                case Keyboard::Button::BHash:
                {
                    MenuElement * currentElement = interface->GetSelectedElement();
                    switch(currentElement->type)
                    {
                        case MenuElement::Type::Catalog:
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
                            interface->SetInput("");
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
}

void ControlPanel::KeyboardInterrupt(Peripheral::Keyboard::Button button)
{
    _keyboardInterruptButton = button;
    _keyboardInterrupt = true;
}

void ControlPanel::OpenMainMenu()
{
    if(interface->GetInput() != _temporaryPin)
    {
        return;
    }
    
    interface->SetMenu(&Menus::mainMenu);
    interface->SetMode(Screen::Interface::Mode::Menu);
}

void ControlPanel::ArmAlarm()
{
    
}
