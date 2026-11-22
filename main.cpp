// Fantasy Pet Game
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>

using namespace std;

bool loadGameFont(sf::Font& font) {
    if (font.loadFromFile("arial.ttf")) {
        return true;
    }
#ifdef _WIN32
    return font.loadFromFile("C:/Windows/Fonts/arial.ttf");
#else
    return false;
#endif
}

// Forward declarations
class Game;
class Pet;
class Ability;
class Dragon;
class Phoenix;
class Unicorn;
class Griffin;
class Guild;
class TrainingCamp;
class Battle;
class Arena;
class Shop;
class Item;

// Constants
const int MAX_ROCKS = 7;
const int MAX_BULLETS = 5;
const int ROCK_SIZE = 30;
const int GAME_TIME_LIMIT = 15;
const int ROCK_FALL_SPEED = 7;
const int ROCK_SIDE_SPEED = 1;
const int BULLET_SPEED = -15;
const int SHIP_SPEED = 10;
const int MAX_USERS = 10; // Maximum number of users 

// User structure for leaderboard
struct User {
    string username;
    int score;
};

// Function prototypes
void saveUser (const string& username, int score);
bool loadUser (const string& username);
void displayLeaderboard(User users[], int count);
void registerUser ();
bool loginUser ();

// Leaderboard functions
void saveUser (const string& username, int score) {
    ofstream file("leaderboard.txt", ios::app);
    if (file.is_open()) {
        file << username << " " << score << endl;
        file.close();
    }
}

bool loadUser (const string& username) {
    ifstream file("leaderboard.txt");
    string user;
    int score;
    while (file >> user >> score) {
        if (user == username) {
            cout << "Welcome back, " << username << "! Your score: " << score << endl;
            return true;
        }
    }
    return false;
}

void displayLeaderboard(User users[], int count) {
    cout << "=== Leaderboard ===" << endl;
    for (int i = 0; i < count; i++) {
        cout << users[i].username << ": " << users[i].score << endl;
    }
}

// Ability Classes
class Ability
{
private:
    string name;
    string description;
    int manaCost;
    int cooldown;
    int currentCooldown;
    bool isActive;

public:
    // Constructor
    Ability(const string &n, const string &desc, int cost, int cd)
        : name(n), description(desc), manaCost(cost), cooldown(cd), currentCooldown(0), isActive(true) {}

    // Virtual destructor for proper inheritance
    virtual ~Ability() {}

    // Clone method for deep copying
    virtual Ability *clone() const
    {
        return new Ability(*this);
    }

    // Getters
    string getName() const { return name; }
    string getDescription() const { return description; }
    int getManaCost() const { return manaCost; }
    int getCooldown() const { return cooldown; }
    int getCurrentCooldown() const { return currentCooldown; }
    bool getIsActive() const { return isActive; }

    // Setters
    void setCurrentCooldown(int cd) { currentCooldown = cd; }
    void setIsActive(bool active) { isActive = active; }

    // Update cooldown
    void updateCooldown()
    {
        if (currentCooldown > 0)
        {
            currentCooldown--;
        }
    }

    // Check if ability can be used
    bool canUse() const
    {
        return isActive && currentCooldown == 0;
    }

    // Use ability (to be overridden by derived abilities)
    virtual void use(Pet *user, Pet *target);

    // Display ability info
    virtual void displayInfo() const;
};
class FireballAbility : public Ability
{
public:
    FireballAbility() : Ability("Fireball", "Launches a fireball causing significant damage", 20, 2) {}

    Ability *clone() const override
    {
        return new FireballAbility(*this);
    }

    void use(Pet *user, Pet *target) override;
};

class DragonRoarAbility : public Ability
{
public:
    DragonRoarAbility() : Ability("Dragon's Roar", "Temporarily stuns nearby enemies", 25, 4) {}

    Ability *clone() const override
    {
        return new DragonRoarAbility(*this);
    }

    void use(Pet *user, Pet *target) override;
};

class FlameHealingAbility : public Ability
{
public:
    FlameHealingAbility() : Ability("Flame Healing", "Heals the Phoenix or an ally over time", 15, 3) {}

    Ability *clone() const override
    {
        return new FlameHealingAbility(*this);
    }

    void use(Pet *user, Pet *target) override;
};

class RebirthAbility : public Ability
{
private:
    bool hasBeenUsed;

public:
    RebirthAbility() : Ability("Rebirth", "Revives the Phoenix if it dies, usable once per match", 50, 10), hasBeenUsed(false) {}

    Ability *clone() const override
    {
        RebirthAbility *cloned = new RebirthAbility(*this);
        cloned->hasBeenUsed = hasBeenUsed;
        return cloned;
    }

    void use(Pet *user, Pet *target) override;
    bool getHasBeenUsed() const { return hasBeenUsed; }
    void setHasBeenUsed(bool used) { hasBeenUsed = used; }
};

class SpeedBurstAbility : public Ability
{
public:
    SpeedBurstAbility() : Ability("Speed Burst", "Increases speed for a limited time", 10, 3) {}

    Ability *clone() const override
    {
        return new SpeedBurstAbility(*this);
    }

    void use(Pet *user, Pet *target) override;
};

class MagicShieldAbility : public Ability
{
public:
    MagicShieldAbility() : Ability("Magic Shield", "Absorbs damage for a short duration", 15, 4) {}

    Ability *clone() const override
    {
        return new MagicShieldAbility(*this);
    }

    void use(Pet *user, Pet *target) override;
};

class ClawStrikeAbility : public Ability
{
public:
    ClawStrikeAbility() : Ability("Claw Strike", "Powerful melee attack", 10, 1) {}

    Ability *clone() const override
    {
        return new ClawStrikeAbility(*this);
    }

    void use(Pet *user, Pet *target) override;
};

class DiveBombAbility : public Ability
{
public:
    DiveBombAbility() : Ability("Dive Bomb", "Aerial attack that stuns opponents", 20, 4) {}

    Ability *clone() const override
    {
        return new DiveBombAbility(*this);
    }

    void use(Pet *user, Pet *target) override;
};

class Pet
{
protected:
    string name;
    int health;
    int maxHealth;
    int attackPower;
    int defense;
    int speed;
    int level;
    int experiencePoints;
    int mana;
    int maxMana;
    Ability **abilities; // Dynamic array of abilities
    int numAbilities;
    int maxAbilities;
    bool isAlive;
    Guild *currentGuild; // Guild this pet belongs to (can be nullptr)

    // Status effects
    bool isStunned;
    int stunnedTurns;
    bool hasShield;
    int shieldTurns;
    int tempSpeedBoost;
    int speedBoostTurns;
    int tempDefenseBoost;
    int defenseBoostTurns;

    // SFML properties
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float rotation;

    // Animation properties
    int animationFrame;
    float animationTimer;
    bool isAnimating;

public:
    // Constructors and destructor
    Pet(const string &petName, int hp, int atk, int def, int spd)
        : name(petName), health(hp), maxHealth(hp), attackPower(atk),
          defense(def), speed(spd), level(1), experiencePoints(0),
          mana(100), maxMana(100), numAbilities(0), maxAbilities(4),
          isAlive(true), currentGuild(nullptr),
          isStunned(false), stunnedTurns(0),
          hasShield(false), shieldTurns(0),
          tempSpeedBoost(0), speedBoostTurns(0),
          tempDefenseBoost(0), defenseBoostTurns(0),
          position(0.f, 0.f), velocity(0.f, 0.f), rotation(0.f),
          animationFrame(0), animationTimer(0.f), isAnimating(false)
    {

        // Allocate memory for abilities array
        abilities = new Ability *[maxAbilities];
        for (int i = 0; i < maxAbilities; i++)
        {
            abilities[i] = nullptr;
        }
    }

    // Copy constructor
    Pet(const Pet &other)
        : name(other.name), health(other.health), maxHealth(other.maxHealth),
          attackPower(other.attackPower), defense(other.defense), speed(other.speed),
          level(other.level), experiencePoints(other.experiencePoints),
          mana(other.mana), maxMana(other.maxMana),
          numAbilities(other.numAbilities), maxAbilities(other.maxAbilities),
          isAlive(other.isAlive), currentGuild(other.currentGuild),
          isStunned(other.isStunned), stunnedTurns(other.stunnedTurns),
          hasShield(other.hasShield), shieldTurns(other.shieldTurns),
          tempSpeedBoost(other.tempSpeedBoost), speedBoostTurns(other.speedBoostTurns),
          tempDefenseBoost(other.tempDefenseBoost), defenseBoostTurns(other.defenseBoostTurns),
          position(other.position), velocity(other.velocity), rotation(other.rotation),
          animationFrame(other.animationFrame), animationTimer(other.animationTimer),
          isAnimating(other.isAnimating)
    {

        // Deep copy of abilities
        abilities = new Ability *[maxAbilities];
        for (int i = 0; i < numAbilities; i++)
        {
            if (other.abilities[i] != nullptr)
            {
                abilities[i] = other.abilities[i]->clone(); // Clone creates a deep copy
            }
            else
            {
                abilities[i] = nullptr;
            }
        }
        // Initialize the rest of the array
        for (int i = numAbilities; i < maxAbilities; i++)
        {
            abilities[i] = nullptr;
        }

        // Copy texture and sprite
        texture = other.texture;
        sprite = other.sprite;
    }

    // Virtual destructor for proper cleanup in derived classes
    virtual ~Pet()
    {
        // Clean up abilities array
        for (int i = 0; i < numAbilities; i++)
        {
            delete abilities[i];
        }
        delete[] abilities;
    }

    // getters
    string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttackPower() const { return attackPower; }
    int getDefense() const { return defense; }
    int getSpeed() const { return speed; }
    int getLevel() const { return level; }
    int getExperiencePoints() const { return experiencePoints; }
    int getMana() const { return mana; }
    int getMaxMana() const { return maxMana; }
    int getExperience() const { return experiencePoints; }
    int getExperienceToNextLevel() const { return level * 100 - experiencePoints; }
    bool getIsAlive() const { return isAlive; }
    Guild *getGuild() const { return currentGuild; }
    bool getIsStunned() const { return isStunned; }
    int getStunnedTurns() const { return stunnedTurns; }
    bool getHasShield() const { return hasShield; }
    int getShieldTurns() const { return shieldTurns; }
    int getEffectiveSpeed() const
    {
        return speed + (speedBoostTurns > 0 ? tempSpeedBoost : 0);
    }
    int getEffectiveDefense() const
    {
        return defense + (defenseBoostTurns > 0 ? tempDefenseBoost : 0);
    }
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getVelocity() const { return velocity; }
    float getRotation() const { return rotation; }

    // setters
    void setName(const string &newName) { name = newName; }
    void setHealth(int hp)
    {
        health = (hp > maxHealth) ? maxHealth : (hp < 0 ? 0 : hp);
        isAlive = (health > 0);
    }
    void setMaxHealth(int maxHp) { maxHealth = maxHp; }
    void setLevel(int lvl) { level = lvl; }
    void setAttackPower(int atk) { attackPower = atk; }
    void setDefense(int def) { defense = def; }
    void setSpeed(int spd) { speed = spd; }
    void setMana(int m) { mana = (m > maxMana) ? maxMana : (m < 0 ? 0 : m); }
    void setMaxMana(int mm) { maxMana = mm; }
    void setGuild(Guild *guild) { currentGuild = guild; }
    void setPosition(sf::Vector2f pos)
    {
        position = pos;
        sprite.setPosition(pos);
    }
    void setVelocity(sf::Vector2f vel) { velocity = vel; }
    void setRotation(float rot)
    {
        rotation = rot;
        sprite.setRotation(rot);
    }

    // Status effect methods
    void applyStun(int duration)
    {
        isStunned = true;
        stunnedTurns = duration;
    }

    void applyShield(int duration)
    {
        hasShield = true;
        shieldTurns = duration;
    }

    void applySpeedBoost(int amount, int duration)
    {
        tempSpeedBoost = amount;
        speedBoostTurns = duration;
    }

    void applyDefenseBoost(int amount, int duration)
    {
        tempDefenseBoost = amount;
        defenseBoostTurns = duration;
    }

    // Update status effects
    void updateStatusEffects()
    {
        if (isStunned)
        {
            stunnedTurns--;
            if (stunnedTurns <= 0)
            {
                isStunned = false;
            }
        }

        if (hasShield)
        {
            shieldTurns--;
            if (shieldTurns <= 0)
            {
                hasShield = false;
            }
        }

        if (speedBoostTurns > 0)
        {
            speedBoostTurns--;
        }

        if (defenseBoostTurns > 0)
        {
            defenseBoostTurns--;
        }

        // Update ability cooldowns
        for (int i = 0; i < numAbilities; i++)
        {
            if (abilities[i] != nullptr)
            {
                abilities[i]->updateCooldown();
            }
        }
    }

    // Pet management techniques
    void takeDamage(int damage)
    {
        // Apply defense reduction
        int actualDamage = damage - getEffectiveDefense();
        if (actualDamage < 0)
            actualDamage = 0;

        // If pet has shield, reduce damage further
        if (hasShield)
        {
            actualDamage /= 2;
        }

        setHealth(health - actualDamage);
        cout << name << " takes " << actualDamage << " damage! Health: " << health << "/" << maxHealth << endl;

        // Visual indication of damage (animate sprite)
        isAnimating = true;
        animationFrame = 0;
        animationTimer = 0.f;
    }

    void heal(int amount)
    {
        setHealth(health + amount);
        cout << name << " heals for " << amount << "! Health: " << health << "/" << maxHealth << endl;

        // Visual indication of healing (animate sprite)
        isAnimating = true;
        animationFrame = 0;
        animationTimer = 0.f;
    }

    void consumeMana(int amount)
    {
        setMana(mana - amount);
    }

    void regenerateMana(int amount)
    {
        setMana(mana + amount);
    }

    void addExperience(int exp)
    {
        experiencePoints += exp;
        cout << name << " gained " << exp << " experience points!" << endl;

        // Check if pet can level up
        if (experiencePoints >= level * 100)
        {
            levelUp();
        }
    }

    void levelUp()
    {
        level++;
        experiencePoints = 0;

        // Increase stats
        maxHealth += 10;
        health = maxHealth;
        attackPower += 5;
        defense += 3;
        speed += 2;
        maxMana += 10;
        mana = maxMana;

        cout << name << " leveled up to level " << level << "!" << endl;

        // Visual indication of level up (animate sprite)
        isAnimating = true;
        animationFrame = 0;
        animationTimer = 0.f;
    }

    void addAbility(Ability *newAbility)
    {
        if (numAbilities < maxAbilities)
        {
            abilities[numAbilities++] = newAbility;
            cout << "Added " << newAbility->getName() << " ability to " << name << endl;
        }
        else
        {
            cout << "Cannot add more abilities, maximum reached!" << endl;
        }
    }

    void removeAbility(int index)
    {
        if (index >= 0 && index < numAbilities)
        {
            delete abilities[index];

            // Shift abilities to fill the gap
            for (int i = index; i < numAbilities - 1; i++)
            {
                abilities[i] = abilities[i + 1];
            }

            abilities[numAbilities - 1] = nullptr;
            numAbilities--;
        }
    }

