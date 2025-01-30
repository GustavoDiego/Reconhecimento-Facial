// face_recognition.cpp
#include "face_recognition.h"
#include "database.h"
#include "utils.h"
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <map>

// Fun��es auxiliares (detectFaceDNN, extractEmbedding, cosineSimilarity) podem estar em utils.cpp


void verificarIdentidade() {
    // Carrega os modelos
    cv::dnn::Net faceNet = cv::dnn::readNetFromCaffe("models/deploy.prototxt", "models/res10_300x300_ssd_iter_140000.caffemodel");
    if (faceNet.empty()) {
        std::cerr << "N�o foi poss�vel carregar o modelo de detec��o facial." << std::endl;
        logError("Falha ao carregar o modelo de detec��o facial.");
        return;
    }

    cv::dnn::Net embedNet = cv::dnn::readNetFromTorch("models/openface.nn4.small2.v1.t7");
    if (embedNet.empty()) {
        std::cerr << "N�o foi poss�vel carregar o modelo de embeddings faciais." << std::endl;
        logError("Falha ao carregar o modelo de embeddings faciais.");
        return;
    }

    // Solicita o nome da pessoa
    std::string personName;
    std::cout << "Digite o seu nome para verifica��o: ";
    std::getline(std::cin, personName);

    // Carrega o banco de dados
    std::map<std::string, cv::Mat> database = loadDatabase("database_config.txt");
    if (database.empty()) {
        std::cerr << "Banco de dados de embeddings vazio. Por favor, cadastre antes." << std::endl;
        logError("Banco de dados vazio durante a verifica��o para " + personName);
        return;
    }

    // Verifica se o nome existe no banco de dados
    if (database.find(personName) == database.end()) {
        std::cerr << "Nome n�o encontrado no banco de dados. Por favor, cadastre-se primeiro." << std::endl;
        logError("Nome n�o encontrado durante a verifica��o: " + personName);
        return;
    }

    cv::Mat storedEmbedding = database[personName];

    // Acessa a c�mera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "N�o foi poss�vel acessar a c�mera." << std::endl;
        logError("Falha ao acessar a c�mera durante a verifica��o para " + personName);
        return;
    }

    std::cout << "Iniciando verifica��o de identidade. Pressione 's' para capturar a imagem ou 'q' para sair." << std::endl;

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
            // Desenha um ret�ngulo no rosto detectado
            cv::rectangle(frame, faceRect, cv::Scalar(0, 255, 0), 2);
            cv::putText(frame, "Pressione 's' para capturar", cv::Point(faceRect.x, faceRect.y - 10),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);

            cv::imshow("Verifica��o de Identidade - Pressione 's' para capturar", frame);
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

                if (similarity > 0.65) { // Ajuste o limiar conforme necess�rio
                    std::cout << "Usu�rio logado com sucesso!" << std::endl;
                    logInfo("Usu�rio logado: " + personName);
                    verificado = true;
                    break;
                }
                else {
                    std::cout << "Verifica��o falhou. Rosto n�o corresponde ao nome fornecido." << std::endl;
                    logError("Falha na verifica��o para: " + personName + " - Similaridade: " + std::to_string(similarity));
                }

                tentativas++;
                std::cout << "Tentativas restantes: " << (maxTentativas - tentativas) << std::endl;
            }
            else {
                std::cout << "Nenhum rosto detectado. Tente novamente." << std::endl;
                logError("Nenhum rosto detectado durante a verifica��o para " + personName);
                tentativas++;
                std::cout << "Tentativas restantes: " << (maxTentativas - tentativas) << std::endl;
            }
        }
    }

    cap.release();
    cv::destroyAllWindows();

    if (verificado) {
        std::cout << "Usu�rio logado" << std::endl;
       
    }
    else {
        std::cout << "N�mero m�ximo de tentativas alcan�ado. Verifica��o falhou." << std::endl;
        logError("N�mero m�ximo de tentativas alcan�ado para " + personName);
    }
}