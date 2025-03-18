#include <stdexcept>

namespace pimu 
{

template<typename T>
class LowPass 
{

private:
    T _alpha;      
    T prev_output_;  
    bool alpha_defined_ = false;

public:
    LowPass() : prev_output_(static_cast<T>(0)) {} // define el tipo de dato que se usara

    void setAlpha(T value) {
        if (value < static_cast<T>(0) || value > static_cast<T>(1)) {
            throw std::invalid_argument("The smoothing coefficient must be in the range [0,1].");
        }
        this->alpha_defined_ = true;
        this->_alpha = value;
    }

    void setInitialValue(T value) {
        this->prev_output_ = value;
    }

    bool isAlphaDefined() {
        return this->alpha_defined_;
    }

    T filter(T input) {
        if (!this->alpha_defined_) {
            throw std::runtime_error("The smoothing coefficient has not been initialized or has an invalid value.");
        }

        T output = this->_alpha * input + (static_cast<T>(1) - this->_alpha) * this->prev_output_;
        this->prev_output_ = output;
        return output;
    }

};

} // namespace pimu