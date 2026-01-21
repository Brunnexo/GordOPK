// #########################################################################################################
// #########################################################################################################
// ####       ___              _   /\/           ___                                                    ####
// ####      / _ \___  _ __ __| | __ _  ___     / _ \_ __ ___   __ _ _ __ __ _ _ __ ___   __ _ ___      ####
// ####     / /_\/ _ \| '__/ _` |/ _` |/ _ \   / /_)/ '__/ _ \ / _` | '__/ _` | '_ ` _ \ / _` / __|     ####
// ####    / /_\\ (_) | | | (_| | (_| | (_) | / ___/| | | (_) | (_| | | | (_| | | | | | | (_| \__ \     ####
// ####    \____/\___/|_|  \__,_|\__,_|\___/  \/    |_|  \___/ \__, |_|  \__,_|_| |_| |_|\__,_|___/     ####
// ####                                                   |___/                                         ####
// #########################################################################################################
// #########################################################################################################
// Os verdadeiros créditos são os amigos que fazemos no caminho

// A fazer:
// - Tutorial de como obter ponteiros + básico de engenharia reversa
// - Tópico completo da DLL no Discord
// - Documentação do código

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#define BUF_SIZE 1024 * 32

#include <winsock2.h>
#include <windows.h>
#include <exception>
#include <string>
#include <iostream>
#include <ws2tcpip.h>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <shellapi.h>

#pragma comment(lib, "Ws2_32.lib")

// ######################################
// ##   +   DEFINIÇÕES GLOBAIS   +     ##
// ######################################

// Aqui você vai definir os ponteiros e 
// variáveis globais que serão usadas na DLL

// Lembre-se: valores hexadecimais
// começam com 0x!!                 \/
#define T_ADDRESS_PTR               0x14EFBD8
#define DOMAIN_ADDRESS_PTR          0x11764B0

#define CHECKSUM_FUN_ADDRESS        0x51A2B0
#define SEED_FUN_ADDRESS            0x51A490
#define CRAG_CONNECTION_PTR         0xBE8760

#define CONSOLE // Se quiser tirar o console, basta tirar ou comentar essa linha

// Basicamente, não precisa mudar nada no
// código, apenas os parâmetros acima.

// Curioso para entender como funciona?
// Fique a vontade para estudar!!
// Dúvidas? https://discord.gg/HyJjHK5zB2

// ######################################
// ##   -   DEFINIÇÕES GLOBAIS   -     ##
// ######################################


// ######################################
// ##    +   VALORES OPCIONAIS   +     ##
// ######################################

// Não precisa alterar os valores abaixo
// se você quiser utilizar argumentos de
// execução para sobrescrevê-los.
// Exemplo: ragexe.exe 1rag1 -no-hook -ip 172.65.175.70 -korePort 6902 -socketPort 2349

#define SOCKET_PORT				    2349            // -socketPort <porta>
#define KORE_PORT				    6901            // -korePort <porta>
#define KORE_IP                     "172.65.175.70" // -ip <endereço sem aspas>

// ######################################
// ##    -   VALORES OPCIONAIS   -     ##
// ######################################


// Tipagem - frescura de programador
// Um QWORD ocupa 8 bytes na memória, ou 64 bits
// É o equivalente a isso:
// 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
// O valor máximo inteiro é de 18.446.744.073.709.551.615 - por isso a RAM tá cara!
typedef unsigned long long QWORD;


// Assinatura de funções essenciais - o coração do código!
typedef char(__cdecl* CalculateChecksumFunction)(void*, int, DWORD, QWORD);
typedef QWORD(__cdecl* GetSeedFunction)(void*, int);
typedef void* (__stdcall* CRagConnection)(void);

// Struct
struct ChecksumResponse {
    char checksum;
    QWORD currentSeed;
    DWORD counter;
};

// Assinatura de funções da DLL
ChecksumResponse ProcessChecksumPacket(char* buffer, int len);
SOCKET CreateSocketServer(int port, const std::string& ip);
QWORD CalculateChecksum(char* buffer, int len, DWORD counter, QWORD seed);
QWORD GetSeed(char* buffer, int len);
void AllocateConsole();
std::vector<std::wstring> LerArgs();
void GravarArgs();

