#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <cmath>

class BlockDevice
{
private:
    std::fstream file;
    std::string filename;
    std::size_t blockSize;
    std::size_t blockCount;
    bool isOpen;

public:
    BlockDevice() : blockSize(0), blockCount(0), isOpen(false) {}

    std::string getFilename()
    {
        return filename;
    }
    std::size_t getBlockSize()
    {
        return blockSize;
    }
    std::size_t getBlockCount()
    {
        return blockCount;
    }

    bool open(const std::string &filename)
    {
        if (file.is_open())
        {
            std::cout << "Error: Ya hay un archivo abierto.\n";
            return false;
        }

        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open())
        {
            std::cout << "Error al abrir el archivo.\n";
            return false;
        }

        this->filename = filename;

        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char *>(&blockSize), sizeof(blockSize));
        file.read(reinterpret_cast<char *>(&blockCount), sizeof(blockCount));

        isOpen = true;
        return true;
    }

    bool create(const std::string &filename, std::size_t blockSize, std::size_t blockCount)
    {
        std::ifstream testFile(filename);
        if (testFile.good())
        {
            testFile.close();
            return false;
        }

        std::ofstream archivo(filename, std::ios::binary);
        if (!archivo.is_open())
        {
            std::cout << "Error al crear el archivo.\n";
            return false;
        }

        this->blockSize = blockSize;
        this->blockCount = blockCount;

        archivo.write(reinterpret_cast<const char *>(&blockSize), sizeof(blockSize));
        archivo.write(reinterpret_cast<const char *>(&blockCount), sizeof(blockCount));

        std::vector<char> emptyBlock(blockSize, 0);
        for (std::size_t i = 0; i < blockCount; ++i)
        {
            archivo.write(emptyBlock.data(), blockSize);
        }

        archivo.close();
        return true;
    }

 bool close()
{
    if (!isOpen)
    {
        std::cout << "No hay un dispositivo abierto para cerrar." << std::endl;
        return false;
    }

    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<const char *>(&blockSize), sizeof(blockSize));
    file.write(reinterpret_cast<const char *>(&blockCount), sizeof(blockCount));

    file.close();
    isOpen = false;
    return true;
}

bool writeBlock(std::size_t num, const std::vector<char> &data) {
    if (!isOpen) {
        std::cout << "El dispositivo no está abierto." << std::endl;
        return false;
    }

    if (num >= blockCount) {
        std::cout << "El número de bloque está fuera del rango." << std::endl;
        return false;
    }

    std::vector<char> datos = data;

    
    datos.erase(datos.begin(), std::find_if(datos.begin(), datos.end(), [](unsigned char caracterAct) { return !std::isspace(caracterAct); }));
    datos.erase(std::find_if(datos.rbegin(), datos.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), datos.end());

   
    std::size_t totalSize = datos.size();
    if (totalSize > blockSize * 8) {
        std::cout << "Error: Los datos exceden el límite de 8 bloques." << std::endl;
        return false;
    }

    std::size_t offset = num * blockSize; 
    std::size_t written = 0;  

 
    while (written < totalSize) {

        std::size_t blockDataSize = std::min(blockSize, totalSize - written);


        std::vector<char> buffer(blockSize, '\0'); 
        std::copy(datos.begin() + written, datos.begin() + written + blockDataSize, buffer.begin());

        file.seekp(offset);
        file.write(buffer.data(), blockSize);

        written += blockDataSize;
        offset += blockSize;
    }

    return true;
}


    std::string readBlock(std::size_t num, std::size_t offset = 0, std::size_t length = 0)
    {
        if (!isOpen)
        {
            std::cout << "El dispositivo no está abierto." << std::endl;
            return "";
        }
        if (num >= blockCount)
        {
            std::cout << "El número de bloque está fuera del rango." << std::endl;
            return "";
        }
        if (length < offset)
        {
            std::cout << "El tamaño de la lectura es menor que el offset." << std::endl;
            return "";
        }

        if (length == 0)
        {
            length = blockSize - offset;
        }

        if (offset + length > blockSize)
        {
            std::cout << "El rango solicitado excede el tamaño del bloque." << std::endl;
            return "";
        }

        std::size_t totalOffset = num * blockSize + offset;

        std::vector<char> data(length);
        file.seekg(totalOffset);
        file.read(data.data(), length);

        std::ostringstream hexadecimal;
        bool firstValue = true;
        for (char c : data)
        {
            if (c != '\0')
            {
                if (!firstValue)
                {
                    hexadecimal << " ";
                }
                hexadecimal << std::hex << std::setfill('0') << std::setw(2) << (static_cast<int>(c) & 0xff);
                firstValue = false;
            }
        }

        return hexadecimal.str();
    }
};