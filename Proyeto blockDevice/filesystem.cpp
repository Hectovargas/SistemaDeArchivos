#include "filesystem.hpp"

bool SistemaArchivos::format()
{
    std::fstream archivo(nfile);
    if (!archivo.is_open())
    {
        std::cout << "Sistema de bloques no encontrado" << std::endl;
        return false;
    }

    MapaDeBloquesLibres.assign(Bcount, true);

    size_t superbloqueLong = ceil(sizeof(Bsize) + sizeof(Bcount) + sizeof(MapaDeBloquesLibres) + 35072) / Bsize;

    for (int i = 0; i <= superbloqueLong; i++)
    {
        MapaDeBloquesLibres[i] = false;
    }

    fileTable.clear();
    for (std::size_t i = 0; i < maxFiles; ++i)
    {
        Inodo inodo;
        inodo.isFree = true;
        inodo.size = 0;
        inodo.name[0] = '\0'; // Limpia el nombre
        inodo.Apuntadores.clear();
        fileTable.push_back(inodo);
    }

    archivo.write(reinterpret_cast<char *>(&Bsize), sizeof(Bsize));
    archivo.write(reinterpret_cast<char *>(&Bcount), sizeof(Bcount));

    GuardarMapa(archivo);
    GuardarFiletable(archivo);

    // Rellena el resto del disco con bloques vacíos
    std::vector<char> emptyBlock(Bsize, '\0');
    for (std::size_t i = 0; i < Bcount; ++i)
    {
        archivo.write(emptyBlock.data(), emptyBlock.size());
    }

    archivo.close();
    std::cout << "Sistema de archivos formateado con éxito." << std::endl;
    return true;
}

bool SistemaArchivos::writeFile(const std::string &name, const std::string &data)
{

    std::fstream archivo(nfile);
    if (!archivo.is_open())
    {
        std::cout << "Sistema de bloques no encontrado" << std::endl;
        return false;
    }

    leerMapa(archivo);
    leerFileTable(archivo);

    for (Inodo i : fileTable)
    {
        if (!i.isFree && i.name == name)
        {
            i.size += data.size();
            AsignarBloques(archivo, data.size(), name, data, false);
            GuardarFiletable(archivo);
            GuardarMapa(archivo);
            return true;
        }
    }

    // si no encuentra el archivo
    // Archivo nuevo
    for (int i = 0; i < fileTable.size(); i++)
    {
        if (fileTable[i].isFree)
        {
            std::strcpy(fileTable[i].name, name.c_str());
            fileTable[i].isFree = false;
            fileTable[i].size = data.size();
            AsignarBloques(archivo, data.size(), name, data, true);
            GuardarFiletable(archivo);
            GuardarMapa(archivo);
            return true;
        }
    }
    return false;
}

std::string SistemaArchivos::cat(const std::string &name)
{
    return "";
}

bool SistemaArchivos::copyOut(const std::string &file1, const std::string &file2)
{
    return false;
}

bool SistemaArchivos::copyIn(const std::string &file1, const std::string &file2)
{
    return false;
}

bool SistemaArchivos::deleteFile(const std::string &name)
{
    return false;
}

void SistemaArchivos::listFiles()
{
    bool archivosEncontrados = false; // Indicador de si hay archivos en el sistema.

    std::cout << "Archivos en el sistema de archivos:" << std::endl;
    for (const auto &inodo : fileTable)
    {
        if (!inodo.isFree)
        { // Si el inodo está ocupado, significa que representa un archivo.
            archivosEncontrados = true;
            std::cout << "- Nombre: " << inodo.name
                      << ", Tamaño: " << inodo.size << " bytes"
                      << std::endl;
        }
    }

    if (!archivosEncontrados)
    {
        std::cout << "No hay archivos en el sistema." << std::endl;
    }
}

std::size_t SistemaArchivos::getNextFreeBlock()
{
    for (std::size_t i = 0; i < MapaDeBloquesLibres.size(); ++i)
    {
        if (MapaDeBloquesLibres[i]) // Si el bloque está libre (valor true)
        {
            MapaDeBloquesLibres[i] = false; // Marca el bloque como ocupado
            return i;                       // Retorna el índice del bloque libre
        }
    }

    std::cout << "Error: No hay bloques libres disponibles." << std::endl;
    return -1; // Indica que no hay bloques libres disponibles
}

void SistemaArchivos::verificarDatos()
{
    std::fstream archivo(nfile);
    if (!archivo.is_open())
    {
        std::cout << "Sistema de bloques no encontrado" << std::endl;
        return;
    }

    archivo.seekp(0);
    archivo.write(reinterpret_cast<char *>(&Bsize), sizeof(Bsize));
    archivo.write(reinterpret_cast<char *>(&Bcount), sizeof(Bcount));
    archivo.close();
}

void SistemaArchivos::leerFileTable(std::fstream &archivo)
{
    archivo.seekg(sizeof(Bcount) + sizeof(Bsize) + Bcount);
    for (int i = 0; i < 256; i++)
    {
        archivo.read(reinterpret_cast<char *>(&fileTable[i]), sizeof(Inodo));
    }
}

