#include "../includes/cub3d.h"
#include <mlx.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#define WIDTH 1248
#define HEIGHT 960
#define MOVE_SPEED 0.09
#define ROT_SPEED 0.055
#define IS_FLOOR(c) ((c) == '0' || (c) == 'N' || (c) == 'S' || (c) == 'E' || (c) == 'W' || (c) == 'O') // TODO: refactor esto, meter en map_utils junto a is_valid_map_char

#define MINIMAP_SCALE 12         // Size of each map cell in pixels
#define MINIMAP_MARGIN 10        // Margin from the top-left corner
#define MINIMAP_VIEW_RADIUS 5    // Radius of cells to display around the player

#define COLOR_WALL 0x404040      // Dark gray for walls ('1', 'D')
#define COLOR_FLOOR 0x808080     // Medium gray for floor ('0', 'O', 'N', 'S', 'E', 'W')
#define COLOR_PLAYER 0xFF0000    // Red for the player
#define COLOR_EMPTY 0x000000     // Black for out-of-bounds cells

/* Prototipo de la función centralizada de salida (implementada en main.c) */
int exit_program(t_cub3d *cub);
void ft_error(char *message, t_cub3d *cub, char *line);
void draw_door_prompt(t_cub3d *cub);

static int get_wall_texture(int side, double rayDirX, double rayDirY)
{
	if (side == 0)
	{
		/* impacto en pared vertical (se avanzó en X): elegir WE o EA según rayDirX */
		if (rayDirX > 0)
			return 2; /* WE */
		else
			return 3; /* EA */
	}
	else
	{
		/* impacto en pared horizontal (se avanzó en Y): elegir SO o NO según rayDirY */
		if (rayDirY > 0)
			return 1; /* SO */
		else
			return 0; /* NO */
	}
}

