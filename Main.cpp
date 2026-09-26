#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <ctime>

// ***** Simulation parameters *****

// Environment globals
const int YEARS = 300;
const int YEAR_DAYS = 365;
const int SIMULATION_LENGTH = YEARS * YEAR_DAYS;

const int POP_AMOUNT = 10;
int LIFESPAN = 80;
int ADULT_AGE = 18 * YEAR_DAYS;
int SENIOR_AGE = 65 * YEAR_DAYS;

int LOCUST_DAYS = 0;
int LOCUST_DELAY = 0;

// Stats
int AGE_DEATH = 0;
int STARVATION_DEATH = 0;

// Data stores
class Pop {
    public:
        double money;
        double income;
        int age;
        bool alive;
        bool food;
        int hunger;

    Pop(double initial_money, double initial_income, int initial_age){
        money = initial_money;
        income = initial_income;
        age = initial_age;
        alive = true;
        food = true;
        hunger = 0;
    }
};

class Food {
    public:
        double price;
        double stored;

    Food(double initial_price, double initial_stored){
        price = initial_price;
        stored = initial_stored;
    }
};

void GiveIncome(std::vector<Pop>& pops) {

    // *** Pops income ***
    for (Pop& pop : pops){
        if (pop.alive){
            pop.money += pop.income;
        }
    }
}

void ProductionFood(std::vector<Pop>& pops,Food& food, int day){

    // *** Production ***

    // Harvest multipliers by season
    double harvest;

    if (day % YEAR_DAYS >= 1 && day % YEAR_DAYS <= 91){ // Spring
        harvest = (rand() % 41 + 80) / 100.0;
    } else if (day % YEAR_DAYS >= 92 && day % YEAR_DAYS <= 182){ // Summer
        harvest = (rand() % 51 + 100) / 100.0;
    } else if (day % YEAR_DAYS >= 183 && day % YEAR_DAYS <= 273){ // Autumn
        harvest = (rand() % 51 + 150) / 100.0;
    } else if ((day % YEAR_DAYS >= 274 && day % YEAR_DAYS  <= 364) || day % YEAR_DAYS == 0){ // Winter
        harvest = (rand() % 51 + 50) / 100.0;
    }

    for (Pop pop : pops) {// Food production determined by time of the year
        if (pop.age >= ADULT_AGE && pop.age <= SENIOR_AGE) {
                double production = (rand() % 22 + 6) / 10.0; //  Production of my simple villagers
                
                if (LOCUST_DAYS > 0){
                    production *= 0.4;
                }

                food.stored += production * harvest;
            }
    }
}

void Locust(std::vector<Pop>& pops, Food& food){

    // *** Swarm of locust (unlocks at 100 pops for stability) ***

    if (pops.size() >= 100 && LOCUST_DELAY <= 0){
        if (rand() % 20000 < 1){
            food.stored -= food.stored * (rand() % 51) / 100.0;

            LOCUST_DAYS = 50;
            LOCUST_DELAY = 2 * YEAR_DAYS;
        }

    }
    
}

void ConsumeFood(std::vector<Pop>& pops, Food& food){

    // *** Consumption ***

    // Pops buying food
    for (Pop& pop : pops){
        pop.food = false;

        // Adults get food first
        if (pop.age >= ADULT_AGE && pop.age < SENIOR_AGE){
            if (pop.alive && pop.money >= food.price && food.stored >= 1){
                pop.money -= food.price;
                food.stored--;
                pop.food = true;
            }
        }
    }

        // Children next fed
    for (Pop& pop : pops){
        
        if (pop.food == false){
            if (pop.age < ADULT_AGE && food.stored >= 1){
                food.stored--;
                pop.food = true;
            }
        }
    }
    
    // Elders fed last
    for (Pop& pop : pops){

        if (pop.food == false){
            if (pop.age >= SENIOR_AGE && pop.alive && pop.money >= food.price && food.stored >= 1){
                pop.money -= food.price;
                food.stored--;
                pop.food = true;
            }
        }
    }
}

double GetTargetStorage(std::vector<Pop>& pops) {
    return pops.size() * 120;
}

