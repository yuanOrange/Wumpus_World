// ======================================================================
// FILE:        MyAI.cpp
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

#include "MyAI.hpp"

MyAI::MyAI() : Agent()
{

    goldFound = false;
    goldLooted = false;
    wumpusLocated = false;
    wumpusKilled = false;
    targetSet = false;
    arrowShot = false;
    currCost = 0;
    agentDir = 0;  // 0 - right, 1 - down, 2 - left, 3 - up
    agentCoord.x = 0;
    agentCoord.y = 0;
    targetCoord.x = 0;
    targetCoord.y = 0;
    wumpusCoord.x = -1;
    wumpusCoord.y = -1;
    colNum = 10;
    rowNum = 10;
    lastAction = CLIMB;

    map = new GRID*[colNum];
    for(int i=0; i<colNum; i++){
        map[i] = new GRID[rowNum];
    }

    for(int i=0; i<colNum; i++){
        for(int j=0; j<rowNum; j++){
            map[i][j].visited = false;
            map[i][j].wumpusPb = U_PB;
            map[i][j].pitPb = U_PB;
            map[i][j].wumpusCf = 0;
            map[i][j].dijChecked = false;
            map[i][j].dijCost = 1001;
            map[i][j].dijDir = 4;
            map[i][j].dijNextNode.x = -1;
            map[i][j].dijNextNode.y = -1;
        }
    }

    map[0][0].wumpusPb = N_PB;
    map[0][0].pitPb = N_PB;

}


Agent::Action MyAI::getAction(bool stench, bool breeze, bool glitter, bool bump, bool scream)
{

    currCost ++;

    if(!map[agentCoord.x][agentCoord.y].visited || lastAction == SHOOT)
    {
        detectNode(stench, breeze, glitter, bump, scream);
    }

    return goToTarget();

}

void MyAI::detectNode(bool stench, bool breeze, bool glitter, bool bump, bool scream)
{
    map[agentCoord.x][agentCoord.y].visited = true;
    deleteListItem(safeUnvList, agentCoord);

    if(bump){
        detectBump();
        targetSet = false;
        return;
    }
    
    if(agentCoord.x == targetCoord.x && agentCoord.y == targetCoord.y){
        targetSet = false;
    }

    if(glitter){
        goldFound = true;
    }

    if(lastAction == SHOOT){
        if(scream){
            wumpusKilled = true;

            for(auto it = wumpusList.begin(); it != wumpusList.end(); it++){
                    if(map[it->x][it->y].pitPb == N_PB){
                        safeUnvList.push_back(*it);
                    }
            }

            wumpusList.clear();

        }
        else{
            for(auto it = wumpusList.begin(); it != wumpusList.end(); it++){
                if(targetDir(*it, agentCoord, agentDir) == 0){
                    if(map[it->x][it->y].pitPb == N_PB){
                        safeUnvList.push_back(*it);
                    }
                    deleteListItem(wumpusList, *it);
                    break;
                }
            }
        }

        targetSet = false;
        lastAction = CLIMB;
    }

    detectStenchAndBreeze(stench, breeze);

}

void MyAI::detectBump()
{
    if(agentDir == 0){
        agentCoord.x -= 1;
        colNum = agentCoord.x + 1;
        for(auto it = safeUnvList.begin(); it != safeUnvList.end(); ){
            if(it->x >= colNum){
                it = safeUnvList.erase(it);
            }
            else{
                it ++;
            }
        }
        for(auto it = wumpusList.begin(); it != wumpusList.end(); ){
            if(it->x >= colNum){
                it = wumpusList.erase(it);
            }
            else{
                it ++;
            }
        }
    }
    if(agentDir == 3){
        agentCoord.y -= 1;
        rowNum = agentCoord.y + 1;
        for(auto it = safeUnvList.begin(); it != safeUnvList.end(); ){
            if(it->y >= rowNum){
                it = safeUnvList.erase(it);
            }
            else{
                it ++;
            }
        }
        for(auto it = wumpusList.begin(); it != wumpusList.end(); ){
            if(it->y >= rowNum){
                it = wumpusList.erase(it);
            }
            else{
                it ++;
            }
        }
    }
}

