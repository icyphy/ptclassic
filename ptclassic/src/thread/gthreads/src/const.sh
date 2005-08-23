#!/bin/sh
#  Copyright (C) 1992, the Florida State University
#  Distributed by the Florida State University under the terms of the
#  GNU Library General Public License.
#
#This file is part of Pthreads.
#
#Pthreads is free software; you can redistribute it and/or
#modify it under the terms of the GNU Library General Public
#License as published by the Free Software Foundation (version 2).
#
#Pthreads is distributed "AS IS" in the hope that it will be
#useful, but WITHOUT ANY WARRANTY; without even the implied
#warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#See the GNU Library General Public License for more details.
#
#You should have received a copy of the GNU Library General Public
#License along with Pthreads; see the file COPYING.  If not, write
#to the Free Software Foundation, 675 Mass Ave, Cambridge,
#MA 02139, USA.
#
#Report problems and direct all questions to:
#
#  pthreads-bugs@ada.cs.fsu.edu
#
#  @(#)const.sh	3.1 10/24/96
#

echo "#if defined(__linux__) || defined(__FreeBSD__) || defined(_M_UNIX)"
echo -n "#define SIGACTION_CONST "
grep sigaction /usr/include/signal.h | grep -v _sigaction | grep const > tmp
if (test -s tmp); then
  echo -n const;
fi
echo ""
echo -n "#define SIGSUSPEND_CONST "
grep sigsuspend /usr/include/signal.h | grep -v _sigsuspend | grep const > tmp
if (test -s tmp); then
  echo -n const
fi;
echo ""
echo -n "#define SIGPROCMASK_CONST "
grep sigprocmask /usr/include/signal.h | grep -v _sigprocmask | grep const > tmp
if (test -s tmp); then
  echo -n const
fi
echo ""
echo -n "#define LONGJMP_CONST "
grep longjmp /usr/include/setjmp.h | grep -v _longjmp | grep const > tmp
if (test -s tmp); then
  echo -n const
fi
echo ""
echo -n "#define SIGLONGJMP_CONST "
grep siglongjmp /usr/include/setjmp.h | grep -v _siglongjmp | grep const > tmp
if (test -s tmp); then
  echo -n const
fi
echo ""
echo "#endif /* defined(__linux__) || defined(__FreeBSD__) || defined(_M_UNIX) */"
