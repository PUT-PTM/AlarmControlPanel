#include "Screen.hpp"
#include "main.hpp"

#include <string>

namespace Screen
{
    LCD::LCD(GPIO_TypeDef *peripheral, GPIO::Pin RS, GPIO::Pin RW, GPIO::Pin E, GPIO::Pin D4, GPIO::Pin D5, GPIO::Pin D6, GPIO::Pin D7)
    : _RS(peripheral, {RS}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low),
      _RW(peripheral, {RW}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low),
      _E(peripheral, {E},  GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low),
      _D4(peripheral, {D4}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low),
      _D5(peripheral, {D5}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low),
      _D6(peripheral, {D6}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low),
      _D7(peripheral, {D7}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low)
	{
		// GPIO Pins Initialization
        debug("Screen - Begin GPIO init\n");

        debug("Screen - End GPIO init\n");

        HAL_Delay(100);

        /** Special initialization **/

        debug("Screen - Function Set #1\n");
        this->WriteHalf(0, 0b00110000, false); // Function Set
        HAL_Delay(5);

        debug("Screen - Function Set #2\n");
        this->WriteHalf(0, 0b00110000, false); // Function Set
        HAL_Delay(1);

        debug("Screen - Function Set #3\n");
        this->WriteHalf(0, 0b00110000, false); // Function Set
        HAL_Delay(1);

        debug("Screen - Setting 4-bit mode\n");
        this->WriteHalf(0, 0b00100000, false); // Set 4 bit mode
        HAL_Delay(1);

        /** End special initialization **/

        /** Personalization **/

        debug("Screen - Begin Personalization\n");
        this->FunctionSet(1,1);
        this->SetDisplay(1,0,0);
        this->Clear();
        this->EntryMode(1,0);
        this->SetDisplay(1,0,0);
        debug("Screen - End Personalization\n");

        /** End personalization **/
    }

    LCD::~LCD()
    {
    }

    void LCD::WaitForUnblock()
    {
        while(this->Read(0) & 0b10000000) { }
    }

    void LCD::Write(bool RS, uint8_t data, bool checkForBlockFlag)
    {
        WriteHalf(RS, data, checkForBlockFlag);

        _E.set_state(true);

        _D4.set_state(data & 0b00000001); //1
        _D5.set_state(data & 0b00000010); //2
        _D6.set_state(data & 0b00000100); //4
        _D7.set_state(data & 0b00001000); //8

        _E.set_state(false);
    }

    void LCD::WriteHalf(bool RS, uint8_t data, bool checkForBlockFlag)
    {
        if(checkForBlockFlag)
            WaitForUnblock();

        _E.set_state(true);

        _RS.set_state(RS);
        _RW.set_state(false); //Write mode

        _D4.set_state(data & 0b00010000); //16
        _D5.set_state(data & 0b00100000); //32
        _D6.set_state(data & 0b01000000); //64
        _D7.set_state(data & 0b10000000); //128

        _E.set_state(false);
    }

    void LCD::WriteString(std::string str)
    {
        for (const char &chr : str)
        {
            this->Write(1, chr);
            HAL_Delay(1);
        }
    }

    void LCD::WriteChar(char character)
    {
        this->Write(1, character);
        HAL_Delay(1);
    }

    void LCD::WriteStringAt(std::string str, bool line, uint8_t pos)
    {
        this->SetCursorPosition(line, pos);
        this->WriteString(str);
    }

    void LCD::WriteCharAt(char character, bool line, uint8_t pos)
    {
        this->SetCursorPosition(line, pos);
        this->WriteChar(character);
    }

    uint8_t LCD::Read(bool RS)
    {
        uint8_t result = 0;

        _RS.set_state(RS);
        _RW.set_state(true); //Read mode

        _D4.set_state(false);
        _D5.set_state(false);
        _D6.set_state(false);
        _D7.set_state(false);

        _E.set_state(false);

        result |= (_D4.get_state() * 0b1000);
        result |= (_D5.get_state() * 0b1000);
        result |= (_D6.get_state() * 0b1000);
        result |= (_D7.get_state() * 0b1000);

        // Wait ?
        HAL_Delay(1);

        _E.set_state(true);

        _E.set_state(false);

        result |= _D4.get_state();
        result |= _D5.get_state();
        result |= _D6.get_state();
        result |= _D7.get_state();

        return result;
    }

    void LCD::Clear()
    {
        LCD::Write(0, 0b00000001);
    }

    void LCD::Home()
    {
        LCD::Write(0, 0b00000010);
    }

    void LCD::SetDisplay(bool displayOn, bool cursorOn, bool cursorBlinkOn)
    {
        //0b00001DCB
        uint8_t data = 0b1000 + (displayOn * 0b100) + (cursorOn * 0b10) + cursorBlinkOn;
        this->Write(0, data);
    }

    void LCD::FunctionSet(bool lineCount, bool dotCount)
    {
        //0b0010NF00
        uint8_t data = 0b100000 + (lineCount * 0b1000) + (dotCount * 0b100);
        this->Write(0, data);
    }

