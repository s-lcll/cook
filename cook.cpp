#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <time.h>
#include <algorithm>
using namespace std;
//max ingredient, burger, order num
const int INGREDIENT = 8;
const int BURGER = 5;
const int ORDER = 50;

void inputCheck()
{
    cin.ignore();
    cin.clear();
}
string timeToString(int);
string setFormat(string, bool);
string resetFormat();
void clearScreen()
{
    //*nux
    cout << "\e[2J\e[0;0H";
    //win
    // system("CLS");
}
void dispMsg(string);

class Ingredient
{
public:
    void set(string n, char s, string i)
    {
        name = n;
        symbol = toupper(s);
        instruct = i;
    };
    string getName() { return name; };
    char getSymbol() { return symbol; };
    string getInstruct() { return instruct; }

private:
    string instruct, name;
    char symbol;
};

class Burger
{
public:
    void set(string n, int t, string s, int sc)
    {
        name = n;
        cookingTime = t;
        cook = s;
        score = sc;
    };
    string getName() { return name; }
    string getCook() { return cook; }
    int getTime() { return cookingTime; };
    int getScore() { return score; };

private:
    string name, cook;
    int cookingTime, score;
};

class Order
{
public:
    void setProcessState(char ps)
    {
        processState = ps;
        switch (processState)
        {
        case 'P':
            timeP = time(0);
            break;
        case 'C':
            timeC = time(0);
            break;
        };
    }
    void setOrder(int i, int b)
    {
        index = i;
        burger = b;
        setProcessState('P');
    };
    int getIndex() { return index; }
    int getBurger() { return burger; }
    int getTimeElapsed() { return time(0) - timeP; }
    int getTimeCooked() { return time(0) - timeC; }
    int getScore(int setTL, int burgerScore)
    {
        if (getTimeElapsed() <= setTL)
            if (processState == 'D')
                return burgerScore;
            else
                return 0;
        else
        {
            processState = 'D';
            return -5;
        }; //5 marks is deduced if timeout
    }
    char getProcessStateCode() { return processState; }
    string getProcessState()
    {
        switch (processState)
        {
        case 'P':
            return "preparing";
        case 'C':
            return setFormat("Yellow", 0) + "cooking" + resetFormat();
        case 'R':
            return setFormat("Green", 0) + "ready to serve" + resetFormat();
        default:
            return 0;
        };
    }

private:
    int index, burger;
    char processState; // [P]erpare, [C]ooking, [R]eady, [D]one
    int timeP, timeC;  //timeP = timeCreated timeC = timeStartedCooking
};

void displayIntro();
void displayMenu();
void gmInit(Ingredient(&)[INGREDIENT], Burger(&)[BURGER]);

void gsgOrderList(Order(&arrOl)[ORDER], int& index, int& current, int max, Burger(arrB)[BURGER], int setTL, int& score)
{
    int counter = 0;
    for (int i = 1; i <= current; i++)
    {
        //update order state
        if (arrOl[i].getProcessStateCode() == 'C')
        {
            if (arrOl[i].getTimeCooked() >= arrB[arrOl[i].getBurger()].getTime())
            {
                arrOl[i].setProcessState('R');
                stringstream msgString;
                msgString << "[" << setFormat("Magenta", 1) << setw(max / 10 > 0 ? 2 : 1) << i << resetFormat() << "] ";
                msgString << setFormat("Green", 1) << "Order #" << arrOl[i].getIndex() << " is ready to serve!" << resetFormat();
                dispMsg(msgString.str());
            }
        }
        //update score
        int orderScore = arrOl[i].getScore(setTL, arrB[arrOl[i].getBurger()].getScore());
        if (orderScore < 0)
            counter++;

        score += orderScore;
        //remove done & timeout orders
        if (arrOl[i].getProcessStateCode() == 'D')
        {
            current--;
            for (int j = i; j <= max; j++)
                arrOl[j] = arrOl[j + 1];
            i--;
        }
    }

    if (counter != 0)
    {
        if (counter == 1)
            dispMsg(setFormat("Red", 1) + "An order is TIMEOUT-ed =.=\"" + resetFormat());
        else
            dispMsg(setFormat("Red", 1) + to_string(counter) + " orders are TIMEOUT-ed =_=|||" + resetFormat());
    }
    counter = current;

    //create new orders
    if (current < max)
    {
        //create new orders
        for (int i = max - current; i > 0; i--)
        {
            index++;
            current++;

            arrOl[current].setOrder(index, rand() % BURGER);
        }
    }
    //cout list
    cout << "---- ORDER LIST ----" << endl;
    for (int i = 1; i <= max; i++)
    {
        cout << "[" << setFormat("Magenta", 1) << setw(max / 10 > 0 ? 2 : 1) << i << resetFormat() << "]  "
            << "Order #" << arrOl[i].getIndex() << ": " << arrB[arrOl[i].getBurger()].getName() << ", " << arrOl[i].getProcessState() << ", "
            << timeToString(setTL - arrOl[i].getTimeElapsed());
        if (counter < i)
            cout << setFormat("Cyan", 1) << " NEW" << resetFormat() << endl;
        else
            cout << endl;
    }
}

