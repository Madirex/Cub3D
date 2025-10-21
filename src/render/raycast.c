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
#define IS_FLOOR(c) ((c) == '0' || (c) == 'N' || (c) == 'S' \
	|| (c) == 'E' || (c) == 'W' || (c) == 'O')

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

/*
** Grouped parameter structs to respect argument limit
*/
typedef struct s_col_geom
{
	int		x;
	int		lineHeight;
	int		drawStart;
	int		drawEnd;
	int		texX;
	int		side;
	double	wallX;
}				t_col_geom;

typedef struct s_dda_in
{
	int		mapX;
	int		mapY;
	double	rayDirX;
	double	rayDirY;
}				t_dda_in;

typedef struct s_dda_out
{
	int		side;
	double	perpWallDist;
	int		stepX;
	int		stepY;
}				t_dda_out;

typedef struct s_col_calc_in
{
	double	perpWallDist;
	int		side;
	double	rayDirX;
	double	rayDirY;
}				t_col_calc_in;

typedef struct s_render_colors
{
	int	ceiling;
	int	floor;
}				t_render_colors;

typedef struct s_vec2
{
	double	x;
	double	y;
}				t_vec2;

typedef struct s_tex_query
{
	int	side;
	int	mapX;
	int	mapY;
	int	stepX;
	int	stepY;
}				t_tex_query;

/* moved minimap row context struct up with the other typedefs */
typedef struct s_minimap_row_ctx
{
	int	j;
	int	map_cell_x;
	int	map_cell_y;
	int	cell_draw_x;
	int	cell_draw_y;
	int	color;
}				t_minimap_row_ctx;

/* Context struct to reduce number of local declarations in render_column */
typedef struct s_render_ctx
{
	t_dda_in		dda_in;
	t_dda_out		dda_out;
	t_col_calc_in	calc_in;
	t_col_geom		g;
	t_tex_query		q;
	int				*tex_buf;
}				t_render_ctx;

/*
** Small helpers
*/
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
	return (((r & 0xFF) << 16)
		| ((g & 0xFF) << 8)
		| (b & 0xFF));
}

/*
** Pixel helper: usa t_img para evitar pasar 3 argumentos
*/
static void	draw_pixel(t_img *img, int x, int y, int color)
{
	unsigned int	*pixel;
	unsigned char	*addr;

	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
		return ;
	addr = (unsigned char *)img->data;
	addr += y * img->size_line;
	addr += x * img->bytes_per_pixel;
	pixel = (unsigned int *)addr;
	*pixel = (unsigned int)color;
}

/*
** Minimap helpers: kept short
*/
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

/* fill entire cell area with color (no border handling) */
static void	minimap_fill_cell(t_img *img, int base_x, int base_y, int color)
{
	int	px;
	int	py;

	py = 0;
	while (py < MINIMAP_SCALE)
	{
		px = 0;
		while (px < MINIMAP_SCALE)
		{
			draw_pixel(img, base_x + px, base_y + py, color);
			px++;
		}
		py++;
	}
}

/* draw top + left border for a cell */
static void	minimap_draw_border(t_img *img, int draw_x, int draw_y)
{
	int	i;

	i = 0;
	while (i < MINIMAP_SCALE)
	{
		draw_pixel(img, draw_x + i, draw_y, 0x000000);
		draw_pixel(img, draw_x, draw_y + i, 0x000000);
		i++;
	}
}

static void	minimap_draw_cell(t_img *img, int draw_x, int draw_y, int color)
{
	minimap_fill_cell(img, draw_x, draw_y, color);
	minimap_draw_border(img, draw_x, draw_y);
}

/* iterate one row of minimap cells (keeps per-row logic short) */
static void	minimap_draw_row(t_cub3d *cub, t_img *img, int i, int start_x,
	int start_y)
{
	t_minimap_row_ctx ctx;

	ctx.j = -MINIMAP_VIEW_RADIUS;
	while (ctx.j <= MINIMAP_VIEW_RADIUS)
	{
		ctx.map_cell_x = (int)cub->pos_x + ctx.j;
		ctx.map_cell_y = (int)cub->pos_y + i;
		ctx.cell_draw_x = start_x + (ctx.j + MINIMAP_VIEW_RADIUS) * MINIMAP_SCALE;
		ctx.cell_draw_y = start_y + (i + MINIMAP_VIEW_RADIUS) * MINIMAP_SCALE;
		ctx.color = minimap_cell_color(cub, ctx.map_cell_y, ctx.map_cell_x);
		minimap_draw_cell(img, ctx.cell_draw_x, ctx.cell_draw_y, ctx.color);
		ctx.j++;
	}
}