    void LCD::EntryMode(bool increment, bool displayShift)
    {
        //0b000001(I/D)S
        uint8_t data = 0b100 + (increment * 0b10) + displayShift;
        this->Write(0, data);
    }

    void LCD::SetCursorPosition(bool line, uint8_t pos)
    {
        if(pos > 0b111111) return;
        this->Write(0, 0b10000000 + (line * 0b1000000) + pos);
    }

    MenuElement::MenuElement()
    :   type(Type::None), name(""), displayString("") {}

    MenuElement::MenuElement(Type type, std::string name, std::string displayString)
    :   type(type), name(name), displayString(displayString)
    {
        if(displayString.length() > maxDisplayString)
            displayString = displayString.substr(maxDisplayString);
    }

    MenuElement::MenuElement(Type type, std::string name, std::string displayString, bool *boolSetting)
    :   type(type), name(name), displayString(displayString), boolSetting(boolSetting)
    {
        if(displayString.length() > maxDisplayString - 2)
            displayString = displayString.substr(maxDisplayString);
    }

    MenuElement::MenuElement(Type type, std::string name, std::string displayString, Menu *childMenu)
    :   type(type), name(name), displayString(displayString), childMenu(childMenu)
    {
        if(displayString.length() > maxDisplayString)
            displayString = displayString.substr(maxDisplayString);
    }

    Menu::Menu()
    :   menuName(""), parentMenu(0) {}

    Menu::Menu(std::string menuName, std::vector <MenuElement> positions)
    :   menuName(menuName), positions(positions), parentMenu(0) {}

    Menu::Menu(std::string menuName, std::vector <MenuElement> positions, Menu *parentMenu)
    :   menuName(menuName), positions(positions), parentMenu(parentMenu) {}

    Interface::Interface(LCD *screen) : _screen(screen)
    {
        Redraw();
    }

    std::string Interface::GetMenuFirstRow()
    {
        if(_currentMenu->positions.size() == 0) return "";
        return _currentMenu->positions[_menuPosition].displayString;
    }

    std::string Interface::GetMenuSecondRow()
    {
        if(_currentMenu->positions.size() < 2) return "";

        if(_menuPosition == _currentMenu->positions.size() - 1)
            return _currentMenu->positions[0].displayString;
        else return _currentMenu->positions[_menuPosition+1].displayString;
    }

    LCD *Interface::GetLCD()
    {
        return _screen;
    }

    void Interface::SetMenu(Menu *menu)
    {
        _currentMenu = menu;

        debug("Menu set.\n");
        if(_mode == Mode::Menu) Redraw();
    }

    void Interface::SetMenu(Menu *menu, uint8_t menuPosition, uint8_t rowSelected)
    {
        _currentMenu = menu;

        debug("Menu set.\n");
        _menuPosition = menuPosition;
        _rowSelected = rowSelected;
        if(_mode == Mode::Menu) Redraw();
    }

    void Interface::SetMenuPosition(uint8_t position)
    {
        _menuPosition = position;
        Redraw();
    }

    void Interface::SetMode(Mode mode)
    {
        if(_mode == mode) return;
        if(mode == Mode::Menu && _currentMenu == 0) return;

        _mode = mode;
        Redraw();
        switch(_mode)
        {
            case Mode::Idle:
            case Mode::Menu:
                _screen->SetDisplay(1,0,0);
                break;
            case Mode::Input:
                _screen->SetDisplay(1,1,1);
                break;
        }
    }

    uint8_t Interface::GetSelectedIndex()
    {
        //debug("MenuPos: %d, MenuArrayLength: %d, RowSelected: %d\n", _menuPosition, _menuArrayLength, _rowSelected);
        return _menuPosition + _rowSelected == _currentMenu->positions.size() ? 0 : _menuPosition + _rowSelected;
    }

    Interface::Mode Interface::GetMode()
    {
        return _mode;
    }

    MenuElement *Interface::GetSelectedElement()
    {
        if(_menuPosition + _rowSelected == _currentMenu->positions.size())
            return &_currentMenu->positions[0];
        else
            return &_currentMenu->positions[_menuPosition + _rowSelected];
    }

    void Interface::ScrollUp()
    {
        if(_currentMenu->positions.size() < 2) return;
        
        switch(_rowSelected)
        {
            case 0:
                if(_menuPosition == 0)
                    _menuPosition = _currentMenu->positions.size() - 1;
                else
                    _menuPosition--;
                Redraw();
                break;
            case 1:
                _rowSelected--;
                Redraw(true);
                break;
        }
    }

    void Interface::ScrollDown()
    {
        if(_currentMenu->positions.size() < 2) return;

        switch(_rowSelected)
        {
            case 0:
                _rowSelected++;
                Redraw(true);
                break;
            case 1:
                if(_menuPosition == _currentMenu->positions.size() - 1)
                    _menuPosition = 0;
                else
                    _menuPosition++;
                Redraw();
                break;
        }
    }

