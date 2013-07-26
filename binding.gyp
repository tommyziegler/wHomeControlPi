{
  "targets": [
    {
      "target_name": "whomecontrolpi",
      "sources": [ "whomecontrolpi.cpp", "HomeControl.cpp", "HomeControl.h"],
      "include_dirs": [ "/usr/local/include" ],
      "ldflags": [ "-lwiringPi" ]
    }
  ]
}