#include "filesystem.hpp"

// blockdevice.bin

void mostrarAyuda()
{
    std::cout << "\n-----------------------------------------------------------------\n";
    std::cout << "Comandos disponibles:\n";
    std::cout << "  create <nombre_archivo> <tamaño_bloque> <cantidad_bloques>\n";
    std::cout << "  open <nombre_archivo>\n";
    std::cout << "  close\n";
    std::cout << "  write <numero_bloque> \"<datos_a_escribir>\"\n";
    std::cout << "  read <numero_bloque> [offset] [length]\n";
    std::cout << "  info\n";
    std::cout << "  exit\n";
    std::cout << "  archivos (para gestionar archivos en el sistema)\n";
    std::cout << "-----------------------------------------------------------------\n";
}

void interfazComandos(SistemaArchivos &sistema)
{
    std::string comando;
    std::cout << "\n-------------------------------------------\n";
    std::cout << "    Bienvenido al Sistema de Archivos\n";
    std::cout << "---------------------------------------------\n";
    std::cout << "Escribe 'exit' para salir del sistema.\n\n";

    while (true)
    {
        std::cout << std::setw(3) << ">> ";
        std::getline(std::cin, comando);

        // Si el usuario quiere salir
        if (comando == "exit")
        {
            std::cout << "Saliendo del sistema de archivos. ¡Hasta luego!\n";
            exit(0);
        }

        std::istringstream stream(comando);
        std::string operacion, arg1, arg2, texto;
        stream >> operacion;

        if (operacion == "format")
        {
            if (sistema.format())
            {
                std::cout << "\033[1;32m✔ Disco virtual formateado con éxito.\033[0m\n";
            }
            else
            {
                std::cerr << "\033[1;31m✘ Error al formatear el disco virtual.\033[0m\n";
            }
        }
        else if (operacion == "ls")
        {
            std::cout << "\n\033[34mArchivos en el sistema:\033[0m\n";
            sistema.listFiles();
            std::cout << "\n";
        }
        else if (operacion == "cat")
        {
            stream >> arg1;
            if (arg1.empty())
            {
                std::cerr << "\033[1;31m✘ Error: Debes especificar un archivo.\033[0m\n";
            }
            else
            {
                std::string contenido = sistema.cat(arg1);
                std::cout << "Contenido de " << arg1 << ":\n"
                          << contenido << '\n';
            }
        }
        else if (operacion == "write")
        {
            stream >> std::quoted(texto) >> arg1;
            if (arg1.empty() || texto.empty())
            {
                std::cerr << "\033[1;31m✘ Error: Formato inválido. Usa: write \"<texto>\" <archivo>\033[0m\n";
            }
            else
            {
                if (sistema.writeFile(arg1, texto))
                {
                    std::cout << "\033[1;32m✔ Texto escrito exitosamente en el archivo '" << arg1 << "'.\033[0m\n";
                }
                else
                {
                    std::cerr << "\033[1;31m✘ Error al escribir en el archivo.\033[0m\n";
                }
            }
        }
        else if (operacion == "rm")
        {
            stream >> arg1;
            if (arg1.empty())
            {
                std::cerr << "\033[1;31m✘ Error: Debes especificar un archivo.\033[0m\n";
            }
            else
            {
                if (sistema.deleteFile(arg1))
                {
                    std::cout << "\033[1;32m✔ Archivo eliminado: " << arg1 << "\033[0m\n";
                }
                else
                {
                    std::cerr << "\033[1;31m✘ Error al eliminar el archivo: " << arg1 << "\033[0m\n";
                }
            }
        }
        else if (operacion == "hexdump")
        {
            stream >> arg1;
            if (arg1.empty())
            {
                std::cerr << "\033[1;31m✘ Error: Debes especificar un archivo.\033[0m\n";
            }
            else
            {
                std::cout << "Hexdump de " << arg1 << ":\n";
                sistema.hexDump(arg1);
            }
        }
        else if (operacion == "copy")
        {
            std::string direccion, archivoSistema, archivoHost;

            // Leer y validar los argumentos
            if (!(stream >> direccion >> archivoSistema >> archivoHost))
            {
                std::cerr << "\033[1;31m✘ Error: Formato inválido. Usa: copy in <archivo_host> <archivo_sistema> o copy out <archivo_sistema> <archivo_host>\033[0m\n";
                return;
            }

            // Eliminar espacios adicionales y depurar
            archivoSistema.erase(archivoSistema.find_last_not_of(" \t\n\r\f\v") + 1);
            archivoHost.erase(archivoHost.find_last_not_of(" \t\n\r\f\v") + 1);

            if (direccion == "in")
            {
                std::string temp = archivoSistema;
                archivoSistema = archivoHost;
                archivoHost = temp;
                
                // Operación copy in
                if (sistema.copyIn(archivoHost, archivoSistema))
                {
                    std::cout << "\033[1;32m✔ Archivo copiado exitosamente al sistema de archivos.\033[0m\n";
                }
                else
                {
                    std::cerr << "\033[1;31m✘ Error al copiar el archivo al sistema de archivos.\033[0m\n";
                }
            }
            else if (direccion == "out")
            {
                // Operación copy out
                if (sistema.copyOut(archivoSistema, archivoHost))
                {
                    std::cout << "\033[1;32m✔ Archivo copiado exitosamente al sistema host.\033[0m\n";
                }
                else
                {
                    std::cerr << "\033[1;31m✘ Error al copiar el archivo al sistema host.\033[0m\n";
                }
            }
            else
            {
                std::cerr << "\033[1;31m✘ Error: Operación desconocida. Usa 'copy in' o 'copy out'.\033[0m\n";
            }
        }
        else if (operacion == "help")
        {
            std::cout << "-----------------------------\n";
            std::cout << "Comandos disponibles:\n";
            std::cout << "  format                      - Formatea el disco virtual\n";
            std::cout << "  ls                          - Lista los archivos en el sistema\n";
            std::cout << "  cat <archivo>               - Muestra el contenido de un archivo\n";
            std::cout << "  write \"<texto>\" <archivo>  - Escribe texto en un archivo\n";
            std::cout << "  rm <archivo>                - Elimina un archivo\n";
            std::cout << "  hexdump <archivo>           - Muestra el volcado hexadecimal de un archivo\n";
            std::cout << "  copy in <archivo_host> <archivo_sistema> - Copia un archivo del host al sistema\n";
            std::cout << "  copy out <archivo_sistema> <archivo_host> - Copia un archivo del sistema al host\n";
            std::cout << "  exit                        - Sale del sistema de archivos\n";
            std::cout << "  back                        - Regresa al sistema de bloques\n";
            std::cout << "-----------------------------\n";
        }
        else if (operacion == "back")
        {
            std::cout << "Regresando al sistema de bloques...\n";
            break;
        }
        else
        {
            std::cerr << "\033[1;31m✘ Error: Comando desconocido.\033[0m\n";
            std::cout << "Escribe 'help' para obtener ayuda.\n";
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
                std::cout << "\033[1;31m✘ ¡Error! Ya hay un dispositivo abierto. Cierra el dispositivo actual antes de crear uno nuevo.\033[0m\n";
                continue;
            }

            std::string nombreArchivo;
            std::size_t tamanoBloque, cantidadBloques;
            if (opcion >> nombreArchivo >> tamanoBloque >> cantidadBloques)
            {
                if (dispositivo.create(nombreArchivo, tamanoBloque, cantidadBloques))
                {
                    std::cout << "Dispositivo creado con éxito: " << nombreArchivo << "\033[0m\n";
                }
                else
                {
                    std::cout << "\033[1;31m✘ Error al crear el dispositivo. Verifica los parámetros.\033[0m\n";
                }
            }
            else
            {
                std::cout << "\033[1;31m✘ Error: El formato es: create <nombre_archivo> <tamaño_bloque> <cantidad_bloques>\033[0m\n";
            }
        }
        else if (cmd == "open")
        {
            if (dispositivoAbierto)
            {
                std::cout << "\033[1;31m✘ ¡Error! Ya hay un dispositivo abierto. Cierra el dispositivo actual antes de abrir otro.\033[0m\n";
            }
            else
            {
                std::string nombreArchivo;
                if (opcion >> nombreArchivo)
                {
                    if (dispositivo.open(nombreArchivo))
                    {
                        std::cout << "\033[1;32m✔ Dispositivo abierto con éxito: " << nombreArchivo << "\033[0m\n";
                        dispositivoAbierto = true;
                    }
                    else
                    {
                        std::cout << "\033[1;31m✘ Error al abrir el dispositivo. Verifica el archivo.\033[0m\n";
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
                std::cout << "\033[1;32m✔ Dispositivo cerrado con éxito.\033[0m\n";
                dispositivoAbierto = false;
            }
            else
            {
                std::cout << "\033[1;31m✘ Error al cerrar el dispositivo. Intenta nuevamente.\033[0m\n";
            }
        }
        else if (cmd == "write")
        {
            if (!dispositivoAbierto)
            {
                std::cout << "\033[1;31m✘ ¡Error! No hay un dispositivo abierto para escribir.\033[0m\n";
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
                        std::cout << "\033[1;31m✘ Datos escritos en el bloque " << numeroBloque << " con éxito.\033[0m\n";
                    }
                    else
                    {
                        std::cout << "\033[1;31m✘ Error al escribir en el bloque. Verifica el bloque y los datos.\033[0m\n";
                    }
                }
                else
                {
                    std::cout << "\033[1;31m✘ Datos vacíos. El formato es: write <numero_bloque> \"<datos_a_escribir>\" \033[0m\n";
                }
            }
            else
            {
                std::cout << "\033[1;31m✘ Error, el formato es: write <numero_bloque> \"<datos_a_escribir>\" \033[0m\n";
            }
        }
        else if (cmd == "read")
        {
            if (!dispositivoAbierto)
            {
                std::cout << "\033[1;31m✘ ¡Error! No hay un dispositivo abierto para leer.\033[0m\n";
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
                    catch (const std::invalid_argument &)
                    {
                        std::cout << "\033[1;31m✘ Error: El offset no es válido.\033[0m\n";
                        continue;
                    }
                }

                if (opcion >> lengthStr)
                {
                    try
                    {
                        length = std::stoull(lengthStr);
                    }
                    catch (const std::invalid_argument &)
                    {
                        std::cout << "\033[1;31m✘ Error: La longitud no es válida.\033[0m\n";
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
                    std::cout << "\033[1;31m✘ Error al leer del bloque.\033[0m\n";
                }
            }
            else
            {
                std::cout << "\033[1;31m✘ Error, el formato es: read <numero_bloque> [offset] [length]\033[0m\n";
            }
        }
        else if (cmd == "info")
        {
            if (!dispositivoAbierto)
            {
                std::cout << "\033[1;31m✘ ¡Error! No hay un dispositivo abierto para mostrar información.\033[0m\n";
            }
            else
            {
                std::cout << "\nInformación del dispositivo:\n";
                std::cout << "  Nombre del archivo: " << dispositivo.getFilename() << "\n";
                std::cout << "  Tamaño de bloque: " << dispositivo.getBlockSize() << " bytes\n";
                std::cout << "  Cantidad total de bloques: " << dispositivo.getBlockCount() << " bloques\n";
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
        }
        else if (cmd == "archivos")
        {
            if (dispositivoAbierto)
            {
                SistemaArchivos s(dispositivo);
                interfazComandos(s);
            }
            else
            {
                std::cout << "\033[1;31m✘¡Error! No hay un dispositivo abierto para mostrar archivos.\033[0m\n";
            }
        }
        else
        {
            std::cout << "\033[1;31m✘ Comando no reconocido. Escribe 'help' para ver los comandos disponibles.\033[0m\n";
        }
    }

    return 0;
}