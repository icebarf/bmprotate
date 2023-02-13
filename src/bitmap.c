//  This file is part of bmprotate
//  bmprotate is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by the
//  Free Software Foundation, either version 3 of the License,
//  or (at your option) any later version.
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

int
main (int argc, char *argv[])
{
    if (argc != 2)
        {
            fprintf (stderr,
                     "%s: Invalid Usage\n"
                     "Usage: %s file.bmp\n",
                     argv[0], argv[0]);
            exit (EX_USAGE);
        }
}