    Ability *getAbility(int index) const
    {
        if (index >= 0 && index < numAbilities)
        {
            return abilities[index];
        }
        return nullptr;
    }

    // Attack method
    virtual void attack(Pet *target)
    {
        if (!isAlive || isStunned)
            return;

        cout << name << " attacks " << target->getName() << "!" << endl;
        int damage = attackPower;
        target->takeDamage(damage);

        // Visual indication of attack (animate sprite)
        isAnimating = true;
        animationFrame = 0;
        animationTimer = 0.f;
    }

    // Pure virtual methods that derived classes must use
    virtual void useAbility(int abilityIndex, Pet *target) = 0;
    virtual void specialAttack(Pet *target) = 0;
    virtual string getType() const = 0;

    // Display info about the pet
    virtual void displayInfo() const
    {
        cout << "=== " << name << " (" << getType() << ") ===" << endl;
        cout << "Level: " << level << " (EXP: " << experiencePoints << ")" << endl;
        cout << "Health: " << health << "/" << maxHealth << endl;
        cout << "Mana: " << mana << "/" << maxMana << endl;
        cout << "Attack: " << attackPower << endl;
        cout << "Defense: " << defense << endl;
        cout << "Speed: " << speed << endl;

        cout << "Abilities:" << endl;
        for (int i = 0; i < numAbilities; i++)
        {
            if (abilities[i] != nullptr)
            {
                cout << "  " << i + 1 << ". " << abilities[i]->getName();
                if (abilities[i]->getCurrentCooldown() > 0)
                {
                    cout << " (Cooldown: " << abilities[i]->getCurrentCooldown() << ")";
                }
                cout << endl;
            }
        }

        // Status effects
        if (isStunned)
            cout << "Status: Stunned (" << stunnedTurns << " turns)" << endl;
        if (hasShield)
            cout << "Status: Shielded (" << shieldTurns << " turns)" << endl;
        if (speedBoostTurns > 0)
            cout << "Status: Speed Boosted (" << speedBoostTurns << " turns)" << endl;
        if (defenseBoostTurns > 0)
            cout << "Status: Defense Boosted (" << defenseBoostTurns << " turns)" << endl;
    }

    // Simple sine function replacement for animation
    float simpleSin(float x)
    {
        // Simple approximation of sine function for animation
        float x2 = x * x;
        float x3 = x2 * x;
        float x5 = x3 * x2;
        float x7 = x5 * x2;

        // Simplified Taylor series for sine
        return x - x3 / 6.0f + x5 / 120.0f - x7 / 5040.0f;
    }

    // Update method for SFML
    virtual void update(float deltaTime)
    {
        // Update position based on velocity
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        sprite.setPosition(position);

        // Update animation
        if (isAnimating)
        {
            animationTimer += deltaTime;

            // Simple animation: oscillate the sprite
            if (animationTimer < 0.5f)
            {
                // Flash the sprite by altering its color
                sf::Color color = sprite.getColor();

                if ((int)(animationTimer * 10) % 2 == 0)
                {
                    color.a = 128; // Semi-transparent
                }
                else
                {
                    color.a = 255; // Fully opaque
                }

                sprite.setColor(color);

                // move the sprite
                float offset = simpleSin(animationTimer * 30) * 5.0f;
                sprite.setPosition(position.x + offset, position.y);
            }
            else
            {
                // resetting the animation
                isAnimating = false;
                sprite.setColor(sf::Color::White);
                sprite.setPosition(position);
            }
        }

        // Update status effects
        updateStatusEffects();
    }

    // drawing method for SFML
    virtual void draw(sf::RenderWindow &window)
    {
        // draw pet sprite
        window.draw(sprite);

        // draw health bar
        sf::RectangleShape healthBarBg(sf::Vector2f(60.f, 8.f));
        healthBarBg.setFillColor(sf::Color(100, 100, 100));
        healthBarBg.setPosition(position.x - 30.f, position.y - 50.f);

        sf::RectangleShape healthBar(sf::Vector2f(60.f * ((float)health / maxHealth), 8.f));
        healthBar.setFillColor(sf::Color::Green);
        healthBar.setPosition(position.x - 30.f, position.y - 50.f);

        window.draw(healthBarBg);
        window.draw(healthBar);

        // draw mana bar
        sf::RectangleShape manaBarBg(sf::Vector2f(60.f, 5.f));
        manaBarBg.setFillColor(sf::Color(100, 100, 100));
        manaBarBg.setPosition(position.x - 30.f, position.y - 40.f);

        sf::RectangleShape manaBar(sf::Vector2f(60.f * ((float)mana / maxMana), 5.f));
        manaBar.setFillColor(sf::Color::Blue);
        manaBar.setPosition(position.x - 30.f, position.y - 40.f);

        window.draw(manaBarBg);
        window.draw(manaBar);

        // draw status
        if (isStunned)
        {
            sf::CircleShape stunIcon(5.f);
            stunIcon.setFillColor(sf::Color::Yellow);
            stunIcon.setPosition(position.x - 30.f, position.y - 60.f);
            window.draw(stunIcon);
        }

        if (hasShield)
        {
            sf::CircleShape shieldIcon(5.f);
            shieldIcon.setFillColor(sf::Color::Cyan);
            shieldIcon.setPosition(position.x - 20.f, position.y - 60.f);
            window.draw(shieldIcon);
        }
    }

    // load texture
    bool loadTexture(const string &filename)
    {
        if (texture.loadFromFile(filename))
        {
            sprite.setTexture(texture);
            sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);

            // Scale sprite to fit a standard size (120x120)
            float desiredWidth = 100.f;
            float desiredHeight = 100.f;
            float scaleX = desiredWidth / texture.getSize().x;
            float scaleY = desiredHeight / texture.getSize().y;
            sprite.setScale(scaleX, scaleY);

            return true;
        }
        return false;
    }

    // operator overloading
    Pet &operator=(const Pet &other)
    {
        if (this != &other)
        {
            for (int i = 0; i < numAbilities; i++)
            {
                delete abilities[i];
            }
            delete[] abilities;

            // Copy
            name = other.name;
            health = other.health;
            maxHealth = other.maxHealth;
            attackPower = other.attackPower;
            defense = other.defense;
            speed = other.speed;
            level = other.level;
            experiencePoints = other.experiencePoints;
            mana = other.mana;
            maxMana = other.maxMana;
            numAbilities = other.numAbilities;
            maxAbilities = other.maxAbilities;
            isAlive = other.isAlive;
            currentGuild = other.currentGuild;

            // Status
            isStunned = other.isStunned;
            stunnedTurns = other.stunnedTurns;
            hasShield = other.hasShield;
            shieldTurns = other.shieldTurns;
            tempSpeedBoost = other.tempSpeedBoost;
            speedBoostTurns = other.speedBoostTurns;
            tempDefenseBoost = other.tempDefenseBoost;
            defenseBoostTurns = other.defenseBoostTurns;

            // SFML
            position = other.position;
            velocity = other.velocity;
            rotation = other.rotation;
            texture = other.texture;
            sprite = other.sprite;

            // Animation
            animationFrame = other.animationFrame;
            animationTimer = other.animationTimer;
            isAnimating = other.isAnimating;

            // Deep copy
            abilities = new Ability *[maxAbilities];
            for (int i = 0; i < numAbilities; i++)
            {
                if (other.abilities[i] != nullptr)
                {
                    abilities[i] = other.abilities[i]->clone();
                }
                else
                {
                    abilities[i] = nullptr;
                }
            }
            // Initialize rest of array
            for (int i = numAbilities; i < maxAbilities; i++)
            {
                abilities[i] = nullptr;
            }
        }
        return *this;
    }

    bool operator==(const Pet &other) const
    {
        // operator overloading for comparison
        return (name == other.name &&
                health == other.health &&
                attackPower == other.attackPower &&
                defense == other.defense &&
                speed == other.speed &&
                level == other.level);
    }

    // Friend function for output
    friend ostream &operator<<(ostream &os, const Pet &pet)
    {
        os << pet.name << " (" << pet.getType() << ") - Level " << pet.level
           << " - HP: " << pet.health << "/" << pet.maxHealth;
        return os;
    }
};

// specific/derived pet classes
class Dragon : public Pet
{
public:
    Dragon(const string &name) : Pet(name, 120, 15, 10, 8)
    {
        // dragon special abilities
        addAbility(new FireballAbility());
        addAbility(new DragonRoarAbility());
    }

    string getType() const override { return "Dragon"; }

    void specialAttack(Pet *target) override
    {
        if (!getIsAlive() || getIsStunned())
            return;

        cout << getName() << " breathes fire on " << target->getName() << endl;
        int damage = getAttackPower() * 2;
        target->takeDamage(damage);

        // 30% chance to burn (reduce defense)
        if (rand() % 100 < 30)
        {
            target->applyDefenseBoost(-5, 3);
            cout << target->getName() << "'s defense was harmed by the fire" << endl;
        }
    }

    void useAbility(int abilityIndex, Pet *target) override
    {
        Ability *ability = getAbility(abilityIndex);
        if (ability == nullptr || !ability->canUse() || getMana() < ability->getManaCost())
        {
            cout << "Cannot use this ability :)" << endl;
            return;
        }

        consumeMana(ability->getManaCost());
        ability->use(this, target);
        ability->setCurrentCooldown(ability->getCooldown());
    }
};

// Exception Class
class GameException
{
protected:
    string message;

public:
    GameException(const string &msg) : message(msg) {}
    string getMessage() const { return message; }
};

class FileException : public GameException
{
public:
    FileException(const string &msg) : GameException("File Error: " + msg) {}
};

class InputException : public GameException
{
public:
    InputException(const string &msg) : GameException("Input Error: " + msg) {}
};

// Item Classes
class Item
{
private:
    string name;
    string description;
    int price;

public:
    Item(const string &n, const string &desc, int p)
        : name(n), description(desc), price(p) {}

    virtual ~Item() {}

    // deep copying using clone function
    virtual Item *clone() const
    {
        return new Item(*this);
    }

    // Getters
    string getName() const { return name; }
    string getDescription() const { return description; }
    int getPrice() const { return price; }

    virtual void use(Pet *target)
    {
        cout << "Using " << name << " on " << target->getName() << endl;
    }

    // Display items desc
    virtual void displayInfo() const
    {
        cout << "Item: " << name << " - " << description << " (Price: " << price << ")" << endl;
    }
};

// Derived Item Classes
class HealingPotion : public Item
{
private:
    int healAmount;

public:
    HealingPotion(int amount = 50)
        : Item("Healing Potion", "Restores health to a pet", 30), healAmount(amount) {}

    Item *clone() const override
    {
        return new HealingPotion(*this);
    }

    void use(Pet *target) override;
};

class ManaPotion : public Item
{
private:
    int manaAmount;

public:
    ManaPotion(int amount = 30)
        : Item("Mana Potion", "Refills ability usage for a pet", 25), manaAmount(amount) {}

    Item *clone() const override
    {
        return new ManaPotion(*this);
    }

    void use(Pet *target) override;
};

class SpeedBoost : public Item
{
private:
    int boostAmount;
    int duration;

public:
    SpeedBoost(int amount = 20, int dur = 3)
        : Item("Speed Boost", "Temporarily increases a pet's speed", 40),
          boostAmount(amount), duration(dur) {}

    Item *clone() const override
    {
        return new SpeedBoost(*this);
    }

    void use(Pet *target) override;
};

class Shield : public Item
{
private:
    int defenseBoost;
    int duration;

public:
    Shield(int boost = 15, int dur = 3)
        : Item("Shield", "Temporarily increases a pet's defense", 35),
          defenseBoost(boost), duration(dur) {}

    Item *clone() const override
    {
        return new Shield(*this);
    }

    void use(Pet *target) override;
};

class Phoenix : public Pet
{
private:
    bool hasRebirth;

public:
    Phoenix(const string &name) : Pet(name, 100, 12, 8, 10), hasRebirth(false)
    {
        // Add phoenix-specific abilities
        addAbility(new FlameHealingAbility());
        addAbility(new RebirthAbility());
    }

    string getType() const override { return "Phoenix"; }

    bool setIsAlive(bool status)
    {
        this->isAlive = status;
    }
    void specialAttack(Pet *target) override
    {
        if (!getIsAlive() || getIsStunned())
            return;

        cout << getName() << " burns " << target->getName() << " with flames" << endl;
        int damage = getAttackPower() * 1.5;
        target->takeDamage(damage);

        // Heal self for half damage dealt
        int healAmount = damage / 2;
        heal(healAmount);
    }

    void useAbility(int abilityIndex, Pet *target) override
    {
        Ability *ability = getAbility(abilityIndex);
        if (ability == nullptr || !ability->canUse() || getMana() < ability->getManaCost())
        {
            cout << "Cannot use this ability :)" << endl;
            return;
        }

        // Special handling for Rebirth ability
        RebirthAbility *rebirth = dynamic_cast<RebirthAbility *>(ability);
        if (rebirth != nullptr && rebirth->getHasBeenUsed())
        {
            cout << "Rebirth has already been used" << endl;
            return;
        }

        consumeMana(ability->getManaCost());
        ability->use(this, target);
        ability->setCurrentCooldown(ability->getCooldown());

        if (rebirth != nullptr)
        {
            rebirth->setHasBeenUsed(true);
        }
    }

    void takeDamage(int damage)
    {
        Pet::takeDamage(damage);

        // Check for rebirth
        if (!getIsAlive() && !hasRebirth)
        {
            RebirthAbility *rebirth = dynamic_cast<RebirthAbility *>(getAbility(1));
            if (rebirth != nullptr && rebirth->getHasBeenUsed())
            {
                // Phoenix revives with half health
                setHealth(getMaxHealth() / 2);
                setIsAlive(true);
                hasRebirth = true;
                cout << getName() << " is reborn" << endl;
            }
        }
    }
};

class Unicorn : public Pet
{
public:
    Unicorn(const string &name) : Pet(name, 90, 10, 12, 12)
    {
        // unicorn special abilities
        addAbility(new MagicShieldAbility());
        addAbility(new SpeedBurstAbility());
    }

    string getType() const override { return "Unicorn"; }

    void specialAttack(Pet *target) override
    {
        if (!getIsAlive() || getIsStunned())
            return;

        cout << getName() << " attacks " << target->getName() << " using horn!" << endl;
        int damage = getAttackPower() * 1.8;
        target->takeDamage(damage);

        // 20% chance to stun
        if (rand() % 100 < 20)
        {
            target->applyStun(1);
            cout << target->getName() << " is stunned" << endl;
        }
    }

    void useAbility(int abilityIndex, Pet *target) override
    {
        Ability *ability = getAbility(abilityIndex);
        if (ability == nullptr || !ability->canUse() || getMana() < ability->getManaCost())
        {
            cout << "Cannot use this ability :)" << endl;
            return;
        }

        consumeMana(ability->getManaCost());
        ability->use(this, target);
        ability->setCurrentCooldown(ability->getCooldown());
    }
};