static void	minimap_draw_cells(t_cub3d *cub, t_img *img,
	int minimap_start_x, int minimap_start_y)
{
	int	i;

	i = -MINIMAP_VIEW_RADIUS;
	while (i <= MINIMAP_VIEW_RADIUS)
	{
		minimap_draw_row(cub, img, i, minimap_start_x, minimap_start_y);
		i++;
	}
}

static void	minimap_draw_player_square(t_img *img, int player_pixel_x,
	int player_pixel_y)
{
	int	player_size;
	int	y;
	int	x;

	player_size = MINIMAP_SCALE / 4;
	y = player_pixel_y - player_size / 2;
	while (y < player_pixel_y + player_size / 2)
	{
		x = player_pixel_x - player_size / 2;
		while (x < player_pixel_x + player_size / 2)
		{
			draw_pixel(img, x, y, COLOR_PLAYER);
			x++;
		}
		y++;
	}
}

static void	minimap_draw_player_dir(t_img *img, int player_pixel_x,
	int player_pixel_y, t_vec2 *dir)
{
	int	dir_len;
	int	dir_end_x;
	int	dir_end_y;

	dir_len = MINIMAP_SCALE / 2;
	dir_end_x = player_pixel_x + (int)(dir->x * dir_len);
	dir_end_y = player_pixel_y + (int)(dir->y * dir_len);
	draw_pixel(img, dir_end_x, dir_end_y, COLOR_PLAYER);
	draw_pixel(img, dir_end_x + 1, dir_end_y, COLOR_PLAYER);
	draw_pixel(img, dir_end_x, dir_end_y + 1, COLOR_PLAYER);
	draw_pixel(img, dir_end_x - 1, dir_end_y, COLOR_PLAYER);
	draw_pixel(img, dir_end_x, dir_end_y - 1, COLOR_PLAYER);
}

/* compute player pixel coords (short helper) */
static void	minimap_compute_player_coords(t_cub3d *cub, int start_x,
	int start_y, int *out_x, int *out_y)
{
	int	player_center_x;
	int	player_center_y;
	double	relative_x;
	double	relative_y;

	player_center_x = start_x + MINIMAP_VIEW_RADIUS * MINIMAP_SCALE
		+ MINIMAP_SCALE / 2;
	player_center_y = start_y + MINIMAP_VIEW_RADIUS * MINIMAP_SCALE
		+ MINIMAP_SCALE / 2;
	relative_x = cub->pos_x - (int)cub->pos_x;
	relative_y = cub->pos_y - (int)cub->pos_y;
	*out_x = player_center_x + (int)((relative_x - 0.5) * MINIMAP_SCALE);
	*out_y = player_center_y + (int)((relative_y - 0.5) * MINIMAP_SCALE);
}

void	draw_minimap(t_cub3d *cub, t_img *img)
{
	int			minimap_start_x;
	int			minimap_start_y;
	int			player_pixel_x;
	int			player_pixel_y;
	t_vec2		dir;

	minimap_start_x = MINIMAP_MARGIN;
	minimap_start_y = MINIMAP_MARGIN;
	minimap_draw_cells(cub, img, minimap_start_x, minimap_start_y);
	minimap_compute_player_coords(cub, minimap_start_x, minimap_start_y,
		&player_pixel_x, &player_pixel_y);
	minimap_draw_player_square(img, player_pixel_x, player_pixel_y);
	dir.x = cub->dir_x;
	dir.y = cub->dir_y;
	minimap_draw_player_dir(img, player_pixel_x, player_pixel_y, &dir);
}