void MyAI::detectStenchAndBreeze(bool stench, bool breeze)
{
    int i;
    COORD agentNeib[4];
    neighbor(agentNeib, agentCoord);

    if(!wumpusLocated)
    {
        if(stench){
            for(i = 0; i < 4; i++){
                if( isInBounds(agentNeib[i]) ){
                    if(map[agentNeib[i].x][agentNeib[i].y].wumpusPb == Y_PB){
                        map[agentNeib[i].x][agentNeib[i].y].wumpusCf += 1;
                    }
                    if(map[agentNeib[i].x][agentNeib[i].y].wumpusPb == U_PB){
                        map[agentNeib[i].x][agentNeib[i].y].wumpusPb = Y_PB;
                        map[agentNeib[i].x][agentNeib[i].y].wumpusCf += 1;
                        wumpusList.push_back(agentNeib[i]);
                    }

                }
            }
        }
        else{
            for(i = 0; i < 4; i++){
                if( isInBounds(agentNeib[i]) && map[agentNeib[i].x][agentNeib[i].y].wumpusPb != N_PB ){
                    map[agentNeib[i].x][agentNeib[i].y].wumpusPb = N_PB;
                    deleteListItem(wumpusList, agentNeib[i]);
                }
            }
        }

        if(wumpusList.size() == 1){
            wumpusCoord.x = wumpusList.at(0).x;
            wumpusCoord.y = wumpusList.at(0).y;
            wumpusLocated = true;
        }
        if(!wumpusLocated){
            COORD node = highWumpusCfNode();
            if(node.x != -1){
                wumpusCoord.x = node.x;
                wumpusCoord.y = node.y;
                wumpusLocated = true;
		wumpusList.clear();
		wumpusList.push_back(wumpusCoord);
            }
        }
    }


    if(breeze){
        for(i = 0; i < 4; i++){
            if( isInBounds(agentNeib[i]) && map[agentNeib[i].x][agentNeib[i].y].pitPb == U_PB ){
                map[agentNeib[i].x][agentNeib[i].y].pitPb = Y_PB;
            }
        }
    }
    else{
        for(i = 0; i < 4; i++){
            if( isInBounds(agentNeib[i]) && map[agentNeib[i].x][agentNeib[i].y].pitPb != N_PB ){
                map[agentNeib[i].x][agentNeib[i].y].pitPb = N_PB;
            }
        }
    }


    for(i = 0; i < 4; i++){
        if( isInBounds(agentNeib[i]) && !isInList(safeUnvList, agentNeib[i]) ){
            if( wumpusLocated
                && !isInList(wumpusList, agentNeib[i])
                && !map[agentNeib[i].x][agentNeib[i].y].visited
                && map[agentNeib[i].x][agentNeib[i].y].pitPb == N_PB )
            {
                safeUnvList.push_back(agentNeib[i]);
            }
            if( !wumpusLocated
                && !map[agentNeib[i].x][agentNeib[i].y].visited
                && map[agentNeib[i].x][agentNeib[i].y].pitPb == N_PB
                && map[agentNeib[i].x][agentNeib[i].y].wumpusPb == N_PB )
            {
                safeUnvList.push_back(agentNeib[i]);
            }
        }
    }

}

void MyAI::deleteListItem(std::vector<COORD> &list, COORD node)
{
    for(auto it = list.begin(); it != list.end(); ){
        if( it->x == node.x && it->y == node.y){
            it = list.erase(it);
        }
        else{
            it++;
        }
    }
}

