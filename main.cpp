#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include <fstream>

int state{1};

std::string slova[30] = {"a","b","v","g","d","dj","e","ž","z","i","j","k","l","lj","m","n","nj","o","p","r","s","t","ć","u","f","h","c","č","dž","š"};

bool shaky{false};

using namespace std;

string read(string name) {
        std::ifstream input(name);
        string line;
        string result;
        input >> line;
        while (std::getline(input, line)) {
                input >> line;
                result = line + "\n";
        }
        return result;
}

void write(string name, vector<string> lines) {
        std::ofstream output(name);
        if (output.is_open()) {
                for (const std::string& line : lines) {
                        output << line << "\n";
                }
                output.close();
        }
}

struct vec2 {
        float x,y;
};



sf::ConvexShape createSquircle(vec2 topLeft, vec2 size, sf::Color color) {
        sf::ConvexShape squircle;

        // broj tacaka

        const unsigned int pointCount = 64;
        squircle.setPointCount(pointCount);

        // fancy math
        vec2 center = { topLeft.x + (size.x / 2.f), topLeft.y + (size.y / 2.f) };
        float rx = size.x / 2.f;
        float ry = size.y / 2.f;
        const float pi = 3.14159265f;

        for (unsigned int i = 0; i < pointCount; ++i) {
                // trigonometrija sa gugla za ugao
                float angle = (static_cast<float>(i) / pointCount) * 2.f * pi;
                float cosA = std::cos(angle);
                float sinA = std::sin(angle);

                // koordinate za tacke
                float xOffset = rx * (cosA >= 0 ? 1.f : -1.f) * std::sqrt(std::abs(cosA));
                float yOffset = ry * (sinA >= 0 ? 1.f : -1.f) * std::sqrt(std::abs(sinA));

                // postavljanje tacaka
                squircle.setPoint(i, sf::Vector2f(center.x + xOffset, center.y + yOffset));
        }

        squircle.setFillColor(color);
        return squircle;
}


void shake(sf::RenderWindow& window) {
        window.display();
        sf::View defaultView = window.getView();
        sf::View cameraView = defaultView;

        sf::Vector2u windowSize = window.getSize();
        sf::Texture backgroundTexture;
        if (!backgroundTexture.resize({windowSize.x, windowSize.y})) {
                return;
        }
        backgroundTexture.update(window);
        sf::Sprite backgroundSprite(backgroundTexture);

        sf::Clock shakeClock;
        const float duration = 0.35f;
        const float magnitude = 12.f;
        const float speed = 65.f;



        while (shakeClock.getElapsedTime().asSeconds() < duration) {
                float elapsed = shakeClock.getElapsedTime().asSeconds();


                float damping = 1.f - (elapsed / duration);
                float offsetX = std::sin(elapsed * speed) * magnitude * damping;

                cameraView.setCenter(defaultView.getCenter() + sf::Vector2f(offsetX, 0.f));
                window.setView(cameraView);

                window.clear(sf::Color(30, 30, 32));

                window.draw(backgroundSprite);


                window.display();
        }

        window.setView(defaultView);
}

int ran(int a)
{

        int b = (rand() % a) + 1;

        return b;
}


vector<string> random_slova(int q)
{
        vector<string> result;
        srand(time(NULL));
        for (int i = 0; i < q; ++i)
        {
                int a = ran(30);
                result.push_back(::slova[a]);
                cout << ::slova[a] << endl;
        }

        return result;

}



class textBox {
private:
        vec2 pos;
        vec2 size;
        bool visible;
        bool selected{false};
        string text;
        int menu_loc{};
        bool locked{false};

public:
        textBox(vec2 _pos, vec2 _size, int _m) : pos(_pos), size(_size), menu_loc(_m) {this->visible = true;}
        textBox(vec2 _pos, vec2 _size, int _m, bool display) : pos(_pos), size(_size), menu_loc(_m), locked(display) {this->visible = true;}

        vec2 getPos() const { return pos; }
        vec2 getSize() const { return size; }

        void use() { if (/*::state == menu_loc*/ true) visible = true; }

