#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

struct Material {
    string name;
    string unit;
    double consumptionPerM2;
};

struct WorkType {
    string name;
    string surface;
    vector<Material> materials;
};

vector<WorkType> loadWorkTypes(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    json j;
    file >> j;

    vector<WorkType> works;
    for (const auto& item : j) {
        WorkType wt;
        wt.name = item.value("name", "Unnamed");
        wt.surface = item.value("surface", "walls");

        if (item.contains("materials") && item["materials"].is_array()) {
            for (const auto& mat : item["materials"]) {
                Material m;
                m.name = mat.value("name", "Material");
                m.unit = mat.value("unit", "unit");
                m.consumptionPerM2 = mat.value("consumption_per_m2", 0.0);
                wt.materials.push_back(m);
            }
        }
        works.push_back(wt);
    }
    return works;
}

void displayWorkTypes(const vector<WorkType>& works) {
    cout << "\nAvailable work types:\n";
    for (size_t i = 0; i < works.size(); ++i) {
        cout << "  " << i + 1 << ". " << works[i].name << " (surface: " << works[i].surface << ")\n";
    }
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    try {
        string roomName;
        double height, width, length;

        cout << "Enter room name: ";
        getline(cin, roomName);

        cout << "Enter height (m): ";
        cin >> height;
        cout << "Enter width (m): ";
        cin >> width;
        cout << "Enter length (m): ";
        cin >> length;

        double wallArea = 2.0 * height * (width + length);
        double ceilingArea = width * length;
        double floorArea = width * length;

        cout << "\n--- Room areas ---\n";
        cout << "Walls   : " << fixed << setprecision(2) << wallArea << " m2\n";
        cout << "Ceiling : " << ceilingArea << " m2\n";
        cout << "Floor   : " << floorArea << " m2\n";

        vector<WorkType> works = loadWorkTypes("works.json");

        displayWorkTypes(works);
        cout << "Select a work type: ";
        int choice;
        cin >> choice;
        clearInput();

        if (choice < 1 || choice > static_cast<int>(works.size())) {
            throw runtime_error("Invalid selection.");
        }

        const WorkType& selected = works[choice - 1];

        double area = 0.0;
        if (selected.surface == "walls") {
            area = wallArea;
        }
        else if (selected.surface == "ceiling") {
            area = ceilingArea;
        }
        else if (selected.surface == "floor") {
            area = floorArea;
        }
        else {
            throw runtime_error("Unknown surface type: " + selected.surface);
        }

        cout << "\n--- Work: " << selected.name << " (applied to " << selected.surface << ") ---\n";
        cout << "Surface: " << fixed << setprecision(2) << area << " m2\n\n";

        vector<pair<Material, double>> materialsWithAmount;
        cout << "Required materials:\n";
        for (const auto& mat : selected.materials) {
            double amount = mat.consumptionPerM2 * area;
            materialsWithAmount.push_back({ mat, amount });
            cout << "  " << mat.name << " : " << fixed << setprecision(2) << amount << " " << mat.unit << "\n";
        }

        cout << "\n--- Enter cost per unit ---\n";
        double totalCost = 0.0;
        vector<pair<string, double>> costDetails;

        for (auto& item : materialsWithAmount) {
            const Material& mat = item.first;
            double amount = item.second;

            double unitCost;
            cout << "Cost per " << mat.unit << " for " << mat.name << ": ";
            cin >> unitCost;
            clearInput();

            double materialCost = amount * unitCost;
            totalCost += materialCost;
            costDetails.push_back({ mat.name, materialCost });
        }

        cout << "\n========== RENOVATION COST SUMMARY ==========\n";
        cout << "Room          : " << roomName << "\n";
        cout << "Work type     : " << selected.name << "\n";
        cout << "Surface area  : " << fixed << setprecision(2) << area << " m2\n";
        cout << "Materials and costs:\n";
        for (const auto& detail : costDetails) {
            cout << "  " << detail.first << " : " << fixed << setprecision(2) << detail.second << " ₽\n";
        }
        cout << "----------------------------------------------\n";
        cout << "Total cost    : " << fixed << setprecision(2) << totalCost << " ₽\n";
        cout << "==============================================\n";

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}