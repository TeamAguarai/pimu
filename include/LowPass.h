namespace pimu 
{

template<typename T>
class LowPass 
{

private:
    T _alpha;      
    T _prevOutput;  
    bool _alphaDefined = false;

public:
    LowPass();  
    bool isAlphaDefined();
    void setInitialValue(T value);
    void setAlpha(T value);
    T filter(T input);
};
    
} // namespace pimu