class Griffin : public Pet
{
public:
    Griffin(const string &name) : Pet(name, 110, 14, 9, 9)
    {
        // griffin special abilities
        addAbility(new ClawStrikeAbility());
        addAbility(new DiveBombAbility());
    }

    string getType() const override { return "Griffin"; }

    void specialAttack(Pet *target) override
    {
        if (!getIsAlive() || getIsStunned())
            return;

        cout << getName() << " attacks " << target->getName() << endl;
        int damage = getAttackPower() * 1.6;
        target->takeDamage(damage);

        // 40% chance to attack again
        if (rand() % 100 < 40)
        {
            cout << getName() << " attacks again" << endl;
            target->takeDamage(damage / 2);
        }
    }

    void useAbility(int abilityIndex, Pet *target) override
    {
        Ability *ability = getAbility(abilityIndex);
        if (ability == nullptr || !ability->canUse() || getMana() < ability->getManaCost())
        {
            cout << "Cannot use this ability :)" << endl;
            return;
        }

        consumeMana(ability->getManaCost());
        ability->use(this, target);
        ability->setCurrentCooldown(ability->getCooldown());
    }
};

// Guild Class
class Guild
{
private:
    string name;
    int level;
    int experience;
    Pet **members;
    int numMembers;
    int maxMembers;

public:
    Guild(const string &guildName)
        : name(guildName), level(1), experience(0), numMembers(0), maxMembers(10)
    {
        members = new Pet *[maxMembers];
        for (int i = 0; i < maxMembers; i++)
        {
            members[i] = nullptr;
        }
    }

    ~Guild()
    {
        delete[] members;
    }

    string getName() const { return name; }
    int getGuildLevel() const { return level; }
    int getNumMembers() const { return numMembers; }

    void addMember(Pet *pet)
    {
        if (numMembers < maxMembers)
        {
            members[numMembers++] = pet;
            pet->setGuild(this);
            cout << pet->getName() << " has joined the " << name << " guild!" << endl;
        }
        else
        {
            cout << "Guild is full!" << endl;
        }
    }

    void removeMember(Pet *pet)
    {
        for (int i = 0; i < numMembers; i++)
        {
            if (members[i] == pet)
            {
                // Shift remaining members
                for (int j = i; j < numMembers - 1; j++)
                {
                    members[j] = members[j + 1];
                }

                members[numMembers - 1] = nullptr;
                numMembers--;
                pet->setGuild(nullptr);
                cout << pet->getName() << " has left the " << name << " guild." << endl;
                return;
            }
        }
    }

    Pet *getMember(int index) const
    {
        if (index >= 0 && index < numMembers)
        {
            return members[index];
        }
        return nullptr;
    }

    void addExperience(int exp)
    {
        experience += exp;

        // Check for level up
        if (experience >= level * 500)
        {
            level++;
            experience = 0;
            cout << name << " guild is now: Level " << level << endl;
        }
    }

    void displayInfo() const
    {
        cout << "/|____ " << name << " Guild: Level " << level << " ____|" << endl;
        cout << "Experience: " << experience << "/" << level * 500 << endl;
        cout << "Members (" << numMembers << "/" << maxMembers << "):" << endl;

        for (int i = 0; i < numMembers; i++)
        {
            if (members[i] != nullptr)
            {
                cout << "  " << i + 1 << ". " << members[i]->getName()
                     << " (" << members[i]->getType() << ") - Level "
                     << members[i]->getLevel() << endl;
            }
        }
    }
};

// TrainingCamp Class
class TrainingCamp
{
public:
    TrainingCamp() {}

    void trainPet(Pet *pet, int trainingType)
    {
        switch (trainingType)
        {
        case 1: // Health
            pet->setMaxHealth(pet->getMaxHealth() + 10);
            pet->setHealth(pet->getMaxHealth());
            break;
        case 2: // Attack
            pet->setAttackPower(pet->getAttackPower() + 5);
            break;
        case 3: // Defense
            pet->setDefense(pet->getDefense() + 3);
            break;
        case 4: // Speed
            pet->setSpeed(pet->getSpeed() + 2);
            break;
        case 5: // Mana
            pet->setMaxMana(pet->getMaxMana() + 10);
            pet->setMana(pet->getMaxMana());
            break;
        }
    }
};

// Battle Class
class Battle
{
private:
    Pet *player;
    Pet *opponent;
    bool isPlayerTurn;
    int turnCount;
    bool battleOver;
    bool playerWon;

    // VF for battle
    sf::Texture effectTextures[5]; // For all special abs animations
    sf::Sprite effectSprites[5];

public:
    Battle(Pet *playerPet, Pet *opponentPet)
        : player(playerPet), opponent(opponentPet), turnCount(0), battleOver(false), playerWon(false)
    {
        // First turn based on speed
        isPlayerTurn = player->getEffectiveSpeed() >= opponent->getEffectiveSpeed();

        cout << "|==========================|" << endl;
        cout << "|==<---Battle Started--->==|" << endl;
        cout << "|==========================|" << endl;
        cout << player->getName() << " vs " << opponent->getName() << endl;

        if (isPlayerTurn)
        {
            cout << player->getName() << " goes first" << endl;
        }
        else
        {
            cout << opponent->getName() << " goes first" << endl;
        }

        // Load effect textures
        loadEffectTextures();
    }

    void loadEffectTextures()
    {
        // Load textures for animations and effects
        if (!effectTextures[0].loadFromFile("attack.png"))
        {
            cout << "Error loading attack effect" << endl;
        }
        effectSprites[0].setTexture(effectTextures[0]);
        // Scale effect sprite
        effectSprites[0].setScale(80.f / effectTextures[0].getSize().x, 80.f / effectTextures[0].getSize().y);

        if (!effectTextures[1].loadFromFile("healing.png"))
        {
            cout << "Error loading healing effect" << endl;
        }
        effectSprites[1].setTexture(effectTextures[1]);
        effectSprites[1].setScale(80.f / effectTextures[1].getSize().x, 80.f / effectTextures[1].getSize().y);

        if (!effectTextures[2].loadFromFile("shield.png"))
        {
            cout << "Error loading shield effect" << endl;
        }
        effectSprites[2].setTexture(effectTextures[2]);
        effectSprites[2].setScale(80.f / effectTextures[2].getSize().x, 80.f / effectTextures[2].getSize().y);

        if (!effectTextures[3].loadFromFile("stun.png"))
        {
            cout << "Error loading stun effect" << endl;
        }
        effectSprites[3].setTexture(effectTextures[3]);
        effectSprites[3].setScale(80.f / effectTextures[3].getSize().x, 80.f / effectTextures[3].getSize().y);

        if (!effectTextures[4].loadFromFile("special.png"))
        {
            cout << "Error loading special effect" << endl;
        }
        effectSprites[4].setTexture(effectTextures[4]);
        effectSprites[4].setScale(80.f / effectTextures[4].getSize().x, 80.f / effectTextures[4].getSize().y);
    }

    Pet *getPlayer() const { return player; }
    Pet *getOpponent() const { return opponent; }
    bool isBattleOver() const { return battleOver; }
    bool didPlayerWin() const { return playerWon; }
    int getTurnCount() const { return turnCount; }
    bool getIsPlayerTurn() const { return isPlayerTurn; }

    void executeTurn(int action, int abilityIndex = -1)
    {
        if (battleOver)
            return;

        turnCount++;
        cout << "Turn Number: " << turnCount << endl;

        if (isPlayerTurn)
        {
            // Player's turn
            executePlayerAction(action, abilityIndex);

            // Check if battle is over
            if (!opponent->getIsAlive())
            {
                endBattle(true);
                return;
            }
        }
        else
        {
            // AI turn
            executeOpponentAction();

            // Check if battle is over
            if (!player->getIsAlive())
            {
                endBattle(false);
                return;
            }
        }

        // Switch turns
        isPlayerTurn = !isPlayerTurn;

        // Update status effects
        player->updateStatusEffects();
        opponent->updateStatusEffects();

        // Add mana regeneration
        player->regenerateMana(5);
        opponent->regenerateMana(5);
    }

    void executePlayerAction(int action, int abilityIndex)
    {
        if (player->getIsStunned())
        {
            cout << player->getName() << " is stunned and cannot fight" << endl;
            return;
        }

        switch (action)
        {
        case 1: // Basic Attack
            player->attack(opponent);
            break;
        case 2: // Special Attack
            player->specialAttack(opponent);
            break;
        case 3: // Use Ability
            if (abilityIndex >= 0)
            {
                player->useAbility(abilityIndex, opponent);
            }
            else
            {
                cout << "No ability selected rn" << endl;
            }
            break;
        default:
            cout << "Invalid" << endl;
            break;
        }
    }

    void executeOpponentAction()
    {
        if (opponent->getIsStunned())
        {
            cout << opponent->getName() << " is stunned and cannot fiht" << endl;
            return;
        }

        // Simple AI for opponent
        int action = rand() % 3 + 1; // Random action between 1 and 3

        switch (action)
        {
        case 1: // Basic Attack
            opponent->attack(player);
            break;
        case 2: // Special Attack
            opponent->specialAttack(player);
            break;
        case 3: // Use Ability
            // Choose a random ability that can be used
            int availableAbilities[4];
            int count = 0;

            for (int i = 0; i < 4; i++)
            {
                Ability *ability = opponent->getAbility(i);
                if (ability != nullptr && ability->canUse() &&
                    opponent->getMana() >= ability->getManaCost())
                {
                    availableAbilities[count++] = i;
                }
            }

            if (count > 0)
            {
                int abilityIndex = availableAbilities[rand() % count];
                opponent->useAbility(abilityIndex, player);
            }
            else
            {
                // If no abilities available, do basic attack
                opponent->attack(player);
            }
            break;
        }
    }

    void endBattle(bool playerVictory)
    {
        battleOver = true;
        playerWon = playerVictory;

        cout << "|========================|" << endl;
        cout << "|==<---Battle Ended--->==|" << endl;
        cout << "|========================|" << endl;

        if (playerWon)
        {
            cout << player->getName() << " won the battle" << endl;

            // Award experience to player
            int expGained = opponent->getLevel() * 50;
            player->addExperience(expGained);

            // If player is in a guild, also award guild exp
            if (player->getGuild() != nullptr)
            {
                player->getGuild()->addExperience(expGained / 2);
            }
        }
        else
        {
            cout << opponent->getName() << " won the battle" << endl;
        }
    }

    void displayBattleStatus() const
    {
        cout << "|=========================|" << endl;
        cout << "|==<---Battle Status--->==|" << endl;
        cout << "|=========================|" << endl;
        cout << player->getName() << " - HP: " << player->getHealth() << "/" << player->getMaxHealth()
             << " - Mana: " << player->getMana() << "/" << player->getMaxMana() << endl;
        if (player->getIsStunned())
            cout << " Stunned " << endl;
        if (player->getHasShield())
            cout << " Shielded " << endl;

        cout << opponent->getName() << " - HP: " << opponent->getHealth() << "/" << opponent->getMaxHealth()
             << " - Mana: " << opponent->getMana() << "/" << opponent->getMaxMana() << endl;
        if (opponent->getIsStunned())
            cout << " Stunned " << endl;
        if (opponent->getHasShield())
            cout << " Shielded " << endl;

        cout << "Turn: " << (isPlayerTurn ? player->getName() : opponent->getName()) << endl;
    }

    // Play attack animation between two pets
    void playAttackAnimation(sf::RenderWindow &window, Pet *attacker, Pet *target, int animationType)
    {
        sf::Sprite &effectSprite = effectSprites[animationType];

        // Set initial position at attacker
        sf::Vector2f startPos = attacker->getPosition();
        sf::Vector2f targetPos = target->getPosition();

        // Animation parameters
        float animDuration = 0.5f; // seconds
        float elapsed = 0.0f;
        sf::Clock animClock;

        // Simple animation loop
        while (elapsed < animDuration)
        {
            float progress = elapsed / animDuration;

            // Linear interpolation between start and target positions
            sf::Vector2f currentPos;
            currentPos.x = startPos.x + (targetPos.x - startPos.x) * progress;
            currentPos.y = startPos.y + (targetPos.y - startPos.y) * progress;

            effectSprite.setPosition(currentPos);

            // Handle any window events but don't process input
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                    return;
                }
            }

            // Draw the scene with the animation
            window.clear();
            attacker->draw(window);
            target->draw(window);
            window.draw(effectSprite);
            window.display();

            // Update elapsed time
            elapsed = animClock.getElapsedTime().asSeconds();
        }
    }
};

// Arena Class
class Arena
{
private:
    Battle *currentBattle;
    sf::RenderWindow *window;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

public:
    Arena() : currentBattle(nullptr), window(nullptr)
    {
        // Load arena background
        if (!backgroundTexture.loadFromFile("arena.png"))
        {
            cout << "Error loading arena background" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);
        // Scale to window size if window is set later
    }

    ~Arena()
    {
        if (currentBattle != nullptr)
        {
            delete currentBattle;
        }
    }

    void setWindow(sf::RenderWindow *gameWindow)
    {
        window = gameWindow;
        if (window)
        {
            float scaleX = window->getSize().x / backgroundTexture.getSize().x;
            float scaleY = window->getSize().y / backgroundTexture.getSize().y;
            backgroundSprite.setScale(scaleX, scaleY);
        }
    }

    void startBattle(Pet *playerPet, Pet *opponentPet)
    {
        if (currentBattle != nullptr)
        {
            delete currentBattle;
        }

        currentBattle = new Battle(playerPet, opponentPet);

        // Position the pets for battle display
        if (window != nullptr)
        {
            // Position player pet on the left
            sf::Vector2f playerPos(window->getSize().x * 0.25f, window->getSize().y * 0.5f);
            playerPet->setPosition(playerPos);

            // Position opponent pet on the right
            sf::Vector2f opponentPos(window->getSize().x * 0.75f, window->getSize().y * 0.5f);
            opponentPet->setPosition(opponentPos);
        }
    }

    void executeTurn(int action, int abilityIndex = -1)
    {
        if (currentBattle != nullptr)
        {
            currentBattle->executeTurn(action, abilityIndex);
        }
    }

    bool isBattleActive() const
    {
        return (currentBattle != nullptr && !currentBattle->isBattleOver());
    }

    bool didPlayerWin() const
    {
        if (currentBattle != nullptr)
        {
            return currentBattle->didPlayerWin();
        }
        return false;
    }

    bool isPlayerTurn() const
    {
        if (currentBattle != nullptr)
        {
            return currentBattle->getIsPlayerTurn();
        }
        return true;
    }

    int getTurnCount() const
    {
        if (currentBattle != nullptr)
        {
            return currentBattle->getTurnCount();
        }
        return 0;
    }

    Pet *getOpponent() const
    {
        if (currentBattle != nullptr)
        {
            return currentBattle->getOpponent();
        }
        return nullptr;
    }

