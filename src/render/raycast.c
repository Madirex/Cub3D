#include "../../includes/cub3d.h"
#include <mlx.h>
#include <math.h>

#define WIDTH 640
#define HEIGHT 480
#define MOVE_SPEED 0.09
#define ROT_SPEED 0.055

static int	get_color(char cell, int side)
{
	int color;

	if (cell == '1') color = 0xFF0000;
	else if (cell == '2') color = 0x00FF00;
	else if (cell == '3') color = 0x0000FF;
	else if (cell == '4') color = 0xFFFFFF;
	else color = 0xFFFF00;
	if (side == 1) color = (color >> 1) & 0x7F7F7F;
	return color;
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
			if (mapY < 0 || mapY >= cub->map_height || mapX < 0 || mapX >= cub->map_width)
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

		int color = get_color(cub->map[mapY][mapX], side);

		// Fondo (negro)
		for (int y = 0; y < drawStart; y++)
			*(unsigned int *)(img_data + (y * size_line + x * (bpp/8))) = 0x202020;
		for (int y = drawEnd+1; y < HEIGHT; y++)
			*(unsigned int *)(img_data + (y * size_line + x * (bpp/8))) = 0x707070;
		// Pared
		for (int y = drawStart; y <= drawEnd; y++)
			*(unsigned int *)(img_data + (y * size_line + x * (bpp/8))) = color;
	}
}

// Para controles de movimiento (WASD y flechas, ESC para salir)
int	handle_keys(int key, t_cub3d *cub)
{
	if (key == 65307) // ESC
		exit(0);
	double oldDirX, oldPlaneX;
	// W: adelante
	if (key == 119) {
		if (cub->map[(int)(cub->pos_y)][(int)(cub->pos_x + cub->dir_x * MOVE_SPEED)] == '0')
			cub->pos_x += cub->dir_x * MOVE_SPEED;
		if (cub->map[(int)(cub->pos_y + cub->dir_y * MOVE_SPEED)][(int)(cub->pos_x)] == '0')
			cub->pos_y += cub->dir_y * MOVE_SPEED;
	}
	// S: atrás
	if (key == 115) {
		if (cub->map[(int)(cub->pos_y)][(int)(cub->pos_x - cub->dir_x * MOVE_SPEED)] == '0')
			cub->pos_x -= cub->dir_x * MOVE_SPEED;
		if (cub->map[(int)(cub->pos_y - cub->dir_y * MOVE_SPEED)][(int)(cub->pos_x)] == '0')
			cub->pos_y -= cub->dir_y * MOVE_SPEED;
	}
	// D: rotar derecha
	if (key == 100 || key == 65363) {
		oldDirX = cub->dir_x;
		cub->dir_x = cub->dir_x * cos(-ROT_SPEED) - cub->dir_y * sin(-ROT_SPEED);
		cub->dir_y = oldDirX * sin(-ROT_SPEED) + cub->dir_y * cos(-ROT_SPEED);
		oldPlaneX = cub->plane_x;
		cub->plane_x = cub->plane_x * cos(-ROT_SPEED) - cub->plane_y * sin(-ROT_SPEED);
		cub->plane_y = oldPlaneX * sin(-ROT_SPEED) + cub->plane_y * cos(-ROT_SPEED);
	}
	// A: rotar izquierda
	if (key == 97 || key == 65361) {
		oldDirX = cub->dir_x;
		cub->dir_x = cub->dir_x * cos(ROT_SPEED) - cub->dir_y * sin(ROT_SPEED);
		cub->dir_y = oldDirX * sin(ROT_SPEED) + cub->dir_y * cos(ROT_SPEED);
		oldPlaneX = cub->plane_x;
		cub->plane_x = cub->plane_x * cos(ROT_SPEED) - cub->plane_y * sin(ROT_SPEED);
		cub->plane_y = oldPlaneX * sin(ROT_SPEED) + cub->plane_y * cos(ROT_SPEED);
	}
	return (0);
}

// Bucle de renderizado
int	render_loop(t_cub3d *cub)
{
	raycast_render(cub, cub->img_data, cub->size_line, cub->bpp);
	mlx_put_image_to_window(cub->mlx, cub->win, cub->img, 0, 0);
	return (0);
}

// Inicializa la posición/dirección del jugador según el mapa
void	init_player(t_cub3d *cub)
{
	for (int y = 0; y < cub->map_height; y++)
	for (int x = 0; x < (int)ft_strlen(cub->map[y]); x++)
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