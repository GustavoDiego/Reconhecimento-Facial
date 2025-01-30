// database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <map>
#include <opencv2/opencv.hpp>

// Carrega o banco de dados a partir de um arquivo de configuração
std::map<std::string, cv::Mat> loadDatabase(const std::string& dbFile);

// Salva um embedding no banco de dados
bool saveEmbedding(const std::string& name, const cv::Mat& embedding, const std::string& dbFile);

#endif // DATABASE_H
