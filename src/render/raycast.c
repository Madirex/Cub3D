#include "../includes/cub3d.h"
#include <mlx.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#define WIDTH 1248
#define HEIGHT 960
#define MOVE_SPEED 0.09
#define ROT_SPEED 0.055
#define MOUSE_SENSITIVITY 0.003
#define IS_FLOOR(c) ((c) == '0' || (c) == 'N' || (c) == 'S' || \
	(c) == 'E' || (c) == 'W' || (c) == 'O')

#define MINIMAP_SCALE 12
#define MINIMAP_MARGIN 10
#define MINIMAP_VIEW_RADIUS 5

#define COLOR_WALL 0x404040
#define COLOR_FLOOR 0x808080
#define COLOR_PLAYER 0xFF0000
#define COLOR_EMPTY 0x000000

int		exit_program(t_cub3d *cub);
void	ft_error(char *message, t_cub3d *cub, char *line);
void	draw_door_prompt(t_cub3d *cub);

/* Small helpers for minimap and pixels */
static int	get_wall_texture(int side, double rayDirX, double rayDirY)
{
	if (side == 0)
	{
		if (rayDirX > 0)
			return (2);
		return (3);
	}
	if (rayDirY > 0)
		return (1);
	return (0);
}

static int	rgb_to_int(int r, int g, int b)
{
	return (((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF));
}

static void	draw_pixel(char *img_data, int size_line, int bpp,
	int x, int y, int color)
{
	int	bytes_per_pixel;

	bytes_per_pixel = bpp / 8;
	if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
	{
		unsigned int *pixel;

		pixel = (unsigned int *)(img_data + (y * size_line
					+ x * bytes_per_pixel));
		*pixel = (unsigned int)color;
	}
}

/* Minimap: color / cell drawing helpers (keeps functions short) */
static int	minimap_cell_color(t_cub3d *cub, int map_y, int map_x)
{
	char	cell;

	if (map_y < 0 || map_y >= cub->map_height)
		return (COLOR_EMPTY);
	if (map_x < 0 || cub->map[map_y] == NULL)
		return (COLOR_EMPTY);
	if (map_x >= (int)strlen(cub->map[map_y]))
		return (COLOR_EMPTY);
	cell = cub->map[map_y][map_x];
	if (cell == '1' || cell == 'D')
		return (COLOR_WALL);
	if (IS_FLOOR(cell))
		return (COLOR_FLOOR);
	return (COLOR_EMPTY);
}

static void	minimap_draw_cell(char *img_data, int size_line, int bpp,
	int draw_x, int draw_y, int color)
{
	int	px;
	int	py;
	int	border;

	border = 0x000000;
	py = 0;
	while (py < MINIMAP_SCALE)
	{
		px = 0;
		while (px < MINIMAP_SCALE)
		{
			if (px == 0 || py == 0)
				draw_pixel(img_data, size_line, bpp, draw_x + px,
					draw_y + py, border);
			else
				draw_pixel(img_data, size_line, bpp, draw_x + px,
					draw_y + py, color);
			px++;
		}
		py++;
	}
}

static void	minimap_draw_player_square(char *img_data, int size_line,
	int bpp, int player_pixel_x, int player_pixel_y)
{
	int player_size;
	int y;
	int x;

	player_size = MINIMAP_SCALE / 4;
	y = player_pixel_y - player_size / 2;
	while (y < player_pixel_y + player_size / 2)
	{
		x = player_pixel_x - player_size / 2;
		while (x < player_pixel_x + player_size / 2)
		{
			draw_pixel(img_data, size_line, bpp, x, y, COLOR_PLAYER);
			x++;
		}
		y++;
	}
}

static void	minimap_draw_player_dir(char *img_data, int size_line,
	int bpp, int player_pixel_x, int player_pixel_y, double dir_x,
	double dir_y)
{
	int	dir_len;
	int	dir_end_x;
	int	dir_end_y;

	dir_len = MINIMAP_SCALE / 2;
	dir_end_x = player_pixel_x + (int)(dir_x * dir_len);
	dir_end_y = player_pixel_y + (int)(dir_y * dir_len);
	draw_pixel(img_data, size_line, bpp, dir_end_x, dir_end_y, COLOR_PLAYER);
	draw_pixel(img_data, size_line, bpp, dir_end_x + 1, dir_end_y, COLOR_PLAYER);
	draw_pixel(img_data, size_line, bpp, dir_end_x, dir_end_y + 1, COLOR_PLAYER);
	draw_pixel(img_data, size_line, bpp, dir_end_x - 1, dir_end_y, COLOR_PLAYER);
	draw_pixel(img_data, size_line, bpp, dir_end_x, dir_end_y - 1, COLOR_PLAYER);
}

static void	minimap_draw_cells(t_cub3d *cub, char *img_data,
	int size_line, int bpp, int minimap_start_x, int minimap_start_y)
{
	int	i;
	int	j;
	int	map_cell_x;
	int	map_cell_y;
	int	cell_draw_x;
	int	cell_draw_y;
	int	color;

	i = -MINIMAP_VIEW_RADIUS;
	while (i <= MINIMAP_VIEW_RADIUS)
	{
		j = -MINIMAP_VIEW_RADIUS;
		while (j <= MINIMAP_VIEW_RADIUS)
		{
			map_cell_x = (int)cub->pos_x + j;
			map_cell_y = (int)cub->pos_y + i;
			cell_draw_x = minimap_start_x + (j + MINIMAP_VIEW_RADIUS)
				* MINIMAP_SCALE;
			cell_draw_y = minimap_start_y + (i + MINIMAP_VIEW_RADIUS)
				* MINIMAP_SCALE;
			color = minimap_cell_color(cub, map_cell_y, map_cell_x);
			minimap_draw_cell(img_data, size_line, bpp,
				cell_draw_x, cell_draw_y, color);
			j++;
		}
		i++;
	}
}

void	draw_minimap(t_cub3d *cub, char *img_data, int size_line, int bpp)
{
	int	minimap_start_x;
	int	minimap_start_y;
	int	player_center_x;
	int	player_center_y;
	double	relative_x;
	double	relative_y;
	int	player_pixel_x;
	int	player_pixel_y;

	minimap_start_x = MINIMAP_MARGIN;
	minimap_start_y = MINIMAP_MARGIN;
	minimap_draw_cells(cub, img_data, size_line, bpp,
		minimap_start_x, minimap_start_y);
	player_center_x = minimap_start_x + MINIMAP_VIEW_RADIUS * MINIMAP_SCALE
		+ MINIMAP_SCALE / 2;
	player_center_y = minimap_start_y + MINIMAP_VIEW_RADIUS * MINIMAP_SCALE
		+ MINIMAP_SCALE / 2;
	relative_x = cub->pos_x - (int)cub->pos_x;
	relative_y = cub->pos_y - (int)cub->pos_y;
	player_pixel_x = player_center_x + (int)((relative_x - 0.5)
			* MINIMAP_SCALE);
	player_pixel_y = player_center_y + (int)((relative_y - 0.5)
			* MINIMAP_SCALE);
	minimap_draw_player_square(img_data, size_line, bpp,
		player_pixel_x, player_pixel_y);
	minimap_draw_player_dir(img_data, size_line, bpp,
		player_pixel_x, player_pixel_y, cub->dir_x, cub->dir_y);
}

/* Raycast helpers split to keep functions short */
static int	*select_texture_buffer(t_cub3d *cub, int side, int mapX,
	int mapY, double rayDirX, double rayDirY)
{
	int	neighborX;
	int	neighborY;
	int	cell_hit;

	cell_hit = cub->map[mapY][mapX];
	if (cub->is_bonus && cell_hit == 'D' && cub->door_textures
		&& cub->door_textures[0] != NULL)
		return (cub->door_textures[0]);
	neighborX = mapX;
	neighborY = mapY;
	if (side == 0)
		neighborX = mapX - (rayDirX < 0 ? -1 : 1);
	else
		neighborY = mapY - (rayDirY < 0 ? -1 : 1);
	if (cub->is_bonus && neighborY >= 0 && neighborY < cub->map_height
		&& neighborX >= 0 && neighborX < (int)strlen(cub->map[neighborY])
		&& cub->map[neighborY][neighborX] == 'O' && cub->door_textures
		&& cub->door_textures[1] != NULL)
		return (cub->door_textures[1]);
	return (cub->wall_textures[get_wall_texture(side, rayDirX, rayDirY)]);
}

static void	draw_column_floor_ceiling(char *img_data, int size_line,
	int bpp, int x, int drawStart, int drawEnd, int ceiling_color,
	int floor_color)
{
	int	y;
	int	bytes_per_pixel;
	unsigned int *pixel;

	bytes_per_pixel = bpp / 8;
	y = 0;
	while (y < drawStart)
	{
		pixel = (unsigned int *)(img_data + (y * size_line
					+ x * bytes_per_pixel));
		*pixel = (unsigned int)ceiling_color;
		y++;
	}
	y = drawEnd + 1;
	while (y < HEIGHT)
	{
		pixel = (unsigned int *)(img_data + (y * size_line
					+ x * bytes_per_pixel));
		*pixel = (unsigned int)floor_color;
		y++;
	}
}

static void	draw_column_textured(t_cub3d *cub, char *img_data, int size_line,
	int bpp, int x, int drawStart, int drawEnd, int lineHeight,
	int texX, int *tex_buf, int side)
{
	int	bytes_per_pixel;
	double	step;
	double	texPos;
	int	y;
	int	texY;
	unsigned int *pixel;
	int	color;

	if (lineHeight <= 0 || tex_buf == NULL)
		return ;
	bytes_per_pixel = bpp / 8;
	step = 1.0 * cub->tex_height / lineHeight;
	texPos = (drawStart - HEIGHT / 2 + lineHeight / 2) * step;
	y = drawStart;
	while (y <= drawEnd)
	{
		texY = (int)texPos & (cub->tex_height - 1);
		texPos += step;
		color = tex_buf[cub->tex_width * texY + texX];
		if (side == 1)
			color = (color >> 1) & 0x7F7F7F;
		pixel = (unsigned int *)(img_data + (y * size_line
					+ x * bytes_per_pixel));
		*pixel = (unsigned int)color;
		y++;
	}
}

/* DDA loop: returns 1 if hit and fills side & perpWallDist */
static int	perform_dda(t_cub3d *cub, int *mapX, int *mapY, double rayDirX,
	double rayDirY, int *side, double *perpWallDist)
{
	double	sideDistX;
	double	sideDistY;
	double	deltaDistX;
	double	deltaDistY;
	int		stepX;
	int		stepY;

	deltaDistX = rayDirX == 0 ? 1e30 : fabs(1 / rayDirX);
	deltaDistY = rayDirY == 0 ? 1e30 : fabs(1 / rayDirY);
	if (rayDirX < 0)
	{
		stepX = -1;
		sideDistX = (cub->pos_x - *mapX) * deltaDistX;
	}
	else
	{
		stepX = 1;
		sideDistX = (*mapX + 1.0 - cub->pos_x) * deltaDistX;
	}
	if (rayDirY < 0)
	{
		stepY = -1;
		sideDistY = (cub->pos_y - *mapY) * deltaDistY;
	}
	else
	{
		stepY = 1;
		sideDistY = (*mapY + 1.0 - cub->pos_y) * deltaDistY;
	}
	while (1)
	{
		if (sideDistX < sideDistY)
		{
			sideDistX += deltaDistX;
			*mapX += stepX;
			*side = 0;
		}
		else
		{
			sideDistY += deltaDistY;
			*mapY += stepY;
			*side = 1;
		}
		if (*mapY < 0 || *mapY >= cub->map_height
			|| *mapX < 0 || *mapX >= (int)strlen(cub->map[*mapY]))
			return (0);
		if (cub->map[*mapY][*mapX] == '1'
			|| (cub->is_bonus && cub->map[*mapY][*mapX] == 'D'))
		{
			if (*side == 0)
				*perpWallDist = (sideDistX - deltaDistX);
			else
				*perpWallDist = (sideDistY - deltaDistY);
			return (1);
		}
	}
}

static void	compute_column_params(t_cub3d *cub, double perpWallDist,
	int *lineHeight, int *drawStart, int *drawEnd, double *wallX,
	int side, double rayDirX, double rayDirY)
{
	*lineHeight = (int)(HEIGHT / perpWallDist);
	*drawStart = - *lineHeight / 2 + HEIGHT / 2;
	if (*drawStart < 0)
		*drawStart = 0;
	*drawEnd = *lineHeight / 2 + HEIGHT / 2;
	if (*drawEnd >= HEIGHT)
		*drawEnd = HEIGHT - 1;
	if (side == 0)
		*wallX = cub->pos_y + perpWallDist * rayDirY;
	else
		*wallX = cub->pos_x + perpWallDist * rayDirX;
	*wallX -= floor(*wallX);
}

static void	render_column(int x, t_cub3d *cub, char *img_data,
	int size_line, int bpp, int ceiling_color, int floor_color)
{
	double	cameraX;
	double	rayDirX;
	double	rayDirY;
	int		mapX;
	int		mapY;
	int		side;
	double	perpWallDist;
	int		lineHeight;
	int		drawStart;
	int		drawEnd;
	double	wallX;
	int		texX;
	int		*tex_buf;

	cameraX = 2 * x / (double)WIDTH - 1;
	rayDirX = cub->dir_x + cub->plane_x * cameraX;
	rayDirY = cub->dir_y + cub->plane_y * cameraX;
	mapX = (int)cub->pos_x;
	mapY = (int)cub->pos_y;
	if (!perform_dda(cub, &mapX, &mapY, rayDirX, rayDirY, &side, &perpWallDist))
		return ;
	compute_column_params(cub, perpWallDist, &lineHeight, &drawStart,
		&drawEnd, &wallX, side, rayDirX, rayDirY);
	texX = (int)(wallX * (double)cub->tex_width);
	if (side == 0 && rayDirX > 0)
		texX = cub->tex_width - texX - 1;
	if (side == 1 && rayDirY < 0)
		texX = cub->tex_width - texX - 1;
	tex_buf = select_texture_buffer(cub, side, mapX, mapY, rayDirX, rayDirY);
	draw_column_floor_ceiling(img_data, size_line, bpp, x, drawStart,
		drawEnd, ceiling_color, floor_color);
	draw_column_textured(cub, img_data, size_line, bpp, x, drawStart,
		drawEnd, lineHeight, texX, tex_buf, side);
}

void	raycast_render(t_cub3d *cub, char *img_data, int size_line, int bpp)
{
	int		x;
	int		bytes_per_pixel;
	int		ceiling_color;
	int		floor_color;

	bytes_per_pixel = bpp / 8;
	if (cub->textures.ceiling.r != -1)
		ceiling_color = rgb_to_int(cub->textures.ceiling.r,
				cub->textures.ceiling.g, cub->textures.ceiling.b);
	else
		ceiling_color = 0x202020;
	if (cub->textures.floor.r != -1)
		floor_color = rgb_to_int(cub->textures.floor.r,
				cub->textures.floor.g, cub->textures.floor.b);
	else
		floor_color = 0x707070;
	x = 0;
	while (x < WIDTH)
	{
		render_column(x, cub, img_data, size_line, bpp, ceiling_color,
			floor_color);
		x++;
		(void)bytes_per_pixel;
	}
}

/* Door helpers */
static int	get_target_cell_coords(t_cub3d *cub, double dist,
	int *tx, int *ty)
{
	*tx = (int)(cub->pos_x + cub->dir_x * dist);
	*ty = (int)(cub->pos_y + cub->dir_y * dist);
	if (*ty < 0 || *ty >= cub->map_height)
		return (0);
	if (*tx < 0 || *tx >= (int)strlen(cub->map[*ty]))
		return (0);
	return (1);
}

void	handle_door_action(t_cub3d *cub)
{
	int	target_mapX;
	int	target_mapY;
	int	player_mapX;
	int	player_mapY;
	char	*target_cell;

	if (!cub->is_bonus)
		return ;
	if (!get_target_cell_coords(cub, 1.0, &target_mapX, &target_mapY))
		return ;
	target_cell = &cub->map[target_mapY][target_mapX];
	if (*target_cell == 'D')
	{
		*target_cell = 'O';
		return ;
	}
	if (*target_cell == 'O')
	{
		player_mapX = (int)cub->pos_x;
		player_mapY = (int)cub->pos_y;
		if (target_mapX == player_mapX && target_mapY == player_mapY)
			return ;
		*target_cell = 'D';
	}
}

int	handle_key_press(int key, t_cub3d *cub)
{
	if (key == 119)
		cub->is_moving_forward = 1;
	else if (key == 115)
		cub->is_moving_backward = 1;
	else if (key == 97 || key == 65361)
		cub->is_rotating_left = 1;
	else if (key == 100 || key == 65363)
		cub->is_rotating_right = 1;
	else if (key == 65307)
		return (exit_program(cub));
	else if (key == 49 || key == 32 || key == 65349)
		handle_door_action(cub);
	return (0);
}

int	handle_key_release(int key, t_cub3d *cub)
{
	if (key == 119)
		cub->is_moving_forward = 0;
	else if (key == 115)
		cub->is_moving_backward = 0;
	else if (key == 97 || key == 65361)
		cub->is_rotating_left = 0;
	else if (key == 100 || key == 65363)
		cub->is_rotating_right = 0;
	return (0);
}

/* Movement helpers */
static void	move_forward(t_cub3d *cub, double move_speed_dt)
{
	if (IS_FLOOR(cub->map[(int)(cub->pos_y)]
			[(int)(cub->pos_x + cub->dir_x * move_speed_dt)]))
		cub->pos_x += cub->dir_x * move_speed_dt;
	if (IS_FLOOR(cub->map[(int)(cub->pos_y + cub->dir_y * move_speed_dt)]
			[(int)(cub->pos_x)]))
		cub->pos_y += cub->dir_y * move_speed_dt;
}

static void	move_backward(t_cub3d *cub, double move_speed_dt)
{
	if (IS_FLOOR(cub->map[(int)(cub->pos_y)]
			[(int)(cub->pos_x - cub->dir_x * move_speed_dt)]))
		cub->pos_x -= cub->dir_x * move_speed_dt;
	if (IS_FLOOR(cub->map[(int)(cub->pos_y - cub->dir_y * move_speed_dt)]
			[(int)(cub->pos_x)]))
		cub->pos_y -= cub->dir_y * move_speed_dt;
}

static void	rotate_right(t_cub3d *cub, double rot_speed_dt)
{
	double	oldDirX;
	double	oldPlaneX;

	oldDirX = cub->dir_x;
	cub->dir_x = cub->dir_x * cos(rot_speed_dt) - cub->dir_y
		* sin(rot_speed_dt);
	cub->dir_y = oldDirX * sin(rot_speed_dt) + cub->dir_y
		* cos(rot_speed_dt);
	oldPlaneX = cub->plane_x;
	cub->plane_x = cub->plane_x * cos(rot_speed_dt) - cub->plane_y
		* sin(rot_speed_dt);
	cub->plane_y = oldPlaneX * sin(rot_speed_dt) + cub->plane_y
		* cos(rot_speed_dt);
}

static void	rotate_left(t_cub3d *cub, double rot_speed_dt)
{
	double	oldDirX;
	double	oldPlaneX;

	oldDirX = cub->dir_x;
	cub->dir_x = cub->dir_x * cos(-rot_speed_dt) - cub->dir_y
		* sin(-rot_speed_dt);
	cub->dir_y = oldDirX * sin(-rot_speed_dt) + cub->dir_y
		* cos(-rot_speed_dt);
	oldPlaneX = cub->plane_x;
	cub->plane_x = cub->plane_x * cos(-rot_speed_dt) - cub->plane_y
		* sin(-rot_speed_dt);
	cub->plane_y = oldPlaneX * sin(-rot_speed_dt) + cub->plane_y
		* cos(-rot_speed_dt);
}

void	perform_movements(t_cub3d *cub)
{
	double	move_speed_dt;
	double	rot_speed_dt;

	move_speed_dt = MOVE_SPEED * cub->time_frame * 60.0;
	rot_speed_dt = ROT_SPEED * cub->time_frame * 60.0;
	if (cub->is_moving_forward)
		move_forward(cub, move_speed_dt);
	if (cub->is_moving_backward)
		move_backward(cub, move_speed_dt);
	if (cub->is_rotating_right)
		rotate_right(cub, rot_speed_dt);
	if (cub->is_rotating_left)
		rotate_left(cub, rot_speed_dt);
}

long	fn_get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (0);
	return ((long)(tv.tv_sec * 1000 + tv.tv_usec / 1000));
}

