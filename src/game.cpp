#include "../include/game.h"
#include "../include/commands.h"
#include "../include/utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <conio.h>
#include <thread>
#include <chrono>
#include <iomanip>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

void Game::clearUndoStack() {
    for (auto* cmd : undoStack) delete cmd;
    undoStack.clear();
}

Game::Game(int numTab, int drawN, bool largeCards, bool polish) 
    : moveCount(0), drawNum(drawN), numTableaus(numTab), 
      wasteWindowStart(0), wasteWindowSize(0), largeCards(largeCards),
      polish(polish), gameStartTime(std::chrono::steady_clock::now()) {
    for (int s = Hearts; s <= Spades; ++s) foundations.emplace_back(static_cast<Suit>(s));
    tableaus.resize(numTableaus);
}

Game::~Game() { clearUndoStack(); }

void Game::setup() {
    Deck deck;
    deck.shuffle();
    for (int i = 0; i < numTableaus; ++i) {
        for (int j = 0; j < i; ++j) tableaus[i].addInitialCard(deck.draw(), false);
        tableaus[i].addInitialCard(deck.draw(), true);
    }
    while (!deck.empty()) stock.addCards({deck.draw()});
}

std::wstring Game::getCardDisplay(const Card& card) const {
    if (!largeCards) {
        return card.toWString();
    }

    std::wstringstream ss;
    static const std::wstring ranks = L"A23456789TJQK";
    static const std::wstring suits = L"\u2665\u2666\u2663\u2660"; // ♥♦♣♠
    
    // Card top
    ss << L"┌────────┐\n";
    // Card middle
    ss << L"│   " << ranks[card.getRank()] << L"" << suits[card.getSuit()] << L"   │\n";
    ss << L"│    "<< L"    │\n";
    ss << L"│   " << ranks[card.getRank()] << L"" << suits[card.getSuit()] << L"   │\n";
    // Card bottom
    ss << L"└────────┘";
    return ss.str();
}

std::wstring Game::getEmptyCardDisplay() const {
    if (!largeCards) {
        return L"XX";
    }
    
    return L"┌────────┐\n"
           L"│        │\n"
           L"│        │\n"
           L"│        │\n"
           L"└────────┘";
}

