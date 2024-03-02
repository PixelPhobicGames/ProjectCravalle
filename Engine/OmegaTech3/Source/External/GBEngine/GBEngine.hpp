
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "peanut_gb.h"
#include "raylib.h"

struct priv_t
{
	/* Pointer to allocated memory holding GB file. */
	uint8_t *rom;
	/* Pointer to allocated memory holding save file. */
	uint8_t *cart_ram;

	/* Frame buffer */
	uint32_t fb[LCD_HEIGHT][LCD_WIDTH];
};

/**
 * Returns a byte from the ROM file at the given address.
 */


char *rom_file_name = NULL;
static struct gb_s gb;
static struct priv_t priv;
enum gb_init_error_e ret;

uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr)
{
	const struct priv_t * const p = static_cast<const struct priv_t*>(gb->direct.priv);
	return p->rom[addr];
}

/**
 * Returns a byte from the cartridge RAM at the given address.
 */
uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr)
{
	const struct priv_t * const p = static_cast<const struct priv_t*>(gb->direct.priv);
	return p->cart_ram[addr];
}

/**
 * Writes a given byte to the cartridge RAM at the given address.
 */
void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr,
		       const uint8_t val)
{
    const struct priv_t* const p = static_cast<const struct priv_t*>(gb->direct.priv);

	p->cart_ram[addr] = val;
}

/**
 * Returns a pointer to the allocated space containing the ROM. Must be freed.
 */
uint8_t *read_rom_to_ram(const char *file_name)
{
	FILE *rom_file = fopen(file_name, "rb");
	size_t rom_size;
	uint8_t *rom = NULL;

	if(rom_file == NULL)
		return NULL;

	fseek(rom_file, 0, SEEK_END);
	rom_size = ftell(rom_file);
	rewind(rom_file);
	rom = static_cast<uint8_t*>(malloc(rom_size));


	if(fread(rom, sizeof(uint8_t), rom_size, rom_file) != rom_size)
	{
		free(rom);
		fclose(rom_file);
		return NULL;
	}

	fclose(rom_file);
	return rom;
}

/**
 * Ignore all errors.
 */
void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val)
{
	const char* gb_err_str[GB_INVALID_MAX] = {
		"UNKNOWN",
		"INVALID OPCODE",
		"INVALID READ",
		"INVALID WRITE",
		"HALT FOREVER"
	};

	struct priv_t *priv = static_cast<struct priv_t*>(gb->direct.priv);

	free(priv->cart_ram);
	free(priv->rom);
	exit(EXIT_FAILURE);
}


void lcd_draw_line(struct gb_s *gb, const uint8_t pixels[160], const uint_least8_t line)
{
    const uint32_t palette[] = { 0x9bbc0fFF, 0x8bac0fFF, 0x306230FF, 0x0f380fFF };


    for (unsigned int x = 0; x < LCD_WIDTH; x++)
    {
        Color pixelColor = GetColor(palette[pixels[x] & 3]);
        DrawPixel(x, line, pixelColor);
    }
}

static RenderTexture2D GBTarget;

char* convertToChar(const char* source) {
    // Check if the source pointer is valid
    if (source == NULL) {
        return NULL;
    }

    // Calculate the length of the source string
    size_t length = strlen(source);

    // Allocate memory for the new char*
    char* result = (char*)malloc(length + 1);

    // Check if memory allocation was successful
    if (result == NULL) {
        return NULL;
    }

    // Copy the content of the source to the new memory
    strcpy(result, source);

    // Return the char* pointing to the new memory
    return result;
}

void InitGB(char *Path){
    rom_file_name = Path;

	if((priv.rom = read_rom_to_ram(rom_file_name)) == NULL)
	{
		printf("%d: %s\n", __LINE__, strerror(errno));
		exit(EXIT_FAILURE);
	}

	ret = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read,
		      &gb_cart_ram_write, &gb_error, &priv);

	if(ret != GB_INIT_NO_ERROR)
	{
		printf("Error: %d\n", ret);
		exit(EXIT_FAILURE);
	}

	priv.cart_ram = static_cast<uint8_t*>(malloc(gb_get_save_size(&gb)));

    gb_init_lcd(&gb, &lcd_draw_line);

    GBTarget = LoadRenderTexture(160 , 144);

}

int FrameCounter = 0;
int NOPTicks = 0;

int StopCounter = 0;

void RunGB(){
    while(1 && !WindowShouldClose()){
        BeginTextureMode(GBTarget);
            ClearBackground(BLACK);

            const double target_speed_us = 1000000.0 / VERTICAL_SYNC;
            int_fast16_t delay;
            unsigned long start, end;
            struct timeval timecheck;
            int state;

            gettimeofday(&timecheck, NULL);
            start = (long)timecheck.tv_sec * 1000000 +
                (long)timecheck.tv_usec;

            gb_run_frame(&gb);

            state = 0;

            if(state < 0)
                break;

            gettimeofday(&timecheck, NULL);
            end = (long)timecheck.tv_sec * 1000000 +
                (long)timecheck.tv_usec;

            delay = target_speed_us - (end - start);

            if(delay < 0)
                continue;

            usleep(delay);

        EndTextureMode();


        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(GBTarget.texture, {0 , 0 , GBTarget.texture.width , -GBTarget.texture.height} , { GetScreenWidth() / 4 , 25 , GetScreenWidth() / 2 , GetScreenHeight() - 50} , {0 , 0} , 0.0f , WHITE);

        EndDrawing();

        if (IsKeyDown(KEY_W) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)){
            gb.direct.joypad_bits.up = false;
        }
        else {
            gb.direct.joypad_bits.up = true;
        }

        if (IsKeyDown(KEY_S) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)){
            gb.direct.joypad_bits.down = false;
        }
        else {
            gb.direct.joypad_bits.down = true;
        }

        if (IsKeyDown(KEY_A) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)){
            gb.direct.joypad_bits.left = false;
        }
        else {
            gb.direct.joypad_bits.left = true;
        }

        if (IsKeyDown(KEY_D) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)){
            gb.direct.joypad_bits.right = false;
        }
        else {
            gb.direct.joypad_bits.right = true;
        }

        if (IsMouseButtonPressed(0) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
            gb.direct.joypad_bits.a = false;
        }
        else {
            gb.direct.joypad_bits.a = true;
        }

        if (IsMouseButtonPressed(1) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)){
            gb.direct.joypad_bits.b = false;
        }
        else {
            gb.direct.joypad_bits.b = true;
        }

        if (FrameCounter == 60){
            FrameCounter = 0;
            NOPTicks = NOPCounter;

            if (int(NOPTicks) == 915){
                StopCounter ++;
            }

            NOPCounter = 0;
        }
        else {
            FrameCounter ++;
        }

        if (StopCounter == 5){
            break;
        }
	}
}