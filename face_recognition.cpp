// face_recognition.cpp
#include "face_recognition.h"
#include "database.h"
#include "utils.h"
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <map>

// Funções auxiliares (detectFaceDNN, extractEmbedding, cosineSimilarity) podem estar em utils.cpp


void verificarIdentidade() {
    // Carrega os modelos
    cv::dnn::Net faceNet = cv::dnn::readNetFromCaffe("models/deploy.prototxt", "models/res10_300x300_ssd_iter_140000.caffemodel");
    if (faceNet.empty()) {
        std::cerr << "Não foi possível carregar o modelo de detecção facial." << std::endl;
        logError("Falha ao carregar o modelo de detecção facial.");
        return;
    }

    cv::dnn::Net embedNet = cv::dnn::readNetFromTorch("models/openface.nn4.small2.v1.t7");
    if (embedNet.empty()) {
        std::cerr << "Não foi possível carregar o modelo de embeddings faciais." << std::endl;
        logError("Falha ao carregar o modelo de embeddings faciais.");
        return;
    }

    // Solicita o nome da pessoa
    std::string personName;
    std::cout << "Digite o seu nome para verificação: ";
    std::getline(std::cin, personName);

    // Carrega o banco de dados
    std::map<std::string, cv::Mat> database = loadDatabase("database_config.txt");
    if (database.empty()) {
        std::cerr << "Banco de dados de embeddings vazio. Por favor, cadastre antes." << std::endl;
        logError("Banco de dados vazio durante a verificação para " + personName);
        return;
    }

    // Verifica se o nome existe no banco de dados
    if (database.find(personName) == database.end()) {
        std::cerr << "Nome não encontrado no banco de dados. Por favor, cadastre-se primeiro." << std::endl;
        logError("Nome não encontrado durante a verificação: " + personName);
        return;
    }

    cv::Mat storedEmbedding = database[personName];

    // Acessa a câmera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Não foi possível acessar a câmera." << std::endl;
        logError("Falha ao acessar a câmera durante a verificação para " + personName);
        return;
    }

    std::cout << "Iniciando verificação de identidade. Pressione 's' para capturar a imagem ou 'q' para sair." << std::endl;

    cv::Mat frame;
    bool verificado = false;
    int tentativas = 0;
    const int maxTentativas = 5;

    while (tentativas < maxTentativas && !verificado) {
        cap >> frame;
        if (frame.empty()) break;

        // Detecta o rosto
        cv::Rect faceRect = detectFaceDNN(frame, faceNet);
        if (faceRect.width > 0 && faceRect.height > 0) {
            // Desenha um retângulo no rosto detectado
            cv::rectangle(frame, faceRect, cv::Scalar(0, 255, 0), 2);
            cv::putText(frame, "Pressione 's' para capturar", cv::Point(faceRect.x, faceRect.y - 10),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);

            cv::imshow("Verificação de Identidade - Pressione 's' para capturar", frame);
        }

        char c = (char)cv::waitKey(30);
        if (c == 'q' || c == 'Q') {
            break;
        }
        else if (c == 's' || c == 'S') {
            if (faceRect.width > 0 && faceRect.height > 0) {
                cv::Mat faceROI = frame(faceRect).clone();
                // Extrai o embedding
                cv::Mat emb = extractEmbedding(faceROI, embedNet);

                // Calcula a similaridade com o embedding armazenado
                double similarity = cosineSimilarity(emb, storedEmbedding);

                std::cout << "Similaridade: " << similarity << std::endl;

                if (similarity > 0.65) { // Ajuste o limiar conforme necessário
                    std::cout << "Usuário logado com sucesso!" << std::endl;
                    logInfo("Usuário logado: " + personName);
                    verificado = true;
                    break;
                }
                else {
                    std::cout << "Verificação falhou. Rosto não corresponde ao nome fornecido." << std::endl;
                    logError("Falha na verificação para: " + personName + " - Similaridade: " + std::to_string(similarity));
                }

                tentativas++;
                std::cout << "Tentativas restantes: " << (maxTentativas - tentativas) << std::endl;
            }
            else {
                std::cout << "Nenhum rosto detectado. Tente novamente." << std::endl;
                logError("Nenhum rosto detectado durante a verificação para " + personName);
                tentativas++;
                std::cout << "Tentativas restantes: " << (maxTentativas - tentativas) << std::endl;
            }
        }
    }

    cap.release();
    cv::destroyAllWindows();

    if (verificado) {
        std::cout << "Usuário logado" << std::endl;
       
    }
    else {
        std::cout << "Número máximo de tentativas alcançado. Verificação falhou." << std::endl;
        logError("Número máximo de tentativas alcançado para " + personName);
    }
}