void StockTake(std::vector<Pop>& pops, Food& food){

    // *** Stock take ***
    const double TARGET_PRICE = 5.0;
    double target_stored = GetTargetStorage(pops);
    
    // Adjust price based on how far from target storage
    double storage_ratio = food.stored / target_stored;  // 0.5 = half full, 2.0 = double full
    food.price = TARGET_PRICE * (2.0 - storage_ratio);  // Inverse relationship
    
    // Clamp
    if (food.price < 1) food.price = 1;
    if (food.price > 20) food.price = 10;
}

void PopulationAdjustment(std::vector<Pop>& pops,Food& food, int day){
    
    double target_stored = GetTargetStorage(pops);

    // *** Population adjustments ***

    for (Pop& pop : pops){
        if (pop.age == ADULT_AGE){
            pop.income = rand() % 11 + 5;
        }
    }

    // Age and starvation pops

    for (Pop& pop : pops){
        pop.age++;
        if (pop.age >= LIFESPAN * YEAR_DAYS){
            pop.alive = false;
            AGE_DEATH++;
            //std::cout << "Pop died of old age" << std::endl;
        }

        if (pop.food == false){
            pop.hunger++;
        } else {
            pop.hunger = 0;
        }

        if (pop.hunger >= 7){
            pop.alive = false;
            STARVATION_DEATH++;
            //std::cout << "Pop starved" << std::endl;
        }
    }
    
    // Remove dead pops
    for (auto it = pops.begin(); it != pops.end();){
        if (it->alive == false){
            it = pops.erase(it);
        } else {
            ++it;
        }
    }

    // Births
    int births = 0;

    for (Pop& pop : pops){
        if (pop.age >= ADULT_AGE && pop.age < SENIOR_AGE && LOCUST_DAYS == 0 && food.stored > target_stored * 0.5){
            if (rand() % 7000 < 1){
                births++;
            }
        }

    }

    for (int i = 0; i < births; i++){
        pops.emplace_back(100, 0, 0);
    }

}

void Statistics(std::vector<Pop>& pops, Food& food, int day, std::ofstream& data){

    // *** Math and data ***
    double total_money = 0;
    double average_money = 0;

    for (Pop& pop : pops){
        if (pop.alive){
            total_money += pop.money;
        }
    } 

    if (!pops.empty()){
        average_money = total_money / pops.size();
    }


    // Calculate max food storage capacity
    double max_food_storage = GetTargetStorage(pops);  // Your target
    double food_percentage = (food.stored / max_food_storage) * 100.0;

    // Display average money
    if (day % 7 == 0) {
        std::cout << "Week " << day / 7
                << " | Pops: " << pops.size()
                << " | Average Pop money: " << average_money
                << " | Food price: " << food.price
                << " | Food stored: " << food.stored
                << " | Food %: " << food_percentage
                << std::endl;

        data << day / 7 << ","
            << pops.size() << ","
            << average_money << ","
            << food.price << ","
            << food.stored << ","
            << food_percentage << ","
            << AGE_DEATH << ","
            << STARVATION_DEATH << "\n";

        AGE_DEATH = 0;
        STARVATION_DEATH = 0;
    }
}

int main(){

    // Random seed
    srand (time(NULL));

    // Creation of Pops
    std::vector<Pop> pops;

    for (int i = 0; i < POP_AMOUNT; i++){
        pops.emplace_back(100, rand() % 11 + 5, (rand() % 60 + 18) * YEAR_DAYS); // Between 15 and 5 income, Between 18 and 77 in age too
    }

    // Creation of food
    Food food(10,100);

    // Create CSV file for graphing in python
    std::ofstream data("simulation.csv");

    // CSV headings
    data << "Week,Population,AverageMoney,Price,Stored,FoodPercentage,AgeDeath,Starvation\n";

    // Simulation
    for (int day = 1; day <= SIMULATION_LENGTH; day++){

        // Pops Income
        GiveIncome(pops);

        // Production of food
        ProductionFood(pops,food,day);

        // Locust
        Locust(pops,food);

        // Food consumption
        ConsumeFood(pops, food);

        // Stock take of food
        StockTake(pops,food);

        // Adjustment of pops (births/deaths)
        PopulationAdjustment(pops,food,day);

        Statistics(pops,food, day, data);

        if (LOCUST_DAYS > 0){
            LOCUST_DAYS--;
        }
        if (LOCUST_DELAY > 0){
            LOCUST_DELAY--;
        }

    } 
    data.close();
}