    void displayBattleStatus() const
    {
        if (currentBattle != nullptr)
        {
            currentBattle->displayBattleStatus();
        }
    }

    void renderBattle()
    {
        if (window == nullptr || currentBattle == nullptr)
            return;

        // Draw the background
        window->draw(backgroundSprite);

        // Draw the pets
        if (currentBattle->getPlayer() != nullptr)
        {
            currentBattle->getPlayer()->draw(*window);
        }

        if (currentBattle->getOpponent() != nullptr)
        {
            currentBattle->getOpponent()->draw(*window);
        }

        // Draw turn indicator
        sf::Text turnText;
        sf::Font font;
        if (loadGameFont(font))
        {
            turnText.setFont(font);
            turnText.setCharacterSize(20);
            turnText.setFillColor(sf::Color::White);
            turnText.setString("Turn " + to_string(currentBattle->getTurnCount()) + ": " +
                               (currentBattle->getIsPlayerTurn() ? currentBattle->getPlayer()->getName() : currentBattle->getOpponent()->getName()));
            turnText.setPosition(window->getSize().x / 2 - turnText.getGlobalBounds().width / 2, 20);
            window->draw(turnText);
        }
    }

    void playAttackAnimation(int attackType)
    {
        if (window == nullptr || currentBattle == nullptr)
            return;

        Pet *attacker = currentBattle->getIsPlayerTurn() ? currentBattle->getPlayer() : currentBattle->getOpponent();
        Pet *target = currentBattle->getIsPlayerTurn() ? currentBattle->getOpponent() : currentBattle->getPlayer();

        currentBattle->playAttackAnimation(*window, attacker, target, attackType);
    }
};

// 11. Shop Class
class Shop
{
private:
    Item **availableItems;
    int numItems;
    int maxItems;

public:
    Shop() : numItems(0), maxItems(10)
    {
        availableItems = new Item *[maxItems];

        // Initialize shop items
        availableItems[numItems++] = new HealingPotion();
        availableItems[numItems++] = new ManaPotion();
        availableItems[numItems++] = new SpeedBoost();
        availableItems[numItems++] = new Shield();
    }

    ~Shop()
    {
        for (int i = 0; i < numItems; i++)
        {
            delete availableItems[i];
        }
        delete[] availableItems;
    }

    void displayItems() const
    {
        cout << "=== Shop Items ===" << endl;
        for (int i = 0; i < numItems; i++)
        {
            if (availableItems[i] != nullptr)
            {
                availableItems[i]->displayInfo();
            }
        }
    }

    Item *getItem(int index) const
    {
        if (index >= 0 && index < numItems)
        {
            return availableItems[index];
        }
        return nullptr;
    }
};

// 12. Game Class (Main Game Loop and SFML Implementation)
class Game
{
private:
    Pet *playerPet;
    Guild *playerGuild;
    Arena *arena;
    Shop *shop;
    TrainingCamp *trainingCamp;

    // Item inventory
    Item **inventory;
    int numItems;
    int maxItems;

    // Gold currency
    int playerGold;

    // SFML window and rendering components
    sf::RenderWindow window;
    sf::Font font;
    sf::Music backgroundMusic;

    // Menu components
    sf::RectangleShape menuBackground;
    sf::Text menuTitle;
    sf::Text menuOptions[10]; // Array to store menu options
    int currentMenuOptionCount;
    int selectedMenuOption;

    // Game state tracking
    enum GameState
    {
        WELCOME_SCREEN,
        PET_SELECTION,
        MAIN_MENU,
        BATTLE_MENU,
        BATTLE_ACTION,
        BATTLE_ABILITY,
        BATTLE_ITEM,
        TRAINING_MENU,
        SHOP_MENU,
        BUY_ITEM_MENU,
        SELL_ITEM_MENU,
        GUILD_MENU,
        PET_MANAGEMENT
    };
    GameState currentState;

    // Background textures
    sf::Texture backgroundTextures[7]; // One for each main game state
    sf::Sprite backgroundSprite;

    // Effect textures
    sf::Texture effectTextures[5]; // For attack animations, healing, etc.
    sf::Sprite effectSprites[5];
    bool effectActive;
    float effectTimer;

    // UI Elements
    sf::RectangleShape statusBar;
    sf::Text goldText;
    sf::Text levelText;
    sf::Text messageText;
    float messageTimer;
    bool showMessage;

public:
    Game() : playerPet(nullptr), playerGuild(nullptr),
             numItems(0), maxItems(20), playerGold(1000),
             currentMenuOptionCount(0), selectedMenuOption(0),
             currentState(WELCOME_SCREEN), effectActive(false), effectTimer(0.0f),
             messageTimer(0.0f), showMessage(false)
    {

        // Initialize SFML
        window.create(sf::VideoMode(800, 600), "Fantasy Pet Game", sf::Style::Titlebar | sf::Style::Close);
        window.setFramerateLimit(60);

        // Try to load font
        if (!loadGameFont(font))
        {
            cout << "Error loading font" << endl;
        }

        // Try to load music
        if (!backgroundMusic.openFromFile("bg.ogg"))
        {
            cout << "Error loading bg audio" << endl;
        }
        else
        {
            backgroundMusic.setLoop(true);
            backgroundMusic.setVolume(50.0f);
            backgroundMusic.play();
        }

        // Load textures for effects
        loadEffectTextures();

        // Load background textures
        loadBackgroundTextures();

        // Setup UI components
        setupUIComponents();

        // Seed random number generator
        srand(static_cast<unsigned int>(time(nullptr)));

        // Create game components
        arena = new Arena();
        arena->setWindow(&window);
        shop = new Shop();
        trainingCamp = new TrainingCamp();

        // Allocate inventory
        inventory = new Item *[maxItems];
        for (int i = 0; i < maxItems; i++)
        {
            inventory[i] = nullptr;
        }
    }

    ~Game()
    {
        // Clean up
        delete playerPet;
        delete playerGuild;
        delete arena;
        delete shop;
        delete trainingCamp;

        // Clean up inventory
        for (int i = 0; i < numItems; i++)
        {
            delete inventory[i];
        }
        delete[] inventory;
    }

    void loadEffectTextures()
    {
        // Load textures for animations and effects
        if (!effectTextures[0].loadFromFile("attack.png"))
        {
            cout << "Error loading attack effect" << endl;
        }
        effectSprites[0].setTexture(effectTextures[0]);

        if (!effectTextures[1].loadFromFile("healing.png"))
        {
            cout << "Error loading healing effect" << endl;
        }
        effectSprites[1].setTexture(effectTextures[1]);

        if (!effectTextures[2].loadFromFile("shield.png"))
        {
            cout << "Error loading shield effect" << endl;
        }
        effectSprites[2].setTexture(effectTextures[2]);

        if (!effectTextures[3].loadFromFile("stun.png"))
        {
            cout << "Error loading stun effect" << endl;
        }
        effectSprites[3].setTexture(effectTextures[3]);

        if (!effectTextures[4].loadFromFile("spell.png"))
        {
            cout << "Error loading spell effect" << endl;
        }
        effectSprites[4].setTexture(effectTextures[4]);
    }

    void loadBackgroundTextures()
    {
        // Load background images for different game states
        if (!backgroundTextures[0].loadFromFile("welcome.png"))
        {
            cout << "Error loading welcome background" << endl;
        }

        if (!backgroundTextures[1].loadFromFile("main_menu.png"))
        {
            cout << "Error loading main menu background" << endl;
        }

        if (!backgroundTextures[2].loadFromFile("battle.png"))
        {
            cout << "Error loading battle background" << endl;
        }

        if (!backgroundTextures[3].loadFromFile("training.png"))
        {
            cout << "Error loading training background" << endl;
        }

        if (!backgroundTextures[4].loadFromFile("shop.png"))
        {
            cout << "Error loading shop background" << endl;
        }

        if (!backgroundTextures[5].loadFromFile("guild.png"))
        {
            cout << "Error loading guild background" << endl;
        }

        if (!backgroundTextures[6].loadFromFile("pet_management.png"))
        {
            cout << "Error loading pet management background" << endl;
        }

        // Set initial background
        backgroundSprite.setTexture(backgroundTextures[0]);
        float scaleX = window.getSize().x / (float)backgroundTextures[0].getSize().x;
        float scaleY = window.getSize().y / (float)backgroundTextures[0].getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
    }

    void setupUIComponents()
    {
        // Setup menu background
        menuBackground.setSize(sf::Vector2f(600.f, 500.f));
        menuBackground.setFillColor(sf::Color(0, 0, 0, 200));   // Semi-transparent black
        menuBackground.setOutlineColor(sf::Color(255, 215, 0)); // Gold outline
        menuBackground.setOutlineThickness(3.f);
        menuBackground.setPosition(
            (window.getSize().x - menuBackground.getSize().x) / 2.f,
            (window.getSize().y - menuBackground.getSize().y) / 2.f);

        // Setup menu title
        menuTitle.setFont(font);
        menuTitle.setCharacterSize(32);
        menuTitle.setFillColor(sf::Color::White);
        menuTitle.setOutlineColor(sf::Color::Black);
        menuTitle.setOutlineThickness(2.f);
        menuTitle.setString("Fantasy Pet Game");
        menuTitle.setPosition(
            (window.getSize().x - menuTitle.getGlobalBounds().width) / 2.f,
            menuBackground.getPosition().y + 20.f);

        // Setup menu options
        for (int i = 0; i < 10; i++)
        {
            menuOptions[i].setFont(font);
            menuOptions[i].setCharacterSize(24);
            menuOptions[i].setFillColor(sf::Color::White);
            // Initially positioned off-screen, will be positioned correctly when used
            menuOptions[i].setPosition(-100.f, -100.f);
        }

        // Setup status bar
        statusBar.setSize(sf::Vector2f(window.getSize().x, 60.f));
        statusBar.setFillColor(sf::Color(0, 0, 0, 180)); // Semi-transparent black
        statusBar.setPosition(0.f, window.getSize().y - 60.f);

        goldText.setFont(font);
        goldText.setCharacterSize(18);
        goldText.setFillColor(sf::Color::Yellow);
        goldText.setPosition(20.f, window.getSize().y - 45.f);

        levelText.setFont(font);
        levelText.setCharacterSize(18);
        levelText.setFillColor(sf::Color::White);
        levelText.setPosition(200.f, window.getSize().y - 45.f);

        // Setup message text
        messageText.setFont(font);
        messageText.setCharacterSize(24);
        messageText.setFillColor(sf::Color::White);
        messageText.setOutlineColor(sf::Color::Black);
        messageText.setOutlineThickness(2.f);
        messageText.setPosition(
            (window.getSize().x - messageText.getGlobalBounds().width) / 2.f,
            window.getSize().y * 0.2f);
    }

    void updateStatusBar()
    {
        if (playerPet)
        {
            goldText.setString("Gold: " + to_string(playerGold));
            levelText.setString(playerPet->getName() + " (Level " +
                                to_string(playerPet->getLevel()) + ") - HP: " +
                                to_string(playerPet->getHealth()) + "/" +
                                to_string(playerPet->getMaxHealth()));
        }
        else
        {
            goldText.setString("Gold: " + to_string(playerGold));
            levelText.setString("No Pet Selected");
        }
    }

    void setMenuOptions(const char *options[], int count)
    {
        currentMenuOptionCount = count;
        selectedMenuOption = 0;

        for (int i = 0; i < currentMenuOptionCount; i++)
        {
            menuOptions[i].setString(options[i]);
            menuOptions[i].setPosition(
                menuBackground.getPosition().x + 50.f,
                menuBackground.getPosition().y + 100.f + i * 40.f);
        }
    }

    void updateSelectedMenuOption(int direction)
    {
        // Update selected option (wrap around)
        selectedMenuOption = (selectedMenuOption + direction + currentMenuOptionCount) % currentMenuOptionCount;

        // Update option colors
        for (int i = 0; i < currentMenuOptionCount; i++)
        {
            if (i == selectedMenuOption)
            {
                menuOptions[i].setFillColor(sf::Color::Yellow);
            }
            else
            {
                menuOptions[i].setFillColor(sf::Color::White);
            }
        }
    }

    void displayMenu(const char *title, const char *options[], int optionCount)
    {
        // Set the menu title
        menuTitle.setString(title);
        menuTitle.setPosition(
            (window.getSize().x - menuTitle.getGlobalBounds().width) / 2.f,
            menuBackground.getPosition().y + 20.f);

        // Set menu options
        setMenuOptions(options, optionCount);

        // Set first option as selected
        updateSelectedMenuOption(0);
    }

    void drawMenu()
    {
        // Draw the menu components
        window.draw(menuBackground);
        window.draw(menuTitle);

        // Draw all menu options
        for (int i = 0; i < currentMenuOptionCount; i++)
        {
            window.draw(menuOptions[i]);
        }

        // Draw status bar
        window.draw(statusBar);
        window.draw(goldText);
        window.draw(levelText);

        // Draw message if active
        if (showMessage)
        {
            window.draw(messageText);
        }
    }

    void displayMessage(const char *message, float duration = 2.0f)
    {
        messageText.setString(message);
        messageText.setPosition(
            (window.getSize().x - messageText.getGlobalBounds().width) / 2.f,
            window.getSize().y * 0.2f);
        showMessage = true;
        messageTimer = duration;
    }

    void run()
    {
        // Main game loop
        bool running = true;
        sf::Clock clock;

        // Display welcome screen
        displayWelcomeScreen();

        while (running && window.isOpen())
        {
            sf::Time deltaTime = clock.restart();

            // Process events
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                    running = false;
                }
                else if (event.type == sf::Event::KeyPressed)
                {
                    handleKeyPress(event.key.code);
                }
            }

            // Update game logic
            update(deltaTime);