        void select(sf::RenderWindow& window) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                vec2 mouse{float(mousePos.x), float(mousePos.y)};
                if (visible && !locked) {
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                                if ((mouse.x >= pos.x && mouse.x <= pos.x + size.x) && (mouse.y >= pos.y && mouse.y <= pos.y + size.y)) {
                                        selected = true;
                                }
                                else {
                                        selected = false;
                                }

                        }

                }
        }

        void block(sf::RenderWindow& window, sf::Sound& sound) {


                sound.play();

                // popravka za audio driver
                sf::Clock safetyTimeout;
                while (sound.getStatus() != sf::Sound::Status::Playing && safetyTimeout.getElapsedTime().asMilliseconds() < 50) {
                        while (auto event = window.pollEvent()) {
                                // da se ne crashuje pod peer pressure
                                if (event->is<sf::Event::Closed>()) {
                                        window.close();
                                        return;
                                }
                        }
                }




        }





        void input(char32_t c, sf::RenderWindow& window, sf::Sound& sound) {
                const int BACKSPACE{8};
                const int SPACE{32};

                int d = static_cast<int>(c);

                bool isEnglishOrSpace = (d >= 65 && d <= 90) || (d >= 97 && d <= 122) || d == SPACE;

                bool isSerbianLatin = (d == 269 || d == 268) || // č, Č
                                      (d == 263 || d == 262) || // ć, Ć
                                      (d == 273 || d == 272) || // đ, Đ
                                      (d == 353 || d == 352) || // š, Š
                                      (d == 382 || d == 381);   // ž, Ž

                /*if ((d >= 65 && d <= 90) || (d >= 97 && d <= 122) || d == SPACE)*/
                if (isEnglishOrSpace || isSerbianLatin){

                        if ((size.x - 10) - text.length() * 12 > 0) {
                                sf::String incomingCharacter(c);
                                text += incomingCharacter.toAnsiString();
                        }
                        else {

                                // window.display();

                                ::shaky = true;

                                // block(window, sound);
                        }

                }
                else if (d == BACKSPACE) {
                        if (text.length() > 0) {text.pop_back();}
                }

                 // cout << text << "|\n" << std::flush;

        }


        void displayText(sf::Font& font, unsigned int Fsize, sf::RenderWindow& window) {
                sf::Text t(font);
                t.setCharacterSize(Fsize);
                t.setFillColor(sf::Color::Black);
                t.setString(text);
                t.setPosition({std::floor(pos.x + size.x/2 - Fsize/2 * text.length()/(1.8f)), std::floor(pos.y + size.y/2 - Fsize/4*3)});

                window.draw(t);


        }

        bool isSelected() const {return selected;}

        void setText(string _t) {text = _t;}

        void Draw(sf::RenderWindow& window) {

                auto shape = createSquircle(pos, size, sf::Color::White);

                window.draw(shape);

        }


};

void ui(vector<textBox>& boxes, sf::RenderWindow& window, sf::Font& font) {
        for (auto& box : boxes) {
                box.use();
                box.select(window);
                box.Draw(window);
                box.displayText(font, 18,window);
        }
}







int main() {

        sf::ContextSettings settings;
        settings.antiAliasingLevel = 8;


        sf::RenderWindow window(sf::VideoMode({640, 480}), "Service",sf::State::Windowed, settings);



        // Boxes ---------------------------------------------------------------------------------------------------------

        vector<textBox> boxes;
        boxes.push_back(textBox({100,100}, {100, 50}, 1));

        // Font ----------------------------------------------------------------------------------------------------------


        sf::Font font;
        if (!font.openFromFile("../assets/bold.ttf")) cerr << "Error loading font." << endl;


        // Sound ---------------------------------------------------------------------------------------------------------

        sf::SoundBuffer blockBuffer;
        if (!blockBuffer.loadFromFile("../assets/cutBlock.wav")) cerr << "Error loading sound." << endl;
        sf::Sound blockSound(blockBuffer);
        blockSound.setVolume(100);

        /*blockSound.play();
        sf::sleep(sf::milliseconds(1));
        blockSound.stop();*/

        // GAMESTATE 1 - RANDOM SLOVA ------------------------------------------------------------------------------------
        vector<string> pogadjanje = random_slova(8);

        vector<textBox> slBox;
        for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 4; j++) {
                        slBox.push_back(textBox({200 + j*80.f, 100 + i*100.f}, {40.f,40.f}, 1, true ));
                        slBox[i*4+j].setText(pogadjanje[i*4+j]);
                }
        }


        while (window.isOpen()) {
                while (auto event = window.pollEvent()) {
                        if (event->is<sf::Event::Closed>()) {
                                window.close();
                        }
                        if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                                for (auto& box : boxes) {
                                        if (box.isSelected()) {
                                                box.input(textEvent->unicode, window, blockSound);
                                        }
                                }

                        }
                }
                window.clear(sf::Color::Black);

                const int POGADJANJE{1};
        // GAMESTATE ETC -------------------------------------------------------------------------------------------------
                if (::state == POGADJANJE)
                {

                        for (short i = 0; i < pogadjanje.size(); i++) {}
                        ui(slBox, window, font);
                }



       //  ---------------------------------------------------------------------------------------------------------------
                // BLOCKED FOR EXCEEDING THE CHARACTER LIMIT
                if (shaky) {
                        blockSound.stop();
                        blockSound.play();
                        shake(window);
                        ::shaky = false;
                }


                window.display();

        }




        return 0;
}