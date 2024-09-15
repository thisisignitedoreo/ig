
#ifndef IG_H_
#define IG_H_

#define IGAPI

#define DA_IMPL
#include "da.h"

typedef struct {
    const char* title;
    Rectangle rect;
    float scroll;
    int zorder;
    RenderTexture2D texture;
} IgWindow;

typedef struct {
    Font font;
    int font_size;
    int pad;
    int spacing;
    Color bg_color;
    Color fg_color;
    Color accent_color;
    Color abg_color;
    Color ibg_color;
    Color stroke_color;
    Color neon_color;
} IgStyle;

#define IG_RESIZABLE (1 << 0)
#define IG_MOVABLE (1 << 1)
#define IG_TOPBAR (1 << 2)

IGAPI void ig_init();
// Initialize internal ig ig_context && ig_style

IGAPI void ig_free();
// De-initialize internal ig ig_context && ig_style

IGAPI void ig_begin_window(const char* title, int flags, Rectangle rects);
// Begin window

IGAPI void ig_end_window();
// End window

IGAPI bool ig_button(const char* title, bool* held_down);
// Button

IGAPI void ig_label(const char* text);
// Label

IGAPI bool ig_radio(const char* text, int value, int *store);
// Radio-button

IGAPI bool ig_checkbox(const char* text, bool* store);
// Checkbox

IGAPI void ig_render();
// Blit all windows to the screen

#ifdef IG_IMPLEMENTATION

IgStyle ig_style = {0};
Rectangle ig_render_rect = {0};

IgWindow* ig_windows = 0;

int ig_dragging = -1;
Vector2 ig_dragging_origin = {0};
int ig_tempy = 0;
int ig_topbar_size = 0;
int ig_resize_border = 4;
int ig_resize_overhang = 3;
int ig_scroll_bar = 14;
bool ig_resizing[8] = {0};
int ig_resizing_window = -1;
int ig_active_window = -1;
bool ig_neon = false;
IgWindow* ig_window = 0;
int ig_w = -1;

bool ig_held = false;

IGAPI void ig_init() {
    ig_style.font = GetFontDefault();
    ig_style.font_size = 16;
    ig_style.pad = 4;
    ig_style.spacing = 2;
    ig_style.bg_color = (Color) {30, 30, 30, 255};
    ig_style.fg_color = (Color) {255, 255, 255, 255};
    ig_style.accent_color = (Color) {200, 200, 255, 255};
    ig_style.abg_color = (Color) {50, 50, 150, 255};
    ig_style.ibg_color = (Color) {30, 30, 130, 255};
    ig_style.stroke_color = (Color) {100, 100, 100, 255};
    ig_style.neon_color = (Color) {0, 255, 0, 50};

    ig_windows = da_new(IgWindow);
}

IGAPI void ig_free() {
    for (size_t i = 0; i < da_length(ig_windows); i++) UnloadRenderTexture(ig_windows[i].texture);
    da_free(ig_windows);
}

