/*
 ** This file contains proprietary software owned by Motorola Mobility, Inc. **
 ** No rights, expressed or implied, whatsoever to this software are provided by Motorola Mobility, Inc. hereunder. **
 **
 ** (c) Copyright 2011 Motorola Mobility, Inc.  All Rights Reserved.  **
 */

#include "FreeImage.h"
#include "Image.h"

#include <iostream>
using namespace std;

using namespace node;
using namespace v8;

namespace freeimage {

Persistent<FunctionTemplate> FreeImage::constructor_template;

FreeImage::FreeImage(Handle<Object> wrapper) {
  FreeImage_Initialise(FALSE);
}

FreeImage::~FreeImage() {
  FreeImage_DeInitialise();
}

void FreeImage::Initialize(Handle<Object> target) {
  NanScope();

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  constructor_template = Persistent<FunctionTemplate>::New(t);

  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(JS_STR("FreeImage"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "getVersion", getVersion);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "load", load);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "save", save);

  target->Set(JS_STR("FreeImage"), constructor_template->GetFunction());
}


NAN_METHOD(FreeImage::New) {
  NanScope();
  FreeImage *fi = new FreeImage(args.This());
  fi->Wrap(args.This());
  NanReturnValue(args.This());
}

NAN_METHOD(FreeImage::getVersion) {
  NanScope();
  NanReturnValue(JS_STR(FreeImage_GetVersion()));
}

NAN_METHOD(FreeImage::load) {
  NanScope();

  String::Utf8Value filename(args[0]->ToString());

  FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(*filename);
  FIBITMAP *dib=NULL;

  if( fif != FIF_UNKNOWN )
    dib = FreeImage_Load(fif, *filename);

  // check that dib does not contains pixels
  if(!dib) NanReturnUndefined();
  if(!FreeImage_HasPixels(dib)) NanReturnUndefined();

  //cout<<"dib "<<hex<<dib<<dec<<endl;
  /*FREE_IMAGE_TYPE type = FreeImage_GetImageType(dib);
  int w=0,h=0,pitch=0;

  Handle<ObjectTemplate> obj = ObjectTemplate::New();
  obj->SetInternalFieldCount(1);

  Local<Object> image = obj->NewInstance();
  image->SetInternalField(0, External::New(dib));
  image->Set(JS_STR("width"), JS_INT(w=FreeImage_GetWidth(dib)));
  image->Set(JS_STR("height"), JS_INT(h=FreeImage_GetHeight(dib)));
  image->Set(JS_STR("bpp"), JS_INT(FreeImage_GetBPP(dib)));
  image->Set(JS_STR("pitch"), JS_INT(pitch=FreeImage_GetPitch(dib)));
  image->Set(JS_STR("type"), JS_INT(type));
  image->Set(JS_STR("redMask"), JS_INT(FreeImage_GetRedMask(dib)));
  image->Set(JS_STR("greenMask"), JS_INT(FreeImage_GetGreenMask(dib)));
  image->Set(JS_STR("blueMask"), JS_INT(FreeImage_GetBlueMask(dib)));

  BYTE *bits=FreeImage_GetBits(dib);
  node::Buffer *buf = node::Buffer::New((char*)bits,h*pitch);
  image->Set(JS_STR("buffer"), buf->handle_);

  NanReturnValue(image);*/
  NanReturnValue(Image::New(dib)->handle_);
}

NAN_METHOD(FreeImage::save) {
  NanScope();
  String::Utf8Value filename(args[0]->ToString());
  cout<<"args length: "<<args.Length()<<endl;

  FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(*filename);
  cout<<"saving format "<<format<<endl;

  Local<Object> obj=args[1]->ToObject();
  void *buffer = obj->GetIndexedPropertiesExternalArrayData();
  //cout<<"buffer uchar*: "<<(obj->GetIndexedPropertiesExternalArrayDataType()==kExternalUnsignedByteArray)<<endl;
  //cout<<"buffer size: "<<obj->GetIndexedPropertiesExternalArrayDataLength()<<endl;

  uint32_t width=args[2]->ToUint32()->Value();
  uint32_t height=args[3]->ToUint32()->Value();

  uint32_t pitch=width*4, bpp=32;
  uint32_t redMask=0xFF000000, greenMask=0x00FF0000, blueMask=0x0000FF00;

  if(args.Length()>4) pitch=args[4]->ToUint32()->Value();
  if(args.Length()>5) bpp=args[5]->ToUint32()->Value();
  if(args.Length()>6) redMask=args[6]->ToUint32()->Value();
  if(args.Length()>7) greenMask=args[7]->ToUint32()->Value();
  if(args.Length()>8) blueMask=args[8]->ToUint32()->Value();

  cout<<"wxh: "<<width<<"x"<<height<<" bpp: "<<bpp<<" pitch: "<<pitch<<endl;

  FIBITMAP *image = FreeImage_ConvertFromRawBits(
      (BYTE*)buffer,
      width, height, pitch, bpp,
      redMask, greenMask, blueMask);

  if(format==FIF_JPEG && bpp!=24) {
    FIBITMAP *old=image;
    image=FreeImage_ConvertTo24Bits(image);
    FreeImage_Unload(old);
  }
  NanReturnValue(Boolean::New((FreeImage_Save(format, image, *filename) == TRUE) ? true : false));
}

}
