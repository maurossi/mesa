/** 
 **
 ** Copyright 2010, The Android Open Source Project
 **
 ** Licensed under the Apache License, Version 2.0 (the "License"); 
 ** you may not use this file except in compliance with the License. 
 ** You may obtain a copy of the License at 
 **
 **     http://www.apache.org/licenses/LICENSE-2.0 
 **
 ** Unless required by applicable law or agreed to in writing, software 
 ** distributed under the License is distributed on an "AS IS" BASIS, 
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 ** See the License for the specific language governing permissions and 
 ** limitations under the License.
 */

#ifndef _PIXELFLINGER2_IMAGE_FILE_H_
#define _PIXELFLINGER2_IMAGE_FILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef union {	unsigned char channels[4]; unsigned int val; } Pixel;

static void SaveBMP(const char * filePath, unsigned * data, unsigned int width, unsigned int height)
{
	FILE * file = fopen(filePath, "wb");
    if (!file)
		printf("SaveMap failed fopen '%s'\n", filePath);	
		
	fwrite("BM", 2, 1, file);
	const unsigned int dataOffset = 0x36;
	const unsigned int dataSize = width * height * 4;
	const unsigned int fileSize = dataSize + dataOffset;
	fwrite((const char *)&fileSize, 4, 1, file); 
	fwrite("\0\0\0\0", 4, 1, file); // unused
	fwrite((const char *)&dataOffset, 4, 1, file);
	fwrite("\x28\0\0\0", 4, 1, file); // header size from this point
	fwrite((const char *)&width, 4, 1, file);
	fwrite((const char *)&height, 4, 1, file);
	fwrite("\x01\0", 2, 1, file); // 1 color plane
	fwrite("\x20\0", 2, 1, file); // 32bpp
	fwrite("\0\0\0\0", 4, 1, file); // BI_RGB no compression
	fwrite((const char *)&dataSize, 4, 1, file);
	fwrite("\x13\0\0\0", 4, 1, file); // horz res, 2.835 pixel/meter
	fwrite("\x13\0\0\0", 4, 1, file); // vert res
	fwrite("\0\0\0\0", 4, 1, file); // number of colours in palette
	fwrite("\0\0\0\0", 4, 1, file); // number of important colours, 0 means all are important
	
	// for non 32bpp image, need padding at end of each row for 4 byte alignment
	unsigned x, y;
	//for (y = 0; y < height; y++) // flip vertically
	//	fwrite((const char *)&data[(height - y - 1) * width], width * 4, 1, file);
	
	//fwrite(data, width * height * 4, 1, file);
	
    // vertical flip and convert rgba to bgra
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
        {
            unsigned pixel = data[(height - y - 1) * width + x];
            unsigned r = (pixel & 0xff) << 16;
            unsigned b = (pixel & 0xff0000) >> 16;
            pixel = (pixel & 0xff00ff00) | r | b;
            fwrite(&pixel, sizeof(pixel), 1, file);
        }
    
	fclose(file);
}

#if USE_16BPP_TEXTURE
#define TO_16BPP(rgba) ((((rgba) & 0xF8) >> 3) | (((rgba) & 0xFC00) >> 5) | (((rgba) & 0xF80000) >> 8))
#else
#define TO_16BPP(rgba) (rgba)
#endif

