#include <iostream>
#include <windows.h>
#include <string>
#include <random>
#include <cmath>

using namespace std;

class VillageSimulator {
private:
    int day = 1;
    
    int adults = 5;
    int children = 0;
    
    // Професії
    int farmers = 2;
    int woodcutters = 1;
    int miners = 0;
    
    // Ресурси
    double food = 20.0;
    int wood = 0;
    int stone = 0;
    
    int huts = 1; 

    bool gameOver = false;
    string lastEvent = "Ви заснували нове поселення і розподілили перші професії.";

    int getRandomInt(int min, int max) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distrib(min, max);
        return distrib(gen);
    }

    void killAdult() {
        adults--;
        if (farmers + woodcutters + miners > adults) {
            if (miners > 0) miners--;
            else if (woodcutters > 0) woodcutters--;
            else if (farmers > 0) farmers--;
        }
    }

    void handleNewWorker(string message) {
        adults++; 
        
        cout << "\033[2J\033[1;1H"; 
        cout << "========================================\n";
        cout << " ПОДІЯ: " << message << "\n";
        cout << "========================================\n";
        cout << " У вас з'явилися нові робочі руки!\n";
        cout << " Куди ви хочете призначити цю людину?\n";
        cout << " 1. Фермери     (+" << 3 << " їжі/день)\n";
        cout << " 2. Лісоруби    (+" << 2 << " дерева/день)\n";
        cout << " 3. Каменярі    (+" << 1 << " каменю/день)\n";
        cout << " 4. Будівельники (Зводять нові хатини)\n";
        cout << " Ваш вибір: ";
        
        int choice;
        cin >> choice;
        
        switch(choice) {
            case 1: farmers++; break;
            case 2: woodcutters++; break;
            case 3: miners++; break;
            case 4: break; 
            default: cout << "Невідомий вибір. Людина стала Будівельником.\n"; break;
        }
    }

    void processDay() {
        // Видобуток ресурсів
        food += farmers * 3.0;
        wood += woodcutters * 2;
        stone += miners * 1; 

        // Споживання їжі
        double consumption = (adults * 1.0) + (children * 0.5);
        food -= consumption;
        
        lastEvent = "День пройшов спокійно. Ресурси зібрано."; 
        
        // ГОЛОД
        if (food < 0) {
            int missingFood = ceil(abs(food)); 
            food = 0;
            
            int deaths = getRandomInt(1, missingFood);
            int adultsDied = 0;
            int childrenDied = 0;

            for (int i = 0; i < deaths; i++) {
                if (children > 0 && adults > 0) {
                    if (getRandomInt(1, 100) > 50) { 
                        children--; childrenDied++;
                    } else {
                        killAdult(); adultsDied++;
                    }
                } else if (children > 0) {
                    children--; childrenDied++;
                } else if (adults > 0) {
                    killAdult(); adultsDied++;
                } else {
                    break; 
                }
            }
            
            lastEvent = "УВАГА: МАСОВИЙ ГОЛОД! Через нестачу їжі померло: " + to_string(adultsDied) + " дорослих та " + to_string(childrenDied) + " дітей.";

            if (adults <= 0 && children <= 0) {
                gameOver = true;
                lastEvent = "Усі поселенці загинули від голоду. Поселення перетворилося на руїни...";
                return;
            }
        } 
        else {
            int totalPopulation = adults + children;
            int capacity = huts * 5;
            
            if (totalPopulation < capacity) {
                int eventChance = getRandomInt(1, 100);
                
                if (eventChance <= 15) {
                    handleNewWorker("До вашого поселення прийшов мандрівник!");
                    lastEvent = "Прийшов мандрівник. Ви призначили йому професію.";
                } 
                else if (eventChance > 15 && eventChance <= 35 && adults >= 2) {
                    children++;
                    lastEvent = "Чудові новини! У поселенні народилася дитина.";
                }
            }
        }

        // 4. Дорослішання дітей 
        if (children > 0) {
            if (getRandomInt(1, 100) <= 15) { 
                children--;
                handleNewWorker("Одна з дітей у поселенні подорослішала!");
                lastEvent += " Дитина виросла і отримала професію!"; 
            }
        }

        day++;
    }

    void assignProfession(int& jobVar, string jobName) {
        int builders = adults - (farmers + woodcutters + miners);
        int maxAvailable = jobVar + builders;
        
        cout << "\nЗараз Будівельників: " << builders << " | Зараз " << jobName << ": " << jobVar << "\n";
        cout << "Введіть нову кількість (від 0 до " << maxAvailable << "): ";
        int count;
        cin >> count;
        
        if (count >= 0 && count <= maxAvailable) {
            jobVar = count;
            lastEvent = "Ви перерозподілили професію: " + jobName + ".";
        } else {
            lastEvent = "Помилка! У вас немає стільки людей.";
        }
    }

