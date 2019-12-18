// *************************************************************************************************
//
// Horde3D
//   Next-Generation Graphics Engine
// --------------------------------------
// Copyright (C) 2006-2011 Nicolas Schulz
//
// This software is distributed under the terms of the Eclipse Public License v1.0.
// A copy of the license may be obtained at: http://www.eclipse.org/legal/epl-v10.html
//
// *************************************************************************************************

#ifndef _h3d_config_H_
#define _h3d_config_H_

// =================================================================================================
// Compile Time Configuration
// =================================================================================================

// Validate input arguments of Horde3D API calls
#define H3D_API_VALIDATION

// Send log messages to the output channel of the attached debugger
#define H3D_DEBUGGER_OUTPUT

// Check for errors and invalid data during each drawcall (requires DEBUG config)
//#define H3D_VALIDATE_DRAWCALLS

typedef unsigned int    GLenum;
typedef unsigned char   GLboolean;
typedef unsigned int    GLbitfield;
typedef signed char     GLbyte;
typedef short           GLshort;
typedef int             GLint;
typedef int             GLsizei;
typedef unsigned char   GLubyte;
typedef unsigned short  GLushort;
typedef unsigned int    GLuint;
typedef float           GLfloat;
typedef float           GLclampf;
typedef double          GLdouble;
typedef double          GLclampd;
typedef void            GLvoid;
// ARB_timer_query
typedef int64           GLint64;
typedef uint64          GLuint64;


#endif // _h3d_config_H_
