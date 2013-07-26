{
  "targets": [
    {
      "target_name": "wHomeControlPi",
      "sources": [ "wHomeControlPi.cpp", "HomeControl.cpp", "HomeControl.h"],
      "include_dirs": [ "/usr/local/include" ],
      "ldflags": [ "-lwiringPi" ]
    }
  ]
}