void MyAI::dijkstra()
{
    for(int i = 0; i < colNum; i++){
        for(int j = 0; j < rowNum; j++){
            map[i][j].dijChecked = false;
            map[i][j].dijCost = 1001;
            map[i][j].dijDir = 4;
            map[i][j].dijNextNode.x = -1;
            map[i][j].dijNextNode.y = -1;
        }
    }

    map[agentCoord.x][agentCoord.y].dijChecked = true;
    map[agentCoord.x][agentCoord.y].dijCost = 0;
    map[agentCoord.x][agentCoord.y].dijDir = agentDir;


    COORD node = agentCoord;
    while(node.x != -1 || node.y != -1){
        setCost(node);
        node = leastCostNode();
    }
}

void MyAI::findTarget()
{
    dijkstra();

    targetCoord = agentCoord;

    if(safeUnvList.empty() && !arrowShot){
        if(wumpusList.size() == 1 && map[wumpusCoord.x][wumpusCoord.y].pitPb == N_PB){
            int min = 1001;
            for(int y = 0; y < rowNum; y ++){
                if(map[wumpusCoord.x][y].visited && map[wumpusCoord.x][y].dijCost < min){
                    min = map[wumpusCoord.x][y].dijCost;
                    targetCoord.x = wumpusCoord.x;
                    targetCoord.y = y;
                }
            }
            for(int x = 0; x < colNum; x ++){
                if(map[x][wumpusCoord.y].visited && map[x][wumpusCoord.y].dijCost < min){
                    min = map[x][wumpusCoord.y].dijCost;
                    targetCoord.x = x;
                    targetCoord.y = wumpusCoord.y;
                }
            }
            wumpusLocated = true;
        }

        if(wumpusList.size() >= 2){
            for(auto it = wumpusList.begin(); it != wumpusList.end(); it++){
                if(map[it->x][it->y].pitPb == N_PB){
                    COORD wumpusCandidate = *it;
                    int min = 1001;
                    for(int y = 0; y < rowNum; y ++){
                        if(map[wumpusCandidate.x][y].visited && map[wumpusCandidate.x][y].dijCost < min){
                            min = map[wumpusCandidate.x][y].dijCost;
                            targetCoord.x = wumpusCandidate.x;
                            targetCoord.y = y;
                        }
                    }
                    for(int x = 0; x < colNum; x ++){
                        if(map[x][wumpusCandidate.y].visited && map[x][wumpusCandidate.y].dijCost < min){
                            min = map[x][wumpusCandidate.y].dijCost;
                            targetCoord.x = x;
                            targetCoord.y = wumpusCandidate.y;
                        }
                    }
                }
            }
            wumpusLocated = true;
        }

    }
    else{
        int min = 1001;
        for(auto it = safeUnvList.begin(); it != safeUnvList.end(); it++){
            if(min > map[it->x][it->y].dijCost - it->x - it->y){
                min = map[it->x][it->y].dijCost - it->x - it->y;
                targetCoord = *it;
            }
        }
    }

    if(targetCoord.x == agentCoord.x && targetCoord.y == agentCoord.y){
        targetCoord.x = 0;
        targetCoord.y = 0;
    }
    
    targetSet = true;
}

Agent::Action MyAI::goToTarget()
{
    if(goldFound && !goldLooted){
        goldLooted = true;
        targetSet = false;
        return GRAB;
    }
    
    if(goldLooted){
        if(!targetSet){
            dijkstra();
            targetCoord.x = 0;
            targetCoord.y = 0;
            targetSet = true;
            routeToTarget();
        }
        return takeRoute();
    }
    
    if(wumpusLocated && !arrowShot
       && targetDir(wumpusCoord, agentCoord, agentDir) != 4
       && map[wumpusCoord.x][wumpusCoord.y].pitPb == N_PB){
        targetSet = false;
        return shootWumpus();
    }

    if(!targetSet){
        findTarget();
        routeToTarget();
    }
    
    return takeRoute();
}

