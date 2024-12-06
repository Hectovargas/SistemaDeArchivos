#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "filesystem.hpp" 

//blockdevice.bin

void mostrarAyuda()
{
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "Comandos disponibles:\n";
    std::cout << "  create <nombre_archivo> <tamaño_bloque> <cantidad_bloques>\n";
    std::cout << "  open <nombre_archivo>\n";
    std::cout << "  close\n";
    std::cout << "  write <numero_bloque> \"<datos_a_escribir>\"\n";
    std::cout << "  read <numero_bloque>\n";
    std::cout << "  info\n";
    std::cout << "  exit\n";
    std::cout << "-----------------------------------------------------------------\n";
}


void interfazComandos(SistemaArchivos &sistema) {
    std::string comando;
    std::cout << "Bienvenido al sistema de archivos. Escribe 'exit' para salir.\n";

    while (true) {
        std::cout << ">> ";
        std::getline(std::cin, comando);

        // Si el usuario quiere salir
        if (comando == "exit") {
            std::cout << "Saliendo del sistema de archivos.\n";
            exit(0);
        }

        // Divide el comando en partes
        std::istringstream stream(comando);
        std::string operacion, arg1, arg2, texto;
        stream >> operacion;

        if (operacion == "format") {
            if (sistema.format()) {
                std::cout << "Disco virtual formateado con éxito.\n";
            } else {
                std::cerr << "Error al formatear el disco virtual.\n";
            }
        } else if (operacion == "ls") {
            sistema.listFiles();
        } else if (operacion == "cat") {
            stream >> arg1;
            if (arg1.empty()) {
                std::cerr << "Error: Debes especificar un archivo.\n";
            } else {
                std::string contenido = sistema.cat(arg1);
                std::cout << contenido << '\n';
            }
        } else if (operacion == "write") {
            stream >> std::quoted(texto) >> arg1; // Lee texto entre comillas
            if (arg1.empty() || texto.empty()) {
                std::cerr << "Error: Formato inválido. Usa: write \"<texto>\" <archivo>\n";
            } else {
                if (sistema.writeFile(arg1, texto)) {
                    std::cout << "Texto escrito exitosamente en el archivo '" << arg1 << "'.\n";
                } else {
                    std::cerr << "Error al escribir en el archivo.\n";
                }
            }
        } else if (operacion == "rm") {
            stream >> arg1;
            if (arg1.empty()) {
                std::cerr << "Error: Debes especificar un archivo.\n";
            } else {
                if (sistema.deleteFile(arg1)) {
                    std::cout << "Archivo eliminado: " << arg1 << '\n';
                } else {
                    std::cerr << "Error al eliminar el archivo: " << arg1 << '\n';
                }
            }
        /*} else if (operacion == "hexdump") {
            stream >> arg1;
            if (arg1.empty()) {
                std::cerr << "Error: Debes especificar un archivo.\n";
            } else {
                sistema.hexDump(arg1); // Implementa esta función según lo requieras.
            }
            */
        } else if (operacion == "copy" && stream >> arg1 >> arg2) {
            if (arg1 == "in") {
                std::string archivoSistema, archivoHost;
                stream >> archivoHost >> archivoSistema;
                if (archivoHost.empty() || archivoSistema.empty()) {
                    std::cerr << "Error: Formato inválido. Usa: copy in <archivo_host> <archivo_sistema>\n";
                } else {
                    if (sistema.copyIn(archivoHost, archivoSistema)) {
                        std::cout << "Archivo copiado exitosamente al sistema de archivos.\n";
                    } else {
                        std::cerr << "Error al copiar el archivo.\n";
                    }
                }
            } else if (arg1 == "out") {
                std::string archivoSistema, archivoHost;
                stream >> archivoSistema >> archivoHost;
                if (archivoSistema.empty() || archivoHost.empty()) {
                    std::cerr << "Error: Formato inválido. Usa: copy out <archivo_sistema> <archivo_host>\n";
                } else {
                    if (sistema.copyOut(archivoSistema, archivoHost)) {
                        std::cout << "Archivo copiado exitosamente al sistema host.\n";
                    } else {
                        std::cerr << "Error al copiar el archivo.\n";
                    }
                }
            } else {
                std::cerr << "Error: Comando 'copy' inválido. Usa: copy in|out <archivo1> <archivo2>\n";
            }
        } else {
            std::cerr << "Error: Comando desconocido.\n";
        }
    }
}

