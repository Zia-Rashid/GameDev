#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <list>
#include <conio.h>
#include <windows.h>

using namespace std;
int words_typed;
vector<string> word_book;
string displayed_words;
bool game_over{false};

enum Mode { Seconds_10, Seconds_30, Seconds_60, Words_10, Words_25, Words_50 };
Mode mode;

void pickMode() {
    bool modePicked = false;
    
    while (!(modePicked)) {
        cout << "Pick your mode: " << "10s, 30s, 60s, 10-words, 25-words, 50-words" << endl;
        string choice;
        cin >> choice; 
        if (choice.compare("10s") == 0) {
            mode = Seconds_10;
            modePicked = true;
        } else if (choice.compare("30s") == 0) {
            mode = Seconds_30;
            modePicked = true;
        } else if (choice.compare("60s") == 0) {
            mode = Seconds_60;
            modePicked = true;
        } else if (choice.compare("10-words") == 0) {
            mode = Words_10;
            modePicked = true;
        } else if (choice.compare("25-words") == 0) {
            mode = Words_25;
            modePicked = true;
        } else if (choice.compare("50-words") == 0) {
            mode = Words_50;
            modePicked = true;
        } else {
            cout << "Invalid choice..." << endl;
            continue;
        }
    }
}

void Setup()
{
    words_typed = 0;                            // initialize word count

    string word;                                
    ifstream common_words("common_words.txt");
    while (getline(common_words, word)) {       // initialize word_book
        word_book.push_back(word);              
    }
    common_words.close();                       

    pickMode();                                 // initialize mode

} 

void gameOver(int time, string words, int misclicks, string mode) {
    int letterCount{ static_cast<int>(words.length()) };
    int wordCount{ words_typed };
    // may or may not need to have different game overs depending on the mode
    cout << "\n\tGAME OVER!\nwpm: " << (wordCount / (static_cast<int>(time) / 60)) // static cast is more explicit and type safe
    << "\nacc: " << ((100 / letterCount) * (letterCount - misclicks)) << "%" << endl;
    game_over = true;
}

void gameplay(string bananasTyping,int &userSize, ) {
        cout << bananasTyping << flush;             // shows user there input
        if (_kbhit()) {                             // if the key board is hit, evaluate it 
            bananasTyping.push_back(_getch()); 
            if (bananasTyping.size() > userSize) {  // only update data if there is forward progress, not fixing mistakes
                userSize = bananasTyping.size();
                char correct_click = displayed_words.at(userSize - 1);
                char banana_click = bananasTyping.at(userSize - 1);
                if (correct_click != banana_click)  // evaluate whether or not they ahve made a misclick
                    misclicks++;
                if (banana_click == ' ')            // if they have written a new word, add to the score
                    words_typed++;
            }
            if (bananasTyping.size() == displayed_words.size() || _getch() == '`')
                gameOver(originalTime, displayed_words, misclicks, "Time");              // end game early 
}

void TimedGame(double time) {
    for (int i = 0; i < 150; i++) {
        int ranVal = rand() % 5001;
        displayed_words += word_book[ranVal] + " "; // add the random words to the back of a string. This will be constantly displayed in console and comparison will be checked. If they are not 0, add 1 to incorrect count
    }
    
    cout << '\n' << displayed_words << '\n' <<  endl; 

    string bananasTyping{ "" };
    int userSize{ 0 };
    int misclicks{ 0 };
    int originalTime = time;  // time goes down

    while (time > 0)
    {
        
        }
        Sleep(1000);                                // wait 1 second before counting down again
        time--;
    }
    gameOver(originalTime, displayed_words, misclicks, "Time");
}

void CountGame(int count) {
    for (int i = 0; i < count; i++) {
        int ranVal = rand() % 5001;
        displayed_words += word_book[ranVal] + " "; 
    }
    cout << '\n' << displayed_words << '\n' <<  endl; 

    string bananasTyping{ "" };
    int userSize{ 0 };
    int misclicks{ 0 };
    int time_passed{ 0 };   // time goes up

    while (count > words_typed)
    {
        
        cout << bananasTyping << flush;    // shows user there input
        while (_kbhit()) {                             
            bananasTyping.push_back(_getch()); 
            if (bananasTyping.size() > userSize) {  
                userSize = bananasTyping.size();
                char correct_click = displayed_words.at(userSize - 1);
                char banana_click = bananasTyping.at(userSize - 1);
                if (correct_click != banana_click)  
                    misclicks++;
                if (banana_click == ' ')            
                    words_typed++;
            }
            if (bananasTyping.size() == displayed_words.size() || _getch() == '`') //  back tick to exit
                gameOver(time_passed, displayed_words, misclicks, "Count");              
        }
        Sleep(1000);                                
        time_passed++;
    }
    gameOver(time_passed, displayed_words, misclicks, "Count");
}

void Logic()
{
    switch (mode) 
    {
        case Seconds_10:
            TimedGame(10.0);
            break;
        case Seconds_30:
            TimedGame(30.0);
            break;
        case Seconds_60:
            TimedGame(60.0);
            break;
        case Words_10:
            CountGame(10);
            break;
        case Words_25:
            CountGame(25);
            break;
        case Words_50:
            CountGame(50);
            break;
    }
}


int main() 
{
    srand(std::time(NULL));
    Setup();
    while (!(game_over)) {
        Logic();
    }
    return 0;
}

