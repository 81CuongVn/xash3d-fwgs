/*
gl_rmath.c - renderer mathlib
Copyright (C) 2010 Uncle Mike

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "vk_math.h"

#include <memory.h>

/*
========================================================================

	       Matrix4x4 operations (private to renderer)

========================================================================
*/
void Matrix4x4_Concat( matrix4x4 out, const matrix4x4 in1, const matrix4x4 in2 )
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0] + in1[0][3] * in2[3][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1] + in1[0][3] * in2[3][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2] + in1[0][3] * in2[3][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3] * in2[3][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0] + in1[1][3] * in2[3][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1] + in1[1][3] * in2[3][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2] + in1[1][3] * in2[3][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3] * in2[3][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0] + in1[2][3] * in2[3][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1] + in1[2][3] * in2[3][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2] + in1[2][3] * in2[3][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3] * in2[3][3];
	out[3][0] = in1[3][0] * in2[0][0] + in1[3][1] * in2[1][0] + in1[3][2] * in2[2][0] + in1[3][3] * in2[3][0];
	out[3][1] = in1[3][0] * in2[0][1] + in1[3][1] * in2[1][1] + in1[3][2] * in2[2][1] + in1[3][3] * in2[3][1];
	out[3][2] = in1[3][0] * in2[0][2] + in1[3][1] * in2[1][2] + in1[3][2] * in2[2][2] + in1[3][3] * in2[3][2];
	out[3][3] = in1[3][0] * in2[0][3] + in1[3][1] * in2[1][3] + in1[3][2] * in2[2][3] + in1[3][3] * in2[3][3];
}

/*
================
Matrix4x4_CreateProjection

NOTE: produce quake style world orientation
================
*/
void Matrix4x4_CreateProjection( matrix4x4 out, float xMax, float xMin, float yMax, float yMin, float zNear, float zFar )
{
	out[0][0] = ( 2.0f * zNear ) / ( xMax - xMin );
	out[1][1] = ( 2.0f * zNear ) / ( yMax - yMin );
	out[2][2] = -( zFar + zNear ) / ( zFar - zNear );
	out[3][3] = out[0][1] = out[1][0] = out[3][0] = out[0][3] = out[3][1] = out[1][3] = 0.0f;

	out[2][0] = 0.0f;
	out[2][1] = 0.0f;
	out[0][2] = ( xMax + xMin ) / ( xMax - xMin );
	out[1][2] = ( yMax + yMin ) / ( yMax - yMin );
	out[3][2] = -1.0f;
	out[2][3] = -( 2.0f * zFar * zNear ) / ( zFar - zNear );
}

void Matrix4x4_CreateOrtho( matrix4x4 out, float xLeft, float xRight, float yBottom, float yTop, float zNear, float zFar )
{
	out[0][0] = 2.0f / (xRight - xLeft);
	out[1][1] = 2.0f / (yTop - yBottom);
	out[2][2] = -2.0f / (zFar - zNear);
	out[3][3] = 1.0f;
	out[0][1] = out[0][2] = out[1][0] = out[1][2] = out[3][0] = out[3][1] = out[3][2] = 0.0f;

	out[2][0] = 0.0f;
	out[2][1] = 0.0f;
	out[0][3] = -(xRight + xLeft) / (xRight - xLeft);
	out[1][3] = -(yTop + yBottom) / (yTop - yBottom);
	out[2][3] = -(zFar + zNear) / (zFar - zNear);
}

/*
================
Matrix4x4_CreateModelview

NOTE: produce quake style world orientation
================
*/
void Matrix4x4_CreateModelview( matrix4x4 out )
{
	out[0][0] = out[1][1] = out[2][2] = 0.0f;
	out[3][0] = out[0][3] = 0.0f;
	out[3][1] = out[1][3] = 0.0f;
	out[3][2] = out[2][3] = 0.0f;
	out[3][3] = 1.0f;
	out[1][0] = out[0][2] = out[2][1] = 0.0f;
	out[2][0] = out[0][1] = -1.0f;
	out[1][2] = 1.0f;
}

void Matrix4x4_ToArrayFloatGL( const matrix4x4 in, float out[16] )
{
	out[ 0] = in[0][0];
	out[ 1] = in[1][0];
	out[ 2] = in[2][0];
	out[ 3] = in[3][0];
	out[ 4] = in[0][1];
	out[ 5] = in[1][1];
	out[ 6] = in[2][1];
	out[ 7] = in[3][1];
	out[ 8] = in[0][2];
	out[ 9] = in[1][2];
	out[10] = in[2][2];
	out[11] = in[3][2];
	out[12] = in[0][3];
	out[13] = in[1][3];
	out[14] = in[2][3];
	out[15] = in[3][3];
}

void Matrix4x4_FromArrayFloatGL( matrix4x4 out, const float in[16] )
{
	out[0][0] = in[0];
	out[1][0] = in[1];
	out[2][0] = in[2];
	out[3][0] = in[3];
	out[0][1] = in[4];
	out[1][1] = in[5];
	out[2][1] = in[6];
	out[3][1] = in[7];
	out[0][2] = in[8];
	out[1][2] = in[9];
	out[2][2] = in[10];
	out[3][2] = in[11];
	out[0][3] = in[12];
	out[1][3] = in[13];
	out[2][3] = in[14];
	out[3][3] = in[15];
}

