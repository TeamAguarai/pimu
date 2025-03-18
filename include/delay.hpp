#include <unistd.h>

namespace pimu {

/* stops the program execution for a duration in miliseconds */
void delay(int duration_miliseconds){
    while(duration_miliseconds--){
        usleep(1000);
    }
}
} // namespace pimu