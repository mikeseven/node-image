/*
** This file contains proprietary software owned by Motorola Mobility, Inc. **
** No rights, expressed or implied, whatsoever to this software are provided by Motorola Mobility, Inc. hereunder. **
**
** (c) Copyright 2011 Motorola Mobility, Inc.  All Rights Reserved.  **
*/

#include "FreeImage.h"
#include "Image.h"

using namespace v8;

extern "C" {
void init (Handle<Object> target)
{
  freeimage::FreeImage::Initialize(target);
  freeimage::Image::Initialize(target);
}

NODE_MODULE(image, init)
}


