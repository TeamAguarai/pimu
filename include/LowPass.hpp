#include <stdexcept>

namespace pimu {

template<typename T>
class LowPass {
private:
    T _alpha;      
    T prev_output_;  
    bool alpha_defined_ = false;

public:
    LowPass();

    void setAlpha(T value);
    void setInitialValue(T value);
    bool isAlphaDefined();
    T filter(T input);
};

/* Constructor: define el tipo de dato que se usará */
template<typename T>
LowPass<T>::LowPass() : prev_output_(static_cast<T>(0)) {}

/* Establece el coeficiente de suavizado */
template<typename T>
void LowPass<T>::setAlpha(T value) {
    if (value < static_cast<T>(0) || value > static_cast<T>(1)) {
        throw std::invalid_argument("The smoothing coefficient must be in the range [0,1].");
    }
    this->alpha_defined_ = true;
    this->_alpha = value;
}

/* Establece el valor inicial del filtro */
template<typename T>
void LowPass<T>::setInitialValue(T value) {
    this->prev_output_ = value;
}

/* Verifica si el coeficiente alpha ha sido definido */
template<typename T>
bool LowPass<T>::isAlphaDefined() {
    return this->alpha_defined_;
}

/* Aplica el filtro pasa-bajos al valor de entrada */
template<typename T>
T LowPass<T>::filter(T input) {
    if (!this->alpha_defined_) {
        throw std::runtime_error("The smoothing coefficient has not been initialized or has an invalid value.");
    }

    T output = this->_alpha * input + (static_cast<T>(1) - this->_alpha) * this->prev_output_;
    this->prev_output_ = output;
    return output;
}

} // namespace pimu
