/*
 * Vega Strike
 * Copyright (C) 2001-2002 Daniel Horn
 *
 * http://vegastrike.sourceforge.net/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <stdlib.h>
#include <iostream.h>

#include "aux_texture.h"
#include "sprite.h"
#include "matrix.h"
#include "gfxlib.h"
#include "vegastrike.h"
#include <assert.h>
#include <math.h>
#ifndef M_PI_2
# define M_PI_2		1.57079632679489661923	/* pi/2 */
#endif
static float *mview = NULL;

Sprite::Sprite(const char *file) {



  xcenter = 0;
  ycenter = 0;
  widtho2 = 0;
  heighto2 = 0;
  rotation = 0;
  surface = NULL;
  
  FILE *fp = fopen(file, "r");
  char texture[64]={0};
  char texturea[64]={0};
  fscanf(fp, "%63s %63s", texture, texturea);
  fscanf(fp, "%f %f", &widtho2, &heighto2);
  fscanf(fp, "%f %f", &xcenter, &ycenter);
  fclose(fp);

  widtho2/=2;
  heighto2/=-2;
  if (texturea[0]==0) {
    surface = new Texture(texture);    
  } else {
    surface = new Texture(texture,texturea);    
  }
}	

Sprite::~Sprite()
{
	if(surface!=NULL)
		delete surface;
}



void Sprite::Draw(const Transformation &dtrans, const Matrix m)
{
    GFXDisable(LIGHTING);
    GFXDisable(DEPTHWRITE);
    GFXDisable(DEPTHTEST);
    GFXEnable(TEXTURE0);
    GFXDisable(TEXTURE1);
    surface->MakeActive();
    GFXDisable (CULLFACE);
    GFXBegin(QUADS);
    if (rotation) {
      const float cw = widtho2*cos(rotation);
      const float sw = widtho2*sin(rotation);
      const float ch = heighto2*cos(M_PI_2+rotation);
      const float sh = heighto2*sin(M_PI_2+rotation);
      const float wnew = cw+ch;
      const float hnew = sw+sh;
      GFXTexCoord2f(0.00f, 1.00f);
      GFXVertex3f(xcenter-wnew, ycenter+hnew, 0.00f);
      GFXTexCoord2f(1.00f, 1.00f);
      GFXVertex3f(xcenter+wnew, ycenter+hnew, 0.00f);
      GFXTexCoord2f(1.00f, 0.00f);
      GFXVertex3f(xcenter+wnew, ycenter-hnew, 0.00f);
      GFXTexCoord2f(0.00f, 0.00f);
      GFXVertex3f(xcenter-wnew, ycenter-hnew, 0.00f);
    } else {
      GFXTexCoord2f(0.00f, 1.00f);
      GFXVertex3f(xcenter-widtho2, ycenter+heighto2, 0.00f);
      GFXTexCoord2f(1.00f, 1.00f);
      GFXVertex3f(xcenter+widtho2, ycenter+heighto2, 0.00f);
      GFXTexCoord2f(1.00f, 0.00f);
      GFXVertex3f(xcenter+widtho2, ycenter-heighto2, 0.00f);
      GFXTexCoord2f(0.00f, 0.00f);
      GFXVertex3f(xcenter-widtho2, ycenter-heighto2, 0.00f);
    }
    GFXEnd();
    GFXEnable (CULLFACE);
    GFXEnable(LIGHTING);
    GFXEnable(DEPTHWRITE);
    GFXEnable(DEPTHTEST);
}

void Sprite::SetPosition(const float &x1, const float &y1) {
  xcenter =x1;
  ycenter = y1;
}

void Sprite::GetPosition(float &x1, float &y1)
{
	x1 = xcenter;
	y1 = ycenter;
}
void Sprite::SetSize (float x1, float y1) {
  widtho2 = x1/2;
  heighto2 = y1/2;
}
void Sprite::GetSize (float &x1,float &y1) {
  x1 = widtho2*2;
  y1 = heighto2*2;
}

void Sprite::SetRotation(const float &rot) {
  rotation = rot;
}

void Sprite::GetRotation(float &rot) {
  rot = rotation;
}
