/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d_render.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 11:55:47 by skyce11           #+#    #+#             */
/*   Updated: 2025/10/22 12:08:50 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_RENDER_H
# define CUB3D_RENDER_H

/*
** Core project header: must define t_cub3d, t_img and other project types.
*/
# include "cub3d.h"

# ifndef WIDTH
#  define WIDTH 1248
# endif

# ifndef HEIGHT
#  define HEIGHT 960
# endif

# define MOVE_SPEED 0.09
# define ROT_SPEED 0.055
# define MOUSE_SENSITIVITY 0.003

# define MINIMAP_SCALE 12
# define MINIMAP_MARGIN 10
# define MINIMAP_VIEW_RADIUS 5

# define COLOR_WALL 0x404040
# define COLOR_FLOOR 0x808080
# define COLOR_PLAYER 0xFF0000
# define COLOR_EMPTY 0x000000

/*
** Image info returned by mlx_get_data_addr.
** Moved here so loader helpers can use it without declaring their own types.
*/
typedef struct s_img_info
{
	char	*data;
	int		bpp;
	int		size_line;
	int		endian;
}				t_img_info;

typedef struct s_col_geom
{
	int		x;
	int		line_height;
	int		draw_start;
	int		draw_end;
	int		tex_x;
	int		side;
	double	wall_x;
}				t_col_geom;

typedef struct s_dda_in
{
	int		map_x;
	int		map_y;
	double	ray_dir_x;
	double	ray_dir_y;
}				t_dda_in;

typedef struct s_dda_out
{
	int		side;
	double	perp_wall_dist;
	int		step_x;
	int		step_y;
}				t_dda_out;

typedef struct s_col_calc_in
{
	double	perp_wall_dist;
	int		side;
	double	ray_dir_x;
	double	ray_dir_y;
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
	int	map_x;
	int	map_y;
	int	step_x;
	int	step_y;
}				t_tex_query;

typedef struct s_minimap_row_ctx
{
	int	j;
	int	map_cell_x;
	int	map_cell_y;
	int	cell_draw_x;
	int	cell_draw_y;
	int	color;
}				t_minimap_row_ctx;

typedef struct s_point
{
	int	x;
	int	y;
}				t_point;

typedef struct s_dda_side
{
	double	sx;
	double	sy;
}				t_dda_side;

typedef struct s_target
{
	int	x;
	int	y;
	int	ok;
}				t_target;

typedef struct s_minimap_ctx
{
	t_cub3d	*cub;
	t_img	*img;
	int		start_x;
	int		start_y;
}				t_minimap_ctx;

typedef struct s_render_ctx
{
	t_dda_in		dda_in;
	t_dda_out		dda_out;
	t_col_calc_in	calc_in;
	t_col_geom		g;
	t_tex_query		q;
	int				*tex_buf;
}				t_render_ctx;

int			is_floor(char c);

/* loader (textures) */

void		copy_image_to_buffer(char *data, int bpp,
				int *buffer, long count);
int			*load_xpm_err(void *mlx, void *img,
				const char *fmt, const char *path);

void		init_door_textures_array(t_cub3d *cub);
void		load_door_frames(t_cub3d *cub, void *mlx, int *w, int *h);
void		set_door_open_texture(t_cub3d *cub, void *mlx,
				int *tmp_w, int *tmp_h);
void		load_doors(t_cub3d *cub, void *mlx, int *w, int *h);

void		load_walls(t_cub3d *cub, void *mlx, int *w, int *h);
void		verify_wall_textures(t_cub3d *cub);

/* Public loader API */
void		load_wall_textures(t_cub3d *cub, void *mlx);

/* utils */
int			get_wall_texture(int side, double ray_dir_x, double ray_dir_y);
int			rgb_to_int(int r, int g, int b);
void		draw_pixel(t_img *img, int x, int y, int color);

