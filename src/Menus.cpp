#include "Menus.hpp"

namespace Menus
{
    bool test = true;

    Screen::Menu mainMenu
    (
        "mainMenu",
        {
            Screen::MenuElement(Screen::MenuElement::Type::Text, "pos1", "Pozycja 1"),
            Screen::MenuElement(Screen::MenuElement::Type::Catalog, "pos2", "Pozycja 2", &subMenu),
            Screen::MenuElement(Screen::MenuElement::Type::InputSetting, "pos3", "Pozycja 3"),
            Screen::MenuElement(Screen::MenuElement::Type::BoolSetting, "pos4", "Pozycja 4", &test),
        }
    );

    Screen::Menu subMenu
    {
        "subMenu",
        {
            Screen::MenuElement(Screen::MenuElement::Type::Text, "spos1", "Subpos 1"),
            Screen::MenuElement(Screen::MenuElement::Type::Text, "spos2", "Subpos 2"),
            Screen::MenuElement(Screen::MenuElement::Type::Text, "spos3", "Subpos 3"),
            Screen::MenuElement(Screen::MenuElement::Type::Text, "spos4", "Subpos 4"),
        },
        &mainMenu
    };
}
