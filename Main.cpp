#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <cmath>

// ***** Simulation parameters *****

// Environment globals
int years = 250;
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
        int hunger;

    Pop(double initial_money, double initial_income, int initial_age){
        money = initial_money;
        income = initial_income;
        age = initial_age;
        lifespan = 80;
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

int main(){

    // Random seed
    srand (time(NULL));

    // Creation of Pops
    std::vector<Pop> pops;

    for (int i = 0; i < pop_amount; i++){
        pops.emplace_back(100, rand() % 11 + 5, (rand() % 60 + 18) * 365); // Between 16 and 5 income, Between 18 and 77 in age too
    }
    
    int age_death = 0;
    int starvation_death = 0;

    // Creation of food
    Food food(10,100);

    // Create CSV file for graphing in python
    std::ofstream data("simulation.csv");

    // CSV headings
    data << "Week,Population,AverageMoney,Price,Food,AgeDeath,Starvation\n";

    // Simulation
    for (int day = 1; day <= simulation_length; day++){

        // *** Pops income and reset ***
        for (Pop& pop : pops){
            if (pop.alive){
                pop.money += pop.income;
            }
        }

        // *** Production ***
        
        for (Pop& pop : pops) {
            if (pop.age >= 18 * 365 && pop.age < 65 *365) {
                food.stored += (rand() % 25 + 8) / 10.0; // My people are simple village folk
            }
        }

        // *** Consumption ***

        // Pops buying food
        for (Pop& pop : pops){
            pop.food = false;

            // Adults get food first
            if (pop.age >= 18 * 365 && pop.age < 65 * 365){
                if (pop.alive && pop.money >= food.price && food.stored >= 1){
                    pop.money -= food.price;
                    food.stored--;
                    pop.food = true;
                }
            }
        }

        // Children and elders get whatever is left over
        for (Pop& pop : pops){
            
            if (pop.food == false){
                if (pop.age < 18 * 365 && food.stored >= 1){
                    food.stored--;
                    pop.food = true;
                    }

                else if (pop.age >= 65 * 365 && pop.alive && pop.money >= food.price && food.stored >= 1){
                    pop.money -= food.price;
                    food.stored--;
                    pop.food = true;
                }
            }
        }

        // *** Stock take ***
        double target_stored = pops.size() * 10;
        double food_difference = target_stored - food.stored;
        double price_change = abs(food_difference) * 0.001;

        if (food.stored > target_stored && food.price > 1){
            food.price -= price_change;
        }
         else if (food.stored < target_stored && food.price < 15){
            food.price += price_change;
        }

        if (food.price < 1){
            food.price = 1;
        }

        if (food.price > 15){
            food.price = 15;
        }

        // *** Population adjustments ***

        for (Pop& pop : pops){
            if (pop.age == 18 * 365){
                pop.income = rand() % 11 + 5;
            }
        }

        // Age and starvation pops

        for (Pop& pop : pops){
            pop.age++;
            if (pop.age >= pop.lifespan * 365){
                pop.alive = false;
                age_death++;
                //std::cout << "Pop died of old age" << std::endl;
            }

            if (pop.food == false){
                pop.hunger++;
            } else {
                pop.hunger = 0;
            }

            if (pop.hunger >= 7){
                pop.alive = false;
                starvation_death++;
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

        if (day % 365 == 0){

            // Births
            int births = 0;

            for (Pop& pop : pops){
                if (pop.age >= 18 * 365 && pop.age <= 65 * 365){
                    if (rand() % 100 < 10){
                        births++;
                    }
                }

            }

            for (int i = 0; i < births; i++){
                pops.emplace_back(100, 0, 0);
            }


        }

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

        // Display average money
        if (day % 7 == 0) {
            std::cout << "Week " << day / 7
                    << " | Pops: " << pops.size()
                    << " | Average Pop money: " << average_money
                    << " | Food price: " << food.price
                    << " | Food stored: " << food.stored
                    << std::endl;

            data << day / 7 << ","
                << pops.size() << ","
                << average_money << ","
                << food.price << ","
                << food.stored << ","
                << age_death << ","
                << starvation_death << "\n";

            age_death = 0;
            starvation_death = 0;
        }
    } 
    data.close();
}