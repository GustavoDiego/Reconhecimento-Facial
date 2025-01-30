// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <string>

bool isValidName(const std::string& name);
// Detecta rosto usando DNN
cv::Rect detectFaceDNN(cv::Mat& frame, cv::dnn::Net& faceNet);

// Extrai o embedding de uma imagem de rosto
cv::Mat extractEmbedding(cv::Mat& faceImg, cv::dnn::Net& embedNet);

// Calcula a similaridade do cosseno entre dois vetores
double cosineSimilarity(const cv::Mat& vecA, const cv::Mat& vecB);

void logInfo(const std::string& message);
void logError(const std::string& message);


#endif // UTILS_H
