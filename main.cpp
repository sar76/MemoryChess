#include <iostream>
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include "board_state.cpp"
#include "position_loader.cpp"

using namespace std;

// we want to display the board and then a palette on the right with all the pieces, that we can drag onto the board 

const int BOARD_SIZE = 800; 

const int SQUARE_SIZE = BOARD_SIZE / 8; 

const int PALETTE_WIDTH = 200;

const int WINDOW_WIDTH = BOARD_SIZE + PALETTE_WIDTH;

const int WINDOW_HEIGHT = BOARD_SIZE; 

// textures used by sfml to generate graphics -- stored in memory on GPU 
map<char, sf::Texture> pieceTextures; 

// function to load all piece textures
bool loadPieceTextures() {
    // map of piece characters to filenames
    map<char, string> pieceFiles = {
        {'K', "assets/wK.png"}, {'Q', "assets/wQ.png"}, 
        {'R', "assets/wR.png"}, {'B', "assets/wB.png"},
        {'N', "assets/wN.png"}, {'P', "assets/wP.png"},
        {'k', "assets/bK.png"}, {'q', "assets/bQ.png"},
        {'r', "assets/bR.png"}, {'b', "assets/bB.png"},
        {'n', "assets/bN.png"}, {'p', "assets/bP.png"}
    };
    
    // compiler figures out type of char, string pair 
    for (const auto& pair : pieceFiles) {
        // loadfromfile (part of sf::Texture )loads png into texture, key and path are provided from map above
        // checks if pair.first key exists in the map 
        // if it doesn't, create it and set a default texture
        // if it does, access the existing texture
        // then call loadfromfile on that texture 
        // so when the if conditional check passes, we fill the map pieceTextures
        if (!pieceTextures[pair.first].loadFromFile(pair.second)) {
            cerr << "Failed to load " << pair.second << endl;
            return false;
        }
    }
    
    return true;
}


struct DraggedPiece {
    char piece; 
    bool is_dragging;
    sf::Vector2f position; 

    DraggedPiece() : piece(' '), is_dragging(false), position(0, 0) {}
};

// drawing the chess board and painting it black and white
void draw_board(sf::RenderWindow& window) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            sf::RectangleShape square(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            square.setPosition(sf::Vector2f(file * SQUARE_SIZE, rank * SQUARE_SIZE));
            
            if ((rank + file) % 2 == 0) {
                square.setFillColor(sf::Color(240, 217, 181)); // light squares
            } else {
                square.setFillColor(sf::Color(181, 136, 99)); // dark squares 
            }
            
            window.draw(square);
        }
    }
}

// drawing palette to the right of the board
void drawPalette(sf::RenderWindow& window, sf::Font& font) {

    // making a rectangle for the palette background and then starting it at 800, 0, top right of the chess board
    sf::RectangleShape paletteBg(sf::Vector2f(PALETTE_WIDTH, WINDOW_HEIGHT));
    paletteBg.setPosition(sf::Vector2f(BOARD_SIZE, 0));
    // dark grey shade for the palette background 
    paletteBg.setFillColor(sf::Color(60, 60, 60));
    window.draw(paletteBg);
    
    
    char pieces[] = {'K', 'Q', 'R', 'B', 'N', 'P', 'k', 'q', 'r', 'b', 'n', 'p'};
    
    for (int i = 0; i < 12; i++) {
        char piece = pieces[i];

        // drawing the boxes that will hold the pieces 
        sf::RectangleShape pieceBox(sf::Vector2f(80, 60));
        pieceBox.setPosition(sf::Vector2f(BOARD_SIZE + 10, 10 + i * 65));
        pieceBox.setFillColor(sf::Color(100, 100, 100));
        pieceBox.setOutlineColor(sf::Color(200, 200, 200));
        pieceBox.setOutlineThickness(2);
        window.draw(pieceBox);

    }
    

    sf::Text instructions(font);
    instructions.setCharacterSize(14);
    instructions.setFillColor(sf::Color::White);
    // instructions to appear below the pieces on the bottom 
    instructions.setPosition(sf::Vector2f(BOARD_SIZE + 100, WINDOW_HEIGHT - 80));
    instructions.setString("Drag pieces\nonto board\n\nSpace: Check\nC: Clear\nN: New");
    window.draw(instructions);
}

