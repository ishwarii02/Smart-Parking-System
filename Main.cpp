#include <iostream>
#include <vector>
#include <map>
#include <climits>
#include <ctime>
#include <algorithm>

using namespace std;

class ParkingSlot {
public:
    int slotId;
    bool isAvailable;
    double costPerHour;

    ParkingSlot(int id, bool available, double cost) {
        this->slotId = id;
        this->isAvailable = available;
        this->costPerHour = cost;
    }
};

class SmartParkingSystem {
private:
    static const int NUM_SLOTS = 10;
    static ParkingSlot* slots[NUM_SLOTS];
    static double graph[NUM_SLOTS][NUM_SLOTS];
    static double distances[NUM_SLOTS];
    static bool visited[NUM_SLOTS];

public:
    static void initializeSystem() {
        double baseCost = 100; // Base cost for Slot 1
        double decrement = 10; // Cost decrement for subsequent slots

        for (int i = 0; i < NUM_SLOTS; i++) {
            double cost = baseCost - (i * decrement); // Slot costs decrease by decrement
            slots[i] = new ParkingSlot(i + 1, true, cost); // Initialize slot with cost
        }

        for (int i = 0; i < NUM_SLOTS; i++) {
            for (int j = 0; j < NUM_SLOTS; j++) {
                if (i != j) {
                    graph[i][j] = (i + j) % 3 == 0 ? 1.5 : 0; // Random distance initialization
                } else {
                    graph[i][j] = 0; // No distance to itself
                }
            }
        }
    }

    static double getTimeMultiplier() {
        time_t now = time(0);
        tm* localTime = localtime(&now);
        int currentHour = localTime->tm_hour;

        // Define time slots with corresponding multipliers
        if (currentHour >= 8 && currentHour <= 18) {
            return 1.5; // Peak hours: 8 AM - 6 PM
        } else if (currentHour >= 19 && currentHour <= 22) {
            return 1.2; // Evening hours: 7 PM - 10 PM
        } else {
            return 1.0; // Off-peak hours: Night
        }
    }

    static void dijkstra(int source) {
        fill(distances, distances + NUM_SLOTS, INT_MAX);
        fill(visited, visited + NUM_SLOTS, false);

        distances[source] = 0;

        for (int i = 0; i < NUM_SLOTS - 1; i++) {
            int u = findMinimumDistance();
            visited[u] = true;

            for (int v = 0; v < NUM_SLOTS; v++) {
                if (!visited[v] && graph[u][v] != 0 && distances[u] != INT_MAX && distances[u] + graph[u][v] < distances[v]) {
                    distances[v] = distances[u] + graph[u][v];
                }
            }
        }
    }

    static int findMinimumDistance() {
        double min = INT_MAX;
        int minIndex = -1;

        for (int i = 0; i < NUM_SLOTS; i++) {
            if (!visited[i] && distances[i] < min) {
                min = distances[i];
                minIndex = i;
            }
        }

        return minIndex;
    }

    static int findNearestAvailableSlot(int source) {
        dijkstra(source);

        vector<pair<int, double>> sortedSlots; // Vector to store slots and their distances

        // Collect all slots with their distances
        for (int i = 0; i < NUM_SLOTS; i++) {
            if (distances[i] != INT_MAX) {
                sortedSlots.push_back({i, distances[i]});
            }
        }

        // Sort slots by distance (ascending order)
        sort(sortedSlots.begin(), sortedSlots.end(), [](pair<int, double> a, pair<int, double> b) {
            return a.second < b.second;
        });

        // Find the nearest available slot
        for (auto slot : sortedSlots) {
            int slotId = slot.first;
            if (slots[slotId]->isAvailable) {
                return slotId; // Return the first available slot
            }
        }

        cout << "No available parking slots." << endl;
        return -1; // No slots available
    }

