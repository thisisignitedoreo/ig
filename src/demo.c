
#include "raylib.h"

#define IG_IMPLEMENTATION
#include "ig.h"

int main(void) {
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(800, 600, "ig demo");
	
	ig_init();

	int font_size = 18;
	Font font1 = LoadFontEx("font1.ttf", font_size, 0, 0);
	Font font2 = LoadFontEx("font2.ttf", font_size, 0, 0);
	ig_style.font_size = font_size;
	ig_style.font = font1;
	ig_style.spacing = 0;
	// ig_neon = true;

	bool state = false;
	bool fontstate = false;
	int selected = 0;

	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground((Color){0, 0, 0, 255});

			ig_label("ig demo");

			ig_begin_window("main", IG_TOPBAR, (Rectangle) {GetScreenWidth()/4, GetScreenHeight()/4, GetScreenWidth()/2, GetScreenHeight()/2});
			if (ig_button("Button", 0)) state = !state;
			ig_checkbox("Checkbox", &state);
			ig_radio("1", 1, &selected);
			ig_radio("2", 2, &selected);
			ig_radio("3", 3, &selected);
			if (ig_button("Reset", 0)) selected = 0;
			for (int i = 0; i < 20; i++) ig_label("A lot of labels will make a scrollbar appear");
			ig_end_window();

			ig_begin_window("Floating Window", IG_RESIZABLE | IG_MOVABLE | IG_TOPBAR, (Rectangle) {GetScreenWidth()/2-100, 10, 200, 100});
			if (ig_button("Switch font", 0)) {
				fontstate = !fontstate;
				if (fontstate) ig_style.font = font2;
				else ig_style.font = font1;
			}
			ig_label("You can resize me too!");
			for (int i = 0; i < 20; i++) ig_label("A lot of labels will make a scrollbar appear");
			ig_end_window();

			ig_render();
		EndDrawing();
	}

	UnloadFont(font1);
	UnloadFont(font2);
	ig_free();

	CloseWindow();

	return 0;
}

