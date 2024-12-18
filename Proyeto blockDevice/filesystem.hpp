#include "BlockDevice.hpp"
class SistemaArchivos
{
public:
    std::vector<bool> MapaDeBloquesLibres;
    struct Inodo
    {
        char name[64];
        std::size_t size;
        std::vector<std::size_t> Apuntadores;
        bool isFree;

        Inodo() : size(0), isFree(true), Apuntadores(8, 0)
        {
            std::fill(std::begin(name), std::end(name), '\0');
        }
    };  

    std::vector<Inodo> fileTable;

    std::size_t maxFiles = 256;
    std::size_t BloquesMaximosFile = 8;
    std::size_t Bcount;
    std::size_t Bsize;
    std::string nfile;
    size_t superbloqueLong;

    // principales
    bool format();
    bool writeFile(const std::string &name, std::string &data);
    std::string cat(const std::string &name);
    bool copyOut(const std::string &file1, const std::string &file2);
    bool copyIn(const std::string &file1, const std::string &file2);
    bool deleteFile(const std::string &name);
    void listFiles();
    std::string hexDump(const std::string &file);

    // Auxiliares
    std::size_t getNextFreeBlock();
    void verificarDatos();
    void leerFileTable(std::fstream &archivo);
    void GuardarFiletable(std::fstream &archivo);
    void leerMapa(std::fstream &archivo);
    void GuardarMapa(std::fstream &archivo);
    bool AsignarApuntadores(std::string name, std::vector<size_t> &apunts);
    bool AsignarBloques(std::fstream &archivo, size_t final, const std::string &name, std::string &data, bool nuevo);
    size_t calcularInicio(std::string &name, size_t punteroActual);
    size_t calcularfinal(std::string &name);
    void escribirBloques(std::fstream &archivo, size_t inicio, size_t final, const std::string &data);
    size_t blockisCompleto(size_t offset);
    void liberarBloque(std::fstream &archivo, size_t);
    size_t obteneBloquesLibres();
    //void imprimirContenidoDisco();

    SistemaArchivos(BlockDevice &dis)
    {
        bool nuevo = true;
        nfile = dis.getFilename();
        std::fstream archivo(nfile);
        if (archivo.is_open())
        {
            archivo.seekg(0);
            archivo.read(reinterpret_cast<char *>(&Bsize), sizeof(size_t));

            archivo.read(reinterpret_cast<char *>(&Bcount), sizeof(size_t));
            
            superbloqueLong = ceil(16 + Bcount + (256*137)) / Bsize;
            
            if(Bcount <= superbloqueLong){
                std::cout << "blockdevice sin espacio" << std::endl;
                exit(0);
            }

            fileTable.resize(maxFiles);
            MapaDeBloquesLibres.resize(Bcount);
            leerFileTable(archivo);
            leerMapa(archivo);

            for (int i = 0; i < Bcount; i++)
            {
                if (MapaDeBloquesLibres[i])
                {
                    
                    nuevo = false;
                    break;
                }
            }

            if (nuevo)
            {
                format();
            }
        }
        else
        {
            std::cout << "Error al Abrir el sistema de bloques" << std::endl;
            exit(0);
        }
        archivo.close();
    }
};

/*
 CinodosPorBloque = floor(Bsize / sizeof(Inodo));
 CBloquesTabla = ceil(Bcount / CinodosPorBloque);
 CMaxInodos = CBloquesTabla * CinodosPorBloque;
 */