// Funções de hooking
DWORD WINAPI AddressOverrideThread(LPVOID lpParam);
DWORD WINAPI ChecksumSocketThread(LPVOID lpParam);

// Variáveis
bool isTaAddressOverwrited = false;
bool isDomainOverwrited = false;

QWORD newSeed;

bool keepMainThread = true;

bool noHook = false;
std::string staticIP = KORE_IP;

int korePort = KORE_PORT;
int socketPort = SOCKET_PORT;

// Adivinha o que essa função faz?
ChecksumResponse ProcessChecksumPacket(char* buffer, int len) {
    ChecksumResponse response = { 0, 0, 0 };

    if (IsBadCodePtr((FARPROC)CRAG_CONNECTION_PTR)) {
        std::cout << "Erro fatal!" << "Endereço de CRagConnection inválido!";
        Sleep(5000);
        return response;
    }

    char result;

    CRagConnection instance_r =reinterpret_cast<CRagConnection>(CRAG_CONNECTION_PTR);

    void* instance_ptr = instance_r();
    uintptr_t instance_addr = reinterpret_cast<uintptr_t>(instance_ptr);

    if (instance_addr) {
        DWORD counter = 0;

        if (len >= sizeof(DWORD)) {
            counter = ntohl(*(DWORD*)(buffer + len - sizeof(DWORD)));
        }

        int dataLen = len - sizeof(DWORD);
        if (dataLen < 0) dataLen = 0;

        if (!counter) {
            char rand_byte = (char)rand();
            result = rand_byte;
            buffer[dataLen] = rand_byte;
            newSeed = GetSeed(buffer, dataLen + 1);
            response.currentSeed = newSeed;
        }
        else {
            result = CalculateChecksum(buffer, dataLen, counter, newSeed);
            response.currentSeed = newSeed;
        }

        response.checksum = result;
        response.counter = counter;
    }

    return response;
}

// Threads
// A bendita thread do checksum!!
static DWORD WINAPI ChecksumSocketThread(LPVOID lpParam) {
    // Aloca buffer no heap para evitar uso excessivo da pilha
    char* buffer = new char[BUF_SIZE];

    const SOCKET serverSocket = CreateSocketServer(socketPort, staticIP);

    if (serverSocket == INVALID_SOCKET) {
        delete[] buffer;
        return EXIT_FAILURE;
    }

    while (keepMainThread) {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);

        const SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket != INVALID_SOCKET) {
            int bytesReceived;

            while ((bytesReceived = recv(clientSocket, buffer, BUF_SIZE, 0)) > 0) {
                ChecksumResponse result = ProcessChecksumPacket(buffer, bytesReceived);

                if (send(clientSocket, (char*)&result, sizeof(ChecksumResponse), 0) == SOCKET_ERROR) {
					std::cout << "[DLL] Erro no servidor checksum! Falha ao enviar resposta no socket!" << std::endl;
                    break;
                }
            }

            closesocket(clientSocket);
        }

        Sleep(100);
    }

    closesocket(serverSocket);
    delete[] buffer;
    // WSACleanup();
    return EXIT_SUCCESS;
}

// Substitui o endereço do servidor de autenticação e o servidor de domínio
// Lembra do Ghost.py? Faz a mesma coisa, só que internamente!
static DWORD WINAPI AddressOverrideThread(LPVOID lpParam) {
    const int len = 33;

    const char originalTaAddr[] = "lt-account-01.gnjoylatam.com:6951";
    const char originalDomain[] = "lt-account-01.gnjoylatam.com:6900";

    char value[len] = { 0 };

    std::ostringstream oss;
    oss << staticIP << ":" << korePort;
    std::string ipPortStr = oss.str();

    strncpy_s(value, ipPortStr.c_str(), min(size_t(32), ipPortStr.size()));

	std::cout << "[DLL] Overriding TA addr. and domain to: " << value << "..." << std::endl;

    while (!(isTaAddressOverwrited || isDomainOverwrited)) {
        try {
            char* taAddr = (char*)T_ADDRESS_PTR;

            if (!isTaAddressOverwrited && strcmp(taAddr, originalTaAddr) == 0) {
                memcpy(taAddr, value, len);
                isTaAddressOverwrited = true;
            }

            uintptr_t domainStrAddr = *(uintptr_t*)DOMAIN_ADDRESS_PTR;
            char* domain = (char*)domainStrAddr;

            if (!isDomainOverwrited && strcmp(domain, originalDomain) == 0) {
                memcpy(domain, value, len);
                isDomainOverwrited = true;
            }

            if (isTaAddressOverwrited && isDomainOverwrited) {
                break;
            }
        } catch (std::exception& e) {
            std::cout << "[DLL] Addr. override ERRO: " << e.what() << std::endl;
            break;
        }

        Sleep(100);
    }

    std::cout << "[DLL] Addr. override OK!" << std::endl;
    return EXIT_SUCCESS;
}

