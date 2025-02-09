// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "stdafx.hpp"
#include "iw5.hpp"

namespace xsk::menu::iw5
{

int stricmp(const char* s1, const char* s2)
{
	int i, j = 0;
	for(i=0; s1[i]!='\0'; i++){}
	for(j=0; s2[j]!='\0'; j++){}

	if(i == j || i > j)
	{
		i = 0;
		j = 0;
		for(i=0; s1[i]!='\0'; i++)
		{
			if(s1[i] - s2[i] == 32 || s1[i] - s2[i] == -32 || s1[i] - s2[i] == 0) j++;
			else if(s1[i]<s2[i]) return -1;
			else return 1;
		}
	}
	else
	{
		i = 0;
		j = 0;
		for(i=0; s2[i]!='\0'; i++)
		{
			if(s1[i] - s2[i] == 32 || s1[i] - s2[i] == -32 || s1[i] - s2[i] == 0) j++;
			else if(s1[i]<s2[i]) return -1;
			else return 1;
		}
	}

	return 0;
}

void strlwr(char* str)
{
	int i = 0;

	while(str[i] != '\0')
	{
		if(str[i] >='A' && str[i] <='Z')
			str[i] += 32;
		i++;
	}
}

} // namespace xsk::menu::iw5