            // Render
            render();
        }
    }

    void update(sf::Time deltaTime)
    {
        // Update message timer
        if (showMessage)
        {
            messageTimer -= deltaTime.asSeconds();
            if (messageTimer <= 0.0f)
            {
                showMessage = false;
            }
        }

        // Update status bar
        updateStatusBar();

        // Update pet animations
        if (playerPet != nullptr)
        {
            playerPet->update(deltaTime.asSeconds());
        }

        // Update opponent in battle
        if (currentState == BATTLE_ACTION || currentState == BATTLE_ABILITY || currentState == BATTLE_ITEM)
        {
            Pet *opponent = arena->getOpponent();
            if (opponent != nullptr)
            {
                opponent->update(deltaTime.asSeconds());
            }
        }

        // Update effect animations
        if (effectActive)
        {
            effectTimer -= deltaTime.asSeconds();
            if (effectTimer <= 0.0f)
            {
                effectActive = false;
            }
        }
    }

    void render()
    {
        // Clear the window
        window.clear();

        // Draw background
        window.draw(backgroundSprite);

        // Draw state-specific elements
        switch (currentState)
        {
        case WELCOME_SCREEN:
            // The welcome screen is handled in displayWelcomeScreen()
            break;

        case PET_SELECTION:
            drawMenu();
            break;

        case MAIN_MENU:
            drawMenu();
            break;

        case BATTLE_MENU:
            drawMenu();
            break;

        case BATTLE_ACTION:
        case BATTLE_ABILITY:
        case BATTLE_ITEM:
            arena->renderBattle();
            drawMenu();
            break;

        case TRAINING_MENU:
            drawTrainingScreen();
            break;

        case SHOP_MENU:
        case BUY_ITEM_MENU:
        case SELL_ITEM_MENU:
            drawShopScreen();
            break;

        case GUILD_MENU:
            drawGuildScreen();
            break;

        case PET_MANAGEMENT:
            drawPetManagementScreen();
            break;
        }

        // Draw active effects if any
        if (effectActive)
        {
            for (int i = 0; i < 5; i++)
            {
                if (effectSprites[i].getPosition().x > 0)
                {
                    window.draw(effectSprites[i]);
                }
            }
        }

        // Display the contents of the window
        window.display();
    }

    void displayWelcomeScreen()
    {
        // Set background for welcome screen
        backgroundSprite.setTexture(backgroundTextures[0]);
        float scaleX = window.getSize().x / backgroundTextures[0].getSize().x;
        float scaleY = window.getSize().y / backgroundTextures[0].getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
        // Create welcome text
        sf::Text welcomeText;
        welcomeText.setFont(font);
        welcomeText.setCharacterSize(40);
        welcomeText.setFillColor(sf::Color::White);
        welcomeText.setOutlineColor(sf::Color::Black);
        welcomeText.setOutlineThickness(3.f);
        welcomeText.setString("Welcome to Fantasy Pet Game!");
        welcomeText.setPosition(
            (window.getSize().x - welcomeText.getGlobalBounds().width) / 2.f,
            window.getSize().y * 0.3f);

        sf::Text pressEnterText;
        pressEnterText.setFont(font);
        pressEnterText.setCharacterSize(24);
        pressEnterText.setFillColor(sf::Color::Yellow);
        pressEnterText.setString("Press Enter to Start");
        pressEnterText.setPosition(
            (window.getSize().x - pressEnterText.getGlobalBounds().width) / 2.f,
            window.getSize().y * 0.7f);

        // Animate the "Press Enter" text
        sf::Clock animClock;
        bool showPressEnter = true;

        // Display welcome screen until Enter is pressed
        bool welcomeScreen = true;
        while (welcomeScreen && window.isOpen())
        {
            // Process events
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                    welcomeScreen = false;
                }
                else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
                {
                    welcomeScreen = false;
                }
            }

            // Animate "Press Enter" text
            if (animClock.getElapsedTime().asSeconds() > 0.5f)
            {
                showPressEnter = !showPressEnter;
                animClock.restart();
            }

            // Render welcome screen
            window.clear();
            window.draw(backgroundSprite);
            window.draw(welcomeText);

            if (showPressEnter)
            {
                window.draw(pressEnterText);
            }

            window.display();
        }

        // Move to pet selection
        createPlayerPet();
    }

    void createPlayerPet()
    {
        // Set background for pet selection
        backgroundSprite.setTexture(backgroundTextures[1]);
        float scaleX = window.getSize().x / backgroundTextures[1].getSize().x;
        float scaleY = window.getSize().y / backgroundTextures[1].getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);

        // Change game state
        currentState = PET_SELECTION;

        // Set up pet selection menu options
        const char *options[] = {
            "1. Dragon",
            "2. Phoenix",
            "3. Unicorn",
            "4. Griffin"};

        displayMenu("Choose Your Pet", options, 4);

        // Keep rendering until a pet is selected
        bool selecting = true;
        while (selecting && window.isOpen())
        {
            // Process events
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                    selecting = false;
                }
                else if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Up)
                    {
                        updateSelectedMenuOption(-1);
                    }
                    else if (event.key.code == sf::Keyboard::Down)
                    {
                        updateSelectedMenuOption(1);
                    }
                    else if (event.key.code == sf::Keyboard::Return)
                    {
                        selecting = false;
                    }
                }
            }

            // Clear window and draw menu
            window.clear();
            window.draw(backgroundSprite);
            drawMenu();
            window.display();
        }

        // Get pet name using text input
        string petName = getTextInput("Enter your pet's name:");

        // Create the selected pet
        switch (selectedMenuOption)
        {
        case 0: // Dragon
            playerPet = new Dragon(petName);
            break;
        case 1: // Phoenix
            playerPet = new Phoenix(petName);
            break;
        case 2: // Unicorn
            playerPet = new Unicorn(petName);
            break;
        case 3: // Griffin
            playerPet = new Griffin(petName);
            break;
        }

        // Load texture for the pet
        switch (selectedMenuOption)
        {
        case 0: // Dragon
            playerPet->loadTexture("dragon.png");
            break;
        case 1: // Phoenix
            playerPet->loadTexture("phoenix.png");
            break;
        case 2: // Unicorn
            playerPet->loadTexture("unicorn.png");
            break;
        case 3: // Griffin
            playerPet->loadTexture("griffin.png");
            break;
        }

        // Set initial position for the pet
        playerPet->setPosition(sf::Vector2f(window.getSize().x / 2.f, window.getSize().y / 2.f));

        // Show pet creation success message
        char message[100];
        sprintf(message, "%s the %s has joined your team!",
                petName.c_str(), playerPet->getType().c_str());
        displayMessage(message);

        // Continue to main menu
        currentState = MAIN_MENU;
        displayMainMenu();
    }

    string getTextInput(const char *prompt)
    {
        // Setup text input screen
        sf::Text promptText;
        promptText.setFont(font);
        promptText.setCharacterSize(24);
        promptText.setFillColor(sf::Color::White);
        promptText.setString(prompt);
        promptText.setPosition(
            (window.getSize().x - promptText.getGlobalBounds().width) / 2.f,
            window.getSize().y * 0.4f);

        sf::RectangleShape inputBox;
        inputBox.setSize(sf::Vector2f(400.f, 40.f));
        inputBox.setFillColor(sf::Color(50, 50, 50, 200));
        inputBox.setOutlineColor(sf::Color::White);
        inputBox.setOutlineThickness(2.f);
        inputBox.setPosition(
            (window.getSize().x - inputBox.getSize().x) / 2.f,
            window.getSize().y * 0.5f);

        sf::Text inputText;
        inputText.setFont(font);
        inputText.setCharacterSize(20);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition(
            inputBox.getPosition().x + 10.f,
            inputBox.getPosition().y + 8.f);

        // Input handling
        string input;
        bool inputting = true;

        while (inputting && window.isOpen())
        {
            // Process events
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Resized)
                {
                    float scaleX = event.size.width / (float)backgroundSprite.getTexture()->getSize().x;
                    float scaleY = event.size.height / (float)backgroundSprite.getTexture()->getSize().y;
                    backgroundSprite.setScale(scaleX, scaleY);
                }
                else if (event.type == sf::Event::Closed)
                {
                    window.close();
                    inputting = false;
                }
                else if (event.type == sf::Event::TextEntered)
                {
                    // Handle backspace
                    if (event.text.unicode == 8 && !input.empty())
                    {
                        input.pop_back();
                    }
                    // Handle regular characters
                    else if (event.text.unicode >= 32 && event.text.unicode < 128)
                    {
                        input += static_cast<char>(event.text.unicode);
                    }
                }
                else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
                {
                    if (!input.empty())
                    {
                        inputting = false;
                    }
                }
            }

            // Update input text
            inputText.setString(input + "_");

            // Render
            window.clear();
            window.draw(backgroundSprite);
            window.draw(promptText);
            window.draw(inputBox);
            window.draw(inputText);
            window.display();
        }

        return input;
    }

    void handleKeyPress(sf::Keyboard::Key key)
    {
        switch (currentState)
        {
        case MAIN_MENU:
            handleMainMenuKeyPress(key);
            break;

        case BATTLE_MENU:
            handleBattleMenuKeyPress(key);
            break;

        case BATTLE_ACTION:
            handleBattleActionKeyPress(key);
            break;

        case BATTLE_ABILITY:
            handleBattleAbilityKeyPress(key);
            break;

        case BATTLE_ITEM:
            handleBattleItemKeyPress(key);
            break;

        case TRAINING_MENU:
            handleTrainingMenuKeyPress(key);
            break;

        case SHOP_MENU:
            handleShopMenuKeyPress(key);
            break;

        case BUY_ITEM_MENU:
            handleBuyItemMenuKeyPress(key);
            break;

        case SELL_ITEM_MENU:
            handleSellItemMenuKeyPress(key);
            break;

        case GUILD_MENU:
            handleGuildMenuKeyPress(key);
            break;

        case PET_MANAGEMENT:
            handlePetManagementKeyPress(key);
            break;

        default:
            break;
        }
    }

    void handleMainMenuKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1); // Move up in menu
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1); // Move down in menu
        }
        else if (key == sf::Keyboard::Return)
        {
            // Execute selected menu option
            switch (selectedMenuOption)
            {
            case 0: // Battle
				currentState = BATTLE_MENU;
                displayBattleMenu();
                break;
            case 1: // Train
                currentState = TRAINING_MENU;
                displayTrainingMenu();
                break;
            case 2: // Shop
                currentState = SHOP_MENU;
                displayShopMenu();
                break;
            case 3: // Guild
                currentState = GUILD_MENU;
                displayGuildMenu();
                break;
            case 4: // Manage Pet
                currentState = PET_MANAGEMENT;
                displayPetManagementMenu();
                break;
            case 5: // Arena Practice
			{
			class Rock {
		public:
    	sf::Sprite rockSprite;
    	sf::Vector2f speed;
    	bool isDestroyed;

    	Rock() : isDestroyed(true) {}

    	void setup(sf::Texture& texture, float startX) {
        rockSprite.setTexture(texture);
        rockSprite.setPosition(startX, -ROCK_SIZE);
        
        float scaleX = ROCK_SIZE / float(texture.getSize().x);
        float scaleY = ROCK_SIZE / float(texture.getSize().y);
        rockSprite.setScale(scaleX, scaleY);
        
        int horizontalDirection = (rand() % 3) - 1;
        speed = sf::Vector2f(horizontalDirection * ROCK_SIDE_SPEED, ROCK_FALL_SPEED);
        
        isDestroyed = false;
    	}

    	void move(const sf::Vector2u& windowSize) {
        if (!isDestroyed) {
            rockSprite.move(speed);
            sf::FloatRect bounds = rockSprite.getGlobalBounds();
            if (bounds.left <= 0) {
                speed.x = abs(speed.x);
            } else if (bounds.left + bounds.width >= windowSize.x) {
                speed.x = -abs(speed.x);
            }
        }
    	}

    	bool checkCollision(const sf::FloatRect& bulletBounds) {
        if (!isDestroyed && rockSprite.getGlobalBounds().intersects(bulletBounds)) {
            isDestroyed = true;
            return true;
        }
        return false;
    	}

    	void draw(sf::RenderWindow& window) {
        if (!isDestroyed) {
            window.draw(rockSprite);
        }
    }
};

// Bullet class
		class Bullet {
		public:
    	sf::Sprite bulletSprite;
    	bool isActive;

    	Bullet() : isActive(false) {}

   	 	void fire(const sf::Texture& texture, float posX, float posY) {
        bulletSprite.setTexture(texture);
        bulletSprite.setScale(0.1f, 0.1f);
        bulletSprite.setPosition(posX, posY);
        isActive = true;
    	}

    	void move(const sf::Vector2f& speed) {
        if (isActive) {
            bulletSprite.move(speed);
            if (bulletSprite.getPosition().y < 0) {
                isActive = false;
            }
        }
    	}

    	void draw(sf::RenderWindow& window) {
        if (isActive) {
            window.draw(bulletSprite);
        }
    }
};

		// Falling Rocks Game Class
		class FallingRocksGame {
		private:
    	sf::RenderWindow window;
    	sf::Texture shipTexture;
    	sf::Texture rockTexture;
    	sf::Texture bulletTexture;  
    	sf::Sprite shipSprite;
    	sf::Vector2f bulletSpeed;
    	int score;
    	float gameTime;
    	sf::Clock gameClock;
    	float rockSpawnTimer;
    	Rock fallingRocks[MAX_ROCKS];
    	Bullet bullets[MAX_BULLETS];
    	float bulletCooldown;
    	float bulletCooldownTime;
    	sf::Font font;
    	bool fontLoaded;
    	bool isPaused;

		public:
    	FallingRocksGame() : 
        window(sf::VideoMode(800, 600), "Falling Rocks Battle"), 
        score(0), 
        gameTime(0.0f), 
        rockSpawnTimer(0.0f),
        bulletCooldown(0.0f),
        bulletCooldownTime(0.2f),
        fontLoaded(false),
        isPaused(false) {
        
        // Load textures
        if (!shipTexture.loadFromFile("dragon.png")) {
            cerr << "Error loading pet" << endl;
            exit(1);
        }
        
        if (!rockTexture.loadFromFile("rock.png")) {
            cerr << "Error loading rocks" << endl;
            exit(1);
        }
        
        if (!bulletTexture.loadFromFile("attack.png")) {
            cerr << "Error loading attack" << endl;
            bulletTexture = shipTexture;  
        }
        
        if (!loadGameFont(font)) {
            cerr << "Could not load font!" << endl;
            fontLoaded = false;
        } else {
            fontLoaded = true;
        }

        // Initialize ship sprite
        shipSprite.setTexture(shipTexture);
        float shipWidth = shipTexture.getSize().x;
        float shipHeight = shipTexture.getSize().y;
        
        if (shipWidth > 50 || shipHeight > 50) {
            float scaleFactor = min(50.0f / shipWidth, 50.0f / shipHeight);
            shipSprite.setScale(scaleFactor, scaleFactor);
        }
        
        shipSprite.setPosition(window.getSize().x / 2 - shipSprite.getGlobalBounds().width / 2, 
                                window.getSize().y - shipSprite.getGlobalBounds().height - 10);
        
        bulletSpeed = sf::Vector2f(0, BULLET_SPEED);
        srand(static_cast<unsigned int>(time(nullptr)));
    	}

    	void spawnRock() {
        for (int i = 0; i < MAX_ROCKS; i++) {
            if (fallingRocks[i].isDestroyed || (fallingRocks[i].rockSprite.getTexture() == nullptr)) {
                float spawnX = rand() % (window.getSize().x - ROCK_SIZE);
                fallingRocks[i].setup(rockTexture, spawnX);
                fallingRocks[i].isDestroyed = false; 
                return;
            }
        }
    	}

    	void fireBullet() {
        if (isPaused) {
            return;
        }
        
        if (bulletCooldown > 0) {
            return;
        }
        
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].isActive) {
                float shipCenterX = shipSprite.getPosition().x + shipSprite.getGlobalBounds().width / 2;
                bullets[i].fire(bulletTexture, shipCenterX - bullets[i].bulletSprite.getGlobalBounds().width / 2, shipSprite.getPosition().y);
                bulletCooldown = bulletCooldownTime;
                return;
            }
        }
    	}

    	void update() {
        if (isPaused) {
            return;
        }
        
        float deltaTime = gameClock.restart().asSeconds();
        rockSpawnTimer += deltaTime;
        gameTime += deltaTime;
        
        if (bulletCooldown > 0) {
            bulletCooldown -= deltaTime;
        }
        
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].isActive) {
                bullets[i].move(bulletSpeed);
            }
        }

        for (int i = 0; i < MAX_ROCKS; ++i) {
            if (!fallingRocks[i].isDestroyed) {
                for (int j = 0; j < MAX_BULLETS; j++) {
                    if (bullets[j].isActive) {
                        if (fallingRocks[i].checkCollision(bullets[j].bulletSprite.getGlobalBounds())) {
                            score++;
                            bullets[j].isActive = false;
                            break;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < MAX_ROCKS; ++i) {
            if (!fallingRocks[i].isDestroyed) {
                fallingRocks[i].move(window.getSize());
            }
        }

        for (int i = 0; i < MAX_ROCKS; ++i) {
            if (!fallingRocks[i].isDestroyed && fallingRocks[i].rockSprite.getPosition().y > window.getSize().y) {
                fallingRocks[i].isDestroyed = true;
                spawnRock();
            }
        }
        
        if (gameTime >= GAME_TIME_LIMIT) {
            cout << "Game Over! Final Score: " << score << endl;
            window.close();
        }

        if (rockSpawnTimer >= 2.0f) {
            spawnRock();
            rockSpawnTimer = 0.0f;
        }
    	}
	
    	void draw() {
        window.clear(sf::Color(20, 20, 40));

        for (int i = 0; i < MAX_ROCKS; ++i) {
            fallingRocks[i].draw(window);
        }

        window.draw(shipSprite);

        for (int i = 0; i < MAX_BULLETS; i++) {
            bullets[i].draw(window);
        }

        if (fontLoaded) {
            sf::Text scoreText;
            scoreText.setFont(font);
            scoreText.setString("Score: " + to_string(score));
            scoreText.setCharacterSize(24);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(10, 10);
            window.draw(scoreText);

            sf::Text timeText;
            timeText.setFont(font);
            int timeRemaining = max(0, GAME_TIME_LIMIT - static_cast<int>(gameTime));
            timeText.setString("Time: " + to_string(timeRemaining) + "s");
            timeText.setCharacterSize(24);
            timeText.setFillColor(sf::Color::White);
            timeText.setPosition(window.getSize().x - 150, 10);
            window.draw(timeText);
        }

        window.display();
    	}

    	void togglePause() {
        isPaused = !isPaused;
        if (isPaused) {
            cout << "Game paused!" << endl;
        } else {
            cout << "Game resumed!" << endl;
            gameClock.restart();
        }
    	}

    	void handleMouseClick(const sf::Vector2i& clickPosition) {
        fireBullet();
    	}

    	void handleKeyboardMovement() {
        if (isPaused) {
            return;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            if (shipSprite.getPosition().x > 0) {
                shipSprite.move(-SHIP_SPEED, 0);
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if (shipSprite.getPosition().x < window.getSize().x - shipSprite.getGlobalBounds().width) {
                shipSprite.move(SHIP_SPEED, 0);
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            if (shipSprite.getPosition().y > window.getSize().y / 2) {
                shipSprite.move(0, -SHIP_SPEED);
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            if (shipSprite.getPosition().y < window.getSize().y - shipSprite.getGlobalBounds().height) {
                shipSprite.move(0, SHIP_SPEED);
            }
        }
    	}

    	void acrun() {
        gameClock.restart();
        
        for (int i = 0; i < 3; i++) {
            spawnRock();
        }
        
        cout << "Game starting!" << endl;
        cout << "Press P to pause/resume the game" << endl;

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                    togglePause();
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    handleMouseClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
                
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                    handleMouseClick(sf::Vector2i(0, 0)); 
                }
            }

            handleKeyboardMovement();
            update();
            draw();
            sf::sleep(sf::milliseconds(16));
        }
    }
};
				FallingRocksGame acgame;
    			acgame.acrun();    
				break;}
			case 6: // Quit
				window.close();
                break;
            }
        }
    }

    void handleBattleMenuKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1);
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1);
        }
        else if (key == sf::Keyboard::Return)
        {
            processBattleMenuSelection();
        }
        else if (key == sf::Keyboard::Escape)
        {
            currentState = MAIN_MENU;
            displayMainMenu();
        }
    }

    void handleBattleActionKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1);
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1);
        }
        else if (key == sf::Keyboard::Return)
        {
            processBattleActionSelection();
        }
        else if (key == sf::Keyboard::Escape)
        {
            // Can't escape during battle, show message
            displayMessage("You cannot run from this battle!");
        }
    }

    void handleBattleAbilityKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1);
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1);
        }
        else if (key == sf::Keyboard::Return)
        {
            processBattleAbilitySelection();
        }
        else if (key == sf::Keyboard::Escape)
        {
            // Return to battle action menu
            currentState = BATTLE_ACTION;
            displayBattleActionMenu();
        }
    }

    void handleBattleItemKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1);
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1);
        }
        else if (key == sf::Keyboard::Return)
        {
            processBattleItemSelection();
        }
        else if (key == sf::Keyboard::Escape)
        {
            // Return to battle action menu
            currentState = BATTLE_ACTION;
            displayBattleActionMenu();
        }
    }

    void handleTrainingMenuKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1);
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1);
        }
        else if (key == sf::Keyboard::Return)
        {
            processTrainingMenuSelection();
        }
        else if (key == sf::Keyboard::Escape)
        {
            currentState = MAIN_MENU;
            displayMainMenu();
        }
    }

    void handleShopMenuKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1);
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1);
        }
        else if (key == sf::Keyboard::Return)
        {
            processShopMenuSelection();
        }
        else if (key == sf::Keyboard::Escape)
        {
            currentState = MAIN_MENU;
            displayMainMenu();
        }
    }

    void handleBuyItemMenuKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1);
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1);
        }
        else if (key == sf::Keyboard::Return)
        {
            processBuyItemSelection();
        }
        else if (key == sf::Keyboard::Escape)
        {
            currentState = SHOP_MENU;
            displayShopMenu();
        }
    }

    void handleSellItemMenuKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1);
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1);
        }
        else if (key == sf::Keyboard::Return)
        {
            processSellItemSelection();
        }
        else if (key == sf::Keyboard::Escape)
        {
            currentState = SHOP_MENU;
            displayShopMenu();
        }
    }

    void handleGuildMenuKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1);
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1);
        }
        else if (key == sf::Keyboard::Return)
        {
            processGuildMenuSelection();
        }
        else if (key == sf::Keyboard::Escape)
        {
            currentState = MAIN_MENU;
            displayMainMenu();
        }
    }

    void handlePetManagementKeyPress(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Up)
        {
            updateSelectedMenuOption(-1);
        }
        else if (key == sf::Keyboard::Down)
        {
            updateSelectedMenuOption(1);
        }
        else if (key == sf::Keyboard::Return)
        {
            processPetManagementMenuSelection();
        }
        else if (key == sf::Keyboard::Escape)
        {
            currentState = MAIN_MENU;
            displayMainMenu();
        }
    }

    void displayMainMenu()
    {
        // Set background texture for main menu
        backgroundSprite.setTexture(backgroundTextures[1]);
        float scaleX = window.getSize().x / backgroundTextures[1].getSize().x;
        float scaleY = window.getSize().y / backgroundTextures[1].getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);

        // Set up menu options
        const char *options[] = {
            "1. Battle",
            "2. Train",
            "3. Shop",
            "4. Guild",
            "5. Manage Pet",
            "6. Practice Arena",
            "7. Quit"};

        displayMenu("Main Menu", options, 7);

        // Update status bar
        updateStatusBar();
    }

    void displayBattleMenu()
    {
        // Set background texture for battle menu
        backgroundSprite.setTexture(backgroundTextures[2]);
        float scaleX = window.getSize().x / backgroundTextures[2].getSize().x;
        float scaleY = window.getSize().y / backgroundTextures[2].getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
        // Set up battle menu options
        const char *options[] = {
            "1. Easy Opponent",
            "2. Medium Opponent",
            "3. Hard Opponent",
            "4. Return to Main Menu"};

        displayMenu("Battle Menu", options, 4);
    }

    void processBattleMenuSelection()
    {
        if (selectedMenuOption == 3)
        { // Return to Main Menu
            currentState = MAIN_MENU;
            displayMainMenu();
            return;
        }

        // Create opponent based on difficulty
        Pet *opponent = nullptr;
        const char *names[] = {"W", "X", "Y", "Z"};
        string name = names[rand() % 4];

        switch (selectedMenuOption)
        {
        case 0: // Easy
            switch (rand() % 4)
            {
            case 0:
                opponent = new Dragon(name);
                break;
            case 1:
                opponent = new Phoenix(name);
                break;
            case 2:
                opponent = new Unicorn(name);
                break;
            case 3:
                opponent = new Griffin(name);
                break;
            }
            break;

        case 1: // Medium
            switch (rand() % 4)
            {
            case 0:
                opponent = new Dragon(name);
                break;
            case 1:
                opponent = new Phoenix(name);
                break;
            case 2:
                opponent = new Unicorn(name);
                break;
            case 3:
                opponent = new Griffin(name);
                break;
            }
            // Make the opponent stronger
            opponent->setLevel(playerPet->getLevel() + 1);
            opponent->setMaxHealth(opponent->getMaxHealth() + 20);
            opponent->setHealth(opponent->getMaxHealth());
            opponent->setAttackPower(opponent->getAttackPower() + 5);
            break;

        case 2: // Hard
            switch (rand() % 4)
            {
            case 0:
                opponent = new Dragon(name);
                break;
            case 1:
                opponent = new Phoenix(name);
                break;
            case 2:
                opponent = new Unicorn(name);
                break;
            case 3:
                opponent = new Griffin(name);
                break;
            }
            // Make the opponent much stronger
            opponent->setLevel(playerPet->getLevel() + 2);
            opponent->setMaxHealth(opponent->getMaxHealth() + 40);
            opponent->setHealth(opponent->getMaxHealth());
            opponent->setAttackPower(opponent->getAttackPower() + 10);
            opponent->setDefense(opponent->getDefense() + 5);
            break;
        }

        // Load appropriate texture for opponent
        switch (rand() % 4)
        {
        case 0:
            opponent->loadTexture("dragon.png");
            break;
        case 1:
            opponent->loadTexture("phoenix.png");
            break;
        case 2:
            opponent->loadTexture("unicorn.png");
            break;
        case 3:
            opponent->loadTexture("griffin.png");
            break;
        }

        // Start battle
        arena->startBattle(playerPet, opponent);

        // Change state and display battle action menu
        currentState = BATTLE_ACTION;
        displayBattleActionMenu();
    }

    void displayBattleActionMenu()
    {
        // Set up battle action menu options
        const char *options[] = {
            "1. Basic Attack",
            "2. Special Attack",
            "3. Use Ability",
            "4. Use Item"};

        displayMenu("Choose Your Action", options, 4);
    }

    void processBattleActionSelection()
    {
        if (selectedMenuOption == 2)
        { // Use Ability
            // Change to ability selection menu
            currentState = BATTLE_ABILITY;
            displayAbilitySelectionMenu();
            return;
        }
        else if (selectedMenuOption == 3)
        { // Use Item
            // Change to item selection menu
            currentState = BATTLE_ITEM;
            displayItemSelectionMenu();
            return;
        }

        // Execute battle turn with basic or special attack
        int actionChoice = selectedMenuOption + 1; // Adjust for 1-based indexing in battle logic

        // Play attack animation
        arena->playAttackAnimation(actionChoice == 1 ? 0 : 4); // 0 for basic attack, 4 for special attack

        // Execute the actual battle turn
        arena->executeTurn(actionChoice);

        // Check if battle is over
        if (!arena->isBattleActive())
        {
            handleBattleEnd();
        }
        else
        {
            // If it's still player's turn, display action menu again
            if (arena->isPlayerTurn())
            {
                displayBattleActionMenu();
            }
            else
            {
                // Show a message that it's opponent's turn
                char message[100];
                sprintf(message, "It's %s's turn!", arena->getOpponent()->getName().c_str());
                displayMessage(message, 1.0f);

                // Simulate the opponent's turn with a small delay
                sf::Clock delayClock;
                while (delayClock.getElapsedTime().asSeconds() < 1.0f)
                {
                    // Just wait and update the display
                    window.clear();
                    arena->renderBattle();
                    window.display();
                }

                // Execute opponent's turn (the AI in Battle class will choose action)
                arena->executeTurn(0); // 0 is a placeholder, the AI logic chooses the action

                // Check if battle is over again
                if (!arena->isBattleActive())
                {
                    handleBattleEnd();
                }
                else
                {
                    // Display battle action menu for player's next turn
                    displayBattleActionMenu();
                }
            }
        }
    }

    void displayAbilitySelectionMenu()
    {
        // Create an array of ability options
        char options[5][100]; // Max 4 abilities + Cancel option
        int optionCount = 0;

        // Add each ability to the options
        for (int i = 0; i < 4; i++)
        {
            Ability *ability = playerPet->getAbility(i);
            if (ability != nullptr)
            {
                // Format the text
                if (ability->getCurrentCooldown() > 0)
                {
                    sprintf(options[optionCount], "%d. %s (Cooldown: %d)",
                            i + 1, ability->getName().c_str(), ability->getCurrentCooldown());
                }
                else
                {
                    sprintf(options[optionCount], "%d. %s", i + 1, ability->getName().c_str());
                }

                optionCount++;
            }
        }

        // Add cancel option
        sprintf(options[optionCount], "Cancel");
        optionCount++;

        // Convert to const char* array for displayMenu
        const char *optionPtrs[5];
        for (int i = 0; i < optionCount; i++)
        {
            optionPtrs[i] = options[i];
        }

        // Display the menu
        displayMenu("Choose Ability", optionPtrs, optionCount);
    }

    void processBattleAbilitySelection()
    {
        // Check if cancel was selected
        if (selectedMenuOption == currentMenuOptionCount - 1)
        {
            currentState = BATTLE_ACTION;
            displayBattleActionMenu();
            return;
        }

        // Get the selected ability
        int abilityIndex = selectedMenuOption;
        Ability *ability = playerPet->getAbility(abilityIndex);

        // Check if ability can be used
        if (ability == nullptr ||
            !ability->canUse() ||
            playerPet->getMana() < ability->getManaCost())
        {

            displayMessage("Cannot use this ability!", 1.0f);
            displayAbilitySelectionMenu();
            return;
        }

        // Play ability animation based on type
        int animationType = 0;
        if (dynamic_cast<FireballAbility *>(ability) != nullptr ||
            dynamic_cast<ClawStrikeAbility *>(ability) != nullptr)
        {
            animationType = 0; // Attack animation
        }
        else if (dynamic_cast<FlameHealingAbility *>(ability) != nullptr ||
                 dynamic_cast<RebirthAbility *>(ability) != nullptr)
        {
            animationType = 1; // Healing animation
        }
        else if (dynamic_cast<MagicShieldAbility *>(ability) != nullptr)
        {
            animationType = 2; // Shield animation
        }
        else if (dynamic_cast<DragonRoarAbility *>(ability) != nullptr ||
                 dynamic_cast<DiveBombAbility *>(ability) != nullptr)
        {
            animationType = 3; // Stun animation
        }
        else if (dynamic_cast<SpeedBurstAbility *>(ability) != nullptr)
        {
            animationType = 4; // Special animation
        }

        // Play the animation
        arena->playAttackAnimation(animationType);

        // Execute battle turn with ability
        arena->executeTurn(3, abilityIndex);

        // Check if battle is over
        if (!arena->isBattleActive())
        {
            handleBattleEnd();
        }
        else
        {
            // If it's still player's turn, display action menu again
            if (arena->isPlayerTurn())
            {
                currentState = BATTLE_ACTION;
                displayBattleActionMenu();
            }
            else
            {
                // Show a message that it's opponent's turn
                char message[100];
                sprintf(message, "It's %s's turn!", arena->getOpponent()->getName().c_str());
                displayMessage(message, 1.0f);

                // Simulate the opponent's turn with a small delay
                sf::Clock delayClock;
                while (delayClock.getElapsedTime().asSeconds() < 1.0f)
                {
                    // Just wait and update the display
                    window.clear();
                    arena->renderBattle();
                    window.display();
                }

                // Execute opponent's turn (the AI in Battle class will choose action)
                arena->executeTurn(0); // 0 is a placeholder, the AI logic chooses the action

                // Check if battle is over again
                if (!arena->isBattleActive())
                {
                    handleBattleEnd();
                }
                else
                {
                    // Display battle action menu for player's next turn
                    currentState = BATTLE_ACTION;
                    displayBattleActionMenu();
                }
            }
        }
    }

    void displayItemSelectionMenu()
    {
        if (numItems == 0)
        {
            displayMessage("You have no items to use!", 1.5f);
            currentState = BATTLE_ACTION;
            displayBattleActionMenu();
            return;
        }

        // Create options array for inventory items
        char options[21][100]; // maxItems (20) + Cancel option

        // Add each item to the options
        for (int i = 0; i < numItems; i++)
        {
            sprintf(options[i], "%d. %s", i + 1, inventory[i]->getName().c_str());
        }

        // Add cancel option
        sprintf(options[numItems], "Cancel");

        // Convert to const char* array for displayMenu
        const char *optionPtrs[21];
        for (int i = 0; i <= numItems; i++)
        {
            optionPtrs[i] = options[i];
        }

        // Display the menu
        displayMenu("Choose Item", optionPtrs, numItems + 1);
    }

    void processBattleItemSelection()
    {
        // Check if cancel was selected
        if (selectedMenuOption == numItems)
        {
            currentState = BATTLE_ACTION;
            displayBattleActionMenu();
            return;
        }

        // Get the selected item
        int itemIndex = selectedMenuOption;
        Item *item = inventory[itemIndex];

        if (item != nullptr)
        {
            // Use the item on player's pet
            item->use(playerPet);

            // Play appropriate animation based on item type
            int animationType = 0;
            if (dynamic_cast<HealingPotion *>(item) != nullptr)
            {
                animationType = 1; // Healing animation
            }
            else if (dynamic_cast<ManaPotion *>(item) != nullptr)
            {
                animationType = 1; // Healing animation (blue)
            }
            else if (dynamic_cast<SpeedBoost *>(item) != nullptr)
            {
                animationType = 4; // Special animation
            }
            else if (dynamic_cast<Shield *>(item) != nullptr)
            {
                animationType = 2; // Shield animation
            }

            // Play the animation
            effectSprites[animationType].setPosition(
                playerPet->getPosition().x,
                playerPet->getPosition().y);
            effectActive = true;
            effectTimer = 1.0f;

            // Remove the used item
            delete inventory[itemIndex];

            // Shift remaining items
            for (int i = itemIndex; i < numItems - 1; i++)
            {
                inventory[i] = inventory[i + 1];
            }

            inventory[numItems - 1] = nullptr;
            numItems--;

            // Execute a turn in battle (using an item counts as a turn)
            arena->executeTurn(0); // 0 means no attack or ability

            // Check if battle is over
            if (!arena->isBattleActive())
            {
                handleBattleEnd();
            }
            else
            {
                // If it's still player's turn, display action menu again
                if (arena->isPlayerTurn())
                {
                    currentState = BATTLE_ACTION;
                    displayBattleActionMenu();
                }
                else
                {
                    // Show a message that it's opponent's turn
                    char message[100];
                    sprintf(message, "It's %s's turn!", arena->getOpponent()->getName().c_str());
                    displayMessage(message, 1.0f);

                    // Simulate the opponent's turn with a small delay
                    sf::Clock delayClock;
                    while (delayClock.getElapsedTime().asSeconds() < 1.0f)
                    {
                        // Just wait and update the display
                        window.clear();
                        arena->renderBattle();
                        window.display();
                    }

                    // Execute opponent's turn (the AI in Battle class will choose action)
                    arena->executeTurn(0); // 0 is a placeholder, the AI logic chooses the action

                    // Check if battle is over again
                    if (!arena->isBattleActive())
                    {
                        handleBattleEnd();
                    }
                    else
                    {
                        // Display battle action menu for player's next turn
                        currentState = BATTLE_ACTION;
                        displayBattleActionMenu();
                    }
                }
            }
        }
    }

    void handleBattleEnd()
    {
        // Check if player won
        if (arena->didPlayerWin())
        {
            // Show victory message
            displayMessage("Victory! You won the battle!", 2.0f);

            // Award gold based on opponent level
            Pet *opponent = arena->getOpponent();
            if (opponent != nullptr)
            {
                int goldEarned = opponent->getLevel() * 30;
                playerGold += goldEarned;

                char message[100];
                sprintf(message, "You earned %d gold!", goldEarned);
                displayMessage(message, 2.0f);
            }
        }
        else
        {
            // Show defeat message
            displayMessage("Defeat! You lost the battle...", 2.0f);
        }

        // Return to battle menu
        currentState = BATTLE_MENU;
        displayBattleMenu();
    }

    void displayTrainingMenu()
    {
        // Set background texture for training menu
        backgroundSprite.setTexture(backgroundTextures[3]);
        float scaleX = window.getSize().x / backgroundTextures[3].getSize().x;
        float scaleY = window.getSize().y / backgroundTextures[3].getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);

        // Set up training menu options
        const char *options[] = {
            "1. Train Health (50 gold)",
            "2. Train Attack (50 gold)",
            "3. Train Defense (50 gold)",
            "4. Train Speed (50 gold)",
            "5. Train Mana (50 gold)",
            "6. Return to Main Menu"};

        displayMenu("Training Menu", options, 6);
    }

    void processTrainingMenuSelection()
    {
        if (selectedMenuOption == 5)
        { // Return to Main Menu
            currentState = MAIN_MENU;
            displayMainMenu();
            return;
        }

        const int trainingCost = 50;

        if (playerGold < trainingCost)
        {
            displayMessage("Not enough gold for training!", 1.5f);
            return;
        }

        playerGold -= trainingCost;
        trainingCamp->trainPet(playerPet, selectedMenuOption + 1); // +1 to match the 1-based indexing in training logic

        // Display success message
        char message[100];
        switch (selectedMenuOption)
        {
        case 0: // Health
            sprintf(message, "%s's health increased to %d!",
                    playerPet->getName().c_str(), playerPet->getMaxHealth());
            break;
        case 1: // Attack
            sprintf(message, "%s's attack increased to %d!",
                    playerPet->getName().c_str(), playerPet->getAttackPower());
            break;
        case 2: // Defense
            sprintf(message, "%s's defense increased to %d!",
                    playerPet->getName().c_str(), playerPet->getDefense());
            break;
        case 3: // Speed
            sprintf(message, "%s's speed increased to %d!",
                    playerPet->getName().c_str(), playerPet->getSpeed());
            break;
        case 4: // Mana
            sprintf(message, "%s's mana increased to %d!",
                    playerPet->getName().c_str(), playerPet->getMaxMana());
            break;
        }

        displayMessage(message, 1.5f);

        // Update status bar
        updateStatusBar();
    }

    void drawTrainingScreen()
    {
        // Draw the background
        window.draw(backgroundSprite);

        // Draw menu
        drawMenu();

        // Draw pet
        if (playerPet != nullptr)
        {
            playerPet->draw(window);
        }
    }

    void displayShopMenu()
    {
        // Set background texture for shop menu
        backgroundSprite.setTexture(backgroundTextures[4]);
        float scaleX = window.getSize().x / backgroundTextures[4].getSize().x;
        float scaleY = window.getSize().y / backgroundTextures[4].getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);

        // Set up shop menu options
        const char *options[] = {
            "1. Buy Items",
            "2. Sell Items",
            "3. Return to Main Menu"};

        displayMenu("Shop Menu", options, 3);
    }

    void processShopMenuSelection()
    {
        switch (selectedMenuOption)
        {
        case 0: // Buy Items
            currentState = BUY_ITEM_MENU;
            displayBuyItemMenu();
            break;

        case 1: // Sell Items
            currentState = SELL_ITEM_MENU;
            displaySellItemMenu();
            break;

        case 2: // Return to Main Menu
            currentState = MAIN_MENU;
            displayMainMenu();
            break;
        }
    }

    void displayBuyItemMenu()
    {
        // Set up buy item menu options
        const char *options[] = {
            "1. Healing Potion (30 gold)",
            "2. Mana Potion (25 gold)",
            "3. Speed Boost (40 gold)",
            "4. Shield (35 gold)",
            "5. Return to Shop Menu"};

        displayMenu("Buy Items", options, 5);
    }

    void processBuyItemSelection()
    {
        if (selectedMenuOption == 4)
        { // Return to Shop Menu
            currentState = SHOP_MENU;
            displayShopMenu();
            return;
        }

        int cost = 0;
        Item *item = nullptr;

        switch (selectedMenuOption)
        {
        case 0: // Healing Potion
            cost = 30;
            item = new HealingPotion();
            break;
        case 1: // Mana Potion
            cost = 25;
            item = new ManaPotion();
            break;
        case 2: // Speed Boost
            cost = 40;
            item = new SpeedBoost();
            break;
        case 3: // Shield
            cost = 35;
            item = new Shield();
            break;
        }

        if (playerGold < cost)
        {
            displayMessage("Not enough gold!", 1.5f);
            delete item;
            return;
        }

        if (numItems >= maxItems)
        {
            displayMessage("Inventory is full!", 1.5f);
            delete item;
            return;
        }

        playerGold -= cost;
        inventory[numItems++] = item;

        char message[100];
        sprintf(message, "Purchased %s!", item->getName().c_str());
        displayMessage(message, 1.5f);

        // Update status bar
        updateStatusBar();
    }

    void displaySellItemMenu()
    {
        if (numItems == 0)
        {
            displayMessage("You have no items to sell!", 1.5f);
            currentState = SHOP_MENU;
            displayShopMenu();
            return;
        }

        // Create options array for inventory items
        char options[21][100]; // maxItems (20) + Cancel option

        // Add each item to the options
        for (int i = 0; i < numItems; i++)
        {
            int sellValue = inventory[i]->getPrice() / 2;
            sprintf(options[i], "%d. %s (%d gold)",
                    i + 1, inventory[i]->getName().c_str(), sellValue);
        }

        // Add cancel option
        sprintf(options[numItems], "Cancel");

        // Convert to const char* array for displayMenu
        const char *optionPtrs[21];
        for (int i = 0; i <= numItems; i++)
        {
            optionPtrs[i] = options[i];
        }

        // Display the menu
        displayMenu("Sell Items", optionPtrs, numItems + 1);
    }

    void processSellItemSelection()
    {
        // Check if cancel was selected
        if (selectedMenuOption == numItems)
        {
            currentState = SHOP_MENU;
            displayShopMenu();
            return;
        }

        int index = selectedMenuOption;
        int sellValue = inventory[index]->getPrice() / 2;

        char message[100];
        sprintf(message, "Sold %s for %d gold.",
                inventory[index]->getName().c_str(), sellValue);

        playerGold += sellValue;

        delete inventory[index];

        // Shift remaining items
        for (int i = index; i < numItems - 1; i++)
        {
            inventory[i] = inventory[i + 1];
        }

        inventory[numItems - 1] = nullptr;
        numItems--;

        displayMessage(message, 1.5f);

        // Update status bar
        updateStatusBar();

        // Return to sell menu or shop menu
        if (numItems > 0)
        {
            displaySellItemMenu();
        }
        else
        {
            currentState = SHOP_MENU;
            displayShopMenu();
        }
    }

    void drawShopScreen()
    {
        // Draw the background
        window.draw(backgroundSprite);

        // Draw menu
        drawMenu();

        // If in sell item menu, draw inventory visual
        if (currentState == SELL_ITEM_MENU && numItems > 0)
        {
            // Draw inventory title
            sf::Text inventoryTitle;
            inventoryTitle.setFont(font);
            inventoryTitle.setCharacterSize(22);
            inventoryTitle.setFillColor(sf::Color::White);
            inventoryTitle.setString("Inventory:");
            inventoryTitle.setPosition(
                window.getSize().x * 0.75f - 80.f,
                window.getSize().y * 0.55f);
            window.draw(inventoryTitle);

            // Draw inventory slots
            for (int i = 0; i < numItems; i++)
            {
                sf::RectangleShape itemSlot(sf::Vector2f(50.f, 50.f));
                itemSlot.setFillColor(sf::Color(50, 50, 50));
                itemSlot.setOutlineColor(sf::Color::White);
                itemSlot.setOutlineThickness(1.f);
                itemSlot.setPosition(
                    window.getSize().x * 0.75f - 75.f + (i % 3) * 60.f,
                    window.getSize().y * 0.6f + (i / 3) * 60.f);
                window.draw(itemSlot);

                // Draw item icon (placeholder)
                sf::CircleShape itemIcon(15.f);

                // Different colors for different items
                if (dynamic_cast<HealingPotion *>(inventory[i]) != nullptr)
                {
                    itemIcon.setFillColor(sf::Color::Green);
                }
                else if (dynamic_cast<ManaPotion *>(inventory[i]) != nullptr)
                {
                    itemIcon.setFillColor(sf::Color::Blue);
                }
                else if (dynamic_cast<SpeedBoost *>(inventory[i]) != nullptr)
                {
                    itemIcon.setFillColor(sf::Color::Yellow);
                }
                else if (dynamic_cast<Shield *>(inventory[i]) != nullptr)
                {
                    itemIcon.setFillColor(sf::Color::Cyan);
                }
                else
                {
                    itemIcon.setFillColor(sf::Color::White);
                }

                itemIcon.setPosition(
                    itemSlot.getPosition().x + 10.f,
                    itemSlot.getPosition().y + 10.f);
                window.draw(itemIcon);
            }
        }
    }

    void displayGuildMenu()
    {
        // Set background texture for guild menu
        backgroundSprite.setTexture(backgroundTextures[5]);
        float scaleX = window.getSize().x / backgroundTextures[5].getSize().x;
        float scaleY = window.getSize().y / backgroundTextures[5].getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);

        if (playerGuild == nullptr)
        {
            // Set up guild creation menu options
            const char *options[] = {
                "1. Create a Guild (500 gold)",
                "2. Return to Main Menu"};

            displayMenu("Guild Menu", options, 2);
        }
        else
        {
            // Set up guild management menu options
            const char *options[] = {
                "1. Recruit New Member (300 gold)",
                "2. Leave Guild",
                "3. Return to Main Menu"};

            // Set custom title including guild name
            char title[100];
            sprintf(title, "%s Guild (Level %d)",
                    playerGuild->getName().c_str(), playerGuild->getGuildLevel());

            menuTitle.setString(title);
            menuTitle.setPosition(
                (window.getSize().x - menuTitle.getGlobalBounds().width) / 2.f,
                menuBackground.getPosition().y + 20.f);

            setMenuOptions(options, 3);
            updateSelectedMenuOption(0);

            // Display guild members visually
            drawGuildMembers();
        }
    }

    void processGuildMenuSelection()
    {
        if (playerGuild == nullptr)
        {
            // Process guild creation menu selection
            switch (selectedMenuOption)
            {
            case 0: // Create a Guild
            {
                if (playerGold < 500)
                {
                    displayMessage("Not enough gold to create a guild!", 1.5f);
                    return;
                }

                // Get guild name
                string guildName = getTextInput("Enter a name for your guild:");

                if (guildName.empty())
                {
                    displayMessage("Guild creation cancelled.", 1.5f);
                    return;
                }

                playerGold -= 500;
                playerGuild = new Guild(guildName);
                playerGuild->addMember(playerPet);

                char message[100];
                sprintf(message, "Created the %s guild!", guildName.c_str());
                displayMessage(message, 1.5f);

                // Update status bar
                updateStatusBar();

                // Refresh guild menu
                displayGuildMenu();
            }
            break;

            case 1: // Return to main menu
            {
                currentState = MAIN_MENU;
                displayMainMenu();
            }
            break;
            }
        }
        else
        {
            // Process guild management menu selection
            switch (selectedMenuOption)
            {
            case 0: // Recruit new memb
            {
                if (playerGold < 300)
                {
                    displayMessage("Not enough gold to recruit a member", 1.5f);
                    return;
                }

                if (playerGuild->getNumMembers() >= 10)
                {
                    displayMessage("Guild is already full", 1.5f);
                    return;
                }

                playerGold -= 300;

                // random pet 4 guild
                const char *names[] = {"A", "B", "C", "D", "E",
                                       "F", "G", "H", "I", "J"};
                string name = names[rand() % 10];

                Pet *newMember = nullptr;
                switch (rand() % 4)
                {
                case 0:
                    newMember = new Dragon(name);
                    break;
                case 1:
                    newMember = new Phoenix(name);
                    break;
                case 2:
                    newMember = new Unicorn(name);
                    break;
                case 3:
                    newMember = new Griffin(name);
                    break;
                }

                // Set the level
                int levelDiff = rand() % 3 - 1; // -1, 0, or 1
                newMember->setLevel(playerPet->getLevel() + levelDiff);
                if (newMember->getLevel() < 1)
                    newMember->setLevel(1);

                playerGuild->addMember(newMember);

                char message[100];
                sprintf(message, "%s joined your guild!", name.c_str());
                displayMessage(message, 1.5f);

                // Update status bar
                updateStatusBar();

                // Refresh guild menu
                displayGuildMenu();
            }
            break;

            case 1: // Leave Guild
            {
                playerGuild->removeMember(playerPet);

                // If the player was the last member, delete the guild
                if (playerGuild->getNumMembers() == 0)
                {
                    delete playerGuild;
                    playerGuild = nullptr;
                    displayMessage("You left your guild and it was disbanded.", 1.5f);
                }
                else
                {
                    displayMessage("You left your guild.", 1.5f);
                }

                // Update status bar
                updateStatusBar();

                // Refresh guild menu
                displayGuildMenu();
            }
            break;

            case 2: // Return to Main Menu
            {
                currentState = MAIN_MENU;
                displayMainMenu();
            }
            break;
            }
        }
    }

    void drawGuildMembers()
    {
        if (playerGuild == nullptr)
            return;

        // Draw guld info
        sf::Text guildInfo;
        guildInfo.setFont(font);
        guildInfo.setCharacterSize(18);
        guildInfo.setFillColor(sf::Color::White);

        char info[100];
        sprintf(info, "Members: %d/10 - Guild Experience: %d/%d",
                playerGuild->getNumMembers(),
                playerGuild->getGuildLevel() * 100, // Placeholder for actual guild exp
                playerGuild->getGuildLevel() * 500);

        guildInfo.setString(info);
        guildInfo.setPosition(
            menuBackground.getPosition().x + 50.f,
            menuBackground.getPosition().y + 60.f);

        window.draw(guildInfo);
    }

    void drawGuildScreen()
    {
        // Draw the background
        window.draw(backgroundSprite);

        // Draw menu
        drawMenu();

        // If guild exists, display member as circle
        if (playerGuild != nullptr)
        {
            for (int i = 0; i < playerGuild->getNumMembers(); i++)
            {
                Pet *member = playerGuild->getMember(i);
                if (member != nullptr)
                {
                    sf::CircleShape memberCircle(15.f);

                    if (member == playerPet)
                    {
                        memberCircle.setFillColor(sf::Color::Yellow);
                    }
                    else
                    {
                        memberCircle.setFillColor(sf::Color::White);
                    }

                    memberCircle.setPosition(
                        window.getSize().x * 0.75f - 75.f + (i % 5) * 40.f,
                        window.getSize().y * 0.5f + (i / 5) * 40.f);

                    window.draw(memberCircle);

                    // Draw member level
                    sf::Text levelText;
                    levelText.setFont(font);
                    levelText.setCharacterSize(12);
                    levelText.setFillColor(sf::Color::Black);
                    levelText.setString(to_string(member->getLevel()));
                    levelText.setPosition(
                        memberCircle.getPosition().x + 10.f - levelText.getGlobalBounds().width / 2.f,
                        memberCircle.getPosition().y + 10.f - levelText.getGlobalBounds().height / 2.f);

                    window.draw(levelText);
                }
            }
        }
    }

    void displayPetManagementMenu()
    {
        // Set background texture for pm menu
        backgroundSprite.setTexture(backgroundTextures[6]);
        float scaleX = window.getSize().x / backgroundTextures[6].getSize().x;
        float scaleY = window.getSize().y / backgroundTextures[6].getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);

        // Set up pet management menu options
        const char *options[] = {
            "1. Rename Pet",
            "2. Rest (Restore Health/Mana)",
            "3. Return to Main Menu"};

        displayMenu("Pet Management", options, 3);

        // Position pet 
        if (playerPet != nullptr)
        {
            playerPet->setPosition(sf::Vector2f(window.getSize().x * 0.75f, window.getSize().y * 0.4f));
        }
    }

    void processPetManagementMenuSelection()
    {
        switch (selectedMenuOption)
        {
        case 0: // Rename ur pet
        {
            string newName = getTextInput("Enter new name:");

            if (!newName.empty())
            {
                playerPet->setName(newName);

                char message[100];
                sprintf(message, "Pet renamed to %s!", newName.c_str());
                displayMessage(message, 1.5f);

                // Update status bar
                updateStatusBar();
            }
        }
        break;

        case 1: // Rest
            playerPet->setHealth(playerPet->getMaxHealth());
            playerPet->setMana(playerPet->getMaxMana());
            displayMessage("Pet fully restored!", 1.5f);

            // Update status bar
            updateStatusBar();
            break;

        case 2: // Return to Main Menu
            currentState = MAIN_MENU;
            displayMainMenu();
            break;
        }
    }

    void drawPetManagementScreen()
    {
        // Draw the background
        backgroundSprite.setTexture(backgroundTextures[6]);
        float scaleX = window.getSize().x / (float)backgroundTextures[6].getSize().x;
        float scaleY = window.getSize().y / (float)backgroundTextures[6].getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
        window.draw(backgroundSprite);

        // Draw menu
        drawMenu();

        // Draw pet
        if (playerPet != nullptr)
        {
            playerPet->draw(window);

            // Draw pet info
            sf::Text petInfo;
            petInfo.setFont(font);
            petInfo.setCharacterSize(18);
            petInfo.setFillColor(sf::Color::White);

            char info[256];
            sprintf(info,
                    "Name: %s\nType: %s\nLevel: %d\nEXP: %d/%d\n\nHealth: %d/%d\nMana: %d/%d\nAttack: %d\nDefense: %d\nSpeed: %d",
                    playerPet->getName().c_str(),
                    playerPet->getType().c_str(),
                    playerPet->getLevel(),
                    playerPet->getExperience(),
                    playerPet->getExperienceToNextLevel(),
                    playerPet->getHealth(),
                    playerPet->getMaxHealth(),
                    playerPet->getMana(),
                    playerPet->getMaxMana(),
                    playerPet->getAttackPower(),
                    playerPet->getDefense(),
                    playerPet->getSpeed());

            petInfo.setString(info);
            petInfo.setPosition(
                window.getSize().x * 0.75f - 150.f,
                window.getSize().y * 0.55f);

            window.draw(petInfo);

            float leftPadding = menuBackground.getPosition().x + 40.f;
            float abilitiesBoxHeight = 4 * 22.f + 25.f;
            float menuCenterY = menuBackground.getPosition().y + menuBackground.getSize().y / 2.f;
            float startY = menuCenterY - abilitiesBoxHeight / 2.f + 30;

            sf::Text abilitiesTitle;
            abilitiesTitle.setFont(font);
            abilitiesTitle.setCharacterSize(18);
            abilitiesTitle.setFillColor(sf::Color::Yellow);
            abilitiesTitle.setString("Abilities:");
            abilitiesTitle.setPosition(leftPadding, startY);

            window.draw(abilitiesTitle);

            for (int i = 0; i < 4; i++)
            {
                Ability *ability = playerPet->getAbility(i);
                if (ability != nullptr)
                {
                    sf::Text abilityText;
                    abilityText.setFont(font);
                    abilityText.setCharacterSize(16);
                    abilityText.setFillColor(sf::Color::White);

                    char abilityInfo[100];
                    if (ability->getCurrentCooldown() > 0)
                    {
                        sprintf(abilityInfo, "%d. %s (Cooldown: %d)",
                                i + 1, ability->getName().c_str(), ability->getCurrentCooldown());
                    }
                    else
                    {
                        sprintf(abilityInfo, "%d. %s", i + 1, ability->getName().c_str());
                    }

                    abilityText.setString(abilityInfo);
                    abilityText.setPosition(leftPadding, startY + 40.f + i * 22.f);

                    window.draw(abilityText);
                }
            }
        }
    }
};