    static void parkVehicle(int slotId, double duration) {
        if (slotId >= 0 && slotId < NUM_SLOTS && slots[slotId]->isAvailable) {
            slots[slotId]->isAvailable = false;

            double baseCost = slots[slotId]->costPerHour;
            double totalCost = 0;

            // Pricing logic
            if (duration <= 1) {
                totalCost = baseCost; // Flat cost for 1 hour
            } else if (duration > 1 && duration <= 1.5) {
                totalCost = baseCost + (baseCost * 0.5); // Add 50% of base cost for 1.5 hours
            } else {
                totalCost = baseCost + (baseCost * 0.5); // Cost for 1.5 hours
                totalCost += (duration - 1.5) * baseCost; // Normal rate for additional hours
            }

            // Apply time multiplier ONLY for durations beyond the base hour
            double timeMultiplier = (duration > 1) ? getTimeMultiplier() : 1.0;
            totalCost *= timeMultiplier;

            cout << "Vehicle parked in slot " << (slotId + 1) << ". Total cost: ₹" << totalCost << endl;
            cout << "(Cost includes a time multiplier of " << timeMultiplier << " for extended hours.)" << endl;
        } else {
            cout << "Slot is unavailable or invalid." << endl;
        }
    }

    static void freeParkingSlot(int slotId) {
        if (slotId >= 0 && slotId < NUM_SLOTS && !slots[slotId]->isAvailable) {
            slots[slotId]->isAvailable = true;
            cout << "Slot " << (slotId + 1) << " is now available." << endl;
        } else {
            cout << "Slot is already free or invalid." << endl;
        }
    }

    static void displayParkingSlots() {
        cout << "Time-based multipliers apply depending on current hours:" << endl;
        cout << "Peak (8 AM - 6 PM): 1.5x | Evening (7 PM - 10 PM): 1.2x | Off-Peak: 1.0x" << endl;
        for (int i = 0; i < NUM_SLOTS; i++) {
            cout << "Slot " << (i + 1) << ": " << (slots[i]->isAvailable ? "Available" : "Occupied")
                 << ", Cost per hour: ₹" << slots[i]->costPerHour << endl;
        }
    }
};

ParkingSlot* SmartParkingSystem::slots[SmartParkingSystem::NUM_SLOTS];
double SmartParkingSystem::graph[SmartParkingSystem::NUM_SLOTS][SmartParkingSystem::NUM_SLOTS];
double SmartParkingSystem::distances[SmartParkingSystem::NUM_SLOTS];
bool SmartParkingSystem::visited[SmartParkingSystem::NUM_SLOTS];

int main() {
    SmartParkingSystem::initializeSystem(); // Initialize the system

    while (true) {
        int choice;
        cout << "\nSmart Parking System with Costing" << endl;
        cout << "1. View Parking Slot Availability" << endl;
        cout << "2. Park a Vehicle" << endl;
        cout << "3. Free a Parking Slot" << endl;
        cout << "4. Find Nearest Available Parking Slot" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            SmartParkingSystem::displayParkingSlots();
            break;
        case 2: {
            int parkSlot;
            double duration;
            cout << "Enter parking slot number to park (1-10): ";
            cin >> parkSlot;
            cout << "Enter parking duration (hours): ";
            cin >> duration;
            SmartParkingSystem::parkVehicle(parkSlot - 1, duration);
            break;
        }
        case 3: {
            int freeSlot;
            cout << "Enter parking slot number to free (1-10): ";
            cin >> freeSlot;
            SmartParkingSystem::freeParkingSlot(freeSlot - 1);
            break;
        }
        case 4: {
            int location;
            cout << "Enter your current location slot (1-10): ";
            cin >> location;

            int nearestSlot = SmartParkingSystem::findNearestAvailableSlot(location - 1);
            if (nearestSlot != -1) {
                cout << "Nearest parking slot for your location is Slot " << (nearestSlot + 1) << endl;
            }
            break;
        }
        case 0:
            cout << "Exiting system." << endl;
            return 0;
        default:
            cout << "Invalid choice! Please try again." << endl;
        }
    }

    return 0;
}