#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <ctime>

// ***** Simulation parameters *****

// Environment globals
const int YEARS = 500;
const int YEAR_DAYS = 365;
const int SIMULATION_LENGTH = YEARS * YEAR_DAYS;

const int POP_AMOUNT = 10;
int ADULT_AGE = 18 * YEAR_DAYS;
int SENIOR_AGE = 65 * YEAR_DAYS;

// Events (Delay in years, Length in days)
int LOCUST_DELAY = 0;
int LOCUST_LENGTH = 0;

int PLAGUE_DELAY = 0;
int PLAGUE_LENGTH = 0;

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

class YearlyStats {
    public:
    int age_deaths = 0;
    int starvation_deaths = 0;
    int plague_deaths = 0;
};

void GiveIncome(std::vector<Pop>& pops){

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
        harvest = (rand() % 41 + 90) / 100.0;
    } else if (day % YEAR_DAYS >= 92 && day % YEAR_DAYS <= 182){ // Summer
        harvest = (rand() % 51 + 100) / 100.0;
    } else if (day % YEAR_DAYS >= 183 && day % YEAR_DAYS <= 273){ // Autumn
        harvest = (rand() % 51 + 150) / 100.0;
    } else if ((day % YEAR_DAYS >= 274 && day % YEAR_DAYS  <= 364) || day % YEAR_DAYS == 0){ // Winter
        harvest = (rand() % 51 + 50) / 100.0;
    }

    for (Pop pop : pops) {// Food production determined by time of the year
        if (pop.age >= ADULT_AGE && pop.age <= SENIOR_AGE){
                double production = (rand() % 16 + 10) / 10.0; //  Production of my simple villagers
                
                if (LOCUST_LENGTH > 0){
                    production *= 0;
                }

                food.stored += production * harvest;
            }
    }
}

void Locust(std::vector<Pop>& pops, Food& food){

    // *** Swarm of locust (unlocks at 100 pops for stability) ***

    if (pops.size() >= 100 && LOCUST_DELAY <= 0){
        if ((rand() % 5000 < 1)){
            food.stored -= food.stored * (rand() % 31 + 60) / 100.0;

            LOCUST_LENGTH = 30;
            LOCUST_DELAY = 3 * YEAR_DAYS;
        }
    }
}

void Plague(std::vector<Pop>& pops, YearlyStats& stats){

    // *** Active plague kills people over time ***
    if (PLAGUE_LENGTH > 0){
        for (Pop& pop : pops){
            if (rand() % 5000 < 1){  // Small chance each day
                pop.alive = false;
                stats.plague_deaths++;
            }
        }
    }

    // *** Plague trigger (only happens when no active plague) ***
    if (pops.size() >= 500 && PLAGUE_DELAY <= 0){
        if ((rand() % 20000 < 1)){
            PLAGUE_LENGTH = 365 * 10;  // Plague lasting length
            PLAGUE_DELAY = 75 * YEAR_DAYS;  // Can't trigger again for a certain amount of time
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
            
            double max_affordable_price = pop.income * 1.3;

            if (pop.alive && pop.money >= food.price && food.stored >= 1 && food.price <= max_affordable_price){
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
                double max_affordable_price = pop.income * 1.3;
                if (food.price <= max_affordable_price){
                    pop.money -= food.price;
                    food.stored--;
                    pop.food = true;
                }
            }
        }
    }
}

double GetTargetStorage(std::vector<Pop>& pops) {
    return pops.size() * 120;
}

void StockTake(std::vector<Pop>& pops, Food& food, int day){

    // *** Stock take ***
    const double TARGET_PRICE = 5.0;
    double target_stored = GetTargetStorage(pops);

    // Food spoilage
    if (day % 7 == 0){
        double spoilage_rate = 0.001;
        food.stored *= (1.0 - spoilage_rate);
    }
    
    // Adjust price based on how far from target storage
    double storage_ratio = std::min(food.stored / target_stored, 3.0);  // Cap at 3.0
    food.price = TARGET_PRICE * pow(0.5 + storage_ratio, -1.5);
    
    if (food.price < 0.5) food.price = 0.5;
    if (food.price > 50) food.price = 50;  // Allow much higher prices
}

void PopulationAdjustment(std::vector<Pop>& pops,Food& food, int day,YearlyStats& stats){
    
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

        if (pop.age > SENIOR_AGE){
            int age_years = pop.age / YEAR_DAYS;
            if (rand() % (150 - age_years) < 1){
                pop.alive = false;
                stats.age_deaths++;
            }
        }

        if (pop.food == false){
            pop.hunger++;
        } else {
            pop.hunger = 0;
        }

        if (pop.hunger >= 7){
            pop.alive = false;
            stats.starvation_deaths++;
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
        if (pop.age >= ADULT_AGE && pop.age < SENIOR_AGE && LOCUST_LENGTH == 0 && food.stored > target_stored * 0.5){
            if (rand() % 10000 < 1){
                births++;
            }
        }

    }

    for (int i = 0; i < births; i++){
        pops.emplace_back(100, 0, 0);
    }

}

void Statistics(std::vector<Pop>& pops, Food& food, int day, std::ofstream& data,YearlyStats& stats){

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

    // Display and record yearly
    if (day % 365 == 0) {
        int year = day / YEAR_DAYS;
        std::cout << "Year " << year
                << " | Pops: " << pops.size()
                << " | Average Pop money: " << average_money
                << " | Food price: " << food.price
                << " | Food stored: " << food.stored
                << " | Food %: " << food_percentage
                << std::endl;

        data << year << ","
            << pops.size() << ","
            << average_money << ","
            << food.price << ","
            << food.stored << ","
            << food_percentage << ","
            << stats.age_deaths << ","
            << stats.starvation_deaths << ","
            << stats.plague_deaths << "\n";

        stats = YearlyStats();
    }
}

int main(){

    // Random seed
    srand (time(NULL));

    // Creation of instances
    std::vector<Pop> pops;
    Food food(10,100);
    YearlyStats stats;

    for (int i = 0; i < POP_AMOUNT; i++){
        pops.emplace_back(100, rand() % 11 + 5, (rand() % 30 + 18) * YEAR_DAYS); // Between 15 and 5 income, Between 18 and 48 in age too
    }

    // Create CSV file for graphing in python
    std::ofstream data("simulation.csv");

    // CSV headings
    data << "Year,Population,AverageMoney,Price,Stored,FoodPercentage,AgeDeath,Starvation,PlagueDeath\n";

    // Simulation
    for (int day = 1; day <= SIMULATION_LENGTH; day++){

        // Pops Income
        GiveIncome(pops);

        // Production of food
        ProductionFood(pops,food,day);

        // Locust
        Locust(pops,food);

        // Plague
        Plague(pops,stats);

        // Food consumption
        ConsumeFood(pops, food);

        // Stock take of food
        StockTake(pops,food,day);

        // Adjustment of pops (births/deaths)
        PopulationAdjustment(pops,food,day,stats);

        Statistics(pops,food, day, data,stats);

        if (LOCUST_DELAY > 0){
            LOCUST_DELAY--;
        }
        if (LOCUST_LENGTH > 0){
            LOCUST_LENGTH--;
        }
        if (PLAGUE_DELAY > 0){
            PLAGUE_DELAY--;
        }
        if(PLAGUE_LENGTH > 0){
            PLAGUE_LENGTH--;
        }

    } 
    data.close();
}