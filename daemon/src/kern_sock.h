/***************************************************************************
 *   Copyright (C) 2005 by Steven Joseph   *
 *   stevenjose@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <sys/socket.h>


#include "/lib/modules/2.6.9-1.667/build/include/linux/netlink.h"
#include "emailfs.h"
#define MAX_PAYLOAD 1024  /* maximum payload size*/



void initconn() ;
void kernsend(char sndbuff[]);
char * kernrcv();
void close();
