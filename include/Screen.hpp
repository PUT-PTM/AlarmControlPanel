#ifndef SCREEN_HPP
#define SCREEN_HPP

#include "GPIO.hpp"
#include "Keyboard.hpp"

namespace Screen
{
    class LCD
    {
    private:
        GPIO::GPIOPins _RS;   // Registry Set ( 0 = Instruction Register (W) Busy Flag: address counter (R), 1 = Data Register (RW) )
        GPIO::GPIOPins _RW;   // Read/Write ( 0 = W, 1 = R)
        GPIO::GPIOPins _E;    // Clock, screen starts reading on falling edge

        GPIO::GPIOPins _D4;
        GPIO::GPIOPins _D5;
        GPIO::GPIOPins _D6;
        GPIO::GPIOPins _D7;

        void WaitForUnblock();
        
        void Write(bool RS, uint8_t data, bool checkForBlockFlag = true);
        void WriteHalf(bool RS, uint8_t data, bool checkForBlockFlag = true);

        uint8_t Read(bool RS);
	public:
		LCD(GPIO_TypeDef *peripheral = GPIOA, GPIO::Pin RS = GPIO::Pin::P0, GPIO::Pin RW = GPIO::Pin::P2, GPIO::Pin E = GPIO::Pin::P4, GPIO::Pin D4 = GPIO::Pin::P1, GPIO::Pin D5 = GPIO::Pin::P3, GPIO::Pin D6 = GPIO::Pin::P5, GPIO::Pin D7 = GPIO::Pin::P7);
        ~LCD();

        void WriteString(std::string str);
        void WriteChar(char character);

        void WriteStringAt(std::string str, bool line, uint8_t pos);
        void WriteCharAt(char character, bool line, uint8_t pos);

        void Clear();
        void Home();
        void SetDisplay(bool displayOn, bool cursorOn, bool cursorBlinkOn);
        void FunctionSet(bool lineCount, bool dotCount);
        void EntryMode(bool increment, bool displayShift);
        void SetCursorPosition(bool line, uint8_t pos);
	};

    struct Menu;

    struct MenuElement
    {
        enum class Type : uint8_t
        {
            None,
            Text,
            BoolSetting,
            InputSetting,
            Catalog
        };

        static const uint8_t maxDisplayString = 15;

        std::string name;
        std::string displayString;
        Type type;

        bool *boolSetting = 0;
        
        Menu *childMenu = 0;

        MenuElement();
        MenuElement(Type type, std::string name, std::string displayString);
        MenuElement(Type type, std::string name, std::string displayString, bool *boolSetting);
        MenuElement(Type type, std::string name, std::string displayString, Menu *childMenu);
    };

    struct Menu
    {
        std::string menuName;
        std::vector <MenuElement> positions;

        Menu *parentMenu = 0;

        uint8_t parentPosition = -1;
        uint8_t parentRowSelected = -1;

        Menu();
        Menu(std::string menuName, std::vector <MenuElement> positions);
        Menu(std::string menuName, std::vector <MenuElement> positions, Menu *parentMenu);
    };

    class Interface
    {
    public:
        enum class Mode : uint8_t
        {
            Idle = 0,
            Menu = 1,
            Input = 2
        };
        
    private:
        const uint8_t _width = 16;
        const uint8_t _height = 2;

        LCD *_screen;
        Mode _mode = Mode::Idle;

        Menu *_currentMenu;

        uint8_t _menuPosition = 0;
        uint8_t _rowSelected = 0;

        std::string _idleMessage = "SUPER CENTRALEX";

        std::string _inputComment = "";
        std::string _input = "";

        std::string GetMenuFirstRow();
        std::string GetMenuSecondRow();
        
    public:
        Interface(LCD *screen);        

        LCD *GetLCD();

        void SetMenu(Menu *menu);
        void SetMenu(Menu *menu, uint8_t menuPosition, uint8_t rowSelected);
        void SetMenuPosition(uint8_t position);
        void SetMode(Mode mode);

        uint8_t GetSelectedIndex();
        Mode GetMode();
        MenuElement *GetSelectedElement();

        void Redraw(bool onlyArrows = false);
        void DrawBoolValues();

        void ScrollUp();
        void ScrollDown();
        bool IsMenuRoot();
        
        void MoveUp();
        void MoveDown();

        void SetIdleMessage(std::string message);

        void SetInputComment(std::string comment);
        void AppendCharToInput(char character);
        void AppendCharToInput(Peripheral::Keyboard::Button button);
        void SetInput(std::string input);
        
        std::string GetInput();
    };
}

#endif // SCREEN_HPP
