/*
** This file contains proprietary software owned by Motorola Mobility, Inc. **
** No rights, expressed or implied, whatsoever to this software are provided by Motorola Mobility, Inc. hereunder. **
**
** (c) Copyright 2011 Motorola Mobility, Inc.  All Rights Reserved.  **
*/


#ifndef FREEIMAGE_H_
#define FREEIMAGE_H_

#include "common.h"

using namespace v8;

namespace freeimage {

class FreeImage : public node::ObjectWrap
{

public:
  static void Initialize(Handle<Object> target);

  static JS_METHOD(New);

  static JS_METHOD(getVersion);
  static JS_METHOD(load);
  static JS_METHOD(save);

protected:
  ~FreeImage();

private:
  FreeImage(Handle<Object> wrapper);

  static Persistent<FunctionTemplate> constructor_template;

};

};



#endif /* FREEIMAGE_H_ */
