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
        inodo.name[0] = '\0';
        inodo.Apuntadores = std::vector<size_t>(8, 0);
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
    if (name.size() > 64)
    {
        std::cout << "Nombre de archivo demasiado largo" << std::endl;
        return false;
    }

    std::fstream archivo(nfile, std::ios::in | std::ios::out | std::ios::binary);
    if (!archivo.is_open())
    {
        std::cerr << "Sistema de bloques no encontrado." << std::endl;
        return false;
    }

    std::cout << 1 << std::endl;

    leerMapa(archivo);
    leerFileTable(archivo);

    std::cout << 2 << std::endl;

    for (Inodo &i : fileTable)
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

    std::cout << 3 << std::endl;

    for (Inodo &i : fileTable)
    {
        if (i.isFree)
        {
            std::strncpy(i.name, name.c_str(), sizeof(i.name) - 1);
            i.name[sizeof(i.name) - 1] = '\0';
            i.isFree = false;
            i.size = data.size();
            AsignarBloques(archivo, data.size(), name, data, true);
            GuardarFiletable(archivo);
            GuardarMapa(archivo);
            return true;
        }
    }

    std::cout << 4 << std::endl;
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

    leerFileTable(archivo);
    std::string texto;

    for (const Inodo &nodo : fileTable)
    {

        if (std::string(nodo.name) == name)
        {

            for (size_t ptr : nodo.Apuntadores)
            {
                if (ptr != 0)
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
    std::fstream archivo(nfile, std::ios::in | std::ios::out);
    leerFileTable(archivo);
    leerMapa(archivo);

    for (Inodo &i : fileTable)
    {
        if (i.name == name)
        {
            // Liberar los bloques de datos
            for (int j = 0; j < i.Apuntadores.size(); j++)
            {
                if (i.Apuntadores[j] != 0)
                {

                    liberarBloque(archivo, floor(i.Apuntadores[j] / Bsize));
                    i.Apuntadores[j] = 0;
                }
            }

            GuardarMapa(archivo);

            for (int j = 0; j < 64; ++j)
            {
                i.name[j] = '\0';
            }
            i.isFree = true;
            i.Apuntadores = std::vector<size_t>(8, 0);
            i.size = 0;

            GuardarFiletable(archivo);

            return true;
        }
    }

    std::cerr << "Error: El archivo no se encontró." << std::endl;
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
                if (inodo.Apuntadores[i] != 0)
                {
                    std::cout << i << ". offset: " << inodo.Apuntadores[i] << " - " << std::endl;
                    std::cout << i << ". bloque: " << floor(inodo.Apuntadores[i] / Bsize) << " - " << std::endl;
                }
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

bool SistemaArchivos::AsignarApuntadores(std::string name, std::vector<size_t> &apunts)
{
    int pos = 0;

    for (int i = 0; i < 256; i++)
    {
        std::cout << 20 << std::endl;
        if (fileTable[i].name == name)
        {
            std::cout << "p: " << fileTable[i].Apuntadores.size() << std::endl;
            std::cout << 21 << std::endl;

            for (int j = 0; j < 8; ++j)
            {
                std::cout << 22 << std::endl;

                if (fileTable[i].Apuntadores[j] == 0 && pos < apunts.size())
                {
                    std::cout << 23 << std::endl;
                    fileTable[i].Apuntadores[j] = apunts[pos];
                    pos++;

                    if (pos >= apunts.size())
                    {
                        break;
                    }

                    std::cout << 24 << std::endl;
                }
            }

            if (pos < apunts.size())
            {
                std::cout << "No hay espacio suficiente, se acortará el contenido del archivo" << std::endl;
                return true;
            }

            return true;
        }
    }

    return false;
}

void SistemaArchivos::AsignarBloques(std::fstream &archivo, size_t final, const std::string &name, const std::string &data, bool nuevo)
{
    std::vector<size_t> bloquesAsignados;
    size_t dataPos = 0;

    size_t ultimoBloqueUsado = static_cast<size_t>(-1);
    size_t espacioDisponibleEnUltimoBloque = 0;

    for (Inodo &i : fileTable)
    {
        if (std::string(i.name) == name && !nuevo && !i.Apuntadores.empty())
        {

            for (int j = 7; j >= 0; --j)
            {
                if (i.Apuntadores[j] != 0)
                {
                    ultimoBloqueUsado = i.Apuntadores[j] / Bsize;
                    espacioDisponibleEnUltimoBloque = Bsize - blockisCompleto(i.Apuntadores[j]);
                    break;
                }
            }
            break;
        }
    }

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
    AsignarApuntadores(name, bloquesAsignados);
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

void SistemaArchivos::liberarBloque(std::fstream &archivo, size_t bloque)
{
    archivo.seekp(bloque * Bsize);

    std::vector<char> padding(Bsize, '\0');
    archivo.write(padding.data(), padding.size());

    MapaDeBloquesLibres[bloque] = true;
}
