#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>


namespace pimu { 

class Writer {
private:
    std::ofstream file;     
    bool is_open;           
    std::string delimiter;  
public:
    Writer(std::string filename, std::string header, std::string delim = ",");
    ~Writer();
    void write_row(const std::vector<std::string>& data);
    void write_line(const std::string& line);
    void close();
};

}