#include <iostream>

#include <SDL_mixer.h>

#include "sdl2d.h"
#include "structs.h"



#define FINISHED_WITHOUT_ERRORS 0
#define NOT_ENOUGTH_ARGUMENTS -1
#define UNABLE_TO_LOAD_LEVEL_FILE -2

#define START_LIFES 5

#define ROCKET_DAMAGE 1
#define BOMB_DAMAGE 2

#define NEEDED_LOAD_ROCKET 150
#define NEEDED_LOAD_BOMB 300

#define MAX_STRING_LENGTH 30



using namespace std;



void addEnemySpawn(enemySpawnList **list, enemySpawn *element)
{
    if((*list) == NULL)
    {
        (*list) = new enemySpawnList;
        (*list)->before = NULL;
        (*list)->element = element;
        (*list)->next = NULL;
    }
    else
    {
        enemySpawnList *iterator = (*list);

        while(iterator->next != NULL)
            iterator = iterator->next;

        iterator->next = new enemySpawnList;
        iterator->next->before = iterator;
        iterator = iterator->next;
        iterator->next = NULL;
        iterator->element = element;
    }
}


level *loadLevelFile(const char *levelfile)
{
    FILE *f = NULL;

    if((f = fopen(levelfile, "r")) == NULL)
        return NULL;

    int width, height, bpp;
    char temp[MAX_STRING_LENGTH];

    fgets(temp, MAX_STRING_LENGTH, f);
    width = atoi(temp);
    fgets(temp, MAX_STRING_LENGTH, f);
    height = atoi(temp);
    fgets(temp, MAX_STRING_LENGTH, f);
    bpp = atoi(temp);
    fgets(temp, MAX_STRING_LENGTH, f);
    temp[strlen(temp) - 1] = '\0';

    level *newLevel = new level;
    newLevel->gameWindow = new Window("Copterdefense", width, height, bpp, SDL_HWSURFACE|SDL_DOUBLEBUF, temp);

    fgets(temp, MAX_STRING_LENGTH, f);
    temp[strlen(temp) - 1] = '\0';

    if(Mix_OpenAudio(22050, AUDIO_S16LSB, 2, 4096 ) == -1)
        printf("Failed to initiate SDL Mixer: %s\n", Mix_GetError());
    else
        newLevel->backgroundMusic = Mix_LoadMUS(temp);

    int enemyNumber;

    fgets(temp, MAX_STRING_LENGTH, f);

    enemyNumber = atoi(temp);

    enemySpawn *newEnemy = NULL;
    newLevel->enemys = NULL;

    for(int i = 0; i < enemyNumber; i++)
    {
        newEnemy = new enemySpawn;

        fscanf(f, "%d %d %d %d", (int *)&newEnemy->type, &newEnemy->spawnTime, &newEnemy->x, &newEnemy->y);

        addEnemySpawn(&newLevel->enemys, newEnemy);
    }

    fclose(f);

    return newLevel;
}


void addEnemy(enemyList **list, enemy *element)
{
    if((*list) == NULL)
    {
        (*list) = new enemyList;
        (*list)->before = NULL;
        (*list)->element = element;
        (*list)->next = NULL;
    }
    else
    {
        enemyList *iterator = (*list);

        while(iterator->next != NULL)
            iterator = iterator->next;

        iterator->next = new enemyList;
        iterator->next->before = iterator;
        iterator = iterator->next;
        iterator->next = NULL;
        iterator->element = element;
    }
}


void addObject2D(objectList **list, Object2D *obj)
{
    if((*list) == NULL)
    {
        (*list) = new objectList;
        (*list)->element = obj;
        (*list)->next = NULL;
        (*list)->before = NULL;

        return;
    }

    objectList *iterator = (*list);

    while(iterator->next != NULL)
        iterator = iterator->next;

    iterator->next = new objectList;
    iterator->next->before = iterator;
    iterator = iterator->next;
    iterator->element = obj;
    iterator->next = NULL;
}


void renderObjectList(objectList *list, Window *window)
{
    objectList *current = list;

    while(current != NULL)
    {
        window->renderObject2D(current->element);
        current = current->next;
    }
}