// Using Abilities
void FireballAbility::use(Pet *user, Pet *target)
{
    cout << user->getName() << " launches a fireball at " << target->getName() << endl;
    int damage = user->getAttackPower() * 1.8;
    target->takeDamage(damage);
}

void DragonRoarAbility::use(Pet *user, Pet *target)
{
    cout << user->getName() << " roars" << endl;
    target->applyStun(2);
    cout << target->getName() << " is stunned for 2 turns" << endl;
}

void FlameHealingAbility::use(Pet *user, Pet *target)
{
    cout << user->getName() << " uses flame healing on " << target->getName() << endl;
    int healAmount = user->getAttackPower() * 0.8;
    target->heal(healAmount);
}

void RebirthAbility::use(Pet *user, Pet *target)
{
    cout << user->getName() << " will have rebirth" << endl;
    if (!hasBeenUsed)
    {
        hasBeenUsed = true;
        cout << "Rebirth can be used only if " << user->getName() << " dies" << endl;
    }
    else
    {
        cout << "Rebirth has already been used this match" << endl;
    }
}

void SpeedBurstAbility::use(Pet *user, Pet *target)
{
    cout << user->getName() << " uses speed burst" << endl;
    int speedBoost = user->getSpeed() / 2;
    user->applySpeedBoost(speedBoost, 3);
    cout << user->getName() << " speed increased by " << speedBoost << " for 3 turns" << endl;
}

