#include <iostream>
#include <vector>
#include <cstdlib>

// ***** Simulation parameters *****

// Environment globals
int years = 20;
int simulation_length = years * 365;

int pop_amount = 10;

// Population parameters
class Pop {
    public:
        double money;
        double income;
        int age;
        int lifespan;
        bool alive;
        bool food;

    Pop(double initial_money, double initial_income, int initial_age) {
        money = initial_money;
        income = initial_income;
        age = initial_age;
        lifespan = 80;
        alive = true;
        food = true;
    }
};

class Food {
    public:
        double price;
        double stored;

    Food(double initial_price, double initial_stored) {
        price = initial_price;
        stored = initial_stored;
    }
};

int main(){

    // Random seed
    srand (time(NULL));

    // Creation of Pops
    std::vector<Pop> pops;

    for (int i = 0; i < pop_amount; i++){
        pops.emplace_back(100, rand() % 11 + 5, rand() % 60 + 18); // Between 15 and 5 incomem, Between 18 and 77 in age too
        std::cout << "Starting Money: " << pops[i].money << " Income: " << pops[i].income << std::endl;
    }

    // Creation of food
    Food food(10,100);

    // Simulation
    for (int day = 1; day <= simulation_length; day++){

        // Pops income
        for (Pop& pop : pops){
            if (pop.alive){
                pop.money += pop.income;
            }
        }

        // *** Production ***
        
        for (Pop& pop : pops) {
            food.stored += 0.7 + (rand() / (double)RAND_MAX) * 0.7; // My people are simple village folk
        }

        // *** Consumption ***

        // Pops buying food
        for (Pop& pop : pops){
            pop.food = false;

            if (pop.alive && pop.money >= food.price && food.stored > 0) {
                pop.money -= food.price;
                food.stored--;
                pop.food = true;
            };
        }

        // *** Stock take ***
        double target_stored = pops.size() * 10;

        if (food.stored > target_stored && food.price > 1){
            food.price--;
        } else if (food.stored < target_stored && food.price < 20) {
            food.price++;
        }

        // *** Population adjustments ***

        // Age and starvation pops
        for (Pop& pop : pops){
            pop.age++;
            if (pop.age >= pop.lifespan * 365){
                pop.alive = false;
            }
            if (pop.food == false){
                pop.alive = false;
            }
        }
        
        // Remove dead pops
        for (auto it = pops.begin(); it != pops.end(); ) {
            if (it->alive == false) {
                it = pops.erase(it);
            } else {
                ++it;
            }
        }

        if (day % 365 == 0){

            // Births
            int births = 0;

            for (Pop& pop : pops){
                if (rand() % 100 < 10){
                    births++;
                }
            }

            for (int i = 0; i < births; i++){
                pops.emplace_back(100, rand() % 11 + 5, 0);
            }

        }


        // *** Math and data ***
        double total_money = 0;

        for (Pop& pop : pops){
            if (pop.alive){
                total_money += pop.money;
            }
        } 

        double average_money = total_money / pops.size();

        // Display average money
        if (day % 365 == 0) { // Change between 1 and 7 for daily and weekly
            std::cout << "Year " << day / 365
                      << " | Food price: " << food.price
                      << " | Food stored: " << food.stored
                      << " | Pops: " << pops.size()
                      << " | Average Pop money: " << average_money
                      << std::endl;
        }
    } 
}