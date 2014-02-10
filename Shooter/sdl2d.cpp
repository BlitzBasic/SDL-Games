#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <SDL_image.h>

#include "sdl2d.h"



colorRGB colorRGB::operator=(colorRGBA color)
{
    colorRGB temp;
    temp.r = color.r;
    temp.g = color.g;
    temp.b = color.b;

    return temp;
}



ImageCache::ImageCache()
{
    first = NULL;
}


ImageCache::~ImageCache()
{
    int counter = 0;

    imageList *currentElement = first;
    imageList *nextElement = NULL;

    while(currentElement != NULL)
    {
        counter++;

        nextElement = currentElement->next;

        unloadImage(currentElement);

        currentElement = nextElement;
    }

    printf("%d images unloaded.\n", counter);
}


SDL_Surface *ImageCache::requestImage(const char *filename)
{
    imageList *iterator = first;

    while(iterator != NULL)
    {
        if(strncmp(iterator->element->filename, filename, strlen(filename)) == 0)
        {
            return iterator->element->surface;
        }

        iterator = iterator->next;
    }

    return loadImage(filename);
}


int ImageCache::unloadImage(const char *filename)
{
    imageList *currentElement = first;
    imageList *nextElement = NULL;

    if(first == NULL)
    {
        printf("you have to load images before you can unload them.\n");
        return -1;
    }
    else if(strncmp(first->element->filename, filename, strlen(filename)) == 0)
    {
        nextElement = first->next;

        unloadImage(first);

        first = nextElement;

        return 0;
    }

    while(currentElement->next != NULL)
    {
        nextElement = currentElement->next;

        if(strncmp(nextElement->element->filename, filename, strlen(filename)) == 0)
        {
            nextElement = nextElement->next;

            unloadImage(currentElement->next);

            currentElement->next = nextElement;

            return 0;
        }

        currentElement = nextElement;
    }

    printf("%s can not be unloaded because it is not loaded.\n", filename);

    return -1;
}


void ImageCache::unloadImage(imageList *img)
{
    if(img != NULL)
    {
        if(img->element != NULL)
        {
            if(img->element->surface != NULL)
                SDL_FreeSurface(img->element->surface);

            if(img->element->filename != NULL)
                delete[] img->element->filename;

            delete img->element;
        }

        delete img;
    }
}


SDL_Surface *ImageCache::loadImage(const char *filename)
{
    SDL_Surface *newSurface = IMG_Load(filename);

    if(newSurface == NULL)
        return NULL;

    imageList *iterator = NULL;

    if(first == NULL)
    {
        first = new imageList;
        iterator = first;
    }
    else
    {
        iterator = first;

        while(iterator->next != NULL)
            iterator = iterator->next;

        iterator->next = new imageList;
        iterator = iterator->next;
    }

    iterator->next = NULL;
    iterator->element = new image;
    iterator->element->filename = new char[strlen(filename + 1)];
    strncpy(iterator->element->filename, filename, strlen(filename));
    iterator->element->surface = newSurface;

    printf("new image %s loaded.\n", filename);

    return newSurface;
}



Object2D::Object2D()
{
    initObject2D();
}


Object2D::Object2D(const char *image, ImageCache *cache)
{
    initObject2D();

    name = new char[strlen(image) + 1];
    strncpy(name, image, strlen(image));

    bmp = cache->requestImage(image);

    if(bmp == NULL)
    {
        printf("Unable to load bitmap %s: %s\n", image, SDL_GetError());
        exit(-1);
    }
}


Object2D::Object2D(const char *image, ImageCache *cache, int x, int y)
{
    initObject2D();

    name = new char[strlen(image) + 1];
    strncpy(name, image, strlen(image));

    bmp = cache->requestImage(image);

    if(bmp == NULL)
    {
        printf("Unable to load bitmap %s: %s\n", image, SDL_GetError());
        exit(-1);
    }

    setPosition(x, y);
}


Object2D::Object2D(const char *image, ImageCache *cache, coords2D coords)
{
    initObject2D();

    name = new char[strlen(image) + 1];
    strncpy(name, image, strlen(image));

    bmp = cache->requestImage(image);

    if(bmp == NULL)
    {
        printf("Unable to load bitmap %s: %s\n", image, SDL_GetError());
        exit(-1);
    }

    setPosition(coords);
}