IGAPI void ig_begin_window(const char* title, int flags, Rectangle rects) {
    int w = -1;

    bool topbar = flags & IG_TOPBAR;
    bool resizing = flags & IG_RESIZABLE;
    bool moving = flags & IG_MOVABLE;

    int max_zorder = -1;

    for (size_t i = 0; i < da_length(ig_windows); i++) {
        if (strcmp(ig_windows[i].title, title) == 0) {
            w = i;
        }
        if (ig_windows[i].zorder > max_zorder) {
            max_zorder = ig_windows[i].zorder;
        }
    }
    if (w == -1) {
        w = da_length(ig_windows);
        IgWindow window = {title, rects, 0, max_zorder+1, {0}};
        max_zorder++;
        ig_windows = _da_push(ig_windows, &window);
    }

    ig_w = w;
    
    IgWindow window = ig_windows[w];
    Vector2 title_size = MeasureTextEx(ig_style.font, title, ig_style.font_size, ig_style.spacing);

    int topbar_size;
    if (topbar) topbar_size = title_size.y + ig_style.pad*2;
    else topbar_size = 0;
    ig_topbar_size = topbar_size;
    ig_tempy = topbar_size;

    Rectangle rect = { window.rect.x, window.rect.y, window.rect.width, topbar_size };
    Rectangle rectfull = { window.rect.x, window.rect.y, window.rect.width, topbar_size+window.rect.height };

    if (IsMouseButtonDown(0) && CheckCollisionPointRec(GetMousePosition(), rectfull) && ig_active_window != w && ig_resizing_window == -1 && ig_dragging == -1) {
        ig_active_window = w;
        // ig_windows[max_zorder_w].zorder = ig_windows[w].zorder;
        ig_windows[w].zorder = max_zorder+1;
    }

    Rectangle resize_rect_u = {
        window.rect.x-ig_resize_border,
        window.rect.y-ig_resize_border,
        window.rect.width + ig_resize_border*2,
        ig_resize_border + ig_resize_overhang
    };
    Rectangle resize_rect_d = {
        window.rect.x-ig_resize_border,
        window.rect.y+window.rect.height+topbar_size - ig_resize_overhang,
        window.rect.width + ig_resize_border*2,
        ig_resize_border + ig_resize_overhang
    };
    Rectangle resize_rect_l = {
        window.rect.x-ig_resize_border,
        window.rect.y-ig_resize_border,
        ig_resize_border+ig_resize_overhang,
        window.rect.height + topbar_size + ig_resize_border*2
    };
    Rectangle resize_rect_r = {
        window.rect.x+window.rect.width-ig_resize_overhang,
        window.rect.y-ig_resize_border,
        ig_resize_border+ig_resize_overhang,
        window.rect.height + topbar_size + ig_resize_border*2
    };

    if (ig_windows[w].texture.texture.width == 0) {
        ig_windows[w].texture = LoadRenderTexture(rectfull.width, rectfull.height+1);
    }
    BeginTextureMode(ig_windows[w].texture);
    ClearBackground((Color) {0, 0, 0, 0});

    EndTextureMode();
    if (ig_neon && resizing) {
        if (ig_dragging == -1 && ig_resizing_window == -1) {
            if (CheckCollisionPointRec(GetMousePosition(), resize_rect_u))  { DrawRectangleRec(resize_rect_u, ig_style.neon_color); }
            if (CheckCollisionPointRec(GetMousePosition(), resize_rect_d))  { DrawRectangleRec(resize_rect_d, ig_style.neon_color); }
            if (CheckCollisionPointRec(GetMousePosition(), resize_rect_l))  { DrawRectangleRec(resize_rect_l, ig_style.neon_color); }
            if (CheckCollisionPointRec(GetMousePosition(), resize_rect_r))  { DrawRectangleRec(resize_rect_r, ig_style.neon_color); }
        }
    }
    BeginTextureMode(ig_windows[w].texture);

    if (IsMouseButtonDown(0) && ig_resizing_window == -1 && ig_dragging == -1 && resizing) {
        if (CheckCollisionPointRec(GetMousePosition(), resize_rect_u)) { ig_resizing[0] = true; ig_resizing_window = w; }
        if (CheckCollisionPointRec(GetMousePosition(), resize_rect_d)) { ig_resizing[1] = true; ig_resizing_window = w; }
        if (CheckCollisionPointRec(GetMousePosition(), resize_rect_l)) { ig_resizing[2] = true; ig_resizing_window = w; }
        if (CheckCollisionPointRec(GetMousePosition(), resize_rect_r)) { ig_resizing[3] = true; ig_resizing_window = w; }
    }
    if (resizing && ig_resizing_window == w && ig_resizing[0]) { ig_windows[ig_resizing_window].rect.y += GetMouseDelta().y; ig_windows[ig_resizing_window].rect.height -= GetMouseDelta().y;  }
    if (resizing && ig_resizing_window == w && ig_resizing[1]) { ig_windows[ig_resizing_window].rect.height += GetMouseDelta().y; }
    if (resizing && ig_resizing_window == w && ig_resizing[2]) { ig_windows[ig_resizing_window].rect.x += GetMouseDelta().x; ig_windows[ig_resizing_window].rect.width -= GetMouseDelta().x;  }
    if (resizing && ig_resizing_window == w && ig_resizing[3]) { ig_windows[ig_resizing_window].rect.width += GetMouseDelta().x; }

    if (ig_windows[w].rect.width < 5) ig_windows[ig_resizing_window].rect.width = 5;
    if (ig_windows[w].rect.height < 5) ig_windows[ig_resizing_window].rect.height = 5;

    if (resizing && IsMouseButtonReleased(0)) { ig_resizing_window = -1; ig_resizing[0] = false; ig_resizing[1] = false; ig_resizing[2] = false; ig_resizing[3] = false; ig_resizing[4] = false; ig_resizing[5] = false; ig_resizing[6] = false; ig_resizing[7] = false; SetMouseCursor(MOUSE_CURSOR_DEFAULT); }

    if (resizing && ig_resizing_window == w && (ig_resizing[0] || ig_resizing[1] || ig_resizing[2] || ig_resizing[3]))
    {
        EndTextureMode();
        UnloadRenderTexture(ig_windows[w].texture);
        ig_windows[w].texture = LoadRenderTexture(ig_windows[ig_resizing_window].rect.width, ig_windows[ig_resizing_window].rect.height+topbar_size+1);
        BeginTextureMode(ig_windows[w].texture);
    }

    if (moving && ig_resizing_window == -1 && IsMouseButtonPressed(0) && CheckCollisionPointRec(GetMousePosition(), rect) && ig_dragging == -1) { ig_dragging = w; }
    if (moving && ig_resizing_window == -1 && ig_dragging == w) {
        Vector2 pos = GetMouseDelta();
        ig_windows[w].rect.x += pos.x;
        ig_windows[w].rect.y += pos.y;
    }
    if (moving && ig_resizing_window == -1 && IsMouseButtonReleased(0) && ig_dragging == w) {
        ig_dragging = -1;
    }

    BeginScissorMode(0, 0, window.rect.width, topbar_size);
    ClearBackground(ig_active_window == w ? ig_style.abg_color : ig_style.ibg_color);
    DrawTextEx(ig_style.font, title, (Vector2) { ig_style.pad, ig_style.pad }, ig_style.font_size, ig_style.spacing, ig_style.fg_color);
    EndScissorMode();

    DrawRectangle(0, topbar_size, window.rect.width, window.rect.height, ig_style.bg_color);
    DrawRectangleLines(0, 0, window.rect.width, window.rect.height + topbar_size+1, ig_style.stroke_color);

    ig_render_rect = (Rectangle) { window.rect.x, window.rect.y, window.rect.width, window.rect.height };
    ig_window = ig_windows+w;

    BeginScissorMode(1, topbar ? topbar_size : 1, ig_render_rect.width - 2, topbar ? ig_render_rect.height - 1 : ig_render_rect.height - 2);
    ClearBackground(ig_style.bg_color);
    Camera2D cam = {(Vector2) {0, 0}, (Vector2) {ig_render_rect.x, ig_render_rect.y}, 0, 1.0f};
    BeginMode2D(cam);
}