Agent::Action MyAI::goToNextNode(COORD node)
{
    int relativeDir = targetDir(node, agentCoord, agentDir);

    switch( relativeDir )
    {
        case 0: 					// same direction with agent
            switch(agentDir)
            {
                case 0:				// agent's direction is right
                    agentCoord.x += 1;
                    break;

                case 1:
                    agentCoord.y -= 1;
                    break;

                case 2:
                    agentCoord.x -= 1;
                    break;

                case 3:
                    agentCoord.y += 1;
                    break;
            }
            return FORWARD;

        case 1:
            return makeTurn(TURN_RIGHT);

        case 2:
            return makeTurn(TURN_RIGHT);

        case 3:
            return makeTurn(TURN_LEFT);

        default:
            return CLIMB;
    }
}

MyAI::COORD MyAI::highWumpusCfNode()
{
    int count = 0;
    COORD node;

    for(auto it = wumpusList.begin(); it != wumpusList.end(); it ++){
        if(map[it->x][it->y].wumpusCf >= 2){
            node = *it;
            count += 1;
        }
    }

    if(count == 1){
        return node;
    }
    else{
        node.x = -1;
        node.y = -1;
        return node;
    }
}

bool MyAI:: isInBounds(COORD target)
{
    return( target.x>=0 && target.x<colNum && target.y>=0 && target.y<rowNum );
}

bool MyAI::isInList(std::vector<COORD> &list, COORD node)
{
    if(list.empty()){
        return false;
    }
    for(auto it = list.begin(); it != list.end(); it++){
        if(it->x ==node.x && it->y == node.y){
            return true;
        }
    }
    return false;
}

MyAI::COORD MyAI::leastCostNode()
{
    int min = 1001;
    COORD res, test;
    res.x = -1;
    res.y = -1;
    for(int i = 0; i < colNum; i++){
        for(int j = 0; j < rowNum; j++){
            test.x = i;
            test.y = j;
            if( !map[i][j].dijChecked && (map[i][j].visited || isInList(safeUnvList, test)) ){
                if(min > map[i][j].dijCost){
                    min = map[i][j].dijCost;
                    res = test;
                }
            }
        }
    }
    return res;
}

Agent::Action MyAI::makeTurn(Agent::Action dirToTurn)
{
    if(dirToTurn == TURN_RIGHT){
        agentDir = (agentDir + 1) % 4;
        return TURN_RIGHT;
    }
    if(dirToTurn == TURN_LEFT){
        agentDir = (agentDir + 3) % 4;
        return TURN_LEFT;
    }
    return CLIMB;
}

void MyAI::neighbor(COORD* targetNeib, COORD target)
{
    targetNeib[0].x = target.x + 1;
    targetNeib[0].y = target.y;

    targetNeib[1].x = target.x;
    targetNeib[1].y = target.y - 1;

    targetNeib[2].x = target.x - 1;
    targetNeib[2].y = target.y;

    targetNeib[3].x = target.x;
    targetNeib[3].y = target.y + 1;

}

void MyAI::routeToTarget()
{
    COORD node = targetCoord;
    
    routeList.clear();

    while(node.x != agentCoord.x || node.y != agentCoord.y){
        routeList.push_back(node);
        node = map[node.x][node.y].dijNextNode;
    }
}