int main()
{
    BlockDevice dispositivo;
    std::string comando;
    bool dispositivoAbierto = false;
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "Dispositivo de Bloques\n";
    std::cout << "Escribe 'help' para ver los comandos disponibles.\n";
    std::cout << "-----------------------------------------------------------------\n";
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, comando);

        std::istringstream opcion(comando);
        std::string cmd;
        opcion >> cmd;

        if (cmd == "help")
        {
            mostrarAyuda();
        }
        else if (cmd == "create")
        {
            if (dispositivoAbierto)
            {
                std::cout << "Ya hay un dispositivo abierto. Cierra el dispositivo actual antes de crear uno nuevo.\n";
                continue;
            }
            std::string nombreArchivo;
            std::size_t tamanoBloque, cantidadBloques;
            if (opcion >> nombreArchivo >> tamanoBloque >> cantidadBloques)
            {
                if (dispositivo.create(nombreArchivo, tamanoBloque, cantidadBloques))
                {
                    std::cout << "Dispositivo creado con éxito: " << nombreArchivo << "\n";
                }
                else
                {
                    std::cout << "Error al crear el dispositivo.\n";
                }
            }
            else
            {
                std::cout << "error, el formato es: create <nombre_archivo> <tamaño_bloque> <cantidad_bloques>\n";
            }
        }
        else if (cmd == "open")
        {
            if (dispositivoAbierto)
            {
                std::cout << "Ya hay un dispositivo abierto. Cierra el dispositivo actual antes de abrir otro.\n";
            }
            else
            {
                std::string nombreArchivo;
                if (opcion >> nombreArchivo)
                {
                    if (dispositivo.open(nombreArchivo))
                    {
                        std::cout << "Dispositivo abierto con éxito: " << nombreArchivo << "\n";
                        dispositivoAbierto = true;
                    }
                    else
                    {
                        std::cout << "Error al abrir el dispositivo.\n";
                    }
                }
                else
                {
                    std::cout << "Uso: open <nombre_archivo>\n";
                }
            }
        }
        else if (cmd == "close")
        {
            if (dispositivo.close())
            {
                std::cout << "Dispositivo cerrado con éxito.\n";
                dispositivoAbierto = false;
            }
            else
            {
                std::cout << "Error al cerrar el dispositivo.\n";
            }
        }
        else if (cmd == "write")
        {
            if (!dispositivoAbierto)
            {
                std::cout << "No hay un dispositivo abierto para escribir.\n";
                continue;
            }

            std::size_t numeroBloque;
            std::string datos;
            if (opcion >> numeroBloque)
            {
                std::getline(opcion, datos);
                if (!datos.empty() && datos.front() == '\"' && datos.back() == '\"')
                {
                    datos = datos.substr(1, datos.size() - 2);
                }

                if (!datos.empty())
                {
                    std::vector<char> vectorDatos(datos.begin(), datos.end());
                    if (dispositivo.writeBlock(numeroBloque, vectorDatos))
                    {
                        std::cout << "Datos escritos en el bloque " << numeroBloque << " con éxito.\n";
                    }
                    else
                    {
                        std::cout << "Error al escribir en el bloque.\n";
                    }
                }
                else
                {
                    std::cout << "Datos vacíos. Error, el formato es: write <numero_bloque> \"<datos_a_escribir>\"\n";
                }
            }
            else
            {
                std::cout << "error, el formato es: write <numero_bloque> \"<datos_a_escribir>\"\n";
            }
        }
        else if (cmd == "read")
        {
            if (!dispositivoAbierto)
            {
                std::cout << "No hay un dispositivo abierto para leer.\n";
                continue;
            }

            std::size_t numeroBloque, offset = 0, length = 0;
            if (opcion >> numeroBloque)
            {
                std::string offsetStr, lengthStr;
            
                if (opcion >> offsetStr)
                {
                    try
                    {
                        offset = std::stoull(offsetStr);
                    }
                    catch (const std::invalid_argument &e)
                    {
                        std::cout << "Error: El offset no es válido.\n";
                        continue;
                    }
                }
                if (opcion >> lengthStr)
                {
                    try
                    {
                        length = std::stoull(lengthStr);
                    }
                    catch (const std::invalid_argument &e)
                    {
                        std::cout << "Error: La longitud no es válida.\n";
                        continue;
                    }
                }


                std::string datos = dispositivo.readBlock(numeroBloque, offset, length);
                if (!datos.empty())
                {
                    std::cout << "Datos leídos del bloque " << numeroBloque << ": " << datos << "\n";
                }
                else
                {
                    std::cout << "Error al leer del bloque.\n";
                }
            }
            else
            {
                std::cout << "error, el formato es: read <numero_bloque> [offset] [length]\n";
            }
        }
        else if (cmd == "info")
        {
            if (!dispositivoAbierto)
            {
                std::cout << "No hay un dispositivo abierto para mostrar información.\n";
            }
            else
            {
                std::cout << "Información del dispositivo:\n";
                std::cout << "  Nombre del bloque: " << dispositivo.getFilename() << "\n";
                std::cout << "  Tamaño de bloque: " << dispositivo.getBlockSize() << " bytes\n";
                std::cout << "  Cantidad total de bloques: " << dispositivo.getBlockCount() << " Bloques \n";
            }
        }
        else if (cmd == "exit")
        {
            if (dispositivoAbierto)
            {
                dispositivo.close();
            }
            std::cout << "Saliendo del programa.\n";
            break;

        }else if(cmd == "archivos"){
            SistemaArchivos s;
            interfazComandos(s);

        }
        else
        {
            std::cout << "Comando no reconocido. Escribe 'help' para ver los comandos disponibles.\n";
        }
    }

    return 0;
}