void Game::display() const {
    clearScreen();
    std::wstring buf;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    auto to_wstr = [](int v) {
        std::wstringstream ss;
        ss << v;
        return ss.str();
    };

    // Display foundations in one row
    static const wchar_t* foundationSuits[] = {L"♥", L"♦", L"♣", L"♠"};
    buf += polish ? L"Stosy bazowe:\r\n" : L"Foundation:\r\n"; // Add newline after label
    
    // First row: foundation labels with their respective colors
    for (size_t i = 0; i < foundations.size(); ++i) {
        if (i < 2) { // Hearts and Diamonds
            buf += L"\x1B[91m"; // bright red
        } else { // Clubs and Spades
            buf += L"\x1B[90m"; // bright black
        }
        buf += L"F" + to_wstr((int)i+1) + foundationSuits[i];
        buf += L"\x1B[0m"; // reset color
        buf += std::wstring(largeCards ? 8 : 2, L' '); // 8 spaces for large cards
    }
    buf += L"\r\n";

    // For large cards, we need to display each row of the card separately
    if (largeCards) {
        // Get all foundation cards
        std::vector<std::vector<std::wstring>> foundationCardLines(foundations.size());
        for (size_t i = 0; i < foundations.size(); ++i) {
            const auto& cs = foundations[i].getCards();
            if (!cs.empty()) {
                std::wstring card = getCardDisplay(cs.back());
                std::wistringstream iss(card);
                std::wstring line;
                while (std::getline(iss, line)) {
                    if (cs.back().isRed()) {
                        foundationCardLines[i].push_back(L"\x1B[91m" + line + L"\x1B[0m"); // bright red
                    } else {
                        foundationCardLines[i].push_back(L"\x1B[90m" + line + L"\x1B[0m"); // bright black
                    }
                }
            } else {
                foundationCardLines[i] = std::vector<std::wstring>(5, std::wstring(10, L' '));
            }
        }

        // Display each row of the cards
        for (size_t row = 0; row < 5; ++row) {
            for (size_t i = 0; i < foundations.size(); ++i) {
                buf += foundationCardLines[i][row];
                buf += std::wstring(0, L' '); // No spacing between cards
            }
            buf += L"\r\n";
        }
    } else {
        // For small cards, display normally
        for (size_t i = 0; i < foundations.size(); ++i) {
            const auto& cs = foundations[i].getCards();
            if (!cs.empty()) {
                if (cs.back().isRed()) {
                    buf += L"\x1B[91m"; // bright red
                } else {
                    buf += L"\x1B[90m"; // bright black
                }
                buf += getCardDisplay(cs.back());
                buf += L"\x1B[0m";
            } else {
                buf += std::wstring(2, L' ');
            }
            buf += std::wstring(0, L' '); // No spacing between cards
        }
        buf += L"\r\n";
    }
    buf += L"\r\n"; // Extra newline after foundations

    // Display tableau
    buf += polish ? L"Kolumny:\r\n" : L"Tableau:\r\n";
    
    // Print tableau labels in one row
    for (size_t i = 0; i < tableaus.size(); ++i) {
        buf += L"T" + to_wstr((int)i + 1);
        // Add appropriate spacing based on card size
        buf += std::wstring(largeCards ? 8 : 2, L' '); // 8 spaces for large cards
    }
    buf += L"\r\n";

    // Prepare card lines for each tableau
    size_t maxCards = 0;
    for (const auto& tableau : tableaus) {
        maxCards = std::max(maxCards, tableau.getFaceDown().size() + tableau.getFaceUp().size());
    }
    const size_t cardHeight = largeCards ? 5 : 1;
    const size_t cardWidth = largeCards ? 10 : 2;
    const int overlap = largeCards ? 3 : 0; // -3 for large cards, 0 for small

    // For each tableau, build a vector<vector<wstring>>: each card split into lines
    std::vector<std::vector<std::vector<std::wstring>>> tableauLines(tableaus.size());
    for (size_t t = 0; t < tableaus.size(); ++t) {
        auto& lines = tableauLines[t];
        const auto& faceDown = tableaus[t].getFaceDown();
        const auto& faceUp = tableaus[t].getFaceUp();
        // Face-down cards
        for (size_t j = 0; j < faceDown.size(); ++j) {
            std::wstring card = getEmptyCardDisplay();
            std::vector<std::wstring> cardLines;
            if (largeCards) {
                std::wistringstream iss(card);
                std::wstring line;
                while (std::getline(iss, line)) cardLines.push_back(line);
            } else {
                cardLines.push_back(card);
            }
            lines.push_back(cardLines);
        }
        // Face-up cards
        for (size_t j = 0; j < faceUp.size(); ++j) {
            std::wstring card = getCardDisplay(faceUp[j]);
            std::vector<std::wstring> cardLines;
            if (largeCards) {
                std::wistringstream iss(card);
                std::wstring line;
                while (std::getline(iss, line)) cardLines.push_back(line);
                // Colorize each line if needed
                if (faceUp[j].isRed()) {
                    for (auto& l : cardLines) l = L"\x1B[91m" + l + L"\x1B[0m"; // bright red
                } else {
                    for (auto& l : cardLines) l = L"\x1B[90m" + l + L"\x1B[0m"; // bright black
                }
            } else {
                cardLines.push_back(faceUp[j].isRed() ? L"\x1B[91m" + card + L"\x1B[0m" : L"\x1B[90m" + card + L"\x1B[0m");
            }
            lines.push_back(cardLines);
        }
    }

    if (largeCards) {
        // --- REWRITTEN OVERLAPPING DISPLAY FOR LARGE CARDS (TOP-ALIGNED) ---
        std::vector<std::vector<std::wstring>> tableauBuffers(tableaus.size());
        size_t maxHeight = 0;
        const int overlap = 3;
        for (size_t t = 0; t < tableaus.size(); ++t) {
            size_t n = tableauLines[t].size();
            if (n == 0) {
                tableauBuffers[t] = std::vector<std::wstring>(1, std::wstring(cardWidth, L' '));
                continue;
            }
            size_t height = cardHeight + (n - 1) * (cardHeight - overlap);
            maxHeight = std::max(maxHeight, height);
            tableauBuffers[t] = std::vector<std::wstring>(height, std::wstring(cardWidth, L' '));
            for (size_t k = 0; k < n; ++k) {
                size_t start = k * (cardHeight - overlap);
                for (size_t l = 0; l < cardHeight; ++l) {
                    if (start + l < tableauBuffers[t].size() && tableauLines[t][k].size() > l) {
                        tableauBuffers[t][start + l] = tableauLines[t][k][l];
                    }
                }
            }
        }
        // Pad each column at the bottom to maxHeight
        for (size_t t = 0; t < tableaus.size(); ++t) {
            if (tableauBuffers[t].size() < maxHeight) {
                tableauBuffers[t].resize(maxHeight, std::wstring(cardWidth, L' '));
            }
        }
        // Print row by row
        for (size_t row = 0; row < maxHeight; ++row) {
            for (size_t t = 0; t < tableaus.size(); ++t) {
                buf += tableauBuffers[t][row];
                buf += std::wstring(0, L' '); // 8 spaces between columns
            }
            buf += L"\r\n";
        }
    } else {
        // --- STANDARD DISPLAY FOR SMALL CARDS ---
        for (size_t cardIdx = 0; cardIdx < maxCards; ++cardIdx) {
            for (size_t t = 0; t < tableaus.size(); ++t) {
                if (tableauLines[t].size() > cardIdx && tableauLines[t][cardIdx].size() > 0) {
                    buf += tableauLines[t][cardIdx][0];
                } else {
                    buf += std::wstring(cardWidth, L' ');
                }
                buf += std::wstring(2, L' '); // Reduced spacing between tableaus for small cards
            }
            buf += L"\r\n";
        }
    }

    // Display waste
    buf += polish ? L"\r\nStos [" : L"\r\nWaste [";
    buf += to_wstr((int)waste.getCards().size()) + L"]:\r\n";
    const auto& wc = waste.getCards();
    size_t start = wasteWindowStart;
    size_t end = start + wasteWindowSize;
    if (end > wc.size()) end = wc.size();
    
    if (largeCards) {
        // For large cards, display waste cards in a row
        std::vector<std::vector<std::wstring>> wasteCardLines;
        for (size_t i = start; i < end; ++i) {
            std::wstring card = getCardDisplay(wc[i]);
            std::wistringstream iss(card);
            std::wstring line;
            std::vector<std::wstring> cardLines;
            while (std::getline(iss, line)) {
                if (wc[i].isRed()) {
                    cardLines.push_back(L"\x1B[91m" + line + L"\x1B[0m"); // bright red
                } else {
                    cardLines.push_back(L"\x1B[90m" + line + L"\x1B[0m"); // bright black
                }
            }
            wasteCardLines.push_back(cardLines);
        }

        // Display each row of the cards
        for (size_t row = 0; row < 5; ++row) {
            for (size_t i = 0; i < wasteCardLines.size(); ++i) {
                buf += wasteCardLines[i][row];
                buf += std::wstring(0, L' '); // No spacing between cards
            }
            buf += L"\r\n";
        }
    } else {
        // For small cards, display normally
        for (size_t i = start; i < end; ++i) {
            if (wc[i].isRed()) {
                buf += L"\x1B[91m"; // bright red
            } else {
                buf += L"\x1B[90m"; // bright black
            }
            buf += getCardDisplay(wc[i]) + L" ";
            buf += L"\x1B[0m";
        }
        buf += L"\r\n";
    }
    buf += L"\r\n"; // Extra newline after waste

    buf += polish ? L"Komendy: d, m <skąd> <dokąd> [n], u, q, r\r\n" : L"Commands: d, m <from> <to> [n], u, q, r\r\n";
    
    // Format time as HH:MM:SS
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - gameStartTime).count();
    int hours = elapsed / 3600;
    int minutes = (elapsed % 3600) / 60;
    int seconds = elapsed % 60;
    std::wstringstream timeSS;
    timeSS << std::setfill(L'0') << std::setw(2) << hours << L":"
           << std::setfill(L'0') << std::setw(2) << minutes << L":"
           << std::setfill(L'0') << std::setw(2) << seconds;
    buf += polish ? L"Czas: " : L"Time: ";
    buf += timeSS.str() + L"\r\n";

    DWORD written = 0;
    WriteConsoleW(hOut, buf.c_str(), (DWORD)buf.size(), &written, nullptr);
}

