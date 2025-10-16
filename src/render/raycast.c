#include "../includes/cub3d.h"
#include <mlx.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#define WIDTH 640
#define HEIGHT 480
#define MOVE_SPEED 0.09
#define ROT_SPEED 0.055
#define IS_FLOOR(c) ((c) == '0' || (c) == 'N' || (c) == 'S' || (c) == 'E' || (c) == 'W') // TODO: refactor esto, meter en map_utils junto a is_valid_map_char

/* Prototipo de la función centralizada de salida (implementada en main.c) */
int exit_program(t_cub3d *cub);

static int	get_wall_texture(int side, double rayDirX, double rayDirY)
{
	if (side == 0 && rayDirX < 0)
		return 0; // NO
	else if (side == 0 && rayDirX > 0)
		return 1; // SO
	else if (side == 1 && rayDirY < 0)
		return 2; // WE
	else
		return 3; // EA
}

/* Convierte componentes r,g,b (0-255) a entero RGB 0xRRGGBB */
static int	rgb_to_int(int r, int g, int b)
{
	return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
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
			if (cell == '1')
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

		// --- TEXTURAS ---
		int texNum = get_wall_texture(side, rayDirX, rayDirY);
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
			int color = cub->wall_textures[texNum][cub->tex_width * texY + texX];
			if (side == 1)
				color = (color >> 1) & 0x7F7F7F; // sombra en laterales
			unsigned int *pixel = (unsigned int *)(img_data + (y * size_line + x * bytes_per_pixel));
			*pixel = (unsigned int)color;
		}
	}
}

// Movimiento y rotación con WASD y flechas, ESC para salir
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
    long current_time;

    current_time = fn_get_time_in_ms();
    cub->time_frame = (current_time - cub->time_prev) / 1000.0; 
    cub->time_prev = current_time;
    perform_movements(cub);
    raycast_render(cub, cub->img_data, cub->size_line, cub->bpp);
    mlx_put_image_to_window(cub->mlx, cub->win, cub->img, 0, 0);
    return (0);
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