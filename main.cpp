// main.cpp
#include <iostream>
#include "face_registration.h"
#include "face_recognition.h"
#include <clocale>

int main() {
    int escolha;
    setlocale(LC_ALL, "portuguese");

    while (true) {
        std::cout << "===================================" << std::endl;
        std::cout << "   Sistema de Reconhecimento Facial " << std::endl;
        std::cout << "===================================" << std::endl;
        std::cout << "1. Cadastrar nova pessoa" << std::endl;
        std::cout << "2. Reconhecer pessoa" << std::endl;
        std::cout << "3. Sair" << std::endl;
        std::cout << "Escolha uma opção: ";
        std::cin >> escolha;
        std::cin.ignore(); // Limpar o buffer

        if (escolha == 1) {
            cadastrarPessoa();
        }
        else if (escolha == 2) {
            verificarIdentidade();
        }
        else if (escolha == 3) {
            std::cout << "Saindo do sistema." << std::endl;
            break;
        }
        else {
            std::cout << "Opção inválida. Tente novamente." << std::endl;
        }
    }

    return 0;
}