IGAPI void ig_end_window() {
    EndMode2D();
    EndScissorMode();
    ig_tempy -= ig_topbar_size;
    if (ig_render_rect.height < ig_tempy) {
        DrawRectangle(ig_render_rect.width - ig_scroll_bar - 1, ig_topbar_size+1, ig_scroll_bar, ig_render_rect.height-1, ig_style.ibg_color);
        int sb_size = ig_render_rect.height-6;
        float diff = ig_tempy - sb_size;
        int scroller_size = 10;
        DrawRectangle(ig_render_rect.width - ig_scroll_bar + 1, ig_topbar_size + 3 + ig_window->scroll/diff*(sb_size-scroller_size), ig_scroll_bar - 4, scroller_size, ig_style.accent_color);
        if (CheckCollisionPointRec(GetMousePosition(), ig_render_rect) && ig_w == ig_active_window) {
            float new_scroll = ig_window->scroll + -10.0*GetMouseWheelMove();
            if (new_scroll < 0) new_scroll = 0.0f;
            if (new_scroll > diff) new_scroll = diff;
            ig_window->scroll = new_scroll;
        }
    }
    EndTextureMode();
    ig_w = -1;
    ig_window = 0;
    ig_tempy = 0;
    ig_render_rect = (Rectangle) {0};
}

