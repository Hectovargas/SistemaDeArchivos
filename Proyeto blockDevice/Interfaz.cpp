#include "filesystem.hpp"

void mostrarAyuda()
{
    std::cout << "\n-----------------------------------------------------------------\n";
    std::cout << "Comandos disponibles:\n";
    std::cout << "  create <nombre_archivo> <tamaño_bloque> <cantidad_bloques>\n";
    std::cout << "  open <nombre_archivo>\n";
    std::cout << "  close\n";
    std::cout << "  info\n";
    std::cout << "  exit\n";
    std::cout << "  archivos (para gestionar archivos en el sistema)\n";
    std::cout << "-----------------------------------------------------------------\n";
}

std::string extraerTextoEntreDelimitadores(const std::string &texto, char inicio = '<', char fin = '>')
{
    size_t posInicio = texto.find(inicio);
    size_t posFin = texto.find(fin, posInicio + 1);

    if (posInicio != std::string::npos && posFin != std::string::npos)
    {
        return texto.substr(posInicio + 1, posFin - posInicio - 1);
    }

    return "";
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
        std::istringstream stream(comando);
        std::string operacion, argumento;

        stream >> operacion;

        // Función para extraer texto entre <>
        auto extraerTextoEntreAngulos = [](const std::string &input) -> std::string
        {
            size_t inicio = input.find('<');
            size_t fin = input.find('>');
            if (inicio == std::string::npos || fin == std::string::npos || inicio >= fin)
                return "";
            return input.substr(inicio + 1, fin - inicio - 1);
        };

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
            std::getline(stream, argumento);
            std::string archivo = extraerTextoEntreAngulos(argumento);
            if (archivo.empty())
            {
                std::cerr << "\033[1;31m✘ Error: Formato inválido. Usa: cat <archivo>\033[0m\n";
            }
            else
            {
                std::string contenido = sistema.cat(archivo);
                std::cout << "Contenido de " << archivo << ":\n"
                          << contenido << '\n';
            }
        }
        else if (operacion == "write")
        {
            std::getline(stream, argumento);
            size_t segundoInicio = argumento.find('<', argumento.find('<') + 1);
            size_t segundoFin = argumento.find('>', argumento.find('>') + 1);
            if (segundoInicio == std::string::npos || segundoFin == std::string::npos)
            {
                std::cerr << "\033[1;31m✘ Error: Formato inválido. Usa: write <texto> <archivo>\033[0m\n";
                continue;
            }

            std::string texto = extraerTextoEntreAngulos(argumento.substr(0, segundoInicio));
            std::string archivo = extraerTextoEntreAngulos(argumento.substr(segundoInicio));

            if (texto.empty() || archivo.empty())
            {
                std::cerr << "\033[1;31m✘ Error: Ambos argumentos deben estar entre <>.\033[0m\n";
            }
            else if (sistema.writeFile(archivo, texto))
            {
                std::cout << "\033[1;32m✔ Texto escrito exitosamente en el archivo '" << archivo << "'.\033[0m\n";
            }
            else
            {
                std::cerr << "\033[1;31m✘ Error al escribir en el archivo.\033[0m\n";
            }
        }
        else if (operacion == "rm")
        {
            std::getline(stream, argumento);
            std::string archivo = extraerTextoEntreAngulos(argumento);
            if (archivo.empty())
            {
                std::cerr << "\033[1;31m✘ Error: Formato inválido. Usa: rm <archivo>\033[0m\n";
            }
            else if (sistema.deleteFile(archivo))
            {
                std::cout << "\033[1;32m✔ Archivo eliminado: " << archivo << "\033[0m\n";
            }
            else
            {
                std::cerr << "\033[1;31m✘ Error al eliminar el archivo: " << archivo << "\033[0m\n";
            }
        }
        else if (operacion == "hexdump")
        {
            std::getline(stream, argumento);
            std::string archivo = extraerTextoEntreAngulos(argumento);
            if (archivo.empty())
            {
                std::cerr << "\033[1;31m✘ Error: Formato inválido. Usa: hexdump <archivo>\033[0m\n";
            }
            else
            {
                std::cout << "Hexdump de " << archivo << ":\n";
                std::cout << sistema.hexDump(archivo) << std::endl;
            }
        }
        else if (operacion == "copy")
        {
            std::getline(stream, argumento);

            std::istringstream argStream(argumento);
            std::string direccion, restoArgumentos;
            argStream >> direccion;
            std::getline(argStream, restoArgumentos);

            if (direccion != "in" && direccion != "out")
            {
                std::cerr << "\033[1;31m✘ Error: Dirección inválida. Usa: copy in <archivo_host> <archivo_sistema> o copy out <archivo_sistema> <archivo_host>\033[0m\n";
                continue;
            }

            size_t primerInicio = restoArgumentos.find('<');
            size_t primerFin = restoArgumentos.find('>');
            size_t segundoInicio = restoArgumentos.find('<', primerFin + 1);
            size_t segundoFin = restoArgumentos.find('>', segundoInicio + 1);

            if (primerInicio == std::string::npos || primerFin == std::string::npos ||
                segundoInicio == std::string::npos || segundoFin == std::string::npos)
            {
                std::cerr << "\033[1;31m✘ Error: Formato inválido. Usa: copy in <archivo_host> <archivo_sistema> o copy out <archivo_sistema> <archivo_host>\033[0m\n";
                continue;
            }

            std::string archivo1 = restoArgumentos.substr(primerInicio + 1, primerFin - primerInicio - 1);
            std::string archivo2 = restoArgumentos.substr(segundoInicio + 1, segundoFin - segundoInicio - 1);

            if (archivo1.empty() || archivo2.empty())
            {
                std::cerr << "\033[1;31m✘ Error: Ambos archivos deben estar entre <> y no pueden estar vacíos.\033[0m\n";
                continue;
            }

            if (direccion == "in" && sistema.copyIn(archivo1, archivo2))
            {
                std::cout << "\033[1;32m✔ Archivo copiado exitosamente al sistema de archivos.\033[0m\n";
            }
            else if (direccion == "out" && sistema.copyOut(archivo1, archivo2))
            {
                std::cout << "\033[1;32m✔ Archivo copiado exitosamente al sistema host.\033[0m\n";
            }
            else
            {
                std::cerr << "\033[1;31m✘ Error en la operación de copia.\033[0m\n";
            }
        }
        else if (operacion == "help")
        {
            std::cout << "-----------------------------\n";
            std::cout << "Comandos disponibles:\n";
            std::cout << "  format                      - Formatea el disco virtual\n";
            std::cout << "  ls                          - Lista los archivos en el sistema\n";
            std::cout << "  cat <archivo>               - Muestra el contenido de un archivo\n";
            std::cout << "  write <texto> <archivo>     - Escribe texto en un archivo\n";
            std::cout << "  rm <archivo>                - Elimina un archivo\n";
            std::cout << "  hexdump <archivo>           - Muestra el volcado hexadecimal de un archivo\n";
            std::cout << "  copy in <archivo_host> <archivo_sistema> - Copia un archivo del host al sistema\n";
            std::cout << "  copy out <archivo_sistema> <archivo_host> - Copia un archivo del sistema al host\n";
            std::cout << "  exit                        - Sale del sistema de archivos\n";
            std::cout << "-----------------------------\n";
        }
        else if (operacion == "exit")
        {
            std::cout << "Saliendo del sistema de archivos...\n";
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
                dispositivo.file.seekg(0, std::ios::end);
                std::cout << "  tamaño: " << dispositivo.file.tellg() / 1024 << " Kb\n";
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