/*
** This file contains proprietary software owned by Motorola Mobility, Inc. **
** No rights, expressed or implied, whatsoever to this software are provided by Motorola Mobility, Inc. hereunder. **
**
** (c) Copyright 2011 Motorola Mobility, Inc.  All Rights Reserved.  **
*/

#include "Image.h"
#include <node_buffer.h>

#include <iostream>
using namespace std;

using namespace node;
using namespace v8;

namespace freeimage {

Persistent<Function> Image::constructor_template;

Image::Image(Handle<Object> wrapper) : dib(NULL) {}
Image::~Image() {
  // cout<<"Deleting image"<<endl;
  if(dib) ::FreeImage_Unload(dib);
}

void Image::Initialize(Handle<Object> target) {
  NanScope();

  // constructor
  Local<FunctionTemplate> ctor = FunctionTemplate::New(Image::New);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(JS_STR("Image"));

  // prototype
  //Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
  NODE_SET_PROTOTYPE_METHOD(ctor, "unload", unload);
  NODE_SET_PROTOTYPE_METHOD(ctor, "save", save);
  NODE_SET_PROTOTYPE_METHOD(ctor, "convertTo32Bits", convertTo32Bits);
  NODE_SET_PROTOTYPE_METHOD(ctor, "convertTo24Bits", convertTo24Bits);

  NanAssignPersistent<Function>(constructor_template, ctor->GetFunction());
  target->Set(JS_STR("Image"), ctor->GetFunction());
}

NAN_METHOD(Image::New) {
  NanScope();
  Image *fi = new Image(args.This());
  fi->Wrap(args.This());
  NanReturnValue(args.This());
}

Image *Image::New(FIBITMAP* dib) {

  NanScope();

  Local<Function> cons = NanNew<Function>(constructor_template);
  Local<Object> obj = cons->NewInstance();

  Image *image = ObjectWrap::Unwrap<Image>(obj);
  image->dib = dib;

  int w,h,pitch;
  FREE_IMAGE_TYPE type = FreeImage_GetImageType(dib);

  // obj->SetInternalField(0, External::New(dib));
  obj->Set(JS_STR("width"), JS_INT(w=FreeImage_GetWidth(dib)));
  obj->Set(JS_STR("height"), JS_INT(h=FreeImage_GetHeight(dib)));
  obj->Set(JS_STR("bpp"), JS_INT((int)FreeImage_GetBPP(dib)));
  obj->Set(JS_STR("pitch"), JS_INT(pitch=FreeImage_GetPitch(dib)));
  obj->Set(JS_STR("type"), JS_INT(type));
  obj->Set(JS_STR("redMask"), JS_INT((int)FreeImage_GetRedMask(dib)));
  obj->Set(JS_STR("greenMask"), JS_INT((int)FreeImage_GetGreenMask(dib)));
  obj->Set(JS_STR("blueMask"), JS_INT((int)FreeImage_GetBlueMask(dib)));

  BYTE *bits=FreeImage_GetBits(dib);
  v8::Local<v8::Object> buf = NanNewBufferHandle((char*)bits,h*pitch);
  obj->Set(JS_STR("buffer"), buf);

  return image;
}

NAN_METHOD(Image::unload) {
  NanScope();
  // cout<<"Unloading image DIBs"<<endl;

  Image *image = ObjectWrap::Unwrap<Image>(args.This());
  FreeImage_Unload(image->dib);
  image->dib = NULL;

  NanReturnUndefined();
}

NAN_METHOD(Image::save) {
  NanScope();

  // Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  // FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());
  Image *image = ObjectWrap::Unwrap<Image>(args.This());
  FIBITMAP *dib=image->dib;
  //cout<<"dib "<<hex<<dib<<dec<<endl;

  FREE_IMAGE_FORMAT fif=(FREE_IMAGE_FORMAT) args[0]->Uint32Value();

  String::Utf8Value str(args[1]->ToString());
  int flags=0;
  if(!args[2]->IsUndefined()) {
    flags=args[2]->Int32Value();
  }

  cout<<"Saving image to "<<*str<<" format: "<<hex<<fif<<dec<<" flags: "<<hex<<flags<<dec<<endl;
  if(fif==FIF_JPEG && FreeImage_GetBPP(dib)!=24) {
    //FIBITMAP *old=dib;
    dib=FreeImage_ConvertTo24Bits(dib);
    //FreeImage_Unload(old);
  }
  FreeImage_Save(fif,dib,*str,flags);

  NanReturnUndefined();
}

NAN_METHOD(Image::convertTo32Bits) {
  NanScope();
  // Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  // FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());
  Image *image = ObjectWrap::Unwrap<Image>(args.This());
  FIBITMAP *dib=image->dib;
  FIBITMAP *conv=FreeImage_ConvertTo32Bits(dib);

  NanReturnValue(NanObjectWrapHandle(Image::New(conv)));
}

NAN_METHOD(Image::convertTo24Bits) {
  NanScope();
  // Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  // FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());
  Image *image = ObjectWrap::Unwrap<Image>(args.This());
  FIBITMAP *dib=image->dib;
  FIBITMAP *conv=FreeImage_ConvertTo24Bits(dib);

  NanReturnValue(NanObjectWrapHandle(Image::New(conv)));
}

}
