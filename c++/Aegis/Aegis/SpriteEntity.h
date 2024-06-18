#pragma once

#include <GL/glew.h>

#include "BaseEntity.h"

#define SPRITE_DEFAULT_FPS 10

#define SPRITE_COLORS_IN_PALETTE 256

#define SPRITE_TYPE_PARALLEL_UPRIGHT  0
#define SPRITE_TYPE_FACING_UPRIGHT    1
#define SPRITE_TYPE_PARELLEL		  2
#define SPRITE_TYPE_ORIENTED		  3
#define SPRITE_TYPE_PARELLEL_ORIENTED 4

#define SPRITE_TEXTURE_NORMAL		  0
#define SPRITE_TEXTURE_ADDITIVE		  1
#define SPRITE_TEXTURE_INDEXALPHA	  2
#define SPRITE_TEXTURE_ALPHATEST	  3

typedef struct
{
	char id[4];				// Should be IDSP
	uint32_t version;		// Should be 2
	uint32_t type;			// SPRITE_TYPE
	uint32_t textureformat; // SPRITE_TEXTURE
	float radius;			// sqrt((width/2)^2 + (height/2)^2)
	uint32_t maxwidth;		// Highest width of all the frames
	uint32_t maxheight;		// Highest height of all the frames
	uint32_t numframes;		// Number of frames in the animation
	float beamlength;		// ??
	uint32_t synctype;		// 0 = Synchronized animation, 1 = Random offset
	// Palette comes next
} sprheader_t;

typedef struct
{
	uint16_t numcolors;							// Always 256
	color24_t colors[SPRITE_COLORS_IN_PALETTE]; // Colors array
} sprpalette_t;

typedef struct
{
	int framex;			  // X offset in texture coordinates
	int framey;			  // Y offset in texture coordinates
	uint32_t frameheight; // Height of the frame in pixels
	uint32_t framewidth;  // Width of the frame in pixels
	// Byte array of indices to palette (framewidth * frameheight)
} sprframe_t;

class SpriteEntity : public BaseEntity
{
public:
	SpriteEntity(BSPMap& map);
	~SpriteEntity();

	void LoadTexture(char* texture);
	void SetScale(float scale);
	void SetFramerate(float framerate);
	
	virtual void Think(float deltatime) override;
	virtual void Render() override;
private:
	sprheader_t* mhdr;

	std::vector<GLuint> texturenames;
	std::vector<sprframe_t*> frames;

	float currentframe = 0.0;
	float framerate = SPRITE_DEFAULT_FPS;
	float scale = 1.0;
};

