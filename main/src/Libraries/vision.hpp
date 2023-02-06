#pragma once
#include "main.h"
#include "pros/vision.h"
#include "logging.hpp"

class _Vision: private Vision{
  public:
    using object = vision_object_s_t;
    using signature = vision_signature_s_t;

  private:
    static vision_signature_s_t blue_sig;
    static vision_signature_s_t red_sig;

  public:
    _Vision(int port);

    static void print_object(object obj);

    object blue_object() const;
    bool blue_exists() const;
    int  blue_width() const;
    int  blue_height() const;
    int  blue_vertical() const;
    int  blue_horizontal() const;

    object red_object() const;
    bool red_exists() const;
    int  red_width() const;
    int  red_height() const;
    int  red_vertical() const;
    int  red_horizontal() const;
};