#pragma once
#pragma comment(lib, "libjpeg\libjpeg.lib")
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <libjpeg\jpeglib.h>
#include <string>


/* RGB 颜色空间下图像数据存储的方式为（逐行从左到右）：R1,G1,B1,R2,G2,B2……*/


/*  Default Color Space RGB
*  Color Space Not Returned.   
*/

/* Read jpeg from memory.
* jpeg memory source, data destination, width, height, number of components
* (width, height and components are return values provided by function, no need to be provided by user.)
*/
bool readJPEG(const std::vector<uint8_t>& jpeg_in_dat, std::vector<uint8_t>& decoded_data_dest, int& decoded_data_width, int& decoded_data_height, int& decoded_data_components)
{
    std::string file_name;
    return readJPEG(jpeg_in_dat, file_name, false, decoded_data_dest, decoded_data_width, decoded_data_height, decoded_data_components);
}

/* Read jpeg from file.
* jpeg file path, data destination, width, height, number of components    
* (width, height and components are return values provided by function, no need to be provided by user.)
*/
bool readJPEG(const std::string& jpeg_in_file, std::vector<uint8_t>& decoded_data_dest, int& decoded_data_width, int& decoded_data_height, int& decoded_data_components)
{
    std::vector<uint8_t> empty_data;
    return readJPEG(empty_data, jpeg_in_file, true, decoded_data_dest, decoded_data_width, decoded_data_height, decoded_data_components);
}

bool readJPEG(const std::vector<uint8_t>& jpeg_in_dat, const std::string& file_name, bool read_file, std::vector<uint8_t>& decoded_data_dest, int& decoded_data_width, int& decoded_data_height, int& decoded_data_components)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);

    // optional file stream
    FILE* in_file = NULL;

    // specify data source to jpeg lib
    if (!read_file)
    {
        if (jpeg_in_dat.size())
            jpeg_mem_src(&cinfo, (unsigned char*)jpeg_in_dat.data(),
                jpeg_in_dat.size());
        else
            return false;
    }
    else
    {
        if (file_name.size())
        {
            in_file = fopen(file_name.c_str(), "rb");
            jpeg_stdio_src(&cinfo, in_file);
            if (!in_file)
                return false;
        }
        else
        {
            return false;
        }
    }

    // read the header and start decompress
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    /* JSAMPLEs per row in output buffer */
    int row_stride = cinfo.output_width * cinfo.output_components;

    JSAMPARRAY buffer =
        (*cinfo.mem->alloc_sarray) ((j_common_ptr)&cinfo, JPOOL_IMAGE,
            row_stride, 1);

    // resize the output image
    if (decoded_data_dest.size() != row_stride * cinfo.output_height)
        decoded_data_dest.resize(row_stride * cinfo.output_height);

    /* Here we use the library's state variable cinfo.output_scanline as the
    * loop counter, so that we don't have to keep track ourselves.
    */
    while (cinfo.output_scanline < cinfo.output_height)
    {
        // read a scanline and copy it to the output image
        (void)jpeg_read_scanlines(&cinfo, buffer, 1);
        std::copy(buffer[0], buffer[0] + row_stride, &decoded_data_dest[row_stride * (cinfo.output_scanline - 1)]);
    }

    // Finish decompression
    jpeg_finish_decompress(&cinfo);

    // return the image dimensions, output components
    decoded_data_width = cinfo.image_width;
    decoded_data_height = cinfo.image_height;
    decoded_data_components = cinfo.output_components;

    // destroy the libjpeg decompression object
    jpeg_destroy_decompress(&cinfo);

    if (read_file)
        fclose(in_file);

    // done
    return true;
}

