#include <stdexcept>

namespace ext {
    template<class InputIterator, class T>
    InputIterator find (InputIterator first, InputIterator last, const T& value) {
        for (; first != last; ++first) {
            if (*first == value) 
                return first;
        }
        throw std::out_of_range("Value not in given range");
    }
}