void MagicShieldAbility::use(Pet *user, Pet *target)
{
    cout << user->getName() << " creates a magic shield!" << endl;
    user->applyShield(3);
    cout << user->getName() << " is shielded for 3 turns" << endl;
}

void ClawStrikeAbility::use(Pet *user, Pet *target)
{
    cout << user->getName() << " strikes " << target->getName() << " using claws" << endl;
    int damage = user->getAttackPower() * 1.5;
    target->takeDamage(damage);
}

void DiveBombAbility::use(Pet *user, Pet *target)
{
    cout << user->getName() << " dive bombs " << target->getName() << " from above" << endl;
    int damage = user->getAttackPower() * 1.4;
    target->takeDamage(damage);

    // 50-50 chance of stunning opp
    if (rand() % 100 < 50)
    {
        target->applyStun(1);
        cout << target->getName() << " is stunned for 1 turn" << endl;
    }
}

// Implement Item use methods
void HealingPotion::use(Pet *target)
{
    cout << "Using Healing Potion on " << target->getName() << endl;
    target->heal(healAmount);
}

void ManaPotion::use(Pet *target)
{
    cout << "Using Mana Potion on " << target->getName() << endl;
    target->regenerateMana(manaAmount);
    cout << target->getName() << " recovered " << manaAmount << " mana!" << endl;
}

