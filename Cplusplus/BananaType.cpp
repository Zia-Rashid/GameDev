#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <list>
#include <conio.h>
#include <windows.h>

using namespace std;

enum ModeType { Seconds_10, Seconds_30, Seconds_60, Words_10, Words_25, Words_50 };

class Mode
{
private:
    ModeType mode;
public:
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

    ModeType getMode() const { // const before denotes that it is not able to modify any data within the body, const after promises not to modify and member variables of the class
        return mode;
    }

    bool isTimedMode() const {
        return mode == Seconds_10 || mode == Seconds_30 || mode == Seconds_60;
    }
};

class WordManager
{
private:
    vector<string> word_book;
    string displayed_words;
public:
    void loadWords(const string &filename) {
        ifstream file(filename);
        string word;
        while(file >> word) { // works like cin
            word_book.push_back(word);
        }
    }

    string generateWords(int count) {
        displayed_words = "";
        for (int i = 0; i < count; i ++) {
            int randomIndex = rand() % word_book.size();
            displayed_words += word_book[randomIndex] + " ";
        }
        return displayed_words;
    }

    const string &getDisplayedWords() const {
        return displayed_words;
    }

    bool checkWord(const string &bananaWord, int position) {
        return bananaWord[position] == displayed_words[position];
    }
};

class Player
{
private:
    int words_typed;
    int misclicks;
public:
    Player() :words_typed(0), misclicks(0) {}; // same as line below
    // Player() {
    //     this->words_typed = 0;
    //     this->misclicks = 0;
    // }

    void incrementWordsTyped() { words_typed++; }
    void incrementMisclicks() { misclicks++; }
    int getWordsTyped() const { return words_typed; } 
    int getMisclicks() const { return misclicks; }
};

class Display
{
public:
    void showGameOver(int time,const Player player, const string &typedWords, const string &mode) {  // have all addresses explained
        int totalChars{ static_cast<int>(typedWords.length()) };
        int wordCount{ totalChars / 5 }; // Avg # of Letters/word
        cout << "\n\tGAME OVER!" << endl;
        cout << "mode: " << mode << endl;
        cout << "wpm: " << (wordCount / (time / 60)) << endl;
        cout << "acc: " << (((totalChars - player.getMisclicks()) * 100.0) / totalChars) << "%" << endl;
    }
    
    void showDisplayedWords(const string &words) {
        cout << '\n' << words << '\n' << endl; // in the future look into increasing font size
    }

    void showTypingProgress(const string &typedWords) { // returns current typedWords
        cout << "\r" << typedWords << flush;
    }
};

class InputHandler
{
public:
    char getKeyPress() {
        return _getch();
    }

    bool isKeyHit() {
        return _kbhit();
    }
};

// Main game class
class Game 
{
private:
    Mode mode;
    WordManager wordManager;
    Player player;
    Display display;
    InputHandler inputHandler;
    bool gameOver;
public:
    Game() :gameOver(false) {}

    void setup() {
        wordManager.loadWords("common_words.txt");
        mode.pickMode();
    }

    void run() {
        if (mode.isTimedMode()) {
            playTimedGame();
        } else {
            playCountGame();
        } 
    }

    void playTimedGame() {
        int timeLimit = (mode.getMode() == Seconds_10) ? 10 : (mode.getMode() == Seconds_30) ? 30 : 60;
        string words = wordManager.generateWords(150);
        display.showDisplayedWords(words);

        string typedWords = "";
        auto startTime = chrono::steady_clock::now();
        while (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count() < timeLimit) {
            if (inputHandler.isKeyHit()) {
                char key = inputHandler.getKeyPress();
                if (key == '\b' && !typedWords.empty()) {  // Handle backspace
                    typedWords.pop_back();
                } else if (key != '\b') {
                    typedWords += key;
                    if (typedWords.back() != words[typedWords.size() - 1]) {
                        player.incrementMisclicks();
                    } else if (typedWords.back() == ' ') {
                        player.incrementWordsTyped();
                    }
                }
                display.showTypingProgress(typedWords);
            }
        }
        gameOver = true;
        display.showGameOver(timeLimit, player, typedWords, "Timed");
    }

    void playCountGame() {
        int wordCount = (mode.getMode() == Words_10) ? 10 : (mode.getMode() == Words_25) ? 25 : 50;
        string words = wordManager.generateWords(wordCount);
        display.showDisplayedWords(words);

        string typedWords = "";
        auto startTime = chrono::steady_clock::now();
        while (player.getWordsTyped() < wordCount) {
            if (inputHandler.isKeyHit()) {
                char key = inputHandler.getKeyPress();
                if (key == '\b' && !typedWords.empty()) {  // Handle backspace
                    typedWords.pop_back();
                } else if (key != '\b') {
                    typedWords += key;
                    if (typedWords.back() != words[typedWords.size() - 1]) {
                        player.incrementMisclicks();
                    } else if (typedWords.back() == ' ') {
                        player.incrementWordsTyped();
                    }
                }
                display.showTypingProgress(typedWords);
            }
        }
        int elapsedTime = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count();
        gameOver = true;
        display.showGameOver(elapsedTime, player, typedWords, "Count");
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    Game game;
    game.setup();
    game.run();
    
    return 0;
}