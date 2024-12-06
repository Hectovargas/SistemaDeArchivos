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
    std::fstream archivo(nfile, std::ios::in | std::ios::out | std::ios::binary);
    if (!archivo.is_open())
    {
        std::cerr << "Sistema de bloques no encontrado." << std::endl;
        return false;
    }

    leerMapa(archivo);
    leerFileTable(archivo);

    for (Inodo &i : fileTable) // Uso de referencia para modificar directamente
    {
        if (!i.isFree && std::string(i.name) == name)
        {
            i.size += data.size();
            AsignarBloques(archivo, data.size(), name, data, false);
            GuardarFiletable(archivo);
            GuardarMapa(archivo);
            return true;
        }
    }

    // Si no encuentra el archivo, crea uno nuevo
    for (Inodo &i : fileTable)
    {
        if (i.isFree)
        {
            std::strncpy(i.name, name.c_str(), sizeof(i.name) - 1);
            i.name[sizeof(i.name) - 1] = '\0'; // Garantizar terminación nula
            i.isFree = false;
            i.size = data.size();
            AsignarBloques(archivo, data.size(), name, data, true);
            GuardarFiletable(archivo);
            GuardarMapa(archivo);
            return true;
        }
    }

    std::cerr << "Error: No se pueden crear más archivos, tabla llena." << std::endl;
    return false;
}

std::string SistemaArchivos::cat(const std::string &name)
{
    std::fstream archivo(nfile, std::ios::in | std::ios::binary);
    if (!archivo.is_open())
    {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return "Error: No se pudo abrir el archivo.";
    }

    leerFileTable(archivo); // Lee la tabla de archivos desde el disco
    std::string texto;

    for (const Inodo &nodo : fileTable)
    {

        if (std::string(nodo.name) == name)
        {

            for (size_t ptr : nodo.Apuntadores)
            {

                if (ptr >= Bcount * Bsize)
                {
                    std::cerr << "Error: Apuntador fuera de rango." << std::endl;
                    return "Error: Archivo corrupto.";
                }

                archivo.seekg(ptr); 
                std::vector<char> buffer(Bsize, '\0');
                archivo.read(buffer.data(), Bsize);

                for (char c : buffer)
                {
                    if (c == '\0') 
                        return texto;
                    texto += c;
                }
            }
        }
    }

    return texto.empty() ? "Error: No se encontró el archivo." : texto;
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
                      << ", Tamaño: " << inodo.size << " bytes ";
                      for (int i = 0; i < inodo.Apuntadores.size(); i++)
                      {
                        std::cout << i << ". offset: " << inodo.Apuntadores[i] << " - " << std::endl;
                        std::cout << i << ". bloque: " << floor(inodo.Apuntadores[i]/Bsize) << " - " << std::endl;
                      }
                      
                     std::cout << std::endl;
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
        if (MapaDeBloquesLibres[i]) 
        {
            MapaDeBloquesLibres[i] = false; 
            return i;                       
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

void SistemaArchivos::AsignarApuntadores(std::string name, std::vector<size_t> &apunts)
{
    for (int i = 0; i < 256; i++)
    {
        if (fileTable[i].name == name)
        {
            fileTable[i].Apuntadores.insert(fileTable[i].Apuntadores.end(), apunts.begin(), apunts.end());
        }
    }
}

void SistemaArchivos::AsignarBloques(std::fstream &archivo, size_t final, const std::string &name, const std::string &data, bool nuevo)
{
    std::vector<size_t> bloquesAsignados;
    size_t dataPos = 0;

    // Encuentra el último bloque asignado al archivo si ya existe
    size_t ultimoBloqueUsado = static_cast<size_t>(-1);
    size_t espacioDisponibleEnUltimoBloque = 0;

    for (Inodo &i : fileTable)
    {
        if (std::string(i.name) == name && !nuevo && !i.Apuntadores.empty())
        {
            ultimoBloqueUsado = i.Apuntadores.back() / Bsize;
            espacioDisponibleEnUltimoBloque = Bsize - blockisCompleto(i.Apuntadores.back());
            break;
        }
    }

    // Comienza a escribir los datos
    while (dataPos < data.size())
    {
        size_t bytesToWrite;

        if (ultimoBloqueUsado != static_cast<size_t>(-1) && espacioDisponibleEnUltimoBloque > 0)
        {
            // Si el último bloque tiene espacio, escribe allí
            bytesToWrite = std::min(data.size() - dataPos, espacioDisponibleEnUltimoBloque);

            archivo.seekp(ultimoBloqueUsado * Bsize + (Bsize - espacioDisponibleEnUltimoBloque));
            archivo.write(data.data() + dataPos, bytesToWrite);

            espacioDisponibleEnUltimoBloque -= bytesToWrite;
            dataPos += bytesToWrite;
        }
        else
        {
            // Si no hay bloques disponibles o el último está lleno, asigna uno nuevo
            size_t bloqueLibre = getNextFreeBlock();
            if (bloqueLibre == static_cast<size_t>(-1))
            {
                std::cerr << "Error: No hay bloques libres disponibles." << std::endl;
                return;
            }

            bloquesAsignados.push_back(bloqueLibre * Bsize);

            bytesToWrite = std::min(data.size() - dataPos, static_cast<size_t>(Bsize));
            archivo.seekp(bloqueLibre * Bsize);
            archivo.write(data.data() + dataPos, bytesToWrite);

            // Llena el resto del bloque con ceros si es necesario
            if (bytesToWrite < Bsize)
            {
                std::vector<char> padding(Bsize - bytesToWrite, '\0');
                archivo.write(padding.data(), padding.size());
            }

            dataPos += bytesToWrite;
            ultimoBloqueUsado = bloqueLibre;
            espacioDisponibleEnUltimoBloque = Bsize - bytesToWrite;
        }
    }

    // Asigna los bloques al archivo
    if (nuevo)
    {
        AsignarApuntadores(name, bloquesAsignados);
    }
    else
    {
        for (Inodo &i : fileTable)
        {
            if (std::string(i.name) == name)
            {
                i.Apuntadores.insert(i.Apuntadores.end(), bloquesAsignados.begin(), bloquesAsignados.end());
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

void SistemaArchivos::escribirBloques(std::fstream &archivo, size_t inicio, size_t final, const std::string &data)
{
    archivo.seekp(inicio);
    archivo.write(data.c_str(), data.size());
    if (data.size() < (final - inicio))
    {
        std::vector<char> padding((final - inicio) - data.size(), '\0');
        archivo.write(padding.data(), padding.size());
    }
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