IGAPI void ig_render() {
    int* zorder = da_new(int);
    for (size_t i = 0; i < da_length(ig_windows); i++) {
        da_push(zorder, i);
       
    }
    for (size_t i = 0; i < da_length(zorder); i++) {
        for (size_t j = 0; j < da_length(zorder)-i-1; j++) {
            if (ig_windows[zorder[j]].zorder > ig_windows[zorder[j+1]].zorder) {
                int t = zorder[j];
                zorder[j] = zorder[j+1];
                zorder[j+1] = t;
            }
        }
    }
    for (size_t i = 0; i < da_length(zorder); i++) {
        Texture texture = ig_windows[zorder[i]].texture.texture;
        DrawTextureRec(texture, (Rectangle) {.x = 0, .y = -texture.height, .width = texture.width, .height = -texture.height}, (Vector2) {ig_windows[zorder[i]].rect.x, ig_windows[zorder[i]].rect.y}, WHITE);
    }
}

IGAPI bool ig_button(const char* title, bool* held_down) {
    Vector2 mpos = GetMousePosition();

    Vector2 text_size = MeasureTextEx(ig_style.font, title, ig_style.font_size, ig_style.spacing);
    Vector2 rect_size = { text_size.x + ig_style.pad*2, text_size.y + ig_style.pad*2 };
    Rectangle rect = { ig_render_rect.x + ig_style.pad, ig_render_rect.y + ig_style.pad + ig_tempy- (ig_window == 0 ? 0 : ig_window->scroll), rect_size.x, rect_size.y };
    DrawRectangle(ig_render_rect.x + ig_style.pad, ig_render_rect.y + ig_style.pad + ig_tempy - (ig_window == 0 ? 0 : ig_window->scroll), rect_size.x, rect_size.y, CheckCollisionPointRec(mpos, rect) && ig_w == ig_active_window ? IsMouseButtonDown(0) ? ig_style.accent_color : ig_style.abg_color : ig_style.ibg_color);
    DrawTextEx(ig_style.font, title, (Vector2) { ig_render_rect.x + 2*ig_style.pad, ig_render_rect.y + ig_tempy + ig_style.pad*2 - (ig_window == 0 ? 0 : ig_window->scroll) }, ig_style.font_size, ig_style.spacing, CheckCollisionPointRec(mpos, rect) && ig_w == ig_active_window && IsMouseButtonDown(0) ? ig_style.bg_color : ig_style.fg_color);

    ig_tempy += rect_size.y + ig_style.pad;

    if (held_down != 0) *held_down = IsMouseButtonDown(0) && CheckCollisionPointRec(mpos, rect);

    return IsMouseButtonPressed(0) && CheckCollisionPointRec(mpos, rect) && ig_w == ig_active_window;
}

IGAPI void ig_label(const char* text) {
    Vector2 text_size = MeasureTextEx(ig_style.font, text, ig_style.font_size, ig_style.spacing);
    DrawTextEx(ig_style.font, text, (Vector2) { ig_render_rect.x + ig_style.pad, ig_render_rect.y + ig_tempy + ig_style.pad - (ig_window == 0 ? 0 : ig_window->scroll) }, ig_style.font_size, ig_style.spacing, ig_style.fg_color);

    ig_tempy += text_size.y + ig_style.pad;
}

