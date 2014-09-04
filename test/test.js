var Image=new require('../lib/image').Image,
  	util=require('util');

var log=console.log;

log('FreeImage version: '+Image.version);

var path=__dirname+'/mike_scooter.jpg';
var image=Image.load(path);
log('Image '+path+': \n'+util.inspect(image));

image=image.convertTo32Bits();

image.save(Image.FIF_JPEG,"out_75.jpg",Image.JPEG_PROGRESSIVE | Image.JPEG_QUALITYGOOD);
image.save(Image.FIF_JPEG,"out_25.jpg",Image.JPEG_PROGRESSIVE | Image.JPEG_QUALITYAVERAGE);
image.save(Image.FIF_PNG,"out_9.png",Image.PNG_Z_BEST_COMPRESSION);
image.save(Image.FIF_PNG,"out_0.png",Image.PNG_Z_NO_COMPRESSION);
image.unload();