void gsgOrderInfo(int n, Order order, Burger burger, int setTL, int maxNumOrder)
{
    //updated parameters

    cout << "[" << setFormat("Magenta", 1) << setw(maxNumOrder / 10 > 0 ? 2 : 1) << n << resetFormat() << "] ";

    cout << "*** Process Order ***" << endl;
    cout << left;
    cout << setw(22) << "Order #"
        << ": " << order.getIndex() << endl;
    cout << setw(22) << "Burger"
        << ": " << burger.getName() << endl;
    cout
        << setw(22) << "Status"
        << ": " << order.getProcessState() << endl;
    cout << setw(22) << "Remaining Time"
        << ": " << timeToString(setTL - order.getTimeElapsed()) << endl;
    cout << setw(22) << "Burger Ingredient List"
        << ": " << n << endl;

    cout << setw(22) << "Burger Key List"
        << ": " << setFormat("Magenta", 1) << burger.getCook() << resetFormat() << endl;
    cout << endl;
    cout << "Please choose [U] for update, [R] for return, or" << endl;
    cout << "type correct key list to start cooking: ";
}

int gsgGetCook(Order& order, string matchCheck, string input)
{
    //updated parameters
    //acept all input now for dev
    transform(matchCheck.begin(), matchCheck.end(), matchCheck.begin(), ::toupper);
    transform(input.begin(), input.end(), input.begin(), ::toupper);
    // order.setProcessState('C');
    clearScreen();
    if (input == "U")
        return 1;
    else if (input == matchCheck)
    {
        order.setProcessState('C');
        cout << setFormat("Yellow", 1);
        dispMsg("Order #" + to_string(order.getIndex()) + resetFormat() + " Cooking Now..." + setFormat("Yellow", 1));
        cout << resetFormat();
    }
    else if (input == "R")
    {
    }
    else
    {
        dispMsg(setFormat("Red", 1) + "Wrong Code!" + resetFormat());
        return 1;
    }
    return 0;
}
void gmStartGame(int settingTL, int settingMON, Burger(arrBurger)[BURGER])
{
    clearScreen();
    //init
    int score = 10;
    int startTime = time(0);
    int playTime = settingTL;
    int indexOrder = 0;
    int currentOrder = 0;
    Order arrOrder[ORDER] = {};
    string opt = "U"; //change to string
    do
    {
        int orderSelected = 0;
        try
        {
            orderSelected = stoi(opt);
        }
        catch (const exception& e)
        {
        }

        //case U | Q | default {1-mon} {err}
        if (opt == "U")
        {
            gsgOrderList(arrOrder, indexOrder, currentOrder, settingMON, arrBurger, playTime, score);
            cout << "-----------------------------------------------" << endl;
            cout << "Score: " << score << endl;
            cout << "Enter [U] for update, [Q] for Quit, or [1-" << settingMON << "] for order:";
            opt = "0";
        }
        else if (opt == "Q")
        {
        }
        else if (orderSelected > 0 && orderSelected <= settingMON) // string to integer
        {
            clearScreen();
            Order& tmpO = arrOrder[orderSelected];
            Burger tmpB = arrBurger[tmpO.getBurger()];
            if (tmpO.getProcessStateCode() == 'P')
            {
                string input;
                do
                {
                    gsgOrderInfo(orderSelected, tmpO, tmpB, playTime, settingMON);
                    cout << setFormat("Magenta", 1);
                    cin >> input;
                    cout << resetFormat();
                } while (gsgGetCook(tmpO, tmpB.getCook(), input) != 0);
                opt = 'U';
            }
            else if (tmpO.getProcessStateCode() == 'C')
            {
                if (tmpO.getTimeCooked() >= tmpB.getTime())
                {
                    tmpO.setProcessState('R');
                    stringstream msgString;
                    msgString << "[" << setFormat("Magenta", 1) << setw(settingMON / 10 > 0 ? 2 : 1) << orderSelected << resetFormat() << "] ";
                    msgString << setFormat("Green", 1) << "Order #" << to_string(tmpO.getIndex()) << resetFormat() << " is Ready!";
                    dispMsg(msgString.str());
                }
                else
                {
                    stringstream msgString;
                    msgString << "[" << setFormat("Magenta", 1) << setw(settingMON / 10 > 0 ? 2 : 1) << orderSelected << resetFormat() << "] ";
                    msgString << setFormat("Yellow", 1) << "Order #" << to_string(tmpO.getIndex()) << resetFormat() << " is Cooking...";
                    msgString << "Time remains: " << timeToString(tmpB.getTime() - tmpO.getTimeCooked());
                    dispMsg(msgString.str());
                    opt = 'U';
                }
            }

            else if (tmpO.getProcessStateCode() == 'R')
            {
                tmpO.setProcessState('D');
                stringstream msgString;

                msgString << setFormat("Blue", 1) << "Order #" << to_string(tmpO.getIndex()) << resetFormat() << " is Served.";
                dispMsg(msgString.str());
                opt = "U";
            }
        }
        else
        {
            dispMsg("fku enter again");
            opt = 'U';
        }
        cout << endl;
        if (opt != "U")
        {
            cout << setFormat("Magenta", 1);
            cin >> opt;
            cout << resetFormat();

            clearScreen();
            ;
        }
        transform(opt.begin(), opt.end(), opt.begin(), ::toupper);
    } while (opt != "Q");
    //start game
}

