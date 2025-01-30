// utils.cpp
#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <cmath>
#include <regex>
#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip> 

// Função para validar o nome da pessoa
bool isValidName(const std::string& name) {
    // Permitir apenas letras, números e espaços
    std::regex pattern("^[A-Za-z0-9 ]+$");
    return std::regex_match(name, pattern);
}

// Função para registrar informações (logs de informação)
void logInfo(const std::string& message) {
    std::ofstream logFile("log_info.txt", std::ios::app);
    if (logFile.is_open()) {
        
        std::time_t now = std::time(nullptr);
        std::tm* tm_ptr = std::localtime(&now);
        if (tm_ptr != nullptr) { 
            logFile << "[" << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S") << "] INFO: " << message << std::endl;
        }
        else {
            logFile << "[Unknown Time] INFO: " << message << std::endl;
        }
        logFile.close();
    }
}

// Função para registrar erros (logs de erro)
void logError(const std::string& message) {
    std::ofstream logFile("log_error.txt", std::ios::app);
    if (logFile.is_open()) {
        // Adiciona timestamp de forma segura
        std::time_t now = std::time(nullptr);
        std::tm* tm_ptr = std::localtime(&now);
        if (tm_ptr != nullptr) { // Verifica se localtime não retornou nullptr
            logFile << "[" << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S") << "] ERROR: " << message << std::endl;
        }
        else {
            logFile << "[Unknown Time] ERROR: " << message << std::endl;
        }
        logFile.close();
    }
}

// Função para detectar o rosto com DNN
cv::Rect detectFaceDNN(cv::Mat& frame, cv::dnn::Net& faceNet) {
    cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0));
    faceNet.setInput(blob);
    cv::Mat detections = faceNet.forward();

    float confidenceThreshold = 0.7;
    cv::Rect faceRect;
    cv::Mat detections_2d = detections.reshape(1, detections.total() / 7);

    for (int i = 0; i < detections_2d.rows; i++) {
        float confidence = detections_2d.at<float>(i, 2);
        if (confidence > confidenceThreshold) {
            float x1 = detections_2d.at<float>(i, 3) * frame.cols;
            float y1 = detections_2d.at<float>(i, 4) * frame.rows;
            float x2 = detections_2d.at<float>(i, 5) * frame.cols;
            float y2 = detections_2d.at<float>(i, 6) * frame.rows;
            faceRect = cv::Rect(cv::Point((int)x1, (int)y1), cv::Point((int)x2, (int)y2));
            break; // Pega apenas o primeiro rosto detectado
        }
    }

    return faceRect;
}

// Função para extrair embedding com OpenFace
cv::Mat extractEmbedding(cv::Mat& faceImg, cv::dnn::Net& embedNet) {
    cv::Mat faceRGB;
    cv::cvtColor(faceImg, faceRGB, cv::COLOR_BGR2RGB);
    cv::Mat resized;
    cv::resize(faceRGB, resized, cv::Size(96, 96));
    resized.convertTo(resized, CV_32F, 1.0 / 255.0);

    cv::Mat blob = cv::dnn::blobFromImage(resized, 1.0, cv::Size(96, 96), cv::Scalar(0, 0, 0), false, false);
    embedNet.setInput(blob);
    cv::Mat embedding = embedNet.forward().clone(); // 1x128
    return embedding;
}

// Calcula a similaridade do cosseno entre dois vetores
double cosineSimilarity(const cv::Mat& vecA, const cv::Mat& vecB) {
    double dot = vecA.dot(vecB);
    double normA = cv::norm(vecA);
    double normB = cv::norm(vecB);
    if (normA == 0 || normB == 0) return 0.0;
    return dot / (normA * normB);
}
