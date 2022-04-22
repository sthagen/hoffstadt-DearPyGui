#pragma once

#include <Python.h>
#include "mvMath.h"

//-----------------------------------------------------------------------------
// mvBuffer
//-----------------------------------------------------------------------------
struct mvBuffer
{
    float* data;
    long length;
    int width;
    int height;
};

struct PymvBuffer
{
    PyObject_HEAD
    mvBuffer arr;
};

//-----------------------------------------------------------------------------
// mvVec4
//-----------------------------------------------------------------------------

struct PymvVec4
{
    PyObject_HEAD
    mvVec4 vec4;
};

//-----------------------------------------------------------------------------
// mvMat4
//-----------------------------------------------------------------------------

struct PymvMat4
{
    PyObject_HEAD
    mvMat4 m;
};
