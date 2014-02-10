#include <cstdio>
#include <cstdlib>

#include <SDL_mixer.h>

#include "sdl2d.h"



#define WIDTH 800
#define HEIGHT 600

#define ROCKET_SPAWN_BASE 175
#define SPAWN_ENLARGEMENT 2500

#define ROCKET_SPAWN (ROCKET_SPAWN_BASE - ((number > ROCKET_SPAWN_BASE * SPAWN_ENLARGEMENT - 1 ? ROCKET_SPAWN_BASE * SPAWN_ENLARGEMENT - 1 : number) / SPAWN_ENLARGEMENT))
#define MEDIPACK_SPAWN (ROCKET_SPAWN * 60)
#define GUIDED_ROCKET_SPAWN (ROCKET_SPAWN * 10)

#define GUIDE_AMOUNT 3

#define COLLISION_CHECK objectRectsOverlapSimple

#define RANDOM_SEED TRUE_RANDOM
#define TRUE_RANDOM SDL_GetTicks()
#define CONSTANT_RANDOM 13091998

#define START_LIFES 5

#define EXPLOSION_LIFETIME 500

#define MAX_IMAGENAME_LENGTH 30

#define BACKGROUND_IMAGE_CHANGE_DENSITY 10000
#define BACKGROUND_IMAGE_CHANGE_1 (BACKGROUND_IMAGE_CHANGE_DENSITY * 1)
#define BACKGROUND_IMAGE_CHANGE_2 (BACKGROUND_IMAGE_CHANGE_DENSITY * 2)
#define BACKGROUND_IMAGE_CHANGE_3 (BACKGROUND_IMAGE_CHANGE_DENSITY * 3)
#define BACKGROUND_IMAGE_CHANGE_4 (BACKGROUND_IMAGE_CHANGE_DENSITY * 4)
#define BACKGROUND_IMAGE_CHANGE_5 (BACKGROUND_IMAGE_CHANGE_DENSITY * 5)



struct Object2DList
{
    Object2D *element;
    Object2DList *next;
};



Object2DList *first = NULL;
int lifes = 0;
int highscorePoints[10];
char *highscoreNames[10];

char playerimg[MAX_IMAGENAME_LENGTH];
char bulletimg[MAX_IMAGENAME_LENGTH];
char guidedbulletimg[MAX_IMAGENAME_LENGTH];
char lifeimg[MAX_IMAGENAME_LENGTH];
char medipackimg[MAX_IMAGENAME_LENGTH];
char pauseimg[MAX_IMAGENAME_LENGTH];
char explosionimg[MAX_IMAGENAME_LENGTH];
char background1img[MAX_IMAGENAME_LENGTH];
char background2img[MAX_IMAGENAME_LENGTH];
char background3img[MAX_IMAGENAME_LENGTH];
char background4img[MAX_IMAGENAME_LENGTH];
char background5img[MAX_IMAGENAME_LENGTH];
char background6img[MAX_IMAGENAME_LENGTH];



int cleanObject2DList()
{
    int counter = 0;

    Object2DList *currentElement = first;
    Object2DList *nextElement = first->next;

    while(currentElement != NULL)
    {
        nextElement = currentElement->next;
        delete currentElement->element;
        delete currentElement;
        currentElement = nextElement;
        counter++;
    }

    return counter;
}


void renderObject2DList(Window *window)
{
    window->renderBackground();

    Object2DList *current = first;

    while(current != NULL)
    {
        window->renderObject2D(current->element);
        current = current->next;
    }

    window->flipBuffers();
}


void addObject2D(Object2D *obj)
{
    if(first == NULL)
    {
        first = new Object2DList;
        first->element = obj;
        first->next = NULL;

        return;
    }

    Object2DList *iterator = first;

    while(iterator->next != NULL)
        iterator = iterator->next;

    iterator->next = new Object2DList;
    iterator = iterator->next;
    iterator->element = obj;
    iterator->next = NULL;
}


void addLife(ImageCache *imgcache)
{
    Object2D *life = new Object2D(lifeimg, imgcache, 32 + (lifes * 48), 32);
    addObject2D(life);
    lifes++;
}