void renderEnemyList(enemyList *list, Window *window)
{
    enemyList *current = list;

    while(current != NULL)
    {
        window->renderObject2D(current->element->object);
        current = current->next;
    }
}


void removeObjectNode(objectList **list, objectList **node)
{
    objectList *newNode = NULL;

    if((*list) == (*node) && (*list)->next == NULL)
    {
        (*list) = NULL;
        newNode = NULL;
    }
    else if((*list) == (*node) && (*list)->next != NULL)
    {
        (*list) = (*list)->next;
        (*list)->before = NULL;
        newNode = (*list);
    }
    else if((*node)->next == NULL)
    {
        (*node)->before->next = NULL;
        newNode = NULL;
    }
    else
    {
        (*node)->before->next = (*node)->next;
        (*node)->next->before = (*node)->before;
        newNode = (*node)->next;
    }

    delete (*node)->element;
    delete (*node);

    (*node) = newNode;
}


void removeEnemyNode(enemyList **list, enemyList **node)
{
    enemyList *newNode = NULL;

    if((*list) == (*node) && (*list)->next == NULL)
    {
        (*list) = NULL;
        newNode = NULL;
    }
    else if((*list) == (*node) && (*list)->next != NULL)
    {
        (*list) = (*list)->next;
        (*list)->before = NULL;
        newNode = (*list);
    }
    else if((*node)->next == NULL)
    {
        (*node)->before->next = NULL;
        newNode = NULL;
    }
    else
    {
        (*node)->before->next = (*node)->next;
        (*node)->next->before = (*node)->before;
        newNode = (*node)->next;
    }

    delete (*node)->element;
    delete (*node);

    (*node) = newNode;
}


void addLife(ImageCache *imgcache, objectList **list, int *lifes)
{
    Object2D *life = new Object2D("images/life.bmp", imgcache, 32 + ((*lifes) * 48), 32);
    addObject2D(list, life);
    (*lifes)++;
}


int deleteLife(objectList **list, int *lifes)
{
    objectList *iterator = (*list);

    while(iterator != NULL)
    {
        if(iterator->element->getPosition().x == 32 + (((*lifes) - 1) * 48))
        {
            removeObjectNode(list, &iterator);
            (*lifes)--;

            return 0;
        }

        iterator = iterator->next;
    }

    cerr << "Warning: Tried to remove not existing live." << endl;

    return -1;
}