void gmSettings(int& tl, int& mon, bool isStartGame)
{
    clearScreen();
    char opt;

    do
    {

        if (isStartGame)
            dispMsg("Confirm the Settings to Start ^_^");
        cout
            << "*** Settings Menu ***" << endl;
        cout << "[1] Time limits [" << tl << " sec]" << endl;
        cout << "[2] Max number of orders [" << mon << "]" << endl;
        if (!isStartGame)

            cout << "[3] Return to Game Menu" << endl;
        cout << "*********************" << endl;
        if (isStartGame)
            cout << "Enter " << setFormat("Yellow", 1) << "[S]" << resetFormat() << " to Start Game or Option(1 - 2): ";
        else
            cout << "Option (1 - 3): ";
        cout << setFormat("Magenta", 1);
        cin >> opt;
        cout << resetFormat();
        inputCheck();
        switch (opt)
        {
            int x;
        case '1':
            cout << "   >> Set Time limits to: ";
            cout << setFormat("Magenta", 1);
            cin >> x;
            cout << resetFormat();

            inputCheck();
            clearScreen();
            if (x > 0 && x < 300)
                tl = x;
            else
                dispMsg("Invalid : range 1 - 299");

            break;
        case '2':
            cout << "   >> Set Max number of orders to: ";
            cin >> x;
            inputCheck();
            if (x > 0 && x <= 50)
                mon = x;
            else
                dispMsg("Invalid : range 1-50");

            clearScreen();
            break;
        case '3':
            if (!isStartGame)
                break;
        case 'S':
            if (isStartGame)
                break;
        case 's':
            if (isStartGame)
            {
                opt = 'S';
                break;
            }

        default:
            clearScreen();
            cout << "Enter again" << endl;
            break;
        }
        cout << endl;

    } while ((opt != '3' && !isStartGame) || (opt != 'S' && isStartGame));
}
void gmBurgerMenus(Ingredient(arrI)[INGREDIENT], Burger(arrB)[BURGER])
{
    clearScreen();
    char opt;
    do
    {
        cout << "**** Burger Menu ****" << endl;
        cout << "[1] Types of Burger" << endl;
        cout << "[2] Ingredient Symbols" << endl;
        cout << "[3] Return to Game Menu" << endl;
        cout << "Option (1 - 3): ";
        cout << setFormat("Magenta", 1);
        cin >> opt;
        cout << resetFormat();

        clearScreen();
        switch (opt)
        {
        case '1':
            cout << "**** Types of Burger ****" << endl;
            for (int i = 0; i <= BURGER; i++)
                cout << arrB[i].getName() << endl;
            break;
        case '2':
            cout << "**** Ingredient Symbols ****" << endl;
            for (int i = 0; i < INGREDIENT; i++)
                cout << "[" << arrI[i].getSymbol() << "]  " << arrI[i].getName() << endl;
            break;
        case '3':
            break;
        default:
            cout << "Enter again" << endl;
            break;
        }
        cout << endl;
    } while (opt != '3');
}