void Game::play() {
    setup();
    moveCount = 0;
    clearUndoStack();
    gameStartTime = std::chrono::steady_clock::now();

    std::string input;
    while (true) {
        display();
        std::cout << "Move #" << moveCount << ": ";
        std::getline(std::cin, input);

        auto trim = [](std::string& s) {
            auto f = s.find_first_not_of(" \t\r\n");
            auto l = s.find_last_not_of(" \t\r\n");
            s = (f == std::string::npos) ? "" : s.substr(f, l - f + 1);
        };
        trim(input);
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        if (input.empty()) continue;

        char cmd = input[0];
        if (cmd == 'q') {
            break;
        }
        else if (cmd == 'r') {
            clearUndoStack();
            moveCount = 0;
            stock = Stock();
            waste = Waste();
            foundations.clear();
            for (int s = Hearts; s <= Spades; ++s)
                foundations.emplace_back(static_cast<Suit>(s));
            tableaus.clear();
            tableaus.resize(numTableaus);
            setup();
            continue;
        }
        else if (cmd == 'd') {
            auto *c = new DrawCommand(this, drawNum);
            c->execute();
            undoStack.push_back(c);
            if (undoStack.size() > 3) {
                delete undoStack.front();
                undoStack.erase(undoStack.begin());
            }
            ++moveCount;
        }
        else if (cmd == 'u') {
            if (!undoStack.empty()) {
                auto *c = undoStack.back();
                undoStack.pop_back();
                c->undo();
                delete c;
                --moveCount;
            }
        }
        else if (cmd == 'm') {
            std::istringstream iss(input.substr(1));
            std::string from, to;
            size_t num = 1;
            iss >> from >> to >> num;
            if (from.empty() || to.empty()) {
                std::cout << "Niepoprawny format. Użyj: m <skąd> <dokąd> [<n>]\n";
                continue;
            }
            from[0] = toupper(from[0]);
            to[0] = toupper(to[0]);

            Command *cmdObj = nullptr;
            if (from[0]=='T' && to[0]=='T' && num > 1) {
                int f = std::stoi(from.substr(1)) - 1;
                int t = std::stoi(to.substr(1)) - 1;
                if (f<0 || f>=numTableaus || t<0 || t>=numTableaus) {
                    std::cout << "Niepoprawny indeks kolumny.\n";
                    continue;
                }
                cmdObj = new MoveSequenceCommand(this, f, t, num);
            } else {
                cmdObj = new MoveCardCommand(this, from, to);
            }

            cmdObj->execute();
            if (cmdObj->isExecuted()) {
                undoStack.push_back(cmdObj);
                if (undoStack.size() > 3) {
                    delete undoStack.front();
                    undoStack.erase(undoStack.begin());
                }
                ++moveCount;
            } else {
                delete cmdObj;
            }
        }
        else {
            std::cout << "Niepoprawna komenda. (q,r,d,u,m)\n";
            Sleep(300);
        }

        if (checkWin()) {
            std::cout << "Wygrałeś w " << moveCount << " ruchach! (n – nowa gra, q – wyjście)\n";
            std::string resp;
            std::getline(std::cin, resp);
            if (!resp.empty() && (resp[0]=='n' || resp[0]=='N')) {
                clearUndoStack();
                moveCount = 0;
                stock = Stock();
                waste = Waste();
                foundations.clear();
                for (int s = Hearts; s <= Spades; ++s)
                    foundations.emplace_back(static_cast<Suit>(s));
                tableaus.clear();
                tableaus.resize(numTableaus);
                setup();
            } else {
                break;
            }
        }
    }
}

