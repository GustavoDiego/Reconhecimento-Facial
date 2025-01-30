#include "face_registration.h"
#include "utils.h"
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include "database.h"
#include <iostream>
#include <vector>
#include <string>


void cadastrarPessoa() {
    // Carrega os modelos
    cv::dnn::Net faceNet = cv::dnn::readNetFromCaffe("models/deploy.prototxt", "models/res10_300x300_ssd_iter_140000.caffemodel");
    if (faceNet.empty()) {
        logError("Não foi possível carregar o modelo de detecção facial.");
        std::cerr << "Não foi possível carregar o modelo de detecção facial." << std::endl;
        return;
    }

    cv::dnn::Net embedNet = cv::dnn::readNetFromTorch("models/openface.nn4.small2.v1.t7");
    if (embedNet.empty()) {
        logError("Não foi possível carregar o modelo de embeddings faciais.");
        std::cerr << "Não foi possível carregar o modelo de embeddings faciais." << std::endl;
        return;
    }

    // Solicita o nome da pessoa
    std::string personName;
    std::cout << "Digite o nome da pessoa a ser cadastrada: ";
    std::getline(std::cin, personName);

    // Valida o nome
    if (!isValidName(personName)) {
        logError("Nome inválido inserido: " + personName);
        std::cerr << "Erro: O nome contém caracteres inválidos. Utilize apenas letras, números e espaços." << std::endl;
        return;
    }

    // Verifica se a pessoa já está cadastrada
    std::map<std::string, cv::Mat> database = loadDatabase("database_config.txt");
    if (database.find(personName) != database.end()) {
        logError("Tentativa de cadastrar pessoa já existente: " + personName);
        std::cerr << "Erro: A pessoa '" << personName << "' já está cadastrada no banco de dados." << std::endl;
        return;
    }

    // Acessa a câmera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Não foi possível acessar a câmera." << std::endl;
        return;
    }

    // Instruções iniciais
    std::cout << "Por favor, tire pelo menos 20 fotos do rosto para melhor resultado." << std::endl;
    std::cout << "Recomendamos capturar imagens com diferentes expressões, iluminações e ângulos." << std::endl;
    std::cout << "Pressione 's' para capturar cada imagem e 'q' para finalizar a captura antecipadamente." << std::endl;

    cv::Mat frame;
    std::vector<cv::Mat> embeddings;
    int contador = 0;
    const int totalFotos = 20;

    while (contador < totalFotos) {
        cap >> frame;
        if (frame.empty()) {
            break;
        }

        // Detectar o rosto a cada frame para desenhar o retângulo
        cv::Rect faceRect = detectFaceDNN(frame, faceNet);
        if (faceRect.width > 0 && faceRect.height > 0) {
            // Desenha um quadrado ao redor da face
            cv::rectangle(frame, faceRect, cv::Scalar(0, 255, 0), 2);
            // Instrução sobre pressionar 's'
            cv::putText(
                frame,
                "Pressione 's' para capturar a foto",
                cv::Point(faceRect.x, std::max(faceRect.y - 10, 20)),
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 255, 0),
                1
            );
        }

        // Instruções adicionais na tela
        cv::putText(
            frame,
            "Movimente-se e varie a iluminação e o angulo do rosto",
            cv::Point(10, 30),
            cv::FONT_HERSHEY_SIMPLEX,
            0.5,
            cv::Scalar(255, 255, 255),
            1
        );
        cv::putText(
            frame,
            "Fotos capturadas: " + std::to_string(contador) + "/" + std::to_string(totalFotos),
            cv::Point(10, 50),
            cv::FONT_HERSHEY_SIMPLEX,
            0.5,
            cv::Scalar(255, 255, 255),
            1
        );

        cv::imshow("Cadastro Facial", frame);
        char c = (char)cv::waitKey(30);
        if (c == 'q' || c == 'Q') {
            // Finaliza a captura antecipadamente
            break;
        }
        else if (c == 's' || c == 'S') {
            if (faceRect.width > 0 && faceRect.height > 0) {
                // Extrai a região do rosto detectado
                cv::Mat faceROI = frame(faceRect).clone();
                // Extrai o embedding
                cv::Mat emb = extractEmbedding(faceROI, embedNet);
                embeddings.push_back(emb);
                contador++;
                std::cout << "Imagem capturada: " << contador << "/" << totalFotos << std::endl;
            }
            else {
                std::cout << "Nenhum rosto detectado. Tente novamente." << std::endl;
            }
        }
    }

    cap.release();
    cv::destroyAllWindows();

    if (embeddings.empty()) {
        std::cerr << "Nenhum embedding foi capturado. Cadastro não realizado." << std::endl;
        return;
    }

    // Calcula a média dos embeddings
    cv::Mat meanEmb = cv::Mat::zeros(1, embeddings[0].cols, CV_32F);
    for (auto& emb : embeddings) {
        meanEmb += emb;
    }
    meanEmb = meanEmb / static_cast<float>(embeddings.size());

    // Salva o embedding médio no banco de dados
    bool sucesso = saveEmbedding(personName, meanEmb, "database_config.txt");
    if (sucesso) {
        logInfo("Cadastro concluído para a pessoa: " + personName);
        std::cout << "Cadastro concluído. Embedding salvo para " << personName << "." << std::endl;
    }
    else {
        logError("Erro ao salvar o embedding para a pessoa: " + personName);
        std::cerr << "Erro ao salvar o embedding." << std::endl;
    }
}