void Matrix4x4_CreateTranslate( matrix4x4 out, float x, float y, float z )
{
	out[0][0] = 1.0f;
	out[0][1] = 0.0f;
	out[0][2] = 0.0f;
	out[0][3] = x;
	out[1][0] = 0.0f;
	out[1][1] = 1.0f;
	out[1][2] = 0.0f;
	out[1][3] = y;
	out[2][0] = 0.0f;
	out[2][1] = 0.0f;
	out[2][2] = 1.0f;
	out[2][3] = z;
	out[3][0] = 0.0f;
	out[3][1] = 0.0f;
	out[3][2] = 0.0f;
	out[3][3] = 1.0f;
}

void Matrix4x4_CreateRotate( matrix4x4 out, float angle, float x, float y, float z )
{
	float	len, c, s;

	len = x * x + y * y + z * z;
	if( len != 0.0f ) len = 1.0f / sqrt( len );
	x *= len;
	y *= len;
	z *= len;

	angle *= (-M_PI_F / 180.0f);
	SinCos( angle, &s, &c );

	out[0][0]=x * x + c * (1 - x * x);
	out[0][1]=x * y * (1 - c) + z * s;
	out[0][2]=z * x * (1 - c) - y * s;
	out[0][3]=0.0f;
	out[1][0]=x * y * (1 - c) - z * s;
	out[1][1]=y * y + c * (1 - y * y);
	out[1][2]=y * z * (1 - c) + x * s;
	out[1][3]=0.0f;
	out[2][0]=z * x * (1 - c) + y * s;
	out[2][1]=y * z * (1 - c) - x * s;
	out[2][2]=z * z + c * (1 - z * z);
	out[2][3]=0.0f;
	out[3][0]=0.0f;
	out[3][1]=0.0f;
	out[3][2]=0.0f;
	out[3][3]=1.0f;
}

void Matrix4x4_CreateScale( matrix4x4 out, float x )
{
	out[0][0] = x;
	out[0][1] = 0.0f;
	out[0][2] = 0.0f;
	out[0][3] = 0.0f;
	out[1][0] = 0.0f;
	out[1][1] = x;
	out[1][2] = 0.0f;
	out[1][3] = 0.0f;
	out[2][0] = 0.0f;
	out[2][1] = 0.0f;
	out[2][2] = x;
	out[2][3] = 0.0f;
	out[3][0] = 0.0f;
	out[3][1] = 0.0f;
	out[3][2] = 0.0f;
	out[3][3] = 1.0f;
}

void Matrix4x4_CreateScale3( matrix4x4 out, float x, float y, float z )
{
	out[0][0] = x;
	out[0][1] = 0.0f;
	out[0][2] = 0.0f;
	out[0][3] = 0.0f;
	out[1][0] = 0.0f;
	out[1][1] = y;
	out[1][2] = 0.0f;
	out[1][3] = 0.0f;
	out[2][0] = 0.0f;
	out[2][1] = 0.0f;
	out[2][2] = z;
	out[2][3] = 0.0f;
	out[3][0] = 0.0f;
	out[3][1] = 0.0f;
	out[3][2] = 0.0f;
	out[3][3] = 1.0f;
}

void Matrix4x4_ConcatTranslate( matrix4x4 out, float x, float y, float z )
{
	matrix4x4 base, temp;

	Matrix4x4_Copy( base, out );
	Matrix4x4_CreateTranslate( temp, x, y, z );
	Matrix4x4_Concat( out, base, temp );
}

void Matrix4x4_ConcatRotate( matrix4x4 out, float angle, float x, float y, float z )
{
	matrix4x4 base, temp;

	Matrix4x4_Copy( base, out );
	Matrix4x4_CreateRotate( temp, angle, x, y, z );
	Matrix4x4_Concat( out, base, temp );
}

void Matrix4x4_ConcatScale( matrix4x4 out, float x )
{
	matrix4x4	base, temp;

	Matrix4x4_Copy( base, out );
	Matrix4x4_CreateScale( temp, x );
	Matrix4x4_Concat( out, base, temp );
}

void Matrix4x4_ConcatScale3( matrix4x4 out, float x, float y, float z )
{
	matrix4x4  base, temp;

	Matrix4x4_Copy( base, out );
	Matrix4x4_CreateScale3( temp, x, y, z );
	Matrix4x4_Concat( out, base, temp );
}

void computeTangent(vec3_t out_tangent, const vec3_t v0, const vec3_t v1, const vec3_t v2, const vec2_t uv0, const vec2_t uv1, const vec2_t uv2) {
	vec3_t e1, e2;
	vec2_t duv1, duv2;

	VectorSubtract(v1, v0, e1);
	VectorSubtract(v2, v0, e2);
	Vector2Subtract(uv1, uv0, duv1);
	Vector2Subtract(uv2, uv0, duv2);

	const float div = duv1[0] * duv2[1] - duv1[1] * duv2[0];
	if (fabs(div) < 1e-5f) {
		VectorClear(out_tangent);
		return;
	}
	const float f = 1.f / div;
	out_tangent[0] = f * (duv2[1] * e1[0] - duv1[1] * e2[0]);
	out_tangent[1] = f * (duv2[1] * e1[1] - duv1[1] * e2[1]);
	out_tangent[2] = f * (duv2[1] * e1[2] - duv1[1] * e2[2]);
}
