#ifdef USING_VSCODE_AS_EDITOR
    #include "LowPass.h"
#endif


namespace pimu {

template<typename T>
LowPass<T>::LowPass() : _prevOutput(static_cast<T>(0)) {} // define el tipo de dato que se usara

template<typename T>
void LowPass<T>::setAlpha(T value) {
    if (value < static_cast<T>(0) || value > static_cast<T>(1)) {
        throw std::invalid_argument("The smoothing coefficient must be in the range [0,1].");
    }
    this->_alphaDefined = true;
    this->_alpha = value;
}

template<typename T>
void LowPass<T>::setInitialValue(T value) {
    this->_prevOutput = value;
}

template<typename T>
bool LowPass<T>::isAlphaDefined() {
    return this->_alphaDefined;
}

template<typename T>
T LowPass<T>::filter(T input) {
    if (!this->_alphaDefined) {
        throw std::runtime_error("The smoothing coefficient has not been initialized or has an invalid value.");
    }

    T output = this->_alpha * input + (static_cast<T>(1) - this->_alpha) * this->_prevOutput;
    this->_prevOutput = output;
    return output;
}

template class LowPass<double>;
template class LowPass<float>;
template class LowPass<int>;

} // namespace pimu
