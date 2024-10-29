typedef struct
{
    char b, g, r;
} rgb8_t;


typedef struct
{
    rgb8_t palette[256];
    int w, h;
    char *data;
} bitmap_t;

bitmap_t* LoadBitmap(char* path);