Object2D::~Object2D()
{
    delete[] name;
}


coords2D Object2D::setPosition(int x, int y)
{
    coords2D coords;
    coords.x = x;
    coords.y = y;

    return setPosition(coords);
}


coords2D Object2D::setPosition(coords2D coords)
{
    dest.x = coords.x - (bmp->w / 2);
    dest.y = coords.y - (bmp->h / 2);

    return coords;
}


coords2D Object2D::getPosition()
{
    coords2D coords;
    coords.x = dest.x + (bmp->w / 2);
    coords.y = dest.y + (bmp->h / 2);

    return coords;
}


int Object2D::getWidth()
{
    return bmp->w;
}


int Object2D::getHeight()
{
    return bmp->h;
}


coords2D Object2D::getSize()
{
    coords2D size;
    size.x = bmp->w;
    size.y = bmp->h;

    return size;
}


char *Object2D::getName()
{
    return name;
}


coords2D Object2D::moveRelative(int moveX, int moveY)
{
    coords2D direction;
    direction.x = moveX;
    direction.y = moveY;

    return moveRelative(direction);
}


coords2D Object2D::moveRelative(coords2D direction)
{
    dest.x += direction.x;
    dest.y += direction.y;

    return getPosition();
}


coords2D Object2D::moveRelativeWithBorder(int moveX, int moveY, int borderX, int borderY)
{
    coords2D direction;
    direction.x = moveX;
    direction.y = moveY;

    coords2D border;
    border.x = borderX;
    border.y = borderY;

    return moveRelativeWithBorder(direction, border);
}


coords2D Object2D::moveRelativeWithBorder(coords2D direction, int borderX, int borderY)
{
    coords2D border;
    border.x = borderX;
    border.y = borderY;

    return moveRelativeWithBorder(direction, border);
}


coords2D Object2D::moveRelativeWithBorder(int moveX, int moveY, coords2D border)
{
    coords2D direction;
    direction.x = moveX;
    direction.y = moveY;

    return moveRelativeWithBorder(direction, border);
}


coords2D Object2D::moveRelativeWithBorder(coords2D direction, coords2D border)
{
    coords2D newPosition;
    newPosition.x = dest.x + direction.x;
    newPosition.y = dest.y + direction.y;

    if(newPosition.x + bmp->w > border.x)
        newPosition.x = border.x - bmp->w;

    if(newPosition.y + bmp->h > border.y)
        newPosition.y = border.y - bmp->h;

    dest.x = newPosition.x;
    dest.y = newPosition.y;

    return newPosition;
}


void Object2D::initObject2D()
{
    bmp = NULL;
    memset(&dest, 0, sizeof(dest));
}



Window::Window()
{
    initWindow();
}


Window::Window(int width, int height, int bpp, Uint32 flags)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }
 }


Window::Window(coords2D size, int bpp, Uint32 flags)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }
}


Window::Window(int width, int height, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(red, green, blue);
}


Window::Window(coords2D size, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(red, green, blue);
}


Window::Window(int width, int height, int bpp, Uint32 flags, colorRGB color)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(color);
}


Window::Window(coords2D size, int bpp, Uint32 flags, colorRGB color)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(color);
}


Window::Window(int width, int height, int bpp, Uint32 flags, colorRGBA color)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(color);
}


Window::Window(coords2D size, int bpp, Uint32 flags, colorRGBA color)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(color);
}


Window::Window(int width, int height, int bpp, Uint32 flags, const char *backImg)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    loadBackgroundImage(backImg);
}


Window::Window(coords2D size, int bpp, Uint32 flags, const char *backImg)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    loadBackgroundImage(backImg);
}


Window::Window(int width, int height, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue, const char *backImg)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(red, green, blue);

    loadBackgroundImage(backImg);
}


Window::Window(coords2D size, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue, const char *backImg)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(red, green, blue);

    loadBackgroundImage(backImg);
}


