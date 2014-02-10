#ifndef SDL2D_H_INCLUDED
#define SDL2D_H_INCLUDED



#include <SDL/SDL.h>



struct coords2D
{
    int x;
    int y;
};


struct colorRGBA
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
};


struct colorRGB
{
    Uint8 r;
    Uint8 g;
    Uint8 b;

    colorRGB operator=(colorRGBA color);
};


struct image
{
    char *filename;
    SDL_Surface *surface;
};


struct imageList
{
    image *element;
    imageList *next;
};



class ImageCache
{
    public:
        ImageCache();
        ~ImageCache();

        SDL_Surface *requestImage(const char *filename);
        int unloadImage(const char *filename);

    protected:
        void unloadImage(imageList *img);
        SDL_Surface *loadImage(const char *filename);


        imageList *first;
};


class Object2D
{
    friend class Window;

    public:
        Object2D();
        Object2D(const char *image, ImageCache *cache);
        Object2D(const char *image, ImageCache *cache, int x, int y);
        Object2D(const char *image, ImageCache *cache, coords2D coords);
        ~Object2D();

        coords2D setPosition(int x, int y);
        coords2D setPosition(coords2D coords);
        coords2D getPosition();

        int getWidth();
        int getHeight();
        coords2D getSize();

        char *getName();

        coords2D moveRelative(int moveX, int moveY);
        coords2D moveRelative(coords2D direction);
        coords2D moveRelativeWithBorder(int moveX, int moveY, int borderX, int borderY);
        coords2D moveRelativeWithBorder(coords2D direction, int borderX, int borderY);
        coords2D moveRelativeWithBorder(int moveX, int moveY, coords2D border);
        coords2D moveRelativeWithBorder(coords2D direction, coords2D border);

    protected:
        void initObject2D();

        char *name;
        SDL_Surface *bmp;
        SDL_Rect dest;
};


class Window
{
    public:
        Window();
        Window(int width, int height, int bpp, Uint32 flags);
        Window(coords2D size, int bpp, Uint32 flags);
        Window(int width, int height, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue);
        Window(coords2D size, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue);
        Window(int width, int height, int bpp, Uint32 flags, colorRGB color);
        Window(coords2D size, int bpp, Uint32 flags, colorRGB color);
        Window(int width, int height, int bpp, Uint32 flags, colorRGBA color);
        Window(coords2D size, int bpp, Uint32 flags, colorRGBA color);
        Window(int width, int height, int bpp, Uint32 flags, const char *backImg);
        Window(coords2D size, int bpp, Uint32 flags, const char *backImg);
        Window(int width, int height, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue, const char *backImg);
        Window(coords2D size, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue, const char *backImg);
        Window(int width, int height, int bpp, Uint32 flags, colorRGB color, const char *backImg);
        Window(coords2D size, int bpp, Uint32 flags, colorRGB color, const char *backImg);
        Window(int width, int height, int bpp, Uint32 flags, colorRGBA color, const char *backImg);
        Window(coords2D size, int bpp, Uint32 flags, colorRGBA color, const char *backImg);
        Window(const char *caption, int width, int height, int bpp, Uint32 flags);
        Window(const char *caption, coords2D size, int bpp, Uint32 flags);
        Window(const char *caption, int width, int height, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue);
        Window(const char *caption, coords2D size, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue);
        Window(const char *caption, int width, int height, int bpp, Uint32 flags, colorRGB color);
        Window(const char *caption, coords2D size, int bpp, Uint32 flags, colorRGB color);
        Window(const char *caption, int width, int height, int bpp, Uint32 flags, colorRGBA color);
        Window(const char *caption, coords2D size, int bpp, Uint32 flags, colorRGBA color);
        Window(const char *caption, int width, int height, int bpp, Uint32 flags, const char *backImg);
        Window(const char *caption, coords2D size, int bpp, Uint32 flags, const char *backImg);
        Window(const char *caption, int width, int height, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue, const char *backImg);
        Window(const char *caption, coords2D size, int bpp, Uint32 flags, Uint8 red, Uint8 green, Uint8 blue, const char *backImg);
        Window(const char *caption, int width, int height, int bpp, Uint32 flags, colorRGB color, const char *backImg);
        Window(const char *caption, coords2D size, int bpp, Uint32 flags, colorRGB color, const char *backImg);
        Window(const char *caption, int width, int height, int bpp, Uint32 flags, colorRGBA color, const char *backImg);
        Window(const char *caption, coords2D size, int bpp, Uint32 flags, colorRGBA color, const char *backImg);
        ~Window();

        bool checkBackgroundImage();
        int loadBackgroundImage(const char *filename);
        void unloadBackgroundImage();
        int changeBackgroundImage(const char *filename);

        void renderBackground();
        int renderObject2D(Object2D *obj);
        int flipBuffers();

        int getWidth();
        int getHeight();
        coords2D getSize();

        colorRGB getBackgroundColor();
        colorRGB setBackgroundColor(Uint8 red, Uint8 green, Uint8 blue);
        colorRGB setBackgroundColor(colorRGB color);
        colorRGB setBackgroundColor(colorRGBA color);

        void setCaption(const char *caption);

        static bool objectCirclesOverlap(Object2D *obj1, Object2D *obj2);
        static bool objectRectsOverlap(Object2D *obj1, Object2D *obj2);
        static bool objectRectsOverlapSimple(Object2D *obj1, Object2D *obj2);

    protected:
        void initWindow();
        SDL_Surface *initSDL(int width, int height, int bpp, Uint32 flags);
        SDL_Surface *initSDL(coords2D size, int bpp, Uint32 flags);


        SDL_Surface *screen;
        SDL_Surface *backgroundImage;
        colorRGB backgroundColor;
};



#endif