void MyAI::setCost(COORD currNode)
{
    COORD nodeNeib[4];
    neighbor(nodeNeib, currNode);

    for(int i = 0; i < 4; i++){
        if( isInBounds(nodeNeib[i])
            && (map[nodeNeib[i].x][nodeNeib[i].y].visited || isInList(safeUnvList, nodeNeib[i]))
            && !map[nodeNeib[i].x][nodeNeib[i].y].dijChecked
          )
        {
            int relativeDir = targetDir(nodeNeib[i], currNode, map[currNode.x][currNode.y].dijDir);
            switch(relativeDir)
            {
                case 0:
                    if(map[nodeNeib[i].x][nodeNeib[i].y].dijCost > map[currNode.x][currNode.y].dijCost + 1){
                        map[nodeNeib[i].x][nodeNeib[i].y].dijCost = map[currNode.x][currNode.y].dijCost + 1;
                        map[nodeNeib[i].x][nodeNeib[i].y].dijDir = map[currNode.x][currNode.y].dijDir;
                        map[nodeNeib[i].x][nodeNeib[i].y].dijNextNode = currNode;
                    }
                    break;

                case 1:
                    if(map[nodeNeib[i].x][nodeNeib[i].y].dijCost > map[currNode.x][currNode.y].dijCost + 2) {
                        map[nodeNeib[i].x][nodeNeib[i].y].dijCost = map[currNode.x][currNode.y].dijCost + 2;
                        map[nodeNeib[i].x][nodeNeib[i].y].dijDir = (map[currNode.x][currNode.y].dijDir + 1) % 4;
                        map[nodeNeib[i].x][nodeNeib[i].y].dijNextNode = currNode;
                    }
                    break;

                case 2:
                    if(map[nodeNeib[i].x][nodeNeib[i].y].dijCost > map[currNode.x][currNode.y].dijCost + 3) {
                        map[nodeNeib[i].x][nodeNeib[i].y].dijCost = map[currNode.x][currNode.y].dijCost + 3;
                        map[nodeNeib[i].x][nodeNeib[i].y].dijDir = (map[currNode.x][currNode.y].dijDir + 2) % 4;
                        map[nodeNeib[i].x][nodeNeib[i].y].dijNextNode = currNode;
                    }
                    break;

                case 3:
                    if(map[nodeNeib[i].x][nodeNeib[i].y].dijCost > map[currNode.x][currNode.y].dijCost + 2){
                        map[nodeNeib[i].x][nodeNeib[i].y].dijCost = map[currNode.x][currNode.y].dijCost + 2;
                        map[nodeNeib[i].x][nodeNeib[i].y].dijDir = (map[currNode.x][currNode.y].dijDir + 3) % 4;
                        map[nodeNeib[i].x][nodeNeib[i].y].dijNextNode = currNode;
                    }
                    break;

                default :
                    break;
            }
        }
    }
    map[currNode.x][currNode.y].dijChecked = true;
}

Agent::Action MyAI::shootWumpus()
{
    int wumpusRelativeDir = targetDir(wumpusCoord, agentCoord, agentDir);
    switch( wumpusRelativeDir )
    {
        case 0:
            lastAction = SHOOT;
            arrowShot = true;
            return SHOOT;

        case 1:
            return makeTurn(TURN_RIGHT);

        case 2:
            return makeTurn(TURN_RIGHT);

        case 3:
            return makeTurn(TURN_LEFT);

        default:
            return CLIMB;
    }
}

Agent::Action MyAI::takeRoute()
{
    if(agentCoord.x == 0 && agentCoord.y == 0)
    {
        if(goldLooted){
            return CLIMB;
        }

        if(targetCoord.x == 0 && targetCoord.y == 0){
            return CLIMB;
        }
    }

    COORD node = routeList.back();

    if(node.x == agentCoord.x && node.y == agentCoord.y){
        routeList.pop_back();
        node = routeList.back();
    }

    return goToNextNode(node);
}

int MyAI::targetDir(COORD target, COORD curr, int currDir)
{
    int absDir = 4;

    if(target.x > curr.x && target.y == curr.y){  // right
        absDir = 0;
    }
    if(target.y < curr.y && target.x == curr.x){  // down
        absDir = 1;
    }
    if(target.x < curr.x && target.y == curr.y){  // left
        absDir = 2;
    }
    if(target.y > curr.y && target.x == curr.x){  // up
        absDir = 3;
    }

    if(absDir == 4){
        return 4;
    }

    switch ((absDir - (currDir - 4)) % 4) {
        case 0:
            return 0;

        case 1:
            return 1;

        case 2:
            return 2;

        case 3:
            return 3;

        default:
            return 4;
    }
}
