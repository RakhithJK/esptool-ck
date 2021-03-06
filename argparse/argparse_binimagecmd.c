/**********************************************************************************
 **********************************************************************************
 ***
 ***    argparse_binimagecmd.c
 ***    - parsing of command related to binary flash image functions
 ***
 ***    Copyright (C) 2014 Christian Klippel <ck@atelier-klippel.de>
 ***
 ***    This program is free software; you can redistribute it and/or modify
 ***    it under the terms of the GNU General Public License as published by
 ***    the Free Software Foundation; either version 2 of the License, or
 ***    (at your option) any later version.
 ***
 ***    This program is distributed in the hope that it will be useful,
 ***    but WITHOUT ANY WARRANTY; without even the implied warranty of
 ***    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ***    GNU General Public License for more details.
 ***
 ***    You should have received a copy of the GNU General Public License along
 ***    with this program; if not, write to the Free Software Foundation, Inc.,
 ***    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 ***
 **/
 
#include <stdio.h>
#include <stdint.h>
#include "infohelper.h"
#include "esptool_elf.h"
#include "esptool_elf_object.h"
#include "esptool_binimage.h"

static int argparse_binimagecmd_add_segment(const char *sname, uint32_t padsize)
{
    uint32_t snum;
    uint32_t addr;
    uint32_t size;
    uint32_t pad;
    
    snum = get_elf_secnum_by_name(sname);
    addr = get_elf_section_addr(snum);
    size = get_elf_section_size(snum);
    if(snum)
    {
        print_elf_section_info(snum);
        pad = get_elf_section_size(snum);
        padsize--;
        
        while(pad & padsize)
        {
            pad++;
        }
        
        if(pad > size)
        {
            binimage_add_segment(get_elf_section_addr(snum), pad, get_elf_section_bindata(snum, pad));
            LOGINFO("added section %s at 0x%08X size 0x%08X with padding 0x%08X", get_elf_section_name(snum), addr, size, pad-size);
        }
        else
        {
            binimage_add_segment(get_elf_section_addr(snum), size, get_elf_section_bindata(snum, size));
            LOGINFO("added section %s at 0x%08X size 0x%08X", get_elf_section_name(snum), addr, size);
        }
    }
    
    return snum;
}

int argparse_binimagecmd(int num_args, char **arg_ptr)
{
    char *cur_cmd;
    
    if(arg_ptr[0][1] == 'b' && num_args--)
    {
        cur_cmd = &arg_ptr[0][2];
        arg_ptr++;

        switch(*cur_cmd++)
        {
            case 'o':
                if(num_args < 1)
                {
                    return 0;
                }
                if(binimage_prepare(arg_ptr[0], get_elf_entry()))
                {
                    return 2;
                }
                break;
                
            case 's':
                if(num_args < 1)
                {
                    return 0;
                }
                if(argparse_binimagecmd_add_segment(arg_ptr[0], 4))
                {
                    return 2;
                }
                break;

            case 'c':
                if(binimage_write_close(16))
                {
                    return 1;
                }
                break;
                
            default:
                return 0;
                break;
        }
    }
    return 0;
}
