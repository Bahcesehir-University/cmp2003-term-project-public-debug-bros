#include "analyzer.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <map>

static std::unordered_map<std::string, long long> globalZoneMap;
static std::unordered_map<std::string, std::map<int, long long>> globalSlotMap;

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) return;

    globalZoneMap.clear();
    globalSlotMap.clear();

    std::string line;
    if (!std::getline(file, line)) return;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string tripID, pickupZone, dropoffZone, timestamp, dist, fare;

        if (!std::getline(ss, tripID, ',') ||
            !std::getline(ss, pickupZone, ',') ||
            !std::getline(ss, dropoffZone, ',') ||
            !std::getline(ss, timestamp, ',') ||
            !std::getline(ss, dist, ',') ||
            !std::getline(ss, fare, ',')) {
            continue;
        }

        if (pickupZone.empty() || timestamp.length() < 13) continue;

        try {
            int hour = std::stoi(timestamp.substr(11, 2));
            if (hour >= 0 && hour <= 23) {
                globalZoneMap[pickupZone]++;
                globalSlotMap[pickupZone][hour]++;
            }
        } catch (...) {
            continue;
        }
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    std::vector<ZoneCount> result;
    for (const auto& p : globalZoneMap) {
        result.push_back({p.first, p.second});
    }

    std::sort(result.begin(), result.end(),
        [](const ZoneCount& a, const ZoneCount& b) {
            if (a.count != b.count) return a.count > b.count;
            return a.zone < b.zone;
        });

    if ((int)result.size() > k) result.resize(k);
    return result;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    std::vector<SlotCount> result;
    for (const auto& z : globalSlotMap) {
        for (const auto& h : z.second) {
            result.push_back({z.first, h.first, h.second});
        }
    }

    std::sort(result.begin(), result.end(),
        [](const SlotCount& a, const SlotCount& b) {
            if (a.count != b.count) return a.count > b.count;
            if (a.zone != b.zone) return a.zone < b.zone;
            return a.hour < b.hour;
        });

    if ((int)result.size() > k) result.resize(k);
    return result;
}