int	render_loop(t_cub3d *cub)
{
	long	current_time;

	current_time = fn_get_time_in_ms();
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

/* Door prompt: small and simple */
void	draw_door_prompt(t_cub3d *cub)
{
	int	target_mapX;
	int	target_mapY;
	const char	*message;
	char	target_cell;

	const double	check_dist_near = 0.75;
	const double	check_dist_far = 1.5;

	if (!get_target_cell_coords(cub, check_dist_near, &target_mapX, &target_mapY))
		return ;
	if ((int)cub->pos_x == target_mapX && (int)cub->pos_y == target_mapY)
	{
		if (!get_target_cell_coords(cub, check_dist_far, &target_mapX, &target_mapY))
			return ;
	}
	target_cell = cub->map[target_mapY][target_mapX];
	if (cub->is_bonus && (target_cell == 'D' || target_cell == 'O'))
	{
		if (target_cell == 'D')
			message = "Press SPACE to open";
		else
			message = "Press SPACE to close";
		mlx_string_put(cub->mlx, cub->win, (WIDTH / 2) - 100,
			HEIGHT - 50, 0xFFFFFF, (char *)message);
	}
}

/* Player init refactor: keep find/set small */
static void	set_player_dir(t_cub3d *cub, char dir)
{
	if (dir == 'N')
	{
		cub->dir_x = 0; cub->dir_y = -1;
		cub->plane_x = 0.66; cub->plane_y = 0;
	}
	else if (dir == 'S')
	{
		cub->dir_x = 0; cub->dir_y = 1;
		cub->plane_x = -0.66; cub->plane_y = 0;
	}
	else if (dir == 'E')
	{
		cub->dir_x = 1; cub->dir_y = 0;
		cub->plane_x = 0; cub->plane_y = 0.66;
	}
	else if (dir == 'W')
	{
		cub->dir_x = -1; cub->dir_y = 0;
		cub->plane_x = 0; cub->plane_y = -0.66;
	}
}

static int	find_player_position(t_cub3d *cub)
{
	int	y;
	int	x;

	y = 0;
	while (y < cub->map_height)
	{
		x = 0;
		while (x < (int)strlen(cub->map[y]))
		{
			if (cub->map[y][x] == 'N' || cub->map[y][x] == 'S'
				|| cub->map[y][x] == 'E' || cub->map[y][x] == 'W')
			{
				cub->pos_x = x + 0.5;
				cub->pos_y = y + 0.5;
				set_player_dir(cub, cub->map[y][x]);
				return (1);
			}
			x++;
		}
		y++;
	}
	return (0);
}

void	init_player(t_cub3d *cub)
{
	(void)find_player_position;
	(void)set_player_dir;
	find_player_position(cub);
}

void	rotate_player(t_cub3d *cub, double angle)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = cub->dir_x;
	cub->dir_x = cub->dir_x * cos(angle) - cub->dir_y * sin(angle);
	cub->dir_y = old_dir_x * sin(angle) + cub->dir_y * cos(angle);
	old_plane_x = cub->plane_x;
	cub->plane_x = cub->plane_x * cos(angle) - cub->plane_y * sin(angle);
	cub->plane_y = old_plane_x * sin(angle) + cub->plane_y * cos(angle);
}

int	handle_mouse_move(int x, int y, t_cub3d *cub)
{
	int		delta_x;
	double	rotation_angle;

	(void)y;
	delta_x = x - cub->last_mouse_x;
	if (delta_x == 0)
		return (0);
	rotation_angle = delta_x * MOUSE_SENSITIVITY;
	rotate_player(cub, rotation_angle);
	cub->last_mouse_x = x;
	return (0);
}