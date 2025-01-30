// database.cpp
#include "database.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>

std::map<std::string, cv::Mat> loadDatabase(const std::string& dbFile) {
    std::map<std::string, cv::Mat> database;
    std::ifstream infile(dbFile);
    if (!infile.is_open()) {
        std::cerr << "Não foi possível abrir " << dbFile << ". Um novo banco de dados será criado." << std::endl;
        return database;
    }

    std::string name;
    while (std::getline(infile, name)) {
        std::string filename;
        if (std::getline(infile, filename)) {
            cv::FileStorage fs(filename, cv::FileStorage::READ);
            if (!fs.isOpened()) {
                std::cerr << "Não foi possível abrir " << filename << std::endl;
                continue;
            }
            cv::Mat emb;
            fs["embedding"] >> emb;
            fs.release();
            if (!emb.empty()) {
                database[name] = emb;
            }
            else {
                std::cerr << "Embedding vazio para " << name << " em " << filename << std::endl;
            }
        }
    }

    infile.close();
    return database;
}

bool saveEmbedding(const std::string& name, const cv::Mat& embedding, const std::string& dbFile) {
    
    auto existingDatabase = loadDatabase(dbFile);

    
    std::string filename = name + ".xml";

    
    if (existingDatabase.find(name) != existingDatabase.end()) {
        std::cout << "Atualizando embedding existente para: " << name << std::endl;
        
    }

    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "Não foi possível abrir " << filename << " para escrita." << std::endl;
        return false;
    }
    fs << "embedding" << embedding;
    fs.release();

    
    existingDatabase[name] = embedding;

    std::ofstream outfile(dbFile, std::ios::out | std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "Não foi possível abrir " << dbFile << " para atualização." << std::endl;
        return false;
    }
    for (const auto& [existingName, mat] : existingDatabase) {
        outfile << existingName << std::endl;
        outfile << existingName << ".xml" << std::endl;
    }
    outfile.close();

    return true;
}