    bool Interface::IsMenuRoot()
    {
        if(_currentMenu->parentMenu == 0) return true;
        else return false;
    }

    void Interface::MoveUp()
    {
        if(_currentMenu->parentMenu == 0) return;
        SetMenu(_currentMenu->parentMenu, _currentMenu->parentPosition, _currentMenu->parentRowSelected);
    }

    void Interface::MoveDown()
    {
        MenuElement * selectedElem = GetSelectedElement();
        if(selectedElem->childMenu == 0) return;

        selectedElem->childMenu->parentPosition = _menuPosition;
        selectedElem->childMenu->parentRowSelected = _rowSelected;

        SetMenu(selectedElem->childMenu, 0, 0);
    }

    void Interface::SetIdleMessage(std::string message)
    {
        _idleMessage = message;
        if(_mode == Mode::Idle) Redraw();
    }

    void Interface::Redraw(bool onlyArrows)
    {
        switch(_mode)
        {
            case Mode::Idle:
                _screen->Clear();
                _screen->WriteStringAt(_idleMessage, 0, 0);
                break;
            case Mode::Input:
                _screen->Clear();
                _screen->WriteStringAt(_inputComment, 0, 0);
                _screen->WriteStringAt(_input, 1, 0);
                break;
            case Mode::Menu:
            {
                if(!onlyArrows)
                {
                    _screen->Clear();
                    _screen->WriteStringAt(GetMenuFirstRow(), 0, 1);
                    _screen->WriteStringAt(GetMenuSecondRow(), 1, 1);
                }

                _screen->WriteCharAt('>', _rowSelected, 0);
                _screen->WriteCharAt(' ', !_rowSelected, 0);

                DrawBoolValues();
            }
        }
    }

    void Interface::DrawBoolValues()
    {
        if(_mode != Mode::Menu) return;

        if(_currentMenu->positions.size() < 1) return;

        if(_currentMenu->positions[_menuPosition].type == MenuElement::Type::BoolSetting)
        {
            bool value1 = *_currentMenu->positions[_menuPosition].boolSetting;
            _screen->WriteCharAt(value1 ? 'T' : 'F', 0, 15);
        }

        if(_currentMenu->positions.size() < 2) return;

        bool value2;
        if(_menuPosition == _currentMenu->positions.size() - 1)
            if(_currentMenu->positions[0].type == MenuElement::Type::BoolSetting)
                value2 = *_currentMenu->positions[0].boolSetting;
            else return;
        else if(_currentMenu->positions[_menuPosition+1].type == MenuElement::Type::BoolSetting)
        {
            value2 = *_currentMenu->positions[_menuPosition+1].boolSetting;
        }
        else return;
        
        _screen->WriteCharAt(value2 ? 'T' : 'F', 1, 15);
    }

    void Interface::SetInputComment(std::string comment)
    {
        _inputComment = comment;
        if(_mode == Mode::Input) Redraw();
    }

    void Interface::SetInput(std::string input)
    {
        _input = input;
        if(_mode == Mode::Input) Redraw();
    }

    std::string Interface::GetInput()
    {
        return _input;
    }

    void Interface::AppendCharToInput(char character)
    {
        _screen->WriteCharAt(character, 1, _input.length());
        _input = _input + character;
    }    

    void Interface::AppendCharToInput(Peripheral::Keyboard::Button button)
    {
        switch(button)
        {
            case Peripheral::Keyboard::Button::B0:
                AppendCharToInput('0');
                break;
            case Peripheral::Keyboard::Button::B1:
                AppendCharToInput('1');
                break;
            case Peripheral::Keyboard::Button::B2:
                AppendCharToInput('2');
                break;
            case Peripheral::Keyboard::Button::B3:
                AppendCharToInput('3');
                break;
            case Peripheral::Keyboard::Button::B4:
                AppendCharToInput('4');
                break;
            case Peripheral::Keyboard::Button::B5:
                AppendCharToInput('5');
                break;
            case Peripheral::Keyboard::Button::B6:
                AppendCharToInput('6');
                break;
            case Peripheral::Keyboard::Button::B7:
                AppendCharToInput('7');
                break;
            case Peripheral::Keyboard::Button::B8:
                AppendCharToInput('8');
                break;
            case Peripheral::Keyboard::Button::B9:
                AppendCharToInput('9');
                break;
            case Peripheral::Keyboard::Button::BAsterisk:
                AppendCharToInput('*');
                break;
            case Peripheral::Keyboard::Button::BHash:
                AppendCharToInput('#');
                break;
            case Peripheral::Keyboard::Button::BA:
                AppendCharToInput('A');
                break;
            case Peripheral::Keyboard::Button::BB:
                AppendCharToInput('B');
                break;
            case Peripheral::Keyboard::Button::BC:
                AppendCharToInput('C');
                break;
            case Peripheral::Keyboard::Button::BD:
                AppendCharToInput('D');
                break;
            default:
                return;
        }
    }
}