public:
    void drawUI() {
        cout << "\033[2J\033[1;1H"; 
        cout << "========================================\n";
        cout << " ДЕНЬ " << day << " | ПОСЕЛЕННЯ\n";
        cout << "========================================\n";
        int totalPopulation = adults + children;
        int builders = adults - (farmers + woodcutters + miners);
        
        cout << " Населення: " << totalPopulation << " / " << (huts * 5) << " (Дітей: " << children << ")\n";
        cout << " Дорослих:  " << adults << "\n";
        cout << "----------------------------------------\n";
        cout << " ПРОФЕСІЇ:\n";
        cout << " [Ф] Фермери:      " << farmers << "  (Приносять +" << farmers * 3 << " їжі/день)\n";
        cout << " [Л] Лісоруби:     " << woodcutters << "  (Приносять +" << woodcutters * 2 << " дерева/день)\n";
        cout << " [К] Каменярі:     " << miners << "  (Приносять +" << miners * 1 << " каменю/день)\n";
        cout << " [Б] Будівельники: " << builders << "  (Доступні для зведення хатин)\n";
        cout << "----------------------------------------\n";
        cout << " СКЛАД:\n";
        cout << " Їжа:    " << floor(food) << "\n"; 
        cout << " Дерево: " << wood << "\n";
        cout << " Камінь: " << stone << "\n";
        cout << " Хатини: " << huts << "\n";
        cout << "========================================\n";
        cout << " ОСТАННЯ ПОДІЯ:\n " << lastEvent << "\n";
        cout << "========================================\n";
        cout << " НАКАЗ:\n";
        cout << " 1. Перерозподілити Фермерів\n";
        cout << " 2. Перерозподілити Лісорубів\n";
        cout << " 3. Перерозподілити Каменярів\n";
        cout << " 4. Побудувати хатину (Вартість: 20 дерева, 5 каменю)\n";
        cout << " 5. Зібрати ресурси\n";
        cout << " 0. Здатися і покинути поселення\n";
        cout << " Ваш вибір: ";
    }

    void play() {
        int choice;
        while (!gameOver) {
            drawUI();
            cin >> choice;

            int builders = adults - (farmers + woodcutters + miners);

            switch (choice) {
                case 1: assignProfession(farmers, "Фермери"); break;
                case 2: assignProfession(woodcutters, "Лісоруби"); break;
                case 3: assignProfession(miners, "Каменярі"); break;
                case 4:
                    if (wood >= 20 && stone >= 5 && builders >= 1) {
                        wood -= 20; stone -= 5; huts++;
                        lastEvent = "Побудована нова хатина! Використано матеріали та працю будівельників.";
                    } else if (builders < 1) {
                        lastEvent = "Немає кому будувати! Призначте хоча б одного Будівельника (переведіть з іншої професії).";
                    } else {
                        lastEvent = "Недостатньо матеріалів! (Треба: 20 дерева, 5 каменю)";
                    }
                    break;
                case 5: processDay(); break;
                case 0:
                    gameOver = true;
                    lastEvent = "Ви залишили своє поселення на призволяще...";
                    break;
                default: lastEvent = "Невідомий наказ, мілорде."; break;
            }
        }
        
        drawUI();
        cout << "\nГРА ЗАКІНЧЕНА. Ви протрималися " << day << " днів.\n";
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);
    VillageSimulator game;
    game.play();
    return 0;
}