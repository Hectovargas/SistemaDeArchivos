#include "BlockDevice.hpp"
class SistemaArchivos
{
public:
    BlockDevice device;
    std::vector<bool> MapaDeBloquesLibres;

    struct Inodo
    {
        char name[64];
        std::size_t size;
        std::vector<std::size_t> Apuntadores;
        bool isFree;

        Inodo() : size(0), isFree(true)
        {
            std::fill(std::begin(name), std::end(name), '\0');
        }
    };

    std::vector<Inodo> fileTable;

    std::size_t maxFiles = 256;
    std::size_t BloquesMaximosFile = 8;
    std::size_t Bcount;
    std::size_t Bsize;
    double CinodosPorBloque;
    double CBloquesTabla;
    double CMaxInodos;
    std::string nfile;

    //principales
    bool format();                                                    
    bool writeFile(const std::string &name, const std::string &data); 
    std::string cat(const std::string &name);                    
    bool copyOut(const std::string &file1,const std::string &file2);    
    bool copyIn(const std::string &file1,const std::string &file2);
    bool deleteFile(const std::string &name);
    void listFiles();
    std::string hexDump(const std::string &file);

    //Auxiliares                                  
    std::size_t getNextFreeBlock();
    void verificarDatos();
    void leerFileTable(std::fstream &archivo);
    void GuardarFiletable(std::fstream &archivo);
    void leerMapa(std::fstream &archivo);
    void GuardarMapa(std::fstream &archivo);
    void AsignarApuntadores(std::string &name,std::vector<size_t> &apunts);
    void AsignarBloques(std::fstream &archivo, size_t final,std::string name,std::string data, bool nuevo);
    size_t calcularInicio(std::string &name,size_t punteroActual);
    size_t calcularfinal(std::string &name);
    void escribirBloques(std::fstream &archivo, size_t inicio, size_t final, std::string data);
    size_t blockisCompleto(size_t offset);
    
    SistemaArchivos()
    {
        nfile = "blockdevice.bin";
        std::fstream archivo(nfile);
        std::cout << 1 << std::endl;
        if (archivo.is_open())
        {
            std::cout << 2 << std::endl;
            archivo.seekg(0);
            archivo.read(reinterpret_cast<char *>(&Bsize), sizeof(size_t));
            std::cout << Bsize << std::endl;
            archivo.read(reinterpret_cast<char *>(&Bcount), sizeof(size_t));
            std::cout << Bcount << std::endl;
            CinodosPorBloque = floor(Bsize / sizeof(Inodo));
            CBloquesTabla = ceil(Bcount / CinodosPorBloque);
            CMaxInodos = CBloquesTabla * CinodosPorBloque;
            fileTable.resize(maxFiles);
            MapaDeBloquesLibres.resize(Bcount);
            leerFileTable(archivo);
            leerMapa(archivo);

        }
        else
        {
            std::cout << "Error al Abrir el sistema de bloques" << std::endl;
            exit(0);
            
        }
        archivo.close();
    }
};