/* Convierte componentes r,g,b (0-255) a entero RGB 0xRRGGBB */
static int	rgb_to_int(int r, int g, int b)
{
	return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

static void draw_pixel(char *img_data, int size_line, int bpp, int x, int y, int color)
{
    int bytes_per_pixel = bpp / 8;
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
    {
        unsigned int *pixel = (unsigned int *)(img_data + (y * size_line + x * bytes_per_pixel));
        *pixel = (unsigned int)color;
    }
}

void draw_minimap(t_cub3d *cub, char *img_data, int size_line, int bpp)
{
    int map_cell_x, map_cell_y, pixel_x, pixel_y, color;
    int center_map_x = (int)cub->pos_x;
    int center_map_y = (int)cub->pos_y;
    int minimap_start_x = MINIMAP_MARGIN;
    int minimap_start_y = MINIMAP_MARGIN;

    for (int i = -MINIMAP_VIEW_RADIUS; i <= MINIMAP_VIEW_RADIUS; i++)
    {
        for (int j = -MINIMAP_VIEW_RADIUS; j <= MINIMAP_VIEW_RADIUS; j++)
        {
            map_cell_x = center_map_x + j;
            map_cell_y = center_map_y + i;
            int cell_draw_x = minimap_start_x + (j + MINIMAP_VIEW_RADIUS) * MINIMAP_SCALE;
            int cell_draw_y = minimap_start_y + (i + MINIMAP_VIEW_RADIUS) * MINIMAP_SCALE;

            color = COLOR_EMPTY;
            if (map_cell_y >= 0 && map_cell_y < cub->map_height &&
                map_cell_x >= 0 && cub->map[map_cell_y] &&
                map_cell_x < (int)strlen(cub->map[map_cell_y]))
            {
                char cell = cub->map[map_cell_y][map_cell_x];
                if (cell == '1' || cell == 'D')
                    color = COLOR_WALL;
                else if (IS_FLOOR(cell))
                    color = COLOR_FLOOR;
            }

            for (pixel_y = 0; pixel_y < MINIMAP_SCALE; pixel_y++)
            {
                for (pixel_x = 0; pixel_x < MINIMAP_SCALE; pixel_x++)
                {
                    if (pixel_x == 0 || pixel_y == 0)
                        draw_pixel(img_data, size_line, bpp, cell_draw_x + pixel_x, cell_draw_y + pixel_y, 0x000000);
                    else
                        draw_pixel(img_data, size_line, bpp, cell_draw_x + pixel_x, cell_draw_y + pixel_y, color);
                }
            }
        }
    }

    int player_center_x = minimap_start_x + MINIMAP_VIEW_RADIUS * MINIMAP_SCALE + MINIMAP_SCALE / 2;
    int player_center_y = minimap_start_y + MINIMAP_VIEW_RADIUS * MINIMAP_SCALE + MINIMAP_SCALE / 2;
    double relative_x = cub->pos_x - center_map_x;
    double relative_y = cub->pos_y - center_map_y;
    int player_pixel_x = player_center_x + (int)((relative_x - 0.5) * MINIMAP_SCALE);
    int player_pixel_y = player_center_y + (int)((relative_y - 0.5) * MINIMAP_SCALE);

    int player_size = MINIMAP_SCALE / 4;
    for (int y = player_pixel_y - player_size / 2; y < player_pixel_y + player_size / 2; y++)
    {
        for (int x = player_pixel_x - player_size / 2; x < player_pixel_x + player_size / 2; x++)
        {
            draw_pixel(img_data, size_line, bpp, x, y, COLOR_PLAYER);
        }
    }

    int dir_len = MINIMAP_SCALE / 2;
    int dir_end_x = player_pixel_x + (int)(cub->dir_x * dir_len);
    int dir_end_y = player_pixel_y + (int)(cub->dir_y * dir_len);
    draw_pixel(img_data, size_line, bpp, dir_end_x, dir_end_y, COLOR_PLAYER);
    draw_pixel(img_data, size_line, bpp, dir_end_x + 1, dir_end_y, COLOR_PLAYER);
    draw_pixel(img_data, size_line, bpp, dir_end_x, dir_end_y + 1, COLOR_PLAYER);
    draw_pixel(img_data, size_line, bpp, dir_end_x - 1, dir_end_y, COLOR_PLAYER);
    draw_pixel(img_data, size_line, bpp, dir_end_x, dir_end_y - 1, COLOR_PLAYER);
}

void	raycast_render(t_cub3d *cub, char *img_data, int size_line, int bpp)
{
	int bytes_per_pixel = bpp / 8;

	/* Precalcular colores de techo/suelo usando los valores parseados.
	 * Si algún componente no está definido (-1) usamos un color por defecto. */
	int ceiling_color;
	int floor_color;
	if (cub->textures.ceiling.r != -1)
		ceiling_color = rgb_to_int(cub->textures.ceiling.r,
				cub->textures.ceiling.g, cub->textures.ceiling.b);
	else
		ceiling_color = 0x202020; /* fallback */

	if (cub->textures.floor.r != -1)
		floor_color = rgb_to_int(cub->textures.floor.r,
				cub->textures.floor.g, cub->textures.floor.b);
	else
		floor_color = 0x707070; /* fallback */

	for (int x = 0; x < WIDTH; x++)
	{
		double cameraX = 2 * x / (double)WIDTH - 1;
		double rayDirX = cub->dir_x + cub->plane_x * cameraX;
		double rayDirY = cub->dir_y + cub->plane_y * cameraX;
		int mapX = (int)cub->pos_x;
		int mapY = (int)cub->pos_y;
		double sideDistX, sideDistY;
		double deltaDistX = rayDirX == 0 ? 1e30 : fabs(1 / rayDirX);
		double deltaDistY = rayDirY == 0 ? 1e30 : fabs(1 / rayDirY);
		double perpWallDist;
		int stepX, stepY, hit = 0, side;

		if (rayDirX < 0)
		{	stepX = -1; sideDistX = (cub->pos_x - mapX) * deltaDistX; }
		else
		{	stepX = 1;  sideDistX = (mapX + 1.0 - cub->pos_x) * deltaDistX; }
		if (rayDirY < 0)
		{	stepY = -1; sideDistY = (cub->pos_y - mapY) * deltaDistY; }
		else
		{	stepY = 1;  sideDistY = (mapY + 1.0 - cub->pos_y) * deltaDistY; }

		while (!hit)
		{
			if (sideDistX < sideDistY)
			{	sideDistX += deltaDistX; mapX += stepX; side = 0; }
			else
			{	sideDistY += deltaDistY; mapY += stepY; side = 1; }
			if (mapY < 0 || mapY >= cub->map_height || mapX < 0 || mapX >= (int)strlen(cub->map[mapY]))
				break;
			char cell = cub->map[mapY][mapX];
			if (cell == '1' || (cub->is_bonus && cell == 'D'))
				hit = 1;
		}
		if (!hit) continue;
		if (side == 0)
			perpWallDist = (sideDistX - deltaDistX);
		else
			perpWallDist = (sideDistY - deltaDistY);

		int lineHeight = (int)(HEIGHT / perpWallDist);
		int drawStart = -lineHeight / 2 + HEIGHT / 2;
		if (drawStart < 0) drawStart = 0;
		int drawEnd = lineHeight / 2 + HEIGHT / 2;
		if (drawEnd >= HEIGHT) drawEnd = HEIGHT - 1;

		char cell_hit = cub->map[mapY][mapX];
		int *current_texture_buffer;
		int texNum;

		// Correct neighbor logic to check for adjacent open doors ('O')
		int neighborX = mapX;
		int neighborY = mapY;

		if (side == 0) // Vertical wall
		{
			neighborX = mapX - stepX;
		}
		else // Horizontal wall
		{
			neighborY = mapY - stepY;
		}

		int is_door_adjacent = 0;
		if (cub->is_bonus && neighborY >= 0 && neighborY < cub->map_height &&
			neighborX >= 0 && neighborX < (int)strlen(cub->map[neighborY]) &&
			cub->map[neighborY][neighborX] == 'O')
		{
			is_door_adjacent = 1;
		}

		// Texture selection
		if (cub->is_bonus && cell_hit == 'D' && cub->door_textures != NULL && cub->door_textures[0] != NULL)
		{
			current_texture_buffer = cub->door_textures[0]; // Closed door texture
		}
		else if (is_door_adjacent)
		{
			current_texture_buffer = cub->door_textures[1]; // Open door texture for adjacent walls
		}
		else
		{
			texNum = get_wall_texture(side, rayDirX, rayDirY);
			current_texture_buffer = cub->wall_textures[texNum]; // Regular wall texture
		}

		double wallX;

		if (side == 0)
			wallX = cub->pos_y + perpWallDist * rayDirY;
		else
			wallX = cub->pos_x + perpWallDist * rayDirX;
        wallX -= floor(wallX);

		int texX = (int)(wallX * (double)cub->tex_width);
            if (side == 0 && rayDirX > 0) texX = cub->tex_width - texX - 1;
            if (side == 1 && rayDirY < 0) texX = cub->tex_width - texX - 1;

		double step = 1.0 * cub->tex_height / lineHeight;
		double texPos = (drawStart - HEIGHT / 2 + lineHeight / 2) * step;

		// Fondo (techo y suelo) usando colores parseados
		for (int y = 0; y < drawStart; y++)
		{
			unsigned int *pixel = (unsigned int *)(img_data + (y * size_line + x * bytes_per_pixel));
			*pixel = (unsigned int)ceiling_color;
		}
		for (int y = drawEnd + 1; y < HEIGHT; y++)
		{
			unsigned int *pixel = (unsigned int *)(img_data + (y * size_line + x * bytes_per_pixel));
			*pixel = (unsigned int)floor_color;
		}

		// --- Dibuja la pared con textura ---
		for (int y = drawStart; y <= drawEnd; y++)
		{
			int texY = (int)texPos & (cub->tex_height - 1);
			texPos += step;

			int color = current_texture_buffer[cub->tex_width * texY + texX];
			unsigned int *pixel = (unsigned int *)(img_data + (y * size_line + x * bytes_per_pixel));
			if (side == 1)
				color = (color >> 1) & 0x7F7F7F;
			*pixel = (unsigned int)color;
		}
	}
}

void handle_door_action(t_cub3d *cub)
{
    // Calculate the map position the player is facing (1.0 unit ahead)
    int target_mapX = (int)(cub->pos_x + cub->dir_x * 1.0); 
    int target_mapY = (int)(cub->pos_y + cub->dir_y * 1.0);

    // Check map boundaries
    if (target_mapY < 0 || target_mapY >= cub->map_height ||
        target_mapX < 0 || target_mapX >= (int)strlen(cub->map[target_mapY]))
        return;

    char *target_cell = &cub->map[target_mapY][target_mapX];

    if (cub->is_bonus)
    {
        // Case 1: Open a closed door
        if (*target_cell == 'D')
        {
            *target_cell = 'O'; // Open the door
        }
        // Case 2: Close an open door
        else if (*target_cell == 'O')
        {
            // Prevent closing the door if the player is inside the door cell
            int player_mapX = (int)cub->pos_x;
            int player_mapY = (int)cub->pos_y;

            if (target_mapX == player_mapX && target_mapY == player_mapY)
            {
                // Player is inside the door cell, do not close
                return; 
            }

            // Close the door
            *target_cell = 'D';
        }
    }
}

int handle_key_press(int key, t_cub3d *cub)
{
    if (key == 119) // W
        cub->is_moving_forward = 1;
    else if (key == 115) // S
        cub->is_moving_backward = 1;
    else if (key == 97 || key == 65361) // A o Flecha Izquierda
        cub->is_rotating_left = 1;
    else if (key == 100 || key == 65363) // D o Flecha Derecha
        cub->is_rotating_right = 1;
    else if (key == 65307) // ESC
        return (exit_program(cub));
	else if (key == 49 || key == 32 || key == 65349)
        handle_door_action(cub);
    return (0);
}

int handle_key_release(int key, t_cub3d *cub)
{
    if (key == 119) // W
        cub->is_moving_forward = 0;
    else if (key == 115) // S
        cub->is_moving_backward = 0;
    else if (key == 97 || key == 65361) // A o Flecha Izquierda
        cub->is_rotating_left = 0;
    else if (key == 100 || key == 65363) // D o Flecha Derecha
        cub->is_rotating_right = 0;
    return (0);
}

void perform_movements(t_cub3d *cub)
{
    double oldDirX, oldPlaneX;
    double move_speed_dt;
    double rot_speed_dt;

    move_speed_dt = MOVE_SPEED * cub->time_frame * 60.0;
    rot_speed_dt = ROT_SPEED * cub->time_frame * 60.0;
    
    // Adelante
    if (cub->is_moving_forward) {
        if (IS_FLOOR(cub->map[(int)(cub->pos_y)][(int)(cub->pos_x + cub->dir_x * move_speed_dt)]))
            cub->pos_x += cub->dir_x * move_speed_dt;
        if (IS_FLOOR(cub->map[(int)(cub->pos_y + cub->dir_y * move_speed_dt)][(int)(cub->pos_x)]))
            cub->pos_y += cub->dir_y * move_speed_dt;
    }
    // Atrás
    if (cub->is_moving_backward) {
        if (IS_FLOOR(cub->map[(int)(cub->pos_y)][(int)(cub->pos_x - cub->dir_x * move_speed_dt)]))
            cub->pos_x -= cub->dir_x * move_speed_dt;
        if (IS_FLOOR(cub->map[(int)(cub->pos_y - cub->dir_y * move_speed_dt)][(int)(cub->pos_x)]))
            cub->pos_y -= cub->dir_y * move_speed_dt;
    }
    // Rotar Derecha
    if (cub->is_rotating_right) {
        oldDirX = cub->dir_x;
        cub->dir_x = cub->dir_x * cos(rot_speed_dt) - cub->dir_y * sin(rot_speed_dt);
        cub->dir_y = oldDirX * sin(rot_speed_dt) + cub->dir_y * cos(rot_speed_dt);
        oldPlaneX = cub->plane_x;
        cub->plane_x = cub->plane_x * cos(rot_speed_dt) - cub->plane_y * sin(rot_speed_dt);
        cub->plane_y = oldPlaneX * sin(rot_speed_dt) + cub->plane_y * cos(rot_speed_dt);
    }
    // Rotar Izquierda
    if (cub->is_rotating_left) {
        oldDirX = cub->dir_x;
        cub->dir_x = cub->dir_x * cos(-rot_speed_dt) - cub->dir_y * sin(-rot_speed_dt);
        cub->dir_y = oldDirX * sin(-rot_speed_dt) + cub->dir_y * cos(-rot_speed_dt);
        oldPlaneX = cub->plane_x;
        cub->plane_x = cub->plane_x * cos(-rot_speed_dt) - cub->plane_y * sin(-rot_speed_dt);
        cub->plane_y = oldPlaneX * sin(-rot_speed_dt) + cub->plane_y * cos(-rot_speed_dt);
    }
}

long fn_get_time_in_ms(void)
{
    struct timeval tv;
    
    if (gettimeofday(&tv, NULL) == -1)
        return (0);
        
    return ((long)(tv.tv_sec * 1000 + tv.tv_usec / 1000));
}

// Bucle de renderizado
int render_loop(t_cub3d *cub)
{
    long current_time = fn_get_time_in_ms();
    cub->time_frame = (current_time - cub->time_prev) / 1000.0;
    cub->time_prev = current_time;
    perform_movements(cub);
    raycast_render(cub, cub->img_data, cub->size_line, cub->bpp);
    if (cub->is_bonus)
        draw_minimap(cub, cub->img_data, cub->size_line, cub->bpp);
    mlx_put_image_to_window(cub->mlx, cub->win, cub->img, 0, 0);
    draw_door_prompt(cub);
    return (0);
}

/**
 * @brief Draws the door action prompt based on door state, checking for two cases:
 * 1. Player is standing inside the door cell ('O' open).
 * 2. Player is facing a door ('D' or 'O') within interaction distance.
 *
 * @param cub Pointer to the main Cub3D structure.
 */
void draw_door_prompt(t_cub3d *cub)
{
    int target_mapX;
    int target_mapY;
    const char *message;
    char target_cell;
    
    const double check_dist_near = 0.75; 
    const double check_dist_far = 1.5; 

    target_mapX = (int)(cub->pos_x + cub->dir_x * check_dist_near);  
    target_mapY = (int)(cub->pos_y + cub->dir_y * check_dist_near);
    
    if ((int)cub->pos_x == target_mapX && (int)cub->pos_y == target_mapY)
    {
        target_mapX = (int)(cub->pos_x + cub->dir_x * check_dist_far);
        target_mapY = (int)(cub->pos_y + cub->dir_y * check_dist_far);
    }
    
    if (target_mapY < 0 || target_mapY >= cub->map_height ||
        target_mapX < 0 || target_mapX >= (int)strlen(cub->map[target_mapY]))
        return;

    target_cell = cub->map[target_mapY][target_mapX];

    if (cub->is_bonus && (target_cell == 'D' || target_cell == 'O'))
    {
        if (target_cell == 'D')
            message = "Press SPACE to open";
        else
            message = "Press SPACE to close";
            
        mlx_string_put(cub->mlx, cub->win,
            (WIDTH / 2) - 100, 
            HEIGHT - 50,
            0xFFFFFF,
            (char *)message);
    }
}

// Inicializa la posición/dirección del jugador según el mapa
void	init_player(t_cub3d *cub)
{
	for (int y = 0; y < cub->map_height; y++)
	for (int x = 0; x < (int)strlen(cub->map[y]); x++)
	{
		if (cub->map[y][x] == 'N' || cub->map[y][x] == 'S' ||
			cub->map[y][x] == 'E' || cub->map[y][x] == 'W')
		{
			cub->pos_x = x + 0.5;
			cub->pos_y = y + 0.5;
			if (cub->map[y][x] == 'N')
			{ cub->dir_x = 0; cub->dir_y = -1; cub->plane_x = 0.66; cub->plane_y = 0; }
			else if (cub->map[y][x] == 'S')
			{ cub->dir_x = 0; cub->dir_y = 1; cub->plane_x = -0.66; cub->plane_y = 0; }
			else if (cub->map[y][x] == 'E')
			{ cub->dir_x = 1; cub->dir_y = 0; cub->plane_x = 0; cub->plane_y = 0.66; }
			else if (cub->map[y][x] == 'W')
			{ cub->dir_x = -1; cub->dir_y = 0; cub->plane_x = 0; cub->plane_y = -0.66; }
			return;
		}
	}
}