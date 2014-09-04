/*
** This file contains proprietary software owned by Motorola Mobility, Inc. **
** No rights, expressed or implied, whatsoever to this software are provided by Motorola Mobility, Inc. hereunder. **
**
** (c) Copyright 2011 Motorola Mobility, Inc.  All Rights Reserved.  **
*/


#ifndef IMAGE_H_
#define IMAGE_H_

#include "common.h"
#include <FreeImage.h>

using namespace v8;

namespace freeimage {

class Image : public node::ObjectWrap
{

public:
  ~Image();
  static void Initialize(Handle<Object> target);
  static NAN_METHOD(New);
  static Image *New(FIBITMAP* image);

  static NAN_METHOD(unload);
  static NAN_METHOD(save);
  static NAN_METHOD(convertTo32Bits);
  static NAN_METHOD(convertTo24Bits);

private:
  Image(Handle<Object> wrapper);
  static Persistent<Function> constructor_template;

  FIBITMAP *dib;
};

};




#endif /* IMAGE_H_ */