// Funções essenciais originais do jogo

// Calcula o checksum: obtém o pacote de envio e
// concatena com uma resultante ordenada de uma seed gerada
// aleatoriamente
static QWORD CalculateChecksum(char* buffer, int len, DWORD counter, QWORD seed) {
    static CalculateChecksumFunction fun = reinterpret_cast<CalculateChecksumFunction>(CHECKSUM_FUN_ADDRESS);
    return fun(buffer, len, counter, seed);
}

// Gera a seed inicial para o cálculo do checksum
// Se já jogou Minecraft, sabe o que é seed... mas se não jogou:
// Imagine que seed é um número aleatório inicial que serve como base
// de cálculo de uma fórmula que, a partir de certos parâmetros, gera
// uma resultante previsível e controlável pelo servidor
static QWORD GetSeed(char* buffer, int len) {
    static GetSeedFunction fun = reinterpret_cast<GetSeedFunction>(SEED_FUN_ADDRESS);
    return fun(buffer, len);
}

// Cria um server socket simples
// É o ponto de encontro entre o OpenKore e o jogo!
// O que é socket? https://pt.wikipedia.org/wiki/Soquete_de_rede
SOCKET CreateSocketServer(int port, const std::string& ip) {
    sockaddr_in addr;
    DWORD opt = 1;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(port));

    if (ip.empty()) {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
    }

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(sock);
        return INVALID_SOCKET;
    }

    if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(sock);
        return INVALID_SOCKET;
    }

    return sock;
}

// Função que aloca o console
// Tela preta chata, só serve para avaliar se o programa
// está rodando corretamente...
static void AllocateConsole() {
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
#ifdef UNICODE
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTitle(L"Console");
#else
    SetConsoleTitle("Console");
#endif
}

// Essa função LÊ os argumentos de execução do processo
static std::vector<std::wstring> LerArgs() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    std::vector<std::wstring> args;
    if (argv) {
        for (int i = 0; i < argc; i++) {
            args.emplace_back(argv[i]);
        }
        LocalFree(argv);
    }
    return args;
}

// Essa funçao GRAVA os argumentos de execução, lembra delas?
static void GravarArgs() {
    const static std::vector<std::wstring> args = LerArgs();

    auto hasNext = [&](int i) -> bool {
        return (i + 1 < args.size());
        };

    for (int i = 0; i < args.size(); i++) {
        const std::wstring& arg = args[i];

        if (arg == L"sem-lisinho" || arg == L"-no-hook" || arg == L"-nh") {
            noHook = true;
            continue;
        }

        if (arg == L"-ip") {
            if (hasNext(i)) {
                const auto& ip = args[i + 1];
                staticIP = std::string(ip.begin(), ip.end());
            }
            continue;
        }

        if (arg == L"-korePort" || arg == L"-kp") {
            if (hasNext(i)) korePort = std::stoi(args[i + 1]);
            continue;
        }

        if (arg == L"-socketPort" || arg == L"-sp") {
            if (hasNext(i)) socketPort = std::stoi(args[i + 1]);
            continue;
        }
    }
}


// Essa função é chamada quando a DLL é carregada no jogo
static void Main() {
#ifdef CONSOLE
	AllocateConsole();
#endif
	GravarArgs();

    if (!noHook) {
        CreateThread(NULL, 0, AddressOverrideThread, NULL, 0, NULL);
        CreateThread(NULL, 0, ChecksumSocketThread, NULL, 0, NULL);
    }
}

// A DLL começa aqui, não há necessidade de modificar
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  reason, LPVOID lpReserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        Main();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}