Window::Window(int width, int height, int bpp, Uint32 flags, colorRGB color, const char *backImg)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(color);

    loadBackgroundImage(backImg);
}


Window::Window(coords2D size, int bpp, Uint32 flags, colorRGB color, const char *backImg)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(color);

    loadBackgroundImage(backImg);
}


Window::Window(int width, int height, int bpp, Uint32 flags, colorRGBA color, const char *backImg)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(color);

    loadBackgroundImage(backImg);
}


Window::Window(coords2D size, int bpp, Uint32 flags, colorRGBA color, const char *backImg)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setBackgroundColor(color);

    loadBackgroundImage(backImg);
}


Window::Window(const char *caption, int width, int height, int bpp, Uint32 flags)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);
 }


Window::Window(const char *caption, coords2D size, int bpp, Uint32 flags)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);
}


Window::Window(const char *caption, int width, int height, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(red, green, blue);
}


Window::Window(const char *caption, coords2D size, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(red, green, blue);
}


Window::Window(const char *caption, int width, int height, int bpp, Uint32 flags, colorRGB color)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(color);
}


Window::Window(const char *caption, coords2D size, int bpp, Uint32 flags, colorRGB color)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(color);
}


Window::Window(const char *caption, int width, int height, int bpp, Uint32 flags, colorRGBA color)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(color);
}


Window::Window(const char *caption, coords2D size, int bpp, Uint32 flags, colorRGBA color)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(color);
}


Window::Window(const char *caption, int width, int height, int bpp, Uint32 flags, const char *backImg)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    loadBackgroundImage(backImg);
}


Window::Window(const char *caption, coords2D size, int bpp, Uint32 flags, const char *backImg)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    loadBackgroundImage(backImg);
}


Window::Window(const char *caption, int width, int height, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue, const char *backImg)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(red, green, blue);

    loadBackgroundImage(backImg);
}


Window::Window(const char *caption, coords2D size, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue, const char *backImg)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(red, green, blue);

    loadBackgroundImage(backImg);
}


Window::Window(const char *caption, int width, int height, int bpp, Uint32 flags, colorRGB color, const char *backImg)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(color);

    loadBackgroundImage(backImg);
}


Window::Window(const char *caption, coords2D size, int bpp, Uint32 flags, colorRGB color, const char *backImg)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(color);

    loadBackgroundImage(backImg);
}


Window::Window(const char *caption, int width, int height, int bpp, Uint32 flags, colorRGBA color, const char *backImg)
{
    initWindow();

    screen = initSDL(width, height, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(color);

    loadBackgroundImage(backImg);
}


Window::Window(const char *caption, coords2D size, int bpp, Uint32 flags, colorRGBA color, const char *backImg)
{
    initWindow();

    screen = initSDL(size, bpp, flags);

    if(screen == NULL)
    {
        printf("Unable to open graphics window.\n");
        exit(-1);
    }

    setCaption(caption);

    setBackgroundColor(color);

    loadBackgroundImage(backImg);
}


Window::~Window()
{
    if(screen != NULL)
        SDL_FreeSurface(screen);

    if(backgroundImage != NULL)
        SDL_FreeSurface(backgroundImage);
}


bool Window::checkBackgroundImage()
{
    if(backgroundImage == NULL)
        return false;
    else
        return true;
}


int Window::loadBackgroundImage(const char *filename)
{
    if(backgroundImage != NULL)
    {
        printf("You have to unload the current background image before you can load a new background image.\n");
        return -1;
    }

    backgroundImage = SDL_LoadBMP(filename);

    if(backgroundImage == NULL)
    {
        printf("Unable to load background image %s: %s\n", filename, SDL_GetError());
        return -1;
    }

    return 0;
}


void Window::unloadBackgroundImage()
{
    if(backgroundImage != NULL)
    {
        SDL_FreeSurface(backgroundImage);

        backgroundImage = NULL;
    }
}


int Window::changeBackgroundImage(const char *filename)
{
    unloadBackgroundImage();
    return loadBackgroundImage(filename);
}


void Window::renderBackground()
{
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, backgroundColor.r, backgroundColor.g, backgroundColor.b));

    if(backgroundImage != NULL)
    {
        SDL_BlitSurface(backgroundImage, 0, screen, 0);
    }
}


