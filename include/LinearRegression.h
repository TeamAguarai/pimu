namespace pimu
{
    
class LinearRegression {
private:
    int numPoints;      
    float xSum;        
    float ySum;        
    float xSquaredSum; 
    float xTimesYSum;  
    float slope;       
    float intercept;   
    bool computed;      

public:
    LinearRegression(); 

    void addDataPoint(float x, float y); 
    void computeCoefficients();
    void setCoefficients(float slope, float intercept);
    void zero();            
    float getSlope() const;               
    float getIntercept() const;           
    float predict(float x) const;        
    void printEquation() const;            
};

} // namespace pimu