void gmInstructions()
{
    cout << "INSTRUCTIONS" << endl;
    cout << "This is a cooking game. Enter option 1 to start the game." << endl;
    cout << "You are a cook. You have to receive order and cook for it. Type the ingredient of the order and serve it to customer.\n";
    cout << "If you can successfully serve the food to customer, you get some scores.\n";
    cin.get();
    cin.get();
}

void gmCredits()
{
    clearScreen();
    //credit msg
    cout
        << "CREDITS" << endl;
    cout << "    Name       Student ID  Class \n";
    cout << "*********************************\n\n";
    cout << "Lee Chi Lung   18184498A   201B   GROUP LEADER\n";
    cout << "Chow Lap Fung  18025069A   201D\n";
    cout << "Cheung Wai Yu  18200953A   201B\n";
    cout << "Chan Kui Yuen  18148600A   201A\n";
    cout << "Chan Hon Fai   18091800A   201C\n\n";
    cout << "*********************************\n";
    cin.get();
    cin.get();
}

void gmExit(char& o)
{
    //exit msg??
    char opt;
    do
    {
        clearScreen();

        cout << "User's confirmation? y/n" << endl;
        cout << setFormat("Magenta", 1);
        cin >> opt;
        cout << resetFormat();

        opt = toupper(opt);

        switch (opt)
        {
        case 'Y':
            cout << "Goodbye" << endl;
            break;
        case 'N':
            o = 0;
            break;
        default:
            cout << "Enter again" << endl;
            break;
        }
        cout << (opt != 'N' && opt != 'Y') << endl;
    } while (opt != 'N' && opt != 'Y');
}
//creative???
void gmcRandomBurger(Burger& burger, Ingredient arr[])
{
    //create random burger for hidden mode
    string name[] = { "Appetizing",
                     "Delectable",
                     "Flavorful",
                     "Luscious",
                     "Pungent",
                     "Savory",
                     "Spicy",
                     "Tasty" };
    stringstream cookKeys;
    cookKeys << "b"
        << "b";

    burger.set(name[rand() % (sizeof(name) / sizeof(string))], (rand() % 12 + 1) * 5, cookKeys.str(), (rand() % 6 + 1) * 5);
}

