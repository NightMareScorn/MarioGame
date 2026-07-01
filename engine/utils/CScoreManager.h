#pragma once
#include <string>
#include <map>
#include <fstream>
#include <sstream>

struct LevelScore {
    long score = 0;
    std::string time = "--:--";
};

class CScoreManager {
    static CScoreManager* __instance;
    std::map<std::string, LevelScore> scores;
    std::string filePath = "content/configs/scores.txt";

public:
    static CScoreManager* GetInstance() {
        if (__instance == nullptr) __instance = new CScoreManager();
        return __instance;
    }

    void Load() {
        std::ifstream file(filePath);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::string id, time;
            long score;
            if (std::getline(ss, id, ',') && (ss >> score) && std::getline(ss >> std::ws, time)) {
                scores[id] = { score, time };
            }
        }
        file.close();
    }

    void Save() {
        std::ofstream file(filePath);
        for (auto const& [id, best] : scores) {
            file << id << "," << best.score << "," << best.time << "\n";
        }
        file.close();
    }

    LevelScore GetBest(std::string levelId) {
        if (scores.find(levelId) == scores.end()) return { 0, "--:--" };
        return scores[levelId];
    }

    void UpdateBest(std::string levelId, long score, std::string time) {
        if (scores.find(levelId) == scores.end() || score > scores[levelId].score) {
            scores[levelId] = { score, time };
            Save();
        }
    }
};
