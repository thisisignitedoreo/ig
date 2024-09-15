
# ig
custom immediate mode gui library for raylib, bugs included

because raygui sucks

## quickstart
download src/ig.h, put it into your projects src folder, #define IG_IMPLEMENTATION, #include "ig.h"

## usage
- `void ig_init()`: initialize internal context and style
  
  call before main loop

- `void ig_free()`: de-initialize internal context and style
  
  call after main loop

- `void ig_render()`: blit all windows to screen
  
  call in main loop, after defining all windows
  
- `void ig_begin_window(const char* title, int flags, Rectangle rect)`: define a new window

  title is a string that's displayed in the window topbar

  flags can be IG_RESIZABLE, IG_MOVABLE, IG_TOPBAR or any combination of those with a pipe

  rect is a starting position and size of the window

  call in main loop, before rendering
  
- `void ig_end_window()`: end a window

  call in main loop, after begining a window

- `bool ig_button(const char* title, bool* held_down)`: draw a button
  
  title is a text displayed on the button

  held_down is a bool ptr, that is set when the button is held

  returns true on a click

  call in main loop, between begining and ending a window

- `void ig_label(const char* title)`: draw a text
  
  title is a text that is drawn (duh)

  call in main loop, between begining and ending a window

- `bool ig_radio(const char* text, int value, int *store)`: draw a radio button
  
  title is a text displayed on the radio button

  value is a value that will be stored in store ptr

  returns true on a click

  call in main loop, between begining and ending a window

- `bool ig_checkbox(const char* text, bool* store)`: draw a checkbox
  
  title is a text displayed on the radio button

  store is a bool-ptr, which value is flipped on click

  returns true on a click

  call in main loop, between begining and ending a window

see src/demo.c for an example