void SpeedBoost::use(Pet *target)
{
    cout << "Using Speed Boost on " << target->getName() << endl;
    target->applySpeedBoost(boostAmount, duration);
    cout << target->getName() << " speed increased by " << boostAmount << " for " << duration << " turns" << endl;
}

void Shield::use(Pet *target)
{
    cout << "Using Shield on " << target->getName() << endl;
    target->applyShield(duration);
    target->applyDefenseBoost(defenseBoost, duration);
    cout << target->getName() << " defense increased by " << defenseBoost << " for " << duration << " turns" << endl;
}

void Ability::use(Pet *user, Pet *target)
{
    cout << user->getName() << " uses " << name << " on " << target->getName() << endl;
}

// Displaying Abilities
void Ability::displayInfo() const
{
    cout << name << " - " << description << endl;
    cout << "Mana Cost: " << manaCost << " - Cooldown: " << cooldown << " turns" << endl;

    if (currentCooldown > 0)
    {
        cout << "Currently on cooldown: " << currentCooldown << " turns remaining" << endl;
    }
}

// Main function
int main()
{
    // Registration and login
    string username;
    cout << "Enter your username: ";
    cin >> username;
    if (!loadUser (username)) {
        cout << "New user detected. Registering..." << endl;
        saveUser (username, 0); 
    }
	Game game;
    game.run();
    return 0;
}