int deleteLife()
{
    Object2DList *currentElement = first;
    Object2DList *nextElement = NULL;

    if(first == NULL)
    {
        printf("there are no lifes to delete.\n");
        return -1;
    }
    else if(first->element->getPosition().x == 32 + ((lifes - 1) * 48) && nextElement->element->getPosition().y == 32 && strncmp(lifeimg, first->element->getName(), strlen(lifeimg)) == 0)
    {
        nextElement = first->next;

        delete first->element;
        delete first;

        first = nextElement;

        lifes--;

        return 0;
    }

    while(currentElement->next != NULL)
    {
        nextElement = currentElement->next;

        if(nextElement->element->getPosition().x == 32 + ((lifes - 1) * 48) && nextElement->element->getPosition().y == 32 && strncmp(lifeimg, nextElement->element->getName(), strlen(lifeimg)) == 0)
        {
            nextElement = nextElement->next;

            delete currentElement->next->element;
            delete currentElement->next;

            currentElement->next = nextElement;

            lifes--;

            return 0;
        }

        currentElement = nextElement;
    }

    printf("there are no lifes to delete.\n");

    return -1;
}


void addBullet(ImageCache *cache)
{
    Object2D *bullet = new Object2D(bulletimg, cache, (rand() % (WIDTH - 64)) + 32, 32);
    addObject2D(bullet);
}


void addMedipack(ImageCache *cache)
{
    Object2D *medipack = new Object2D(medipackimg, cache, (rand() % (WIDTH - 64)) + 32, 32);
    addObject2D(medipack);
}


void addGuidedBullet(ImageCache *cache)
{
    Object2D *guidedBullet = new Object2D(guidedbulletimg, cache, (rand() % (WIDTH - 64)) + 32, 32);
    addObject2D(guidedBullet);
}


void addPauseSymbol(ImageCache *cache, coords2D pos)
{
    Object2D *pauseSymbol = new Object2D(pauseimg, cache, pos);
    addObject2D(pauseSymbol);
}


int removePauseSymbol()
{
    Object2DList *currentElement = first;
    Object2DList *nextElement = NULL;

    if(first == NULL)
    {
        printf("there is no pause symbol to delete.\n");
        return -1;
    }
    else if(strncmp(pauseimg, first->element->getName(), strlen(pauseimg) == 0))
    {
        nextElement = first->next;

        delete first->element;
        delete first;

        first = nextElement;

        return 0;
    }

    while(currentElement->next != NULL)
    {
        nextElement = currentElement->next;

        if(strncmp(pauseimg, nextElement->element->getName(), strlen(pauseimg)) == 0)
        {
            nextElement = nextElement->next;

            delete currentElement->next->element;
            delete currentElement->next;

            currentElement->next = nextElement;

            return 0;
        }

        currentElement = nextElement;
    }

    printf("there is no pause symbol to delete.\n");

    return -1;
}


void addExplosion(ImageCache *cache, coords2D pos)
{
    Object2D *explosion = new Object2D(explosionimg, cache, pos);
    addObject2D(explosion);
}


int removeExplosions()
{
    Object2DList *currentElement = first;
    Object2DList *nextElement = NULL;

    if(first == NULL)
    {
        return -1;
    }
    else if(strncmp(explosionimg, first->element->getName(), strlen(explosionimg) == 0))
    {
        nextElement = first->next;

        delete first->element;
        delete first;

        first = nextElement;

        return 0;
    }

    while(currentElement->next != NULL)
    {
        nextElement = currentElement->next;

        if(strncmp(explosionimg, nextElement->element->getName(), strlen(explosionimg)) == 0)
        {
            nextElement = nextElement->next;

            delete currentElement->next->element;
            delete currentElement->next;

            currentElement->next = nextElement;

            return 0;
        }

        currentElement = nextElement;
    }

    return -1;
}


void printHighscore()
{
    for(int i = 0; i < 10; i++)
        printf("%2d. %10d %s\n", i+1, highscorePoints[i], highscoreNames[i]);
}


