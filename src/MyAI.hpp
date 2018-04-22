// ======================================================================
// FILE:        MyAI.hpp
//
// AUTHOR:      Abdullah Younis
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#ifndef MYAI_LOCK
#define MYAI_LOCK

#include "Agent.hpp"
#include <vector>

class MyAI : public Agent
{
public:
    MyAI ();

    Action getAction
            (
                    bool stench,
                    bool breeze,
                    bool glitter,
                    bool bump,
                    bool scream
            );

    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

private:

    typedef enum
    {
        Y_PB,
        N_PB,
        U_PB
    }POSB;

    typedef struct
    {
        int x;
        int y;
    }COORD;

    typedef struct
    {
        bool visited;       // visited or not
        POSB wumpusPb;
        POSB pitPb;
        int wumpusCf;
        bool dijChecked;
        int dijCost;
        int dijDir;
        COORD dijNextNode;
    }GRID;

    GRID** map;

    std::vector<COORD> safeUnvList;
    std::vector<COORD> routeList;
    std::vector<COORD> wumpusList;

    int currCost;
    bool goldFound;
    bool goldLooted;
    bool wumpusLocated;
    bool wumpusKilled;
    bool arrowShot;
    bool targetSet;
    int agentDir;
    COORD agentCoord;
    COORD wumpusCoord;
    COORD targetCoord;
    int colNum;
    int rowNum;
    Action lastAction;

    void detectNode(bool stench, bool breeze, bool glitter, bool bump, bool scream);
    void detectBump();
    void detectStenchAndBreeze(bool, bool);
    void findTarget();
    Action goToTarget();
    Action goToNextNode(COORD);
    Action shootWumpus();
    Action makeTurn(Action);
    void routeToTarget();
    Action takeRoute();
    int targetDir(COORD, COORD, int);
    void deleteListItem(std::vector<COORD> &list, COORD);
    bool isInBounds(COORD cd);
    bool isInList(std::vector<COORD> &list, COORD);
    void neighbor(COORD *, COORD);
    void dijkstra();
    void setCost(COORD);
    COORD leastCostNode();
    COORD highWumpusCfNode();
};

#endif