int Window::renderObject2D(Object2D *obj)
{
    return SDL_BlitSurface(obj->bmp, 0, screen, &obj->dest);
}


int Window::flipBuffers()
{
    return SDL_Flip(screen);
}


int Window::getWidth()
{
    return screen->w;
}


int Window::getHeight()
{
    return screen->h;
}


coords2D Window::getSize()
{
    coords2D coords;
    coords.x = screen->w;
    coords.y = screen->h;

    return coords;
}


colorRGB Window::getBackgroundColor()
{
    return backgroundColor;
}


colorRGB Window::setBackgroundColor(Uint8 red, Uint8 green, Uint8 blue)
{
    backgroundColor.r = red;
    backgroundColor.g = green;
    backgroundColor.b = blue;

    return backgroundColor;
}


colorRGB Window::setBackgroundColor(colorRGB color)
{
    return (backgroundColor = color);
}


colorRGB Window::setBackgroundColor(colorRGBA color)
{
    return (backgroundColor = color);
}


void Window::setCaption(const char *caption)
{
    SDL_WM_SetCaption(caption, NULL);
}


bool Window::objectCirclesOverlap(Object2D *obj1, Object2D *obj2)
{
    int circle1 = sqrt(((obj1->getHeight() / 2) * (obj1->getHeight() / 2)) + ((obj1->getWidth() / 2) * (obj1->getWidth() / 2)));
    int circle2 = sqrt(((obj2->getHeight() / 2) * (obj2->getHeight() / 2)) + ((obj2->getWidth() / 2) * (obj2->getWidth() / 2)));
    int distance = sqrt(((obj1->getPosition().x - obj2->getPosition().x) * (obj1->getPosition().x - obj2->getPosition().x)) + ((obj1->getPosition().y - obj2->getPosition().y) * (obj1->getPosition().y - obj2->getPosition().y)));

    return (distance < (circle1 + circle2));
}


bool Window::objectRectsOverlap(Object2D *obj1, Object2D *obj2)
{
    // TODO

    return false;
}


bool Window::objectRectsOverlapSimple(Object2D *obj1, Object2D *obj2)
{
    coords2D vertex[4];

    vertex[0].x = obj1->getPosition().x + obj1->getWidth() / 2;
    vertex[0].y = obj1->getPosition().y + obj1->getHeight() / 2;

    vertex[1].x = obj1->getPosition().x - obj1->getWidth() / 2;
    vertex[1].y = obj1->getPosition().y + obj1->getHeight() / 2;

    vertex[2].x = obj1->getPosition().x + obj1->getWidth() / 2;
    vertex[2].y = obj1->getPosition().y - obj1->getHeight() / 2;

    vertex[3].x = obj1->getPosition().x - obj1->getWidth() / 2;
    vertex[3].y = obj1->getPosition().y - obj1->getHeight() / 2;

    for(int i = 0; i < 4; i++)
    {
        if(vertex[i].x <= obj2->getPosition().x + obj2->getWidth() / 2 && vertex[i].x >= obj2->getPosition().x - obj2->getWidth() / 2 && vertex[i].y <= obj2->getPosition().y + obj2->getHeight() / 2 && vertex[i].y >= obj2->getPosition().y - obj2->getHeight() / 2)
            return true;
    }

    return false;
}


void Window::initWindow()
{
    screen = NULL;
    backgroundImage = NULL;
    memset(&backgroundColor, 0, sizeof(backgroundColor));
}


SDL_Surface *Window::initSDL(int width, int height, int bpp, Uint32 flags)
{
    if(SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        return NULL;
    }

    atexit(SDL_Quit);

    SDL_Surface* screen = SDL_SetVideoMode(width, height, bpp, flags);

    if(!screen)
    {
        printf("Unable to set %dx%dx%d video: %s\n", width, height, bpp, SDL_GetError());
        return NULL;
    }

    return screen;
}


SDL_Surface *Window::initSDL(coords2D size, int bpp, Uint32 flags)
{
    return initSDL(size.x, size.y, bpp, flags);
}
