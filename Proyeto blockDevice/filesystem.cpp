#include "filesystem.hpp"

bool SistemaArchivos::format()
{
    std::fstream archivo(nfile);
    if (!archivo.is_open())
    {
        std::cout << "\033[31m" << "Sistema de bloques no encontrado\033[0m" << std::endl;
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

    return true;
}

bool SistemaArchivos::writeFile(const std::string &name, const std::string &data)
{
    if (name.size() > 64)
    {
        std::cout << "\033[31m" << "Nombre de archivo demasiado largo\033[0m" << std::endl;
        return false;
    }

    std::fstream archivo(nfile, std::ios::in | std::ios::out | std::ios::binary);
    if (!archivo.is_open())
    {
        std::cerr << "\033[31m" << "Sistema de bloques no encontrado.\033[0m" << std::endl;
        return false;
    }

    leerMapa(archivo);
    leerFileTable(archivo);

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

    std::cerr << "\033[31m" << "Error: No se pueden crear más archivos, tabla llena.\033[0m" << std::endl;
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
                        std::cerr << "\033[31m" << "Error: Apuntador fuera de rango.\033[0m" << std::endl;
                        return "\033[31m  Error: Archivo corrupto.\033[0m";
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

    return texto.empty() ? " \033[31m Error: No se encontró el archivo.\033[0m" : texto;
}

bool SistemaArchivos::copyOut(const std::string &file1, const std::string &file2)
{
    std::string contenido = cat(file1);
    if (contenido.empty() || contenido.find("Error") != std::string::npos)
    {
        std::cerr << "\033[31mError: No se pudo leer el archivo del sistema simulado.\033[0m" << std::endl;
        return false;
    }

    std::ofstream archivoReal(file2, std::ios::binary);
    if (!archivoReal.is_open())
    {
        std::cerr << "\033[31mError: No se pudo crear el archivo real.\033[0m" << std::endl;
        return false;
    }

    archivoReal.write(contenido.data(), contenido.size());
    archivoReal.close();

    std::cout << "\033[32mArchivo copiado exitosamente al sistema real.\033[0m" << std::endl;
    return true;
}

bool SistemaArchivos::copyIn(const std::string &file1, const std::string &file2)
{

    std::cout << "\033[1;33m[DEBUG] Intentando abrir el archivo host: " << file1 << "\033[0m\n";

    std::fstream archivoReal(file1);
    if (!archivoReal.is_open())
    {
        std::cerr << "\033[31mError: No se pudo abrir el archivo real.\033[0m" << std::endl;
        return false;
    }

    // Obtiene el tamaño del archivo real
    archivoReal.seekg(0, std::ios::end);
    std::size_t fileSize = archivoReal.tellg();
    archivoReal.seekg(0, std::ios::beg);

    // Verifica si el archivo excede 8 veces el tamaño de un bloque
    if (fileSize > 8 * Bsize)
    {
        std::cerr << "\033[31mError: El archivo real excede el tamaño permitido (8 bloques).\033[0m" << std::endl;
        return false;
    }

    // Lee el contenido del archivo real
    std::vector<char> buffer(fileSize);
    archivoReal.read(buffer.data(), fileSize);
    archivoReal.close();

    // Convierte el contenido a un string y escribe en el sistema simulado
    std::string data(buffer.begin(), buffer.end());
    if (!writeFile(file2, data))
    {
        std::cerr << "\033[31mError: No se pudo copiar el archivo al sistema simulado.\033[0m" << std::endl;
        return false;
    }

    std::cout << "\033[32mArchivo copiado exitosamente al sistema simulado.\033[0m" << std::endl;
    return true;
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

    std::cerr << "\033[31m" << "Error: El archivo no se encontró.\033[0m" << std::endl;
    return false;
}
void SistemaArchivos::listFiles()
{
    bool archivosEncontrados = false;
    for (const auto &inodo : fileTable)
    {
        if (!inodo.isFree)
        {
            std::cout << "----------------------------------------------------------------------------------------" << std::endl;
            archivosEncontrados = true;
            std::cout << "\033[36m" << "- Nombre: " << inodo.name
                      << "\033[36m" << ", Tamaño: " << inodo.size << " bytes \033[0m" << std::endl;
            for (int i = 0; i < inodo.Apuntadores.size(); i++)
            {

                if (inodo.Apuntadores[i] != 0)
                {

                    std::cout << i << ". offset: " << inodo.Apuntadores[i] << " - " << std::endl;
                    std::cout << i << ". bloque: " << floor(inodo.Apuntadores[i] / Bsize) << " - " << std::endl;
                }
            }
            std::cout << "----------------------------------------------------------------------------------------" << std::endl;
        }
    }

    if (!archivosEncontrados)
    {
        std::cout << "No hay archivos en el sistema.\033[0m" << std::endl;
    }
}

std::string SistemaArchivos::hexDump(const std::string &name)
{
    std::string contenido = cat(name);

    std::ostringstream hexStream;

    for (unsigned char c : contenido)
    {
        hexStream << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(c) << " ";
    }

    return hexStream.str();
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

    std::cout << "\033[31m" << "Error: No hay bloques libres disponibles.\033[0m" << std::endl;
    return -1;
}

void SistemaArchivos::verificarDatos()
{
    std::fstream archivo(nfile);
    if (!archivo.is_open())
    {
        std::cout << "\033[36m" << "Sistema de bloques no encontrado\033[0m" << std::endl;
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
        if (fileTable[i].name == name)
        {

            for (int j = 0; j < 8; ++j)
            {

                if (fileTable[i].Apuntadores[j] == 0 && pos < apunts.size())
                {
                    fileTable[i].Apuntadores[j] = apunts[pos];
                    pos++;

                    if (pos >= apunts.size())
                    {
                        break;
                    }
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

            bytesToWrite = std::min(data.size() - dataPos, espacioDisponibleEnUltimoBloque);

            archivo.seekp(ultimoBloqueUsado * Bsize + (Bsize - espacioDisponibleEnUltimoBloque));
            archivo.write(data.data() + dataPos, bytesToWrite);

            espacioDisponibleEnUltimoBloque -= bytesToWrite;
            dataPos += bytesToWrite;
        }
        else
        {

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
        std::cout << "\033[36m" << "No se pudo abrir el archivo.\033[0m" << std::endl;
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
