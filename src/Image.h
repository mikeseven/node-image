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
  static JS_METHOD(New);
  static Image *New(FIBITMAP* image);

  static JS_METHOD(unload);
  static JS_METHOD(save);
  static JS_METHOD(saveToMemory);
  static JS_METHOD(convertTo32Bits);
  static JS_METHOD(convertTo24Bits);

  static JS_METHOD(flipHorizontal);
  static JS_METHOD(flipVertical);

private:
  Image(Handle<Object> wrapper);
  static Persistent<FunctionTemplate> constructor_template;
};

};




#endif /* IMAGE_H_ */
