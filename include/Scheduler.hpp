#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <functional>

/**
 * Implements simple scheduling mechanism.
 */
class Scheduler {
public: 
    Scheduler();
    /**
     * Adds function to the scheduler queue.
     * @param function_to_schedule retrieved with std::bind(function, containing_object)
     */
    void add_function(std::function<void()> function_to_schedule);
    
    /**
     * Starts scheduling.
     */
    void start();
    
    void read_registers() {
        register int LR asm("r14");
    }
};
#endif // SCHEDULER_HPP