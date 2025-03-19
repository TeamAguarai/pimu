#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

namespace pimu {

class Writer {
private:
    std::ofstream file_;     
    bool is_open_;           
    std::string delimiter_;  

public:
    Writer(std::string file_name, std::string header, std::string delim);
    ~Writer();

    void write_row(const std::vector<std::string>& data);
    void write_line(const std::string& line);
    void close();
};

/* Constructor: abre el archivo y escribe el encabezado si es posible */
Writer::Writer(std::string file_name, std::string header, std::string delim) 
    : is_open_(false), delimiter_(delim) {
    file_.open(file_name);
    if (file_.is_open()) {
        is_open_ = true;
        file_ << header << "\n";
    } else {
        std::cerr << "Error: No se pudo abrir el archivo " << file_name << "\n";
    }
}

/* Destructor: cierra el archivo si está abierto */
Writer::~Writer() {
    if (is_open_) file_.close();
}

/* Escribe una fila en el archivo separando los valores con el delimitador */
void Writer::write_row(const std::vector<std::string>& data) {
    if (!is_open_) {
        std::cerr << "Error: El archivo no está abierto.\n";
        return;
    } 
    std::ostringstream oss;
    for (size_t i = 0; i < data.size(); ++i) {
        oss << data[i];
        if (i != data.size() - 1) {
            oss << delimiter_;
        }
    }
    file_ << oss.str() << "\n";
}

/* Escribe una línea en el archivo */
void Writer::write_line(const std::string& line) {
    if (!is_open_) {
        std::cerr << "Error: El archivo no está abierto.\n";
        return;
    }
    file_ << line << "\n";
}

/* Cierra el archivo si está abierto */
void Writer::close() {
    if (is_open_) {
        file_.close();
        is_open_ = false;
    }
}

} // namespace pimu