IGAPI bool ig_checkbox(const char* text, bool *store) {
    Vector2 mpos = GetMousePosition();
    Vector2 text_size = MeasureTextEx(ig_style.font, text, ig_style.font_size, ig_style.spacing);
    Rectangle rect = {ig_render_rect.x + ig_style.pad + 5, ig_render_rect.y + ig_tempy + ig_style.pad - (ig_window == 0 ? 0 : ig_window->scroll), text_size.x + 20, text_size.y};
    DrawTextEx(ig_style.font, text, (Vector2) { 20 + ig_render_rect.x + ig_style.pad, ig_render_rect.y + ig_tempy + ig_style.pad - (ig_window == 0 ? 0 : ig_window->scroll) }, ig_style.font_size, ig_style.spacing, ig_style.fg_color);
    DrawRectangle(ig_render_rect.x + ig_style.pad + 5, text_size.y/2 + ig_render_rect.y + ig_tempy + ig_style.pad - 5 - (ig_window == 0 ? 0 : ig_window->scroll), 10, 10, CheckCollisionPointRec(mpos, rect) ? IsMouseButtonDown(0) ? ig_style.accent_color : ig_style.abg_color : ig_style.ibg_color);
    DrawRectangleLines(ig_render_rect.x + ig_style.pad + 5, text_size.y/2 + ig_render_rect.y + ig_tempy + ig_style.pad - 5 - (ig_window == 0 ? 0 : ig_window->scroll), 10, 10, ig_style.stroke_color);

    if (*store)
        DrawRectangle(ig_render_rect.x + ig_style.pad + 7, text_size.y/2 + ig_render_rect.y + ig_tempy + ig_style.pad - 3 - (ig_window == 0 ? 0 : ig_window->scroll), 6, 6, ig_style.stroke_color);
    if (CheckCollisionPointRec(mpos, rect) && IsMouseButtonPressed(0))
        *store = !*store;

    ig_tempy += text_size.y + ig_style.pad;
    return IsMouseButtonPressed(0) && CheckCollisionPointRec(mpos, rect) && ig_w == ig_active_window;
}

IGAPI bool ig_radio(const char* text, int value, int *store) {
    Vector2 mpos = GetMousePosition();
    Vector2 text_size = MeasureTextEx(ig_style.font, text, ig_style.font_size, ig_style.spacing);
    Rectangle rect = {ig_render_rect.x + ig_style.pad + 3, ig_render_rect.y + ig_tempy + ig_style.pad - (ig_window == 0 ? 0 : ig_window->scroll), text_size.x + 20, text_size.y};
    DrawTextEx(ig_style.font, text, (Vector2) { 20 + ig_render_rect.x + ig_style.pad, ig_render_rect.y + ig_tempy + ig_style.pad - (ig_window == 0 ? 0 : ig_window->scroll) }, ig_style.font_size, ig_style.spacing, ig_style.fg_color);
    DrawCircle(ig_render_rect.x + ig_style.pad + 10, text_size.y/2 + ig_render_rect.y + ig_tempy + ig_style.pad - (ig_window == 0 ? 0 : ig_window->scroll), 5, CheckCollisionPointRec(mpos, rect) ? IsMouseButtonDown(0) ? ig_style.accent_color : ig_style.abg_color : ig_style.ibg_color);
    DrawCircleLines(ig_render_rect.x + ig_style.pad + 10, text_size.y/2 + ig_render_rect.y + ig_tempy + ig_style.pad - (ig_window == 0 ? 0 : ig_window->scroll), 5, ig_style.stroke_color);

    if (*store == value) {
        DrawCircle(ig_render_rect.x + ig_style.pad + 10, text_size.y/2 + ig_render_rect.y + ig_tempy + ig_style.pad - (ig_window == 0 ? 0 : ig_window->scroll), 2.5, ig_style.stroke_color);
    }
    if (CheckCollisionPointRec(mpos, rect) && IsMouseButtonPressed(0)) {
        *store = value;
    }

    ig_tempy += text_size.y + ig_style.pad;
    if (ig_w != ig_active_window) return false;
    return IsMouseButtonPressed(0) && CheckCollisionPointRec(mpos, rect) && ig_w == ig_active_window;
}

#endif // IG_IMPLEMENTATION

#endif // IG_H_