/*
** Raycast helpers using grouped args
*/
static int	*select_texture_buffer(t_cub3d *cub, t_tex_query *q)
{
	int		cell_hit;
	int		neighbor_x;
	int		neighbor_y;
	int		is_door_adjacent;

	cell_hit = cub->map[q->mapY][q->mapX];
	if (cub->is_bonus && cell_hit == 'D' && cub->door_textures
		&& cub->door_textures[cub->door_anim_frame] != NULL)
		return (cub->door_textures[cub->door_anim_frame]);
	neighbor_x = q->mapX;
	neighbor_y = q->mapY;
	if (q->side == 0)
		neighbor_x = q->mapX - q->stepX;
	else
		neighbor_y = q->mapY - q->stepY;
	is_door_adjacent = 0;
	if (cub->is_bonus && neighbor_y >= 0 && neighbor_y < cub->map_height
		&& neighbor_x >= 0 && neighbor_x < (int)strlen(cub->map[neighbor_y])
		&& cub->map[neighbor_y][neighbor_x] == 'O'
		&& cub->door_textures && cub->door_textures[1] != NULL)
		is_door_adjacent = 1;
	if (is_door_adjacent)
		return (cub->door_textures[3]);
	return (cub->wall_textures[get_wall_texture(q->side, 0, 0)]);
}

/*
** Draw column floor and ceiling (split into two short helpers)
*/
static void	draw_column_ceiling(t_img *img, t_col_geom *g, int ceiling)
{
	int				y;
	unsigned int	*pixel;
	unsigned char	*addr;

	y = 0;
	while (y < g->drawStart)
	{
		addr = (unsigned char *)img->data;
		addr += y * img->size_line;
		addr += g->x * img->bytes_per_pixel;
		pixel = (unsigned int *)addr;
		*pixel = (unsigned int)ceiling;
		y++;
	}
}

static void	draw_column_floor(t_img *img, t_col_geom *g, int floor)
{
	int				y;
	unsigned int	*pixel;
	unsigned char	*addr;

	y = g->drawEnd + 1;
	while (y < HEIGHT)
	{
		addr = (unsigned char *)img->data;
		addr += y * img->size_line;
		addr += g->x * img->bytes_per_pixel;
		pixel = (unsigned int *)addr;
		*pixel = (unsigned int)floor;
		y++;
	}
}

static void	draw_column_floor_ceiling(t_img *img, t_col_geom *g,
	t_render_colors *cols)
{
	draw_column_ceiling(img, g, cols->ceiling);
	draw_column_floor(img, g, cols->floor);
}

/*
** Draw a single textured pixel for a column (short)
*/
static void	draw_textured_pixel(t_img *img, t_col_geom *g,
	int y, int color)
{
	unsigned int	*pixel;
	unsigned char	*addr;

	if (g->side == 1)
		color = (color >> 1) & 0x7F7F7F;
	addr = (unsigned char *)img->data;
	addr += y * img->size_line;
	addr += g->x * img->bytes_per_pixel;
	pixel = (unsigned int *)addr;
	*pixel = (unsigned int)color;
}

/*
** Draw textured column using helper per-pixel to keep function short.
*/
static void	draw_column_textured(t_cub3d *cub, t_img *img, t_col_geom *g,
	int *tex_buf)
{
	int				y;
	double			step;
	double			texPos;
	int				texY;
	int				color;

	if (g->lineHeight <= 0 || tex_buf == NULL)
		return ;
	step = 1.0 * cub->tex_height / g->lineHeight;
	texPos = (g->drawStart - HEIGHT / 2 + g->lineHeight / 2) * step;
	y = g->drawStart;
	while (y <= g->drawEnd)
	{
		texY = (int)texPos & (cub->tex_height - 1);
		texPos += step;
		color = tex_buf[cub->tex_width * texY + g->texX];
		draw_textured_pixel(img, g, y, color);
		y++;
	}
}

/*
** DDA helpers split to keep perform_dda <= 25 lines and no ternaries.
*/
static void	init_delta_dist(t_dda_in *in, double *deltaDistX,
	double *deltaDistY)
{
	if (in->rayDirX == 0)
		*deltaDistX = 1e30;
	else
		*deltaDistX = fabs(1.0 / in->rayDirX);
	if (in->rayDirY == 0)
		*deltaDistY = 1e30;
	else
		*deltaDistY = fabs(1.0 / in->rayDirY);
}

static void	init_dda_steps(t_cub3d *cub, t_dda_in *in,
	t_dda_out *out, double *sideDistX, double *sideDistY)
{
	double	deltaDistX;
	double	deltaDistY;

	init_delta_dist(in, &deltaDistX, &deltaDistY);
	if (in->rayDirX < 0)
	{
		out->stepX = -1;
		*sideDistX = (cub->pos_x - in->mapX) * deltaDistX;
	}
	else
	{
		out->stepX = 1;
		*sideDistX = (in->mapX + 1.0 - cub->pos_x) * deltaDistX;
	}
	if (in->rayDirY < 0)
	{
		out->stepY = -1;
		*sideDistY = (cub->pos_y - in->mapY) * deltaDistY;
	}
	else
	{
		out->stepY = 1;
		*sideDistY = (in->mapY + 1.0 - cub->pos_y) * deltaDistY;
	}
}