int main (int argc, char **argv)
{
    FILE *f = fopen("highscore.dat", "r");

    for(int h = 0; h < 10; h++)
    {
        highscoreNames[h] = new char[30];
        fscanf(f, "%s %d %s", highscoreNames[h], &highscorePoints[h], highscoreNames[h]);
    }

    fclose(f);

    f = fopen("settings.dat", "r");

    char texturepack[MAX_IMAGENAME_LENGTH - 15];
    fscanf(f, "%s", texturepack);

    fclose(f);

    strncpy(background1img, texturepack, strlen(texturepack));
    strncpy(background2img, texturepack, strlen(texturepack));
    strncpy(background3img, texturepack, strlen(texturepack));
    strncpy(background4img, texturepack, strlen(texturepack));
    strncpy(background5img, texturepack, strlen(texturepack));
    strncpy(background6img, texturepack, strlen(texturepack));
    strncpy(bulletimg, texturepack, strlen(texturepack));
    strncpy(guidedbulletimg, texturepack, strlen(texturepack));
    strncpy(lifeimg, texturepack, strlen(texturepack));
    strncpy(medipackimg, texturepack, strlen(texturepack));
    strncpy(pauseimg, texturepack, strlen(texturepack));
    strncpy(playerimg, texturepack, strlen(texturepack));
    strncpy(explosionimg, texturepack, strlen(texturepack));
    strcat(background1img, "background1.bmp");
    strcat(background2img, "background2.bmp");
    strcat(background3img, "background3.bmp");
    strcat(background4img, "background4.bmp");
    strcat(background5img, "background5.bmp");
    strcat(background6img, "background6.bmp");
    strcat(bulletimg, "bullet.bmp");
    strcat(guidedbulletimg, "gb.bmp");
    strcat(lifeimg, "life.bmp");
    strcat(medipackimg, "medipack.bmp");
    strcat(pauseimg, "pause.bmp");
    strcat(playerimg, "spaceship.bmp");
    strcat(explosionimg, "explosion.bmp");

    srand(RANDOM_SEED);

    ImageCache imgcache;

    Window mainWindow("Mission: Alien", WIDTH, HEIGHT, 16, SDL_HWSURFACE|SDL_DOUBLEBUF, 100, 0, 50, background1img);

    if(Mix_OpenAudio(22050, AUDIO_S16LSB, 2, 4096 ) == -1)
    {
        printf("Failed to initiate SDL Mixer: %s\n", Mix_GetError());
    }
    else
    {
        Mix_Music *music = Mix_LoadMUS("music.mp3");

        if(music == NULL)
            printf("Unable to load background music.\n");
        else
            Mix_PlayMusic(music, -1);
    }

    coords2D middle = mainWindow.getSize();
    middle.x /= 2;
    middle.y /= 2;

    Object2D *player = new Object2D(playerimg, &imgcache);
    player->setPosition(mainWindow.getWidth() / 2, mainWindow.getHeight() - (player->getHeight() / 2));
    addObject2D(player);

    for(int h = 0; h < START_LIFES; h++)
        addLife(&imgcache);

    bool done = false;
    bool pause = false;
    bool left = false;
    bool right = false;
    bool first_time = true;

    int start = SDL_GetTicks();
    int pstart = 0;

    int number = 0;
    int medipacks = 0;
    int rockets = 0;
    int misses = 0;

    while (!done)
    {
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

                        case SDLK_LEFT:
                            left = true;
                        break;

                        case SDLK_RIGHT:
                            right = true;
                        break;

                        case SDLK_p:
                            if(pause == false)
                            {
                                pstart = SDL_GetTicks();
                                pause = true;
                                addPauseSymbol(&imgcache, middle);
                            }
                            else
                            {
                                start += (SDL_GetTicks() - pstart);
                                pause = false;
                                removePauseSymbol();
                            }
                        break;

                        case SDLK_d:
                            //printf("%d\n", ROCKET_SPAWN);
                        break;

                        default:

                        break;
                    }
                break;

                case SDL_KEYUP:
                    switch(event.key.keysym.sym)
                    {
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

        renderObject2DList(&mainWindow);

        if(!pause)
        {
            number++;

            if(number % EXPLOSION_LIFETIME == 0)
                removeExplosions();

            if(number % MEDIPACK_SPAWN == 0)
                addMedipack(&imgcache);
            else if(number % GUIDED_ROCKET_SPAWN == 0)
                addGuidedBullet(&imgcache);
            else if(number % ROCKET_SPAWN == 0)
                addBullet(&imgcache);

            if(left)
                player->moveRelativeWithBorder(-2, 0, WIDTH, HEIGHT);

            if(right)
                player->moveRelativeWithBorder(2, 0, WIDTH, HEIGHT);

            Object2DList *iterator = first;
            Object2DList *before = NULL;

            while(iterator != NULL)
            {
                if(strncmp(bulletimg, iterator->element->getName(), strlen(bulletimg)) == 0 || strncmp(guidedbulletimg, iterator->element->getName(), strlen(guidedbulletimg)) == 0 || strncmp(medipackimg, iterator->element->getName(), strlen(medipackimg)) == 0)
                {
                    iterator->element->moveRelative(0, 1);

                    if(strncmp(guidedbulletimg, iterator->element->getName(), strlen(guidedbulletimg)) == 0 && number % GUIDE_AMOUNT == 0)
                        iterator->element->moveRelative(iterator->element->getPosition().x < player->getPosition().x ? 1 : -1, 0);

                    if(iterator->element->getPosition().y > WIDTH)
                    {
                        iterator = iterator->next;
                        delete before->next;
                        before->next = iterator;
                        misses++;
                    }

                    if(mainWindow.COLLISION_CHECK(iterator->element, player))
                    {
                        if(strncmp(medipackimg, iterator->element->getName(), strlen(medipackimg)) == 0)
                        {
                            iterator = iterator->next;
                            delete before->next;
                            before->next = iterator;
                            addLife(&imgcache);
                            medipacks++;
                        }
                        else
                        {
                            addExplosion(&imgcache, iterator->element->getPosition());
                            iterator = iterator->next;
                            delete before->next;
                            before->next = iterator;
                            deleteLife();
                            rockets++;
                        }
                    }
                }

                before = iterator;

                if(iterator == NULL)
                    break;

                iterator = iterator->next;
            }
        }

        switch(number % (BACKGROUND_IMAGE_CHANGE_5 + 1))
        {
            case 0:
                mainWindow.changeBackgroundImage(background1img);
            break;

            case BACKGROUND_IMAGE_CHANGE_1:
                mainWindow.changeBackgroundImage(background2img);

                if(first_time)
                    printf("Level 1 completed!\n");
            break;

            case BACKGROUND_IMAGE_CHANGE_2:
                mainWindow.changeBackgroundImage(background3img);

                if(first_time)
                    printf("Level 2 completed!\n");
            break;

            case BACKGROUND_IMAGE_CHANGE_3:
                mainWindow.changeBackgroundImage(background4img);

                if(first_time)
                    printf("Level 3 completed!\n");
            break;

            case BACKGROUND_IMAGE_CHANGE_4:
                mainWindow.changeBackgroundImage(background5img);

                if(first_time)
                    printf("Level 4 completed!\n");
            break;

            case BACKGROUND_IMAGE_CHANGE_5:
                mainWindow.changeBackgroundImage(background6img);

                if(first_time)
                    printf("Game completed!\n");

                first_time = false;
            break;
        }

        if(lifes == 0)
            done = true;
    }

    int finish = SDL_GetTicks();
    int points = (finish-start) / 100;

    printf("\nCaptured medipacks: %d\n", medipacks);
    printf("Times got hit:      %d\n", rockets);
    printf("Missed objects:     %d\n", misses);
    printf("Your Score:       %d\n", points);

    int i;

    for(i = 0; i < 10 && highscorePoints[i] > points; i++);

    if(i == 10)
    {
        printf("Out of highscore! :(\nTry again!\n");
    }
    else
    {
        printf("Position %d of 10\n", i + 1);
        char *newName = new char[30];
        printf("Enter your name: ");
        scanf("%s", newName);

        for(int j = 8; j >= i; j--)
        {
            highscorePoints[j+1] = highscorePoints[j];
            highscoreNames[j+1] = highscoreNames[j];
        }

        highscorePoints[i] = points;
        highscoreNames[i] = newName;

        f = fopen("highscore.dat", "w");

        for(i = 0; i < 10; i++)
        {
            fprintf(f, "%2d. %10d %s\n", i+1, highscorePoints[i], highscoreNames[i]);
        }

        fclose(f);
    }

    printf("\n");

    printHighscore();

    printf("\nDeleted %d objects.\n", cleanObject2DList());

    Mix_CloseAudio();

    printf("Audio closed.\n");

    return 0;
}
