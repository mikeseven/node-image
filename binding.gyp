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
        'VERSION=0.7.1'
      ],
      'include_dirs' : [
        "<!(node -e \"require('nan')\")",
      ],
      'sources': [ 'src/bindings.cpp', 'src/FreeImage.cpp', 'src/Image.cpp' ],
      'conditions': [
        ['OS=="linux"', {'libraries': ['-lfreeimage']}],
        ['OS=="mac"',
          {
            'libraries': ['-lfreeimage'],
            'include_dirs': ['/usr/local/include'],
            'library_dirs': ['/usr/local/lib'],
          }
        ],
        ['OS=="win"',
          {
            'include_dirs': [
              './deps/FreeImage/include'
              ],
            'library_dirs': [
              './deps/FreeImage/windows/lib/<(target_arch)'
              ],
            'libraries': [
              'FreeImage.lib'
              ],
            'defines' : [
              'WIN32_LEAN_AND_MEAN',
              'VC_EXTRALEAN'
            ],
            'cflags' : [
              '/O2','/Oy','/GL','/GF','/Gm-','/EHsc','/MT','/GS','/Gy','/GR-','/Gd'
            ],
            'ldflags' : [
              '/OPT:REF','/OPT:ICF','/LTCG'
            ]
          },
        ],
      ],
    }
  ]
}