void SistemaArchivos::GuardarFiletable(std::fstream &archivo)
{
    archivo.seekp(sizeof(Bcount) + sizeof(Bsize) + Bcount);
    for (int i = 0; i < 256; i++)
    {
        archivo.write(reinterpret_cast<char *>(&fileTable[i]), sizeof(Inodo));
    }
}

void SistemaArchivos::leerMapa(std::fstream &archivo)
{
    archivo.seekg(sizeof(Bcount) + sizeof(Bsize));
    for (size_t i = 0; i < Bcount; i++)
    {
        bool temp;
        archivo.read(reinterpret_cast<char *>(&temp), sizeof(bool));
        MapaDeBloquesLibres[i] = temp;
    }
}

void SistemaArchivos::GuardarMapa(std::fstream &archivo)
{
    archivo.seekp(sizeof(Bcount) + sizeof(Bsize));
    for (size_t i = 0; i < Bcount; i++)
    {
        bool temp;
        temp = MapaDeBloquesLibres[i];
        archivo.write(reinterpret_cast<char *>(&temp), sizeof(bool));
    }
}

void SistemaArchivos::AsignarApuntadores(std::string &name, std::vector<size_t> &apunts)
{
    for (int i = 0; i < 256; i++)
    {
        if (fileTable[i].name == name)
        {
            fileTable[i].Apuntadores.insert(fileTable[i].Apuntadores.end(), apunts.begin(), apunts.end());
        }
    }
}

void SistemaArchivos::AsignarBloques(std::fstream &archivo, size_t final, std::string name, std::string data, bool nuevo)
{
    if (nuevo)
    {
        std::vector<size_t> temp;
        int e;
        int posIsubstr = 0;

        for (int i = 0; i <= Bcount; i++)
        {
            if (MapaDeBloquesLibres[i] == true)
            {
                e = i;
            }
            for (int i = e; i < e + ceil(final / Bsize); i++)
            {

                MapaDeBloquesLibres[i] = false;
                //--------------------------------------------------------------------------------------------------------------------------------
                temp.push_back(i * Bsize);

                if (data.size() > Bsize && data.size() < posIsubstr + Bsize)
                {

                    int posFinal = data.size() - (floor(data.size() / Bsize) * Bsize);
                    for (int j = 0; j < posFinal; j++)
                    {
                        data.append("\0");
                    }

                    escribirBloques(archivo, i * Bsize, (i * Bsize) + data.size(), data.substr(posIsubstr, posFinal));
                }
                else if (data.size() > Bsize && data.size() > posIsubstr + Bsize)
                {

                    escribirBloques(archivo, i * Bsize, (i * Bsize) + data.size(), data.substr(posIsubstr, Bsize));
                }
                else if (data.size() <= Bsize)
                {
                    for (int j = data.size(); j < Bsize; j++)
                    {
                        data.append("\0");
                    }
                    escribirBloques(archivo, i * Bsize, (i * Bsize) + data.size(), data);
                }
            }
            AsignarApuntadores(name, temp);
        }
    }
    else
    {
        size_t ultimo;
        size_t pos;

        for (size_t i = 0; i < 256; i++)
        {
            if (fileTable[i].name == name)
            {
                ultimo = fileTable[i].Apuntadores.back();
                pos = blockisCompleto(ultimo);
                if (pos != 0)
                {
                    archivo.seekp(ultimo + pos);
                    if (Bsize >= pos + data.size())
                    {
                        archivo.write(reinterpret_cast<char *>(&data), sizeof(data));
                    }
                    else
                    {
                        std::string text = data.substr(0, Bsize - pos);
                        archivo.write(reinterpret_cast<char *>(&text), (Bsize - pos));
                        AsignarBloques(archivo, ultimo + (Bsize - pos), name, data.substr(Bsize - pos, data.size()), true);
                    }
                }
                else
                {
                    AsignarBloques(archivo, ultimo, name, data, true);
                }
            }
        }
    }
}

size_t SistemaArchivos::calcularInicio(std::string &name, size_t punteroActual)
{
    size_t longitud;
    for (int i = 0; i < 256; i++)
    {
        if (fileTable[i].name == name)
        {
            return punteroActual - fileTable[i].size;
        }
    }
    return 0;
}

size_t SistemaArchivos::calcularfinal(std::string &name)
{
    for (int i = 0; i < 256; i++)
    {
        if (fileTable[i].name == name)
        {
            return fileTable[i].size;
        }
    }
    return 0;
}

void SistemaArchivos::escribirBloques(std::fstream &archivo, size_t inicio, size_t final, std::string data)
{
    archivo.seekp(inicio);
    archivo.write(reinterpret_cast<char *>(&data), sizeof(data));
}

size_t SistemaArchivos::blockisCompleto(size_t offset)
{
    std::fstream archivo(nfile);
    if (!archivo.is_open())
    {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        exit(0);
    }

    archivo.seekg(offset);

    char caracter;
    for (size_t i = 0; i < Bsize; i++)
    {
        archivo.read(&caracter, sizeof(caracter));

        if (caracter == '\0')
        {
            return i;
        }
    }

    return 0;
}