/* perform one step of the DDA loop (keeps loop body short) */
static void	dda_step_update(t_dda_in *in, t_dda_out *out,
	double *sideDistX, double *sideDistY)
{
	double	invRx;
	double	invRy;

	if (*sideDistX < *sideDistY)
	{
		if (in->rayDirX == 0)
			invRx = 1e30;
		else
			invRx = 1.0 / in->rayDirX;
		*sideDistX += fabs(invRx);
		in->mapX += out->stepX;
		out->side = 0;
	}
	else
	{
		if (in->rayDirY == 0)
			invRy = 1e30;
		else
			invRy = 1.0 / in->rayDirY;
		*sideDistY += fabs(invRy);
		in->mapY += out->stepY;
		out->side = 1;
	}
}

/* compute perpWallDist without ternaries (short helper) */
static void	compute_perp_wall_dist(t_dda_in *in, t_dda_out *out,
	double sideDistX, double sideDistY)
{
	double	inv;

	if (out->side == 0)
	{
		if (in->rayDirX == 0)
			inv = 1e30;
		else
			inv = 1.0 / in->rayDirX;
		out->perpWallDist = sideDistX - fabs(inv);
	}
	else
	{
		if (in->rayDirY == 0)
			inv = 1e30;
		else
			inv = 1.0 / in->rayDirY;
		out->perpWallDist = sideDistY - fabs(inv);
	}
}

/*
** perform_dda: <= 25 lines (header .. closing brace) and <= 5 locals
*/
static int	perform_dda(t_cub3d *cub, t_dda_in *in, t_dda_out *out)
{
	double	sideDistX;
	double	sideDistY;

	init_dda_steps(cub, in, out, &sideDistX, &sideDistY);
	while (1)
	{
		dda_step_update(in, out, &sideDistX, &sideDistY);
		if (in->mapY < 0 || in->mapY >= cub->map_height
			|| in->mapX < 0
			|| in->mapX >= (int)strlen(cub->map[in->mapY]))
			return (0);
		if (cub->map[in->mapY][in->mapX] == '1'
			|| (cub->is_bonus && cub->map[in->mapY][in->mapX] == 'D'))
		{
			compute_perp_wall_dist(in, out, sideDistX, sideDistY);
			return (1);
		}
	}
}

/*
** compute column parameters using grouped input
*/
static void	compute_column_params(t_cub3d *cub, t_col_calc_in *in,
	t_col_geom *g)
{
	g->lineHeight = (int)(HEIGHT / in->perpWallDist);
	g->drawStart = -g->lineHeight / 2 + HEIGHT / 2;
	if (g->drawStart < 0)
		g->drawStart = 0;
	g->drawEnd = g->lineHeight / 2 + HEIGHT / 2;
	if (g->drawEnd >= HEIGHT)
		g->drawEnd = HEIGHT - 1;
	if (in->side == 0)
		g->wallX = cub->pos_y + in->perpWallDist * in->rayDirY;
	else
		g->wallX = cub->pos_x + in->perpWallDist * in->rayDirX;
	g->wallX -= floor(g->wallX);
	g->texX = (int)(g->wallX * (double)cub->tex_width);
	g->side = in->side;
}

/*
** adjust texX for texture flip depending on side and ray direction
** kept short and explicit
*/
static void	render_column_adjust_texx(t_col_geom *g, t_dda_in *dda_in,
	int tex_width)
{
	if (g->side == 0 && dda_in->rayDirX > 0)
		g->texX = tex_width - g->texX - 1;
	if (g->side == 1 && dda_in->rayDirY < 0)
		g->texX = tex_width - g->texX - 1;
}

