#include <iostream>
#include <chrono>
#include <algorithm>
#include <vector>
#include <fstream>
#include <list>
#include <conio.h>
#include <windows.h>
#include <thread>

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
        double wpm = (wordCount / static_cast<double>(time)) * 60;
        double acc = (totalChars > 0) 
        ? (((totalChars - player.getMisclicks()) * 100.0) / totalChars) 
        : 100.0; // If no chars were typed, assume perfect accuracy

        cout << "\n\n\tGAME OVER!\n" << endl;
        cout << "mode: " << mode << endl;
        cout << "wpm: " << wpm << endl;
        cout << "acc: " << acc << "%" << endl;
    }
    
    void showDisplayedWords(const string &words, const string &typedWords) {
        const int lineWidth{ 125 };
        string border(lineWidth, '-');
        cout << border << endl;

        int charCount = 0;
        for (char c : words) {
            if (charCount >= lineWidth && c == ' ') {
                cout << '\n';
                charCount = 0;
            }
            cout << c;
            charCount++;
        }
        cout << '\n' << border << "\n\n" << endl;
    }

    void showTypingProgress(const string &typedWords, const string &displayedWords) {
        const int lineWidth = 125;                 // Maximum line width
        static vector<string> frozenLines;       // Store frozen lines
        static size_t frozenLineCount = 0;       // Track how many lines are frozen

        size_t numCompleteLines = typedWords.length() / lineWidth; // Completed lines
        size_t currentLineStart = numCompleteLines * lineWidth;    // Start of the current line
        string currentLine = typedWords.substr(currentLineStart);  // Extract the current line

        // Handle new frozen lines
        while (frozenLines.size() < numCompleteLines) {
            frozenLines.push_back(typedWords.substr(frozenLines.size() * lineWidth, lineWidth));
        }

        // Move cursor up to the position where frozen lines are
        cout << "\033[" << (frozenLineCount + 1) << "F";

        // Redraw frozen lines (if any) only once
        for (size_t i = frozenLineCount; i < frozenLines.size(); ++i) {
            cout << "\033[K" << frozenLines[i] << '\n';
        }
        frozenLineCount = frozenLines.size(); // Update count of frozen lines

        // Clear the current line
        cout << "\033[K";

        // Print the current line with character highlighting
        for (size_t i = 0; i < currentLine.length(); ++i) {
            if (i < displayedWords.length() && currentLine[i] == displayedWords[currentLineStart + i]) {
                // Correct character
                cout << currentLine[i];
            } else if (i < displayedWords.length()) {
                // Mistyped character (highlight in red)
                cout << "\033[31m" << currentLine[i] << "\033[0m";
            } else {
                // Remaining characters (not yet typed)
                cout << "_";
            }
        }

        // Move cursor back to the appropriate position after updating the current line
        cout << "\033[" << (frozenLineCount + 1) << "E" << flush;
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

    void playTimedGame() { // same as Game::playTimedGame()
        int timeLimit = (mode.getMode() == Seconds_10) ? 10 : (mode.getMode() == Seconds_30) ? 30 : 60;
        string words = wordManager.generateWords(80);
        string typedWords = "";
        display.showDisplayedWords(words, typedWords);

        auto startTime = chrono::steady_clock::now();
        while (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count() < timeLimit) {
            if (inputHandler.isKeyHit()) {
                char key = inputHandler.getKeyPress();
                
                if (key == '\b' && !typedWords.empty()) { // Handle backspace
                    typedWords.pop_back();
                } else if (key != '\b') {
                    typedWords += key;

                    // Check and record misclicks
                    if (typedWords.size() <= words.size() && typedWords.back() != words[typedWords.size() - 1]) {
                        player.incrementMisclicks();
                    } else if (typedWords.back() == ' ') {
                        player.incrementWordsTyped();
                    }
                }
                display.showTypingProgress(typedWords, words);
            }
        }
        gameOver = true;
        display.showGameOver(timeLimit, player, typedWords, "Timed");
    }


    void playCountGame() { 
        int maxWordCount = (mode.getMode() == Words_10) ? 10 : (mode.getMode() == Words_25) ? 25 : 50;
        string words = wordManager.generateWords(maxWordCount); // Generate words for the game
        string typedWords = "";
        display.showDisplayedWords(words, typedWords);

        auto startTime = chrono::steady_clock::now();
        while (!gameOver) {
            if (inputHandler.isKeyHit()) {
                char key = inputHandler.getKeyPress();

                if (key == '\b' && !typedWords.empty()) { // Handle backspace
                    typedWords.pop_back();
                } else if (key != '\b') {
                    typedWords += key;

                    // Check word completion
                    size_t typedWordCount = count(typedWords.begin(), typedWords.end(), ' ');
                    size_t displayedWordCount = count(words.begin(), words.end(), ' ');

                    if (typedWordCount > displayedWordCount || typedWords.back() != words[typedWords.size() - 1]) {
                        player.incrementMisclicks();
                    } else if (typedWords.back() == ' ') {
                        player.incrementWordsTyped();
                    }
                }
                display.showTypingProgress(typedWords, words);
            }

            // Check if all words have been typed correctly
            if (typedWords == words) {
                gameOver = true;
                break;
            }
        }
        auto endTime = chrono::steady_clock::now();
        chrono::duration<double> elapsedTime = endTime - startTime;

        display.showGameOver(elapsedTime.count(), player, typedWords, "Count Words");
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    Game game;
    game.setup();
    game.run();
    
    return 0;
}