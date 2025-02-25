#ifdef USING_VSCODE_AS_EDITOR
    #include "Writer.h"
#endif

namespace pimu {
    
Writer::Writer(std::string filename, std::string header, std::string delim) : is_open(false), delimiter(delim)
{
    file.open(filename);
    if (file.is_open()) {
        is_open = true;
        file << header << "\n";
    } else {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << "\n";
    }
}

Writer::~Writer() 
{
    if (is_open) file.close();
}

void Writer::write_row(const std::vector<std::string>& data) 
{
    if (!is_open) {
        std::cerr << "Error: El archivo no está abierto.\n";
        return;
    } 
    std::ostringstream oss;
    for (size_t i = 0; i < data.size(); ++i) {
        oss << data[i];
        if (i != data.size() - 1) {
            oss << delimiter;
        }
    }
    file << oss.str() << "\n";
}

void Writer::write_line(const std::string& line) 
{
    if (!is_open) {
        std::cerr << "Error: El archivo no está abierto.\n";
        return;
    }
    file << line << "\n";
}

void Writer::close() 
{
    if (is_open) {
        file.close();
        is_open = false;
    }
}

}