{
  'variables': {
    'platform': '<(OS)',
  },
  'conditions': [
    # Replace gyp platform with node platform, blech
    ['platform == "mac"', {'variables': {'platform': 'darwin'}}],
    ['platform == "win"', {'variables': {'platform': 'win32'}}],
  ],
  'targets': [
    {
      #'target_name': 'image-<(platform)-<(target_arch)',
      'target_name': 'image',
      'defines': [
        'VERSION=0.5.0'
      ],
      'sources': [ 'src/bindings.cpp', 'src/freeimage.cpp', 'src/image.cpp' ],
      'conditions': [
        ['OS=="linux"', {'libraries': ['-lFreeImage']}],
        ['OS=="mac"', {'libraries': ['-lFreeImage']}],
        ['OS=="win"', {
          'libraries': [
            'FreeImage64.lib'
            ],
          'defines' : [
            'WIN32_LEAN_AND_MEAN',
            'VC_EXTRALEAN'
          ]
          },
        ],
      ],
    }
  ]
}
