#include "../includes/cub3d.h"
#include <mlx.h>
#include <math.h>
#include <string.h>

#define WIDTH 640
#define HEIGHT 480
#define MOVE_SPEED 0.09
#define ROT_SPEED 0.055
#define IS_FLOOR(c) ((c) == '0' || (c) == 'N' || (c) == 'S' || (c) == 'E' || (c) == 'W')

#include "../includes/cub3d.h"
#include <mlx.h>
#include <math.h>
#include <string.h>

#define WIDTH 640
#define HEIGHT 480
#define MOVE_SPEED 0.09
#define ROT_SPEED 0.055
#define IS_FLOOR(c) ((c) == '0' || (c) == 'N' || (c) == 'S' || (c) == 'E' || (c) == 'W')

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

void	raycast_render(t_cub3d *cub, char *img_data, int size_line, int bpp)
{
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
			if (cell && cell != '0' && cell != ' ')
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

		// Fondo (techo y suelo)
		for (int y = 0; y < drawStart; y++)
			*(unsigned int *)(img_data + (y * size_line + x * (bpp/8))) = 0x202020;
		for (int y = drawEnd + 1; y < HEIGHT; y++)
			*(unsigned int *)(img_data + (y * size_line + x * (bpp/8))) = 0x707070;

		// --- Dibuja la pared con textura ---
		for (int y = drawStart; y <= drawEnd; y++)
		{
			int texY = (int)texPos & (cub->tex_height - 1);
			texPos += step;
			int color = cub->wall_textures[texNum][cub->tex_width * texY + texX];
			if (side == 1)
				color = (color >> 1) & 0x7F7F7F; // sombra en laterales
			*(unsigned int *)(img_data + (y * size_line + x * (bpp/8))) = color;
		}
	}
}
// El resto de funciones (movimiento, render_loop, init_player) igual que antes

// Movimiento y rotación con WASD y flechas, ESC para salir
int handle_keys(int key, t_cub3d *cub)
{
	double oldDirX, oldPlaneX;
	// W: adelante
	if (key == 119) {
		if (IS_FLOOR(cub->map[(int)(cub->pos_y)][(int)(cub->pos_x + cub->dir_x * MOVE_SPEED)]))
			cub->pos_x += cub->dir_x * MOVE_SPEED;
		if (IS_FLOOR(cub->map[(int)(cub->pos_y + cub->dir_y * MOVE_SPEED)][(int)(cub->pos_x)]))
			cub->pos_y += cub->dir_y * MOVE_SPEED;
	}
	// S: atrás
	if (key == 115) {
		if (IS_FLOOR(cub->map[(int)(cub->pos_y)][(int)(cub->pos_x - cub->dir_x * MOVE_SPEED)]))
			cub->pos_x -= cub->dir_x * MOVE_SPEED;
		if (IS_FLOOR(cub->map[(int)(cub->pos_y - cub->dir_y * MOVE_SPEED)][(int)(cub->pos_x)]))
			cub->pos_y -= cub->dir_y * MOVE_SPEED;
	}
	// D o flecha derecha: rotar DERECHA (+ROT_SPEED)
	if (key == 100 || key == 65363) {
		oldDirX = cub->dir_x;
		cub->dir_x = cub->dir_x * cos(ROT_SPEED) - cub->dir_y * sin(ROT_SPEED);
		cub->dir_y = oldDirX * sin(ROT_SPEED) + cub->dir_y * cos(ROT_SPEED);
		oldPlaneX = cub->plane_x;
		cub->plane_x = cub->plane_x * cos(ROT_SPEED) - cub->plane_y * sin(ROT_SPEED);
		cub->plane_y = oldPlaneX * sin(ROT_SPEED) + cub->plane_y * cos(ROT_SPEED);
	}
	// A o flecha izquierda: rotar IZQUIERDA (-ROT_SPEED)
	if (key == 97 || key == 65361) {
		oldDirX = cub->dir_x;
		cub->dir_x = cub->dir_x * cos(-ROT_SPEED) - cub->dir_y * sin(-ROT_SPEED);
		cub->dir_y = oldDirX * sin(-ROT_SPEED) + cub->dir_y * cos(-ROT_SPEED);
		oldPlaneX = cub->plane_x;
		cub->plane_x = cub->plane_x * cos(-ROT_SPEED) - cub->plane_y * sin(-ROT_SPEED);
		cub->plane_y = oldPlaneX * sin(-ROT_SPEED) + cub->plane_y * cos(-ROT_SPEED);
	}
	// ESC: salir
	if (key == 65307)
		exit(0);
	return (0);
}

// Bucle de renderizado
int render_loop(t_cub3d *cub)
{
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