/*
** Render a single column with grouped parameters.
** Now uses a single local context variable to comply with variable limits.
*/
static void	render_column(t_cub3d *cub, t_img *img, int x,
	t_render_colors *cols)
{
	t_render_ctx	ctx;

	ctx.dda_in.mapX = (int)cub->pos_x;
	ctx.dda_in.mapY = (int)cub->pos_y;
	ctx.dda_in.rayDirX = cub->dir_x + cub->plane_x
		* (2 * x / (double)WIDTH - 1);
	ctx.dda_in.rayDirY = cub->dir_y + cub->plane_y
		* (2 * x / (double)WIDTH - 1);
	if (!perform_dda(cub, &ctx.dda_in, &ctx.dda_out))
		return ;
	ctx.calc_in.perpWallDist = ctx.dda_out.perpWallDist;
	ctx.calc_in.side = ctx.dda_out.side;
	ctx.calc_in.rayDirX = ctx.dda_in.rayDirX;
	ctx.calc_in.rayDirY = ctx.dda_in.rayDirY;
	ctx.g.x = x;
	compute_column_params(cub, &ctx.calc_in, &ctx.g);
	render_column_adjust_texx(&ctx.g, &ctx.dda_in, cub->tex_width);
	ctx.q.side = ctx.g.side;
	ctx.q.mapX = ctx.dda_in.mapX;
	ctx.q.mapY = ctx.dda_in.mapY;
	ctx.q.stepX = ctx.dda_out.stepX;
	ctx.q.stepY = ctx.dda_out.stepY;
	ctx.tex_buf = select_texture_buffer(cub, &ctx.q);
	draw_column_floor_ceiling(img, &ctx.g, cols);
	draw_column_textured(cub, img, &ctx.g, ctx.tex_buf);
}

/*
** raycast_render: kept short
*/
void	raycast_render(t_cub3d *cub, t_img *img)
{
	int				x;
	t_render_colors	cols;

	img->bytes_per_pixel = img->bpp / 8;
	/* ceiling */
	if (cub->textures.ceiling.r != -1)
		cols.ceiling = rgb_to_int(cub->textures.ceiling.r,
				cub->textures.ceiling.g,
				cub->textures.ceiling.b);
	else
		cols.ceiling = 0x202020;
	/* floor */
	if (cub->textures.floor.r != -1)
		cols.floor = rgb_to_int(cub->textures.floor.r,
				cub->textures.floor.g,
				cub->textures.floor.b);
	else
		cols.floor = 0x707070;
	x = 0;
	while (x < WIDTH)
	{
		render_column(cub, img, x, &cols);
		x++;
	}
}

/*
** Door helpers
*/
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

/* toggle door state at coordinates (short) */
static void	toggle_door_at(t_cub3d *cub, int mx, int my)
{
	char	*cell;

	cell = &cub->map[my][mx];
	if (*cell == 'D')
		*cell = 'O';
	else if (*cell == 'O')
		*cell = 'D';
}

/* check if target is same as player (short) */
static int	is_same_as_player(t_cub3d *cub, int mx, int my)
{
	return ((int)cub->pos_x == mx && (int)cub->pos_y == my);
}