int spawnEnemys(enemySpawnList **spawns, enemyList **enemys, int start, ImageCache *imgcache)
{
    int number = 0;
    enemySpawnList *spawnIterator = (*spawns);

    while(spawnIterator != NULL)
    {
        if(spawnIterator->element->spawnTime < SDL_GetTicks() - start && spawnIterator->element->type != AlreadySpawned)
        {
            number++;

            enemy *newEnemy = new enemy;

            switch(spawnIterator->element->type)
            {
                case AlreadySpawned:

                break;

                case AlienAirMine:
                    newEnemy->lifes = 1;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/airmine.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienDownTentacle:
                    newEnemy->lifes = 2;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/tentacle.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienGiantTentacle:
                    newEnemy->lifes = 3;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/gianttentacle.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienUpTentacle:
                    newEnemy->lifes = 2;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/tentacleup.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienAirTransporter:
                    newEnemy->lifes = 2;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/airtransporter.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienGroundTransporter:
                    newEnemy->lifes = 2;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/groundtransporter.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienScout:
                    newEnemy->lifes = 1;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/scout.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienRocket:
                    newEnemy->lifes = 1;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/alienrocket.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienSeekingRocket:
                    newEnemy->lifes = 1;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/seekrocket.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienSapper:
                    newEnemy->lifes = 3;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/sapper.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienPlanter:
                    newEnemy->lifes = 5;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/planter.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienFighter:
                    newEnemy->lifes = 2;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/fighter.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienGunship:
                    newEnemy->lifes = 2;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/gunship.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienTank:
                    newEnemy->lifes = 4;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/tank.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienAirStation:
                    newEnemy->lifes = 3;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/airstation.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienGroundStation:
                    newEnemy->lifes = 5;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/groundstation.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienBomber:
                    newEnemy->lifes = 2;
                    newEnemy->collisionDamage = 2;
                    newEnemy->object = new Object2D("images/bomber.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienBulletLeft:
                    newEnemy->lifes = 1000;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/bullet.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienBulletRight:
                    newEnemy->lifes = 1000;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/bullet.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienBulletUp:
                    newEnemy->lifes = 1000;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/bullet.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case AlienBulletDown:
                    newEnemy->lifes = 1000;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/bullet.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case HumanSupply:
                    newEnemy->lifes = 2;
                    newEnemy->collisionDamage = 0;
                    newEnemy->object = new Object2D("images/supply.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;

                case HumanBox:
                    newEnemy->lifes = 2;
                    newEnemy->collisionDamage = 1;
                    newEnemy->object = new Object2D("images/box.bmp", imgcache, spawnIterator->element->x, spawnIterator->element->y);
                break;
            }

            newEnemy->type = spawnIterator->element->type;

            addEnemy(enemys, newEnemy);

            spawnIterator->element->type = AlreadySpawned;
        }

        spawnIterator = spawnIterator->next;
    }

    return number;
}


bool enemyListEmpty(enemyList *list)
{
    return (list == NULL);
}


bool enemySpawnListEmpty(enemySpawnList *list)
{
    while(list != NULL)
    {
        if(list->element->type != AlreadySpawned)
            return false;

        list = list->next;
    }

    return true;
}


void playLevel(level *stage)
{
    srand(SDL_GetTicks());

    ImageCache imgcache;

    Object2D *player = new Object2D("images/copter.bmp", &imgcache, 48, stage->gameWindow->getHeight() / 2);
    Object2D *pauseSymbol = new Object2D("images/pause.bmp", &imgcache, stage->gameWindow->getWidth() / 2, stage->gameWindow->getHeight() / 2);

    Mix_PlayMusic(stage->backgroundMusic, 0);

    objectList *bombs = NULL;
    objectList *rockets = NULL;
    objectList *lifeList = NULL;
    enemyList *attackers = NULL;

    bool done = false;
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool pause = false;

    int load_rocket = 0;
    int load_bomb = 0;
    int lifes = 0;

    for(int i = 0; i < START_LIFES; i++)
        addLife(&imgcache, &lifeList, &lifes);

    int start = SDL_GetTicks();
    int pstart = 0;
    int number = 0;

    while(!done)
    {
        stage->gameWindow->renderBackground();

        stage->gameWindow->renderObject2D(player);
        renderObjectList(bombs, stage->gameWindow);
        renderObjectList(rockets, stage->gameWindow);
        renderObjectList(lifeList, stage->gameWindow);
        renderEnemyList(attackers, stage->gameWindow);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    done = true;
                break;

                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            done = true;
                        break;

                        case SDLK_UP:
                            up = true;
                        break;

                        case SDLK_DOWN:
                            down = true;
                        break;

                        case SDLK_LEFT:
                            left = true;
                        break;

                        case SDLK_RIGHT:
                            right = true;
                        break;

                        case SDLK_p:
                            if(pause == false)
                            {
                                pause = true;
                                pstart = SDL_GetTicks();
                            }
                            else
                            {
                                pause = false;
                                start += (SDL_GetTicks() - pstart);
                            }
                        break;

                        case SDLK_RCTRL:
                            if(load_bomb == NEEDED_LOAD_BOMB)
                            {
                                addObject2D(&bombs, new Object2D("images/bomb.bmp", &imgcache, player->getPosition().x, player->getPosition().y + (player->getHeight() / 2 + 16)));
                                load_bomb = 0;
                            }
                        break;

                        case SDLK_SPACE:
                            if(load_rocket == NEEDED_LOAD_ROCKET)
                            {
                                addObject2D(&rockets, new Object2D("images/rocket.bmp", &imgcache, player->getPosition().x + (player->getWidth() / 2 + 16), player->getPosition().y));
                                load_rocket = 0;
                            }
                        break;

                        default:

                        break;
                    }
                break;

                case SDL_KEYUP:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_UP:
                            up = false;
                        break;

                        case SDLK_DOWN:
                            down = false;
                        break;

                        case SDLK_LEFT:
                            left = false;
                        break;

                        case SDLK_RIGHT:
                            right = false;
                        break;

                        default:

                        break;
                    }
                break;
            }
        }

        if(!pause)
        {
            number++;

            if(load_bomb < NEEDED_LOAD_BOMB)
                load_bomb++;

            if(load_rocket < NEEDED_LOAD_ROCKET)
                load_rocket++;

            spawnEnemys(&stage->enemys, &attackers, start, &imgcache);

            objectList *objectIterator = bombs;
            enemyList *currentEnemy = attackers;

            while(objectIterator != NULL)
            {
                bool finish = false;
                objectIterator->element->moveRelative(0, 3);

                if(objectIterator->element->getPosition().y > stage->gameWindow->getHeight())
                {
                    //objectIterator->element->setPosition(objectIterator->element->getPosition().x, 0);
                    removeObjectNode(&bombs, &objectIterator);
                    continue;
                }
                else if(stage->gameWindow->objectRectsOverlapSimple(objectIterator->element, player))
                {
                    removeObjectNode(&bombs, &objectIterator);

                    for(int i = 0; i < BOMB_DAMAGE; i++)
                        deleteLife(&lifeList, &lifes);

                    continue;
                }

                currentEnemy = attackers;

                while(currentEnemy != NULL)
                {
                    if(stage->gameWindow->objectRectsOverlapSimple(objectIterator->element, currentEnemy->element->object))
                    {
                        currentEnemy->element->lifes -= BOMB_DAMAGE;
                        removeObjectNode(&bombs, &objectIterator);
                        finish = true;
                        break;
                    }

                    currentEnemy = currentEnemy->next;
                }

                if(!finish)
                    objectIterator = objectIterator->next;
            }

            objectIterator = rockets;

            while(objectIterator != NULL)
            {
                bool finish = false;
                objectIterator->element->moveRelative(3, 0);

                if(objectIterator->element->getPosition().x > stage->gameWindow->getWidth())
                {
                    //objectIterator->element->setPosition(0, objectIterator->element->getPosition().y);
                    removeObjectNode(&rockets, &objectIterator);
                    continue;
                }
                else if(stage->gameWindow->objectRectsOverlapSimple(objectIterator->element, player))
                {
                    removeObjectNode(&rockets, &objectIterator);

                    for(int i = 0; i < ROCKET_DAMAGE; i++)
                        deleteLife(&lifeList, &lifes);

                    continue;
                }

                currentEnemy = attackers;

                while(currentEnemy != NULL)
                {
                    if(stage->gameWindow->objectRectsOverlapSimple(objectIterator->element, currentEnemy->element->object))
                    {
                        currentEnemy->element->lifes -= ROCKET_DAMAGE;
                        removeObjectNode(&rockets, &objectIterator);
                        finish = true;
                        break;
                    }

                    currentEnemy = currentEnemy->next;
                }

                if(!finish)
                    objectIterator = objectIterator->next;
            }

            currentEnemy = attackers;

            while(currentEnemy != NULL)
            {
                if(currentEnemy->element->lifes <= 0 || currentEnemy->element->object->getPosition().x <= currentEnemy->element->object->getWidth() / 2 || currentEnemy->element->object->getPosition().y <= currentEnemy->element->object->getHeight() / 2|| currentEnemy->element->object->getPosition().y >= stage->gameWindow->getHeight() - currentEnemy->element->object->getHeight() / 2 || currentEnemy->element->object->getPosition().x >= stage->gameWindow->getWidth() - currentEnemy->element->object->getWidth() / 2)
                {
                    removeEnemyNode(&attackers, &currentEnemy);
                    continue;
                }
                else if(stage->gameWindow->objectRectsOverlapSimple(currentEnemy->element->object, player))
                {
                    if(currentEnemy->element->type == HumanSupply)
                        addLife(&imgcache, &lifeList, &lifes);

                    for(int i = 0; i < currentEnemy->element->collisionDamage; i++)
                        deleteLife(&lifeList, &lifes);

                    removeEnemyNode(&attackers, &currentEnemy);
                    continue;
                }

                enemy *secundaryEnemy = NULL;

                switch(currentEnemy->element->type)
                {
                    case AlienAirTransporter:
                        if(number % 2 == 0)
                            currentEnemy->element->object->moveRelative(-1, 0);
                    break;

                    case AlienGroundTransporter:
                        if(number % 2 == 0)
                            currentEnemy->element->object->moveRelative(-1, 0);
                    break;

                    case AlienScout:
                        currentEnemy->element->object->moveRelative(-1, 0);
                        currentEnemy->element->object->moveRelativeWithBorder(0, (number % 150) < 75 ? 2 : -2, stage->gameWindow->getSize());

                        if(currentEnemy->element->object->getPosition().y <= currentEnemy->element->object->getHeight() / 2)
                            currentEnemy->element->object->moveRelative(0, 3);

                        if(currentEnemy->element->object->getPosition().y >= stage->gameWindow->getHeight() - (currentEnemy->element->object->getHeight() / 2))
                            currentEnemy->element->object->moveRelative(0, -3);
                    break;

                    case AlienRocket:
                        currentEnemy->element->object->moveRelative(-1, 0);
                    break;

                    case AlienSeekingRocket:
                        currentEnemy->element->object->moveRelative(-1, 0);
                        currentEnemy->element->object->moveRelative(0, currentEnemy->element->object->getPosition().y < player->getPosition().y ? 1 : -1);
                    break;

                    case AlienSapper:
                        if(number % 2 == 0)
                            currentEnemy->element->object->moveRelative(-1, 0);

                        if(number % 400 == 0)
                        {
                            secundaryEnemy = new enemy;
                            secundaryEnemy->collisionDamage = 1;
                            secundaryEnemy->lifes = 1;
                            secundaryEnemy->type = AlienAirMine;
                            secundaryEnemy->object = new Object2D("images/airmine.bmp", &imgcache, currentEnemy->element->object->getPosition().x + 80, currentEnemy->element->object->getPosition().y);
                            addEnemy(&attackers, secundaryEnemy);
                        }
                    break;

                    case AlienPlanter:
                        if(number % 2 == 0)
                            currentEnemy->element->object->moveRelative(-1, 0);

                        if(number % 400 == 0)
                        {
                            secundaryEnemy = new enemy;
                            secundaryEnemy->collisionDamage = 1;
                            secundaryEnemy->lifes = 2;
                            secundaryEnemy->type = AlienDownTentacle;
                            secundaryEnemy->object = new Object2D("images/tentacle.bmp", &imgcache, currentEnemy->element->object->getPosition().x + 80, currentEnemy->element->object->getPosition().y + 16);
                            addEnemy(&attackers, secundaryEnemy);
                        }
                    break;

                    case AlienFighter:
                        if(number % 2 == 0)
                            currentEnemy->element->object->moveRelative(-1, currentEnemy->element->object->getPosition().y < player->getPosition().y ? 1 : -1);

                        if(currentEnemy->element->object->getPosition().y <= currentEnemy->element->object->getHeight() / 2)
                            currentEnemy->element->object->moveRelative(0, 2);

                        if(currentEnemy->element->object->getPosition().y >= stage->gameWindow->getHeight() - (currentEnemy->element->object->getHeight() / 2))
                            currentEnemy->element->object->moveRelative(0, -2);

                        if(number % 300 == 0)
                        {
                            secundaryEnemy = new enemy;
                            secundaryEnemy->collisionDamage = 1;
                            secundaryEnemy->lifes = 1000;
                            secundaryEnemy->type = AlienBulletLeft;
                            secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x - (currentEnemy->element->object->getWidth() / 2 + 16), currentEnemy->element->object->getPosition().y);
                            addEnemy(&attackers, secundaryEnemy);
                        }
                    break;

                    case AlienGunship:
                        if(number % 2 == 0)
                            currentEnemy->element->object->moveRelative(-1, 0);

                        if(number % 300 == 0)
                        {
                            secundaryEnemy = new enemy;
                            secundaryEnemy->collisionDamage = 1;
                            secundaryEnemy->lifes = 1000;
                            secundaryEnemy->type = AlienBulletLeft;
                            secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x - (currentEnemy->element->object->getWidth() / 2 + 16), currentEnemy->element->object->getPosition().y);
                            addEnemy(&attackers, secundaryEnemy);
                        }
                    break;

                    case AlienTank:
                        if(number % 2 == 0)
                            currentEnemy->element->object->moveRelative(-1, 0);

                        if(number % 300 == 0)
                        {
                            secundaryEnemy = new enemy;
                            secundaryEnemy->collisionDamage = 1;
                            secundaryEnemy->lifes = 1000;
                            secundaryEnemy->type = AlienBulletUp;
                            secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x, currentEnemy->element->object->getPosition().y - (currentEnemy->element->object->getHeight() / 2 + 16));
                            addEnemy(&attackers, secundaryEnemy);
                        }
                    break;

                    case AlienAirStation:
                        if(number % 100 == 0)
                        {
                            switch(rand() % 4)
                            {
                                case 0:
                                    secundaryEnemy = new enemy;
                                    secundaryEnemy->collisionDamage = 1;
                                    secundaryEnemy->lifes = 1000;
                                    secundaryEnemy->type = AlienBulletUp;
                                    secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x, currentEnemy->element->object->getPosition().y - (currentEnemy->element->object->getHeight() / 2 + 16));
                                    addEnemy(&attackers, secundaryEnemy);
                                break;

                                case 1:
                                    secundaryEnemy = new enemy;
                                    secundaryEnemy->collisionDamage = 1;
                                    secundaryEnemy->lifes = 1000;
                                    secundaryEnemy->type = AlienBulletRight;
                                    secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x + (currentEnemy->element->object->getWidth() / 2 + 16), currentEnemy->element->object->getPosition().y);
                                    addEnemy(&attackers, secundaryEnemy);
                                break;

                                case 2:
                                    secundaryEnemy = new enemy;
                                    secundaryEnemy->collisionDamage = 1;
                                    secundaryEnemy->lifes = 1000;
                                    secundaryEnemy->type = AlienBulletDown;
                                    secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x, currentEnemy->element->object->getPosition().y + (currentEnemy->element->object->getHeight() / 2) + 16);
                                    addEnemy(&attackers, secundaryEnemy);
                                break;

                                case 3:
                                    secundaryEnemy = new enemy;
                                    secundaryEnemy->collisionDamage = 1;
                                    secundaryEnemy->lifes = 1000;
                                    secundaryEnemy->type = AlienBulletLeft;
                                    secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x - (currentEnemy->element->object->getWidth() / 2 + 16), currentEnemy->element->object->getPosition().y);
                                    addEnemy(&attackers, secundaryEnemy);
                                break;
                            }
                        }
                    break;

                    case AlienGroundStation:
                                                if(number % 100 == 0)
                        {
                            switch(rand() % 3)
                            {
                                case 0:
                                    secundaryEnemy = new enemy;
                                    secundaryEnemy->collisionDamage = 1;
                                    secundaryEnemy->lifes = 1000;
                                    secundaryEnemy->type = AlienBulletUp;
                                    secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x, currentEnemy->element->object->getPosition().y - (currentEnemy->element->object->getHeight() / 2 + 16));
                                    addEnemy(&attackers, secundaryEnemy);
                                break;

                                case 1:
                                    secundaryEnemy = new enemy;
                                    secundaryEnemy->collisionDamage = 1;
                                    secundaryEnemy->lifes = 1000;
                                    secundaryEnemy->type = AlienBulletRight;
                                    secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x + (currentEnemy->element->object->getWidth() / 2 + 16), currentEnemy->element->object->getPosition().y);
                                    addEnemy(&attackers, secundaryEnemy);
                                break;

                                case 2:
                                    secundaryEnemy = new enemy;
                                    secundaryEnemy->collisionDamage = 1;
                                    secundaryEnemy->lifes = 1000;
                                    secundaryEnemy->type = AlienBulletLeft;
                                    secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x - (currentEnemy->element->object->getWidth() / 2 + 16), currentEnemy->element->object->getPosition().y);
                                    addEnemy(&attackers, secundaryEnemy);
                                break;
                            }
                        }
                    break;

                    case AlienBomber:
                        if(number % 2 == 0)
                            currentEnemy->element->object->moveRelative(-1, -1);

                        if(currentEnemy->element->object->getPosition().y <= currentEnemy->element->object->getHeight() / 2)
                            currentEnemy->element->object->moveRelative(0, 2);

                        if(number % 300 == 0)
                        {
                            secundaryEnemy = new enemy;
                            secundaryEnemy->collisionDamage = 1;
                            secundaryEnemy->lifes = 1000;
                            secundaryEnemy->type = AlienBulletDown;
                            secundaryEnemy->object = new Object2D("images/bullet.bmp", &imgcache, currentEnemy->element->object->getPosition().x, currentEnemy->element->object->getPosition().y + (currentEnemy->element->object->getHeight() / 2 + 16));
                            addEnemy(&attackers, secundaryEnemy);
                        }
                    break;

                    case AlienBulletLeft:
                        currentEnemy->element->object->moveRelative(-2, 0);
                    break;

                    case AlienBulletRight:
                        currentEnemy->element->object->moveRelative(2, 0);
                    break;

                    case AlienBulletUp:
                        currentEnemy->element->object->moveRelative(0, -2);
                    break;

                    case AlienBulletDown:
                        currentEnemy->element->object->moveRelative(0, 2);
                    break;

                    case HumanSupply:
                        if(number % 2 == 0)
                            currentEnemy->element->object->moveRelative(0, 1);
                    break;

                    default:

                    break;
                }

                currentEnemy = currentEnemy->next;
            }

            if(up)
                player->moveRelativeWithBorder(0, -2, stage->gameWindow->getWidth(), stage->gameWindow->getHeight());

            if(down)
                player->moveRelativeWithBorder(0, 2, stage->gameWindow->getWidth(), stage->gameWindow->getHeight());

            if(left)
                player->moveRelativeWithBorder(-2, 0, stage->gameWindow->getWidth(), stage->gameWindow->getHeight());

            if(right)
                player->moveRelativeWithBorder(2, 0, stage->gameWindow->getWidth(), stage->gameWindow->getHeight());
        }
        else
        {
            stage->gameWindow->renderObject2D(pauseSymbol);
        }

        if(lifes <= 0)
        {
            printf("You loose!\n");
            done = true;
        }

        if(enemyListEmpty(attackers) && enemySpawnListEmpty(stage->enemys))
        {
            printf("You win!\n");
            done = true;
        }

        stage->gameWindow->flipBuffers();
    }

    delete stage->gameWindow;
    delete player;
    delete pauseSymbol;

    Mix_FreeMusic(stage->backgroundMusic);
    Mix_CloseAudio();
}


int main (int argc, char **argv)
{
    if(argc > 2)
    {
        cerr << "Warning: Too many arguments (" << argc - 1 << " got, 1 expected)." << endl;
        cerr << argc - 2 << " will be ignored." << endl;
    }
    else if(argc < 2)
    {
        cerr << "Critical error: Not enougth arguments (0 got, 1 expected)." << endl;
        cerr << "Program will stop execution." << endl;
        return NOT_ENOUGTH_ARGUMENTS;
    }

    level *scene = NULL;

    if((scene = loadLevelFile(argv[1])) == NULL)
    {
        cerr << "Critical error: Unable to load level file." << endl;
        cerr << "Program will stop execution." << endl;
        return UNABLE_TO_LOAD_LEVEL_FILE;
    }

    if(scene->backgroundMusic == NULL)
    {
        cerr << "Error: Unable to load background music " << scene->backgroundMusic << endl;
    }

    playLevel(scene);

    return FINISHED_WITHOUT_ERRORS;
}
