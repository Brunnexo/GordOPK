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
#include <fstream> // Adicione este include no topo do arquivo
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")

// ######################################
// ##        DEFINIÇÕES GLOBAIS        ##
// ######################################

// Aqui você vai definir os ponteiros e 
// variáveis globais que serão usadas na DLL

// Lembre-se, os endereços começam com o 0x!!
//                                  \/
#define T_ADDRESS_PTR               0x14EFBD8
#define DOMAIN_ADDRESS_PTR          0x11764B0

#define CHECKSUM_FUN_ADDRESS        0x51A2B0
#define SEED_FUN_ADDRESS            0x51A490
#define CRAG_CONNECTION_PTR         0xBE8760

// Isso aqui não é endereço, então não use 0x
#define SOCKET_PORT				    2349
#define KORE_PORT				    6902
// Sabe aquele endereço IP que você define no
// adaptador de rede? Coloque ele aqui! ENTRE ASPAS!!
#define KORE_IP                     "172.65.175.70"

// Se quiser tirar o console, basta tirar essa linha abaixo
#define CONSOLE


// Basicamente, não precisa mudar nada no
// código, apenas os parâmetros acima.

// Curioso para entender como funciona?
// Fique a vontade para estudar!!
// Dúvidas? https://discord.gg/HyJjHK5zB2

// ######################################
// ##              FIM!                ##
// ######################################


// Tipagem - frescura de programador
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


// Funções de hooking
DWORD WINAPI AddressOverrideThread(LPVOID lpParam);
DWORD WINAPI ChecksumSocketThread(LPVOID lpParam);

// Variáveis
bool isTaAddressOverwrited = false;
bool isDomainOverwrited = false;
QWORD newSeed;
bool keepMainThread = true;

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

// A bendita thread do checksum
static DWORD WINAPI ChecksumSocketThread(LPVOID lpParam) {
    const SOCKET serverSocket = CreateSocketServer(SOCKET_PORT, KORE_IP);

    if (serverSocket == INVALID_SOCKET) {
        return EXIT_FAILURE;
    }

    while (keepMainThread) {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);

        const SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket != INVALID_SOCKET) {
            char buffer[BUF_SIZE];
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
    // WSACleanup();
    return EXIT_SUCCESS;
}

// Substitui o endereço do servidor de autenticação e o domínio
// Lembra do Ghost.py? Faz a mesma coisa só que internamente!
static DWORD WINAPI AddressOverrideThread(LPVOID lpParam) {
    const int len = 33;

    const char originalTaAddr[] = "lt-account-01.gnjoylatam.com:6951";
    const char originalDomain[] = "lt-account-01.gnjoylatam.com:6900";

    char value[len] = { 0 };

    std::ostringstream oss;
    oss << KORE_IP << ":" << KORE_PORT;

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
        } catch (std::exception&) {
			// Se chegou aqui, deu erro!
			// É uma DLL básica! Se quiser implementar algum log, fique à vontade!!
            break;
        }

        Sleep(100);
    }


    std::cout << "[DLL] Addr. override OK!" << std::endl;
    return EXIT_SUCCESS;
}

static QWORD CalculateChecksum(char* buffer, int len, DWORD counter, QWORD seed) {
    static CalculateChecksumFunction fun = reinterpret_cast<CalculateChecksumFunction>(CHECKSUM_FUN_ADDRESS);
    return fun(buffer, len, counter, seed);
}

static QWORD GetSeed(char* buffer, int len) {
    static GetSeedFunction fun = reinterpret_cast<GetSeedFunction>(SEED_FUN_ADDRESS);

    return fun(buffer, len);
}

// Cria um server socket simples
// O que é socket? https://pt.wikipedia.org/wiki/Soquete_de_rede
SOCKET CreateSocketServer(int port, const std::string& ip) {
    sockaddr_in addr;
    DWORD opt = 1;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == INVALID_SOCKET)
        return INVALID_SOCKET;

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(port));

    if (ip.empty()) {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
    else {
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

static void AllocateConsole() {
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
#ifdef UNICODE
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTitle(L"Gordolog");
#else
    SetConsoleTitle("Console");
#endif
}


// Essa função é chamada quando a DLL é carregada no jogo
static void Main() {
#ifdef CONSOLE
	AllocateConsole();
#endif

    CreateThread(NULL, 0, AddressOverrideThread, NULL, 0, NULL);
    CreateThread(NULL, 0, ChecksumSocketThread, NULL, 0, NULL);
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