#ifndef FUNCTION_STATE_HPP
#define FUNCTION_STATE_HPP

#include <functional>

/**
 * Stores function with CPU registers values.
 */
class FunctionState {
private:
    std::function<void()> function;
public: 
    FunctionState(std::function<void()>);
};
#endif // FUNCTION_STATE_HPP