bool Game::checkWin() const {
    for (const auto& f : foundations)
        if (f.getCards().size() != 13) return false;
    return true;
}

void Game::showInstructions(bool isEnglish) const {
    clearScreen();
    std::wstringstream ss;
    
    if (!isEnglish) {
        // Polish version
        ss << L"\x1B[1;36m" << L"PASJANS – Klasyczna Gra Karciana\x1B[0m\r\n\r\n";
        
        ss << L"\x1B[1;33m" << L"O grze:\x1B[0m\r\n";
        ss << L"Pasjans (z franc. patience) to gra karciana dla jednej osoby. Twoim celem jest uporządkowanie wszystkich kart do czterech stosów bazowych (Foundation), zaczynając od Asa aż do Króla, zachowując odpowiedni kolor i kolorystykę.\r\n";
        ss << L"Wersja, którą grasz, bazuje na klasycznym wariancie Klondike, znanym z systemów Windows i uwielbianym przez miliony graczy na całym świecie.\r\n\r\n";
        
        ss << L"\x1B[1;33m" << L"Jak wygrać:\x1B[0m\r\n";
        ss << L"Przenieś wszystkie 52 karty z talii na cztery stosy bazowe, układając je rosnąco według koloru: \x1B[90m♠\x1B[0m \x1B[90m♣\x1B[0m \x1B[91m♥\x1B[0m \x1B[91m♦\x1B[0m. Wykorzystuj puste miejsca, przenoś całe sekwencje i planuj każdy ruch!\r\n\r\n";
        
        ss << L"\x1B[1;33m" << L"Sterowanie:\x1B[0m\r\n";
        ss << L"\x1B[1;32md\x1B[0m – Dobierz kartę z talii (Draw)\r\n\r\n";
        ss << L"\x1B[1;32mm <skąd> <dokąd> [n]\x1B[0m – Przenieś kartę lub sekwencję\r\n";
        ss << L"<skąd>: W (Stos odrzuconych) lub T1–T7 (Kolumny robocze)\r\n";
        ss << L"<dokąd>: F1–F4 (Stosy bazowe) lub T1–T7\r\n";
        ss << L"[n]: liczba kart do przeniesienia (domyślnie 1)\r\n\r\n";
        ss << L"\x1B[1;32mu\x1B[0m – Cofnij ostatni ruch (Undo)\r\n";
        ss << L"\x1B[1;32mr\x1B[0m – Zrestartuj grę (nowe rozdanie)\r\n";
        ss << L"\x1B[1;32mq\x1B[0m – Zakończ grę (Quit)\r\n\r\n";
    } else {
        // English version
        ss << L"\x1B[1;36m" << L"SOLITAIRE – Classic Card Game\x1B[0m\r\n\r\n";
        
        ss << L"\x1B[1;33m" << L"About the Game:\x1B[0m\r\n";
        ss << L"Solitaire (from French patience) is a single-player card game. Your goal is to move all cards to the four Foundation piles, building up from Ace to King, one suit per pile.\r\n";
        ss << L"This version is based on the classic Klondike, made famous by Microsoft Windows and beloved by millions.\r\n\r\n";
        
        ss << L"\x1B[1;33m" << L"How to Win:\x1B[0m\r\n";
        ss << L"Move all 52 cards from the deck into the Foundations, following ascending order and matching suits: \x1B[90m♠\x1B[0m \x1B[90m♣\x1B[0m \x1B[91m♥\x1B[0m \x1B[91m♦\x1B[0m. Use empty tableau slots, move card sequences, and think ahead!\r\n\r\n";
        
        ss << L"\x1B[1;33m" << L"Controls:\x1B[0m\r\n";
        ss << L"\x1B[1;32md\x1B[0m – Draw a card from the deck\r\n\r\n";
        ss << L"\x1B[1;32mm <from> <to> [n]\x1B[0m – Move a card or sequence\r\n";
        ss << L"<from>: W (Waste) or T1–T7 (Tableau)\r\n";
        ss << L"<to>: F1–F4 (Foundations) or T1–T7\r\n";
        ss << L"[n]: number of cards to move (default is 1)\r\n\r\n";
        ss << L"\x1B[1;32mu\x1B[0m – Undo the last move\r\n";
        ss << L"\x1B[1;32mr\x1B[0m – Restart the game\r\n";
        ss << L"\x1B[1;32mq\x1B[0m – Quit the game\r\n\r\n";
    }
    
    ss << L"\x1B[1;33m" << L"Press any key to continue...\x1B[0m";
    
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written = 0;
    WriteConsoleW(hOut, ss.str().c_str(), (DWORD)ss.str().size(), &written, nullptr);
    
    _getch(); // Wait for any key press
} 