void	handle_door_action(t_cub3d *cub)
{
	int	target_mapX;
	int	target_mapY;

	if (!cub->is_bonus)
		return ;
	if (!get_target_cell_coords(cub, 1.0, &target_mapX, &target_mapY))
		return ;
	if (cub->map[target_mapY][target_mapX] == 'D')
	{
		toggle_door_at(cub, target_mapX, target_mapY);
		return ;
	}
	if (cub->map[target_mapY][target_mapX] == 'O')
	{
		if (is_same_as_player(cub, target_mapX, target_mapY))
			return ;
		toggle_door_at(cub, target_mapX, target_mapY);
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

/*
** Movement helpers
*/
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
	cub->dir_x = cub->dir_x * cos(rot_speed_dt)
		- cub->dir_y * sin(rot_speed_dt);
	cub->dir_y = oldDirX * sin(rot_speed_dt)
		+ cub->dir_y * cos(rot_speed_dt);
	oldPlaneX = cub->plane_x;
	cub->plane_x = cub->plane_x * cos(rot_speed_dt)
		- cub->plane_y * sin(rot_speed_dt);
	cub->plane_y = oldPlaneX * sin(rot_speed_dt)
		+ cub->plane_y * cos(rot_speed_dt);
}

static void	rotate_left(t_cub3d *cub, double rot_speed_dt)
{
	double	oldDirX;
	double	oldPlaneX;

	oldDirX = cub->dir_x;
	cub->dir_x = cub->dir_x * cos(-rot_speed_dt)
		- cub->dir_y * sin(-rot_speed_dt);
	cub->dir_y = oldDirX * sin(-rot_speed_dt)
		+ cub->dir_y * cos(-rot_speed_dt);
	oldPlaneX = cub->plane_x;
	cub->plane_x = cub->plane_x * cos(-rot_speed_dt)
		- cub->plane_y * sin(-rot_speed_dt);
	cub->plane_y = oldPlaneX * sin(-rot_speed_dt)
		+ cub->plane_y * cos(-rot_speed_dt);
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

static void	update_door_animation(t_cub3d *cub)
{
	if (!cub->is_bonus || !cub->door_textures)
		return ;
	cub->door_anim_timer += cub->time_frame;
	if (cub->door_anim_timer >= cub->door_anim_speed)
	{
		cub->door_anim_frame = (cub->door_anim_frame + 1) % 3;
		cub->door_anim_timer = 0.0;
	}
}

int	render_loop(t_cub3d *cub)
{
	long	current_time;
	t_img	img;

	current_time = fn_get_time_in_ms();
	cub->time_frame = (current_time - cub->time_prev) / 1000.0;
	cub->time_prev = current_time;
	perform_movements(cub);
	update_door_animation(cub);
	img.data = cub->img_data;
	img.size_line = cub->size_line;
	img.bpp = cub->bpp;
	img.bytes_per_pixel = img.bpp / 8;
	raycast_render(cub, &img);
	if (cub->is_bonus)
		draw_minimap(cub, &img);
	mlx_put_image_to_window(cub->mlx, cub->win, cub->img, 0, 0);
	draw_door_prompt(cub);
	return (0);
}

/*
** Door prompt helpers
*/
static int	check_near_far_target(t_cub3d *cub, double near_d,
	double far_d, int *out_x, int *out_y)
{
	if (!get_target_cell_coords(cub, near_d, out_x, out_y))
		return (0);
	if ((int)cub->pos_x == *out_x && (int)cub->pos_y == *out_y)
	{
		if (!get_target_cell_coords(cub, far_d, out_x, out_y))
			return (0);
	}
	return (1);
}

static const char	*select_door_message(char cell)
{
	if (cell == 'D')
		return ("Press SPACE to open");
	return ("Press SPACE to close");
}

void	draw_door_prompt(t_cub3d *cub)
{
	int			target_mapX;
	int			target_mapY;
	const char	*message;
	char		target_cell;

	if (!check_near_far_target(cub, 0.75, 1.5, &target_mapX, &target_mapY))
		return ;
	target_cell = cub->map[target_mapY][target_mapX];
	if (cub->is_bonus && (target_cell == 'D' || target_cell == 'O'))
	{
		message = select_door_message(target_cell);
		mlx_string_put(cub->mlx, cub->win,
			(WIDTH / 2) - 100, HEIGHT - 50,
			0xFFFFFF, (char *)message);
	}
}

/*
** Player init helpers (split small setters)
*/
static void	set_dir_n(t_cub3d *cub)
{
	cub->dir_x = 0;
	cub->dir_y = -1;
	cub->plane_x = 0.66;
	cub->plane_y = 0;
}

static void	set_dir_s(t_cub3d *cub)
{
	cub->dir_x = 0;
	cub->dir_y = 1;
	cub->plane_x = -0.66;
	cub->plane_y = 0;
}

static void	set_dir_e(t_cub3d *cub)
{
	cub->dir_x = 1;
	cub->dir_y = 0;
	cub->plane_x = 0;
	cub->plane_y = 0.66;
}

static void	set_dir_w(t_cub3d *cub)
{
	cub->dir_x = -1;
	cub->dir_y = 0;
	cub->plane_x = 0;
	cub->plane_y = -0.66;
}

static void	set_player_dir(t_cub3d *cub, char dir)
{
	if (dir == 'N')
		set_dir_n(cub);
	else if (dir == 'S')
		set_dir_s(cub);
	else if (dir == 'E')
		set_dir_e(cub);
	else if (dir == 'W')
		set_dir_w(cub);
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
	find_player_position(cub);
}

void	rotate_player(t_cub3d *cub, double angle)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = cub->dir_x;
	cub->dir_x = cub->dir_x * cos(angle)
		- cub->dir_y * sin(angle);
	cub->dir_y = old_dir_x * sin(angle)
		+ cub->dir_y * cos(angle);
	old_plane_x = cub->plane_x;
	cub->plane_x = cub->plane_x * cos(angle)
		- cub->plane_y * sin(angle);
	cub->plane_y = old_plane_x * sin(angle)
		+ cub->plane_y * cos(angle);
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
