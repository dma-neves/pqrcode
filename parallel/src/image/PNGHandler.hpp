#ifndef PNG_HANDLER_HPP
#define PNG_HANDLER_HPP

#include "ImageHandler.hpp"
#include "Image.hpp"
#include <png.h>
#include <cstdlib>
#include <iostream>

class PNGHandler : public ImageHandler
{
private:

    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep * row_pointers;

    int nchan;

public:

    PNGHandler() {}

    ~PNGHandler()
    {
        for (int y = 0; y < height; y++)
            free(row_pointers[y]);
        free(row_pointers);
    }

    Image readImage(char* file)
    {
        // open file and test if png
        unsigned char header[8];
        FILE *fp = fopen(file, "rb");
        if (!fp)
            throw UNABLE_TO_OPEN_FILE_EXCEPTION;
        fread(header, 1, 8, fp);
        bool isPng = !png_sig_cmp(header, 0, 8);
        if (!isPng) 
            throw UNEXPECTED_FILE_TYPE_EXCEPTION;

        // Init
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
            throw FAILED_TO_READ_FILE_EXCEPTION;

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
            throw FAILED_TO_READ_FILE_EXCEPTION;

        if (setjmp(png_jmpbuf(png_ptr)))
            throw FAILED_TO_READ_FILE_EXCEPTION;

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);

        // Read file
        if (setjmp(png_jmpbuf(png_ptr)))
            throw FAILED_TO_READ_FILE_EXCEPTION;

        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        for (int y = 0; y < height; y++)
            row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

        png_read_image(png_ptr, row_pointers);

        fclose(fp);

        // Convert to Image

        Image img(width, height);
        nchan = 0;
        
        if(color_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_palette_to_rgb(png_ptr);
            color_type = png_get_color_type(png_ptr, info_ptr);
        }

        if(color_type == PNG_COLOR_TYPE_GRAY)
        {
            // TODO: this conversion sometimes results in an execution error
            png_set_gray_to_rgb(png_ptr);
            color_type = png_get_color_type(png_ptr, info_ptr);
        }

        if (color_type == PNG_COLOR_TYPE_RGBA)
            nchan = 4;
        else if (color_type == PNG_COLOR_TYPE_RGB)
            nchan = 3;
        else
        {
            std::cout << "color type: " << (int)color_type << std::endl;
            throw INVALID_COLOR_FORMAT_EXCEPTION;
        }

        for (int y = 0; y < height; y++)
        {
            png_byte* row = row_pointers[y];
            for (int x = 0;  x <width; x++)
            {
                png_byte* ptr = &(row[x*nchan]);

                img.pixels[y][x].r = ptr[0];
                img.pixels[y][x].g = ptr[1];
                img.pixels[y][x].b = ptr[2];
            }
        }

        // Free memory

        return img;
    }

    void writeImage(Image& image, char* file)
    {
        FILE *fp = fopen(file, "wb");
        if (!fp)
            throw UNABLE_TO_OPEN_FILE_EXCEPTION;


        // init
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
            throw FAILED_TO_WRITE_FILE_EXCEPTION;

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
            throw FAILED_TO_WRITE_FILE_EXCEPTION;

        if (setjmp(png_jmpbuf(png_ptr)))
            throw FAILED_TO_WRITE_FILE_EXCEPTION;

        png_init_io(png_ptr, fp);


        // write header
        if (setjmp(png_jmpbuf(png_ptr)))
            throw FAILED_TO_WRITE_FILE_EXCEPTION;

        png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);


        // write bytes
        if (setjmp(png_jmpbuf(png_ptr)))
            throw FAILED_TO_WRITE_FILE_EXCEPTION;

        // copy image to row_pointers

        for (int y = 0; y < height; y++)
        {
            png_byte* row = row_pointers[y];
            for (int x = 0;  x <width; x++)
            {
                png_byte* ptr = &(row[x*nchan]);

                ptr[0] = image.pixels[y][x].r;
                ptr[1] = image.pixels[y][x].g;
                ptr[2] = image.pixels[y][x].b;
            }
        }

        png_write_image(png_ptr, row_pointers);


        /* end write */

        if (setjmp(png_jmpbuf(png_ptr)))
            throw FAILED_TO_WRITE_FILE_EXCEPTION;

        png_write_end(png_ptr, NULL);

        fclose(fp);
    }
};

#endif