/*
** This file contains proprietary software owned by Motorola Mobility, Inc. **
** No rights, expressed or implied, whatsoever to this software are provided by Motorola Mobility, Inc. hereunder. **
**
** (c) Copyright 2011 Motorola Mobility, Inc.  All Rights Reserved.  **
*/

#include "string.h"

#include "Image.h"
#include <node_buffer.h>

#include <iostream>
using namespace std;

using namespace node;
using namespace v8;

namespace freeimage {

Persistent<FunctionTemplate> Image::constructor_template;

Image::Image(Handle<Object> wrapper) {}
Image::~Image() {
  Local<Value> internalField = handle_->GetInternalField(0);
  if (internalField->IsNull()) return;

  FIBITMAP *dib = static_cast<FIBITMAP*>(Local<External>::Cast(internalField)->Value());
  FreeImage_Unload(dib);
}

void Image::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(Image::New);
  constructor_template = Persistent<FunctionTemplate>::New(t);

  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(JS_STR("Image"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "unload", unload);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "save", save);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "saveToMemory", saveToMemory);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "convertTo32Bits", convertTo32Bits);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "convertTo24Bits", convertTo24Bits);

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "flipHorizontal", flipHorizontal);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "flipVertical", flipVertical);

  target->Set(JS_STR("Image"), constructor_template->GetFunction());
}

JS_METHOD(Image::New) {
  HandleScope scope;
  Image *fi = new Image(args.This());
  fi->Wrap(args.This());
  return scope.Close(args.This());
}

Image *Image::New(FIBITMAP* dib) {
  HandleScope scope;

  Local<Value> arg = Integer::NewFromUnsigned(0);
  Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

  Image *image = ObjectWrap::Unwrap<Image>(obj);

  int w,h,pitch;
  FREE_IMAGE_TYPE type = FreeImage_GetImageType(dib);

  obj->SetInternalField(0, External::New(dib));
  obj->Set(JS_STR("width"), JS_INT(w=FreeImage_GetWidth(dib)));
  obj->Set(JS_STR("height"), JS_INT(h=FreeImage_GetHeight(dib)));
  obj->Set(JS_STR("bpp"), JS_INT((int)FreeImage_GetBPP(dib)));
  obj->Set(JS_STR("pitch"), JS_INT(pitch=FreeImage_GetPitch(dib)));
  obj->Set(JS_STR("type"), JS_INT(type));
  obj->Set(JS_STR("redMask"), JS_INT((int)FreeImage_GetRedMask(dib)));
  obj->Set(JS_STR("greenMask"), JS_INT((int)FreeImage_GetGreenMask(dib)));
  obj->Set(JS_STR("blueMask"), JS_INT((int)FreeImage_GetBlueMask(dib)));

  BYTE *bits=FreeImage_GetBits(dib);
  node::Buffer *buf = node::Buffer::New((char*)bits,h*pitch);
  obj->Set(JS_STR("buffer"), buf->handle_);

  return image;
}

JS_METHOD(Image::unload) {
  HandleScope scope;

  Local<Value> internalField = args.This()->GetInternalField(0);
  if (!internalField->IsNull()) {
    FIBITMAP *dib = static_cast<FIBITMAP*>(Local<External>::Cast(internalField)->Value());
    FreeImage_Unload(dib);
    args.This()->SetInternalField(0, v8::Null());
  }

  return Undefined();
}

JS_METHOD(Image::save) {
  HandleScope scope;

  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());

  FREE_IMAGE_FORMAT fif=(FREE_IMAGE_FORMAT) args[0]->Uint32Value();

  String::Utf8Value str(args[1]->ToString());
  int flags=0;
  if(!args[2]->IsUndefined()) {
    flags=args[2]->Int32Value();
  }

  // cout<<"Saving image to "<<*str<<" format: "<<hex<<fif<<dec<<" flags: "<<hex<<flags<<dec<<endl;
  if(fif==FIF_JPEG && FreeImage_GetBPP(dib)!=24) {
    //FIBITMAP *old=dib;
    dib=FreeImage_ConvertTo24Bits(dib);
    //FreeImage_Unload(old);
  }
  FreeImage_Save(fif,dib,*str,flags);

  return Undefined();
}

JS_METHOD(Image::saveToMemory) {
  HandleScope scope;

  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());
  //cout<<"dib "<<hex<<dib<<dec<<endl;

  FREE_IMAGE_FORMAT fif=(FREE_IMAGE_FORMAT) args[0]->Uint32Value();

  int flags=0;
  if(!args[1]->IsUndefined()) {
    flags=args[1]->Int32Value();
  }

  if(fif==FIF_JPEG && FreeImage_GetBPP(dib)!=24) {
    // FIBITMAP *old=dib;
    dib=FreeImage_ConvertTo24Bits(dib);
    // FreeImage_Unload(old);
  }

  BYTE *mem_buffer = NULL;
  DWORD file_size;
  FIMEMORY *hmem = FreeImage_OpenMemory();

  FreeImage_SaveToMemory(fif,dib,hmem,flags);

  FreeImage_AcquireMemory(hmem, &mem_buffer, &file_size);
  Buffer *slowBuffer = Buffer::New(file_size);
  memcpy(Buffer::Data(slowBuffer), (char*) mem_buffer,(size_t) file_size);

  FreeImage_CloseMemory(hmem);

  Local<Object> globalObj = Context::GetCurrent()->Global();
  Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
  Handle<Value> constructorArgs[3] = { slowBuffer->handle_, v8::Integer::New(file_size), v8::Integer::New(0) };
  Local<Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);

  return scope.Close(actualBuffer);
}

JS_METHOD(Image::convertTo32Bits) {
  HandleScope scope;
  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());
  FIBITMAP *conv=FreeImage_ConvertTo32Bits(dib);

  return scope.Close(Image::New(conv)->handle_);
}

JS_METHOD(Image::convertTo24Bits) {
  HandleScope scope;
  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());
  FIBITMAP *conv=FreeImage_ConvertTo24Bits(dib);

  return scope.Close(Image::New(conv)->handle_);
}


JS_METHOD(Image::flipHorizontal) {
  HandleScope scope;

  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib = static_cast<FIBITMAP *>(wrap->Value());
  BOOL flip = FreeImage_FlipHorizontal(dib);

  return scope.Close(Boolean::New(flip));
}

JS_METHOD(Image::flipVertical) {
  HandleScope scope;

  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib = static_cast<FIBITMAP *>(wrap->Value());
  BOOL flip = FreeImage_FlipVertical(dib);

  return scope.Close(Boolean::New(flip));
}

}
