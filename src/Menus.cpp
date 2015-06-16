#include "Menus.hpp"
#include "ControlPanel.hpp"

namespace Menus
{
    Screen::Menu mainMenu
    (
        "mainMenu",
        {
            Screen::MenuElement(Screen::MenuElement::Type::Catalog, "pirSettings", "Sensors", &pirMenu),
            Screen::MenuElement(Screen::MenuElement::Type::InputSetting, "armTime", "Arm time"),
            Screen::MenuElement(Screen::MenuElement::Type::InputSetting, "disarmTime", "Disarm time"),
            Screen::MenuElement(Screen::MenuElement::Type::InputSetting, "pin", "Pincode")
        }
    );

    Screen::Menu pirMenu
    (
        "pirSettings",
        {
            Screen::MenuElement(Screen::MenuElement::Type::Catalog, "pir1settings", "Sensor #1", &pir1Menu),
            Screen::MenuElement(Screen::MenuElement::Type::Catalog, "pir2settings", "Sensor #2", &pir2Menu),
            Screen::MenuElement(Screen::MenuElement::Type::Catalog, "pir3settings", "Sensor #3", &pir3Menu)
        }
    );

    Screen::Menu pir1Menu
    (
        "pir1Settings",
        {
            Screen::MenuElement(Screen::MenuElement::Type::BoolSetting, "pir1enabled", "Enabled", &ControlPanel::settings.P1Settings.Enabled),
            Screen::MenuElement(Screen::MenuElement::Type::BoolSetting, "pir1delayed", "Delayed", &ControlPanel::settings.P1Settings.Delayed)
        }
    );

    Screen::Menu pir2Menu
    (
        "pir2Settings",
        {
            Screen::MenuElement(Screen::MenuElement::Type::BoolSetting, "pir1enabled", "Enabled", &ControlPanel::settings.P2Settings.Enabled),
            Screen::MenuElement(Screen::MenuElement::Type::BoolSetting, "pir1delayed", "Delayed", &ControlPanel::settings.P2Settings.Delayed)
        }
    );

    Screen::Menu pir3Menu
    (
        "pir3Settings",
        {
            Screen::MenuElement(Screen::MenuElement::Type::BoolSetting, "pir1enabled", "Enabled", &ControlPanel::settings.P3Settings.Enabled),
            Screen::MenuElement(Screen::MenuElement::Type::BoolSetting, "pir1delayed", "Delayed", &ControlPanel::settings.P3Settings.Delayed)
        }
    );
}
