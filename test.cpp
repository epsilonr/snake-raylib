#include <iostream>
#include <string>

typedef struct Player {
    std::string name;
    int x;
    int y;
    int score = 0;

    void Greet(std::string &name);
} Player;

void Player::Greet(std::string &name){
    std::cout << "Hello " << name << "!" << std::endl;
}

int main() {
    Player player {"Rafet", 0, 0};

    std::cout << player.score << std::endl;
    return 0;
}