// draw pieces on the board
void drawPieces(sf::RenderWindow& window, const board_state& board) {
    for (int square = 0; square < 64; square++) {
        char piece = board.get_piece_at(square);
        
        if (piece == ' ') continue;
        
        int file = square % 8;
        int rank = square / 8;
        
        // create sprite from texture
        sf::Sprite pieceSprite(pieceTextures[piece]);
        
        // scale the sprite to fit the square
        sf::Vector2u textureSize = pieceTextures[piece].getSize();
        float scaleX = (float)SQUARE_SIZE / textureSize.x;
        float scaleY = (float)SQUARE_SIZE / textureSize.y;
        pieceSprite.setScale(sf::Vector2f(scaleX, scaleY));
        
        // position the sprite
        pieceSprite.setPosition(sf::Vector2f(file * SQUARE_SIZE, rank * SQUARE_SIZE));
        
        window.draw(pieceSprite);
    }
}

// check if mouse is in palette and return piece if clicked
char getPieceFromPalette(int mouseX, int mouseY) {
    if (mouseX < BOARD_SIZE || mouseX > BOARD_SIZE + PALETTE_WIDTH) return ' ';
    
    char pieces[] = {'K', 'Q', 'R', 'B', 'N', 'P', 'k', 'q', 'r', 'b', 'n', 'p'};
    
    for (int i = 0; i < 12; i++) {
        int boxX = BOARD_SIZE + 10;
        int boxY = 10 + i * 65;
        
        // each box we drew is 80x60 so we are checking if our mouse is within this range
        if (mouseX >= boxX && mouseX <= boxX + 80 &&
            mouseY >= boxY && mouseY <= boxY + 60) {
            return pieces[i];
        }
    }
    
    return ' ';
}

// convert mouse coordinates to square index
int getSquareFromMouse(int mouseX, int mouseY) {
    // once we are sure we are in the board then only run the calculations of figuring out what square we are in
    if (mouseX < 0 || mouseX >= BOARD_SIZE || mouseY < 0 || mouseY >= BOARD_SIZE) return -1;
    
    int file = mouseX / SQUARE_SIZE;
    int rank = mouseY / SQUARE_SIZE;
    return rank * 8 + file;
}

// draw the piece being dragged
void drawDraggedPiece(sf::RenderWindow& window, const DraggedPiece& dragged) {
    if (!dragged.is_dragging) return;
    
    sf::Sprite pieceSprite(pieceTextures[dragged.piece]);
    
    sf::Vector2u textureSize = pieceTextures[dragged.piece].getSize();
    float scaleX = (float)SQUARE_SIZE / textureSize.x;
    float scaleY = (float)SQUARE_SIZE / textureSize.y;
    pieceSprite.setScale(sf::Vector2f(scaleX, scaleY));
    
    // center the piece on the cursor
    pieceSprite.setPosition(sf::Vector2f(dragged.position.x - SQUARE_SIZE/2, 
                           dragged.position.y - SQUARE_SIZE/2));
    
    window.draw(pieceSprite);
}