static int LoadTGA(const char * filePath, unsigned int * width, unsigned int * height, void ** data)
{
	FILE * file = fopen(filePath, "rb");
	if (!file)
	{
		printf("LoadTGA failed to open %s \n", filePath);
		assert(file);
		return -1;
	}
	
	struct TGAHeader 
	{
		unsigned char idLength, cMapType;
		enum TGAType { NoImageData = 0, UncompressedCM = 1, 
			UncompressedRGB = 2, UncompressedBW = 3, RLECM = 9, 
			RLERGB = 10, CompressedBW = 11, 
			CompressedCM = 32, //32  -  Compressed color-mapped data, using Huffman, Delta, and runlength encoding.
			CompressedCM4 = 33 //33  -  Compressed color-mapped data, using Huffman, Delta, and runlength encoding.  4-pass quadtree-type process.
		} type : 8;
        unsigned short cMapStart/* __attribute__ ((packed))*/;
        unsigned short cMapLength/* __attribute__ ((packed))*/;
		unsigned char cMapDepth;
        unsigned short xOffset/* __attribute__ ((packed))*/;
        unsigned short yOffset/* __attribute__ ((packed))*/;
		unsigned short width/* __attribute__ ((packed))*/;
        unsigned short height/* __attribute__ ((packed))*/;
		unsigned char bpp;
        struct { 
            unsigned attribs : 4; 
            unsigned reserved : 1;
            unsigned origin : 1; 
            enum InterleaveType { None = 0, TwoWay = 1, FourWay = 2, Reserved = 3 } interleave : 2;
        } __attribute__ ((packed)) imageDescriptor;
	} __attribute__ ((packed)) header;
	
    assert(18 == sizeof(header));
    fread(&header, sizeof(header), 1, file);
	
#if USE_16BPP_TEXTURE
    unsigned short * dest = (unsigned short *)malloc(header.width * header.height * 2);
#else
    unsigned * dest = (unsigned *)malloc(header.width * header.height * 4);
#endif
    
	*data = dest;
	*width = header.width;
	*height = header.height;
	
	if (!*data)
    {
        fclose(file);
		return -1;
	}

    Pixel pixel;
	unsigned int i;
        
	if (TGAHeader::UncompressedRGB == header.type)
	{
		if (24 == header.bpp)
		{
            pixel.channels[3] = 255;
            for (i = 0; i < (unsigned)header.width * header.height; i++)
			{
				pixel.channels[2] = fgetc(file);
                pixel.channels[1] = fgetc(file);
                pixel.channels[0] = fgetc(file);
				*dest++ = TO_16BPP(pixel.val); 
			}
		}
		else if (32 == header.bpp)
        {
            for (i = 0; i < (unsigned)header.width * header.height; i++)
            {
                pixel.channels[2] = fgetc(file);
                pixel.channels[1] = fgetc(file);
                pixel.channels[0] = fgetc(file);
                pixel.channels[3] = fgetc(file);
                *dest++ = TO_16BPP(pixel.val);
            }
			//fread(dest, header.width * header.height * 4, 1, file);
        }
	}
	else if (TGAHeader::UncompressedBW == header.type)
	{
		if (8 == header.bpp)
		{
			for (i = 0; i < (unsigned)header.width * header.height; i++)
			{
				pixel.channels[3] = pixel.channels[2] = 
				pixel.channels[1] = pixel.channels[0] = fgetc(file);
				*dest++ = TO_16BPP(pixel.val); 
			}
		}
		else
			assert(0);
	}
    else if (TGAHeader::RLERGB == header.type)
    {
        if (24 == header.bpp)
        {
            pixel.channels[3] = 255;
            for (i = 0; i < (unsigned)header.width * header.height;)
            {
                unsigned char count = fgetc(file);
                if (0x80 & count) // repeated run
                {
                    count = (0x7f & count) + 1;
                    pixel.channels[2] = fgetc(file);
                    pixel.channels[1] = fgetc(file);
                    pixel.channels[0] = fgetc(file);
                    for (unsigned j = 0; j < count; j++)
                        *dest++ = TO_16BPP(pixel.val);
                }
                else // literal run
                {
                    count += 1;
                    for (unsigned j = 0; j < count; j++)
                    {
                        pixel.channels[2] = fgetc(file);
                        pixel.channels[1] = fgetc(file);
                        pixel.channels[0] = fgetc(file);
                        *dest++ = TO_16BPP(pixel.val);
                    }
                }
                i += count;
            }
        }
        else if (32 == header.bpp)
        {
            for (i = 0; i < (unsigned)header.width * header.height;)
            {
                unsigned char count = fgetc(file);
                if (0x80 & count) // repeated run
                {
                    count = (0x7f & count) + 1;
                    pixel.channels[2] = fgetc(file);
                    pixel.channels[1] = fgetc(file);
                    pixel.channels[0] = fgetc(file);
                    pixel.channels[3] = fgetc(file);
                    for (unsigned j = 0; j < count; j++)
                        *dest++ = TO_16BPP(pixel.val);
                }
                else // literal run
                {
                    count += 1;
                    for (unsigned j = 0; j < count; j++)
                    {
                        pixel.channels[2] = fgetc(file);
                        pixel.channels[1] = fgetc(file);
                        pixel.channels[0] = fgetc(file);
                        pixel.channels[3] = fgetc(file);
                        *dest++ = TO_16BPP(pixel.val);
                    }
                }
                i += count;
            }
            
        }
        else
            assert(0);
    }
	else
		assert(0);
			
	fclose(file);
	
	return -1;
};

static unsigned GenerateMipmaps(void ** data, const unsigned width, const unsigned height)
{
    unsigned levels = 1;
    unsigned dim = (width > height ? width : height) >> 1;
    unsigned size = 0, w = width, h = height;
    while (dim)
    {
        levels++;
        w = (w + 1) / 2;
        h = (h + 1) / 2;
        size += w * h;
        dim >>= 1;
    }
    Pixel * buffer = (Pixel *)malloc(size * sizeof(*buffer));
    Pixel * previous = (Pixel *)data[0];
    w = width; h = height;
    for (unsigned i = 1; i < levels; i++)
    {
        const unsigned cw = (w + 1) / 2, ch = (h + 1) / 2;
        unsigned * current = (unsigned *)(data[i] = buffer);
        for (unsigned y = 0; y < ch; y++)
            for (unsigned x = 0; x < cw; x++)
            {
                unsigned channels[4] = {0,0,0,0};
                for (unsigned yy = 0; yy < 2; yy++)
                    for (unsigned xx = 0; xx < 2; xx++)
                    {
                        unsigned s = x * 2 + xx;
                        unsigned t = y * 2 + yy;
                        s = s < w - 1 ? s : w - 1;
                        t = t < h - 1 ? t : h - 1;
                        const Pixel * p = previous + t * w + s;
                        for (unsigned i = 0; i < 4; i++)
                            channels[i] += p->channels[i];
                    }
                for (unsigned i = 0; i < 4; i++)
                    channels[i] /= 4;
                current[y * cw + x] = channels[0] | (channels[1] << 8) |
                (channels[2] << 16) | (channels[3] << 24);

            }
        
        
        buffer += cw * ch;
        w = cw;
        h = ch;
        previous = (Pixel *)current;
    }
    
    return levels;
}

#endif // _PIXELFLINGER2_IMAGE_FILE_H_