/* minimap */
int			minimap_cell_color(t_cub3d *cub, int map_y, int map_x);
void		minimap_fill_cell(t_img *img, int base_x, int base_y, int color);
void		minimap_draw_border(t_img *img, int draw_x, int draw_y);
void		minimap_draw_cell(t_img *img, int draw_x, int draw_y, int color);
void		minimap_draw_row(t_minimap_ctx *ctx, int i);
void		minimap_draw_cells(t_minimap_ctx *ctx);
void		minimap_draw_player_square(t_img *img,
				int player_pixel_x,
				int player_pixel_y);
void		minimap_draw_player_dir(t_img *img,
				int player_pixel_x,
				int player_pixel_y,
				t_vec2 *dir);
t_point		minimap_compute_player_coords(t_minimap_ctx *ctx);
void		draw_minimap(t_cub3d *cub, t_img *img);

/* textures & columns */
int			*select_texture_buffer(t_cub3d *cub, t_tex_query *q);
void		draw_column_ceiling(t_img *img, t_col_geom *g, int ceiling);
void		draw_column_floor(t_img *img, t_col_geom *g, int floor);
void		draw_column_floor_ceiling(t_img *img,
				t_col_geom *g,
				t_render_colors *cols);
void		draw_textured_pixel(t_img *img,
				t_col_geom *g,
				int y,
				int color);

/* column render */
void		draw_column_textured(t_cub3d *cub,
				t_img *img,
				t_col_geom *g,
				int *tex_buf);
void		compute_column_params(t_cub3d *cub,
				t_col_calc_in *in, t_col_geom *g);
void		render_column_adjust_texx(t_col_geom *g,
				t_dda_in *dda_in, int tex_width);
void		render_column(t_cub3d *cub,
				t_img *img,
				int x,
				t_render_colors *cols);
void		raycast_render(t_cub3d *cub, t_img *img);

/* DDA */
void		init_delta_dist(t_dda_in *in,
				double *delta_dist_x, double *delta_dist_y);
t_dda_side	init_dda_steps(t_cub3d *cub, t_dda_in *in, t_dda_out *out);
void		dda_step_update(t_dda_in *in,
				t_dda_out *out,
				double *side_dist_x,
				double *side_dist_y);
void		compute_perp_wall_dist(t_dda_in *in,
				t_dda_out *out,
				double side_dist_x,
				double side_dist_y);
int			perform_dda(t_cub3d *cub, t_dda_in *in, t_dda_out *out);

/* doors */
int			get_target_cell_coords(t_cub3d *cub, double dist, int *tx, int *ty);
void		toggle_door_at(t_cub3d *cub, int mx, int my);
int			is_same_as_player(t_cub3d *cub, int mx, int my);
void		handle_door_action(t_cub3d *cub);
t_target	check_near_far_target(t_cub3d *cub,
				double near_d,
				double far_d);
const char	*select_door_message(char cell);
void		draw_door_prompt(t_cub3d *cub);

/* input */
int			handle_key_press(int key, t_cub3d *cub);
int			handle_key_release(int key, t_cub3d *cub);
int			handle_mouse_move(int x, int y, t_cub3d *cub);

/* movement */
void		move_forward(t_cub3d *cub, double move_speed_dt);
void		move_backward(t_cub3d *cub, double move_speed_dt);
void		rotate_right(t_cub3d *cub, double rot_speed_dt);
void		rotate_left(t_cub3d *cub, double rot_speed_dt);
void		perform_movements(t_cub3d *cub);

/* timing / loop */
long		fn_get_time_in_ms(void);
void		update_door_animation(t_cub3d *cub);
int			render_loop(t_cub3d *cub);

/* player */
void		set_dir_n(t_cub3d *cub);
void		set_dir_s(t_cub3d *cub);
void		set_dir_e(t_cub3d *cub);
void		set_dir_w(t_cub3d *cub);
void		set_player_dir(t_cub3d *cub, char dir);
int			find_player_position(t_cub3d *cub);
void		init_player(t_cub3d *cub);
void		rotate_player(t_cub3d *cub, double angle);

#endif /* CUB3D_RENDER_H */