int main()
{
    resetFormat();
    // clearScreen();
    srand(time(0));
    displayIntro();
    //init i & b
    Ingredient ingredientList[INGREDIENT];
    Burger burgerList[BURGER];
    gmInit(ingredientList, burgerList);
    //init settings
    int settingTimeLimit = 40;
    int settingMaxOrderNum = 5;

    //load menu
    char menuOpt;
    do
    {
        displayMenu();
        cout << setFormat("Magenta", 1);
        cin >> menuOpt;
        cout << resetFormat();

        switch (menuOpt)
        {
        case '1':
            gmSettings(settingTimeLimit, settingMaxOrderNum, 1);
            gmStartGame(settingTimeLimit, settingMaxOrderNum, burgerList);
            break;
        case '2':
            gmSettings(settingTimeLimit, settingMaxOrderNum, 0);
            break;
        case '3':
            gmBurgerMenus(ingredientList, burgerList);
            break;
        case '4':
            gmInstructions();
            break;
        case '5':
            gmCredits();
            break;
        case '6':
            gmExit(menuOpt);
            break;
        default:
            clearScreen();
            ;
            dispMsg(setFormat("Red", 0) + "invalid input" + resetFormat());
            break;
        }
    } while (menuOpt != '6');
    return 0;
}

string timeToString(int t) { return (to_string(t / 60) + "\'" + to_string(t % 60) + "\""); }
//
string setFormat(string color, bool isBold)
{
    //*nix
    stringstream cCode;
    cCode << "\033[";
    if (isBold)
        cCode << "1;3";
    else
        cCode << "0;3";
    if (color == "Red")
        cCode << "1";
    else if (color == "Green")
        cCode << "2";
    else if (color == "Yellow")
        cCode << "3";
    else if (color == "Blue")
        cCode << "4";
    else if (color == "Magenta")
        cCode << "5";
    else if (color == "Cyan")
        cCode << "6";
    else if (color == "White")
        cCode << "7";
    else if (color == "Black")
        cCode << "0";
    cCode << "m";
    return cCode.str();
    //win ("Not support BOLD")
    // if (color == "Red")
    //     system("color 04");
    // else if (color == "Green")
    //     system("color 02");
    // else if (color == "Yellow")
    //     system("color 06");
    // else if (color == "Blue")
    //     system("color 01");
    // else if (color == "Magenta")
    //     system("color 05");
    // else if (color == "Cyan")
    //     system("color 03");
    // else if (color == "White")
    //     system("color 07");
    // else if (color == "Black")
    //     system("color 00");
    // return "";
}

string resetFormat()
{
    //*nix
    return "\033[0m";
    //win
    //system("color 07");
    //return "";
}
void gmInit(Ingredient(&arrI)[INGREDIENT], Burger(&arrB)[BURGER])
{
    //init ingredient
    arrI[0].set("Bread", 'b', "[B]read");
    arrI[1].set("Cheese", 'c', "[C]heese");
    arrI[2].set("Beef", 'b', "Bee[f]");
    arrI[3].set("Lettuce", 'l', "[L]ettuce");
    arrI[4].set("Tomato", 't', "[T]omato");
    arrI[5].set("Mushroom", 'm', "[M]ushroom");
    arrI[6].set("Egg", 'e', "[E]gg");
    arrI[7].set("Salmon", 's', "[S]almon");
    //init burger
    arrB[0].set("Cheese burger", 10, "bcflb", 10);
    arrB[1].set("Beef burger", 10, "bctflb", 10);
    arrB[2].set("Mushroom burger", 15, "bcfmb", 10);
    arrB[3].set("Veggie burger", 10, "btelb", 10);
    arrB[4].set("Salmon burger", 15, "bcfsb", 10);
}
void displayIntro()
{
    cout << setFormat("Yellow", 1) << "This is the welcome msg" << resetFormat() << endl;
    //welcome msg designed by group
}

void displayMenu()
{
    cout << "*** Game Menu ***" << endl;
    cout << setFormat("Yellow", 0) << "[1] Start Game" << resetFormat() << endl;
    cout << "[2] Settings" << endl;
    cout << "[3] Burger Menus" << endl;
    cout << "[4] Instructions" << endl;
    cout << "[5] Credits" << endl;
    cout << "[6] Exit" << endl;
    cout << "*****************" << endl;
    cout << "Option (1 - 6): ";
}

void dispMsg(string st) { cout << "   !!! " << st << " !!!" << endl; }
