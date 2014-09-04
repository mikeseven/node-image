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

  static NAN_METHOD(New);

  static NAN_GETTER(getVersion);
  static NAN_METHOD(load);
  static NAN_METHOD(save);

protected:
  ~FreeImage();

private:
  FreeImage(Handle<Object> wrapper);

  static Persistent<Function> constructor_template;

};

};



#endif /* FREEIMAGE_H_ */