// draw piece images inside the palette boxes
void drawPalettePieces(sf::RenderWindow& window) {
    char pieces[] = {'K', 'Q', 'R', 'B', 'N', 'P', 'k', 'q', 'r', 'b', 'n', 'p'};
    
    for (int i = 0; i < 12; i++) {
        sf::Sprite pieceSprite(pieceTextures[pieces[i]]);
        
        // scale to fit in the box (50 pixels for 80x60 box)
        sf::Vector2u textureSize = pieceTextures[pieces[i]].getSize();
        float scale = 50.0f / textureSize.x;
        pieceSprite.setScale(sf::Vector2f(scale, scale));
        
        // center in the box
        // offset is (15, 10) within the boxes that we drew earlier 
        pieceSprite.setPosition(sf::Vector2f(BOARD_SIZE + 25, 20 + i * 65));
        
        window.draw(pieceSprite);
    }
}

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "Memory Chess");
    window.setFramerateLimit(60);
    
    // Load font
    sf::Font font;
    if (!font.openFromFile("fonts/comicbd.ttf")) {
        cerr << "Could not load font!" << endl;
        return 1;
    }

    
    // Load piece textures
    if (!loadPieceTextures()) {
        cerr << "Failed to load piece textures!" << endl;
        return 1;
    }
    
    // Load positions
    position_loader loader;
    vector<string> positions = loader.load_position("lichess_db_puzzle.csv");
    
    if (positions.empty()) {
        cerr << "No positions loaded. Exiting." << endl;
        return 1;
    }
    
    // Initialize random seed
    srand(time(0));
    
    // Game states
    enum GameState { MENU, MEMORIZING, PLAYING };
    GameState gameState = MENU;
    
    // Get a random position
    string randomFEN = loader.get_random_position(positions);
    cout << "Loaded position: " << randomFEN << endl;
    
    // Create board states
    board_state solutionBoard;
    board_state userBoard;
    
    solutionBoard.populate_from_FEN(randomFEN);
    
    // Timer for showing solution
    sf::Clock timer;
    float displayTime = 5.0f;
    
    // Dragging state
    DraggedPiece dragged;
    
    // Feedback state
    string feedbackMessage = "";
    sf::Clock feedbackTimer;
    float feedbackDisplayTime = 3.0f;
    bool showingFeedback = false;
    
    cout << "Welcome to Memory Chess!" << endl;
    
    // Game loop
    while (window.isOpen()) {
        // Event handling
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            // Menu state - waiting for start
            if (gameState == MENU) {
                if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPress->code == sf::Keyboard::Key::Space) {
                        gameState = MEMORIZING;
                        timer.restart();
                        cout << "Starting new puzzle! Memorize the position..." << endl;
                    }
                }
            }
            
            // Only allow interaction during PLAYING state
            else if (gameState == PLAYING) {
                // Mouse button pressed - start dragging
                if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mousePress->button == sf::Mouse::Button::Left) {
                        char piece = getPieceFromPalette(mousePress->position.x, mousePress->position.y);
                        if (piece != ' ') {
                            dragged.piece = piece;
                            dragged.is_dragging = true;
                            dragged.position = sf::Vector2f(mousePress->position.x, mousePress->position.y);
                            cout << "Started dragging: " << piece << endl;
                        }
                    } else if (mousePress->button == sf::Mouse::Button::Right) {
                        // Right click to clear square
                        int square = getSquareFromMouse(mousePress->position.x, mousePress->position.y);
                        if (square >= 0 && square < 64) {
                            userBoard.set_piece_at_square(square, ' ');
                            cout << "Cleared square " << square << endl;
                        }
                    }
                }
                
                // Mouse moved - update drag position
                if (const auto* mouseMove = event->getIf<sf::Event::MouseMoved>()) {
                    if (dragged.is_dragging) {
                        dragged.position = sf::Vector2f(mouseMove->position.x, mouseMove->position.y);
                    }
                }
                
                // Mouse button released - place piece
                if (const auto* mouseRelease = event->getIf<sf::Event::MouseButtonReleased>()) {
                    if (mouseRelease->button == sf::Mouse::Button::Left && dragged.is_dragging) {
                        int square = getSquareFromMouse(mouseRelease->position.x, mouseRelease->position.y);
                        if (square >= 0 && square < 64) {
                            userBoard.set_piece_at_square(square, dragged.piece);
                            cout << "Placed " << dragged.piece << " at square " << square << endl;
                        }
                        dragged.is_dragging = false;
                    }
                }
                
                // Keyboard input
                if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                    switch (keyPress->code) {
                        case sf::Keyboard::Key::Space:
                            if (userBoard == solutionBoard) {
                                feedbackMessage = "CORRECT! Perfect match!";
                                cout << "\n✓ CORRECT! You solved it perfectly!" << endl;
                            } else {
                                uint32_t correct = userBoard.how_many_squares_correct(solutionBoard);
                                feedbackMessage = "Not quite! " + to_string(correct) + "/64 squares correct";
                                cout << "\nNot quite! " << correct << "/64 squares correct." << endl;
                            }
                            showingFeedback = true;
                            feedbackTimer.restart();
                            break;
                            
                        case sf::Keyboard::Key::S:
                            gameState = MEMORIZING;
                            timer.restart();
                            cout << "Showing solution again..." << endl;
                            break;
                            
                        case sf::Keyboard::Key::C:
                            userBoard = board_state();
                            cout << "Board cleared." << endl;
                            break;
                            
                        case sf::Keyboard::Key::N:
                            randomFEN = loader.get_random_position(positions);
                            solutionBoard.populate_from_FEN(randomFEN);
                            userBoard = board_state();
                            gameState = MEMORIZING;
                            timer.restart();
                            showingFeedback = false;
                            cout << "\nNew position loaded!" << endl;
                            break;
                            
                        default:
                            break;
                    }
                }
            }
        }
        
        // Check if we should transition from MEMORIZING to PLAYING
        if (gameState == MEMORIZING && timer.getElapsedTime().asSeconds() >= displayTime) {
            gameState = PLAYING;
            cout << "\nSolution hidden! Recreate the position from memory." << endl;
        }
        
        // Check if we should hide feedback
        if (showingFeedback && feedbackTimer.getElapsedTime().asSeconds() >= feedbackDisplayTime) {
            showingFeedback = false;
        }
        
        // Rendering
        window.clear(sf::Color(40, 40, 40));
        
        draw_board(window);
        drawPalette(window, font);
        drawPalettePieces(window);
        
        // MENU STATE - Show welcome screen with instructions
        if (gameState == MENU) {
            // Semi-transparent overlay
            sf::RectangleShape overlay(sf::Vector2f(BOARD_SIZE, BOARD_SIZE));
            overlay.setPosition(sf::Vector2f(0, 0));
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);
            
            // Title
            sf::Text title(font);
            title.setCharacterSize(60);
            title.setFillColor(sf::Color(255, 215, 0));
            title.setOutlineColor(sf::Color::Black);
            title.setOutlineThickness(3);
            title.setString("MEMORY CHESS");
            
            sf::FloatRect titleBounds = title.getLocalBounds();
            title.setOrigin(sf::Vector2f(titleBounds.position.x + titleBounds.size.x / 2.0f,
                           titleBounds.position.y + titleBounds.size.y / 2.0f));
            title.setPosition(sf::Vector2f(BOARD_SIZE / 2.0f, 100));
            window.draw(title);
            
            // Instructions
            sf::Text instructions(font);
            instructions.setCharacterSize(20);
            instructions.setFillColor(sf::Color::White);
            instructions.setString(
                "HOW TO PLAY:\n\n"
                "1. Memorize the chess position shown\n"
                "2. Recreate it from memory by dragging pieces\n"
                "3. Check your accuracy!\n\n\n"
                "CONTROLS:\n\n"
                "Left Click - Drag pieces from palette to board\n"
                "Right Click - Clear a square\n"
                "SPACE - Check your solution / Start puzzle\n"
                "S - Show solution again (5 seconds)\n"
                "C - Clear the board\n"
                "N - New puzzle\n\n\n"
            );
            
            sf::FloatRect instBounds = instructions.getLocalBounds();
            instructions.setOrigin(sf::Vector2f(instBounds.position.x + instBounds.size.x / 2.0f,
                                  instBounds.position.y + instBounds.size.y / 2.0f));
            instructions.setPosition(sf::Vector2f(BOARD_SIZE / 2.0f, 400));  // prev: y --> 340
            window.draw(instructions);
            
            // Start button prompt
            sf::Text startPrompt(font);
            startPrompt.setCharacterSize(32);
            startPrompt.setFillColor(sf::Color(100, 255, 100));
            startPrompt.setOutlineColor(sf::Color::Black);
            startPrompt.setOutlineThickness(2);
            startPrompt.setString("Press SPACE to Start!");
            
            sf::FloatRect promptBounds = startPrompt.getLocalBounds();
            startPrompt.setOrigin(sf::Vector2f(promptBounds.position.x + promptBounds.size.x / 2.0f,
                                 promptBounds.position.y + promptBounds.size.y / 2.0f));
            startPrompt.setPosition(sf::Vector2f(BOARD_SIZE / 2.0f, 680));
            
            // Blinking effect
            if (((int)(timer.getElapsedTime().asSeconds() * 2)) % 2 == 0) {
                window.draw(startPrompt);
            }
        }
        
        // MEMORIZING STATE - Show position with countdown
        else if (gameState == MEMORIZING) {
            drawPieces(window, solutionBoard);
            
            // Memorization message at top
            sf::Text memoryMessage(font);
            memoryMessage.setCharacterSize(28);
            memoryMessage.setFillColor(sf::Color::White);
            memoryMessage.setOutlineColor(sf::Color::Black);
            memoryMessage.setOutlineThickness(2);
            memoryMessage.setString("Memorize this position!");
            
            sf::FloatRect msgBounds = memoryMessage.getLocalBounds();
            memoryMessage.setOrigin(sf::Vector2f(msgBounds.position.x + msgBounds.size.x / 2.0f,
                                   msgBounds.position.y + msgBounds.size.y / 2.0f));
            memoryMessage.setPosition(sf::Vector2f(BOARD_SIZE / 2.0f, 40));
            window.draw(memoryMessage);
            
            // Countdown timer
            sf::Text countdown(font);
            countdown.setCharacterSize(72);
            countdown.setFillColor(sf::Color(255, 100, 100));
            countdown.setOutlineColor(sf::Color::Black);
            countdown.setOutlineThickness(3);
            
            int timeLeft = (int)(displayTime - timer.getElapsedTime().asSeconds()) + 1;
            countdown.setString(to_string(timeLeft));
            
            sf::FloatRect bounds = countdown.getLocalBounds();
            countdown.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.0f,
                               bounds.position.y + bounds.size.y / 2.0f));
            countdown.setPosition(sf::Vector2f(BOARD_SIZE / 2.0f, BOARD_SIZE / 2.0f));
            
            window.draw(countdown);
        }
        
        // PLAYING STATE - Show user's recreation
        else if (gameState == PLAYING) {
            drawPieces(window, userBoard);
            drawDraggedPiece(window, dragged);
            
            // Instructions at top
            sf::Text playInstructions(font);
            playInstructions.setCharacterSize(18);
            playInstructions.setFillColor(sf::Color(200, 200, 200));
            playInstructions.setString("Recreate the position from memory - Press SPACE to check");
            
            sf::FloatRect playBounds = playInstructions.getLocalBounds();
            playInstructions.setOrigin(sf::Vector2f(playBounds.position.x + playBounds.size.x / 2.0f,
                                       playBounds.position.y + playBounds.size.y / 2.0f));
            playInstructions.setPosition(sf::Vector2f(BOARD_SIZE / 2.0f, 25));
            window.draw(playInstructions);
        }
        
        // Draw feedback message if active (overlays on any state)
        if (showingFeedback) {
            sf::Text feedback(font);
            feedback.setCharacterSize(36);
            
            // Color based on message
            if (feedbackMessage.find("CORRECT") != string::npos) {
                feedback.setFillColor(sf::Color(100, 255, 100));
            } else {
                feedback.setFillColor(sf::Color(255, 200, 100));
            }
            
            feedback.setOutlineColor(sf::Color::Black);
            feedback.setOutlineThickness(3);
            feedback.setString(feedbackMessage);
            
            sf::FloatRect fbBounds = feedback.getLocalBounds();
            feedback.setOrigin(sf::Vector2f(fbBounds.position.x + fbBounds.size.x / 2.0f,
                              fbBounds.position.y + fbBounds.size.y / 2.0f));
            feedback.setPosition(sf::Vector2f(BOARD_SIZE / 2.0f, BOARD_SIZE - 60));
            
            window.draw(feedback);
        }
        
        window.display();
    }
    
    return 0;
}