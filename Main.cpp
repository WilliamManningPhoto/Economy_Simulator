#include <iostream>
#include <vector>
#include <cstdlib>

// ***** Simulation parameters *****

// Environment globals
int weeks = 52;
int simulation_length = weeks * 7;

int pop_amount = 10;

// Population parameters
class Pop {
    public:
        double money;
        double income;

    Pop(double initial_money, double initial_income) {
        money = initial_money;
        income = initial_income;
    }
};

class Food {
    public:
        double price;
        double stored;
        double target_stored = pop_amount * 10;

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
        pops.emplace_back(100, rand() % 11 + 5); // Between 15 and 5 income
        std::cout << "Starting Money: " << pops[i].money << " Income: " << pops[i].income << std::endl;
    }

    // Creation of food
    Food food(10,100);

    // Simulation
    for (int day = 1; day <= simulation_length; day++){

        // Pops income
        for (Pop& pop : pops){
            pop.money += pop.income;
        }

        // *** Production ***
        
        food.stored += rand() % 7 + 7; // between 7 and 13 produced per day

        // *** Consumption ***

        // Pops buying food
        for (Pop& pop : pops){
            if (pop.money >= food.price) {
                pop.money -= food.price;
                food.stored--;
            };
        }

        // *** Stock take ***
        if (food.stored > food.target_stored && food.price > 1){
            food.price--;
        } else if (food.stored < food.target_stored && food.price < 20) {
            food.price++;
        }

        // Calculate pop average money
        double total_money = 0;

        for (Pop& pop : pops){
            total_money += pop.money;
        } 

        double average_money = total_money / pops.size();

        // Display average money every day
        if (day % 7 == 0) { // Change between 1 and 7 for daily and weekly
            std::cout << "Week " << day / 7
                      << " | Food price: " << food.price
                      << " | Food stored: " << food.stored
                      << " | Average Pop money: " << average_money
                      << std::endl;
        }
    } 
}